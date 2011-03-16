#ifndef _Select3D_Macro_HeaderFile
#define _Select3D_Macro_HeaderFile

#include <Standard_ShortReal.hxx>

// Safe conversion of Standard_ShortReal(float) to Standard_Real(double)
inline Standard_ShortReal DToF (Standard_Real a) 
{
  return a > ShortRealLast()  ? ShortRealLast()  :
         a < ShortRealFirst() ? ShortRealFirst() : (Standard_ShortReal)a;
}

#endif
