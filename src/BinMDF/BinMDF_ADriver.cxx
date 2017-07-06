// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDF_ADriver,Standard_Transient)

//=======================================================================
//function : BinMDF_ADriver
//purpose  : Constructor
//=======================================================================
BinMDF_ADriver::BinMDF_ADriver (const Handle(CDM_MessageDriver)& theMsgDriver,
                                const Standard_CString           theName)
     : myMessageDriver (theMsgDriver)
{
  if (theName)
    myTypeName = theName;
}
//=======================================================================
//function : WriteMessage
//purpose  :
//=======================================================================

void BinMDF_ADriver::WriteMessage
                              (const TCollection_ExtendedString& aMessage) const
{
  myMessageDriver -> Write (aMessage.ToExtString());
}
