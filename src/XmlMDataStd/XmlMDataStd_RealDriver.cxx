// File:        XmlMDataStd_RealDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001-2002
// History:     AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

#define OCC6010 // vro 09.06.2004

#include <stdio.h>
#include <XmlMDataStd_RealDriver.ixx>
#include <TDataStd_Real.hxx>
#include <XmlObjMgt.hxx>

//=======================================================================
//function : XmlMDataStd_RealDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_RealDriver::XmlMDataStd_RealDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_RealDriver::NewEmpty() const
{
  return (new TDataStd_Real());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_RealDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Real aValue;
  XmlObjMgt_DOMString aRealStr= XmlObjMgt::GetStringValue (theSource);

  if (XmlObjMgt::GetReal(aRealStr, aValue) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Real attribute from \"")
        + aRealStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_Real) anInt = Handle(TDataStd_Real)::DownCast(theTarget);
  anInt->Set(aValue);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_RealDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Real) anInt = Handle(TDataStd_Real)::DownCast(theSource);
#ifndef OCC6010
  TCollection_AsciiString aValueStr (anInt->Get());
#else
  char aValueChar[32];
  sprintf(aValueChar, "%.15g", anInt->Get());
  TCollection_AsciiString aValueStr(aValueChar);
#endif
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
