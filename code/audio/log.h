// log.h

#if defined(_WIN32)
#include <windows.h> // For LPCWSTR, OutputDebugStringW, etc.
#include <stdio.h>   // For vsprintf_s, vswprintf_s (used in log.cpp for Windows)

// Windows-specific declarations (wide strings)
void LOG(LPCWSTR format, ...);
void ERR(LPCWSTR format, ...);

// Windows-specific declarations (narrow strings - if these are meant to be public)
// void LOGA(LPCSTR format, ...);
// void ERRA(LPCSTR format, ...);

#else // Non-Windows platforms

#include <stdio.h>   // For printf, fprintf, va_list, vsnprintf
#include <stdarg.h>  // For va_list, va_start, va_end

// Non-Windows declarations (narrow strings)
void LOG(const char* format, ...);
void ERR(const char* format, ...);

// Define LPCWSTR and other Windows types if they are used in common code sections
// that are NOT platform-specific. For now, assuming they are confined to _WIN32 blocks
// or handled by other shims (like utility.h shim for basic types if needed).
// Note: If common.h or other headers included by Linux .cpp files directly use LPCWSTR
// from this file's global scope, this would need adjustment.
// Based on current changes, common.h wraps its windows.h include, so this should be okay.
#endif // _WIN32 / Non-Windows
