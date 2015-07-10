// Created on: 2003-03-18
// Created by: Oleg FEDYAEV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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


#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_ExtPS.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_Tool.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>

// The functions Parameter(s) are used to compute parameter(s) of point
// on curves and surfaces. The main rule is that tested point must lied
// on curves or surfaces otherwise the resulted parameter(s) may be wrong.
// To make search process more common the tolerance value is used to define
// the proximity of point to curve or surface. It is clear that this tolerance
// value can't be too high to be not in conflict with previous rule.
static const Standard_Real MAXTOLERANCEGEOM = 1.e-4;
static const Standard_Real PARTOLERANCE = 1.e-9;

//=======================================================================
//function : Parameter
//purpose  : Get parameter on curve of given point
//           return FALSE if point is far from curve than tolerance
//           or computation fails
//=======================================================================

Standard_Boolean GeomLib_Tool::Parameter(const Handle(Geom_Curve)& Curve,
                                         const gp_Pnt&             Point,
                                         const Standard_Real       Tolerance,
                                               Standard_Real&            U)
{
  if( Curve.IsNull() ) return Standard_False;
  //
  U = 0.;
  Standard_Real aTol = (Tolerance < MAXTOLERANCEGEOM) ? Tolerance : MAXTOLERANCEGEOM;
  aTol *= aTol;
  //
  Handle(Standard_Type) KindOfCurve = Curve->DynamicType();

  // process analitical curves
  if( KindOfCurve == STANDARD_TYPE (Geom_Line) ||
    KindOfCurve == STANDARD_TYPE (Geom_Circle) ||
    KindOfCurve == STANDARD_TYPE (Geom_Ellipse) ||
    KindOfCurve == STANDARD_TYPE (Geom_Parabola) ||
    KindOfCurve == STANDARD_TYPE (Geom_Hyperbola) )
  {
    Standard_Real D = 0.;

    if( KindOfCurve == STANDARD_TYPE (Geom_Line) )
    {
      Handle(Geom_Line) aGL = Handle(Geom_Line)::DownCast(Curve);
      gp_Lin aLin = aGL->Lin();
      D = aLin.SquareDistance(Point);
      if(D > aTol)
      {
        return Standard_False;
      }
      U = ElCLib::Parameter(aLin,Point);
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom_Circle) )
    {
      Handle(Geom_Circle) aGC = Handle(Geom_Circle)::DownCast(Curve);
      gp_Circ aCirc = aGC->Circ();
      D = aCirc.SquareDistance(Point);
      if(D > aTol)
      {
        return Standard_False;
      }
      U = ElCLib::Parameter(aCirc,Point);
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom_Ellipse) )
    {
      Handle(Geom_Ellipse) aGE = Handle(Geom_Ellipse)::DownCast(Curve);
      gp_Elips anElips = aGE->Elips();
      U = ElCLib::Parameter(anElips,Point);
      D = Point.SquareDistance(aGE->Value(U));
      if(D > aTol)
      {
        return Standard_False;
      }
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom_Parabola) )
    { 
      Handle(Geom_Parabola) aGP = Handle(Geom_Parabola)::DownCast(Curve);
      gp_Parab aParab = aGP->Parab();
      U = ElCLib::Parameter(aParab,Point);
      D = Point.SquareDistance(aGP->Value(U));
      if(D > aTol)
      {
        return Standard_False;
      }
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom_Hyperbola) )
    {
      Handle(Geom_Hyperbola) aGH = Handle(Geom_Hyperbola)::DownCast(Curve);
      gp_Hypr aHypr = aGH->Hypr();
      U = ElCLib::Parameter(aHypr,Point);
      D = Point.SquareDistance(aGH->Value(U));
      if(D > aTol)
      {
        return Standard_False;
      }
    }
  }
  // process parametrical curves
  else if( KindOfCurve == STANDARD_TYPE (Geom_BSplineCurve) ||
    KindOfCurve == STANDARD_TYPE (Geom_BezierCurve) ||
    KindOfCurve == STANDARD_TYPE (Geom_TrimmedCurve) ||
    KindOfCurve == STANDARD_TYPE (Geom_OffsetCurve) )
  {
    GeomAdaptor_Curve aGAC(Curve);
    Extrema_ExtPC extrema(Point,aGAC);
    if( !extrema.IsDone() ) return Standard_False;
    Standard_Integer n = extrema.NbExt();
    if( n <= 0 ) return Standard_False;
    Standard_Integer i = 0, iMin = 0;
    Standard_Real Dist2Min = 1.e+100;
    for( i = 1; i <= n; i++ )
    {
      if (extrema.SquareDistance(i) < Dist2Min)
      {
        iMin = i;
        Dist2Min = extrema.SquareDistance(i);
      }
    }
    if( iMin != 0 && Dist2Min <= aTol ) U = (extrema.Point(iMin)).Parameter();
    else return Standard_False;
  }
  else { return Standard_False; }

  return Standard_True;

}

