// Created on: 2007-08-21
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <XmlMDataStd_AsciiStringDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
#include <TDataStd_AsciiString.hxx>

//=======================================================================
//function : XmlMDataStd_AsciiStringDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_AsciiStringDriver::XmlMDataStd_AsciiStringDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty()
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_AsciiStringDriver::NewEmpty () const
{
  return (new TDataStd_AsciiString());
}

//=======================================================================
//function : Paste()
//purpose  : retrieve
//=======================================================================
Standard_Boolean XmlMDataStd_AsciiStringDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable&  ) const
{
  if(!theTarget.IsNull()) {
    const TCollection_AsciiString aString = XmlObjMgt::GetStringValue (theSource);
    Handle(TDataStd_AsciiString)::DownCast(theTarget) -> Set (aString);
    return Standard_True;
  }
  WriteMessage("error retrieving AsciiString for type TDataStd_AsciiString");
  return Standard_False;
}

//=======================================================================
//function : Paste()
//purpose  : store
//=======================================================================
void XmlMDataStd_AsciiStringDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_AsciiString) aS = Handle(TDataStd_AsciiString)::DownCast(theSource);
  if (aS.IsNull()) return;
  XmlObjMgt_DOMString aString = aS->Get().ToCString();
  XmlObjMgt::SetStringValue (theTarget, aString);
}
