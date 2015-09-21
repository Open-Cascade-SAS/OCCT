// Created on: 1994-03-18
// Created by: Jean Marc LACHAUME
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


#include <HatchGen_IntersectionPoint.hxx>

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
