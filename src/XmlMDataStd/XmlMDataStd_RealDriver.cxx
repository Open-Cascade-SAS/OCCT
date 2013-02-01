// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

//AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

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
  Sprintf(aValueChar, "%.15g", anInt->Get());
  TCollection_AsciiString aValueStr(aValueChar);
#endif
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString(), Standard_True);
}
