// File:	StepRepr_MakeFromUsageOption.cxx
// Created:	Mon Jul  3 20:13:36 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepRepr_MakeFromUsageOption.ixx>

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
