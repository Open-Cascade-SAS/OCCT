// File:	StepGeom_SurfaceBoundary.cxx
// Created:	Fri Nov 26 16:26:28 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepGeom_SurfaceBoundary.ixx>

//=======================================================================
//function : StepGeom_SurfaceBoundary
//purpose  : 
//=======================================================================

StepGeom_SurfaceBoundary::StepGeom_SurfaceBoundary ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepGeom_SurfaceBoundary::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_BoundaryCurve))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_DegeneratePcurve))) return 2;
  return 0;
}

//=======================================================================
//function : BoundaryCurve
//purpose  : 
//=======================================================================

Handle(StepGeom_BoundaryCurve) StepGeom_SurfaceBoundary::BoundaryCurve () const
{
  return Handle(StepGeom_BoundaryCurve)::DownCast(Value());
}

//=======================================================================
//function : DegeneratePcurve
//purpose  : 
//=======================================================================

Handle(StepGeom_DegeneratePcurve) StepGeom_SurfaceBoundary::DegeneratePcurve () const
{
  return Handle(StepGeom_DegeneratePcurve)::DownCast(Value());
}
