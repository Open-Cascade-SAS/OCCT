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


#include <XmlMXCAFDoc_VolumeDriver.ixx>
#include <XCAFDoc_Volume.hxx>
#include <XmlObjMgt.hxx>

//=======================================================================
//function : XmlMXCAFDoc_VolumeDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_VolumeDriver::XmlMXCAFDoc_VolumeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "Volume")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_VolumeDriver::NewEmpty() const
{
  return (new XCAFDoc_Volume());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_VolumeDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Real aValue;
  XmlObjMgt_DOMString aRealStr = XmlObjMgt::GetStringValue (theSource);

  if (XmlObjMgt::GetReal(aRealStr, aValue) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Volume attribute from \"")
        + aRealStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(XCAFDoc_Volume) anInt = Handle(XCAFDoc_Volume)::DownCast(theTarget);
  anInt->Set(aValue);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_VolumeDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_Volume) anInt = Handle(XCAFDoc_Volume)::DownCast(theSource);
  TCollection_AsciiString aValueStr (anInt->Get());
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString());
}
