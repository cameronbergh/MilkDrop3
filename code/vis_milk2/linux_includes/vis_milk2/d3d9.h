#ifndef __D3D9_H__ // Use one of the common guards for d3d9.h
#define __D3D9_H__

// This is a fake d3d9.h for Linux builds.
// Its main purpose is to satisfy the #include <d3d9.h> directive
// when the original dxcontext.h is (incorrectly) processed,
// and to prevent the compiler from searching for the real (missing) d3d9.h.

// Include the fake windows.h which includes the utility.h shim for basic types.
#include "windows.h" // Should resolve to our fake windows.h

// Provide minimal, incomplete type definitions for D3D types if absolutely necessary
// to prevent immediate errors in dxcontext.h if it uses them before they are
// properly shimmed or conditionalized away in a (still non-existent) dxcontext.h shim.
// The goal is to make the original dxcontext.h parse without finding the real d3d9.h.

// Forward declare common D3D9 interfaces as opaque pointers
typedef struct IDirect3D9 IDirect3D9;
typedef struct IDirect3DDevice9 IDirect3DDevice9;
typedef struct IDirect3DBaseTexture9 IDirect3DBaseTexture9;
typedef struct IDirect3DTexture9 IDirect3DTexture9;
typedef struct IDirect3DVolumeTexture9 IDirect3DVolumeTexture9;
typedef struct IDirect3DVertexBuffer9 IDirect3DVertexBuffer9;
typedef struct IDirect3DIndexBuffer9 IDirect3DIndexBuffer9;
typedef struct IDirect3DSurface9 IDirect3DSurface9;
typedef struct IDirect3DSwapChain9 IDirect3DSwapChain9;
typedef struct ID3DXFont ID3DXFont; // D3DX type, often used alongside D3D9
typedef struct ID3DXConstantTable ID3DXConstantTable; // D3DX type

// Define pointer types based on the forward declarations
typedef IDirect3D9* LPDIRECT3D9;
typedef IDirect3DDevice9* LPDIRECT3DDEVICE9;
typedef IDirect3DBaseTexture9* LPDIRECT3DBASETEXTURE9;
typedef IDirect3DTexture9* LPDIRECT3DTEXTURE9;
typedef IDirect3DVolumeTexture9* LPDIRECT3DVOLUMETEXTURE9;
typedef IDirect3DVertexBuffer9* LPDIRECT3DVERTEXBUFFER9;
typedef IDirect3DIndexBuffer9* LPDIRECT3DINDEXBUFFER9;
typedef IDirect3DSurface9* LPDIRECT3DSURFACE9;
typedef IDirect3DSwapChain9* LPDIRECT3DSWAPCHAIN9;
typedef ID3DXFont* LPD3DXFONT;
typedef ID3DXConstantTable* LPD3DXCONSTANTTABLE;


// Minimal D3DPRESENT_PARAMETERS if original dxcontext.h uses it directly.
// This is very simplified.
typedef struct _D3DPRESENT_PARAMETERS_
{
    UINT BackBufferWidth;
    UINT BackBufferHeight;
    UINT BackBufferFormat; // Will be just an int, not the D3DFORMAT enum
    UINT BackBufferCount;
    UINT MultiSampleType;
    UINT MultiSampleQuality;
    UINT SwapEffect;
    HWND hDeviceWindow;
    BOOL Windowed;
    BOOL EnableAutoDepthStencil;
    UINT AutoDepthStencilFormat;
    DWORD Flags;
    UINT FullScreen_RefreshRateInHz;
    UINT PresentationInterval;
} D3DPRESENT_PARAMETERS;

// Minimal D3DCAPS9
typedef struct _D3DCAPS9
{
    UINT    DeviceType;
    UINT    AdapterOrdinal;
    DWORD   Caps;
    // ... many other fields omitted ...
    DWORD   MaxTextureWidth, MaxTextureHeight;
    DWORD   PixelShaderVersion;
} D3DCAPS9;

// Minimal D3DFORMAT enum (just enough to allow parsing)
typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN              =  0,
    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D16                  = 80,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_A2R10G10B10          = 31,
    D3DFMT_A16B16G16R16F        = 113,
    D3DFMT_A32B32G32R32F        = 116,
} D3DFORMAT;

// Minimal D3DVIEWPORT9
typedef struct _D3DVIEWPORT9 {
    DWORD       X;
    DWORD       Y;
    DWORD       Width;
    DWORD       Height;
    float       MinZ;
    float       MaxZ;
} D3DVIEWPORT9;


// Define some common HRESULT values if they are checked
#ifndef D3D_OK
#define D3D_OK ((long)0)
#endif
#ifndef E_FAIL
#define E_FAIL ((long)0x80004005L)
#endif
#ifndef D3DERR_OUTOFVIDEOMEMORY
#define D3DERR_OUTOFVIDEOMEMORY         MAKE_D3DHRESULT(2152)
#endif
#ifndef D3DERR_NOTAVAILABLE
#define D3DERR_NOTAVAILABLE             MAKE_D3DHRESULT(2154)
#endif
#ifndef D3DERR_INVALIDCALL
#define D3DERR_INVALIDCALL              MAKE_D3DHRESULT(2156)
#endif
#ifndef D3DERR_DEVICELOST
#define D3DERR_DEVICELOST               MAKE_D3DHRESULT(2152) // Re-using a value
#endif
#ifndef D3DERR_DEVICENOTRESET
#define D3DERR_DEVICENOTRESET           MAKE_D3DHRESULT(2153) // Re-using a value
#endif

// Minimal MAKE_D3DHRESULT macro (if not pulled from utility.h shim's windows.h fake)
#ifndef MAKE_D3DHRESULT
#define MAKE_D3DHRESULT(code) ((long)(0x88760000L | (code)))
#endif


// Other D3D constants that might be used in dxcontext.h or files including it
#define D3DCREATE_HARDWARE_VERTEXPROCESSING 0
#define D3DDEVTYPE_HAL 0
#define D3DSWAPEFFECT_DISCARD 0
#define D3DMULTISAMPLE_NONE 0
#define D3DADAPTER_DEFAULT 0
#define D3DPOOL_DEFAULT 0
#define D3DUSAGE_RENDERTARGET 0
#define D3DUSAGE_DYNAMIC 0
#define D3DLOCK_DISCARD 0

// D3DX types if used by dxcontext.h (less likely but possible)
// These are just opaque pointers.
struct ID3DXMesh;
typedef ID3DXMesh* LPD3DXMESH;


#endif // __D3D9_H__
