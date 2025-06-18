// audiobuf.h

#ifndef AUDIOBUF_H_
#define AUDIOBUF_H_

#include <mutex>
#include <stdint.h> // For uint32_t

#ifdef _WIN32
    #include <windows.h> // For BYTE, UINT32, WAVEFORMATEX
    // No need to redefine BYTE or UINT32 if windows.h is included
#else
    // Define BYTE and UINT32 for non-Windows
    #ifndef BYTE
        #define BYTE unsigned char
    #endif
    #ifndef UINT32
        #define UINT32 uint32_t
    #endif
    // Forward declare WAVEFORMATEX for the function signature, as it's Windows-specific
    // and the Linux path will pass nullptr for it.
    // Use 'struct' to ensure it's treated as a type name for the pointer.
    // The actual definition is not needed for Linux if we only pass it as an opaque pointer.
    struct tWAVEFORMATEX; // Opaque forward declaration
    typedef struct tWAVEFORMATEX WAVEFORMATEX;
#endif


// Reset audio buffer discarding stored audio data
void ResetAudioBuf();

// Return previously saved audio data for visualizer
void GetAudioBuf(unsigned char *pWaveL, unsigned char *pWaveR, int SamplesCount);

// Save audio data for visualizer
// The WAVEFORMATEX* is only used by the Windows implementation in loopback-capture.cpp
// For Linux, pwfx_win will be nullptr, and the function should use
// num_channels, bits_per_sample, and is_float.
void SetAudioBuf(const BYTE *pData,
                 const UINT32 nNumFramesToRead,
                 int num_channels,
                 int bits_per_sample,
                 bool is_float,
                 const WAVEFORMATEX *pwfx_win = nullptr);

#endif // AUDIOBUF_H_
