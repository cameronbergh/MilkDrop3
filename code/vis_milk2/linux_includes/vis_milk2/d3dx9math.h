#ifndef __D3DX9MATH_H__ // Use one of the common guards also defined in fake d3dx9.h
#define __D3DX9MATH_H__

// This is a fake d3dx9math.h for Linux builds.
// Its main purpose is to satisfy include directives when original headers are processed.

// It might need to include our fake d3d9.h if it uses D3D types,
// or just basic types from our fake windows.h (via utility.h).
#include "d3d9.h" // Should pick up our fake d3d9.h for D3DVECTOR etc.

#ifndef D3DXFLOAT16
typedef struct _D3DXFLOAT16 {
    unsigned short Value;
} D3DXFLOAT16, *LPD3DXFLOAT16;
#endif

// D3DXVECTOR3 definition based on D3DVECTOR
// D3DVECTOR should be defined in our fake d3d9.h or fake windows.h->utility.h
#ifndef D3DXVECTOR3_DEFINED
#define D3DXVECTOR3_DEFINED
#ifdef __cplusplus
typedef struct D3DXVECTOR3 : public _D3DVECTOR {
public:
    D3DXVECTOR3() {}
    D3DXVECTOR3( float fx, float fy, float fz ) { x = fx; y = fy; z = fz; }
} D3DXVECTOR3, *LPD3DXVECTOR3;
#else
typedef struct _D3DVECTOR D3DXVECTOR3, *LPD3DXVECTOR3;
#endif //__cplusplus
#endif // D3DXVECTOR3_DEFINED


// D3DXMATRIX definition (already in fake d3dx9.h, but good for standalone include)
#ifndef _D3DXMATRIX_DEFINED
#define _D3DXMATRIX_DEFINED
typedef struct _D3DXMATRIX {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
} D3DXMATRIX, *LPD3DXMATRIX;
#endif // _D3DXMATRIX_DEFINED

// D3DXQUATERNION
#ifndef D3DXQUATERNION_DEFINED
#define D3DXQUATERNION_DEFINED
typedef struct _D3DXQUATERNION {
    float x, y, z, w;
} D3DXQUATERNION, *LPD3DXQUATERNION;
#endif // D3DXQUATERNION_DEFINED

// D3DXPLANE
#ifndef D3DXPLANE_DEFINED
#define D3DXPLANE_DEFINED
typedef struct _D3DXPLANE {
    float a, b, c, d;
} D3DXPLANE, *LPD3DXPLANE;
#endif // D3DXPLANE_DEFINED

// D3DXCOLOR
#ifndef D3DXCOLOR_DEFINED
#define D3DXCOLOR_DEFINED
typedef struct _D3DCOLORVALUE {
    float r,g,b,a;
} D3DCOLORVALUE;
typedef struct _D3DXCOLOR : public D3DCOLORVALUE {
public:
    _D3DXCOLOR() {}
    _D3DXCOLOR( DWORD dw );
    _D3DXCOLOR( float r, float g, float b, float a );
} D3DXCOLOR, *LPD3DXCOLOR;
#endif // D3DXCOLOR_DEFINED


// Add function stubs if they are called from headers (unlikely but possible)
// For example:
// D3DXMATRIX* WINAPI D3DXMatrixIdentity(D3DXMATRIX *pOut);

#endif // __D3DX9MATH_H__
