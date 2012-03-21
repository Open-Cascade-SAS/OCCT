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


#include <GeomToStep_MakeRectangularTrimmedSurface.ixx>
#include <StdFail_NotDone.hxx>

#include <GeomToStep_MakeSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <TCollection_HAsciiString.hxx>

#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <UnitsMethods.hxx>
#include <Geom_Plane.hxx>

//=============================================================================
// Creation d' une rectangular_trimmed_surface de STEP
// a partir d' une RectangularTrimmedSurface de Geom
//=============================================================================

GeomToStep_MakeRectangularTrimmedSurface::
  GeomToStep_MakeRectangularTrimmedSurface( const
    Handle(Geom_RectangularTrimmedSurface)& RTSurf )
								      
{

  Handle(StepGeom_RectangularTrimmedSurface) StepRTS = new StepGeom_RectangularTrimmedSurface;

  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");

  GeomToStep_MakeSurface mkSurf(RTSurf->BasisSurface());
  if (!mkSurf.IsDone()) {
    done = Standard_False;
    return;
  }
  Handle(StepGeom_Surface) StepSurf = mkSurf.Value();

  Standard_Real U1,U2,V1,V2;
  RTSurf->Bounds(U1, U2, V1, V2);
  
  // -----------------------------------------
  // Modification of the Trimming Parameters ?
  // -----------------------------------------

  Standard_Real AngleFact = 180./M_PI;
  Standard_Real uFact = 1.;
  Standard_Real vFact = 1.;
  Standard_Real LengthFact  = UnitsMethods::LengthFactor();
  Handle(Geom_Surface) theSurf = RTSurf->BasisSurface();
  if (theSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
    uFact = AngleFact;
    vFact = 1. / LengthFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    uFact = AngleFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)) ||
	   theSurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    uFact = AngleFact;
    vFact = AngleFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
    Handle(Geom_ConicalSurface) conicS = 
      Handle(Geom_ConicalSurface)::DownCast(theSurf);
    Standard_Real semAng = conicS->SemiAngle();
    uFact = AngleFact;
    vFact = Cos(semAng) / LengthFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_Plane))) {
    uFact = vFact = 1. / LengthFact;
  }
  
  U1 = U1 * uFact;
  U2 = U2 * uFact;
  V1 = V1 * vFact;
  V2 = V2 * vFact;

  StepRTS->Init(aName, StepSurf, U1, U2, V1, V2, Standard_True, Standard_True);
  theRectangularTrimmedSurface = StepRTS;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_RectangularTrimmedSurface) &
      GeomToStep_MakeRectangularTrimmedSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theRectangularTrimmedSurface;
}
