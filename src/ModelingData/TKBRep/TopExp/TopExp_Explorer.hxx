// Created on: 1993-01-14
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

#ifndef _TopExp_Explorer_HeaderFile
#define _TopExp_Explorer_HeaderFile

#include <NCollection_Vector.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//! An Explorer is a Tool to visit a Topological Data
//! Structure from the TopoDS package.
//!
//! An Explorer is built with:
//!
//! * The Shape to explore.
//!
//! * The type of Shapes to find: e.g VERTEX, EDGE.
//! This type cannot be SHAPE.
//!
//! * The type of Shapes to avoid. e.g SHELL, EDGE.
//! By default this type is SHAPE which means no
//! restriction on the exploration.
//!
//! The Explorer visits all the structure to find
//! shapes of the requested type which are not
//! contained in the type to avoid.
//!
//! Example to find all the Faces in the Shape S :
//!
//! TopExp_Explorer Ex;
//! for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {
//! ProcessFace(Ex.Current());
//! }
//!
//! // an other way
//! TopExp_Explorer Ex(S,TopAbs_FACE);
//! while (Ex.More()) {
//! ProcessFace(Ex.Current());
//! Ex.Next();
//! }
//!
//! To find all the vertices which are not in an edge :
//!
//! for (Ex.Init(S,TopAbs_VERTEX,TopAbs_EDGE); ...)
//!
//! To find all the faces in a SHELL, then all the
//! faces not in a SHELL :
//!
//! TopExp_Explorer Ex1, Ex2;
//!
//! for (Ex1.Init(S,TopAbs_SHELL),...) {
//! // visit all shells
//! for (Ex2.Init(Ex1.Current(),TopAbs_FACE),...) {
//! // visit all the faces of the current shell
//! }
//! }
//!
//! for (Ex1.Init(S,TopAbs_FACE,TopAbs_SHELL),...) {
//! // visit all faces not in a shell
//! }
//!
//! If the type to avoid is the same or is less
//! complex than the type to find it has no effect.
//!
//! For example searching edges not in a vertex does
//! not make a difference.
class TopExp_Explorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty explorer, becomes useful after Init.
  TopExp_Explorer()
      : myStack(20),
        myTop(-1),
        toFind(TopAbs_SHAPE),
        toAvoid(TopAbs_SHAPE),
        hasMore(Standard_False)
  {
  }

  //! Creates an Explorer on the Shape <S>.
  //!
  //! <ToFind> is the type of shapes to search.
  //! TopAbs_VERTEX, TopAbs_EDGE, ...
  //!
  //! <ToAvoid> is the type of shape to skip in the
  //! exploration. If <ToAvoid> is equal or less
  //! complex than <ToFind> or if <ToAVoid> is SHAPE it
  //! has no effect on the exploration.
  TopExp_Explorer(const TopoDS_Shape&    S,
                  const TopAbs_ShapeEnum ToFind,
                  const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE)
      : myStack(20),
        myTop(-1),
        toFind(ToFind),
        toAvoid(ToAvoid),
        hasMore(Standard_False)
  {
    Init(S, ToFind, ToAvoid);
  }

  //! Resets this explorer on the shape S. It is initialized to
  //! search the shape S, for shapes of type ToFind, that
  //! are not part of a shape ToAvoid.
  //! If the shape ToAvoid is equal to TopAbs_SHAPE, or
  //! if it is the same as, or less complex than, the shape
  //! ToFind it has no effect on the search.
  void Init(const TopoDS_Shape&    S,
            const TopAbs_ShapeEnum ToFind,
            const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE)
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
      else if (!isSameType(ty))
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

  //! Returns True if there are more shapes in the exploration.
  Standard_Boolean More() const { return hasMore; }

  //! Moves to the next Shape in the exploration.
  //! Exceptions
  //! Standard_NoMoreObject if there are no more shapes to explore.
  void Next()
  {
    Standard_NoMoreObject_Raise_if(!hasMore, "TopExp_Explorer::Next");

    if (myTop < 0)
    {
      TopAbs_ShapeEnum ty = myShape.ShapeType();

      if (isSameType(ty))
      {
        hasMore = Standard_False;
        return;
      }
      else if (shouldAvoid(ty))
      {
        hasMore = Standard_False;
        return;
      }
      else
      {
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
        TopoDS_Shape     ShapTop = myStack[myTop].Value();
        TopAbs_ShapeEnum ty      = ShapTop.ShapeType();

        if (isSameType(ty))
        {
          hasMore = Standard_True;
          return;
        }
        else if (isMoreComplex(ty) && !shouldAvoid(ty))
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
        myStack.EraseLast();
        myTop--;
        if (myTop < 0)
          break;
        myStack[myTop].Next();
      }
    }
    hasMore = Standard_False;
  }

  //! Returns the current shape in the exploration.
  //! Exceptions
  //! Standard_NoSuchObject if this explorer has no more shapes to explore.
  const TopoDS_Shape& Value() const { return Current(); }

  //! Returns the current shape in the exploration.
  //! Exceptions
  //! Standard_NoSuchObject if this explorer has no more shapes to explore.
  const TopoDS_Shape& Current() const
  {
    Standard_NoSuchObject_Raise_if(!hasMore, "TopExp_Explorer::Current");
    return (myTop >= 0) ? myStack[myTop].Value() : myShape;
  }

  //! Reinitialize the exploration with the original arguments.
  void ReInit() { Init(myShape, toFind, toAvoid); }

  //! Return explored shape.
  const TopoDS_Shape& ExploredShape() const { return myShape; }

  //! Returns the current depth of the exploration. 0 is
  //! the shape to explore itself.
  Standard_Integer Depth() const { return myTop + 1; }

  //! Clears the content of the explorer. It will return
  //! False on More().
  void Clear()
  {
    hasMore = Standard_False;
    myStack.Clear();
    myTop = -1;
  }

  //! Destructor.
  ~TopExp_Explorer() { Clear(); }

protected:
  //! Returns true if the given type matches the type to find.
  Standard_Boolean isSameType(const TopAbs_ShapeEnum theType) const
  {
    return toFind == theType;
  }

  //! Returns true if the given type should be avoided.
  Standard_Boolean shouldAvoid(const TopAbs_ShapeEnum theType) const
  {
    return toAvoid != TopAbs_SHAPE && toAvoid == theType;
  }

  //! Returns true if the given type is more complex than the type to find.
  Standard_Boolean isMoreComplex(const TopAbs_ShapeEnum theType) const
  {
    return toFind > theType;
  }

private:
  NCollection_Vector<TopoDS_Iterator> myStack;
  TopoDS_Shape                        myShape;
  Standard_Integer                    myTop;
  TopAbs_ShapeEnum                    toFind;
  TopAbs_ShapeEnum                    toAvoid;
  Standard_Boolean                    hasMore;
};

#endif // _TopExp_Explorer_HeaderFile
