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

#include <Standard_Type.hxx>
#include <StepFEA_FeaLinearElasticity.hxx>
#include <StepFEA_SymmetricTensor43d.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_FeaLinearElasticity,StepFEA_FeaMaterialPropertyRepresentationItem)

//=======================================================================
//function : StepFEA_FeaLinearElasticity
//purpose  : 
//=======================================================================
StepFEA_FeaLinearElasticity::StepFEA_FeaLinearElasticity ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaLinearElasticity::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                        const StepFEA_SymmetricTensor43d &aFeaConstants)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstants = aFeaConstants;
}

//=======================================================================
//function : FeaConstants
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor43d StepFEA_FeaLinearElasticity::FeaConstants () const
{
  return theFeaConstants;
}

//=======================================================================
//function : SetFeaConstants
//purpose  : 
//=======================================================================

void StepFEA_FeaLinearElasticity::SetFeaConstants (const StepFEA_SymmetricTensor43d &aFeaConstants)
{
  theFeaConstants = aFeaConstants;
}
