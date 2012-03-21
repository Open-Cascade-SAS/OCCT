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


#include "XmlTObjDrivers_ModelDriver.hxx"

#include <CDM_MessageDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Attribute.hxx>

#include <TObj_TModel.hxx>
#include <TObj_Model.hxx>
#include <TObj_Assistant.hxx>

IMPLEMENT_STANDARD_HANDLE(XmlTObjDrivers_ModelDriver,XmlMDF_ADriver)
IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_ModelDriver,XmlMDF_ADriver);

//=======================================================================
//function : XmlTObjDrivers_ModelDriver
//purpose  : constructor
//=======================================================================

XmlTObjDrivers_ModelDriver::XmlTObjDrivers_ModelDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: XmlMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) XmlTObjDrivers_ModelDriver::NewEmpty() const
{
  return new TObj_TModel;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           Set CurrentModel of TObj_Assistant into Target TObj_TModel
//           if its GUID and GUID stored in Source are same
//=======================================================================

Standard_Boolean XmlTObjDrivers_ModelDriver::Paste
                         (const XmlObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          XmlObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  TCollection_ExtendedString aString;
  if (XmlObjMgt::GetExtendedString (Source, aString))
  {
    Standard_GUID aGUID (aString.ToExtString());
    Handle(TObj_Model) aCurrentModel = TObj_Assistant::GetCurrentModel();
    if (aGUID == aCurrentModel->GetGUID()) 
    {
      Handle(TObj_TModel) aTModel = Handle(TObj_TModel)::DownCast( Target );
      aCurrentModel->SetLabel ( aTModel->Label() );
      aTModel->Set( aCurrentModel );
      return Standard_True;
    }
    WriteMessage("TObj_TModel retrieval: wrong model GUID");
    return Standard_False;
  }
  WriteMessage("error retrieving ExtendedString for type TObj_TModel");
  return Standard_False;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           a Model is stored as its GUID
//=======================================================================

void XmlTObjDrivers_ModelDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          XmlObjMgt_Persistent&        Target,
                          XmlObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TObj_TModel) aTModel =
    Handle(TObj_TModel)::DownCast( Source );
  Handle(TObj_Model) aModel = aTModel->Model();

  // Store model GUID.
  Standard_PCharacter aPGuidString = new Standard_Character[256];
  aModel->GetGUID().ToCString( aPGuidString );
  XmlObjMgt::SetExtendedString (Target, *aPGuidString);
  delete []aPGuidString;
}
