// Attempt to block d3d9.h and d3dx9.h if they're included after this fake windows.h
#define __D3D9_H__       // Common include guard for d3d9.h
#define _D3D9_H_         // Another possible guard for d3d9.h
#define D3D9_H           // Yet another possible guard
#define __D3DX9_H__      // Common include guard for d3dx9.h
#define _D3DX9_H_        // Another possible guard for d3dx9.h
#define D3DX9_H          // Yet another possible guard for d3dx9.h
#define DIRECT3D_VERSION 0x0900 // Often checked within d3d9.h related files

#ifndef FAKE_WINDOWS_H_FOR_LINUX
#define FAKE_WINDOWS_H_FOR_LINUX

// This is a minimal stand-in for windows.h on Linux.
// Its main purpose is to prevent "windows.h not found" errors
// when original, unmodified headers try to include it.

#include "utility.h" // This should pick up the shim utility.h

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef NULL
#define NULL 0
#endif

// Minimal D3D version, in case anything checks this after including windows.h
#ifndef DIRECT3D_VERSION
#define DIRECT3D_VERSION 0x0900 // Duplicated above, but fine
#endif

#endif // FAKE_WINDOWS_H_FOR_LINUX
