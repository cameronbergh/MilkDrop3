// linux_utility_stubs.cpp
// Provides implementations for functions declared in utility.h for the Linux platform.

#ifndef _WIN32 // Ensure this whole file is only compiled for non-Windows

#include "utility.h" // To get function declarations and common types like GUID (if defined there for Linux)
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cwchar>
#include <cstring> // For memset, wcslen, wcsncpy
#include <map>     // For INI stubs
#include <algorithm> // For std::replace
#include <sys/stat.h> // For mkdir
#include <wordexp.h>  // For wordexp (tilde expansion)

// Define types if not already available via utility.h's Linux path
// These are basic fallbacks. A more robust solution would use a shared config.h or ensure utility.h handles this.
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
#endif

#ifndef _LOCALE_T_DEFINED
typedef void* _locale_t; // Basic placeholder
#define _LOCALE_T_DEFINED
#endif
_locale_t g_use_C_locale = (_locale_t)0; // Placeholder

// Definition for GUID_NULL, expected to be declared as extern in the shim utility.h
const GUID GUID_NULL = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

// Helper to convert std::wstring to std::string (UTF-8)
std::string LinuxWstringToString(const std::wstring& wstr) {
    // This is a simplified conversion. Production code should handle UTF-8 properly.
    // std::wstring_convert is deprecated in C++17.
    // For basic ASCII, this might suffice:
    std::string str(wstr.length(), ' ');
    std::transform(wstr.begin(), wstr.end(), str.begin(), [](wchar_t wc){
        if (wc < 0x80) return static_cast<char>(wc); // Basic ASCII
        return '?'; // Placeholder for non-ASCII
    });
    return str;
}
// Helper to convert const char* to std::wstring
std::wstring LinuxStringToWstring(const std::string& str) {
    std::wstring wstr(str.length(), L' ');
    std::transform(str.begin(), str.end(), wstr.begin(), [](char c){ return static_cast<wchar_t>(c); });
    return wstr;
}


// Very simplified INI handling for stubs
static std::map<std::wstring, std::map<std::wstring, std::wstring>> linux_ini_data;
static std::wstring linux_ini_current_file_path; // Store full path

std::wstring GetLinuxConfigFilePathRecursive(LPCWSTR lpFileName) {
    std::wstring configFilePath;
    const char* configHome = getenv("XDG_CONFIG_HOME");
    std::string narrowPath;

    if (configHome && configHome[0] != '\0') {
        narrowPath = configHome;
        narrowPath += "/milkdrop3/";
    } else {
        const char* home = getenv("HOME");
        if (home && home[0] != '\0') {
            narrowPath = home;
            narrowPath += "/.config/milkdrop3/";
        } else {
            narrowPath = "./.milkdrop3_config/"; // Fallback to current directory if HOME is not set
        }
    }

    // Create directory using mkdir -p equivalent
    std::string tempPath = "";
    size_t start_pos = 0;
    do {
        size_t end_pos = narrowPath.find('/', start_pos);
        if (end_pos == std::string::npos) {
            tempPath += narrowPath.substr(start_pos);
            mkdir(tempPath.c_str(), 0755); // S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
            break;
        }
        tempPath += narrowPath.substr(start_pos, end_pos - start_pos + 1);
        mkdir(tempPath.c_str(), 0755);
        start_pos = end_pos + 1;
    } while(true);


    configFilePath = LinuxStringToWstring(narrowPath);

    if (lpFileName) {
        if (lpFileName[0] == L'/') {
             configFilePath = lpFileName;
        } else {
             configFilePath += lpFileName;
        }
    } else {
        configFilePath += L"milkdrop.ini";
    }
    return configFilePath;
}


