// Created on: 1993-10-29
// Created by: Jean Marc LACHAUME
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Standard_Stream.hxx>
#include <HatchGen_PointOnHatching.ixx>

#define RAISE_IF_NOSUCHOBJECT 0

//=======================================================================
// Function : HatchGen_PointOnHatching
// Purpose  : Constructor.
//=======================================================================

HatchGen_PointOnHatching::HatchGen_PointOnHatching () :
       HatchGen_IntersectionPoint () ,
       myPoints ()
{
}

//=======================================================================
// Function : HatchGen_PointOnHatching
// Purpose  : Constructor.
//=======================================================================

HatchGen_PointOnHatching::HatchGen_PointOnHatching (const HatchGen_PointOnHatching& Point)
{
  myIndex  = Point.myIndex ;
  myParam  = Point.myParam ;
  myPosit  = Point.myPosit ;
  myBefore = Point.myBefore ;
  myAfter  = Point.myAfter ;
  mySegBeg = Point.mySegBeg ;
  mySegEnd = Point.mySegEnd ;
  myPoints = Point.myPoints ;
}

//=======================================================================
// Function : HatchGen_PointOnHatching
// Purpose  : Constructor.
//=======================================================================

HatchGen_PointOnHatching::HatchGen_PointOnHatching (const IntRes2d_IntersectionPoint& Point)
{
  myIndex = 0 ;
  myParam = Point.ParamOnFirst() ;
  switch (Point.TransitionOfFirst().PositionOnCurve()) {
      case IntRes2d_Head   : myPosit = TopAbs_FORWARD  ; break ;
      case IntRes2d_Middle : myPosit = TopAbs_INTERNAL ; break ;
      case IntRes2d_End    : myPosit = TopAbs_REVERSED ; break ;
  }
  myBefore = TopAbs_UNKNOWN ;
  myAfter  = TopAbs_UNKNOWN ;
  mySegBeg = Standard_False ;
  mySegEnd = Standard_False ;
  myPoints.Clear() ;
}

void HatchGen_PointOnHatching::Delete()
{}

//=======================================================================
// Function : AddPoint
// Purpose  : Adds a point on element to the point.
//=======================================================================

void HatchGen_PointOnHatching::AddPoint (const HatchGen_PointOnElement& Point,
					 const Standard_Real Confusion)
{
  Standard_Integer NbPnt = myPoints.Length() ;
 // for (Standard_Integer IPnt = 1 ;
  Standard_Integer IPnt;
  for ( IPnt = 1 ;
       IPnt <= NbPnt && myPoints(IPnt).IsDifferent (Point, Confusion) ;
       IPnt++) ;
  if (IPnt > NbPnt) myPoints.Append (Point) ;
}


//=======================================================================
// Function : NbPoints
// Purpose  : Returns the number of elements intersecting the hatching at
//            this point.
//=======================================================================

Standard_Integer HatchGen_PointOnHatching::NbPoints () const
{
  return myPoints.Length() ;
}
  
//=======================================================================
// Function : Point
// Purpose  : Returns the Index-th point on element of the point.
//=======================================================================

const HatchGen_PointOnElement& HatchGen_PointOnHatching::Point (const Standard_Integer Index) const
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbPnt = myPoints.Length() ;
  Standard_OutOfRange_Raise_if (Index < 1 || Index > NbPnt, "") ;
#endif
  const HatchGen_PointOnElement& Point = myPoints.Value (Index) ;
  return Point ;
}
  
//=======================================================================
// Function : RemPoint
// Purpose  : Removes the Index-th point on element of the point..
//=======================================================================

void HatchGen_PointOnHatching::RemPoint (const Standard_Integer Index)
{
#if RAISE_IF_NOSUCHOBJECT
  Standard_Integer NbPnt = myPoints.Length() ;
  Standard_OutOfRange_Raise_if (Index < 1 || Index > NbPnt, "") ;
#endif
  myPoints.Remove (Index) ;
}
  
