#ifdef _WIN32 // Placed at the very top to control CRT an Windows.h includes
    #ifdef DEBUG
    #define _CRTDBG_MAP_ALLOC
    #endif
    #include <crtdbg.h> // Windows-specific
    #include <windows.h>
    #include <process.h> // For _beginthreadex
    #include <ShellScalingApi.h> // for dpi awareness
    #pragma comment(lib, "shcore.lib") // for dpi awareness
    #include "resource.h" // For IDI_PLUGIN_ICON etc.
#else // Linux
    #include <stdio.h>
    #include "glad/glad.h"
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
    #include <string.h>
    #include <vector>
    #include <map>
    #include "menu_strings.h"
    #include <pthread.h> // For pthreads
    #include <unistd.h>  // For usleep
    #include "../audio/log.h" // For LOG, ERR macros
    // Define missing Windows types for cross-compatibility if not in utility.h shim
    #ifndef _WIN32_TYPES_DEFINED
    #define _WIN32_TYPES_DEFINED
    typedef void* HWND;
    typedef void* HANDLE; // Note: HANDLE is void* on Win64, but int on Win32. For pthreads, pthread_t is different.
    typedef void* HICON;
    typedef void* HMODULE;
    typedef void* HINSTANCE; // Added for WinMain and other functions
    typedef unsigned int UINT;
    typedef unsigned long DWORD;
    typedef long LONG_PTR;
    typedef LONG_PTR LRESULT;
    typedef unsigned int UINT_PTR;
    typedef UINT_PTR WPARAM;
    typedef LONG_PTR LPARAM;
    typedef wchar_t WCHAR;
    typedef WCHAR *LPWSTR;
    typedef const WCHAR *LPCWSTR;
    typedef char CHAR;
    typedef CHAR *LPSTR; // Added for PSTR
    typedef const CHAR *LPCSTR;
    typedef char* PSTR; // Added for WinMain
    #define WINAPI
    #define APIENTRY
    #define __stdcall
    #define CALLBACK
    typedef struct tagRECT { long left; long top; long right; long bottom; } RECT;
    #define CW_USEDEFAULT ((int)0x80000000)
    #define SW_SHOW 5
    #define WM_USER 0x0400
    #define CS_DBLCLKS 0x0008
    #define IDC_ARROW ((LPCWSTR)32512) // Standard arrow cursor ID
    #define BLACK_BRUSH 4
    #define WS_OVERLAPPEDWINDOW 0
    #define WM_LBUTTONDBLCLK 0x0203
    #define WM_CLOSE 0x0010
    #define WM_DESTROY 0x0002
    #define WM_KEYDOWN 0x0100
    #define WM_SYSKEYDOWN 0x0104
    #define VK_F4 0x73
    #define VK_SHIFT 0x10
    #define VK_RETURN 0x0D
    #define WM_SETICON 0x0080
    #define ICON_BIG 1
    #define ICON_SMALL 0
    #define WM_NULL 0x0000
    #define PM_NOREMOVE 0x0000
    #define PM_REMOVE 0x0001
    #define WM_QUIT 0x0012
    // For DPI awareness function call
    #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void*)-4) // Placeholder, actual value might differ
    typedef void* DPI_AWARENESS_CONTEXT;
    #endif // _WIN32_TYPES_DEFINED
#endif

#include <stdlib.h>
#include <malloc.h>

// D3D9 includes only for Windows
#ifdef _WIN32
#include <d3d9.h>
#include <math.h>
#include <dwmapi.h>
#endif


#include "plugin.h"
#include <mutex>
#include <atomic>

#include "../audio/common.h" // Corrected path


#ifdef _WIN32
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __attribute__((visibility("default")))
#endif

#define SAMPLE_SIZE 576
const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 800;

CPlugin g_plugin;

#ifdef _WIN32
    HINSTANCE api_orig_hinstance = nullptr;
    _locale_t g_use_C_locale_milkdrop = nullptr;
    char keyMappings[8];

    static IDirect3D9* pD3D9 = nullptr;
    static IDirect3DDevice9* pD3DDevice = nullptr;
    static D3DPRESENT_PARAMETERS d3dPp;

    static LONG lastWindowStyle = 0;
    static LONG lastWindowStyleEx = 0;

    static bool fullscreen = false;
    static bool stretch = false;
    static RECT lastRect = { 0 };

    static HMODULE module_handle = nullptr;
    static std::atomic<HANDLE> render_thread_handle = nullptr;
    static unsigned render_thread_id = 0;
    static HICON icon_handle = nullptr;
