// File:	StepElement_SurfaceSectionFieldVarying.cxx
// Created:	Thu Dec 12 16:29:14 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_SurfaceSectionFieldVarying.ixx>

//=======================================================================
//function : StepElement_SurfaceSectionFieldVarying
//purpose  : 
//=======================================================================

StepElement_SurfaceSectionFieldVarying::StepElement_SurfaceSectionFieldVarying ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_SurfaceSectionFieldVarying::Init (const Handle(StepElement_HArray1OfSurfaceSection) &aDefinitions,
                                                   const Standard_Boolean aAdditionalNodeValues)
{
  //StepElement_SurfaceSectionField::Init();

  theDefinitions = aDefinitions;

  theAdditionalNodeValues = aAdditionalNodeValues;
}

//=======================================================================
//function : Definitions
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfSurfaceSection) StepElement_SurfaceSectionFieldVarying::Definitions () const
{
  return theDefinitions;
}

//=======================================================================
//function : SetDefinitions
//purpose  : 
//=======================================================================

void StepElement_SurfaceSectionFieldVarying::SetDefinitions (const Handle(StepElement_HArray1OfSurfaceSection) &aDefinitions)
{
  theDefinitions = aDefinitions;
}

//=======================================================================
//function : AdditionalNodeValues
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_SurfaceSectionFieldVarying::AdditionalNodeValues () const
{
  return theAdditionalNodeValues;
}

//=======================================================================
//function : SetAdditionalNodeValues
//purpose  : 
//=======================================================================

void StepElement_SurfaceSectionFieldVarying::SetAdditionalNodeValues (const Standard_Boolean aAdditionalNodeValues)
{
  theAdditionalNodeValues = aAdditionalNodeValues;
}
