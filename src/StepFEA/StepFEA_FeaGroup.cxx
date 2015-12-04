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
#include <StepFEA_FeaGroup.hxx>
#include <StepFEA_FeaModel.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_FeaGroup,StepBasic_Group)

//=======================================================================
//function : StepFEA_FeaGroup
//purpose  : 
//=======================================================================
StepFEA_FeaGroup::StepFEA_FeaGroup ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaGroup::Init (const Handle(TCollection_HAsciiString) &aGroup_Name,
                             const Handle(TCollection_HAsciiString) &aGroup_Description,
                             const Handle(StepFEA_FeaModel) &aModelRef)
{
  StepBasic_Group::Init(aGroup_Name,
                        Standard_True,
                        aGroup_Description);

  theModelRef = aModelRef;
}

//=======================================================================
//function : ModelRef
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel) StepFEA_FeaGroup::ModelRef () const
{
  return theModelRef;
}

//=======================================================================
//function : SetModelRef
//purpose  : 
//=======================================================================

void StepFEA_FeaGroup::SetModelRef (const Handle(StepFEA_FeaModel) &aModelRef)
{
  theModelRef = aModelRef;
}