#else // Linux globals
    GLFWwindow* g_glfwWindow = nullptr;
    static int linux_menu_selection = 0;
    static const int LINUX_MENU_ITEM_COUNT = 5;
    std::wstring g_tooltip_text_linux;

    static GLuint text_shader_program_gl = 0;
    static GLuint text_VAO_gl = 0, text_VBO_gl = 0;

    const char* text_vertex_shader_source_gl = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    uniform mat4 projection;
    uniform mat4 model;
    void main()
    {
        gl_Position = projection * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
    )glsl";

    const char* text_fragment_shader_source_gl = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 textColor;
    void main()
    {
        FragColor = vec4(textColor, 1.0);
    }
    )glsl";
    GLuint CompileShader(GLenum type, const char* source); // Declaration
    GLuint CreateShaderProgram(const char* vs_source, const char* fs_source); // Declaration
    void SetupTextRendering(int screenWidth, int screenHeight); // Declaration
    void RenderStringOpenGL(const char* str, float x, float y, float scale, float r, float g, float b); // Declaration
    void SetOrthoProjection(float left, float right, float bottom, float top, float near_p, float far_p); // Declaration
#endif

static std::mutex pcm_mutex_global;
static unsigned char pcm_left_in_global[SAMPLE_SIZE];
static unsigned char pcm_right_in_global[SAMPLE_SIZE];
static unsigned char pcm_left_out_global[SAMPLE_SIZE];
static unsigned char pcm_right_out_global[SAMPLE_SIZE];


#ifdef _WIN32
// All Windows-specific D3D functions, window procedures, thread functions are wrapped in _WIN32.
void InitD3d(HWND hwnd, int width, int height) { pD3D9 = Direct3DCreate9(D3D_SDK_VERSION); if(!pD3D9) return; D3DDISPLAYMODE mode; pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode); UINT adapterId = g_plugin.m_adapterId; if (adapterId >= pD3D9->GetAdapterCount()) adapterId = D3DADAPTER_DEFAULT; memset(&d3dPp, 0, sizeof(d3dPp)); d3dPp.BackBufferCount = 1; d3dPp.BackBufferFormat = mode.Format; d3dPp.BackBufferWidth = width; d3dPp.BackBufferHeight = height; d3dPp.SwapEffect = D3DSWAPEFFECT_DISCARD; d3dPp.EnableAutoDepthStencil = TRUE; d3dPp.AutoDepthStencilFormat = D3DFMT_D24X8; d3dPp.Windowed = TRUE; d3dPp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; d3dPp.MultiSampleType = D3DMULTISAMPLE_NONE; d3dPp.hDeviceWindow = hwnd; pD3D9->CreateDevice(adapterId, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPp, &pD3DDevice); }
void DeinitD3d() { if (pD3DDevice) { pD3DDevice->Release(); pD3DDevice = nullptr; } if (pD3D9) { pD3D9->Release(); pD3D9 = nullptr; } }
void ToggleStretch(HWND hwnd) { /* ... original ... */ }
void ToggleFullScreen(HWND hwnd) { /* ... original ... */ }
LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { /* ... original (ensure g_plugin.PluginShellWindowProc is called) ... */ return DefWindowProc(hWnd, uMsg, wParam, lParam); }

void RenderFrame_Windows() {
    { std::unique_lock<std::mutex> lock(pcm_mutex_global);
      memcpy(pcm_left_out_global, pcm_left_in_global, SAMPLE_SIZE);
      memcpy(pcm_right_out_global, pcm_right_in_global, SAMPLE_SIZE);
      memset(pcm_left_in_global, 0, SAMPLE_SIZE);
      memset(pcm_right_in_global, 0, SAMPLE_SIZE); }
    g_plugin.PluginRender( (unsigned char*) pcm_left_out_global, (unsigned char*) pcm_right_out_global);
}

