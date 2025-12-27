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
//      Uses variadic macro to support template types with commas (e.g., NCollection_Array1<A, B>)

#define DEFINE_HARRAY1(HClassName, ...)                                                            \
  class HClassName : public __VA_ARGS__, public Standard_Transient                                 \
  {                                                                                                \
    using Array1Type_ = __VA_ARGS__;                                                               \
                                                                                                   \
  public:                                                                                          \
    DEFINE_STANDARD_ALLOC                                                                          \
    DEFINE_NCOLLECTION_ALLOC                                                                       \
    HClassName()                                                                                   \
        : Array1Type_()                                                                            \
    {                                                                                              \
    }                                                                                              \
    HClassName(const int theLower, const int theUpper)                                             \
        : Array1Type_(theLower, theUpper)                                                          \
    {                                                                                              \
    }                                                                                              \
    HClassName(const int                               theLower,                                   \
               const int                               theUpper,                                   \
               const typename Array1Type_::value_type& theValue)                                   \
        : Array1Type_(theLower, theUpper)                                                          \
    {                                                                                              \
      Init(theValue);                                                                              \
    }                                                                                              \
    explicit HClassName(const typename Array1Type_::value_type& theBegin,                          \
                        const int                               theLower,                          \
                        const int                               theUpper,                          \
                        const bool)                                                                \
        : Array1Type_(theBegin, theLower, theUpper)                                                \
    {                                                                                              \
    }                                                                                              \
    HClassName(const Array1Type_& theOther)                                                        \
        : Array1Type_(theOther)                                                                    \
    {                                                                                              \
    }                                                                                              \
    const Array1Type_& Array1() const noexcept                                                     \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    Array1Type_& ChangeArray1() noexcept                                                           \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    DEFINE_STANDARD_RTTI_INLINE(HClassName, Standard_Transient)                                    \
  };                                                                                               \
  DEFINE_STANDARD_HANDLE(HClassName, Standard_Transient)

#define IMPLEMENT_HARRAY1(HClassName)

#endif
