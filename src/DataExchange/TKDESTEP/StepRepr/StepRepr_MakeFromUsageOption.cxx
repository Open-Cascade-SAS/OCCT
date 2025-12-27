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

#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepRepr_MakeFromUsageOption.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_MakeFromUsageOption, StepRepr_ProductDefinitionUsage)

//=================================================================================================

StepRepr_MakeFromUsageOption::StepRepr_MakeFromUsageOption() {}

//=================================================================================================

void StepRepr_MakeFromUsageOption::Init(
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Id,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Name,
  const bool                  hasProductDefinitionRelationship_Description,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Description,
  const occ::handle<StepBasic_ProductDefinition>&
    aProductDefinitionRelationship_RelatingProductDefinition,
  const occ::handle<StepBasic_ProductDefinition>&
                                          aProductDefinitionRelationship_RelatedProductDefinition,
  const int                  aRanking,
  const occ::handle<TCollection_HAsciiString>& aRankingRationale,
  const occ::handle<Standard_Transient>&       aQuantity)
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

//=================================================================================================

void StepRepr_MakeFromUsageOption::Init(
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Id,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Name,
  const bool                  hasProductDefinitionRelationship_Description,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Description,
  const StepBasic_ProductDefinitionOrReference&
    aProductDefinitionRelationship_RelatingProductDefinition,
  const StepBasic_ProductDefinitionOrReference&
                                          aProductDefinitionRelationship_RelatedProductDefinition,
  const int                  aRanking,
  const occ::handle<TCollection_HAsciiString>& aRankingRationale,
  const occ::handle<Standard_Transient>&       aQuantity)
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

//=================================================================================================

int StepRepr_MakeFromUsageOption::Ranking() const
{
  return theRanking;
}

//=================================================================================================

void StepRepr_MakeFromUsageOption::SetRanking(const int aRanking)
{
  theRanking = aRanking;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepRepr_MakeFromUsageOption::RankingRationale() const
{
  return theRankingRationale;
}

//=================================================================================================

void StepRepr_MakeFromUsageOption::SetRankingRationale(
  const occ::handle<TCollection_HAsciiString>& aRankingRationale)
{
  theRankingRationale = aRankingRationale;
}

//=================================================================================================

occ::handle<Standard_Transient> StepRepr_MakeFromUsageOption::Quantity() const
{
  return theQuantity;
}

//=================================================================================================

void StepRepr_MakeFromUsageOption::SetQuantity(const occ::handle<Standard_Transient>& aQuantity)
{
  theQuantity = aQuantity;
}
