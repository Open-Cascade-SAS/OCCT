// File:	StepBasic_ExternallyDefinedItem.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_ExternallyDefinedItem.ixx>

//=======================================================================
//function : StepBasic_ExternallyDefinedItem
//purpose  : 
//=======================================================================

StepBasic_ExternallyDefinedItem::StepBasic_ExternallyDefinedItem ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ExternallyDefinedItem::Init (const StepBasic_SourceItem &aItemId,
                                            const Handle(StepBasic_ExternalSource) &aSource)
{

  theItemId = aItemId;

  theSource = aSource;
}

//=======================================================================
//function : ItemId
//purpose  : 
//=======================================================================

StepBasic_SourceItem StepBasic_ExternallyDefinedItem::ItemId () const
{
  return theItemId;
}

//=======================================================================
//function : SetItemId
//purpose  : 
//=======================================================================

void StepBasic_ExternallyDefinedItem::SetItemId (const StepBasic_SourceItem &aItemId)
{
  theItemId = aItemId;
}

//=======================================================================
//function : Source
//purpose  : 
//=======================================================================

Handle(StepBasic_ExternalSource) StepBasic_ExternallyDefinedItem::Source () const
{
  return theSource;
}

//=======================================================================
//function : SetSource
//purpose  : 
//=======================================================================

void StepBasic_ExternallyDefinedItem::SetSource (const Handle(StepBasic_ExternalSource) &aSource)
{
  theSource = aSource;
}
