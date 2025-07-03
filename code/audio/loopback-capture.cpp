// loopback-capture.cpp

#ifdef _WIN32
#include "common.h" // Includes windows.h, audioclient.h, etc. for Windows
// common.h also includes audiobuf.h and log.h

HRESULT LoopbackCapture(
    IMMDevice *pMMDevice,
    HMMIO hFile,
    bool bInt16,
    HANDLE hStartedEvent,
    HANDLE hStopEvent,
    PUINT32 pnFrames
);

HRESULT WriteWaveHeader(HMMIO hFile, LPCWAVEFORMATEX pwfx, MMCKINFO *pckRIFF, MMCKINFO *pckData);
HRESULT FinishWaveFile(HMMIO hFile, MMCKINFO *pckRIFF, MMCKINFO *pckData);

DWORD WINAPI LoopbackCaptureThreadFunction(LPVOID pContext) {
    LoopbackCaptureThreadFunctionArguments *pArgs =
        (LoopbackCaptureThreadFunctionArguments*)pContext;

    pArgs->hr = CoInitialize(NULL);
    if (FAILED(pArgs->hr)) {
        ERR(L"CoInitialize failed: hr = 0x%08x", pArgs->hr);
        return 0;
    }
    CoUninitializeOnExit cuoe;

    pArgs->hr = LoopbackCapture(
        pArgs->pMMDevice,
        pArgs->hFile,
        pArgs->bInt16,
        pArgs->hStartedEvent,
        pArgs->hStopEvent,
        &pArgs->nFrames
    );

    return 0;
}

