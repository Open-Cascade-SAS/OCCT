#ifndef _FSD_CFile_HeaderFile
#define _FSD_CFile_HeaderFile

#ifdef WNT

//#define _MBCS

#include <afx.h>

typedef CFile FSD_CFile;
#else
typedef void* FSD_CFile;
#endif

#endif
