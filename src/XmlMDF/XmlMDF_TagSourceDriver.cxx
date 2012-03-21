// Created on: 2001-08-29
// Created by: Julia DOROVSKIKH
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

#include <XmlMDF_TagSourceDriver.ixx>
#include <XmlObjMgt.hxx>
#include <TDF_TagSource.hxx>

//=======================================================================
//function : XmlMDF_TagSourceDriver
//purpose  : Constructor
//=======================================================================

XmlMDF_TagSourceDriver::XmlMDF_TagSourceDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDF_TagSourceDriver::NewEmpty() const
{
  return (new TDF_TagSource());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDF_TagSourceDriver::Paste 
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aTag;
  XmlObjMgt_DOMString aTagStr = XmlObjMgt::GetStringValue(theSource.Element());

  if (aTagStr.GetInteger(aTag) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString ("Cannot retrieve TagSource attribute from \"")
        + aTagStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  if (aTag < 0) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString ("Invalid value of TagSource retrieved: ")
        + aTag;
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDF_TagSource) aT = Handle(TDF_TagSource)::DownCast (theTarget);
  aT->Set(aTag);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDF_TagSourceDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDF_TagSource) aTag = Handle(TDF_TagSource)::DownCast(theSource);
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue (theTarget.Element(), aTag->Get(), Standard_True);
}