HRESULT LoopbackCapture(
    IMMDevice *pMMDevice,
    HMMIO hFile,
    bool bInt16,
    HANDLE hStartedEvent,
    HANDLE hStopEvent,
    PUINT32 pnFrames
) {
    HRESULT hr;

    IAudioClient *pAudioClient = NULL;
    hr = pMMDevice->Activate(
        __uuidof(IAudioClient),
        CLSCTX_ALL, NULL,
        (void**)&pAudioClient
    );
    if (FAILED(hr)) {
        ERR(L"IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x", hr);
        return hr;
    }
    ReleaseOnExit releaseAudioClient(pAudioClient);

    REFERENCE_TIME hnsDefaultDevicePeriod;
    hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetDevicePeriod failed: hr = 0x%08x", hr);
        return hr;
    }

    WAVEFORMATEX *pwfx = NULL;
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetMixFormat failed: hr = 0x%08x", hr);
        return hr;
    }
    CoTaskMemFreeOnExit freeMixFormat(pwfx);

    WAVEFORMATEX tempWfx = *pwfx; // Store original format for determining float/int
                                 // As pwfx might be coerced to 16-bit PCM

    if (bInt16) {
        switch (pwfx->wFormatTag) {
            case WAVE_FORMAT_IEEE_FLOAT:
                tempWfx = *pwfx; // original is float
                pwfx->wFormatTag = WAVE_FORMAT_PCM;
                pwfx->wBitsPerSample = 16;
                pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                break;
            case WAVE_FORMAT_EXTENSIBLE: {
                PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
                tempWfx = *pwfx; // original might be float (check SubFormat)
                if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
                    pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    pEx->Samples.wValidBitsPerSample = 16;
                    pwfx->wBitsPerSample = 16;
                    pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                } else if (!IsEqualGUID(KSDATAFORMAT_SUBTYPE_PCM, pEx->SubFormat)) {
                     ERR(L"%s", L"Don't know how to coerce mix format to int-16 from WAVE_FORMAT_EXTENSIBLE non-float");
                     return E_UNEXPECTED;
                } // If already PCM, bInt16 assumes it's the correct PCM format (e.g. 16-bit)
                break;
            }
            default: // If already PCM and bInt16 is true, assume it's already 16-bit.
                     // Or if it's another format, error out.
                if (pwfx->wFormatTag != WAVE_FORMAT_PCM) {
                    ERR(L"Don't know how to coerce WAVEFORMATEX with wFormatTag = 0x%04x to int-16", pwfx->wFormatTag);
                    return E_UNEXPECTED;
                }
                break;
        }
    }

    MMCKINFO ckRIFF = { 0 };
    MMCKINFO ckData = { 0 };
    if (NULL != hFile) {
        hr = WriteWaveHeader(hFile, pwfx, &ckRIFF, &ckData);
        if (FAILED(hr)) return hr;
    }

    HANDLE hWakeUp = CreateWaitableTimer(NULL, FALSE, NULL);
    if (NULL == hWakeUp) {
        DWORD dwErr = GetLastError();
        ERR(L"CreateWaitableTimer failed: last error = %u", dwErr);
        return HRESULT_FROM_WIN32(dwErr);
    }
    CloseHandleOnExit closeWakeUp(hWakeUp);

    UINT32 nBlockAlign = pwfx->nBlockAlign; // Use potentially coerced nBlockAlign
    *pnFrames = 0;

    hr = pAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, 0 );
    if (FAILED(hr)) {
        ERR(L"IAudioClient::Initialize failed: hr = 0x%08x", hr);
        return hr;
    }

    IAudioCaptureClient *pAudioCaptureClient = NULL;
    hr = pAudioClient->GetService( __uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient );
    if (FAILED(hr)) {
        ERR(L"IAudioClient::GetService(IAudioCaptureClient) failed: hr = 0x%08x", hr);
        return hr;
    }
    ReleaseOnExit releaseAudioCaptureClient(pAudioCaptureClient);

    LARGE_INTEGER liFirstFire;
    liFirstFire.QuadPart = -hnsDefaultDevicePeriod / 2;
    LONG lTimeBetweenFires = (LONG)hnsDefaultDevicePeriod / 2 / (10 * 1000);
    BOOL bOK = SetWaitableTimer( hWakeUp, &liFirstFire, lTimeBetweenFires, NULL, NULL, FALSE );
    if (!bOK) {
        DWORD dwErr = GetLastError();
        ERR(L"SetWaitableTimer failed: last error = %u", dwErr);
        return HRESULT_FROM_WIN32(dwErr);
    }
    CancelWaitableTimerOnExit cancelWakeUp(hWakeUp);

    hr = pAudioClient->Start();
    if (FAILED(hr)) {
        ERR(L"IAudioClient::Start failed: hr = 0x%08x", hr);
        return hr;
    }
    AudioClientStopOnExit stopAudioClient(pAudioClient);
    SetEvent(hStartedEvent);

    HANDLE waitArray[2] = { hStopEvent, hWakeUp };
    DWORD dwWaitResult;
    bool bDone = false;
    bool bFirstPacket = true;

    for (UINT32 nPasses = 0; !bDone; nPasses++) {
        UINT32 nNextPacketSize;
        for ( hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize); SUCCEEDED(hr) && nNextPacketSize > 0; hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize) ) {
            BYTE *pData;
            UINT32 nNumFramesToRead;
            DWORD dwFlags;

            hr = pAudioCaptureClient->GetBuffer( &pData, &nNumFramesToRead, &dwFlags, NULL, NULL );
            if (FAILED(hr)) {
                ERR(L"IAudioCaptureClient::GetBuffer failed: hr = 0x%08x", hr);
                return hr;
            }

            if (bFirstPacket && AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY == dwFlags) { LOG(L"%s", L"Probably spurious glitch reported on first packet"); }
            else if (0 != dwFlags) { LOG(L"IAudioCaptureClient::GetBuffer set flags to 0x%08x", dwFlags); }
            if (0 == nNumFramesToRead) { ERR(L"IAudioCaptureClient::GetBuffer said to read 0 frames"); return E_UNEXPECTED; }

            // Determine if original format was float
            bool is_original_float = (tempWfx.wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
                                     (tempWfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
                                      IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, reinterpret_cast<PWAVEFORMATEXTENSIBLE>(&tempWfx)->SubFormat)));

            // If bInt16 is true, data is already 16-bit int. Otherwise, it's in the original mix format (pwfx before coercion, stored in tempWfx).
            bool is_input_float = !bInt16 && is_original_float;
            int input_bits_per_sample = bInt16 ? 16 : tempWfx.wBitsPerSample;

            SetAudioBuf(pData, nNumFramesToRead, tempWfx.nChannels, input_bits_per_sample, is_input_float, pwfx);

            if (NULL != hFile) {
                LONG lBytesToWrite = nNumFramesToRead * nBlockAlign; // Use nBlockAlign of the *output* format (pwfx)
                LONG lBytesWritten = mmioWrite(hFile, reinterpret_cast<PCHAR>(pData), lBytesToWrite);
                if (lBytesToWrite != lBytesWritten) { ERR(L"mmioWrite error"); return E_UNEXPECTED; }
            }
            *pnFrames += nNumFramesToRead;

            hr = pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
            if (FAILED(hr)) { ERR(L"IAudioCaptureClient::ReleaseBuffer failed: hr = 0x%08x", hr); return hr; }
            bFirstPacket = false;
        }
        if (FAILED(hr)) { ERR(L"IAudioCaptureClient::GetNextPacketSize failed: hr = 0x%08x", hr); return hr; }

        dwWaitResult = WaitForMultipleObjects( ARRAYSIZE(waitArray), waitArray, FALSE, INFINITE );
        if (WAIT_OBJECT_0 == dwWaitResult) { LOG(L"Received stop event"); bDone = true; continue; }
        if (WAIT_OBJECT_0 + 1 != dwWaitResult) { ERR(L"Unexpected WaitForMultipleObjects return value %u", dwWaitResult); return E_UNEXPECTED; }
    }

    if (NULL != hFile) {
        hr = FinishWaveFile(hFile, &ckData, &ckRIFF);
        if (FAILED(hr)) return hr;
    }
    return hr;
}

