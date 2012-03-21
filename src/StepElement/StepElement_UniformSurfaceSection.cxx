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
