// Created on: 2004-11-22
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

// Purpose:   This header file collects basic definitions 
//            to be included in most of sources

#ifndef TObj_Common_HeaderFile
#define TObj_Common_HeaderFile

//! Basic CASCADE definitions
#include <Standard_DefineHandle.hxx>
#include <Standard_Type.hxx>
#include <MMgt_TShared.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <Standard_Failure.hxx>

//! The function IsEqual explicitly defined for Handle(Standard_Transient)
//! in order to avoid the problem with implicit using of IsEqual() defined 
//! for Standard_Address. The problem is that Handle(Standard_Transient)
//! have only operator of type casting to (non-const) Standard_Transient*,
//! hence this implicit way cannot be used for const Handle(Standard_Transient)
inline Standard_Boolean IsEqual(const Handle(Standard_Transient)& One,
                                const Handle(Standard_Transient)& Two)
{
  return One == Two;
}

//! Methods inline implimentation for HExtendedString

inline Standard_Integer HashCode (const Handle(TCollection_HExtendedString)& theStr,
                                  const Standard_Integer theBnd)
{
  return TCollection_ExtendedString::HashCode(theStr->String(), theBnd);
}

inline Standard_Boolean IsEqual (const Handle(TCollection_HExtendedString)& theStr1,
                                 const Handle(TCollection_HExtendedString)& theStr2)
{
  return TCollection_ExtendedString::IsEqual(theStr1->String(),theStr2->String());
}

#endif

#ifdef _MSC_VER
#pragma once
#endif