HRESULT WriteWaveHeader(HMMIO hFile, LPCWAVEFORMATEX pwfx, MMCKINFO *pckRIFF, MMCKINFO *pckData) { /* ... unchanged ... */
    MMRESULT result;
    pckRIFF->ckid = MAKEFOURCC('R', 'I', 'F', 'F');
    pckRIFF->fccType = MAKEFOURCC('W', 'A', 'V', 'E');
    result = mmioCreateChunk(hFile, pckRIFF, MMIO_CREATERIFF);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioCreateChunk(\"RIFF/WAVE\") failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    MMCKINFO chunk;
    chunk.ckid = MAKEFOURCC('f', 'm', 't', ' ');
    result = mmioCreateChunk(hFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioCreateChunk(\"fmt \") failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    LONG lBytesInWfx = sizeof(WAVEFORMATEX) + pwfx->cbSize;
    LONG lBytesWritten = mmioWrite( hFile, reinterpret_cast<PCHAR>(const_cast<LPWAVEFORMATEX>(pwfx)), lBytesInWfx );
    if (lBytesWritten != lBytesInWfx) { ERR(L"mmioWrite(fmt data) wrote %u bytes; expected %u bytes", lBytesWritten, lBytesInWfx); return E_FAIL; }
    result = mmioAscend(hFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioAscend(\"fmt \" failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    chunk.ckid = MAKEFOURCC('f', 'a', 'c', 't');
    result = mmioCreateChunk(hFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioCreateChunk(\"fact\") failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    DWORD frames = 0;
    lBytesWritten = mmioWrite(hFile, reinterpret_cast<PCHAR>(&frames), sizeof(frames));
    if (lBytesWritten != sizeof(frames)) { ERR(L"mmioWrite(fact data) wrote %u bytes; expected %u bytes", lBytesWritten, (UINT32)sizeof(frames)); return E_FAIL; }
    result = mmioAscend(hFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioAscend(\"fact\" failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    pckData->ckid = MAKEFOURCC('d', 'a', 't', 'a');
    result = mmioCreateChunk(hFile, pckData, 0);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioCreateChunk(\"data\") failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    return S_OK;
}
HRESULT FinishWaveFile(HMMIO hFile, MMCKINFO *pckData, MMCKINFO *pckRIFF) { /* ... unchanged ... */
    MMRESULT result;
    result = mmioAscend(hFile, pckData, 0);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioAscend(\"data\" failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    result = mmioAscend(hFile, pckRIFF, 0);
    if (MMSYSERR_NOERROR != result) { ERR(L"mmioAscend(\"RIFF/WAVE\" failed: MMRESULT = 0x%08x", result); return E_FAIL; }
    return S_OK;
}

#else // Linux/PulseAudio implementation

#include <stdio.h>
#include <string.h> // For memset if not in stdio
#include <pthread.h>
#include <unistd.h> // for usleep

#include <pulse/simple.h>
#include <pulse/error.h>

#include "audiobuf.h"
#include "log.h"
#include "loopback-capture.h" // For declarations of StartAudioCapture/StopAudioCapture

static pa_simple *s_pa_simple = NULL;
static pthread_t capture_thread_id = 0;
static volatile int stop_capture_flag = 0;

void LoopbackCaptureLinux() {
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_FLOAT32LE;
    ss.rate = 44100;
    ss.channels = 2;
    int error;

    s_pa_simple = pa_simple_new( NULL, "MilkDrop3", PA_STREAM_RECORD, NULL, "audio capture", &ss, NULL, NULL, &error );
    if (!s_pa_simple) {
        ERR("pa_simple_new() failed: %s", pa_strerror(error));
        return;
    }
    LOG("PulseAudio connection established for recording.");

    while (!stop_capture_flag) {
        unsigned char read_buf[256 * 2 * sizeof(float)]; // Buffer for 256 stereo float samples
        int pa_error;
        if (pa_simple_read(s_pa_simple, read_buf, sizeof(read_buf), &pa_error) < 0) {
            ERR("pa_simple_read() failed: %s", pa_strerror(pa_error));
            if (s_pa_simple) { pa_simple_free(s_pa_simple); s_pa_simple = NULL; }
            return;
        }
        SetAudioBuf(read_buf, 256, ss.channels, 32, true, nullptr );
        usleep(10000);
    }

    if (s_pa_simple) {
        pa_simple_free(s_pa_simple);
        s_pa_simple = NULL;
        LOG("PulseAudio connection freed.");
    }
}

void* LoopbackCaptureThreadFunctionLinux(void* arg) {
    (void)arg;
    LOG("Starting Linux loopback capture thread.");
    LoopbackCaptureLinux();
    LOG("Exiting Linux loopback capture thread.");
    pthread_exit(NULL);
    return NULL;
}

void StartAudioCapture() {
#ifdef _WIN32
    // This function would need to be called from appropriate place in Windows code path
    // e.g. from StartThreads in Milkdrop2PcmVisualizer.cpp, replacing CreateThread there.
    // For now, Windows path uses its original LoopbackCaptureThreadFunction via CreateThread.
    // If abstracting, the Windows part of StartAudioCapture would create its thread.
    ERR(L"StartAudioCapture called on Windows - not implemented to replace CreateThread yet.");
#else
    LOG("Attempting to start Linux audio capture...");
    stop_capture_flag = 0;
    if (capture_thread_id != 0) {
        LOG("Linux capture thread already running or not properly cleaned up.");
    }
    if (pthread_create(&capture_thread_id, NULL, LoopbackCaptureThreadFunctionLinux, NULL) != 0) {
        ERR("Failed to create Linux capture thread.");
        capture_thread_id = 0;
    } else {
        LOG("Linux capture thread created successfully.");
    }
#endif
}

void StopAudioCapture() {
#ifdef _WIN32
    // See comment in StartAudioCapture
    ERR(L"StopAudioCapture called on Windows - not implemented to replace event signalling yet.");
#else
    LOG("Attempting to stop Linux audio capture...");
    if (capture_thread_id != 0) {
        stop_capture_flag = 1;
        LOG("Joining Linux capture thread...");
        pthread_join(capture_thread_id, NULL);
        capture_thread_id = 0;
        LOG("Linux capture thread stopped and joined.");
    } else {
        LOG("Linux capture thread not running or already stopped.");
    }
    stop_capture_flag = 0;
#endif
}

#endif // _WIN32
