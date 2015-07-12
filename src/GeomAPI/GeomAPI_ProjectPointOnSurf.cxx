// Created on: 1994-03-17
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


#include <Extrema_ExtPS.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GeomAPI_ProjectPointOnSurf
//purpose  : 
//=======================================================================
GeomAPI_ProjectPointOnSurf::GeomAPI_ProjectPointOnSurf() 
: myIsDone (Standard_False) { }


//=======================================================================
//function : GeomAPI_ProjectPointOnSurf
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf::GeomAPI_ProjectPointOnSurf (const gp_Pnt&               P, 
							  const Handle(Geom_Surface)& Surface,
							  const Extrema_ExtAlgo   theProjAlgo)
{ 
  Init (P, Surface, theProjAlgo); 
}
//=======================================================================
//function : GeomAPI_ProjectPointOnSurf
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf::GeomAPI_ProjectPointOnSurf (const gp_Pnt&               P, 
							  const Handle(Geom_Surface)& Surface,
							  const Standard_Real         Tolerance,
							  const Extrema_ExtAlgo       theProjAlgo)
{ 
  Init (P, Surface, Tolerance, theProjAlgo); 
}
//=======================================================================
//function : GeomAPI_ProjectPointOnSurf
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf::GeomAPI_ProjectPointOnSurf(const gp_Pnt&               P, 
							 const Handle(Geom_Surface)& Surface,
							 const Standard_Real         Umin,
							 const Standard_Real         Usup,
							 const Standard_Real         Vmin,
							 const Standard_Real         Vsup,
							 const Extrema_ExtAlgo       theProjAlgo)

{ 
  Init (P, Surface, Umin, Usup, Vmin, Vsup, theProjAlgo); 
}
//=======================================================================
//function : GeomAPI_ProjectPointOnSurf
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf::GeomAPI_ProjectPointOnSurf  (const gp_Pnt&               P, 
							   const Handle(Geom_Surface)& Surface,
							   const Standard_Real         Umin,
							   const Standard_Real         Usup,
							   const Standard_Real         Vmin,
							   const Standard_Real         Vsup,
							   const Standard_Real         Tolerance,
							   const Extrema_ExtAlgo       theProjAlgo)

