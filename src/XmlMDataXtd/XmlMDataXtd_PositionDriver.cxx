// Created on: 2001-09-04
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

#include <XmlMDataXtd_PositionDriver.hxx>

#include <CDM_MessageDriver.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Position.hxx>
#include <TDF_Attribute.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

#include <stdio.h>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataXtd_PositionDriver,XmlMDF_ADriver)

//=======================================================================
//function : XmlMDataXtd_PositionDriver
//purpose  : Constructor
//=======================================================================
XmlMDataXtd_PositionDriver::XmlMDataXtd_PositionDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataXtd_PositionDriver::NewEmpty() const
{
  return (new TDataXtd_Position());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataXtd_PositionDriver::Paste
                (const XmlObjMgt_Persistent&  theSource,
                 const Handle(TDF_Attribute)& theTarget,
                 XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TDataXtd_Position) aTPos = Handle(TDataXtd_Position)::DownCast(theTarget);

  // position
  XmlObjMgt_DOMString aPosStr = XmlObjMgt::GetStringValue(theSource.Element());
  if (aPosStr == NULL)
  {
    WriteMessage ("Cannot retrieve position string from element");
    return Standard_False;
  }

  gp_Pnt aPos;
  Standard_Real aValue;
  Standard_CString aValueStr = Standard_CString(aPosStr.GetString());

  // X
  if (!XmlObjMgt::GetReal(aValueStr, aValue))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString
        ("Cannot retrieve X coordinate for TDataXtd_Position attribute as \"")
          + aValueStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aPos.SetX(aValue);

  // Y
  if (!XmlObjMgt::GetReal(aValueStr, aValue))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString
        ("Cannot retrieve Y coordinate for TDataXtd_Position attribute as \"")
          + aValueStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aPos.SetY(aValue);

  // Z
  if (!XmlObjMgt::GetReal(aValueStr, aValue))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString
        ("Cannot retrieve Z coordinate for TDataXtd_Position attribute as \"")
          + aValueStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aPos.SetZ(aValue);

  aTPos->SetPosition(aPos);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataXtd_PositionDriver::Paste
                (const Handle(TDF_Attribute)& theSource,
                 XmlObjMgt_Persistent&        theTarget,
                 XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataXtd_Position) aTPos = Handle(TDataXtd_Position)::DownCast(theSource);
  if (!aTPos.IsNull())
  {
    gp_Pnt aPos = aTPos->GetPosition();
    char buf [75]; // (24 + 1) * 3
    Sprintf (buf, "%.17g %.17g %.17g", aPos.X(), aPos.Y(), aPos.Z());
    XmlObjMgt::SetStringValue(theTarget.Element(), buf);
  }
}
