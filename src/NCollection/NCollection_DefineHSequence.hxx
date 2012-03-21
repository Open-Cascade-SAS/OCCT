// Created on: 2001-01-29
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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
