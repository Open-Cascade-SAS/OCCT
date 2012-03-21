// Created on: 2002-04-29
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