//=======================================================================
//function : Parameters
//purpose  : Get parameters on surface of given point
//           return FALSE if point is far from surface than tolerance
//           or computation fails
//=======================================================================

Standard_Boolean GeomLib_Tool::Parameters(const Handle(Geom_Surface)& Surface,
                                          const gp_Pnt&               Point,
                                          const Standard_Real         Tolerance,
                                                Standard_Real&              U,
                                                Standard_Real&              V)
{
  if( Surface.IsNull() ) return Standard_False;
  //
  U = 0.;
  V = 0.;
  Standard_Real aTol = (Tolerance < MAXTOLERANCEGEOM) ? Tolerance : MAXTOLERANCEGEOM;
  aTol *= aTol;
  //
  Handle(Standard_Type) KindOfSurface = Surface->DynamicType();

  // process analitical surfaces
  if( KindOfSurface == STANDARD_TYPE (Geom_Plane) ||
    KindOfSurface == STANDARD_TYPE (Geom_CylindricalSurface) ||
    KindOfSurface == STANDARD_TYPE (Geom_ConicalSurface) ||
    KindOfSurface == STANDARD_TYPE (Geom_SphericalSurface) ||
    KindOfSurface == STANDARD_TYPE (Geom_ToroidalSurface) )
  {
    Standard_Real D = 0.;

    if( KindOfSurface == STANDARD_TYPE (Geom_Plane) )
    {
      Handle(Geom_Plane) aGP = Handle(Geom_Plane)::DownCast(Surface);
      gp_Pln aPlane = aGP->Pln(); 
      D = aPlane.SquareDistance(Point);
      if(D > aTol)
      {
        return Standard_False;
      }
      ElSLib::Parameters (aPlane,Point,U,V);
    }
    else if( KindOfSurface == STANDARD_TYPE (Geom_CylindricalSurface) )
    {
      Handle(Geom_CylindricalSurface) aGC = Handle(Geom_CylindricalSurface)::DownCast(Surface);
      gp_Cylinder aCylinder = aGC->Cylinder();
      ElSLib::Parameters(aCylinder,Point,U,V);
      D = Point.SquareDistance(aGC->Value(U, V));
      if(D > aTol)
      {
        return Standard_False;
      }
    }
    else if( KindOfSurface == STANDARD_TYPE (Geom_ConicalSurface) )
    {
      Handle(Geom_ConicalSurface) aGC = Handle(Geom_ConicalSurface)::DownCast(Surface);
      gp_Cone aCone = aGC->Cone();
      ElSLib::Parameters(aCone,Point,U,V);
      D = Point.SquareDistance(aGC->Value(U, V));
      if(D > aTol)
      {
        return Standard_False;
      }
    }
    else if( KindOfSurface == STANDARD_TYPE (Geom_SphericalSurface) )
    {
      Handle(Geom_SphericalSurface) aGS = Handle(Geom_SphericalSurface)::DownCast(Surface);
      gp_Sphere aSphere = aGS->Sphere(); 
      ElSLib::Parameters(aSphere,Point,U,V);
      D = Point.SquareDistance(aGS->Value(U, V));
      if(D > aTol)
      {
        return Standard_False;
      }
    }
    else if( KindOfSurface == STANDARD_TYPE (Geom_ToroidalSurface) )
    {
      Handle(Geom_ToroidalSurface) aTS = Handle(Geom_ToroidalSurface)::DownCast(Surface);
      gp_Torus aTorus = aTS->Torus();
      ElSLib::Parameters(aTorus,Point,U,V);
      D = Point.SquareDistance(aTS->Value(U, V));
      if(D > aTol)
      {
        return Standard_False;
      }
    }
    else return Standard_False;
  }
  // process parametrical surfaces
  else if( KindOfSurface == STANDARD_TYPE (Geom_BSplineSurface) ||
    KindOfSurface == STANDARD_TYPE (Geom_BezierSurface) ||
    KindOfSurface == STANDARD_TYPE (Geom_RectangularTrimmedSurface) ||
    KindOfSurface == STANDARD_TYPE (Geom_OffsetSurface) ||
    KindOfSurface == STANDARD_TYPE (Geom_SurfaceOfLinearExtrusion) ||
    KindOfSurface == STANDARD_TYPE (Geom_SurfaceOfRevolution) )
  {
    GeomAdaptor_Surface aGAS(Surface);
    Standard_Real aTolU = PARTOLERANCE, aTolV = PARTOLERANCE;
    Extrema_ExtPS extrema(Point,aGAS,aTolU,aTolV);
    if( !extrema.IsDone() ) return Standard_False;
    Standard_Integer n = extrema.NbExt();
    if( n <= 0 ) return Standard_False;

    Standard_Real Dist2Min = 1.e+100;
    Standard_Integer i = 0, iMin = 0;
    for( i = 1; i <= n; i++ )
    {
      if( extrema.SquareDistance(i) < Dist2Min )
      {
        Dist2Min = extrema.SquareDistance(i);
        iMin = i;
      }
    }
    if( iMin != 0 && Dist2Min <= aTol) extrema.Point(iMin).Parameter(U,V);
    else return Standard_False;
  }
  else { return Standard_False; }

  return Standard_True;

}

