// Created on: 1994-03-24
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


#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>

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
//  If aSeg.IsOpposite() == TRUE
//
//                U1            U2
//    Curve 1:    *------------>*
//
//                V2            V1
//    Curve 2:    *<------------*
//
//    Segment:  FirstPoint--->LastPoint
//
//
//  If aSeg.IsOpposite() == FALSE
//
//                U1            U2
//    Curve 1:    *------------>*
//
//                V1            V2
//    Curve 2:    *------------>*
//
//    Segment:  FirstPoint--->LastPoint
//=======================================================================

void Geom2dAPI_InterCurveCurve::Segment
  (const Standard_Integer      theIndex,
         Handle(Geom2d_Curve)& theCurve1,
         Handle(Geom2d_Curve)& theCurve2) const 
{
  Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > NbSegments(),
                               "Geom2dAPI_InterCurveCurve::Segment");

  Standard_NullObject_Raise_if(myCurve1.IsNull(),
                               "Geom2dAPI_InterCurveCurve::Segment");

  Standard_Real aU1, aU2, aV1, aV2;
  aU1 = myCurve1->FirstParameter();
  aU2 = myCurve1->LastParameter();
  if (myCurve2.IsNull())
  {
    aV1 = aU1;
    aV2 = aU2;
  }
  else
  {
    aV1 = myCurve2->FirstParameter();
    aV2 = myCurve2->LastParameter();
  }

  const IntRes2d_IntersectionSegment& aSeg = myIntersector.Segment(theIndex);
  const Standard_Boolean isOpposite = aSeg.IsOpposite();

  if(aSeg.HasFirstPoint())
  {
    const IntRes2d_IntersectionPoint& anIPF = aSeg.FirstPoint();
    aU1 = anIPF.ParamOnFirst();
    
    if(isOpposite)
      aV2 = anIPF.ParamOnSecond();
    else
      aV1 = anIPF.ParamOnSecond();
  }

  if(aSeg.HasLastPoint())
  {
    const IntRes2d_IntersectionPoint& anIPL = aSeg.LastPoint();
    aU2 = anIPL.ParamOnFirst();

    if(isOpposite)
      aV1 = anIPL.ParamOnSecond();
    else
      aV2 = anIPL.ParamOnSecond();
  }

  theCurve1 = new Geom2d_TrimmedCurve(myCurve1, aU1, aU2);
  if (myCurve2.IsNull())
    theCurve2 = new Geom2d_TrimmedCurve(myCurve1, aV1, aV2);
  else
    theCurve2 = new Geom2d_TrimmedCurve(myCurve2, aV1, aV2);
}
