// File:	StepElement_UniformSurfaceSection.cxx
// Created:	Thu Dec 12 17:29:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_UniformSurfaceSection.ixx>

//=======================================================================
//function : StepElement_UniformSurfaceSection
//purpose  : 
//=======================================================================

StepElement_UniformSurfaceSection::StepElement_UniformSurfaceSection ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_UniformSurfaceSection::Init (const StepElement_MeasureOrUnspecifiedValue &aSurfaceSection_Offset,
                                              const StepElement_MeasureOrUnspecifiedValue &aSurfaceSection_NonStructuralMass,
                                              const StepElement_MeasureOrUnspecifiedValue &aSurfaceSection_NonStructuralMassOffset,
                                              const Standard_Real aThickness,
                                              const StepElement_MeasureOrUnspecifiedValue &aBendingThickness,
                                              const StepElement_MeasureOrUnspecifiedValue &aShearThickness)
{
  StepElement_SurfaceSection::Init(aSurfaceSection_Offset,
                                   aSurfaceSection_NonStructuralMass,
                                   aSurfaceSection_NonStructuralMassOffset);

  theThickness = aThickness;

  theBendingThickness = aBendingThickness;

  theShearThickness = aShearThickness;
}

//=======================================================================
//function : Thickness
//purpose  : 
//=======================================================================

Standard_Real StepElement_UniformSurfaceSection::Thickness () const
{
  return theThickness;
}

//=======================================================================
//function : SetThickness
//purpose  : 
//=======================================================================

void StepElement_UniformSurfaceSection::SetThickness (const Standard_Real aThickness)
{
  theThickness = aThickness;
}

//=======================================================================
//function : BendingThickness
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_UniformSurfaceSection::BendingThickness () const
{
  return theBendingThickness;
}

//=======================================================================
//function : SetBendingThickness
//purpose  : 
//=======================================================================

void StepElement_UniformSurfaceSection::SetBendingThickness (const StepElement_MeasureOrUnspecifiedValue &aBendingThickness)
{
  theBendingThickness = aBendingThickness;
}

//=======================================================================
//function : ShearThickness
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_UniformSurfaceSection::ShearThickness () const
{
  return theShearThickness;
}

//=======================================================================
//function : SetShearThickness
//purpose  : 
//=======================================================================

void StepElement_UniformSurfaceSection::SetShearThickness (const StepElement_MeasureOrUnspecifiedValue &aShearThickness)
{
  theShearThickness = aShearThickness;
}
