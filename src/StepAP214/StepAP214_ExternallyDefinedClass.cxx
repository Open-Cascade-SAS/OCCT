// File:	StepAP214_ExternallyDefinedClass.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepAP214_ExternallyDefinedClass.ixx>

//=======================================================================
//function : StepAP214_ExternallyDefinedClass
//purpose  : 
//=======================================================================

StepAP214_ExternallyDefinedClass::StepAP214_ExternallyDefinedClass ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP214_ExternallyDefinedClass::Init (const Handle(TCollection_HAsciiString) &aGroup_Name,
                                             const Standard_Boolean hasGroup_Description,
                                             const Handle(TCollection_HAsciiString) &aGroup_Description,
                                             const StepBasic_SourceItem &aExternallyDefinedItem_ItemId,
                                             const Handle(StepBasic_ExternalSource) &aExternallyDefinedItem_Source)
{
  StepAP214_Class::Init(aGroup_Name,
                        hasGroup_Description,
                        aGroup_Description);
  theExternallyDefinedItem->Init(aExternallyDefinedItem_ItemId,
                                 aExternallyDefinedItem_Source);
}

//=======================================================================
//function : ExternallyDefinedItem
//purpose  : 
//=======================================================================

Handle(StepBasic_ExternallyDefinedItem) StepAP214_ExternallyDefinedClass::ExternallyDefinedItem () const
{
  return theExternallyDefinedItem;
}

//=======================================================================
//function : SetExternallyDefinedItem
//purpose  : 
//=======================================================================

void StepAP214_ExternallyDefinedClass::SetExternallyDefinedItem (const Handle(StepBasic_ExternallyDefinedItem) &aExternallyDefinedItem)
{
  theExternallyDefinedItem = aExternallyDefinedItem;
}