unsigned __stdcall CreateWindowAndRun(void* data) {
    HINSTANCE instance = (HINSTANCE) data;
#if defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)) // CRT Debug calls wrapped
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // _CrtSetBreakAlloc(60); // Optional: set a specific memory allocation number to break on
#endif
    WNDCLASSW wndClass = {0}; wndClass.style = CS_DBLCLKS; wndClass.lpfnWndProc = StaticWndProc; wndClass.hInstance = instance; wndClass.hCursor = LoadCursor(NULL, IDC_ARROW); wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); wndClass.lpszClassName = L"Direct3DWindowClass";
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if (!RegisterClassW(&wndClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return 0;
    HWND hwnd = CreateWindowW(L"Direct3DWindowClass", L"MilkDrop 3.0 (Windows)", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, NULL, instance, 0);
    if (!hwnd) return 0;
    if (!icon_handle) icon_handle = LoadIconW(instance, MAKEINTRESOURCEW(IDI_PLUGIN_ICON));
    if (icon_handle) { SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon_handle); SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon_handle); }
    ShowWindow(hwnd, SW_SHOW);
    g_plugin.PluginPreInitialize((HWND)0, (HINSTANCE)0); // HWND and HINSTANCE might not be needed or available this early for all logic
    InitD3d(hwnd, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    if(pD3DDevice) g_plugin.PluginInitialize(pD3DDevice, &d3dPp, hwnd, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT); else return 0; // Exit if D3D device failed
    MSG msg = {0};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessage(&msg); }
        else {
            GetAudioBuf(pcm_left_in_global, pcm_right_in_global, SAMPLE_SIZE);
            RenderFrame_Windows();
        }
    }
    g_plugin.MyWriteConfig(); g_plugin.PluginQuit(); DeinitD3d(); render_thread_handle = nullptr; render_thread_id = 0;
    return 1;
}
void StartRenderThread(HINSTANCE instance) { render_thread_handle = (HANDLE) _beginthreadex(nullptr, 0, &CreateWindowAndRun, (void *) instance, 0, &render_thread_id); }
int StartThreads(HINSTANCE instance) {
    // Minimal StartThreads for now, focusing on render thread.
    // Original audio thread logic (CreateThread for LoopbackCaptureThreadFunction, events) needs to be integrated here.
    // For now, audio capture might need to be started differently or is assumed to be handled by StartAudioCapture() called elsewhere.
    StartRenderThread(instance);
    if (render_thread_handle) {
        WaitForSingleObject(render_thread_handle, INFINITE);
        CloseHandle(render_thread_handle); // Close handle when thread finishes
        render_thread_handle = nullptr;
    }
    return 0;
}

#ifndef COMPILE_AS_DLL
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
        api_orig_hinstance = hInstance;
        g_use_C_locale_milkdrop = _create_locale(LC_NUMERIC, "C");
        int result = StartThreads(hInstance);
        if (g_use_C_locale_milkdrop) _free_locale(g_use_C_locale_milkdrop);
        return result;
    }
#else
    BOOL APIENTRY DllMain(HMODULE hModuleParam, DWORD reason, LPVOID lpReserved) {
        module_handle = hModuleParam;
        api_orig_hinstance = hModuleParam;
        if (reason == DLL_PROCESS_ATTACH) {
            g_use_C_locale_milkdrop = _create_locale(LC_NUMERIC, "C");
        } else if (reason == DLL_PROCESS_DETACH) {
            if (g_use_C_locale_milkdrop) _free_locale(g_use_C_locale_milkdrop);
        }
        return TRUE;
    }
#endif // COMPILE_AS_DLL / WinMain

#else // Linux main entry point and GLFW loop

