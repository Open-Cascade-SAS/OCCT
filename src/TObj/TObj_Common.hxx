// File:      TObj_Common.hxx
// Created:   Mon Nov 22 13:00:11 2004
// Author:    Pavel TELKOV
// Copyright:   Open CASCADE  2007
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