{ 
  Init (P, Surface, Umin, Usup, Vmin, Vsup, Tolerance, theProjAlgo); 
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Init ()
{
  myIsDone = myExtPS.IsDone() && ( myExtPS.NbExt() > 0);

  if ( myIsDone) {
    // evaluate the lower distance and its index;
    Standard_Real Dist2, Dist2Min = myExtPS.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtPS.NbExt(); i++) {
      Dist2 = myExtPS.SquareDistance(i);
      if (Dist2 < Dist2Min) {
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
  void GeomAPI_ProjectPointOnSurf::Init (const gp_Pnt&               P,
					 const Handle(Geom_Surface)& Surface,
					 const Extrema_ExtAlgo   theProjAlgo)

{ 
  Init (P, Surface, Precision::Confusion(), theProjAlgo); 
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Init(const gp_Pnt&               P,
					const Handle(Geom_Surface)& Surface,
					const Standard_Real         Tolerance,
					const Extrema_ExtAlgo       theProjAlgo)

{
  //modified by NIZNHY-PKV Thu Apr  4 10:37:55 2002 f
  //GeomAdaptor_Surface TheSurface (Surface);
  //myExtPS = Extrema_ExtPS (P, TheSurface, Tolerance, Tolerance);
  
  //modified by NIZNHY-PKV Mon Apr  8 11:13:37 2002 f XXX
  Standard_Real Umin, Usup, Vmin, Vsup;
  Surface->Bounds(Umin, Usup, Vmin, Vsup);
  myGeomAdaptor.Load(Surface, Umin, Usup, Vmin, Vsup);
  //
  //myExtPS = Extrema_ExtPS();
  myExtPS.SetAlgo(theProjAlgo);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  myExtPS.Perform(P);
  //XXXmyExtPS = Extrema_ExtPS (P, myGeomAdaptor, Tolerance, Tolerance);
  //modified by NIZNHY-PKV Mon Apr  8 11:13:44 2002 t XXX
  
  //modified by NIZNHY-PKV Thu Apr  4 10:37:58 2002 t
  Init ();
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Init (const gp_Pnt&               P,
					 const Handle(Geom_Surface)& Surface,
					 const Standard_Real         Umin,
					 const Standard_Real         Usup,
					 const Standard_Real         Vmin,
					 const Standard_Real         Vsup,
					 const Extrema_ExtAlgo       theProjAlgo)
{
  Standard_Real Tolerance = Precision::PConfusion();
  //modified by NIZNHY-PKV Thu Apr  4 10:38:23 2002 f
  //GeomAdaptor_Surface TheSurface (Surface,Umin,Usup,Vmin,Vsup);
  //myExtPS = Extrema_ExtPS (P, TheSurface, Tol, Tol);
  myGeomAdaptor.Load(Surface, Umin,Usup,Vmin,Vsup);
  //myExtPS = Extrema_ExtPS();
  myExtPS.SetAlgo(theProjAlgo);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  myExtPS.Perform(P);
  //XXX myExtPS = Extrema_ExtPS (P, myGeomAdaptor, Tol, Tol);
  //modified by NIZNHY-PKV Thu Apr  4 10:38:30 2002 t
  Init ();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Init (const gp_Pnt&               P,
					 const Handle(Geom_Surface)& Surface,
					 const Standard_Real         Umin,
					 const Standard_Real         Usup,
					 const Standard_Real         Vmin,
					 const Standard_Real         Vsup,
					 const Standard_Real         Tolerance,
					 const Extrema_ExtAlgo       theProjAlgo)
{
  //modified by NIZNHY-PKV Thu Apr  4 10:39:10 2002 f
  //GeomAdaptor_Surface TheSurface (Surface,Umin,Usup,Vmin,Vsup);
  //myExtPS = Extrema_ExtPS (P, TheSurface, Tolerance, Tolerance);
  myGeomAdaptor.Load(Surface, Umin,Usup,Vmin,Vsup);
  //myExtPS = Extrema_ExtPS();
  myExtPS.SetAlgo(theProjAlgo);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  myExtPS.Perform(P);
  //XXX myExtPS = Extrema_ExtPS (P, myGeomAdaptor, Tolerance, Tolerance);
  //modified by NIZNHY-PKV Thu Apr  4 10:39:14 2002 t
  Init ();
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Init (const Handle(Geom_Surface)& Surface,
					 const Standard_Real       Umin,
					 const Standard_Real       Usup,
					 const Standard_Real       Vmin,
					 const Standard_Real       Vsup,
					 const Extrema_ExtAlgo     theProjAlgo)
{
  Standard_Real Tolerance = Precision::PConfusion();
  //modified by NIZNHY-PKV Thu Apr  4 10:41:50 2002 f
  //GeomAdaptor_Surface TheSurface (Surface,Umin,Usup,Vmin,Vsup);
  myGeomAdaptor.Load(Surface, Umin,Usup,Vmin,Vsup);
  //modified by NIZNHY-PKV Thu Apr  4 10:42:29 2002 t
  //myExtPS = Extrema_ExtPS();
  //modified by NIZNHY-PKV Thu Apr  4 10:42:32 2002 f
  //myExtPS.Initialize(TheSurface, Umin, Usup, Vmin, Vsup, Tol, Tol);
  myExtPS.SetAlgo(theProjAlgo);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  //modified by NIZNHY-PKV Thu Apr  4 10:42:39 2002 t
  myIsDone = Standard_False;
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Init (const Handle(Geom_Surface)& Surface,
					 const Standard_Real         Umin,
					 const Standard_Real         Usup,
					 const Standard_Real         Vmin,
					 const Standard_Real         Vsup, 
					 const Standard_Real         Tolerance,
					 const Extrema_ExtAlgo       theProjAlgo)
{
  //modified by NIZNHY-PKV Thu Apr  4 10:43:00 2002 f
  //GeomAdaptor_Surface TheSurface (Surface,Umin,Usup,Vmin,Vsup);
  myGeomAdaptor.Load(Surface, Umin,Usup,Vmin,Vsup);
  //modified by NIZNHY-PKV Thu Apr  4 10:43:16 2002 t
  //myExtPS = Extrema_ExtPS();
  //modified by NIZNHY-PKV Thu Apr  4 10:43:18 2002 f
  //myExtPS.Initialize(TheSurface, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  myExtPS.SetAlgo(theProjAlgo);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  //modified by NIZNHY-PKV Thu Apr  4 10:43:26 2002 t
  myIsDone = Standard_False;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Perform(const gp_Pnt& P)
{
  myExtPS.Perform(P);
  Init ();
}
//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================
  Standard_Boolean GeomAPI_ProjectPointOnSurf::IsDone () const 
{ 
  return myIsDone; 
}
//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================
  Standard_Integer GeomAPI_ProjectPointOnSurf::NbPoints() const 
{
  if ( myIsDone){
    return myExtPS.NbExt();
  }
  else{
    return 0;
  }
}
//=======================================================================
//function : Point
//purpose  : 
//=======================================================================
  gp_Pnt GeomAPI_ProjectPointOnSurf::Point(const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "GeomAPI_ProjectPointOnSurf::Point");
  return (myExtPS.Point(Index)).Value();
}
//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::Parameters(const Standard_Integer Index,
					      Standard_Real&   U,
					      Standard_Real&   V) const
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "GeomAPI_ProjectPointOnSurf::Parameter");
  (myExtPS.Point(Index)).Parameter(U,V);
}
//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================
  Standard_Real GeomAPI_ProjectPointOnSurf::Distance  (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "GeomAPI_ProjectPointOnSurf::Distance");
  return sqrt (myExtPS.SquareDistance(Index));
}
//=======================================================================
//function : NearestPoint
//purpose  : 
//=======================================================================
  gp_Pnt GeomAPI_ProjectPointOnSurf::NearestPoint() const 
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ProjectPointOnSurf::NearestPoint");

  return (myExtPS.Point(myIndex)).Value();
}
//=======================================================================
//function : Standard_Integer
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf::operator Standard_Integer() const
{
  return NbPoints();
}
//=======================================================================
//function : gp_Pnt
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf::operator gp_Pnt() const
{
  return NearestPoint();
}
//=======================================================================
//function : LowerDistanceParameters
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnSurf::LowerDistanceParameters (Standard_Real& U,
							    Standard_Real& V ) const
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ProjectPointOnSurf::LowerDistanceParameters");

  (myExtPS.Point(myIndex)).Parameter(U,V);
}
//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================
  Standard_Real GeomAPI_ProjectPointOnSurf::LowerDistance() const
{
  StdFail_NotDone_Raise_if
    (!myIsDone, "GeomAPI_ProjectPointOnSurf::LowerDistance");

  return sqrt (myExtPS.SquareDistance(myIndex));
}
//=======================================================================
//function : Standard_Real
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnSurf::operator Standard_Real() const
{
  return LowerDistance();
}