// Implementations for shader helpers and text rendering for Linux
GLuint CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type); glShaderSource(shader, 1, &source, NULL); glCompileShader(shader);
    GLint success; glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) { char infoLog[512]; glGetShaderInfoLog(shader, 512, NULL, infoLog); fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog); glDeleteShader(shader); return 0; }
    return shader;
}
GLuint CreateShaderProgram(const char* vs_source, const char* fs_source) {
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vs_source); GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fs_source);
    if (!vs || !fs) { if(vs) glDeleteShader(vs); if(fs) glDeleteShader(fs); return 0; }
    GLuint program = glCreateProgram(); glAttachShader(program, vs); glAttachShader(program, fs); glLinkProgram(program);
    GLint success; glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) { char infoLog[512]; glGetProgramInfoLog(program, 512, NULL, infoLog); fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog); glDeleteProgram(program); program = 0; }
    glDeleteShader(vs); glDeleteShader(fs); return program;
}
void SetOrthoProjection(float left, float right, float bottom, float top, float near_p, float far_p) {
    if (!text_shader_program_gl) return;
    float ortho_projection[4][4] = {
        { 2.0f / (right - left), 0.0f, 0.0f, 0.0f }, { 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f },
        { 0.0f, 0.0f, -2.0f / (far_p - near_p), 0.0f },
        { -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far_p + near_p) / (far_p - near_p), 1.0f }
    };
    glUseProgram(text_shader_program_gl);
    glUniformMatrix4fv(glGetUniformLocation(text_shader_program_gl, "projection"), 1, GL_FALSE, &ortho_projection[0][0]);
}
void SetupTextRendering(int screenWidth, int screenHeight) {
    text_shader_program_gl = CreateShaderProgram(text_vertex_shader_source_gl, text_fragment_shader_source_gl);
    if (!text_shader_program_gl) { fprintf(stderr, "Failed to create text shader program.\n"); return; }
    SetOrthoProjection(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
    float vertices[] = { 0.0f,1.0f, 0.0f,0.0f, 1.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f, 1.0f,1.0f };
    glGenVertexArrays(1, &text_VAO_gl); glGenBuffers(1, &text_VBO_gl);
    glBindVertexArray(text_VAO_gl); glBindBuffer(GL_ARRAY_BUFFER, text_VBO_gl);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0); glBindVertexArray(0);
    if (glGetError() != GL_NO_ERROR) fprintf(stderr, "GL error during text VAO/VBO setup.\n");
}
void RenderStringOpenGL(const char* str, float x, float y, float scale, float r_col, float g_col, float b_col) {
    if (!text_shader_program_gl || !text_VAO_gl || !str) return;
    glUseProgram(text_shader_program_gl);
    glUniform3f(glGetUniformLocation(text_shader_program_gl, "textColor"), r_col, g_col, b_col);
    glBindVertexArray(text_VAO_gl);
    float char_width_screen = 8.0f * scale; float char_height_screen = 16.0f * scale; float current_x = x;
    while (*str) {
        if (*str == '\n') { current_y -= char_height_screen; current_x = x; }
        else {
            float model_matrix[4][4] = {
                { char_width_screen, 0.0f, 0.0f, 0.0f }, { 0.0f, char_height_screen, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f }, { current_x, y - char_height_screen, 0.0f, 1.0f }
            };
            glUniformMatrix4fv(glGetUniformLocation(text_shader_program_gl, "model"), 1, GL_FALSE, &model_matrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 6); current_x += char_width_screen;
        } str++;
    } glBindVertexArray(0);
}

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            if (g_plugin.m_UI_mode == UI_MENU ) { g_plugin.m_UI_mode = UI_REGULAR; }
            else { glfwSetWindowShouldClose(window, GLFW_TRUE); }
        } else if (key == GLFW_KEY_M && mods == 0) {
            if (g_plugin.m_UI_mode == UI_REGULAR) { g_plugin.m_UI_mode = UI_MENU; g_plugin.m_pCurMenu = &g_plugin.m_menuPreset; linux_menu_selection = 0; }
            else if (g_plugin.m_UI_mode == UI_MENU) { g_plugin.m_UI_mode = UI_REGULAR; }
        } else if (g_plugin.m_UI_mode == UI_MENU) {
            if (key == GLFW_KEY_UP) { linux_menu_selection = (linux_menu_selection - 1 + LINUX_MENU_ITEM_COUNT); linux_menu_selection %= LINUX_MENU_ITEM_COUNT; }
            else if (key == GLFW_KEY_DOWN) { linux_menu_selection = (linux_menu_selection + 1) % LINUX_MENU_ITEM_COUNT; }
            else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_RIGHT) {
                wchar_t temp_buf[256];
                printf("Selected menu item (Linux placeholder): %ls\n", LinuxLangStringW(IDS_UNTITLED_MENU_ITEM + linux_menu_selection, temp_buf, 256));
            }
        }
    }
}

