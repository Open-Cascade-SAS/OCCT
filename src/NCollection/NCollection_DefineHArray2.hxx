// Created on: 2002-04-29
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//            Automatically created from NCollection_HArray2.hxx by GAWK

#ifndef NCollection_DefineHArray2_HeaderFile
#define NCollection_DefineHArray2_HeaderFile

#include <Standard_Type.hxx>
#include <NCollection_DefineArray2.hxx>
#include <Standard_Transient.hxx>

//      Declaration of Array2 class managed by Handle

#define DEFINE_HARRAY2(HClassName, _Array2Type_)                               \
class HClassName : public _Array2Type_, public Standard_Transient {                  \
 public:                                                                       \
   DEFINE_STANDARD_ALLOC                                                       \
   DEFINE_NCOLLECTION_ALLOC                                                    \
   HClassName  (const Standard_Integer theRowLow,                              \
                const Standard_Integer theRowUpp,                              \
                const Standard_Integer theColLow,                              \
                const Standard_Integer theColUpp) :                            \
     _Array2Type_ (theRowLow, theRowUpp, theColLow, theColUpp) {}              \
   HClassName  (const Standard_Integer theRowLow,                              \
                const Standard_Integer theRowUpp,                              \
                const Standard_Integer theColLow,                              \
                const Standard_Integer theColUpp,                              \
                const _Array2Type_::value_type& theValue) :                    \
     _Array2Type_ (theRowLow, theRowUpp, theColLow, theColUpp)                 \
   { Init (theValue); }                                                        \
   HClassName  (const _Array2Type_& theOther) : _Array2Type_(theOther) {}      \
   const _Array2Type_& Array2 () const { return *this; }                       \
   _Array2Type_& ChangeArray2 ()       { return *this; }                       \
   DEFINE_STANDARD_RTTI_INLINE(HClassName,Standard_Transient)                              \
};                                                                             \
DEFINE_STANDARD_HANDLE (HClassName, Standard_Transient)

#define IMPLEMENT_HARRAY2(HClassName)                                          



#endif
