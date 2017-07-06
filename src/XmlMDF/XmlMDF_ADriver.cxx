// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
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


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDF_ADriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDF_ADriver,Standard_Transient)

//=======================================================================
//function : XmlMDF_ADriver
//purpose  : Constructor
//=======================================================================
XmlMDF_ADriver::XmlMDF_ADriver (const Handle(CDM_MessageDriver)& theMsgDriver,
                                const Standard_CString           theNS,
                                const Standard_CString           theName)
     : myMessageDriver (theMsgDriver)
{
  if (theNS != NULL)
    if (theNS[0] != '\0') {
      myTypeName = theNS;
      myTypeName += ':';
    }
  if (theName != NULL)
    myTypeName += theName;
}

//=======================================================================
//function : VersionNumber
//purpose  : default version number from which the driver is available
//=======================================================================

Standard_Integer XmlMDF_ADriver::VersionNumber () const
{
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) XmlMDF_ADriver::SourceType () const
{
  return NewEmpty() -> DynamicType();
}

//=======================================================================
//function : TypeName
//purpose  : 
//=======================================================================

const TCollection_AsciiString& XmlMDF_ADriver::TypeName () const
{
  const Standard_CString aString = myTypeName.ToCString();
  if (myTypeName.Length() == 0 || aString [myTypeName.Length() - 1] == ':')
    (TCollection_AsciiString&)myTypeName += SourceType() -> Name();
  return myTypeName;
}

//=======================================================================
//function : WriteMessage
//purpose  : 
//=======================================================================

void XmlMDF_ADriver::WriteMessage
                              (const TCollection_ExtendedString& aMessage) const
{
  myMessageDriver -> Write (aMessage.ToExtString());
}
