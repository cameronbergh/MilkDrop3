// log.cpp

#include "log.h"
#include <string.h> // For strnlen_s, wcsnlen_s, strncpy_s, wcsncpy_s (Windows), strlen, strcpy (Linux)
#include <stdio.h>  // Already included via log.h but good for explicitness (vprintf, etc.)
// va_list is included via log.h for Linux, and implicitly via windows.h/stdio.h for Windows

#define LOG_SIZE 512

#ifdef _WIN32
//LPCTSTR UNICODE _UNICODE // This seems like a comment, not active code

//--------------------------------------------------

//LPCSTR
void LOGA(LPCSTR format, ...) { // This is effectively Windows-only due to OutputDebugStringA
	char output_buff[LOG_SIZE];
	char err[20] = "";
	int sl = strnlen_s(err, 20); // strnlen_s is MSVC specific, but okay in WIN32 block
	strncpy_s(output_buff, err, sl); // strncpy_s is MSVC specific

	va_list args_list;
	va_start(args_list, format);
	vsprintf_s(&output_buff[sl], sizeof(output_buff) / sizeof(output_buff[0]) - sl, format, args_list);
	va_end(args_list);

	OutputDebugStringA(output_buff);
    printf("%s\n", output_buff);
}

//LPCWSTR
void LOG(LPCWSTR format, ...) {
	wchar_t output_buff[LOG_SIZE];
	wchar_t err[20] = L"";
	int sl = wcsnlen_s(err, 20); // wcsnlen_s is MSVC specific
	wcsncpy_s(output_buff, err, sl); // wcsncpy_s is MSVC specific

	va_list args_list;
	va_start(args_list, format);
	vswprintf_s(&output_buff[sl], sizeof(output_buff) / sizeof(output_buff[0]) - sl, format, args_list);
	va_end(args_list);

	OutputDebugStringW(output_buff);
    wprintf(L"%s\n", output_buff);
}

//--------------------------------------------------

//LPCSTR
void ERRA(LPCSTR format, ...) { // Windows-only
	char output_buff[LOG_SIZE];
	char err[20] = "Error: ";
	int sl = strnlen_s(err, 20);
	strncpy_s(output_buff, err, sl);

	va_list args_list;
	va_start(args_list, format);
	vsprintf_s(&output_buff[sl], sizeof(output_buff) / sizeof(output_buff[0]) - sl, format, args_list);
	va_end(args_list);

	OutputDebugStringA(output_buff);
    fprintf(stderr, "%s\n", output_buff);
}

//LPCWSTR
void ERR(LPCWSTR format, ...) {
	wchar_t output_buff[LOG_SIZE];
	wchar_t err[20] = L"Error: ";
	int sl = wcsnlen_s(err, 20);
	wcsncpy_s(output_buff, err, sl);

	va_list args_list;
	va_start(args_list, format);
	vswprintf_s(&output_buff[sl], sizeof(output_buff) / sizeof(output_buff[0]) - sl, format, args_list);
	va_end(args_list);

	OutputDebugStringW(output_buff);
    fwprintf(stderr, L"%s\n", output_buff);
}

//--------------------------------------------------

#else // Non-Windows (Linux, etc.)

// For non-Windows, LOG and ERR take const char*
void LOG(const char* format, ...) {
    // char output_buff[LOG_SIZE]; // Using direct vprintf
    va_list args_list;
    va_start(args_list, format);
    vprintf(format, args_list); // Print to stdout
    printf("\n"); // Add newline
    va_end(args_list);
    fflush(stdout); // Ensure it's visible immediately
}

void ERR(const char* format, ...) {
    // char output_buff[LOG_SIZE]; // Using direct vfprintf
    // char prefix[20] = "Error: ";
    // size_t prefix_len = strlen(prefix);
    // strcpy(output_buff, prefix);

    fprintf(stderr, "Error: "); // Print prefix to stderr
    va_list args_list;
    va_start(args_list, format);
    vfprintf(stderr, format, args_list); // Print to stderr
    fprintf(stderr, "\n"); // Add newline
    va_end(args_list);
    fflush(stderr); // Ensure it's visible
}

// LOGA and ERRA are not declared for Linux in log.h, so no definitions here.
// If they were needed, they'd be identical to LOG and ERR for Linux
// as there's no separate "wide" debug output string mechanism.

#endif // _WIN32
