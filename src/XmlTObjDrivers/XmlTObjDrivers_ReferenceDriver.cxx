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


#include "XmlTObjDrivers_ReferenceDriver.hxx"

#include <CDM_MessageDriver.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Attribute.hxx>

#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>

#include <TObj_TReference.hxx>
#include <TObj_Model.hxx>
#include <TObj_Object.hxx>
#include <TObj_Assistant.hxx>
#include <TDF_ChildIterator.hxx>


IMPLEMENT_STANDARD_HANDLE(XmlTObjDrivers_ReferenceDriver,XmlMDF_ADriver)
IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_ReferenceDriver,XmlMDF_ADriver)

IMPLEMENT_DOMSTRING (MasterEntry,        "master")
IMPLEMENT_DOMSTRING (ReferredEntry,      "entry")
IMPLEMENT_DOMSTRING (ReferredModelEntry, "modelentry")

//=======================================================================
//function : XmlTObjDrivers_ReferenceDriver
//purpose  : constructor
//=======================================================================

XmlTObjDrivers_ReferenceDriver::XmlTObjDrivers_ReferenceDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: XmlMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) XmlTObjDrivers_ReferenceDriver::NewEmpty() const
{
  return new TObj_TReference;
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//=======================================================================

Standard_Boolean XmlTObjDrivers_ReferenceDriver::Paste
                         (const XmlObjMgt_Persistent&  Source,
                          const Handle(TDF_Attribute)& Target,
                          XmlObjMgt_RRelocationTable&  /*RelocTable*/) const
{
  const XmlObjMgt_Element& anElement = Source;
  
  // get entries
  TCollection_AsciiString RefEntry    = anElement.getAttribute(::ReferredEntry());
  TCollection_AsciiString MasterEntry = anElement.getAttribute(::MasterEntry());
  // entry in model holder
  TCollection_AsciiString InHolderEntry =
    anElement.getAttribute(::ReferredModelEntry());

  // master label
  TDF_Label aLabel, aMasterLabel;
  TDF_Tool::Label (Target->Label().Data(), MasterEntry, aMasterLabel);
  // referred label
  if (InHolderEntry.IsEmpty())
    TDF_Tool::Label (Target->Label().Data(), RefEntry, aLabel, Standard_True);
  else
  {
    Handle(TObj_Model) aModel = Handle(TObj_Model)::DownCast
      ( TObj_Assistant::FindModel( InHolderEntry.ToCString() ));
    TDF_Tool::Label (aModel->GetLabel().Data(), RefEntry, aLabel, Standard_True);
  }
  Handle(TObj_TReference) aTarget =
    Handle(TObj_TReference)::DownCast (Target);
  aTarget->Set ( aLabel, aMasterLabel );

  return !aLabel.IsNull() && !aMasterLabel.IsNull();
}

//=======================================================================
//function : Paste
//purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           Store master and referred labels as entry, the other model referred
//           as entry in model-container
//=======================================================================

void XmlTObjDrivers_ReferenceDriver::Paste
                         (const Handle(TDF_Attribute)& Source,
                          XmlObjMgt_Persistent&        Target,
                          XmlObjMgt_SRelocationTable&  /*RelocTable*/) const
{
  Handle(TObj_TReference) aSource =
    Handle(TObj_TReference)::DownCast (Source);

  Handle(TObj_Object) aLObject = aSource->Get();
  if (aLObject.IsNull())
    return;

  // referred entry
  TCollection_AsciiString entry;
  TDF_Label aLabel = aLObject->GetLabel();
  TDF_Tool::Entry( aLabel, entry );
  Target.Element().setAttribute(::ReferredEntry(), entry.ToCString());

  // master entry
  entry.Clear();
  TDF_Label aMasterLabel = aSource->GetMasterLabel();
  TDF_Tool::Entry( aMasterLabel, entry );
  Target.Element().setAttribute(::MasterEntry(), entry.ToCString());

  // is reference to other document 
  if (aLabel.Root() == aMasterLabel.Root()) return;

  Handle(TObj_Model) aModel =
    Handle(TObj_Model)::DownCast( aLObject->GetModel() );
  TCollection_AsciiString aModelName( aModel->GetModelName()->String() );
  Target.Element().setAttribute(::ReferredModelEntry(), aModelName.ToCString());
}
