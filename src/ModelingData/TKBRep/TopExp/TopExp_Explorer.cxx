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
inline bool isSameType(const TopAbs_ShapeEnum theType, const TopAbs_ShapeEnum toFind) noexcept
{
  return toFind == theType;
}

//! Returns true if the given type should be avoided.
inline bool shouldAvoid(const TopAbs_ShapeEnum theType, const TopAbs_ShapeEnum toAvoid) noexcept
{
  return toAvoid != TopAbs_SHAPE && toAvoid == theType;
}

//! Returns true if the given type is more complex than the type to find.
inline bool isMoreComplex(const TopAbs_ShapeEnum theType, const TopAbs_ShapeEnum toFind) noexcept
{
  return toFind > theType;
}
} // namespace

//=================================================================================================

TopExp_Explorer::TopExp_Explorer() noexcept
    : toFind(TopAbs_SHAPE),
      toAvoid(TopAbs_SHAPE),
      hasMore(false)
{
}

//=================================================================================================

TopExp_Explorer::TopExp_Explorer(const TopoDS_Shape&    S,
                                 const TopAbs_ShapeEnum ToFind,
                                 const TopAbs_ShapeEnum ToAvoid)
    : toFind(ToFind),
      toAvoid(ToAvoid),
      hasMore(false)
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
    hasMore = false;
    return;
  }

  if (toFind == TopAbs_SHAPE)
    hasMore = false;
  else
  {
    TopAbs_ShapeEnum ty = S.ShapeType();

    if (ty > toFind)
    {
      hasMore = false;
    }
    else if (!isSameType(ty, toFind))
    {
      hasMore = true;
      Next();
    }
    else
    {
      hasMore = true;
    }
  }
}

//=================================================================================================

void TopExp_Explorer::Next()
{
  if (myStackTop < 0)
  {
    TopAbs_ShapeEnum ty = myShape.ShapeType();

    if (isSameType(ty, toFind))
    {
      hasMore = false;
      return;
    }
    else if (shouldAvoid(ty, toAvoid))
    {
      hasMore = false;
      return;
    }
    else
    {
      pushIterator(TopoDS_Iterator(myShape));
    }
  }
  else
    myStack[myStackTop].Next();

  for (;;)
  {
    TopoDS_Iterator& aTopIter = myStack[myStackTop];

    if (aTopIter.More())
    {
      const TopoDS_Shape&    aShapTop = aTopIter.Value();
      const TopAbs_ShapeEnum ty       = aShapTop.ShapeType();

      if (isSameType(ty, toFind))
      {
        hasMore = true;
        return;
      }
      else if (isMoreComplex(ty, toFind) && !shouldAvoid(ty, toAvoid))
      {
        pushIterator(TopoDS_Iterator(aShapTop));
        // aTopIter reference is now invalid after push
      }
      else
      {
        aTopIter.Next();
      }
    }
    else
    {
      popIterator();
      if (myStackTop < 0)
        break;
      myStack[myStackTop].Next();
    }
  }
  hasMore = false;
}

//=================================================================================================

const TopoDS_Shape& TopExp_Explorer::Current() const noexcept
{
  return myStackTop < 0 ? myShape : myStack[myStackTop].Value();
}

//=================================================================================================

int TopExp_Explorer::Depth() const noexcept
{
  return myStackTop + 1;
}

//=================================================================================================

void TopExp_Explorer::Clear()
{
  // Reset live iterators to release shape references immediately,
  // but keep the array allocation to avoid Reallocate(0)/Reallocate(N) cycles.
  for (int i = myStackTop; i >= 0; --i)
    myStack[i] = TopoDS_Iterator();
  myStackTop = -1;
  hasMore    = false;
}

//=================================================================================================

void TopExp_Explorer::pushIterator(TopoDS_Iterator&& theIter)
{
  ++myStackTop;
  if (static_cast<size_t>(myStackTop) >= myStack.Size())
  {
    const size_t aNewSize =
      std::max<size_t>(myStack.Size() * 2, static_cast<size_t>(THE_INLINE_STACK_SIZE));
    myStack.Reallocate(aNewSize, true);
  }
  myStack[myStackTop] = std::move(theIter);
}

//=================================================================================================

void TopExp_Explorer::popIterator()
{
  // Reset the popped iterator to release shape references immediately.
  myStack[myStackTop] = TopoDS_Iterator();
  --myStackTop;
}
