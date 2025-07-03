#ifndef __NULLSOFT_DX9_PLUGIN_SHELL_DXCONTEXT_H_LINUX_SHIM__
#define __NULLSOFT_DX9_PLUGIN_SHELL_DXCONTEXT_H_LINUX_SHIM__ 1

// Linux shim for dxcontext.h

#include "shell_defines.h" // Should resolve to the shim version
                           // which includes the shim utility.h for basic types

#ifndef _WIN32 // Guard for GLFW and OpenGL specific parts
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
    // GLAD is included in the .cpp file where OpenGL functions are called
#endif

// DXCONTEXT_PARAMS: Minimal version for Linux
// The original struct in the Windows version of dxcontext.h is more complex.
// This shim provides just enough for basic window/mode parameters.
typedef struct
{
    // Common parameters needed for both D3D and OpenGL context/window
    int width;
    int height;
    int windowed;
    wchar_t szIniFile[260]; // MAX_PATH equivalent for storing ini file path

    // Linux/GLFW specific parameters can be added here if needed later
    // For example, specific monitor, or GL context version hints if not hardcoded.
}
DXCONTEXT_PARAMS;


class DXContext
{
public:
    // PUBLIC DATA - Accessible by CPlugin
    int m_ready;            // 0 if not ready, 1 if ready
    int m_window_width;     // Current window width
    int m_window_height;    // Current window height
    int m_client_width;     // drawable area width
    int m_client_height;    // drawable area height
    DXCONTEXT_PARAMS m_current_mode; // Store current parameters

#ifndef _WIN32
    GLFWwindow* m_glfwWindow; // GLFW window handle for Linux
    // Potentially other OpenGL-specific public members if needed by CPlugin directly
#endif

    // Constructor & Destructor
    // The HWND parameter is kept for signature compatibility with Windows calls,
    // but it will be ignored on Linux for this simplified context.
    DXContext(void* ignored_device_ptr, void* ignored_d3dpp_ptr, HWND ignored_hwnd, const wchar_t* szIniFile);
    ~DXContext();

    // Core methods to be stubbed for Linux
    BOOL StartOrRestartDevice(DXCONTEXT_PARAMS *pParams);
    void SetViewport(); // OpenGL viewport setting

    // Other methods from original dxcontext.h that might be called by CPlugin
    // can be added here as stubs if they cause linker errors.
    // For now, keeping it minimal.
    // HWND GetHwnd() { return (HWND)m_glfwWindow; } // Example if needed

protected:
    wchar_t m_szIniFile[260]; // MAX_PATH

    // Protected common methods
    BOOL Internal_Init(DXCONTEXT_PARAMS *pParams, BOOL bFirstInit);
    void Internal_CleanUp();
};

// Minimal error codes if any are used by common calling code
// The original dxcontext.h had many DXC_ERR_... codes
#define DXC_ERR_RESIZEFAILED -8

#endif // __NULLSOFT_DX9_PLUGIN_SHELL_DXCONTEXT_H_LINUX_SHIM__
