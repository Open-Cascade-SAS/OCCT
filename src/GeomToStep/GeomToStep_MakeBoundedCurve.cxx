// File:	GeomToStep_MakeBoundedCurve.cxx
// Created:	Mon Jun 21 11:35:12 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeBoundedCurve.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <GeomToStep_MakeBoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <GeomConvert.hxx>
#include <Geom2dConvert.hxx>
#include <GeomToStep_MakeBSplineCurveWithKnots.hxx>
#include <GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve.hxx>


//=============================================================================
// Creation d' une BoundedCurve de prostep a partir d' une BoundedCurve de Geom
//=============================================================================

GeomToStep_MakeBoundedCurve::GeomToStep_MakeBoundedCurve ( const Handle(Geom_BoundedCurve)& C)
{
  done = Standard_True;

  if (C->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    Handle(Geom_BSplineCurve) Bspli = Handle(Geom_BSplineCurve)::DownCast(C);
    // UPDATE FMA 1-04-96
    if (C->IsPeriodic()) {
      Handle(Geom_BSplineCurve) newBspli = 
	Handle(Geom_BSplineCurve)::DownCast(Bspli->Copy());
      newBspli->SetNotPeriodic();
      Bspli = newBspli;
    }
    if ( Bspli->IsRational() ) {
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli);
      theBoundedCurve = MkRatBSplineC.Value();
    }
    else {
      GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
      theBoundedCurve = MkBSplineC.Value();
    }
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
    Handle(Geom_BezierCurve) Cur = Handle(Geom_BezierCurve)::DownCast(C);
    Handle(Geom_BSplineCurve) Bspli = GeomConvert::CurveToBSplineCurve(Cur);
    if ( Bspli->IsRational() ) {
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli);
      theBoundedCurve = MkRatBSplineC.Value();
    }
    else {
      GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
      theBoundedCurve = MkBSplineC.Value();
    }
  }
  else {
#ifdef DEB
    cout<<"MakeBoundedCurve, type : "<<C->DynamicType()->Name()<<" not processed"<<endl;
#endif
    done = Standard_False;
  }
}
	 
//=============================================================================
// Creation d' une BoundedCurve de prostep a partir d' une BoundedCurve de
// Geom2d
//=============================================================================

GeomToStep_MakeBoundedCurve::GeomToStep_MakeBoundedCurve ( const Handle(Geom2d_BoundedCurve)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      Handle(Geom2d_BSplineCurve) Bspli = 
	Handle(Geom2d_BSplineCurve)::DownCast(C);
      // UPDATE FMA 1-04-96
      if (C->IsPeriodic()) {
	Handle(Geom2d_BSplineCurve) newBspli = 
	  Handle(Geom2d_BSplineCurve)::DownCast(Bspli->Copy());
	newBspli->SetNotPeriodic();
	Bspli = newBspli;
      }
      if ( Bspli->IsRational() ) {
	GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli);
	theBoundedCurve = MkRatBSplineC.Value();
      }
      else {
	GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
	theBoundedCurve = MkBSplineC.Value();
      }
    }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) {
    Handle(Geom2d_BezierCurve) Cur = Handle(Geom2d_BezierCurve)::DownCast(C);
    Handle(Geom2d_BSplineCurve) Bspli = 
      Geom2dConvert::CurveToBSplineCurve(Cur);
    GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
    theBoundedCurve = MkBSplineC.Value();
    }
  else
    done = Standard_False;
  
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_BoundedCurve) &
      GeomToStep_MakeBoundedCurve::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theBoundedCurve;
}
