// Created on: 1994-03-18
// Created by: Jean Marc LACHAUME
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



#include <HatchGen_IntersectionPoint.ixx>

//=======================================================================
// Function : HatchGen_IntersectionPoint
// Purpose  : Constructor
//=======================================================================

HatchGen_IntersectionPoint::HatchGen_IntersectionPoint () :
       myIndex  (0) ,
       myParam  (RealLast()) ,
       myPosit  (TopAbs_INTERNAL) ,
       myBefore (TopAbs_UNKNOWN) ,
       myAfter  (TopAbs_UNKNOWN) ,
       mySegBeg (Standard_False) ,
       mySegEnd (Standard_False)
{
}

//=======================================================================
// Function : SetIndex
// Purpose  : Sets the index of the supporting curve.
//=======================================================================

void HatchGen_IntersectionPoint::SetIndex (const Standard_Integer Index)
{
  myIndex = Index ;
}

//=======================================================================
// Function : Index
// Purpose  : Returns the index of the supporting curve.
//=======================================================================

Standard_Integer HatchGen_IntersectionPoint::Index () const
{
  return myIndex ;
}

//=======================================================================
// Function : SetParameter
// Purpose  : Sets the parameter on the curve.
//=======================================================================

void HatchGen_IntersectionPoint::SetParameter (const Standard_Real Parameter)
{
  myParam = Parameter ;
}

//=======================================================================
// Function : Parameter
// Purpose  : Returns the parameter on the curve.
//=======================================================================

Standard_Real HatchGen_IntersectionPoint::Parameter () const
{
  return myParam ;
}

//=======================================================================
// Function : SetPosition
// Purpose  : Sets the position of the point on the curve.
//=======================================================================

void HatchGen_IntersectionPoint::SetPosition (const TopAbs_Orientation Position)
{
  myPosit = Position ;
}

//=======================================================================
// Function : Position
// Purpose  : Returns the position of the point on the element.
//=======================================================================

TopAbs_Orientation HatchGen_IntersectionPoint::Position () const
{
  return myPosit ;
}

//=======================================================================
// Function : SetStateBefore
// Purpose  : Sets the transition state before the intersection.
//=======================================================================

void HatchGen_IntersectionPoint::SetStateBefore (const TopAbs_State State)
{
  myBefore = State ;
}

//=======================================================================
// Function : StateBefore
// Purpose  : Returns the transition state before the intersection.
//=======================================================================

TopAbs_State HatchGen_IntersectionPoint::StateBefore () const
{
  return myBefore ;
}

//=======================================================================
// Function : SetStateAfter
// Purpose  : Sets the transition state after the intersection.
//=======================================================================

void HatchGen_IntersectionPoint::SetStateAfter (const TopAbs_State State)
{
  myAfter = State ;
}

//=======================================================================
// Function : StateAfter
// Purpose  : Returns the transition state after the intersection.
//=======================================================================

TopAbs_State HatchGen_IntersectionPoint::StateAfter () const
{
  return myAfter ;
}

//=======================================================================
// Function : SetSegmentBeginning
// Purpose  : Sets the flag that the point is the beginning of a segment.
//=======================================================================

void HatchGen_IntersectionPoint::SetSegmentBeginning (const Standard_Boolean State)
{
  mySegBeg = State ;
}

//=======================================================================
// Function : SegmentBeginning
// Purpose  : Returns the flag that the point is the beginning of a
//            segment.
//=======================================================================

Standard_Boolean HatchGen_IntersectionPoint::SegmentBeginning () const
{
  return mySegBeg ;
}

//=======================================================================
// Function : SetSegmentEnd
// Purpose  : Sets the flag that the point is the end of a segment.
//=======================================================================

void HatchGen_IntersectionPoint::SetSegmentEnd (const Standard_Boolean State)
{
  mySegEnd = State ;
}

//=======================================================================
// Function : SegmentEnd
// Purpose  : Returns the flag that the point is the end of a segment.
//=======================================================================

Standard_Boolean HatchGen_IntersectionPoint::SegmentEnd () const
{
  return mySegEnd ;
}
