// Created on: 1996-01-25
// Created by: Frederic MAUPAS
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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
