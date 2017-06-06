// Created on: 1994-03-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Extrema_ExtCC.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//#include <Extrema_POnCurv.hxx>
//=======================================================================
//function : GeomAPI_ExtremaCurveCurve
//purpose  : 
//=======================================================================
GeomAPI_ExtremaCurveCurve::GeomAPI_ExtremaCurveCurve()
{
  myIsDone = Standard_False;
  myTotalExt = Standard_False;
}


//=======================================================================
//function : GeomAPI_ExtremaCurveCurve
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveCurve::GeomAPI_ExtremaCurveCurve
  (const Handle(Geom_Curve)& C1,
   const Handle(Geom_Curve)& C2)
{
  Init(C1,C2);
}


//=======================================================================
//function : GeomAPI_ExtremaCurveCurve
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveCurve::GeomAPI_ExtremaCurveCurve
  (const Handle(Geom_Curve)& C1,
   const Handle(Geom_Curve)& C2,
   const Standard_Real       U1min,
   const Standard_Real       U1max,
   const Standard_Real       U2min,
   const Standard_Real       U2max)
{
  Init(C1,C2,U1min,U1max,U2min,U2max);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveCurve::Init
  (const Handle(Geom_Curve)& C1,
   const Handle(Geom_Curve)& C2)
{

  myTotalExt = Standard_False;
  
  Standard_Real Tol = Precision::PConfusion();
  myC1.Load(C1);
  myC2.Load(C2);
  Extrema_ExtCC theExtCC(myC1, myC2, Tol,Tol);
  myExtCC = theExtCC;

  myIsDone = myExtCC.IsDone() && ( myExtCC.NbExt() > 0);

  if ( myIsDone) {

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
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveCurve::Init
  (const Handle(Geom_Curve)& C1,
   const Handle(Geom_Curve)& C2,
   const Standard_Real       U1min,
   const Standard_Real       U1max,
   const Standard_Real       U2min,
   const Standard_Real       U2max)
{
 
  myTotalExt = Standard_False;

  Standard_Real Tol = Precision::PConfusion();
  myC1.Load(C1);
  myC2.Load(C2);
  Extrema_ExtCC theExtCC(myC1,myC2,U1min,U1max,U2min,U2max,Tol,Tol);
  myExtCC = theExtCC;

  myIsDone = myExtCC.IsDone() && ( myExtCC.NbExt() > 0 );

  if ( myIsDone) {

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

Standard_Integer GeomAPI_ExtremaCurveCurve::NbExtrema() const 
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

void GeomAPI_ExtremaCurveCurve::Points
  (const Standard_Integer Index,
         gp_Pnt&          P1,
         gp_Pnt&          P2) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Points");

  Extrema_POnCurv PC1, PC2;
  myExtCC.Points(Index,PC1,PC2);

  P1 = PC1.Value();
  P2 = PC2.Value();
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveCurve::Parameters
  (const Standard_Integer Index, 
         Standard_Real&   U1,
         Standard_Real&   U2) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Parameters");

  Extrema_POnCurv PC1, PC2;
  myExtCC.Points(Index,PC1,PC2);

  U1 = PC1.Parameter();
  U2 = PC2.Parameter();
}


//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ExtremaCurveCurve::Distance
  (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Distance");

  return sqrt (myExtCC.SquareDistance(Index));
}


//=======================================================================
//function : NearestPoints
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveCurve::NearestPoints(gp_Pnt& P1, gp_Pnt& P2) const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ExtremaCurveCurve::NearestPoints");

  Points(myIndex,P1,P2);
}


//=======================================================================
//function : LowerDistanceParameters
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaCurveCurve::LowerDistanceParameters
  (Standard_Real& U1,
   Standard_Real& U2) const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ExtremaCurveCurve::LowerDistanceParameters");

  Parameters(myIndex,U1,U2);
}


//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ExtremaCurveCurve::LowerDistance() const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ExtremaCurveCurve::LowerDistance");

  return sqrt (myExtCC.SquareDistance(myIndex));
}


//=======================================================================
//function : Standard_Real
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveCurve::operator Standard_Real() const
{
  return LowerDistance();
}


//=======================================================================
//function : Standard_Integer
//purpose  : 
//=======================================================================

GeomAPI_ExtremaCurveCurve::operator Standard_Integer() const
{
  return myExtCC.NbExt();
}


Standard_Boolean GeomAPI_ExtremaCurveCurve::TotalNearestPoints(gp_Pnt& P1,gp_Pnt& P2) 
{

  if(!myTotalExt) {

    TotalPerform();
    myTotalExt = Standard_True;

  }

  if(myIsInfinite) return Standard_False;

  P1 = myTotalPoints[0];
  P2 = myTotalPoints[1];

  return Standard_True;
    
}

Standard_Boolean GeomAPI_ExtremaCurveCurve::TotalLowerDistanceParameters(Standard_Real& U1, 
									 Standard_Real& U2) 
{
  if(!myTotalExt) {

    TotalPerform();
    myTotalExt = Standard_True;

  }

  if(myIsInfinite) return Standard_False;

  U1 = myTotalPars[0];
  U2 = myTotalPars[1];

  return Standard_True;

}

Standard_Real GeomAPI_ExtremaCurveCurve::TotalLowerDistance() 
{
  if(!myTotalExt) {

    TotalPerform();
    myTotalExt = Standard_True;

  }

  return myTotalDist;

}


void GeomAPI_ExtremaCurveCurve::TotalPerform()

{
//  StdFail_NotDone_Raise_if
//    (!myExtCC.IsDone(), "GeomAPI_ExtremaCurveCurve::TotalPerform");

  Standard_Real u11 = myC1.FirstParameter();
  Standard_Real u12 = myC1.LastParameter();
  Standard_Real u21 = myC2.FirstParameter();
  Standard_Real u22 = myC2.LastParameter();
  
  Standard_Boolean infinite = Precision::IsInfinite(u11) &&
                              Precision::IsInfinite(u12) &&
			      Precision::IsInfinite(u21) &&
			      Precision::IsInfinite(u22);

  myIsInfinite = Standard_False;

  if(infinite && myExtCC.IsParallel()) {
    
    myIsInfinite = Standard_True;

    //calculate distance between any suitable point on C1 and C2
    
    gp_Pnt PonC1 = myC1.Value(0.);
    GeomAPI_ProjectPointOnCurve proj(PonC1, myC2.Curve());
    myTotalDist = proj.LowerDistance();

    return;

  }

  myTotalDist = RealLast();

  if(myIsDone && !myExtCC.IsParallel()) {
    
    Points(myIndex, myTotalPoints[0], myTotalPoints[1]);
    Parameters(myIndex, myTotalPars[0], myTotalPars[1]);
    myTotalDist = sqrt (myExtCC.SquareDistance(myIndex));

    if(myTotalDist <= Precision::Confusion()) return;

  }

  gp_Pnt P11, P12, P21, P22;
  Standard_Real d11, d12, d21, d22;
  myExtCC.TrimmedSquareDistances(d11, d12, d21, d22, P11, P12, P21, P22);

  Standard_Real aTotalDist2 = myTotalDist * myTotalDist;
  if(aTotalDist2  > d11) {
    myTotalDist = sqrt (d11);
    myTotalPoints[0] = P11;
    myTotalPoints[1] = P21;
    myTotalPars[0] = u11;
    myTotalPars[1] = u21;
    
    if(myTotalDist <= Precision::Confusion()) return;
    
  }
  
  if(aTotalDist2 > d12) {
    myTotalDist = sqrt (d12);
    myTotalPoints[0] = P11;
    myTotalPoints[1] = P22;
    myTotalPars[0] = u11;
    myTotalPars[1] = u22;
    
    if(myTotalDist <= Precision::Confusion()) return;
    
  }
  
  if(aTotalDist2 > d21) {
    myTotalDist = sqrt (d21);
    myTotalPoints[0] = P12;
    myTotalPoints[1] = P21;
    myTotalPars[0] = u12;
    myTotalPars[1] = u21;
    
    if(myTotalDist <= Precision::Confusion()) return;
    
  }
  
  if(aTotalDist2 > d22) {
    myTotalDist = sqrt (d22);
    myTotalPoints[0] = P12;
    myTotalPoints[1] = P22;
    myTotalPars[0] = u12;
    myTotalPars[1] = u22;
    
    if(myTotalDist <= Precision::Confusion()) return;
    
  }
 
  // calculate distances between extremities one curve and other curve

  if(!Precision::IsInfinite(u11)) {
    GeomAPI_ProjectPointOnCurve proj(P11, myC2.Curve(), u21, u22);  

    if(proj.NbPoints() > 0) {

      Standard_Real dmin = proj.LowerDistance();
      if(myTotalDist > dmin) {
	myTotalDist = dmin;
	myTotalPoints[0] = P11;
	myTotalPars[0] = u11;
	myTotalPoints[1] = proj.NearestPoint();
	myTotalPars[1] = proj.LowerDistanceParameter();

	if(myTotalDist <= Precision::Confusion()) return;
    
      }
    }
  }

  if(!Precision::IsInfinite(u12)) {
    GeomAPI_ProjectPointOnCurve proj(P12, myC2.Curve(), u21, u22);  

    if(proj.NbPoints() > 0) {

      Standard_Real dmin = proj.LowerDistance();
      if(myTotalDist > dmin) {
	myTotalDist = dmin;
	myTotalPoints[0] = P12;
	myTotalPars[0] = u12;
	myTotalPoints[1] = proj.NearestPoint();
	myTotalPars[1] = proj.LowerDistanceParameter();

	if(myTotalDist <= Precision::Confusion()) return;
    
      }
    }
  }

  if(!Precision::IsInfinite(u21)) {
    GeomAPI_ProjectPointOnCurve proj(P21, myC1.Curve(), u11, u12);  

    if(proj.NbPoints() > 0) {

      Standard_Real dmin = proj.LowerDistance();
      if(myTotalDist > dmin) {
	myTotalDist = dmin;
	myTotalPoints[0] = proj.NearestPoint();
	myTotalPars[0] = proj.LowerDistanceParameter();
	myTotalPoints[1] = P21;
	myTotalPars[1] = u21;

	if(myTotalDist <= Precision::Confusion()) return;
    
      }
    }
  }

  if(!Precision::IsInfinite(u22)) {
    GeomAPI_ProjectPointOnCurve proj(P22, myC1.Curve(), u11, u12);  

    if(proj.NbPoints() > 0) {

      Standard_Real dmin = proj.LowerDistance();
      if(myTotalDist > dmin) {
	myTotalDist = dmin;
	myTotalPoints[0] = proj.NearestPoint();
	myTotalPars[0] = proj.LowerDistanceParameter();
	myTotalPoints[1] = P22;
	myTotalPars[1] = u22;
    
      }
    }
  }


}
