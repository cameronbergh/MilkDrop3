// Attempt to block d3dx9math.h if it's included after this fake d3dx9.h
#define __D3DX9MATH_H__  // Common include guard for d3dx9math.h
#define _D3DX9MATH_H_    // Another possible guard
#define D3DX9MATH_H      // Yet another

#ifndef __D3DX9_H__ // Original guard for d3dx9.h itself
#define __D3DX9_H__

// This is a fake d3dx9.h for Linux builds.
// Its main purpose is to satisfy the #include <d3dx9.h> directive
// when the original dxcontext.h is (incorrectly) processed,
// and to prevent the compiler from searching for the real (missing) d3dx9.h.

// Include our fake d3d9.h as d3dx9.h depends on d3d9 types
#include "d3d9.h" // Should resolve to our fake d3d9.h

// D3DXMATRIX is a very common type from d3dx9math.h (which d3dx9.h includes)
// Providing a basic definition can prevent many errors if original headers use it.
#ifndef _D3DXMATRIX_DEFINED
#define _D3DXMATRIX_DEFINED
typedef struct _D3DXMATRIX {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
} D3DXMATRIX, *LPD3DXMATRIX;
#endif // _D3DXMATRIX_DEFINED

// Other commonly used D3DX types can be added as opaque forward declarations
// if errors indicate they are needed by files that include the original dxcontext.h
typedef struct ID3DXSprite ID3DXSprite;
typedef ID3DXSprite* LPD3DXSPRITE;

// LPD3DXCONSTANTTABLE and LPD3DXFONT are already forward-declared in the fake d3d9.h
// via their base IDirect... types, which might be sufficient if only pointers are used.
// If their specific struct definitions are accessed, more detailed fakes would be needed.
typedef struct ID3DXFont ID3DXFont; // Already in fake d3d9.h as typedef struct ID3DXFont ID3DXFont;
// typedef ID3DXFont* LPD3DXFONT; // Already in fake d3d9.h

typedef struct ID3DXConstantTable ID3DXConstantTable; // Already in fake d3d9.h
// typedef ID3DXConstantTable* LPD3DXCONSTANTTABLE; // Already in fake d3d9.h


// Placeholder for D3DXCompileShader if it's called directly by some code still active
// (though shader compilation is usually a more complex process)
#ifndef D3DXCompileShader
#define D3DXCompileShader(pSrcData, SrcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable) (E_FAIL)
#endif

#endif // __D3DX9_H__
