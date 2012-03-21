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

//            Automatically created from NCollection_HArray1.hxx by GAWK

#ifndef NCollection_DefineHArray1_HeaderFile
#define NCollection_DefineHArray1_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineArray1.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Array1 class managed by Handle

#define DEFINE_HARRAY1(HClassName, _Array1Type_)                               \
\
class HClassName : public _Array1Type_,                                        \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                     HClassName    (const Standard_Integer theLower,  \
                                             const Standard_Integer theUpper); \
   inline                     HClassName    (const _Array1Type_&);             \
   inline const _Array1Type_& Array1        () const;                          \
   inline _Array1Type_&       ChangeArray1  ();                                \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName (const Standard_Integer theLower,                \
                               const Standard_Integer theUpper) :              \
       _Array1Type_ (theLower,theUpper),                                       \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _Array1Type_& theOther) :                 \
       _Array1Type_(theOther),                                                 \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _Array1Type_& HClassName::Array1 () const                         \
{ return * (const _Array1Type_ *) this; }                                      \
                                                                               \
inline _Array1Type_& HClassName::ChangeArray1 ()                               \
{ return * (_Array1Type_ *) this; }                                            \

#define IMPLEMENT_HARRAY1(HClassName)                                          \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
