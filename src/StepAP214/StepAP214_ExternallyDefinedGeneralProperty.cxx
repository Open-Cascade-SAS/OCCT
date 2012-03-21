// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepAP214_ExternallyDefinedGeneralProperty.ixx>

//=======================================================================
//function : StepAP214_ExternallyDefinedGeneralProperty
//purpose  : 
//=======================================================================

StepAP214_ExternallyDefinedGeneralProperty::StepAP214_ExternallyDefinedGeneralProperty ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP214_ExternallyDefinedGeneralProperty::Init (const Handle(TCollection_HAsciiString) &aGeneralProperty_Id,
                                                       const Handle(TCollection_HAsciiString) &aGeneralProperty_Name,
                                                       const Standard_Boolean hasGeneralProperty_Description,
                                                       const Handle(TCollection_HAsciiString) &aGeneralProperty_Description,
                                                       const StepBasic_SourceItem &aExternallyDefinedItem_ItemId,
                                                       const Handle(StepBasic_ExternalSource) &aExternallyDefinedItem_Source)
{
  StepBasic_GeneralProperty::Init(aGeneralProperty_Id,
                                  aGeneralProperty_Name,
                                  hasGeneralProperty_Description,
                                  aGeneralProperty_Description);
  theExternallyDefinedItem->Init(aExternallyDefinedItem_ItemId,
                                 aExternallyDefinedItem_Source);
}

//=======================================================================
//function : ExternallyDefinedItem
//purpose  : 
//=======================================================================

Handle(StepBasic_ExternallyDefinedItem) StepAP214_ExternallyDefinedGeneralProperty::ExternallyDefinedItem () const
{
  return theExternallyDefinedItem;
}

//=======================================================================
//function : SetExternallyDefinedItem
//purpose  : 
//=======================================================================

void StepAP214_ExternallyDefinedGeneralProperty::SetExternallyDefinedItem (const Handle(StepBasic_ExternallyDefinedItem) &aExternallyDefinedItem)
{
  theExternallyDefinedItem = aExternallyDefinedItem;
}
