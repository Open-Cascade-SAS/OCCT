// File:      XmlMPrsStd_PositionDriver.cxx
// Created:   04.09.01 14:47:31
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <XmlMPrsStd_PositionDriver.ixx>

#include <XmlObjMgt.hxx>
#include <TDataXtd_Position.hxx>
#include <gp_XYZ.hxx>

#include <stdio.h>

//=======================================================================
//function : XmlMPrsStd_PositionDriver
//purpose  : Constructor
//=======================================================================
XmlMPrsStd_PositionDriver::XmlMPrsStd_PositionDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMPrsStd_PositionDriver::NewEmpty() const
{
  return (new TDataXtd_Position());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMPrsStd_PositionDriver::Paste
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
void XmlMPrsStd_PositionDriver::Paste
                (const Handle(TDF_Attribute)& theSource,
                 XmlObjMgt_Persistent&        theTarget,
                 XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataXtd_Position) aTPos = Handle(TDataXtd_Position)::DownCast(theSource);
  if (!aTPos.IsNull())
  {
    gp_Pnt aPos = aTPos->GetPosition();
    char buf [64];
    sprintf (buf, "%.17g %.17g %.17g", aPos.X(), aPos.Y(), aPos.Z());
    XmlObjMgt::SetStringValue(theTarget.Element(), buf);
  }
}
