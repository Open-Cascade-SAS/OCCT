// File:	StepToGeom_MakeRectangularTrimmedSurface.cxx
// Created:	Thu Jan 25 12:01:33 1996
// Author:	Frederic MAUPAS

#include <StepToGeom_MakeRectangularTrimmedSurface.ixx>

#include <StepToGeom_MakeSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_Plane.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une RectangularTrimmedSurface de Geom a partir d' une
// RectangularTrimmedSurface de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeRectangularTrimmedSurface::Convert (const Handle(StepGeom_RectangularTrimmedSurface)& SS,
                                                                    Handle(Geom_RectangularTrimmedSurface)& CS)
{
  Handle(Geom_Surface) theBasis;
  if (StepToGeom_MakeSurface::Convert(SS->BasisSurface(),theBasis))
  {
    // -----------------------------------------
    // Modification of the Trimming Parameters ?
    // -----------------------------------------
  
    Standard_Real uFact = 1.;
    Standard_Real vFact = 1.;
    const Standard_Real LengthFact  = UnitsMethods::LengthFactor();
    const Standard_Real AngleFact   = UnitsMethods::PlaneAngleFactor(); // abv 30.06.00 trj4_k1_geo-tc-214.stp #1477: PI/180.;

    if (theBasis->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) ||
        theBasis->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
      uFact = vFact = AngleFact;
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
      uFact = AngleFact;
      vFact = LengthFact;
    }
    else if ( theBasis->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
      uFact = AngleFact;
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
      const Handle(Geom_ConicalSurface) conicS = Handle(Geom_ConicalSurface)::DownCast(theBasis);
      uFact = AngleFact;
      vFact = LengthFact / Cos(conicS->SemiAngle());
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_Plane))) {
      uFact = vFact = LengthFact;
    }

    const Standard_Real U1 = SS->U1() * uFact;
    const Standard_Real U2 = SS->U2() * uFact;
    const Standard_Real V1 = SS->V1() * vFact;
    const Standard_Real V2 = SS->V2() * vFact;

    CS = new Geom_RectangularTrimmedSurface(theBasis, U1, U2, V1, V2, SS->Usense(), SS->Vsense());
    return Standard_True;
  }
  return Standard_False;
}
