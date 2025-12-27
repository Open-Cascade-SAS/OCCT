// Created on: 2001-01-29
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

//            Automatically created from NCollection_HSequence.hxx by GAWK

#ifndef NCollection_DefineHSequence_HeaderFile
#define NCollection_DefineHSequence_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <type_traits>

//      Declaration of Sequence class managed by Handle
//      Uses variadic macro to support template types with commas (e.g., NCollection_Sequence<A, B>)

#define DEFINE_HSEQUENCE(HClassName, ...)                                                          \
  class HClassName : public __VA_ARGS__, public Standard_Transient                                 \
  {                                                                                                \
    using SequenceType_ = __VA_ARGS__;                                                             \
                                                                                                   \
  public:                                                                                          \
    DEFINE_STANDARD_ALLOC                                                                          \
    DEFINE_NCOLLECTION_ALLOC                                                                       \
    HClassName() {}                                                                                \
    HClassName(const SequenceType_& theOther)                                                      \
        : SequenceType_(theOther)                                                                  \
    {                                                                                              \
    }                                                                                              \
    const SequenceType_& Sequence() const noexcept                                                 \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    void Append(const typename SequenceType_::value_type& theItem)                                 \
    {                                                                                              \
      SequenceType_::Append(theItem);                                                              \
    }                                                                                              \
    void Append(SequenceType_& theSequence)                                                        \
    {                                                                                              \
      SequenceType_::Append(theSequence);                                                          \
    }                                                                                              \
    SequenceType_& ChangeSequence() noexcept                                                       \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    template <class T>                                                                             \
    void Append(const Handle(T)& theOther,                                                         \
                typename std::enable_if<std::is_base_of<HClassName, T>::value>::type* = 0)         \
    {                                                                                              \
      SequenceType_::Append(theOther->ChangeSequence());                                           \
    }                                                                                              \
    DEFINE_STANDARD_RTTI_INLINE(HClassName, Standard_Transient)                                    \
  };                                                                                               \
  DEFINE_STANDARD_HANDLE(HClassName, Standard_Transient)

#define IMPLEMENT_HSEQUENCE(HClassName)

#endif
