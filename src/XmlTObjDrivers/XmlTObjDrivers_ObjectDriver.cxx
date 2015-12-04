// Created on: 2004-11-24
// Created by: Edward AGAPOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A


#include <CDM_MessageDriver.hxx>
#include "XmlTObjDrivers_ObjectDriver.hxx"
#include <TObj_TObject.hxx>
#include <TObj_Persistence.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt.hxx>




IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_ObjectDriver,XmlMDF_ADriver)

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