void LoadIniFileLinux(LPCWSTR lpFileName) {
    std::wstring filePath = GetLinuxConfigFilePathRecursive(lpFileName);
    if (filePath == linux_ini_current_file_path && !linux_ini_data.empty()) {
        return;
    }
    linux_ini_data.clear();
    linux_ini_current_file_path = filePath;

    std::wifstream inFile(LinuxWstringToString(filePath).c_str());
    if (!inFile.is_open()) {
        return;
    }
    std::wstring currentSection;
    std::wstring line;
    while (std::getline(inFile, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(L" \t\r\n"));
        line.erase(line.find_last_not_of(L" \t\r\n") + 1);

        if (line.empty() || line[0] == L';' || line[0] == L'#') continue;

        if (line[0] == L'[' && line.back() == L']') {
            currentSection = line.substr(1, line.length() - 2);
        } else if (!currentSection.empty()) {
            size_t equalsPos = line.find(L'=');
            if (equalsPos != std::wstring::npos) {
                std::wstring key = line.substr(0, equalsPos);
                std::wstring value = line.substr(equalsPos + 1);
                key.erase(0, key.find_first_not_of(L" \t")); key.erase(key.find_last_not_of(L" \t") + 1);
                value.erase(0, value.find_first_not_of(L" \t")); value.erase(value.find_last_not_of(L" \t") + 1);
                linux_ini_data[currentSection][key] = value;
            }
        }
    }
    inFile.close();
}

void SaveIniFileLinux(LPCWSTR lpFileName) {
    if (lpFileName == nullptr) return;
    std::wstring filePath = GetLinuxConfigFilePathRecursive(lpFileName);
    std::wofstream outFile(LinuxWstringToString(filePath).c_str());
    if (!outFile.is_open()) {
        return;
    }
    for (const auto& sectionPair : linux_ini_data) {
        outFile << L"[" << sectionPair.first << L"]" << std::endl;
        for (const auto& keyPair : sectionPair.second) {
            outFile << keyPair.first << L"=" << keyPair.second << std::endl;
        }
        outFile << std::endl;
    }
    outFile.close();
}


DWORD GetPrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault,
                               LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName) {
    LoadIniFileLinux(lpFileName);
    if (lpAppName && lpKeyName && lpReturnedString && nSize > 0) {
        auto it_app = linux_ini_data.find(lpAppName);
        if (it_app != linux_ini_data.end()) {
            auto it_key = it_app->second.find(lpKeyName);
            if (it_key != it_app->second.end()) {
                wcsncpy(lpReturnedString, it_key->second.c_str(), nSize);
                if (nSize > 0) lpReturnedString[nSize-1] = L'\0'; // Ensure null termination
                return wcslen(lpReturnedString);
            }
        }
        if (lpDefault) {
            wcsncpy(lpReturnedString, lpDefault, nSize);
            if (nSize > 0) lpReturnedString[nSize-1] = L'\0';
            return wcslen(lpReturnedString);
        }
    }
    if (nSize > 0 && lpReturnedString) lpReturnedString[0] = L'\0';
    return 0;
}

BOOL WritePrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString,
                                LPCWSTR lpFileName) {
    if (!lpAppName || !lpKeyName || !lpFileName) return FALSE;
    LoadIniFileLinux(lpFileName);
    linux_ini_data[std::wstring(lpAppName)][std::wstring(lpKeyName)] = (lpString ? std::wstring(lpString) : std::wstring());
    SaveIniFileLinux(lpFileName);
    return TRUE;
}

int GetPrivateProfileIntW(LPCWSTR szSectionName, LPCWSTR szKeyName, int nDefault, LPCWSTR szIniFile) {
    wchar_t default_str[32];
    swprintf(default_str, 32, L"%d", nDefault);
    wchar_t value_str[256];
    GetPrivateProfileStringW(szSectionName, szKeyName, default_str, value_str, 256, szIniFile);
    int val = nDefault;
    if (swscanf(value_str, L"%d", &val) == 1) {
        return val;
    }
    return nDefault;
}

// Definition of g_use_C_locale for utility.cpp is complex without proper C locale setup.
// For now, this assumes standard C functions on Linux will behave as "C" locale by default for numerics.
// If specific locale behavior is needed, this needs proper initialization.
// _locale_t g_use_C_locale = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0); // Example of proper setup

