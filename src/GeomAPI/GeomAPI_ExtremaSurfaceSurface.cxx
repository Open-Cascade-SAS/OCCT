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


#include <Extrema_ExtSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GeomAPI_ExtremaSurfaceSurface
//purpose  : 
//=======================================================================
GeomAPI_ExtremaSurfaceSurface::GeomAPI_ExtremaSurfaceSurface()
{
  myIsDone = Standard_False;
}


//=======================================================================
//function : GeomAPI_ExtremaSurfaceSurface
//purpose  : 
//=======================================================================

GeomAPI_ExtremaSurfaceSurface::GeomAPI_ExtremaSurfaceSurface
  (const Handle(Geom_Surface)& S1,
   const Handle(Geom_Surface)& S2)
{
  Init(S1,S2);
}


//=======================================================================
//function : GeomAPI_ExtremaSurfaceSurface
//purpose  : 
//=======================================================================

GeomAPI_ExtremaSurfaceSurface::GeomAPI_ExtremaSurfaceSurface
  (const Handle(Geom_Surface)& S1,
   const Handle(Geom_Surface)& S2,
   const Standard_Real         U1min,
   const Standard_Real         U1max,
   const Standard_Real         V1min,
   const Standard_Real         V1max,
   const Standard_Real         U2min,
   const Standard_Real         U2max,
   const Standard_Real         V2min,
   const Standard_Real         V2max)
{
  Init(S1,S2,U1min,U1max,V1min,V1max,U2min,U2max,V2min,V2max);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaSurfaceSurface::Init
  (const Handle(Geom_Surface)& S1,
   const Handle(Geom_Surface)& S2)
{
  GeomAdaptor_Surface TheSurface1 (S1);
  GeomAdaptor_Surface TheSurface2 (S2);

  Standard_Real Tol = Precision::PConfusion();
  
  Extrema_ExtSS theExtSS(TheSurface1, TheSurface2,Tol,Tol);
  myExtSS = theExtSS;

  myIsDone = myExtSS.IsDone() && ( myExtSS.NbExt() > 0);

  if ( myIsDone) {

    // evaluate the lower distance and its index;
    
    Standard_Real Dist2, Dist2Min = myExtSS.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtSS.NbExt(); i++) {
      Dist2 = myExtSS.SquareDistance(i);
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

void GeomAPI_ExtremaSurfaceSurface::Init
  (const Handle(Geom_Surface)& S1,
   const Handle(Geom_Surface)& S2, 
   const Standard_Real         U1min,
   const Standard_Real         U1max,
   const Standard_Real         V1min,
   const Standard_Real         V1max,
   const Standard_Real         U2min,
   const Standard_Real         U2max,
   const Standard_Real         V2min,
   const Standard_Real         V2max)
{
  GeomAdaptor_Surface TheSurface1 (S1,U1min,U1max,V1min,V1max);
  GeomAdaptor_Surface TheSurface2 (S2,U2min,U2max,V2min,V2max);

  Standard_Real Tol = Precision::PConfusion();
  Extrema_ExtSS theExtSS(TheSurface1, TheSurface2,
			  U1min,U1max,V1min,V1max,
			  U2min,U2max,V2min,V2max,
			  Tol,Tol);

  myExtSS = theExtSS;
  myIsDone = myExtSS.IsDone() && ( myExtSS.NbExt() > 0);

  if ( myIsDone) {

    // evaluate the lower distance and its index;
    
    Standard_Real Dist2, Dist2Min = myExtSS.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtSS.NbExt(); i++) {
      Dist2 = myExtSS.SquareDistance(i);
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

Standard_Integer GeomAPI_ExtremaSurfaceSurface::NbExtrema() const 
{
  if ( myIsDone) 
    return myExtSS.NbExt();
  else
    return 0;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaSurfaceSurface::Points
  (const Standard_Integer Index,
         gp_Pnt&          P1,
         gp_Pnt&          P2) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Points");

  Extrema_POnSurf PS1,PS2;
  myExtSS.Points(Index,PS1,PS2);

  P1 = PS1.Value();
  P2 = PS2.Value();
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaSurfaceSurface::Parameters
  (const Standard_Integer Index,
         Standard_Real&   U1,
         Standard_Real&   V1,
         Standard_Real&   U2,
         Standard_Real&   V2) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Parameters");

  Extrema_POnSurf PS1,PS2;
  myExtSS.Points(Index,PS1,PS2);

  PS1.Parameter(U1,V1);
  PS2.Parameter(U2,V2);
}


//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ExtremaSurfaceSurface::Distance
  (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbExtrema(),
			       "GeomAPI_ExtremaCurveCurve::Distance");

  return sqrt (myExtSS.SquareDistance(Index));
}


//=======================================================================
//function : NearestPoints
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaSurfaceSurface::NearestPoints
  (gp_Pnt& P1, 
   gp_Pnt& P2) const 
{
  StdFail_NotDone_Raise_if
    ( !myIsDone, "GeomAPI_ExtremaSurfaceSurface::NearestPoints");

  Points(myIndex,P1,P2);
}


//=======================================================================
//function : LowerDistanceParameters
//purpose  : 
//=======================================================================

void GeomAPI_ExtremaSurfaceSurface::LowerDistanceParameters
  (Standard_Real& U1,
   Standard_Real& V1,
   Standard_Real& U2,
   Standard_Real& V2) const 
{
  StdFail_NotDone_Raise_if
    ( !myIsDone, "GeomAPI_ExtremaSurfaceSurface::LowerDistanceParameters");

  Parameters(myIndex,U1,V1,U2,V2);
}


//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ExtremaSurfaceSurface::LowerDistance() const 
{
  StdFail_NotDone_Raise_if
    ( !myIsDone, "GeomAPI_ExtremaSurfaceSurface::LowerDistance");

  return sqrt (myExtSS.SquareDistance(myIndex));
}


//=======================================================================
//function : Standard_Real
//purpose  : 
//=======================================================================

GeomAPI_ExtremaSurfaceSurface::operator Standard_Real() const
{
  return LowerDistance();
}


//=======================================================================
//function : Standard_Integer
//purpose  : 
//=======================================================================

GeomAPI_ExtremaSurfaceSurface::operator Standard_Integer() const
{
  return NbExtrema();
}



