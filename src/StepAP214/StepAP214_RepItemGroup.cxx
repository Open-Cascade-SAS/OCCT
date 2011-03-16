// File:	StepAP214_RepItemGroup.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepAP214_RepItemGroup.ixx>

//=======================================================================
//function : StepAP214_RepItemGroup
//purpose  : 
//=======================================================================

StepAP214_RepItemGroup::StepAP214_RepItemGroup ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepAP214_RepItemGroup::Init (const Handle(TCollection_HAsciiString) &aGroup_Name,
                                   const Standard_Boolean hasGroup_Description,
                                   const Handle(TCollection_HAsciiString) &aGroup_Description,
                                   const Handle(TCollection_HAsciiString) &aRepresentationItem_Name)
{
  StepBasic_Group::Init(aGroup_Name,
                        hasGroup_Description,
                        aGroup_Description);
  theRepresentationItem->Init(aRepresentationItem_Name);
}

//=======================================================================
//function : RepresentationItem
//purpose  : 
//=======================================================================

Handle(StepRepr_RepresentationItem) StepAP214_RepItemGroup::RepresentationItem () const
{
  return theRepresentationItem;
}

//=======================================================================
//function : SetRepresentationItem
//purpose  : 
//=======================================================================

void StepAP214_RepItemGroup::SetRepresentationItem (const Handle(StepRepr_RepresentationItem) &aRepresentationItem)
{
  theRepresentationItem = aRepresentationItem;
}
