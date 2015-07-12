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


#include <Extrema_ExtPC.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GeomAPI_ProjectPointOnCurve
//purpose  : 
//=======================================================================
GeomAPI_ProjectPointOnCurve::GeomAPI_ProjectPointOnCurve()
{
  myIsDone = Standard_False;
}
//=======================================================================
//function : GeomAPI_ProjectPointOnCurve
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnCurve::GeomAPI_ProjectPointOnCurve
  (const gp_Pnt&             P, 
   const Handle(Geom_Curve)& Curve)
{
  Init(P,Curve);
}
//=======================================================================
//function : GeomAPI_ProjectPointOnCurve
//purpose  : 
//=======================================================================
  GeomAPI_ProjectPointOnCurve::GeomAPI_ProjectPointOnCurve
  (const gp_Pnt&             P, 
   const Handle(Geom_Curve)& Curve,
   const Standard_Real       Umin,
   const Standard_Real       Usup)
{
  Init(P,Curve,Umin,Usup);
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnCurve::Init
  (const gp_Pnt&             P,
   const Handle(Geom_Curve)& Curve)
{
  myC.Load(Curve);
/*
  Extrema_ExtPC theExtPC(P, myC);
  myExtPC = theExtPC;
*/
  myExtPC.Initialize(myC, myC.FirstParameter(), myC.LastParameter());
  myExtPC.Perform(P);
  
  myIsDone = myExtPC.IsDone() && ( myExtPC.NbExt() > 0);

  if ( myIsDone) {

    // evaluate the lower distance and its index;
    
    Standard_Real Dist2, Dist2Min = myExtPC.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtPC.NbExt(); i++) {
      Dist2 = myExtPC.SquareDistance(i);
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
  void GeomAPI_ProjectPointOnCurve::Init(const gp_Pnt&             P,
					 const Handle(Geom_Curve)& Curve,
					 const Standard_Real       Umin,
					 const Standard_Real       Usup )
{
  myC.Load(Curve,Umin,Usup);
/*
  Extrema_ExtPC theExtPC(P, myC);
  myExtPC = theExtPC;
*/
  myExtPC.Initialize(myC, myC.FirstParameter(), myC.LastParameter());
  myExtPC.Perform(P);
  
  myIsDone = myExtPC.IsDone() && ( myExtPC.NbExt() > 0);

  if ( myIsDone) {

    // evaluate the lower distance and its index;
    
    Standard_Real Dist2, Dist2Min = myExtPC.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtPC.NbExt(); i++) {
      Dist2 = myExtPC.SquareDistance(i);
      if ( Dist2 < Dist2Min) {
	Dist2Min = Dist2;
	myIndex = i;
      }
    }
  }
}
//modified by NIZNHY-PKV Wed Apr  3 17:48:51 2002f
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnCurve::Init  (const Handle(Geom_Curve)& Curve,
					   const Standard_Real       Umin,
					   const Standard_Real       Usup )
{
  myC.Load(Curve,Umin,Usup);
  //myExtPC = Extrema_ExtPC(P, myC);
  myExtPC.Initialize(myC, Umin, Usup);
  myIsDone = Standard_False;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void GeomAPI_ProjectPointOnCurve::Perform(const gp_Pnt& aP3D)
{
  myExtPC.Perform(aP3D);
  
  myIsDone=myExtPC.IsDone() && ( myExtPC.NbExt() > 0);
  if (myIsDone) {
    // evaluate the lower distance and its index;
    Standard_Real Dist2, Dist2Min = myExtPC.SquareDistance(1);
    myIndex = 1;
    
    for ( Standard_Integer i = 2; i <= myExtPC.NbExt(); i++) {
      Dist2 = myExtPC.SquareDistance(i);
      if ( Dist2 < Dist2Min) {
	Dist2Min = Dist2;
	myIndex = i;
      }
    }
  }
}
//modified by NIZNHY-PKV Wed Apr  3 17:48:53 2002t 
//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================
  Standard_Integer GeomAPI_ProjectPointOnCurve::NbPoints() const 
{
  if ( myIsDone )
    return myExtPC.NbExt();
  else
    return 0;
}
//=======================================================================
//function : Point
//purpose  : 
//=======================================================================
  gp_Pnt GeomAPI_ProjectPointOnCurve::Point(const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "GeomAPI_ProjectPointOnCurve::Point");
  return (myExtPC.Point(Index)).Value();
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ProjectPointOnCurve::Parameter
  (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "GeomAPI_ProjectPointOnCurve::Parameter");
  return (myExtPC.Point(Index)).Parameter();
}
//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void GeomAPI_ProjectPointOnCurve::Parameter
  (const Standard_Integer Index,
         Standard_Real&   U     ) const
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "GeomAPI_ProjectPointOnCurve::Parameter");
  U = (myExtPC.Point(Index)).Parameter();
}
//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================
Standard_Real GeomAPI_ProjectPointOnCurve::Distance
  (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if( Index < 1 || Index > NbPoints(),
			       "GeomAPI_ProjectPointOnCurve::Distance");
  return sqrt (myExtPC.SquareDistance(Index));
}
//=======================================================================
//function : NearestPoint
//purpose  : 
//=======================================================================

gp_Pnt GeomAPI_ProjectPointOnCurve::NearestPoint() const 
{
  StdFail_NotDone_Raise_if 
    ( !myIsDone, "GeomAPI_ProjectPointOnCurve::NearestPoint");
			    
  return (myExtPC.Point(myIndex)).Value();
}
//=======================================================================
//function : Standard_Integer
//purpose  : 
//=======================================================================

GeomAPI_ProjectPointOnCurve::operator Standard_Integer() const
{
  return NbPoints();
}
//=======================================================================
//function : gp_Pnt
//purpose  : 
//=======================================================================

GeomAPI_ProjectPointOnCurve::operator gp_Pnt() const
{
  return NearestPoint();
}
//=======================================================================
//function : LowerDistanceParameter
//purpose  : 
//=======================================================================

Standard_Real GeomAPI_ProjectPointOnCurve::LowerDistanceParameter() const
{
  StdFail_NotDone_Raise_if
    ( !myIsDone, "GeomAPI_ProjectPointOnCurve::LowerDistanceParameter");

  return (myExtPC.Point(myIndex)).Parameter();
}
//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================
Standard_Real GeomAPI_ProjectPointOnCurve::LowerDistance() const
{
  StdFail_NotDone_Raise_if
    ( !myIsDone, "GeomAPI_ProjectPointOnCurve::LowerDistance");

  return sqrt (myExtPC.SquareDistance(myIndex));
}
//=======================================================================
//function : operator
//purpose  : 
//=======================================================================
GeomAPI_ProjectPointOnCurve::operator Standard_Real() const
{
  return LowerDistance();
}
