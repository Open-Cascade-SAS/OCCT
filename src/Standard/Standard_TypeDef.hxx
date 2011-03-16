// File:      Standard_TypeDef.hxx
// Copyright: Open Cascade 2006

#ifndef _Standard_TypeDef_HeaderFile
#define _Standard_TypeDef_HeaderFile

#include <stddef.h>

#define Standard_False (Standard_Boolean)0
#define Standard_True  (Standard_Boolean)1

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

typedef int Standard_Integer;
typedef double Standard_Real;
typedef unsigned int Standard_Boolean;
typedef float Standard_ShortReal;
typedef char  Standard_Character;
typedef short Standard_ExtCharacter;
typedef unsigned char  Standard_Byte;
typedef void* Standard_Address;
typedef size_t Standard_Size;
//
typedef const char*  Standard_CString;
typedef const short* Standard_ExtString;

// declare operator new in global scope for old sun compiler
#ifndef WORKAROUND_SUNPRO_NEW_PLACEMENT
#define WORKAROUND_SUNPRO_NEW_PLACEMENT
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x420)
inline void* operator new(size_t,void* anAddress) 
{
  return anAddress;
}
#endif
#endif

#endif

