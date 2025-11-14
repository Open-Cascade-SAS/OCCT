// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2025 OPEN CASCADE SAS
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

#include <TopExp_Explorer.hxx>

namespace
{
//! Returns true if the given type matches the type to find.
inline Standard_Boolean isSameType(const TopAbs_ShapeEnum theType,
                                   const TopAbs_ShapeEnum toFind) noexcept
{
  return toFind == theType;
}

//! Returns true if the given type should be avoided.
inline Standard_Boolean shouldAvoid(const TopAbs_ShapeEnum theType,
                                    const TopAbs_ShapeEnum toAvoid) noexcept
{
  return toAvoid != TopAbs_SHAPE && toAvoid == theType;
}

//! Returns true if the given type is more complex than the type to find.
inline Standard_Boolean isMoreComplex(const TopAbs_ShapeEnum theType,
                                      const TopAbs_ShapeEnum toFind) noexcept
{
  return toFind > theType;
}
} // namespace

//=================================================================================================

TopExp_Explorer::TopExp_Explorer() noexcept
    : myStack(20),
      toFind(TopAbs_SHAPE),
      toAvoid(TopAbs_SHAPE),
      hasMore(Standard_False)
{
}

//=================================================================================================

TopExp_Explorer::TopExp_Explorer(const TopoDS_Shape&    S,
                                 const TopAbs_ShapeEnum ToFind,
                                 const TopAbs_ShapeEnum ToAvoid)
    : myStack(20),
      toFind(ToFind),
      toAvoid(ToAvoid),
      hasMore(Standard_False)
{
  Init(S, ToFind, ToAvoid);
}

//=================================================================================================

TopExp_Explorer::~TopExp_Explorer()
{
  Clear();
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

  if (toFind == TopAbs_SHAPE)
    hasMore = Standard_False;
  else
  {
    TopAbs_ShapeEnum ty = S.ShapeType();

    if (ty > toFind)
    {
      hasMore = Standard_False;
    }
    else if (!isSameType(ty, toFind))
    {
      hasMore = Standard_True;
      Next();
    }
    else
    {
      hasMore = Standard_True;
    }
  }
}

//=================================================================================================

void TopExp_Explorer::Next()
{
  if (myStack.IsEmpty())
  {
    TopAbs_ShapeEnum ty = myShape.ShapeType();

    if (isSameType(ty, toFind))
    {
      hasMore = Standard_False;
      return;
    }
    else if (shouldAvoid(ty, toAvoid))
    {
      hasMore = Standard_False;
      return;
    }
    else
    {
      myStack.Append(TopoDS_Iterator(myShape));
    }
  }
  else
    myStack.ChangeLast().Next();

  for (;;)
  {
    TopoDS_Iterator& aTopIter = myStack.ChangeLast();

    if (aTopIter.More())
    {
      const TopoDS_Shape&    aShapTop = aTopIter.Value();
      const TopAbs_ShapeEnum ty       = aShapTop.ShapeType();

      if (isSameType(ty, toFind))
      {
        hasMore = Standard_True;
        return;
      }
      else if (isMoreComplex(ty, toFind) && !shouldAvoid(ty, toAvoid))
      {
        myStack.Append(TopoDS_Iterator(aShapTop));
        // aTopIter reference is now invalid after Append
      }
      else
      {
        aTopIter.Next();
      }
    }
    else
    {
      myStack.EraseLast();
      if (myStack.IsEmpty())
        break;
      myStack.ChangeLast().Next();
    }
  }
  hasMore = Standard_False;
}

//=================================================================================================

const TopoDS_Shape& TopExp_Explorer::Current() const noexcept
{
  return myStack.IsEmpty() ? myShape : myStack.Last().Value();
}

//=================================================================================================

Standard_Integer TopExp_Explorer::Depth() const noexcept
{
  return myStack.Length();
}

//=================================================================================================

void TopExp_Explorer::Clear()
{
  hasMore = Standard_False;
  myStack.Clear();
}
