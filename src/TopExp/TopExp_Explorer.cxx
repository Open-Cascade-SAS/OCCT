// Created on: 1993-01-18
// Created by: Remi LEQUETTE
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

#define No_Standard_NoMoreObject
#define No_Standard_NoSuchObject


#include <Standard.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

// macro to compare two types of shapes
// always True if the first one is SHAPE
#define SAMETYPE(x,y) ((x) == (y))
#define AVOID(x,y) (((x) == TopAbs_SHAPE) ? Standard_False : (x) == (y))
#define LESSCOMPLEX(x,y) ((x) > (y))

static const Standard_Integer theStackSize = 20;

//=======================================================================
//function : TopExp_Explorer
//purpose  : 
//=======================================================================

TopExp_Explorer::TopExp_Explorer() :
    myStack(0L),myTop(-1),hasMore(Standard_False)
{
  myStack = (TopoDS_Iterator*)Standard::Allocate(theStackSize*sizeof(TopoDS_Iterator));
  mySizeOfStack = theStackSize;
}


//=======================================================================
//function : TopExp_Explorer
//purpose  : 
//=======================================================================

TopExp_Explorer::TopExp_Explorer(const TopoDS_Shape& S, 
				 const TopAbs_ShapeEnum ToFind, 
				 const TopAbs_ShapeEnum ToAvoid):
       myStack(0L),myTop(-1),hasMore(Standard_False)

{
  myStack = (TopoDS_Iterator*)Standard::Allocate(theStackSize*sizeof(TopoDS_Iterator));
  mySizeOfStack = theStackSize;
  Init(S,ToFind,ToAvoid);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  TopExp_Explorer::Init(const TopoDS_Shape& S, 
			    const TopAbs_ShapeEnum ToFind, 
			    const TopAbs_ShapeEnum ToAvoid)
{
  if(myTop >=0) {
    for(int i=0;i<= myTop; i++)
      myStack[i].~TopoDS_Iterator();
  }
  
  myTop   = -1;
  myShape = S;
  toFind  = ToFind;
  toAvoid = ToAvoid;

  if (S.IsNull()) {
     hasMore = Standard_False;
     return;
  }

#if 0
  // for SOLID, FACE, EDGE ignores the initial orientation
  TopAbs_ShapeEnum T = myShape.ShapeType();
  if ((T == TopAbs_SOLID) || (T == TopAbs_FACE) || (T == TopAbs_EDGE))
    myShape.Orientation(TopAbs_FORWARD);
#endif

  if (toFind == TopAbs_SHAPE)
    hasMore = Standard_False;

  else {
    TopAbs_ShapeEnum ty = S.ShapeType();

    if (LESSCOMPLEX(ty,toFind)) {
      // the first Shape is less complex, nothing to find
      hasMore = Standard_False;
    }
    else if (!SAMETYPE(ty,toFind)) {
      // type is more complex search inside
      hasMore = Standard_True;
      Next();
    }
    else {
      // type is found
      hasMore = Standard_True;
    }
  }
}


//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

const TopoDS_Shape&  TopExp_Explorer::Current()const 
{
  Standard_NoSuchObject_Raise_if(!hasMore,"TopExp_Explorer::Current");
  if (myTop >= 0) {
    const TopoDS_Shape& S = myStack[myTop].Value();
    return S;
  }
  else
    return myShape;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4291) // to avoid warning when using new(buffer) syntax
#endif

void  TopExp_Explorer::Next()
{
  Standard_Integer NewSize;
  TopoDS_Shape ShapTop;
  TopAbs_ShapeEnum ty;
  Standard_NoMoreObject_Raise_if(!hasMore,"TopExp_Explorer::Next");

  if (myTop < 0) {
    // empty stack. Entering the initial shape.
    ty = myShape.ShapeType();

    if (SAMETYPE(toFind,ty)) {
      // already visited once
      hasMore = Standard_False;
      return;
    }
    else if (AVOID(toAvoid,ty)) {
      // avoid the top-level
      hasMore = Standard_False;
      return;
    }
    else {
      // push and try to find
      if(++myTop >= mySizeOfStack) {
	NewSize = mySizeOfStack + theStackSize;
	TopExp_Stack newStack = (TopoDS_Iterator*)Standard::Allocate(NewSize*sizeof(TopoDS_Iterator));
	Standard_Integer i;
	for ( i =0; i < myTop; i++) {
	  new (&newStack[i]) TopoDS_Iterator(myStack[i]);
	  myStack[i].~TopoDS_Iterator();
	}
	Standard::Free(myStack);
	mySizeOfStack = NewSize;
	myStack = newStack;
      }
      new (&myStack[myTop]) TopoDS_Iterator(myShape);
    }
  }
  else myStack[myTop].Next();

  for (;;) {
    if (myStack[myTop].More()) {
      ShapTop = myStack[myTop].Value();
      ty = ShapTop.ShapeType();
      if (SAMETYPE(toFind,ty)) {
	hasMore = Standard_True;
	return;
      }
      else if (LESSCOMPLEX(toFind,ty) && !AVOID(toAvoid,ty)) {
	if(++myTop >= mySizeOfStack) {
	  NewSize = mySizeOfStack + theStackSize;
	  TopExp_Stack newStack = (TopoDS_Iterator*)Standard::Allocate(NewSize*sizeof(TopoDS_Iterator));
	  Standard_Integer i;
	  for (i =0; i < myTop; i++) {
	    new (&newStack[i]) TopoDS_Iterator(myStack[i]);
	    myStack[i].~TopoDS_Iterator();
	  }
	  Standard::Free(myStack);
	  mySizeOfStack = NewSize;
	  myStack = newStack;
	}
	new (&myStack[myTop]) TopoDS_Iterator(ShapTop);
      }
      else {
	myStack[myTop].Next();
      }
    }
    else {
      myStack[myTop].~TopoDS_Iterator();
      myTop--;
      if(myTop < 0) break;
      myStack[myTop].Next();
    }
  }
  hasMore = Standard_False;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//=======================================================================
//function : ReInit
//purpose  : 
//=======================================================================

void  TopExp_Explorer::ReInit()
{
  Init(myShape,toFind,toAvoid);
}

void  TopExp_Explorer::Destroy()
{
  if (myStack) 
    {
      for(int i=0;i<= myTop; i++)myStack[i].~TopoDS_Iterator();
      Standard::Free(myStack);
    }
  mySizeOfStack = 0;
  myStack = 0L;
}

