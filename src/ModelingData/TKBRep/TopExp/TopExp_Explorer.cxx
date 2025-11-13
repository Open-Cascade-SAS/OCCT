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

#include <TopExp_Explorer.hxx>

#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopAbs.hxx>

// macro to compare two types of shapes
// always True if the first one is SHAPE
#define SAMETYPE(x, y) ((x) == (y))
#define AVOID(x, y) (((x) == TopAbs_SHAPE) ? Standard_False : (x) == (y))
#define LESSCOMPLEX(x, y) ((x) > (y))

static const Standard_Integer theStackBlockSize = 20;

//=================================================================================================

TopExp_Explorer::TopExp_Explorer()
    : myStack(theStackBlockSize),
      myTop(-1),
      toFind(TopAbs_SHAPE),
      toAvoid(TopAbs_SHAPE),
      hasMore(Standard_False)
{
}

//=================================================================================================

TopExp_Explorer::TopExp_Explorer(const TopoDS_Shape&    theS,
                                 const TopAbs_ShapeEnum theToFind,
                                 const TopAbs_ShapeEnum theToAvoid)
    : myStack(theStackBlockSize),
      myTop(-1),
      toFind(theToFind),
      toAvoid(theToAvoid),
      hasMore(Standard_False)
{
  Init(theS, theToFind, theToAvoid);
}

//=================================================================================================

void TopExp_Explorer::Init(const TopoDS_Shape&    S,
                           const TopAbs_ShapeEnum ToFind,
                           const TopAbs_ShapeEnum ToAvoid)
{
  Clear();

  myShape = S;
  toFind  = ToFind;
  toAvoid = ToAvoid;

  if (S.IsNull())
  {
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

  else
  {
    TopAbs_ShapeEnum ty = S.ShapeType();

    if (LESSCOMPLEX(ty, toFind))
    {
      // the first Shape is less complex, nothing to find
      hasMore = Standard_False;
    }
    else if (!SAMETYPE(ty, toFind))
    {
      // type is more complex search inside
      hasMore = Standard_True;
      Next();
    }
    else
    {
      // type is found
      hasMore = Standard_True;
    }
  }
}

//=================================================================================================

void TopExp_Explorer::Next()
{
  TopoDS_Shape     ShapTop;
  TopAbs_ShapeEnum ty;
  Standard_NoMoreObject_Raise_if(!hasMore, "TopExp_Explorer::Next");

  if (myTop < 0)
  {
    // empty stack. Entering the initial shape.
    ty = myShape.ShapeType();

    if (SAMETYPE(toFind, ty))
    {
      // already visited once
      hasMore = Standard_False;
      return;
    }
    else if (AVOID(toAvoid, ty))
    {
      // avoid the top-level
      hasMore = Standard_False;
      return;
    }
    else
    {
      // push and try to find
      myStack.Append(TopoDS_Iterator(myShape));
      myTop = myStack.Length() - 1;
    }
  }
  else
    myStack[myTop].Next();

  for (;;)
  {
    if (myStack[myTop].More())
    {
      ShapTop = myStack[myTop].Value();
      ty      = ShapTop.ShapeType();
      if (SAMETYPE(toFind, ty))
      {
        hasMore = Standard_True;
        return;
      }
      else if (LESSCOMPLEX(toFind, ty) && !AVOID(toAvoid, ty))
      {
        myStack.Append(TopoDS_Iterator(ShapTop));
        myTop = myStack.Length() - 1;
      }
      else
      {
        myStack[myTop].Next();
      }
    }
    else
    {
      // Pop: remove the exhausted iterator from the vector
      myStack.EraseLast();
      myTop--;
      if (myTop < 0)
        break;
      myStack[myTop].Next();
    }
  }
  hasMore = Standard_False;
}

//=================================================================================================

void TopExp_Explorer::ReInit()
{
  Init(myShape, toFind, toAvoid);
}

//=================================================================================================

TopExp_Explorer::~TopExp_Explorer()
{
  Clear();
}

//=================================================================================================

void TopExp_Explorer::Clear()
{
  hasMore = Standard_False;
  myStack.Clear();
  myTop = -1;
}
