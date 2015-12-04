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
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <StepElement_UniformSurfaceSection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_UniformSurfaceSection,StepElement_SurfaceSection)

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
