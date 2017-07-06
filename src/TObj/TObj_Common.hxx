// Created on: 2004-11-22
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

// Purpose:   This header file collects basic definitions 
//            to be included in most of sources

#ifndef TObj_Common_HeaderFile
#define TObj_Common_HeaderFile

//! Basic CASCADE definitions
#include <Standard_Type.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <Standard_Failure.hxx>

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
