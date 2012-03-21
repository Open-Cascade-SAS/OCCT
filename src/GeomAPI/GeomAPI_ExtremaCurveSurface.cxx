// Created on: 1994-03-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <GeomAPI_ExtremaCurveSurface.ixx>

#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>

#include <Precision.hxx>


//=======================================================================
//function : GeomAPI_ExtremaCurveSurface
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveSurface::GeomAPI_ExtremaCurveSurface()
{
  myIsDone = Standard_False;
}


//=======================================================================
//function : GeomAPI_ExtremaCurveSurface
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveSurface::GeomAPI_ExtremaCurveSurface
  (const Handle(Geom_Curve)&   Curve,
   const Handle(Geom_Surface)& Surface)
{
  Init(Curve,Surface);
}


//=======================================================================
//function : GeomAPI_ExtremaCurveSurface
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveSurface::GeomAPI_ExtremaCurveSurface
  (const Handle(Geom_Curve)&   Curve,
   const Handle(Geom_Surface)& Surface,
   const Standard_Real         Wmin,
   const Standard_Real         Wmax,
   const Standard_Real         Umin,
   const Standard_Real         Umax,
   const Standard_Real         Vmin,
   const Standard_Real         Vmax)
{
  Init(Curve,Surface,Wmin,Wmax,Umin,Umax,Vmin,Vmax);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveSurface::Init
  (const Handle(Geom_Curve)&   Curve,
   const Handle(Geom_Surface)& Surface)
{
  GeomAdaptor_Curve   TheCurve   (Curve);
  GeomAdaptor_Surface TheSurface (Surface);

  Standard_Real Tol = Precision::PConfusion();
  Extrema_ExtCS theExtCS(TheCurve,TheSurface,Tol,Tol);
  myExtCS = theExtCS;

  myIsDone = myExtCS.IsDone() && ( myExtCS.NbExt() > 0);

  if ( myIsDone) {

    // evaluate the lower distance and its index;
    
    Standard_Real Dist2, Dist2Min = myExtCS.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtCS.NbExt(); i++) {
      Dist2 = myExtCS.SquareDistance(i);
      if ( Dist2 < Dist2Min) {
	Dist2Min = Dist2;
	myIndex = i;
      }
    }
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveSurface::Init
  (const Handle(Geom_Curve)&   Curve,
   const Handle(Geom_Surface)& Surface,
   const Standard_Real         Wmin,
   const Standard_Real         Wmax,
   const Standard_Real         Umin,
   const Standard_Real         Umax,
   const Standard_Real         Vmin,
   const Standard_Real         Vmax)
{
  GeomAdaptor_Curve   TheCurve   (Curve, Wmin, Wmax);
  GeomAdaptor_Surface TheSurface (Surface, Umin, Umax, Vmin, Vmax);

  Standard_Real Tol = Precision::PConfusion();
  Extrema_ExtCS theExtCS(TheCurve,TheSurface,
			      Wmin,Wmax,Umin,Umax,Vmin,Vmax,Tol,Tol);
  myExtCS = theExtCS;

  myIsDone = myExtCS.IsDone() && ( myExtCS.NbExt() > 0);

  if ( myIsDone) {

    // evaluate the lower distance and its index;
    
    Standard_Real Dist2, Dist2Min = myExtCS.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtCS.NbExt(); i++) {
      Dist2 = myExtCS.SquareDistance(i);
      if ( Dist2 < Dist2Min) {
	Dist2Min = Dist2;
	myIndex = i;
      }
    }
  }
}


//=======================================================================
//function : NbExtrema
//purpose  : 
//=======================================================================

Standard_Integer GeomAPI_ExtremaCurveSurface::NbExtrema() const 
{
  if ( myIsDone) 
    return myExtCS.NbExt();
  else
    return 0;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveSurface::Points
  (const Standard_Integer Index,
         gp_Pnt&          P1,
         gp_Pnt&          P2) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Points");

  Extrema_POnCurv PC;
  Extrema_POnSurf PS;
  myExtCS.Points(Index,PC,PS);

  P1 = PC.Value();
  P2 = PS.Value();
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveSurface::Parameters
  (const Standard_Integer Index,
         Standard_Real&   W, 
         Standard_Real&   U, 
         Standard_Real&   V) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Parameters");

  Extrema_POnCurv PC;
  Extrema_POnSurf PS;
  myExtCS.Points(Index,PC,PS);

  W = PC.Parameter();
  PS.Parameter(U,V);
}


//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ExtremaCurveSurface::Distance
  (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Distance");

  return sqrt (myExtCS.SquareDistance(Index));
}


//=======================================================================
//function : NearestPoints
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveSurface::NearestPoints(gp_Pnt& PC, gp_Pnt& PS) const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ExtremaCurveSurface::NearestPoints");

  Points(myIndex,PC,PS);
}


//=======================================================================
//function : LowerDistanceParameters
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveSurface::LowerDistanceParameters
  (Standard_Real& W, 
   Standard_Real& U, 
   Standard_Real& V) const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ExtremaCurveSurface::LowerDistanceParameters");

  Parameters(myIndex,W,U,V);
}


//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ExtremaCurveSurface::LowerDistance() const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ExtremaCurveSurface::LowerDistance");

  return sqrt (myExtCS.SquareDistance(myIndex));
}


//=======================================================================
//function : Standard_Integer
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveSurface::operator Standard_Integer() const
{
  return NbExtrema();
}


//=======================================================================
//function : Standard_Real
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveSurface::operator Standard_Real() const
{
  return LowerDistance();
}


