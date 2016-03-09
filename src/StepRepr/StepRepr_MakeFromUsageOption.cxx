// Created on: 2000-07-03
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
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepRepr_MakeFromUsageOption.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_MakeFromUsageOption,StepRepr_ProductDefinitionUsage)

//=======================================================================
//function : StepRepr_MakeFromUsageOption
//purpose  : 
//=======================================================================
StepRepr_MakeFromUsageOption::StepRepr_MakeFromUsageOption ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_MakeFromUsageOption::Init (const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Id,
                                         const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Name,
                                         const Standard_Boolean hasProductDefinitionRelationship_Description,
                                         const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Description,
                                         const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatingProductDefinition,
                                         const Handle(StepBasic_ProductDefinition) &aProductDefinitionRelationship_RelatedProductDefinition,
                                         const Standard_Integer aRanking,
                                         const Handle(TCollection_HAsciiString) &aRankingRationale,
                                         const Handle(StepBasic_MeasureWithUnit) &aQuantity)
{
  StepRepr_ProductDefinitionUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition);

  theRanking = aRanking;

  theRankingRationale = aRankingRationale;

  theQuantity = aQuantity;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_MakeFromUsageOption::Init (const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Id,
                                         const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Name,
                                         const Standard_Boolean hasProductDefinitionRelationship_Description,
                                         const Handle(TCollection_HAsciiString) &aProductDefinitionRelationship_Description,
                                         const StepBasic_ProductDefinitionOrReference &aProductDefinitionRelationship_RelatingProductDefinition,
                                         const StepBasic_ProductDefinitionOrReference &aProductDefinitionRelationship_RelatedProductDefinition,
                                         const Standard_Integer aRanking,
                                         const Handle(TCollection_HAsciiString) &aRankingRationale,
                                         const Handle(StepBasic_MeasureWithUnit) &aQuantity)
{
  StepRepr_ProductDefinitionUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition);

  theRanking = aRanking;

  theRankingRationale = aRankingRationale;

  theQuantity = aQuantity;
}

//=======================================================================
//function : Ranking
//purpose  : 
//=======================================================================

Standard_Integer StepRepr_MakeFromUsageOption::Ranking () const
{
  return theRanking;
}

//=======================================================================
//function : SetRanking
//purpose  : 
//=======================================================================

void StepRepr_MakeFromUsageOption::SetRanking (const Standard_Integer aRanking)
{
  theRanking = aRanking;
}

//=======================================================================
//function : RankingRationale
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_MakeFromUsageOption::RankingRationale () const
{
  return theRankingRationale;
}

//=======================================================================
//function : SetRankingRationale
//purpose  : 
//=======================================================================

void StepRepr_MakeFromUsageOption::SetRankingRationale (const Handle(TCollection_HAsciiString) &aRankingRationale)
{
  theRankingRationale = aRankingRationale;
}

//=======================================================================
//function : Quantity
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureWithUnit) StepRepr_MakeFromUsageOption::Quantity () const
{
  return theQuantity;
}

//=======================================================================
//function : SetQuantity
//purpose  : 
//=======================================================================

void StepRepr_MakeFromUsageOption::SetQuantity (const Handle(StepBasic_MeasureWithUnit) &aQuantity)
{
  theQuantity = aQuantity;
}
