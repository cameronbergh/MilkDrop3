// This is a wrapper file to ensure ns-eel-int.h is included before asm-nseel-x86-gcc.c
// to resolve macro expansion issues in inline assembly.

#include "ns-eel-int.h"

// Now include the actual assembly file.
// The macros like EEL_F_SUFFIX, EEL_F_SSTR, NSEEL_LOOPFUNC_SUPPORT_MAXLEN_STR
// should be correctly defined via ns-eel-int.h (which includes ns-eel.h and ns-eel-addfuncs.h).
#include "asm-nseel-x86-gcc.c"
