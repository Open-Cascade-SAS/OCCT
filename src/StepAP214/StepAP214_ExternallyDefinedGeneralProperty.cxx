// File:	StepAP214_ExternallyDefinedGeneralProperty.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

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