//=======================================================================
// Function : ClrPoints
// Purpose  : Removes all the points on element of the point.
//=======================================================================

void HatchGen_PointOnHatching::ClrPoints ()
{
  myPoints.Clear() ;
}

//=======================================================================
// Function : IsLower
// Purpose  : Tests if the point is lower than an other.
//=======================================================================

Standard_Boolean HatchGen_PointOnHatching::IsLower (const HatchGen_PointOnHatching& Point,
						    const Standard_Real Confusion) const
{
  return (Point.myParam - myParam > Confusion) ;
}

//=======================================================================
// Function : IsEqual
// Purpose  : Tests if the point is equal to an other.
//=======================================================================

Standard_Boolean HatchGen_PointOnHatching::IsEqual (const HatchGen_PointOnHatching& Point,
						    const Standard_Real Confusion) const
{
  return (Abs (Point.myParam - myParam) <= Confusion) ;
}

//=======================================================================
// Function : IsGreater
// Purpose  : Tests if the point is greater than an other.
//=======================================================================

Standard_Boolean HatchGen_PointOnHatching::IsGreater (const HatchGen_PointOnHatching& Point,
						      const Standard_Real Confusion) const
{
  return (myParam - Point.myParam > Confusion) ;
}

//=======================================================================
// Function : Dump
// Purpose  : Dump of the point.
//=======================================================================

void HatchGen_PointOnHatching::Dump (const Standard_Integer Index) const
{
  cout << "--- Point on hatching " ;
  if (Index > 0) {
    cout << "# " << setw(3) << Index << " " ;
  } else {
    cout << "------" ;
  }
  cout << "------------------" << endl ;

  cout << "    Index of the hatching = " << myIndex << endl ;
  cout << "    Parameter on hatching = " << myParam << endl ;
  cout << "    Position  on hatching = " ;
  switch (myPosit) {
      case TopAbs_FORWARD  : cout << "FORWARD  (i.e. BEGIN  )" ; break ;
      case TopAbs_INTERNAL : cout << "INTERNAL (i.e. MIDDLE )" ; break ;
      case TopAbs_REVERSED : cout << "REVERSED (i.e. END    )" ; break ;
      case TopAbs_EXTERNAL : cout << "EXTERNAL (i.e. UNKNOWN)" ; break ;
  }
  cout << endl ;
  cout << "    State Before          = " ;
  switch (myBefore) {
      case TopAbs_IN      : cout << "IN"      ; break ;
      case TopAbs_OUT     : cout << "OUT"     ; break ;
      case TopAbs_ON      : cout << "ON"      ; break ;
      case TopAbs_UNKNOWN : cout << "UNKNOWN" ; break ;
  }
  cout << endl ;
  cout << "    State After           = " ;
  switch (myAfter) {
      case TopAbs_IN      : cout << "IN"      ; break ;
      case TopAbs_OUT     : cout << "OUT"     ; break ;
      case TopAbs_ON      : cout << "ON"      ; break ;
      case TopAbs_UNKNOWN : cout << "UNKNOWN" ; break ;
  }
  cout << endl ;
  cout << "    Beginning of segment  = " << (mySegBeg ? "TRUE" : "FALSE") << endl ;
  cout << "    End       of segment  = " << (mySegEnd ? "TRUE" : "FALSE") << endl ;

  Standard_Integer NbPnt = myPoints.Length () ;
  if (NbPnt == 0) {
    cout << "    No points on element" << endl ;
  } else {
    cout << "    Contains " << NbPnt << " points on element" << endl ;
    for (Standard_Integer IPnt = 1 ; IPnt <= NbPnt ; IPnt++) {
      const HatchGen_PointOnElement& Point = myPoints.Value (IPnt) ;
      Point.Dump (IPnt) ;
    }
  }

  cout << "----------------------------------------------" << endl ;
}
