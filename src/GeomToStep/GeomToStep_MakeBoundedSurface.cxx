// Created on: 1993-06-22
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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


#include <GeomToStep_MakeBoundedSurface.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <GeomToStep_MakeBoundedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomToStep_MakeBSplineSurfaceWithKnots.hxx>
#include <GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <GeomToStep_MakeRectangularTrimmedSurface.hxx>

//=============================================================================
// Creation d' une BoundedSurface de prostep a partir d' une BoundedSurface
// de Geom
//=============================================================================

GeomToStep_MakeBoundedSurface::GeomToStep_MakeBoundedSurface
  ( const Handle(Geom_BoundedSurface)& S)
{
  done = Standard_True;
  if (S->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
    Handle(Geom_BSplineSurface) BS= Handle(Geom_BSplineSurface)::DownCast(S);
    // UPDATE FMA 1-04-96
    if (S->IsUPeriodic() || S->IsVPeriodic()) {
      Handle(Geom_BSplineSurface) newBS = 
	Handle(Geom_BSplineSurface)::DownCast(BS->Copy());
      newBS->SetUNotPeriodic();
      newBS->SetVNotPeriodic();
      BS = newBS;
    }
    if ( BS->IsURational() || BS->IsVRational() ) {
      GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface 
	MkRatBSplineS(BS);
      theBoundedSurface = MkRatBSplineS.Value();
    }
    else {
      GeomToStep_MakeBSplineSurfaceWithKnots MkBSplineS(BS);
      theBoundedSurface = MkBSplineS.Value();
    }
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
    Handle(Geom_BezierSurface) Sur = Handle(Geom_BezierSurface)::DownCast(S);
    Handle (Geom_BSplineSurface) BS = 
      GeomConvert::SurfaceToBSplineSurface(Sur);
    if ( BS->IsURational() || BS->IsVRational() ) {
      GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface 
	MkRatBSplineS(BS);
      theBoundedSurface = MkRatBSplineS.Value();
    }
    else {
      GeomToStep_MakeBSplineSurfaceWithKnots MkBSplineS(BS);
      theBoundedSurface = MkBSplineS.Value();
    }
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) Sur = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
    GeomToStep_MakeRectangularTrimmedSurface MkRTSurf(Sur);
    theBoundedSurface = MkRTSurf.Value();
  }
  else
    done = Standard_False;
}	 


//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_BoundedSurface) &
      GeomToStep_MakeBoundedSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theBoundedSurface;
}