float GetPrivateProfileFloatW(LPCWSTR szSectionName, LPCWSTR szKeyName, float fDefault, LPCWSTR szIniFile) {
    wchar_t default_str[64];
    swprintf(default_str, 64, L"%f", fDefault); // Use standard swprintf
    wchar_t value_str[256];
    GetPrivateProfileStringW(szSectionName, szKeyName, default_str, value_str, 256, szIniFile);
    float val = fDefault;
    if (swscanf(value_str, L"%f", &val) == 1) { // Use standard swscanf
         return val;
    }
    return fDefault;
}

bool WritePrivateProfileIntW(int d, LPCWSTR szKeyName, LPCWSTR szIniFile, LPCWSTR szSectionName) {
    wchar_t value_str[32];
    swprintf(value_str, 32, L"%d", d);
    return WritePrivateProfileStringW(szSectionName, szKeyName, value_str, szIniFile);
}

bool WritePrivateProfileFloatW(float f, LPCWSTR szKeyName, LPCWSTR szIniFile, LPCWSTR szSectionName) {
    wchar_t value_str[64];
    swprintf(value_str, 64, L"%f", f);
    return WritePrivateProfileStringW(szSectionName, szKeyName, value_str, szIniFile);
}

intptr_t myOpenURL(HWND hwnd, const wchar_t *loc) {
    if (!loc) return 0;
    std::string url = LinuxWstringToString(std::wstring(loc));
    std::string safe_url;
    for (char c : url) {
        if (c == '"') safe_url += "\\\"";
        else safe_url += c;
    }
    std::string cmd = "xdg-open \"" + safe_url + "\" > /dev/null 2>&1"; // Suppress output
    int ret = system(cmd.c_str());
    if (ret == 0) return 33;
    // fprintf(stderr, "Failed to open URL '%s' with xdg-open, status: %d\n", url.c_str(), ret);
    return 0;
}

void RemoveExtension(wchar_t *str) {
    if (!str) return;
    wchar_t *p = wcsrchr(str, L'.');
    if (p) *p = L'\0';
}

void RemoveSingleAmpersands(wchar_t *str) {
    // Placeholder - GDI specific, may not be needed or implemented differently for GL text
}

#include <cpuid.h>
bool CheckForMMX() {
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) { return (edx & (1 << 23)) != 0; }
    return true;
}

bool CheckForSSE() {
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) { return (edx & (1 << 25)) != 0; }
    return true;
}

void TextToGuid(const char *str, GUID *pGUID) {
    if (pGUID) memset(pGUID, 0, sizeof(GUID));
    if (!str || !pGUID) return;
    unsigned long d1; unsigned int d2, d3, d4[8];
    int result = sscanf(str, "%8lx-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x",
        &d1, &d2, &d3, &d4[0], &d4[1], &d4[2], &d4[3], &d4[4], &d4[5], &d4[6], &d4[7]);
    if (result == 11) {
        pGUID->Data1 = d1; pGUID->Data2 = (unsigned short)d2; pGUID->Data3 = (unsigned short)d3;
        for(int i=0; i<8; ++i) pGUID->Data4[i] = (unsigned char)d4[i];
    }
}

void GuidToText(const GUID *pGUID, char *str, int nStrLen) {
    if (str && nStrLen > 0) str[0] = '\0';
    if (!pGUID || !str || nStrLen <= 0) return;
    snprintf(str, nStrLen, "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        pGUID->Data1, pGUID->Data2, pGUID->Data3,
        pGUID->Data4[0], pGUID->Data4[1], pGUID->Data4[2], pGUID->Data4[3],
        pGUID->Data4[4], pGUID->Data4[5], pGUID->Data4[6], pGUID->Data4[7]);
}

void* GetTextResource(UINT id, int no_fallback) {
    return nullptr;
}

void MissingDirectX(HWND hwnd) { /* No-op on Linux */ }
void GetDesktopFolder(char *szDesktopFolder) { if(szDesktopFolder) szDesktopFolder[0] = '\0'; }

#endif // !_WIN32
