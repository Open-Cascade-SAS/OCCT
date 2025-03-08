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

//            Automatically created from NCollection_HArray1.hxx by GAWK

#ifndef NCollection_DefineHArray1_HeaderFile
#define NCollection_DefineHArray1_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

//      Declaration of Array1 class managed by Handle

#define DEFINE_HARRAY1(HClassName, _Array1Type_)                                                   \
  class HClassName : public _Array1Type_, public Standard_Transient                                \
  {                                                                                                \
  public:                                                                                          \
    DEFINE_STANDARD_ALLOC                                                                          \
    DEFINE_NCOLLECTION_ALLOC                                                                       \
    HClassName()                                                                                   \
        : _Array1Type_()                                                                           \
    {                                                                                              \
    }                                                                                              \
    HClassName(const Standard_Integer theLower, const Standard_Integer theUpper)                   \
        : _Array1Type_(theLower, theUpper)                                                         \
    {                                                                                              \
    }                                                                                              \
    HClassName(const Standard_Integer          theLower,                                           \
               const Standard_Integer          theUpper,                                           \
               const _Array1Type_::value_type& theValue)                                           \
        : _Array1Type_(theLower, theUpper)                                                         \
    {                                                                                              \
      Init(theValue);                                                                              \
    }                                                                                              \
    explicit HClassName(const typename _Array1Type_::value_type& theBegin,                         \
                        const Standard_Integer                   theLower,                         \
                        const Standard_Integer                   theUpper,                         \
                        const bool)                                                                \
        : _Array1Type_(theBegin, theLower, theUpper)                                               \
    {                                                                                              \
    }                                                                                              \
    HClassName(const _Array1Type_& theOther)                                                       \
        : _Array1Type_(theOther)                                                                   \
    {                                                                                              \
    }                                                                                              \
    const _Array1Type_& Array1() const                                                             \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    _Array1Type_& ChangeArray1()                                                                   \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    DEFINE_STANDARD_RTTI_INLINE(HClassName, Standard_Transient)                                    \
  };                                                                                               \
  DEFINE_STANDARD_HANDLE(HClassName, Standard_Transient)

#define IMPLEMENT_HARRAY1(HClassName)

#endif