//=======================================================================
//function : Parameter
//purpose  : Get parameter on curve of given point
//           return FALSE if point is far from curve than tolerance
//           or computation fails
//=======================================================================

Standard_Boolean GeomLib_Tool::Parameter(const Handle(Geom2d_Curve)& Curve,
                                         const gp_Pnt2d&             Point,
                                         const Standard_Real         Tolerance,
                                               Standard_Real&              U)
{
  if( Curve.IsNull() ) return Standard_False;
  //
  U = 0.;
  Standard_Real aTol = (Tolerance < MAXTOLERANCEGEOM) ? Tolerance : MAXTOLERANCEGEOM;
  aTol *= aTol;

  Handle(Standard_Type) KindOfCurve = Curve->DynamicType();

  // process analytical curves
  if( KindOfCurve == STANDARD_TYPE (Geom2d_Line) ||
    KindOfCurve == STANDARD_TYPE (Geom2d_Circle) ||
    KindOfCurve == STANDARD_TYPE (Geom2d_Ellipse) ||
    KindOfCurve == STANDARD_TYPE (Geom2d_Parabola) ||
    KindOfCurve == STANDARD_TYPE (Geom2d_Hyperbola) )
  {
    Standard_Real D = 0.;

    if( KindOfCurve == STANDARD_TYPE (Geom2d_Line) )
    {
      Handle(Geom2d_Line) aGL = Handle(Geom2d_Line)::DownCast(Curve);
      gp_Lin2d aLin = aGL->Lin2d();
      D = aLin.SquareDistance(Point);
      if(D > aTol)
      {
        return Standard_False;
      }
      U = ElCLib::Parameter(aLin,Point);
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom2d_Circle) )
    {
      Handle(Geom2d_Circle) aGC = Handle(Geom2d_Circle)::DownCast(Curve);
      gp_Circ2d aCirc = aGC->Circ2d();
      D = aCirc.SquareDistance(Point);
      if(D > aTol)
      {
        return Standard_False;
      }   
      U = ElCLib::Parameter(aCirc,Point);
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom2d_Ellipse) )
    {
      Handle(Geom2d_Ellipse) aGE = Handle(Geom2d_Ellipse)::DownCast(Curve);
      gp_Elips2d anElips = aGE->Elips2d();
      U = ElCLib::Parameter(anElips,Point);
      D = Point.SquareDistance(aGE->Value(U));
      if(D > aTol)
      {
        return Standard_False;
      }   
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom2d_Parabola) )
    {
      Handle(Geom2d_Parabola) aGP = Handle(Geom2d_Parabola)::DownCast(Curve);
      gp_Parab2d aParab = aGP->Parab2d();
      U = ElCLib::Parameter(aParab,Point);
      D = Point.SquareDistance(aGP->Value(U));
      if(D > aTol)
      {
        return Standard_False;
      }   
    }
    else if( KindOfCurve == STANDARD_TYPE (Geom2d_Hyperbola) )
    {
      Handle(Geom2d_Hyperbola) aGH = Handle(Geom2d_Hyperbola)::DownCast(Curve);
      gp_Hypr2d aHypr = aGH->Hypr2d();
      U = ElCLib::Parameter(aHypr,Point);
      D = Point.SquareDistance(aGH->Value(U));
      if(D > aTol)
      {
        return Standard_False;
      }   
    }
    else return Standard_False;
  }
  // process parametrical curves
  else if( KindOfCurve == STANDARD_TYPE (Geom2d_BSplineCurve) ||
    KindOfCurve == STANDARD_TYPE (Geom2d_BezierCurve) ||
    KindOfCurve == STANDARD_TYPE (Geom2d_TrimmedCurve) ||
    KindOfCurve == STANDARD_TYPE (Geom2d_OffsetCurve) )
  {
    Geom2dAdaptor_Curve aGAC(Curve);
    Extrema_ExtPC2d extrema(Point,aGAC);
    if( !extrema.IsDone() ) return Standard_False;
    Standard_Integer n = extrema.NbExt();
    if( n <= 0 ) return Standard_False;
    Standard_Integer i = 0, iMin = 0;
    Standard_Real Dist2Min = 1.e+100;
    for ( i = 1; i <= n; i++ )
    {
      if( extrema.SquareDistance(i) < Dist2Min )
      {
        Dist2Min = extrema.SquareDistance(i);
        iMin = i;
      }
    }
    if( iMin != 0 && Dist2Min <= aTol ) U = (extrema.Point(iMin)).Parameter();
    else return Standard_False;
  }
  else { return Standard_False; }

  return Standard_True;
}
