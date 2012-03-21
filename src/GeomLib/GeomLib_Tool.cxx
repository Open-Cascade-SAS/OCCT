// Created on: 2003-03-18
// Created by: Oleg FEDYAEV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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


#include <GeomLib_Tool.ixx>

#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx> 
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
#include <gp_Hypr.hxx> 
#include <gp_Vec.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPS.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC2d.hxx>


// The functions Parameter(s) are used to compute parameter(s) of point
// on curves and surfaces. The main rule is that tested point must lied
// on curves or surfaces otherwise the resulted parameter(s) may be wrong.
// To make search process more common the tolerance value is used to define
// the proximity of point to curve or surface. It is clear that this tolerance
// value can't be too high to be not in conflict with previous rule.
static const Standard_Real MAXTOLERANCEGEOM = 1.e-4;
static const Standard_Real MAXTOLERANCEPARM = 1.e-3;
static const Standard_Real UNKNOWNVALUE = 1.e+100;
static const Standard_Real PARTOLERANCE = 1.e-9;

//=======================================================================
//function : ProcessAnalyticalSurfaces
//purpose  : Computes the coefficients of the implicit equation
//           of the analytical surfaces in the absolute cartesian
//           coordinate system and check given point
//=======================================================================

static Standard_Boolean ProcessAnalyticalSurfaces(const Handle(Geom_Surface)& Surface,
						  const gp_Pnt&               Point,
						  Standard_Real&              Delta)
{
  Delta = UNKNOWNVALUE;
  Handle(Standard_Type) KindOfSurface = Surface->DynamicType();
  if( KindOfSurface == STANDARD_TYPE (Geom_Plane) )
    {
      Handle(Geom_Plane) aGP = Handle(Geom_Plane)::DownCast(Surface);
      if( aGP.IsNull() ) return Standard_False;
      gp_Pln aPlane = aGP->Pln();
      Delta = aPlane.Distance(Point);
    }
  else if( KindOfSurface == STANDARD_TYPE (Geom_CylindricalSurface) )
    {
      Handle(Geom_CylindricalSurface) aGC = Handle(Geom_CylindricalSurface)::DownCast(Surface);
      if( aGC.IsNull() ) return Standard_False;
      gp_Vec aVec(aGC->Cylinder().Location(),Point);
      Standard_Real X = aVec.Dot(aGC->Cylinder().XAxis().Direction());
      Standard_Real Y = aVec.Dot(aGC->Cylinder().YAxis().Direction());
      Delta = X*X + Y*Y - aGC->Cylinder().Radius()*aGC->Cylinder().Radius();
    }
  else if( KindOfSurface == STANDARD_TYPE (Geom_ConicalSurface) )
    {
      Handle(Geom_ConicalSurface) aGC = Handle(Geom_ConicalSurface)::DownCast(Surface);
      if( aGC.IsNull() ) return Standard_False;
      gp_Vec aVec(aGC->Cone().Location(),Point);
      Standard_Real X = aVec.Dot(aGC->Cone().XAxis().Direction());
      Standard_Real Y = aVec.Dot(aGC->Cone().YAxis().Direction());
      Standard_Real Z = aVec.Dot(aGC->Cone().Axis().Direction());
      Standard_Real tZ = aGC->Cone().RefRadius() + Z * Tan(aGC->Cone().SemiAngle());
      Delta = X*X + Y*Y - tZ*tZ; 
    }
  else if( KindOfSurface == STANDARD_TYPE (Geom_SphericalSurface) )
    {
      Handle(Geom_SphericalSurface) aGS = Handle(Geom_SphericalSurface)::DownCast(Surface);
      if( aGS.IsNull() ) return Standard_False;
      gp_Vec aVec(aGS->Sphere().Location(),Point);
      Standard_Real X = aVec.Dot(aGS->Sphere().XAxis().Direction());
      Standard_Real Y = aVec.Dot(aGS->Sphere().YAxis().Direction());
      gp_Dir Zdir = aGS->Sphere().XAxis().Direction().Crossed(aGS->Sphere().YAxis().Direction());
      Standard_Real Z = aVec.Dot(Zdir);
      Delta =  X*X + Y*Y + Z*Z - aGS->Sphere().Radius() * aGS->Sphere().Radius();
    }
  else if( KindOfSurface == STANDARD_TYPE (Geom_ToroidalSurface) )
    {
      Handle(Geom_ToroidalSurface) aTS = Handle(Geom_ToroidalSurface)::DownCast(Surface);
      if( aTS.IsNull() ) return Standard_False;
      gp_Vec aVec(aTS->Torus().Location(),Point);
      Standard_Real X = aVec.Dot(aTS->Torus().XAxis().Direction());
      Standard_Real Y = aVec.Dot(aTS->Torus().YAxis().Direction());
      Standard_Real Z = aVec.Dot(aTS->Torus().Axis().Direction());
      Delta = X*X + Y*Y + Z*Z - 2.*aTS->Torus().MajorRadius()*Sqrt( X*X + Y*Y ) -
	    aTS->Torus().MinorRadius()*aTS->Torus().MinorRadius() +
	    aTS->Torus().MajorRadius()*aTS->Torus().MajorRadius();
    }
  else
    {
      return Standard_False;
    }
  return Standard_True;
}

