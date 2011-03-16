// File:	GeomToStep_MakeBoundedSurface.cxx
// Created:	Tue Jun 22 12:38:59 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

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
