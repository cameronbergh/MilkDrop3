// audiobuf.cpp

#include "audiobuf.h"
#include <string.h> // For memset
#include <stdint.h> // For int8_t, uint32_t (UINT32 might be defined in audiobuf.h for Linux)

#define SAMPLE_SIZE_LPB 576

std::mutex pcmLpbMutex;
unsigned char pcmLeftLpb[SAMPLE_SIZE_LPB];
unsigned char pcmRightLpb[SAMPLE_SIZE_LPB];
bool pcmBufDrained = false;
signed int pcmLen = 0;
signed int pcmPos = 0;

void ResetAudioBuf() {
    std::unique_lock<std::mutex> lock(pcmLpbMutex);
    memset(pcmLeftLpb, 0, SAMPLE_SIZE_LPB);
    memset(pcmRightLpb, 0, SAMPLE_SIZE_LPB);
    pcmBufDrained = false;
    pcmLen = 0;
    pcmPos = 0;
}

void GetAudioBuf(unsigned char *pWaveL, unsigned char *pWaveR, int SamplesCount) {
    std::unique_lock<std::mutex> lock(pcmLpbMutex);
    if ((pcmLen < SamplesCount) || (pcmBufDrained)) {
        memset(pWaveL, 0, SamplesCount);
        memset(pWaveR, 0, SamplesCount);
    }
    else {
        int read_pos = (pcmPos - pcmLen + SAMPLE_SIZE_LPB) % SAMPLE_SIZE_LPB;
        for (int i = 0; i < SamplesCount; i++) {
            if (i < pcmLen) { // Ensure we don't read beyond what's available
                 pWaveL[i] = pcmLeftLpb[(read_pos + i) % SAMPLE_SIZE_LPB];
                 pWaveR[i] = pcmRightLpb[(read_pos + i) % SAMPLE_SIZE_LPB];
            } else { // Should not happen if pcmLen >= SamplesCount check is correct
                 pWaveL[i] = 0;
                 pWaveR[i] = 0;
            }
        }
        pcmBufDrained = true;
    }
}

int8_t FltToInt(float flt) {
    if (flt >= 1.0f) return +127;
    if (flt < -1.0f) return -128;
    return (int8_t)(flt * 127.0f);
};

union u_type {
    int32_t IntVar;
    int16_t ShortVar;
    float FltVar;
    unsigned char Bytes[4];
};

int8_t GetChannelSampleProcessed(const unsigned char *frame_data_start, int channel_index, int num_channels, int bits_per_sample, bool is_float) {
    u_type sample_union;
    sample_union.IntVar = 0;

    int bytes_per_sample_per_channel = bits_per_sample / 8;
    const unsigned char *sample_ptr = frame_data_start + (channel_index * bytes_per_sample_per_channel);

    if (is_float) {
        if (bits_per_sample == 32) {
            memcpy(sample_union.Bytes, sample_ptr, 4); // Assuming little-endian host for direct copy
            return FltToInt(sample_union.FltVar);
        }
    } else {
        if (bits_per_sample == 16) {
            memcpy(sample_union.Bytes, sample_ptr, 2); // Assuming little-endian host
            return (int8_t)(sample_union.ShortVar / 256);
        }
    }
    return 0;
}

// SetAudioBuf implementation using direct parameters for format info
void SetAudioBuf(const BYTE *pData, const UINT32 nNumFramesToRead,
                 int num_channels, int bits_per_sample, bool is_float,
                 const WAVEFORMATEX *pwfx_win /* = nullptr */) {

    if (!pData || nNumFramesToRead == 0 || num_channels == 0 || bits_per_sample == 0) {
        return;
    }

    std::unique_lock<std::mutex> lock(pcmLpbMutex);

    int bytes_per_sample_per_channel = bits_per_sample / 8;
    int frame_size_in_bytes = num_channels * bytes_per_sample_per_channel;
    if (frame_size_in_bytes == 0) return;

    uint32_t frames_to_process = nNumFramesToRead;
    const unsigned char* data_source_ptr = pData;

    // If new data is larger than our buffer, only process the latest SAMPLE_SIZE_LPB frames
    if (nNumFramesToRead > SAMPLE_SIZE_LPB) {
        data_source_ptr += (nNumFramesToRead - SAMPLE_SIZE_LPB) * frame_size_in_bytes;
        frames_to_process = SAMPLE_SIZE_LPB;
    }

    // Manage circular buffer: advance pcmPos by the number of frames we are about to overwrite
    // if the new data (frames_to_process) would overflow current pcmLen.
    // Or, if new data is larger than buffer, effectively reset.
    if (frames_to_process == SAMPLE_SIZE_LPB) { // Overwriting entire buffer
        pcmPos = 0;
        pcmLen = 0;
    } else if (pcmLen + frames_to_process > SAMPLE_SIZE_LPB) {
        int overflow = (pcmLen + frames_to_process) - SAMPLE_SIZE_LPB;
        pcmPos = (pcmPos + overflow) % SAMPLE_SIZE_LPB;
        pcmLen -= overflow;
    }


    for (uint32_t i = 0; i < frames_to_process; ++i) {
        const unsigned char *current_frame_ptr = data_source_ptr + (i * frame_size_in_bytes);

        int8_t left_sample8 = 0;
        if (num_channels >= 1) {
            left_sample8 = GetChannelSampleProcessed(current_frame_ptr, 0, num_channels, bits_per_sample, is_float);
        }

        int8_t right_sample8 = left_sample8;
        if (num_channels >= 2) {
            right_sample8 = GetChannelSampleProcessed(current_frame_ptr, 1, num_channels, bits_per_sample, is_float);
        }

        int buffer_write_idx = (pcmPos + pcmLen) % SAMPLE_SIZE_LPB;
        pcmLeftLpb[buffer_write_idx] = left_sample8;
        pcmRightLpb[buffer_write_idx] = right_sample8;

        if (pcmLen < SAMPLE_SIZE_LPB) {
            pcmLen++;
        } else { // Buffer is full, advance pcmPos (oldest data is overwritten)
            pcmPos = (pcmPos + 1) % SAMPLE_SIZE_LPB;
        }
    }
    pcmBufDrained = false;
}