int main(int argc, char* argv[]) {
    g_plugin.MyPreInitialize();
    g_plugin.MyReadConfig();
    g_plugin.InitializeMenus();

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) { fprintf(stderr, "Failed to initialize GLFW\n"); return -1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    g_glfwWindow = glfwCreateWindow(1024, 768, "MilkDrop3 OpenGL (Linux)", NULL, NULL);
    if (!g_glfwWindow) { fprintf(stderr, "Failed to create GLFW window\n"); glfwTerminate(); return -1; }

    glfwMakeContextCurrent(g_glfwWindow);
    glfwSetKeyCallback(g_glfwWindow, glfw_key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n"); glfwDestroyWindow(g_glfwWindow); glfwTerminate(); return -1;
    }

    int fbw, fbh; glfwGetFramebufferSize(g_glfwWindow, &fbw, &fbh);
    SetupTextRendering(fbw, fbh);
    if (!text_shader_program_gl) { fprintf(stderr, "Text rendering setup failed. Exiting.\n"); glfwDestroyWindow(g_glfwWindow); glfwTerminate(); return -1; }

    std::vector<std::wstring> linux_menu_items; wchar_t temp_wchar_buf[256];
    linux_menu_items.push_back(LinuxLangStringW(IDS_MENU_RANDOM_PRESET, temp_wchar_buf, 256));
    linux_menu_items.push_back(LinuxLangStringW(IDS_MENU_SELECT_PRESET, temp_wchar_buf, 256));
    linux_menu_items.push_back(LinuxLangStringW(IDS_MENU_EDIT_CUR_PRESET, temp_wchar_buf, 256));
    linux_menu_items.push_back(LinuxLangStringW(IDS_MENU_SHOW_FPS, temp_wchar_buf, 256));
    linux_menu_items.push_back(L"Exit MilkDrop (placeholder)");

    // StartAudioCapture(); // Needs to be called if audio input is desired

    while (!glfwWindowShouldClose(g_glfwWindow)) {
        int current_width, current_height; glfwGetFramebufferSize(g_glfwWindow, &current_width, &current_height);
        glViewport(0, 0, current_width, current_height);
        // if (current_width != fbw || current_height != fbh) { // Update projection if size changed
        //    fbw = current_width; fbh = current_height;
        //    SetOrthoProjection(0.0f, (float)fbw, 0.0f, (float)fbh, -1.0f, 1.0f);
        // }

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // GetAudioBuf(pcm_left_in_global, pcm_right_in_global, SAMPLE_SIZE);
        // g_plugin.PluginRender((unsigned char*)pcm_left_out_global, (unsigned char*)pcm_right_out_global);

        if (g_plugin.m_UI_mode == UI_MENU) {
            float y_pos = (float)current_height - 20.0f;
            float x_pos = 10.0f;
            float text_scale_val = 1.0f;

            std::string menu_title_str = WstringToUtf8String(g_plugin.m_pCurMenu ? g_plugin.m_pCurMenu->GetName() : L"Menu");
            RenderStringOpenGL(menu_title_str.c_str(), x_pos, y_pos, text_scale_val * 1.1f, 0.9f, 0.9f, 0.9f);
            y_pos -= 20.0f * 1.5f * text_scale_val;

            for (size_t i = 0; i < linux_menu_items.size(); ++i) { // Use size_t for loop
                 if (i < LINUX_MENU_ITEM_COUNT) { // Ensure we don't go out of bounds if LINUX_MENU_ITEM_COUNT is smaller
                    std::string item_str = WstringToUtf8String(linux_menu_items[i]);
                    if (i == (size_t)linux_menu_selection) RenderStringOpenGL(item_str.c_str(), x_pos + 10.0f, y_pos, text_scale_val, 1.0f, 1.0f, 0.0f);
                    else RenderStringOpenGL(item_str.c_str(), x_pos + 10.0f, y_pos, text_scale_val, 0.7f, 0.7f, 0.7f);
                    y_pos -= 20.0f * text_scale_val;
                }
            }
        }

        glfwSwapBuffers(g_glfwWindow);
        glfwPollEvents();
    }

    // StopAudioCapture();
    if (text_shader_program_gl) glDeleteProgram(text_shader_program_gl);
    if (text_VAO_gl) glDeleteVertexArrays(1, &text_VAO_gl);
    if (text_VBO_gl) glDeleteBuffers(1, &text_VBO_gl);

    // g_plugin.PluginQuit();

    if (g_glfwWindow) glfwDestroyWindow(g_glfwWindow);
    glfwTerminate();
    return 0;
}
#endif // _WIN32 / Linux

// The _DEBUG_STATE struct and its instance `ds` are Windows-specific CRT debug features.
#ifdef _WIN32
    #if defined(DEBUG) || defined(_DEBUG)
    struct _DEBUG_STATE {
        _DEBUG_STATE() {
            // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Already at top
            // _CrtSetBreakAlloc(60);
        }
        ~_DEBUG_STATE() {
            if (_CrtDumpMemoryLeaks() == TRUE) {
                // Optional: Output message if leaks detected
            }
        }
    };
    _DEBUG_STATE ds_win_debug;
    #endif // DEBUG || _DEBUG
#endif // _WIN32
