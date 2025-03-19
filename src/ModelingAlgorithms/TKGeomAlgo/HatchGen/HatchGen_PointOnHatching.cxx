// Created on: 1993-10-29
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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

#include <HatchGen_PointOnHatching.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Standard_Stream.hxx>

#define RAISE_IF_NOSUCHOBJECT 0

//=================================================================================================

HatchGen_PointOnHatching::HatchGen_PointOnHatching()
    : HatchGen_IntersectionPoint(),
      myPoints()
{
}

//=================================================================================================

HatchGen_PointOnHatching::HatchGen_PointOnHatching(const IntRes2d_IntersectionPoint& Point)
{
  myIndex = 0;
  myParam = Point.ParamOnFirst();
  switch (Point.TransitionOfFirst().PositionOnCurve())
  {
    case IntRes2d_Head:
      myPosit = TopAbs_FORWARD;
      break;
    case IntRes2d_Middle:
      myPosit = TopAbs_INTERNAL;
      break;
    case IntRes2d_End:
      myPosit = TopAbs_REVERSED;
      break;
  }
  myBefore = TopAbs_UNKNOWN;
  myAfter  = TopAbs_UNKNOWN;
  mySegBeg = Standard_False;
  mySegEnd = Standard_False;
  myPoints.Clear();
}

//=======================================================================
// Function : AddPoint
// Purpose  : Adds a point on element to the point.
//=======================================================================

void HatchGen_PointOnHatching::AddPoint(const HatchGen_PointOnElement& Point,
                                        const Standard_Real            Confusion)
{
  Standard_Integer NbPnt = myPoints.Length();
  // for (Standard_Integer IPnt = 1 ;
  Standard_Integer IPnt;
  for (IPnt = 1; IPnt <= NbPnt && myPoints(IPnt).IsDifferent(Point, Confusion); IPnt++)
    ;
  if (IPnt > NbPnt)
    myPoints.Append(Point);
}

//=======================================================================
// Function : NbPoints
// Purpose  : Returns the number of elements intersecting the hatching at
//            this point.
//=======================================================================

Standard_Integer HatchGen_PointOnHatching::NbPoints() const
{
  return myPoints.Length();
}

//=======================================================================
// Function : Point
// Purpose  : Returns the Index-th point on element of the point.
//=======================================================================

const HatchGen_PointOnElement& HatchGen_PointOnHatching::Point(const Standard_Integer Index) const
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbPnt = myPoints.Length();
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPnt, "");
#endif
  const HatchGen_PointOnElement& Point = myPoints.Value(Index);
  return Point;
}

//=======================================================================
// Function : RemPoint
// Purpose  : Removes the Index-th point on element of the point..
//=======================================================================

void HatchGen_PointOnHatching::RemPoint(const Standard_Integer Index)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbPnt = myPoints.Length();
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPnt, "");
#endif
  myPoints.Remove(Index);
}

//=======================================================================
// Function : ClrPoints
// Purpose  : Removes all the points on element of the point.
//=======================================================================

void HatchGen_PointOnHatching::ClrPoints()
{
  myPoints.Clear();
}

//=======================================================================
// Function : IsLower
// Purpose  : Tests if the point is lower than an other.
//=======================================================================

Standard_Boolean HatchGen_PointOnHatching::IsLower(const HatchGen_PointOnHatching& Point,
                                                   const Standard_Real             Confusion) const
{
  return (Point.myParam - myParam > Confusion);
}

//=======================================================================
// Function : IsEqual
// Purpose  : Tests if the point is equal to an other.
//=======================================================================

Standard_Boolean HatchGen_PointOnHatching::IsEqual(const HatchGen_PointOnHatching& Point,
                                                   const Standard_Real             Confusion) const
{
  return (Abs(Point.myParam - myParam) <= Confusion);
}

//=======================================================================
// Function : IsGreater
// Purpose  : Tests if the point is greater than an other.
//=======================================================================

Standard_Boolean HatchGen_PointOnHatching::IsGreater(const HatchGen_PointOnHatching& Point,
                                                     const Standard_Real Confusion) const
{
  return (myParam - Point.myParam > Confusion);
}

//=======================================================================
// Function : Dump
// Purpose  : Dump of the point.
//=======================================================================

void HatchGen_PointOnHatching::Dump(const Standard_Integer Index) const
{
  std::cout << "--- Point on hatching ";
  if (Index > 0)
  {
    std::cout << "# " << std::setw(3) << Index << " ";
  }
  else
  {
    std::cout << "------";
  }
  std::cout << "------------------" << std::endl;

  std::cout << "    Index of the hatching = " << myIndex << std::endl;
  std::cout << "    Parameter on hatching = " << myParam << std::endl;
  std::cout << "    Position  on hatching = ";
  switch (myPosit)
  {
    case TopAbs_FORWARD:
      std::cout << "FORWARD  (i.e. BEGIN  )";
      break;
    case TopAbs_INTERNAL:
      std::cout << "INTERNAL (i.e. MIDDLE )";
      break;
    case TopAbs_REVERSED:
      std::cout << "REVERSED (i.e. END    )";
      break;
    case TopAbs_EXTERNAL:
      std::cout << "EXTERNAL (i.e. UNKNOWN)";
      break;
  }
  std::cout << std::endl;
  std::cout << "    State Before          = ";
  switch (myBefore)
  {
    case TopAbs_IN:
      std::cout << "IN";
      break;
    case TopAbs_OUT:
      std::cout << "OUT";
      break;
    case TopAbs_ON:
      std::cout << "ON";
      break;
    case TopAbs_UNKNOWN:
      std::cout << "UNKNOWN";
      break;
  }
  std::cout << std::endl;
  std::cout << "    State After           = ";
  switch (myAfter)
  {
    case TopAbs_IN:
      std::cout << "IN";
      break;
    case TopAbs_OUT:
      std::cout << "OUT";
      break;
    case TopAbs_ON:
      std::cout << "ON";
      break;
    case TopAbs_UNKNOWN:
      std::cout << "UNKNOWN";
      break;
  }
  std::cout << std::endl;
  std::cout << "    Beginning of segment  = " << (mySegBeg ? "TRUE" : "FALSE") << std::endl;
  std::cout << "    End       of segment  = " << (mySegEnd ? "TRUE" : "FALSE") << std::endl;

  Standard_Integer NbPnt = myPoints.Length();
  if (NbPnt == 0)
  {
    std::cout << "    No points on element" << std::endl;
  }
  else
  {
    std::cout << "    Contains " << NbPnt << " points on element" << std::endl;
    for (Standard_Integer IPnt = 1; IPnt <= NbPnt; IPnt++)
    {
      const HatchGen_PointOnElement& Point = myPoints.Value(IPnt);
      Point.Dump(IPnt);
    }
  }

  std::cout << "----------------------------------------------" << std::endl;
}
