// File:	StepElement_SurfaceSection.cxx
// Created:	Thu Dec 12 17:29:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_SurfaceSection.ixx>

//=======================================================================
//function : StepElement_SurfaceSection
//purpose  : 
//=======================================================================

StepElement_SurfaceSection::StepElement_SurfaceSection ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_SurfaceSection::Init (const StepElement_MeasureOrUnspecifiedValue &aOffset,
                                       const StepElement_MeasureOrUnspecifiedValue &aNonStructuralMass,
                                       const StepElement_MeasureOrUnspecifiedValue &aNonStructuralMassOffset)
{

  theOffset = aOffset;

  theNonStructuralMass = aNonStructuralMass;

  theNonStructuralMassOffset = aNonStructuralMassOffset;
}

//=======================================================================
//function : Offset
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_SurfaceSection::Offset () const
{
  return theOffset;
}

//=======================================================================
//function : SetOffset
//purpose  : 
//=======================================================================

void StepElement_SurfaceSection::SetOffset (const StepElement_MeasureOrUnspecifiedValue &aOffset)
{
  theOffset = aOffset;
}

//=======================================================================
//function : NonStructuralMass
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_SurfaceSection::NonStructuralMass () const
{
  return theNonStructuralMass;
}

//=======================================================================
//function : SetNonStructuralMass
//purpose  : 
//=======================================================================

void StepElement_SurfaceSection::SetNonStructuralMass (const StepElement_MeasureOrUnspecifiedValue &aNonStructuralMass)
{
  theNonStructuralMass = aNonStructuralMass;
}

//=======================================================================
//function : NonStructuralMassOffset
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValue StepElement_SurfaceSection::NonStructuralMassOffset () const
{
  return theNonStructuralMassOffset;
}

//=======================================================================
//function : SetNonStructuralMassOffset
//purpose  : 
//=======================================================================

void StepElement_SurfaceSection::SetNonStructuralMassOffset (const StepElement_MeasureOrUnspecifiedValue &aNonStructuralMassOffset)
{
  theNonStructuralMassOffset = aNonStructuralMassOffset;
}
