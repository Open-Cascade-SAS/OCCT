// Created on: 2002-04-29
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef NCollection_DefineHSet_HeaderFile
#define NCollection_DefineHSet_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineSet.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Set class managed by Handle

#define DEFINE_HSET(HClassName, _SetType_)                                     \
\
class HClassName : public _SetType_,                                           \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                  HClassName ();                                      \
   inline                  HClassName (const _SetType_& anOther);              \
   inline const _SetType_& Set        () const;                                \
   inline _SetType_&       ChangeSet  ();                                      \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName () :                                             \
       _SetType_(),                                                            \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _SetType_& anOther) :                     \
       _SetType_(anOther),                                                     \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _SetType_& HClassName::Set () const                               \
{ return * (const _SetType_ *) this; }                                         \
                                                                               \
inline _SetType_& HClassName::ChangeSet ()                                     \
{ return * (_SetType_ *) this; }                                               \

#define IMPLEMENT_HSET(HClassName)                                             \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
