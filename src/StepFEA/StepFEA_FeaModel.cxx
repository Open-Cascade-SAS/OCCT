// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_FeaModel.ixx>

//=======================================================================
//function : StepFEA_FeaModel
//purpose  : 
//=======================================================================

StepFEA_FeaModel::StepFEA_FeaModel ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaModel::Init (const Handle(TCollection_HAsciiString) &aRepresentation_Name,
                             const Handle(StepRepr_HArray1OfRepresentationItem) &aRepresentation_Items,
                             const Handle(StepRepr_RepresentationContext) &aRepresentation_ContextOfItems,
                             const Handle(TCollection_HAsciiString) &aCreatingSoftware,
                             const Handle(TColStd_HArray1OfAsciiString) &aIntendedAnalysisCode,
                             const Handle(TCollection_HAsciiString) &aDescription,
                             const Handle(TCollection_HAsciiString) &aAnalysisType)
{
  StepRepr_Representation::Init(aRepresentation_Name,
                                aRepresentation_Items,
                                aRepresentation_ContextOfItems);

  theCreatingSoftware = aCreatingSoftware;

  theIntendedAnalysisCode = aIntendedAnalysisCode;

  theDescription = aDescription;

  theAnalysisType = aAnalysisType;
}

//=======================================================================
//function : CreatingSoftware
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepFEA_FeaModel::CreatingSoftware () const
{
  return theCreatingSoftware;
}

//=======================================================================
//function : SetCreatingSoftware
//purpose  : 
//=======================================================================

void StepFEA_FeaModel::SetCreatingSoftware (const Handle(TCollection_HAsciiString) &aCreatingSoftware)
{
  theCreatingSoftware = aCreatingSoftware;
}

//=======================================================================
//function : IntendedAnalysisCode
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfAsciiString) StepFEA_FeaModel::IntendedAnalysisCode () const
{
  return theIntendedAnalysisCode;
}

//=======================================================================
//function : SetIntendedAnalysisCode
//purpose  : 
//=======================================================================

void StepFEA_FeaModel::SetIntendedAnalysisCode (const Handle(TColStd_HArray1OfAsciiString) &aIntendedAnalysisCode)
{
  theIntendedAnalysisCode = aIntendedAnalysisCode;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepFEA_FeaModel::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepFEA_FeaModel::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : AnalysisType
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepFEA_FeaModel::AnalysisType () const
{
  return theAnalysisType;
}

//=======================================================================
//function : SetAnalysisType
//purpose  : 
//=======================================================================

void StepFEA_FeaModel::SetAnalysisType (const Handle(TCollection_HAsciiString) &aAnalysisType)
{
  theAnalysisType = aAnalysisType;
}
