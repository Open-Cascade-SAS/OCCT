#ifndef _FSD_CArchive_HeaderFile
#define _FSD_CArchive_HeaderFile

#ifdef WNT

//#define _MBCS

#include <afx.h>

typedef CArchive* FSD_CArchive;
#else
typedef void* FSD_CArchive;
#endif

#endif
