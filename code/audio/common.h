// common.h

#include <stdio.h> // Standard I/O, generally cross-platform

#ifdef _WIN32
// Windows-specific headers
#include <windows.h> // This is already guarded in its own shim for Linux includes
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>
#endif // _WIN32

// Cross-platform or project-specific headers
#include <mutex> // C++ standard mutex, cross-platform

#include "log.h"
#include "cleanup.h"
#include "prefs.h"
#include "loopback-capture.h"
#include "audiobuf.h"

// Ensure types like HRESULT, HANDLE, etc. if used in function prototypes
// below (that are NOT windows-specific) are handled by the utility.h shim
// or defined appropriately for Linux if this header is included by Linux .cpp files.
// For now, assuming utility.h shim (via other includes like loopback-capture.h) handles these.
#ifndef _WIN32
    // Minimal set of types if they are used in this header's declarations
    // and not covered by other shims that might be included before this.
    // This is mostly for function prototypes that might be exposed from here.
    // However, common.h mostly just includes other headers.
    // typedef void* HWND; // Example, if needed by a declaration in THIS file for Linux
#endif
