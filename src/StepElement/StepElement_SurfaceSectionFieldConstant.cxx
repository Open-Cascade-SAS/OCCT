// File:	StepElement_SurfaceSectionFieldConstant.cxx
// Created:	Thu Dec 12 17:29:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_SurfaceSectionFieldConstant.ixx>

//=======================================================================
//function : StepElement_SurfaceSectionFieldConstant
//purpose  : 
//=======================================================================

StepElement_SurfaceSectionFieldConstant::StepElement_SurfaceSectionFieldConstant ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_SurfaceSectionFieldConstant::Init (const Handle(StepElement_SurfaceSection) &aDefinition)
{
//  StepElement_SurfaceSectionField::Init();

  theDefinition = aDefinition;
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

Handle(StepElement_SurfaceSection) StepElement_SurfaceSectionFieldConstant::Definition () const
{
  return theDefinition;
}

//=======================================================================
//function : SetDefinition
//purpose  : 
//=======================================================================

void StepElement_SurfaceSectionFieldConstant::SetDefinition (const Handle(StepElement_SurfaceSection) &aDefinition)
{
  theDefinition = aDefinition;
}
