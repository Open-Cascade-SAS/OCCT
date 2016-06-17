// Created on: 1996-11-08
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfReal.hxx>

inline static void D0 (const Adaptor3d_Curve& C, const Standard_Real U, gp_Pnt& P)
{
  C.D0 (U, P);
}

inline static void D2 (const Adaptor3d_Curve& C, const Standard_Real U, 
                       gp_Pnt& P, gp_Vec& V1, gp_Vec& V2)
{
  C.D2 (U, P, V1, V2);
}


static void D0 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt& PP)
{
  Standard_Real X, Y;
  gp_Pnt2d P;
  C.D0 (U, P);
  P.Coord (X, Y);
  PP.SetCoord (X, Y, 0.0);
}

static void D2 (const Adaptor2d_Curve2d& C, const Standard_Real U,
	        gp_Pnt& PP, gp_Vec& VV1, gp_Vec& VV2)
{
  Standard_Real X, Y;
  gp_Pnt2d P;
  gp_Vec2d V1,V2;
  C.D2 (U, P, V1, V2);
  P.Coord (X, Y);
  PP.SetCoord  (X, Y, 0.0);
  V1.Coord (X, Y);
  VV1.SetCoord (X, Y, 0.0);
  V2.Coord (X, Y);
  VV2.SetCoord (X, Y, 0.0);
}

static Standard_Real EstimAngl(const gp_Pnt& P1, const gp_Pnt& Pm, const gp_Pnt& P2)
{
  gp_Vec V1(P1, Pm), V2(Pm, P2);
  Standard_Real L = V1.Magnitude() * V2.Magnitude();
  //
  if(L > gp::Resolution())
  {
    return V1.CrossMagnitude(V2)/L;
  }
  else
  {
    return 0.;
  }
}


// Return number of interval of continuity on which theParam is located.
// Last parameter is used to increase search speed.
static Standard_Integer getIntervalIdx(const Standard_Real theParam, 
                                       TColStd_Array1OfReal& theIntervs,
                                       const Standard_Integer thePreviousIdx)
{
  Standard_Integer anIdx;
  for(anIdx = thePreviousIdx; anIdx < theIntervs.Upper(); anIdx++)
  {
    if (theParam >= theIntervs(anIdx) && 
        theParam <= theIntervs(anIdx + 1)) // Inside of anIdx interval.
    {
      break;
    }
  }
  return anIdx;
}
//
//=======================================================================
//function : CPnts_TangentialDeflection
//purpose  : 
//=======================================================================

GCPnts_TangentialDeflection::GCPnts_TangentialDeflection () { }

//=======================================================================
//function : AddPoint
//purpose  : 
//=======================================================================

Standard_Integer GCPnts_TangentialDeflection::AddPoint
 (const gp_Pnt& thePnt,
  const Standard_Real theParam,
  const Standard_Boolean theIsReplace)
{
  const Standard_Real tol = Precision::PConfusion();
  Standard_Integer index = -1;
  const Standard_Integer nb = parameters.Length();
  for ( Standard_Integer i = 1; index == -1 && i <= nb; i++ )
  {
    Standard_Real dist = parameters.Value( i ) - theParam;
    if ( fabs( dist ) <= tol )
    {
      index = i;
      if ( theIsReplace )
      {
	points.ChangeValue(i) = thePnt;
	parameters.ChangeValue(i) = theParam;
      }
    }
    else if ( dist > tol )
    {
      points.InsertBefore( i, thePnt );
      parameters.InsertBefore( i, theParam );
      index = i;
    }
  }
  if ( index == -1 )
  {
    points.Append( thePnt );
    parameters.Append( theParam );
    index = parameters.Length();
  }
  return index;
}

//=======================================================================
//function : ArcAngularStep
//purpose  : 
//=======================================================================
Standard_Real GCPnts_TangentialDeflection::ArcAngularStep(
  const Standard_Real theRadius,
  const Standard_Real theLinearDeflection,
  const Standard_Real theAngularDeflection,
  const Standard_Real theMinLength)
{
  Standard_ConstructionError_Raise_if(theRadius < 0.0, "Negative radius");

  const Standard_Real aPrecision = Precision::Confusion();

  Standard_Real Du = 0.0, aMinSizeAng = 0.0;
  if (theRadius > aPrecision)
  {
    Du = Max(1.0 - (theLinearDeflection / theRadius), 0.0);

    // It is not suitable to consider min size greater than 1/4 arc len.
    if (theMinLength > aPrecision)
      aMinSizeAng = Min(theMinLength / theRadius, M_PI_2);
  }
  Du = 2.0 * ACos(Du);
  Du = Max(Min(Du, theAngularDeflection), aMinSizeAng);
  return Du;
}

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Circ.hxx>
#include <GCPnts_DistFunction.hxx>

#define TheCurve Adaptor3d_Curve
#define Handle_TheBezierCurve   Handle(Geom_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom_BSplineCurve)
#define TheMaxCurvLinDist GCPnts_DistFunction
#define TheMaxCurvLinDistMV GCPnts_DistFunctionMV
#include "GCPnts_TangentialDeflection.pxx"
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve
#undef TheCurve
#undef TheMaxCurvLinDist
#undef TheMaxCurvLinDistMV


#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Circ2d.hxx>
#include <GCPnts_DistFunction2d.hxx>
#define TheCurve Adaptor2d_Curve2d
#define Handle_TheBezierCurve   Handle(Geom2d_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom2d_BSplineCurve)
#define TheMaxCurvLinDist GCPnts_DistFunction2d
#define TheMaxCurvLinDistMV GCPnts_DistFunction2dMV
#include "GCPnts_TangentialDeflection.pxx"
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve
#undef TheCurve
#undef TheMaxCurvLinDist
#undef TheMaxCurvLinDistMV
