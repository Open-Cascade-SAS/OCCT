// Created on: 1994-03-23
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



#include <Geom2dAPI_ExtremaCurveCurve.ixx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Extrema_POnCurv2d.hxx>

#include <Precision.hxx>

//=======================================================================
//function : Geom2dAPI_ExtremaCurveCurve
//purpose  : 
//=======================================================================

//Geom2dAPI_ExtremaCurveCurve::Geom2dAPI_ExtremaCurveCurve()
//{
//}


//=======================================================================
//function : Geom2dAPI_ExtremaCurveCurve
//purpose  : 
//=======================================================================

Geom2dAPI_ExtremaCurveCurve::Geom2dAPI_ExtremaCurveCurve
  (const Handle(Geom2d_Curve)& C1,
   const Handle(Geom2d_Curve)& C2,
   const Standard_Real         U1min,
   const Standard_Real         U1max,
   const Standard_Real         U2min,
   const Standard_Real         U2max)
{
  myC1.Load(C1, U1min, U1max);
  myC2.Load(C2, U2min, U2max);
  Extrema_ExtCC2d theExtCC( myC1, myC2 );

  myExtCC = theExtCC;

  myIsDone = myExtCC.IsDone() && ( myExtCC.NbExt() > 0);


  if ( myIsDone ) {
    // evaluate the lower distance and its index;
    
    Standard_Real Dist2, Dist2Min = myExtCC.SquareDistance(1);
    myIndex = 1;

    for ( Standard_Integer i = 2; i <= myExtCC.NbExt(); i++) {
      Dist2 = myExtCC.SquareDistance(i);
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

Standard_Integer Geom2dAPI_ExtremaCurveCurve::NbExtrema() const 
{
  if ( myIsDone)
    return myExtCC.NbExt();
  else
    return 0;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void Geom2dAPI_ExtremaCurveCurve::Points
  (const Standard_Integer Index,
         gp_Pnt2d&        P1,
         gp_Pnt2d&        P2) const 
{
  Standard_OutOfRange_Raise_if
    (Index<1||Index>NbExtrema(), "Geom2dAPI_ExtremaCurveCurve::Points");

  Extrema_POnCurv2d PC1, PC2;
  myExtCC.Points(Index,PC1,PC2);

  P1 = PC1.Value();
  P2 = PC2.Value();
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void Geom2dAPI_ExtremaCurveCurve::Parameters
  (const Standard_Integer Index, 
         Standard_Real&   U1,
         Standard_Real&   U2) const 
{
  Standard_OutOfRange_Raise_if
    (Index<1||Index>NbExtrema(), "Geom2dAPI_ExtremaCurveCurve::Parameters");

  Extrema_POnCurv2d PC1, PC2;
  myExtCC.Points(Index,PC1,PC2);

  U1 = PC1.Parameter();
  U2 = PC2.Parameter();
}


//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================

Standard_Real Geom2dAPI_ExtremaCurveCurve::Distance
  (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if
    (Index<1||Index>NbExtrema(), "Geom2dAPI_ExtremaCurveCurve:Distance");

  return sqrt (myExtCC.SquareDistance(Index));
}


//=======================================================================
//function : NearestPoints
//purpose  : 
//=======================================================================

void Geom2dAPI_ExtremaCurveCurve::NearestPoints
  (gp_Pnt2d& P1, gp_Pnt2d& P2) const 
{
  StdFail_NotDone_Raise_if
    ( !myIsDone, "Geom2dAPI_ExtremaCurveCurve:NearestPoints");

  Points(myIndex,P1,P2);
}


//=======================================================================
//function : LowerDistanceParameters
//purpose  : 
//=======================================================================

void Geom2dAPI_ExtremaCurveCurve::LowerDistanceParameters
  (Standard_Real& U1,
   Standard_Real& U2) const 
{
  StdFail_NotDone_Raise_if
    ( !myIsDone, "Geom2dAPI_ExtremaCurveCurve:LowerDistanceParameters");

  Parameters(myIndex,U1,U2);
}


//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================

Standard_Real Geom2dAPI_ExtremaCurveCurve::LowerDistance() const
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "Geom2dAPI_ExtremaCurveCurve:LowerDistance");

  return sqrt (myExtCC.SquareDistance(myIndex));
}


//=======================================================================
//function : Standard_Real
//purpose  : 
//=======================================================================

Geom2dAPI_ExtremaCurveCurve::operator Standard_Real() const
{
  return LowerDistance();
}


//=======================================================================
//function : Standard_Integer
//purpose  : 
//=======================================================================

Geom2dAPI_ExtremaCurveCurve::operator Standard_Integer() const
{
  return myExtCC.NbExt();
}
