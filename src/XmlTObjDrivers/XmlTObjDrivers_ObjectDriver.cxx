// Created on: 2004-11-24
// Created by: Edward AGAPOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A


#include <CDM_MessageDriver.hxx>
#include "XmlTObjDrivers_ObjectDriver.hxx"
#include <TObj_TObject.hxx>
#include <TObj_Persistence.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt.hxx>

IMPLEMENT_STANDARD_HANDLE(XmlTObjDrivers_ObjectDriver,XmlMDF_ADriver)
IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_ObjectDriver,XmlMDF_ADriver);

//=======================================================================
//function : XmlTObjDrivers_ObjectDriver
//purpose  : constructor
//=======================================================================

XmlTObjDrivers_ObjectDriver::XmlTObjDrivers_ObjectDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: XmlMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) XmlTObjDrivers_ObjectDriver::NewEmpty() const
{
  return new TObj_TObject;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//=======================================================================

Standard_Boolean XmlTObjDrivers_ObjectDriver::Paste
                         (const XmlObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          XmlObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  TCollection_ExtendedString aString;
  if (XmlObjMgt::GetExtendedString (Source, aString))
  {
    TCollection_AsciiString anAscii (aString);
    Handle(TObj_Object) anObject = 
      TObj_Persistence::CreateNewObject(anAscii.ToCString(),Target->Label());
    Handle(TObj_TObject)::DownCast (Target) ->Set( anObject );
    return Standard_True;
  }
  WriteMessage("error retrieving ExtendedString for type TObj_TModel");
  return Standard_False;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           anObject is stored as a Name of class derived from TObj_Object
//=======================================================================

void XmlTObjDrivers_ObjectDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          XmlObjMgt_Persistent&        Target,
                          XmlObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TObj_TObject) aTObj =
    Handle(TObj_TObject)::DownCast( Source );
  Handle(TObj_Object) anIObject = aTObj->Get();
  
  XmlObjMgt::SetExtendedString (Target, anIObject->DynamicType()->Name());
}

