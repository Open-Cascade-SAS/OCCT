// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
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

//      	-------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb  4 1998	Creation



#include <PTopoDS_TShape1.ixx>

// Enum terms are better than statics, who need to be initialized.
enum {
  PTopoDS_FreeMask       = 1,
  // FreeMask has no sense in D.B. context; it is free for future use.
  PTopoDS_ModifiedMask   = 2,
  PTopoDS_CheckedMask    = 4,
  PTopoDS_OrientableMask = 8,
  PTopoDS_ClosedMask     = 16,
  PTopoDS_InfiniteMask   = 32,
  PTopoDS_ConvexMask     = 64
};


//=======================================================================
//function : PTopoDS_TShape1
//purpose  : 
//=======================================================================

PTopoDS_TShape1::PTopoDS_TShape1()
: myFlags(0)
     // These two flags are always set to the Transient Value
     //Modified(Standard_True);
     //Orientable(Standard_True);
{}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape1::Modified() const 
{ return myFlags & PTopoDS_ModifiedMask; }

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

void  PTopoDS_TShape1::Modified(const Standard_Boolean M)
{
  if (M) myFlags |= PTopoDS_ModifiedMask;
  else   myFlags &= ~PTopoDS_ModifiedMask;
}

//=======================================================================
//function : Checked
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape1::Checked() const 
{ return myFlags & PTopoDS_CheckedMask; }

//=======================================================================
//function : Checked
//purpose  : 
//=======================================================================

void  PTopoDS_TShape1::Checked(const Standard_Boolean M)
{
  if (M) myFlags |= PTopoDS_CheckedMask;
  else   myFlags &= ~PTopoDS_CheckedMask;
}

//=======================================================================
//function : Orientable
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape1::Orientable() const 
{ return myFlags & PTopoDS_OrientableMask; }

//=======================================================================
//function : Orientable
//purpose  : 
//=======================================================================

void  PTopoDS_TShape1::Orientable(const Standard_Boolean M)
{
  if (M) myFlags |= PTopoDS_OrientableMask;
  else   myFlags &= ~PTopoDS_OrientableMask;
}

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape1::Closed() const 
{ return myFlags & PTopoDS_ClosedMask; }

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

void  PTopoDS_TShape1::Closed(const Standard_Boolean M)
{
  if (M) myFlags |= PTopoDS_ClosedMask;
  else   myFlags &= ~PTopoDS_ClosedMask;
}

//=======================================================================
//function : Infinite
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape1::Infinite() const 
{ return myFlags & PTopoDS_InfiniteMask; }

//=======================================================================
//function : Infinite
//purpose  : 
//=======================================================================

void  PTopoDS_TShape1::Infinite(const Standard_Boolean M)
{
  if (M) myFlags |= PTopoDS_InfiniteMask;
  else   myFlags &= ~PTopoDS_InfiniteMask;
}

//=======================================================================
//function : Convex
//purpose  : 
//=======================================================================

Standard_Boolean  PTopoDS_TShape1::Convex() const 
{ return myFlags & PTopoDS_ConvexMask; }

//=======================================================================
//function : Convex
//purpose  : 
//=======================================================================

void  PTopoDS_TShape1::Convex(const Standard_Boolean M)
{
  if (M) myFlags |= PTopoDS_ConvexMask;
  else   myFlags &= ~PTopoDS_ConvexMask;
}


//=======================================================================
//function : Shapes
//purpose  : returns the array of sub-shapes
//=======================================================================

Handle(PTopoDS_HArray1OfShape1) PTopoDS_TShape1::Shapes() const
{ return myShapes; }

//=======================================================================
//function : Shapes
//purpose  : returns the Ith element of the array of sub-shapes
//=======================================================================

PTopoDS_Shape1 PTopoDS_TShape1::Shapes
(const Standard_Integer i) const
{ return myShapes->Value(i); }

//=======================================================================
//function : Shapes
//purpose  : Sets the array of sub-shapes
//=======================================================================

void PTopoDS_TShape1::Shapes(const Handle(PTopoDS_HArray1OfShape1)& S)
{ myShapes = S; }

//=======================================================================
//function : Shapes
//purpose  : Sets the Ith element of the array of sub-shapes
//=======================================================================

void PTopoDS_TShape1::Shapes
(const Standard_Integer i,
 const PTopoDS_Shape1& S)
{ myShapes->SetValue(i, S); }

