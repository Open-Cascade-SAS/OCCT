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

//            Automatically created from NCollection_HArray2.hxx by GAWK

#ifndef NCollection_DefineHArray2_HeaderFile
#define NCollection_DefineHArray2_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <NCollection_DefineArray2.hxx>
#include <MMgt_TShared.hxx>

//      Declaration of Array2 class managed by Handle

#define DEFINE_HARRAY2(HClassName, _Array2Type_)                               \
\
class HClassName : public _Array2Type_,                                        \
                   public MMgt_TShared {                                       \
 public:                                                                       \
   inline                     HClassName  (const Standard_Integer theRowLower, \
                                           const Standard_Integer theRowUpper, \
                                           const Standard_Integer theColLower, \
                                           const Standard_Integer theColUpper);\
   inline                     HClassName  (const _Array2Type_&);               \
   inline const _Array2Type_& Array2      () const;                            \
   inline _Array2Type_&       ChangeArray2();                                  \
   DEFINE_STANDARD_RTTI (HClassName)                                           \
};                                                                             \
                                                                               \
DEFINE_STANDARD_HANDLE (HClassName, MMgt_TShared)                              \
                                                                               \
inline HClassName::HClassName (const Standard_Integer theRowLow,               \
                               const Standard_Integer theRowUpp,               \
                               const Standard_Integer theColLow,               \
                               const Standard_Integer theColUpp) :             \
       _Array2Type_ (theRowLow, theRowUpp, theColLow, theColUpp),              \
       MMgt_TShared() {}                                                       \
                                                                               \
inline HClassName::HClassName (const _Array2Type_& theOther) :                 \
       _Array2Type_(theOther),                                                 \
       MMgt_TShared() {}                                                       \
                                                                               \
inline const _Array2Type_& HClassName::Array2 () const                         \
{ return * (const _Array2Type_ *) this; }                                      \
                                                                               \
inline _Array2Type_& HClassName::ChangeArray2 ()                               \
{ return * (_Array2Type_ *) this; }                                            \

#define IMPLEMENT_HARRAY2(HClassName)                                          \
IMPLEMENT_STANDARD_HANDLE  (HClassName, MMgt_TShared)                          \
IMPLEMENT_STANDARD_RTTIEXT (HClassName, MMgt_TShared)

#endif
