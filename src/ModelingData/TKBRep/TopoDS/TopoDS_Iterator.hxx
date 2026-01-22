// Created on: 1993-01-21
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

#ifndef _TopoDS_Iterator_HeaderFile
#define _TopoDS_Iterator_HeaderFile

#include <Standard_NoSuchObject.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

class TopoDS_TShape;

//! Iterates on the underlying shape underlying a given
//! TopoDS_Shape object, providing access to its
//! component sub-shapes. Each component shape is
//! returned as a TopoDS_Shape with an orientation,
//! and a compound of the original values and the relative values.
//!
//! This iterator uses index-based access to child shapes
//! stored in the TShape's dynamic array for optimal performance.
class TopoDS_Iterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty Iterator.
  TopoDS_Iterator()
      : myTShape(nullptr),
        myIndex(0U),
        myNbChildren(0U),
        myOrientation(TopAbs_FORWARD)
  {
  }

  //! Creates an Iterator on <S> sub-shapes.
  //! Note:
  //! - If cumOri is true, the function composes all
  //! sub-shapes with the orientation of S.
  //! - If cumLoc is true, the function multiplies all
  //! sub-shapes by the location of S, i.e. it applies to
  //! each sub-shape the transformation that is associated with S.
  TopoDS_Iterator(const TopoDS_Shape& S, const bool cumOri = true, const bool cumLoc = true)
      : myTShape(nullptr),
        myIndex(0U),
        myNbChildren(0U),
        myOrientation(TopAbs_FORWARD)
  {
    Initialize(S, cumOri, cumLoc);
  }

  //! Initializes this iterator with shape S.
  //! Note:
  //! - If cumOri is true, the function composes all
  //! sub-shapes with the orientation of S.
  //! - If cumLoc is true, the function multiplies all
  //! sub-shapes by the location of S, i.e. it applies to
  //! each sub-shape the transformation that is associated with S.
  Standard_EXPORT void Initialize(const TopoDS_Shape& S,
                                  const bool          cumOri = true,
                                  const bool          cumLoc = true);

  //! Returns true if there is another sub-shape in the
  //! shape which this iterator is scanning.
  bool More() const { return myIndex < myNbChildren; }

  //! Moves on to the next sub-shape in the shape which
  //! this iterator is scanning.
  //! Exceptions
  //! Standard_NoMoreObject if there are no more sub-shapes in the shape.
  Standard_EXPORT void Next();

  //! Returns the current sub-shape in the shape which
  //! this iterator is scanning.
  //! Exceptions
  //! Standard_NoSuchObject if there is no current sub-shape.
  const TopoDS_Shape& Value() const
  {
    Standard_NoSuchObject_Raise_if(!More(), "TopoDS_Iterator::Value");
    return myShape;
  }

private:
  //! Updates myShape from the current child at myIndex.
  void updateCurrentShape();

private:
  TopoDS_Shape       myShape;       //!< Current composed sub-shape
  TopoDS_TShape*     myTShape;      //!< Pointer to parent TShape (non-owning)
  size_t             myIndex;       //!< Current child index (0-based)
  size_t             myNbChildren;  //!< Cached number of children
  TopAbs_Orientation myOrientation; //!< Cumulative orientation
  TopLoc_Location    myLocation;    //!< Cumulative location
};

#endif // _TopoDS_Iterator_HeaderFile
