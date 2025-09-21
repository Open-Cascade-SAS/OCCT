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

//      Declaration of Sequence class managed by Handle

#define DEFINE_HSEQUENCE(HClassName, _SequenceType_)                                               \
  class HClassName : public _SequenceType_, public Standard_Transient                              \
  {                                                                                                \
  public:                                                                                          \
    DEFINE_STANDARD_ALLOC                                                                          \
    DEFINE_NCOLLECTION_ALLOC                                                                       \
    HClassName() {}                                                                                \
    HClassName(const _SequenceType_& theOther)                                                     \
        : _SequenceType_(theOther)                                                                 \
    {                                                                                              \
    }                                                                                              \
    const _SequenceType_& Sequence() const                                                         \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    void Append(const _SequenceType_::value_type& theItem)                                         \
    {                                                                                              \
      _SequenceType_::Append(theItem);                                                             \
    }                                                                                              \
    void Append(_SequenceType_& theSequence)                                                       \
    {                                                                                              \
      _SequenceType_::Append(theSequence);                                                         \
    }                                                                                              \
    _SequenceType_& ChangeSequence()                                                               \
    {                                                                                              \
      return *this;                                                                                \
    }                                                                                              \
    template <class T>                                                                             \
    void Append(const Handle(T)& theOther,                                                         \
                typename opencascade::std::enable_if<                                              \
                  opencascade::std::is_base_of<HClassName, T>::value>::type* = 0)                  \
    {                                                                                              \
      _SequenceType_::Append(theOther->ChangeSequence());                                          \
    }                                                                                              \
    DEFINE_STANDARD_RTTI_INLINE(HClassName, Standard_Transient)                                    \
  };                                                                                               \
  DEFINE_STANDARD_HANDLE(HClassName, Standard_Transient)

#define IMPLEMENT_HSEQUENCE(HClassName)

#endif
