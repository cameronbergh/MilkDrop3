// loopback-capture.h

#ifdef _WIN32
// These are Windows-specific and used by the Windows implementation in loopback-capture.cpp
#include <windows.h> // For HANDLE, DWORD, LPVOID etc.
#include <mmdeviceapi.h> // For IMMDevice
#include <mmsystem.h>    // For HMMIO

// UINT32 and HRESULT might come from windows.h or other SDK headers.
// If common.h or another header included before this one provides them,
// these explicit includes might not be strictly necessary here but don't hurt.

struct LoopbackCaptureThreadFunctionArguments {
    IMMDevice *pMMDevice;
    bool bInt16;
    HMMIO hFile;
    HANDLE hStartedEvent;
    HANDLE hStopEvent;
    UINT32 nFrames; // Assuming UINT32 is defined (e.g. via windows.h or a typedef)
    HRESULT hr;     // Assuming HRESULT is defined
};

DWORD WINAPI LoopbackCaptureThreadFunction(LPVOID pContext);

#endif // _WIN32

// Cross-platform API for starting/stopping audio capture.
// Implementations are in loopback-capture.cpp, conditionalized by platform.
#ifdef __cplusplus
extern "C" {
#endif

void StartAudioCapture();
void StopAudioCapture();

#ifdef __cplusplus
}
#endif
