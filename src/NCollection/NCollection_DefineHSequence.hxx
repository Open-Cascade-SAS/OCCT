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

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineSequence.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Sequence class managed by Handle

#define DEFINE_HSEQUENCE(HClassName, _SequenceType_)                           \
\
class HClassName : public _SequenceType_,                                      \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                       HClassName ();                                 \
   inline                       HClassName (const _SequenceType_&);            \
   inline const _SequenceType_& Sequence        () const;                      \
   inline _SequenceType_&       ChangeSequence  ();                            \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName () :                                             \
       _SequenceType_(),                                                       \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _SequenceType_& anOther) :                \
       _SequenceType_(anOther),                                                \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _SequenceType_& HClassName::Sequence () const                     \
{ return * (const _SequenceType_ *) this; }                                    \
                                                                               \
inline _SequenceType_& HClassName::ChangeSequence ()                           \
{ return * (_SequenceType_ *) this; }                                          \

#define IMPLEMENT_HSEQUENCE(HClassName)                                        \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
