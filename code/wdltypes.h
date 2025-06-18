#ifndef WDLTYPES_H_
#define WDLTYPES_H_

// Basic types that might be expected in a wdltypes.h for older projects
// This is a minimal stub to try to get past "file not found" errors.

#if !defined(_WIN32) && !defined(GUID_DEFINED)
// This GUID_DEFINED might conflict if utility.h also defines it.
// Ensure only one definition if utility.h is included before this.
// However, ns-eel2 files include this first.
#define GUID_DEFINED
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
#endif

#if !defined(_WIN32)
    #include <stdint.h> // For intptr_t, uintptr_t
    typedef intptr_t INT_PTR;
    typedef uintptr_t UINT_PTR;
    typedef unsigned long DWORD_PTR;
    typedef int BOOL;
    #ifndef TRUE
        #define TRUE 1
    #endif
    #ifndef FALSE
        #define FALSE 0
    #endif
    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned int DWORD;
    typedef void* HWND; // Basic HWND definition
#else
    // On Windows, windows.h should provide these
    #include <windows.h>
#endif

#endif // WDLTYPES_H_
