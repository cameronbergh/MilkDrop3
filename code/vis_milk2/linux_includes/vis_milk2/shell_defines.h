#ifndef __NULLSOFT_DX9_PLUGIN_SHELL_SHELL_DEFINES_H__
#define __NULLSOFT_DX9_PLUGIN_SHELL_SHELL_DEFINES_H__ 1

// Include the Linux shim of utility.h to bring in common type definitions
// that might have been implicitly relied upon from windows.h
#include "utility.h"

#define DEFAULT_FULLSCREEN_WIDTH  640
#define DEFAULT_FULLSCREEN_HEIGHT 480
#define MAX_ICON_TEXTURES 8
#define ICON_TEXTURE_SIZE 256
#define DEFAULT_WINDOW_SIZE 0.625f
#define BGR2RGB(x) (((x>>16)&0xFF) | (x & 0xFF00) | ((x<<16)&0xFF0000))

#define NUM_BASIC_FONTS 4
#define  SYS_FONT 0
#define  DEC_FONT 1
#define  HELP_FONT 2
#define  DESK_FONT 3
#define MAX_EXTRA_FONTS 5
typedef enum
{
    SIMPLE_FONT = 0,
    DECORATIVE_FONT = 1,
    HELPSCREEN_FONT = 2,
    PLAYLIST_FONT = 3,
    EXTRA_1 = 4,
    EXTRA_2 = 5,
    EXTRA_3 = 6,
    EXTRA_4 = 7,
    EXTRA_5 = 8
}
eFontIndex;

#endif // __NULLSOFT_DX9_PLUGIN_SHELL_SHELL_DEFINES_H__
