// Created on: 1993-03-09
// Created by: Remi LEQUETTE
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


#include <PTopoDS_TShape.ixx>

// static const Standard_Integer FreeMask       = 1;
// FreeMask has no sense in D.B. context; it is free for future use.
static const Standard_Integer ModifiedMask   = 2;
static const Standard_Integer CheckedMask    = 4;
static const Standard_Integer OrientableMask = 8;
static const Standard_Integer ClosedMask     = 16;
static const Standard_Integer InfiniteMask   = 32;
static const Standard_Integer ConvexMask     = 64;


//=======================================================================
//function : PTopoDS_TShape
//purpose  : 
//=======================================================================

PTopoDS_TShape::PTopoDS_TShape() :
       myFlags(0)
{
  // UPDATE FMA - 28-11-95
  // These two flags are always set to the Transient Value
  //Modified(Standard_True);
  //Orientable(Standard_True);
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape::Modified() const 
{
  return myFlags & ModifiedMask;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

void  PTopoDS_TShape::Modified(const Standard_Boolean M)
{
  if (M) myFlags |= ModifiedMask;
  else   myFlags &= ~ModifiedMask;
}

//=======================================================================
//function : Checked
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape::Checked() const 
{
  return myFlags & CheckedMask;
}

//=======================================================================
//function : Checked
//purpose  : 
//=======================================================================

void  PTopoDS_TShape::Checked(const Standard_Boolean M)
{
  if (M) myFlags |= CheckedMask;
  else   myFlags &= ~CheckedMask;
}

//=======================================================================
//function : Orientable
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape::Orientable() const 
{
  return myFlags & OrientableMask;
}

//=======================================================================
//function : Orientable
//purpose  : 
//=======================================================================

void  PTopoDS_TShape::Orientable(const Standard_Boolean M)
{
  if (M) myFlags |= OrientableMask;
  else   myFlags &= ~OrientableMask;
}

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape::Closed() const 
{
  return myFlags & ClosedMask;
}

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

void  PTopoDS_TShape::Closed(const Standard_Boolean M)
{
  if (M) myFlags |= ClosedMask;
  else   myFlags &= ~ClosedMask;
}

//=======================================================================
//function : Infinite
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape::Infinite() const 
{
  return myFlags & InfiniteMask;
}

//=======================================================================
//function : Infinite
//purpose  : 
//=======================================================================

void  PTopoDS_TShape::Infinite(const Standard_Boolean M)
{
  if (M) myFlags |= InfiniteMask;
  else   myFlags &= ~InfiniteMask;
}

//=======================================================================
//function : Convex
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape::Convex() const 
{
  return myFlags & ConvexMask;
}

//=======================================================================
//function : Convex
//purpose  : 
//=======================================================================

void  PTopoDS_TShape::Convex(const Standard_Boolean M)
{
  if (M) myFlags |= ConvexMask;
  else   myFlags &= ~ConvexMask;
}


//=======================================================================
//function : Shapes
//purpose  : returns the array of sub-shapes
//=======================================================================

Handle(PTopoDS_HArray1OfHShape) PTopoDS_TShape::Shapes() const
{
  return myShapes;
}

//=======================================================================
//function : Shapes
//purpose  : returns the Ith element of the array of sub-shapes
//=======================================================================

Handle(PTopoDS_HShape)
PTopoDS_TShape::Shapes(const Standard_Integer i) const
{
  return myShapes->Value(i);
}

//=======================================================================
//function : Shapes
//purpose  : Sets the array of sub-shapes
//=======================================================================

void PTopoDS_TShape::Shapes(const Handle(PTopoDS_HArray1OfHShape)& S)
{
  myShapes = S;
}

//=======================================================================
//function : Shapes
//purpose  : Sets the Ith element of the array of sub-shapes
//=======================================================================

void PTopoDS_TShape::Shapes(const Standard_Integer i,
			    const Handle(PTopoDS_HShape)& S)
{
  myShapes->SetValue(i, S);
}