//=======================================================================
//function : ProcessAnalyticalCurves
//purpose  : Computes the coefficients of the implicit equation
//           of the analytical curves and check given point
//=======================================================================

static Standard_Boolean ProcessAnalyticalCurves(const Handle(Geom_Curve)& Curve,
						const gp_Pnt&             Point,
						Standard_Real&            Delta)
{
  Delta = UNKNOWNVALUE;
  Handle(Standard_Type) KindOfCurve = Curve->DynamicType();
  if( KindOfCurve == STANDARD_TYPE (Geom_Line) )
    {
      Handle(Geom_Line) aGL = Handle(Geom_Line)::DownCast(Curve);
      if( aGL.IsNull() ) return Standard_False;
      gp_Lin aLin = aGL->Lin();
      Delta = aLin.Distance(Point);
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom_Circle) )
    {
      Handle(Geom_Circle) aGC = Handle(Geom_Circle)::DownCast(Curve);
      if( aGC.IsNull() ) return Standard_False;
      gp_Circ aCirc = aGC->Circ();
      Delta = aCirc.Distance(Point);
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom_Ellipse) )
    {
      Handle(Geom_Ellipse) aGE = Handle(Geom_Ellipse)::DownCast(Curve);
      if( aGE.IsNull() ) return Standard_False;
      gp_Elips anElips = aGE->Elips();
      gp_Vec aVec(anElips.Location(),Point);
      Standard_Real X = aVec.Dot(anElips.XAxis().Direction());
      Standard_Real Y = aVec.Dot(anElips.YAxis().Direction());
      Standard_Real Z = aVec.Dot(anElips.Axis().Direction());
      Standard_Real AA = anElips.MajorRadius()*anElips.MajorRadius();
      Standard_Real BB = anElips.MinorRadius()*anElips.MinorRadius();
      Standard_Real tY = Sqrt( Abs( (1. - X*X/AA)*BB ) );
      Delta = Max( Abs(Z), Abs( Y - tY ) );
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom_Parabola) )
    { 
      Handle(Geom_Parabola) aGP = Handle(Geom_Parabola)::DownCast(Curve);
      if( aGP.IsNull() ) return Standard_False;
      gp_Parab aParab = aGP->Parab();
      gp_Vec aVec(aParab.Location(),Point);
      Standard_Real X = aVec.Dot(aParab.XAxis().Direction());
      Standard_Real Y = aVec.Dot(aParab.YAxis().Direction());
      Standard_Real Z = aVec.Dot(aParab.Axis().Direction());
      Standard_Real tY = Sqrt( Abs(X*2.*aParab.Parameter()) );
      Delta = Max( Abs(Z), Abs( Y - tY ) );
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom_Hyperbola) )
    {
      Handle(Geom_Hyperbola) aGH = Handle(Geom_Hyperbola)::DownCast(Curve);
      if( aGH.IsNull() ) return Standard_False;
      gp_Hypr aHypr = aGH->Hypr();
      gp_Vec aVec(aHypr.Location(),Point);
      Standard_Real X = aVec.Dot(aHypr.XAxis().Direction());
      Standard_Real Y = aVec.Dot(aHypr.YAxis().Direction());
      Standard_Real Z = aVec.Dot(aHypr.Axis().Direction());
      Standard_Real AA = aHypr.MajorRadius()*aHypr.MajorRadius();
      Standard_Real BB = aHypr.MinorRadius()*aHypr.MinorRadius();
      Standard_Real tY1 = Sqrt( Abs( (X*X/AA - 1.)*BB ) );
      Standard_Real tY2 = Sqrt( (X*X/AA + 1.)*BB );
      Delta = Max( Abs(Z), Min( Abs( tY1 - Y ), Abs( tY2 - Y ) ) );
    }
  else return Standard_False;
  return Standard_True;
}


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
  U = 0.;
  if( Curve.IsNull() ) return Standard_False;
  Handle(Standard_Type) KindOfCurve = Curve->DynamicType();

  // process analitical curves
  if( KindOfCurve == STANDARD_TYPE (Geom_Line) ||
      KindOfCurve == STANDARD_TYPE (Geom_Circle) ||
      KindOfCurve == STANDARD_TYPE (Geom_Ellipse) ||
      KindOfCurve == STANDARD_TYPE (Geom_Parabola) ||
      KindOfCurve == STANDARD_TYPE (Geom_Hyperbola) )
    {
      Standard_Real aTol = (Tolerance < MAXTOLERANCEGEOM) ? Tolerance : MAXTOLERANCEGEOM;
      Standard_Real D = 0.;
      Standard_Boolean isOk = ProcessAnalyticalCurves(Curve,Point,D);
      if( !isOk ) return Standard_False;
      if( Abs(D) > aTol ) return Standard_False;

      if( KindOfCurve == STANDARD_TYPE (Geom_Line) )
	{
	  Handle(Geom_Line) aGL = Handle(Geom_Line)::DownCast(Curve);
	  gp_Lin aLin = aGL->Lin();
	  U = ElCLib::Parameter(aLin,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom_Circle) )
	{
	  Handle(Geom_Circle) aGC = Handle(Geom_Circle)::DownCast(Curve);
	  gp_Circ aCirc = aGC->Circ();
	  U = ElCLib::Parameter(aCirc,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom_Ellipse) )
	{
	  Handle(Geom_Ellipse) aGE = Handle(Geom_Ellipse)::DownCast(Curve);
	  gp_Elips anElips = aGE->Elips();
	  U = ElCLib::Parameter(anElips,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom_Parabola) )
	{ 
	  Handle(Geom_Parabola) aGP = Handle(Geom_Parabola)::DownCast(Curve);
	  gp_Parab aParab = aGP->Parab();
	  U = ElCLib::Parameter(aParab,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom_Hyperbola) )
	{
	  Handle(Geom_Hyperbola) aGH = Handle(Geom_Hyperbola)::DownCast(Curve);
	  gp_Hypr aHypr = aGH->Hypr();
	  U = ElCLib::Parameter(aHypr,Point);
	}
    }
  // process parametrical curves
  else if( KindOfCurve == STANDARD_TYPE (Geom_BSplineCurve) ||
	   KindOfCurve == STANDARD_TYPE (Geom_BSplineCurve) ||
	   KindOfCurve == STANDARD_TYPE (Geom_TrimmedCurve) ||
	   KindOfCurve == STANDARD_TYPE (Geom_OffsetCurve) )
    {
      Standard_Real aTol = (Tolerance < MAXTOLERANCEPARM) ? Tolerance : MAXTOLERANCEPARM;
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
      if( iMin != 0 && Dist2Min <= aTol * aTol ) U = (extrema.Point(iMin)).Parameter();
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
  U = 0.;
  V = 0.;
  if( Surface.IsNull() ) return Standard_False;
  Handle(Standard_Type) KindOfSurface = Surface->DynamicType();

  // process analitical surfaces
  if( KindOfSurface == STANDARD_TYPE (Geom_Plane) ||
      KindOfSurface == STANDARD_TYPE (Geom_CylindricalSurface) ||
      KindOfSurface == STANDARD_TYPE (Geom_ConicalSurface) ||
      KindOfSurface == STANDARD_TYPE (Geom_SphericalSurface) ||
      KindOfSurface == STANDARD_TYPE (Geom_ToroidalSurface) )
    {
      Standard_Real aTol = (Tolerance < MAXTOLERANCEGEOM) ? Tolerance : MAXTOLERANCEGEOM;
      Standard_Real D = 0.;
      Standard_Boolean isOk = ProcessAnalyticalSurfaces(Surface,Point,D);
      if( !isOk ) return Standard_False;
      if( Abs(D) > aTol ) return Standard_False;

      if( KindOfSurface == STANDARD_TYPE (Geom_Plane) )
	{
	  Handle(Geom_Plane) aGP = Handle(Geom_Plane)::DownCast(Surface);
	  gp_Pln aPlane = aGP->Pln(); 
	  ElSLib::Parameters (aPlane,Point,U,V);
	}
      else if( KindOfSurface == STANDARD_TYPE (Geom_CylindricalSurface) )
	{
	  Handle(Geom_CylindricalSurface) aGC = Handle(Geom_CylindricalSurface)::DownCast(Surface);
	  gp_Cylinder aCylinder = aGC->Cylinder();
	  ElSLib::Parameters(aCylinder,Point,U,V);
	}
      else if( KindOfSurface == STANDARD_TYPE (Geom_ConicalSurface) )
	{
	  Handle(Geom_ConicalSurface) aGC = Handle(Geom_ConicalSurface)::DownCast(Surface);
	  gp_Cone aCone = aGC->Cone();
	  ElSLib::Parameters(aCone,Point,U,V);
	}
      else if( KindOfSurface == STANDARD_TYPE (Geom_SphericalSurface) )
	{
	  Handle(Geom_SphericalSurface) aGS = Handle(Geom_SphericalSurface)::DownCast(Surface);
	  gp_Sphere aSphere = aGS->Sphere(); 
	  ElSLib::Parameters(aSphere,Point,U,V);
	}
      else if( KindOfSurface == STANDARD_TYPE (Geom_ToroidalSurface) )
	{
	  Handle(Geom_ToroidalSurface) aTS = Handle(Geom_ToroidalSurface)::DownCast(Surface);
	  gp_Torus aTorus = aTS->Torus();
	  ElSLib::Parameters(aTorus,Point,U,V);
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
      Standard_Real aTol = (Tolerance < MAXTOLERANCEPARM) ? Tolerance : MAXTOLERANCEPARM;
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
      if( iMin != 0 && Dist2Min <= aTol * aTol ) extrema.Point(iMin).Parameter(U,V);
      else return Standard_False;
    }
  else { return Standard_False; }
  
  return Standard_True;

}

//=======================================================================
//function : ProcessAnalyticalCurves2d
//purpose  : Computes the coefficients of the implicit equation
//           of the analytical curves and check given point
//=======================================================================

static Standard_Boolean ProcessAnalyticalCurves2d(const Handle(Geom2d_Curve)& Curve,
						  const gp_Pnt2d&             Point,
						  Standard_Real&              Delta)
{
  Delta = UNKNOWNVALUE;
  Handle(Standard_Type) KindOfCurve = Curve->DynamicType();
  if( KindOfCurve == STANDARD_TYPE (Geom2d_Line) )
    {
      Handle(Geom2d_Line) aGL = Handle(Geom2d_Line)::DownCast(Curve);
      if( aGL.IsNull() ) return Standard_False;
      gp_Lin2d aLin = aGL->Lin2d();
      Delta = aLin.Distance(Point);
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom2d_Circle) )
    {
      Handle(Geom2d_Circle) aGC = Handle(Geom2d_Circle)::DownCast(Curve);
      if( aGC.IsNull() ) return Standard_False;
      gp_Circ2d aCirc = aGC->Circ2d();
      Delta = aCirc.Distance(Point);
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom2d_Ellipse) )
    {
      Handle(Geom2d_Ellipse) aGE = Handle(Geom2d_Ellipse)::DownCast(Curve);
      if( aGE.IsNull() ) return Standard_False;
      gp_Elips2d anElips = aGE->Elips2d();
      Standard_Real A=0., B=0., C=0., D=0., E=0., F=0.;
      anElips.Coefficients(A,B,C,D,E,F);
      Standard_Real X = Point.X(), Y = Point.Y();
      Delta = A*X*X + B*Y*Y + 2.*C*X*Y + 2.*D*X + 2.*E*Y + F;
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom2d_Parabola) )
    { 
      Handle(Geom2d_Parabola) aGP = Handle(Geom2d_Parabola)::DownCast(Curve);
      if( aGP.IsNull() ) return Standard_False;
      gp_Parab2d aParab = aGP->Parab2d();
      Standard_Real A=0., B=0., C=0., D=0., E=0., F=0.;
      aParab.Coefficients(A,B,C,D,E,F);
      Standard_Real X = Point.X(), Y = Point.Y();
      Delta = A*X*X + B*Y*Y + 2.*C*X*Y + 2.*D*X + 2.*E*Y + F;
    }
  else if( KindOfCurve == STANDARD_TYPE (Geom2d_Hyperbola) )
    {
      Handle(Geom2d_Hyperbola) aGH = Handle(Geom2d_Hyperbola)::DownCast(Curve);
      if( aGH.IsNull() ) return Standard_False;
      gp_Hypr2d aHypr = aGH->Hypr2d();
      Standard_Real A=0., B=0., C=0., D=0., E=0., F=0.;
      aHypr.Coefficients(A,B,C,D,E,F);
      Standard_Real X = Point.X(), Y = Point.Y();
      Delta = A*X*X + B*Y*Y + 2.*C*X*Y + 2.*D*X + 2.*E*Y + F;
    }
  else return Standard_False;

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
  U = 0.;
  if( Curve.IsNull() ) return Standard_False;
  Handle(Standard_Type) KindOfCurve = Curve->DynamicType();

  // process analytical curves
  if( KindOfCurve == STANDARD_TYPE (Geom2d_Line) ||
      KindOfCurve == STANDARD_TYPE (Geom2d_Circle) ||
      KindOfCurve == STANDARD_TYPE (Geom2d_Ellipse) ||
      KindOfCurve == STANDARD_TYPE (Geom2d_Parabola) ||
      KindOfCurve == STANDARD_TYPE (Geom2d_Hyperbola) )
    {
      Standard_Real aTol = (Tolerance < MAXTOLERANCEGEOM) ? Tolerance : MAXTOLERANCEGEOM;
      Standard_Real D = 0.;
      Standard_Boolean isOk = ProcessAnalyticalCurves2d(Curve,Point,D);
      if( !isOk ) return Standard_False;
      if( Abs(D) > aTol ) return Standard_False;

      if( KindOfCurve == STANDARD_TYPE (Geom2d_Line) )
	{
	  Handle(Geom2d_Line) aGL = Handle(Geom2d_Line)::DownCast(Curve);
	  gp_Lin2d aLin = aGL->Lin2d();
	  U = ElCLib::Parameter(aLin,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom2d_Circle) )
	{
	  Handle(Geom2d_Circle) aGC = Handle(Geom2d_Circle)::DownCast(Curve);
	  gp_Circ2d aCirc = aGC->Circ2d();
	  U = ElCLib::Parameter(aCirc,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom2d_Ellipse) )
	{
	  Handle(Geom2d_Ellipse) aGE = Handle(Geom2d_Ellipse)::DownCast(Curve);
	  gp_Elips2d anElips = aGE->Elips2d();
	  U = ElCLib::Parameter(anElips,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom2d_Parabola) )
	{
	  Handle(Geom2d_Parabola) aGP = Handle(Geom2d_Parabola)::DownCast(Curve);
	  gp_Parab2d aParab = aGP->Parab2d();
	  U = ElCLib::Parameter(aParab,Point);
	}
      else if( KindOfCurve == STANDARD_TYPE (Geom2d_Hyperbola) )
	{
	  Handle(Geom2d_Hyperbola) aGH = Handle(Geom2d_Hyperbola)::DownCast(Curve);
	  gp_Hypr2d aHypr = aGH->Hypr2d();
	  U = ElCLib::Parameter(aHypr,Point);
	}
      else return Standard_False;
    }
  // process parametrical curves
  else if( KindOfCurve == STANDARD_TYPE (Geom2d_BSplineCurve) ||
	   KindOfCurve == STANDARD_TYPE (Geom2d_BSplineCurve) ||
	   KindOfCurve == STANDARD_TYPE (Geom2d_TrimmedCurve) ||
	   KindOfCurve == STANDARD_TYPE (Geom2d_OffsetCurve) )
    {
      Standard_Real aTol = (Tolerance < MAXTOLERANCEPARM) ? Tolerance : MAXTOLERANCEPARM;
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
      if( iMin != 0 && Dist2Min <= aTol * aTol ) U = (extrema.Point(iMin)).Parameter();
      else return Standard_False;
    }
  else { return Standard_False; }
  
  return Standard_True;
}
