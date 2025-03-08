// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_FeaModel.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_FeaModel, StepRepr_Representation)

//=================================================================================================

StepFEA_FeaModel::StepFEA_FeaModel() {}

//=================================================================================================

void StepFEA_FeaModel::Init(
  const Handle(TCollection_HAsciiString)&             aRepresentation_Name,
  const Handle(StepRepr_HArray1OfRepresentationItem)& aRepresentation_Items,
  const Handle(StepRepr_RepresentationContext)&       aRepresentation_ContextOfItems,
  const Handle(TCollection_HAsciiString)&             aCreatingSoftware,
  const Handle(TColStd_HArray1OfAsciiString)&         aIntendedAnalysisCode,
  const Handle(TCollection_HAsciiString)&             aDescription,
  const Handle(TCollection_HAsciiString)&             aAnalysisType)
{
  StepRepr_Representation::Init(aRepresentation_Name,
                                aRepresentation_Items,
                                aRepresentation_ContextOfItems);

  theCreatingSoftware = aCreatingSoftware;

  theIntendedAnalysisCode = aIntendedAnalysisCode;

  theDescription = aDescription;

  theAnalysisType = aAnalysisType;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepFEA_FeaModel::CreatingSoftware() const
{
  return theCreatingSoftware;
}

//=================================================================================================

void StepFEA_FeaModel::SetCreatingSoftware(
  const Handle(TCollection_HAsciiString)& aCreatingSoftware)
{
  theCreatingSoftware = aCreatingSoftware;
}

//=================================================================================================

Handle(TColStd_HArray1OfAsciiString) StepFEA_FeaModel::IntendedAnalysisCode() const
{
  return theIntendedAnalysisCode;
}

//=================================================================================================

void StepFEA_FeaModel::SetIntendedAnalysisCode(
  const Handle(TColStd_HArray1OfAsciiString)& aIntendedAnalysisCode)
{
  theIntendedAnalysisCode = aIntendedAnalysisCode;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepFEA_FeaModel::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepFEA_FeaModel::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepFEA_FeaModel::AnalysisType() const
{
  return theAnalysisType;
}

//=================================================================================================

void StepFEA_FeaModel::SetAnalysisType(const Handle(TCollection_HAsciiString)& aAnalysisType)
{
  theAnalysisType = aAnalysisType;
}
