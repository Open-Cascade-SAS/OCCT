// Created on: 1994-03-24
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Geom2dAPI_InterCurveCurve.ixx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>

#include <Standard_NotImplemented.hxx>

//=======================================================================
//function : Geom2dAPI_InterCurveCurve
//purpose  : 
//=======================================================================

Geom2dAPI_InterCurveCurve::Geom2dAPI_InterCurveCurve()
{
  myIsDone = Standard_False;
}


//=======================================================================
//function : Geom2dAPI_InterCurveCurve
//purpose  : 
//=======================================================================

Geom2dAPI_InterCurveCurve::Geom2dAPI_InterCurveCurve
  (const Handle(Geom2d_Curve)& C1,
   const Handle(Geom2d_Curve)& C2,
   const Standard_Real         Tol)
{
  Init( C1, C2, Tol);
}


//=======================================================================
//function : Geom2dAPI_InterCurveCurve
//purpose  : 
//=======================================================================

Geom2dAPI_InterCurveCurve::Geom2dAPI_InterCurveCurve
  (const Handle(Geom2d_Curve)& C1,
   const Standard_Real         Tol)
{
  Init( C1, Tol);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_InterCurveCurve::Init
  (const Handle(Geom2d_Curve)& C1,
   const Handle(Geom2d_Curve)& C2,
   const Standard_Real         Tol)
{
  myCurve1 = Handle(Geom2d_Curve)::DownCast(C1->Copy());
  myCurve2 = Handle(Geom2d_Curve)::DownCast(C2->Copy());

  Geom2dAdaptor_Curve AC1(C1);
  Geom2dAdaptor_Curve AC2(C2);
  myIntersector = Geom2dInt_GInter( AC1, AC2, Tol, Tol);
  myIsDone = myIntersector.IsDone();

}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_InterCurveCurve::Init
  (const Handle(Geom2d_Curve)& C1,
   const Standard_Real         Tol)
{
  myCurve1 = Handle(Geom2d_Curve)::DownCast(C1->Copy());
  myCurve2.Nullify();

  Geom2dAdaptor_Curve AC1(C1);
  myIntersector = Geom2dInt_GInter( AC1, Tol, Tol);
  myIsDone = myIntersector.IsDone();

}


//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAPI_InterCurveCurve::NbPoints() const 
{
  if ( myIsDone)  
    return myIntersector.NbPoints();
  else
    return 0;
}


//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_Pnt2d Geom2dAPI_InterCurveCurve::Point
  (const Standard_Integer Index) const 
{
  Standard_OutOfRange_Raise_if(Index < 0 || Index > NbPoints(),
			       "Geom2dAPI_InterCurveCurve::Points");

  return (myIntersector.Point(Index)).Value();
}


//=======================================================================
//function : NbSegments
//purpose  : 
//=======================================================================

Standard_Integer Geom2dAPI_InterCurveCurve::NbSegments() const 
{
  if ( myIsDone)
    return myIntersector.NbSegments();
  else
    return 0;
}


//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

void Geom2dAPI_InterCurveCurve::Segment
  (const Standard_Integer      Index,
         Handle(Geom2d_Curve)& Curve1,
         Handle(Geom2d_Curve)& Curve2) const 
{
  Standard_OutOfRange_Raise_if(Index < 0 || Index > NbSegments(),
			       "Geom2dAPI_InterCurveCurve::Segment");

  Standard_NullObject_Raise_if(myCurve2.IsNull(),
			       "Geom2dAPI_InterCurveCurve::Segment");

  Standard_Real U1, U2, V1, V2;

  IntRes2d_IntersectionSegment Seg = myIntersector.Segment(Index);
  if ( Seg.IsOpposite()) {
    if ( Seg.HasFirstPoint()) {
      IntRes2d_IntersectionPoint IP1 = Seg.FirstPoint();
      U1 = IP1.ParamOnFirst();
      V2 = IP1.ParamOnSecond();
    }
    else {
      U1 = Curve1->FirstParameter();
      V2 = Curve2->LastParameter();
    }
    if ( Seg.HasLastPoint()) {
      IntRes2d_IntersectionPoint IP2 = Seg.LastPoint();
      U2 = IP2.ParamOnFirst();
      V1 = IP2.ParamOnSecond();
    }
    else {
      U2 = Curve1->FirstParameter();
      V1 = Curve2->LastParameter();
    }
  }
  else {
    if ( Seg.HasFirstPoint()) {
      IntRes2d_IntersectionPoint IP1 = Seg.FirstPoint();
      U1 = IP1.ParamOnFirst();
      V1 = IP1.ParamOnSecond();
    }
    else {
      U1 = Curve1->FirstParameter();
      V1 = Curve2->FirstParameter();
    }
    if ( Seg.HasLastPoint()) {
      IntRes2d_IntersectionPoint IP2 = Seg.LastPoint();
      U2 = IP2.ParamOnFirst();
      V2 = IP2.ParamOnSecond();
    }
    else {
      U2 = Curve1->FirstParameter();
      V2 = Curve2->FirstParameter();
    }
  }

  Curve1 = new Geom2d_TrimmedCurve(myCurve1, U1, U2);
  Curve2 = new Geom2d_TrimmedCurve(myCurve2, V1, V2);

}


//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

void Geom2dAPI_InterCurveCurve::Segment (const Standard_Integer Index,
                                         Handle(Geom2d_Curve)&) const 
{
  Standard_NotImplemented::Raise(" ");
  (void)Index; // avoid compiler warning in Release mode
  Standard_OutOfRange_Raise_if(Index < 0 || Index > NbSegments(),
			       "Geom2dAPI_InterCurveCurve::Segment");
}
