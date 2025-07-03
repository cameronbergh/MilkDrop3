// linux_dxcontext_stubs.cpp
// Stub implementations for DXContext class methods for Linux.

#ifndef _WIN32 // Ensure this file is only compiled for non-Windows

#include "dxcontext.h" // Should pick up the shim version
#include "glad/glad.h" // For glViewport, glClearColor, glClear
// GLFW is included via dxcontext.h (shim)
#include <stdio.h>   // For fprintf, for error logging
#include <string.h>  // For wcscpy, wcsncpy (if used for szIniFile)

// Constructor
DXContext::DXContext(void* ignored_device_ptr, void* ignored_d3dpp_ptr, HWND ignored_hwnd, const wchar_t* szIniFile)
    : m_ready(0),
      m_window_width(0), m_window_height(0),
      m_client_width(0), m_client_height(0),
      m_glfwWindow(nullptr)
{
    if (szIniFile) {
        // Ensure m_szIniFile is large enough (e.g. MAX_PATH or 260 as in header)
        wcsncpy(m_szIniFile, szIniFile, sizeof(m_szIniFile)/sizeof(m_szIniFile[0]) - 1);
        m_szIniFile[sizeof(m_szIniFile)/sizeof(m_szIniFile[0]) - 1] = L'\0';
        m_current_mode.szIniFile[0] = L'\0'; // Also ensure this is initialized
        wcsncpy(m_current_mode.szIniFile, szIniFile, sizeof(m_current_mode.szIniFile)/sizeof(m_current_mode.szIniFile[0]) -1);
         m_current_mode.szIniFile[sizeof(m_current_mode.szIniFile)/sizeof(m_current_mode.szIniFile[0]) -1] = L'\0';
    } else {
        m_szIniFile[0] = L'\0';
        m_current_mode.szIniFile[0] = L'\0';
    }
    // Other members of m_current_mode (width, height, windowed) are set in StartOrRestartDevice
}

// Destructor
DXContext::~DXContext() {
    Internal_CleanUp(); // GLFW window destruction is handled here
    // glfwTerminate(); // This should be called globally when the application exits, not per DXContext instance.
                       // Typically after the main loop in plugin.cpp.
}

// Internal_Init: Simplified for GLFW window creation and OpenGL context
BOOL DXContext::Internal_Init(DXCONTEXT_PARAMS *pParams, BOOL bFirstInit) {
    if (!pParams) return FALSE;

    // glfwInit() should have been called once globally at application start (e.g., in plugin.cpp's main())
    // This function now primarily creates the window and GL context.

    m_current_mode = *pParams; // Store current mode settings
    m_window_width = pParams->width;
    m_window_height = pParams->height;
    m_client_width = pParams->width; // Initially, client and window are same
    m_client_height = pParams->height;

    // Set GLFW window hints for OpenGL context (version 3.3 core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // macOS requires forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = pParams->windowed ? NULL : glfwGetPrimaryMonitor();
    m_glfwWindow = glfwCreateWindow(pParams->width, pParams->height, "MilkDrop3 (OpenGL Shim)", monitor, NULL);

    if (!m_glfwWindow) {
        fprintf(stderr, "DXContext Shim: Failed to create GLFW window\n");
        // glfwTerminate(); // Global terminate should be elsewhere
        return FALSE;
    }

    glfwMakeContextCurrent(m_glfwWindow);
    // Vsync (1 = on, 0 = off)
    glfwSwapInterval(1);

    // Initialize GLAD (OpenGL function loader)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "DXContext Shim: Failed to initialize GLAD\n");
        glfwDestroyWindow(m_glfwWindow);
        m_glfwWindow = nullptr;
        // glfwTerminate();
        return FALSE;
    }

    // Set initial viewport
    SetViewport();

    m_ready = TRUE;
    return TRUE;
}

// Internal_CleanUp: Destroys the GLFW window
void DXContext::Internal_CleanUp() {
    if (m_glfwWindow) {
        glfwDestroyWindow(m_glfwWindow);
        m_glfwWindow = nullptr;
    }
    m_ready = FALSE;
    // glfwTerminate(); // Global terminate should be elsewhere
}

// StartOrRestartDevice: Handles initialization and re-initialization
BOOL DXContext::StartOrRestartDevice(DXCONTEXT_PARAMS *pParams) {
    if (!pParams) return FALSE;

    if (m_ready) { // If already initialized, clean up first (e.g., window mode change)
        Internal_CleanUp();
        // Need to re-initialize GLFW if it was terminated, but it should be global.
        // If glfwInit() was called once, destroying and recreating window is usually enough.
    }

    // Initialize with new parameters. bFirstInit is less critical here
    // as GLFW window creation is fairly self-contained.
    return Internal_Init(pParams, TRUE);
}

// SetViewport: Sets the OpenGL viewport
void DXContext::SetViewport() {
    if (m_ready && m_glfwWindow) {
        // Use framebuffer size for viewport as it might differ from window size (e.g. Retina)
        int fb_width, fb_height;
        glfwGetFramebufferSize(m_glfwWindow, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        // Update stored client width/height if they are meant to reflect framebuffer size
        // m_client_width = fb_width;
        // m_client_height = fb_height;
    }
}

// Add stubs for any other DXContext methods that might be called from CPlugin
// and are declared in the shim dxcontext.h.
// For example: HWND DXContext::GetHwnd() { return (HWND)m_glfwWindow; } // If needed

#endif // !_WIN32
