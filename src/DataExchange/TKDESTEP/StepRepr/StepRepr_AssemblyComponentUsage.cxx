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

#include <StepBasic_ProductDefinition.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_AssemblyComponentUsage, StepRepr_ProductDefinitionUsage)

//=================================================================================================

StepRepr_AssemblyComponentUsage::StepRepr_AssemblyComponentUsage()
{
  defReferenceDesignator = false;
}

//=================================================================================================

void StepRepr_AssemblyComponentUsage::Init(
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Id,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Name,
  const bool                  hasProductDefinitionRelationship_Description,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Description,
  const occ::handle<StepBasic_ProductDefinition>&
    aProductDefinitionRelationship_RelatingProductDefinition,
  const occ::handle<StepBasic_ProductDefinition>&
                                          aProductDefinitionRelationship_RelatedProductDefinition,
  const bool                  hasReferenceDesignator,
  const occ::handle<TCollection_HAsciiString>& aReferenceDesignator)
{
  StepRepr_ProductDefinitionUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition);

  defReferenceDesignator = hasReferenceDesignator;
  if (defReferenceDesignator)
  {
    theReferenceDesignator = aReferenceDesignator;
  }
  else
    theReferenceDesignator.Nullify();
}

//=================================================================================================

void StepRepr_AssemblyComponentUsage::Init(
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Id,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Name,
  const bool                  hasProductDefinitionRelationship_Description,
  const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Description,
  const StepBasic_ProductDefinitionOrReference&
    aProductDefinitionRelationship_RelatingProductDefinition,
  const StepBasic_ProductDefinitionOrReference&
                                          aProductDefinitionRelationship_RelatedProductDefinition,
  const bool                  hasReferenceDesignator,
  const occ::handle<TCollection_HAsciiString>& aReferenceDesignator)
{
  StepRepr_ProductDefinitionUsage::Init(aProductDefinitionRelationship_Id,
                                        aProductDefinitionRelationship_Name,
                                        hasProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_Description,
                                        aProductDefinitionRelationship_RelatingProductDefinition,
                                        aProductDefinitionRelationship_RelatedProductDefinition);

  defReferenceDesignator = hasReferenceDesignator;
  if (defReferenceDesignator)
  {
    theReferenceDesignator = aReferenceDesignator;
  }
  else
    theReferenceDesignator.Nullify();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepRepr_AssemblyComponentUsage::ReferenceDesignator() const
{
  return theReferenceDesignator;
}

//=================================================================================================

void StepRepr_AssemblyComponentUsage::SetReferenceDesignator(
  const occ::handle<TCollection_HAsciiString>& aReferenceDesignator)
{
  theReferenceDesignator = aReferenceDesignator;
}

//=================================================================================================

bool StepRepr_AssemblyComponentUsage::HasReferenceDesignator() const
{
  return defReferenceDesignator;
}
