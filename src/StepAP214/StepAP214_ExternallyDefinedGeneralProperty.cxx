// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepAP214_ExternallyDefinedGeneralProperty.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_SourceItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_ExternallyDefinedGeneralProperty,StepBasic_GeneralProperty)

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
