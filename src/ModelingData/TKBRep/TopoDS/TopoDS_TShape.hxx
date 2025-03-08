// Created on: 1990-12-13
// Created by: Remi Lequette
// Copyright (c) 1990-1999 Matra Datavision
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

#ifndef _TopoDS_TShape_HeaderFile
#define _TopoDS_TShape_HeaderFile

#include <TopAbs.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_ListOfShape.hxx>

// resolve name collisions with X11 headers
#ifdef Convex
  #undef Convex
#endif

//! A TShape  is a topological  structure describing a
//! set of points in a 2D or 3D space.
//!
//! A topological shape is a structure made from other
//! shapes.  This is a deferred class  used to support
//! topological objects.
//!
//! TShapes are   defined   by  their  optional domain
//! (geometry)  and  their  components  (other TShapes
//! with  Locations and Orientations).  The components
//! are stored in a List of Shapes.
//!
//! A   TShape contains  the   following boolean flags :
//!
//! - Free       : Free or Frozen.
//! - Modified   : Has been modified.
//! - Checked    : Has been checked.
//! - Orientable : Can be oriented.
//! - Closed     : Is closed (note that only Wires and Shells may be closed).
//! - Infinite   : Is infinite.
//! - Convex     : Is convex.
//!
//! Users have no direct access to the classes derived
//! from TShape.  They  handle them with   the classes
//! derived from Shape.
class TopoDS_TShape : public Standard_Transient
{

public:
  //! Returns the free flag.
  Standard_Boolean Free() const { return ((myFlags & TopoDS_TShape_Flags_Free) != 0); }

  //! Sets the free flag.
  void Free(Standard_Boolean theIsFree) { setFlag(TopoDS_TShape_Flags_Free, theIsFree); }

  //! Returns the locked flag.
  Standard_Boolean Locked() const { return ((myFlags & TopoDS_TShape_Flags_Locked) != 0); }

  //! Sets the locked flag.
  void Locked(Standard_Boolean theIsLocked) { setFlag(TopoDS_TShape_Flags_Locked, theIsLocked); }

  //! Returns the modification flag.
  Standard_Boolean Modified() const { return ((myFlags & TopoDS_TShape_Flags_Modified) != 0); }

  //! Sets the modification flag.
  void Modified(Standard_Boolean theIsModified)
  {
    setFlag(TopoDS_TShape_Flags_Modified, theIsModified);
    if (theIsModified)
    {
      // clang-format off
      setFlag (TopoDS_TShape_Flags_Checked, false); // when a TShape is modified it is also unchecked
      // clang-format on
    }
  }

  //! Returns the checked flag.
  Standard_Boolean Checked() const { return ((myFlags & TopoDS_TShape_Flags_Checked) != 0); }

  //! Sets the checked flag.
  void Checked(Standard_Boolean theIsChecked)
  {
    setFlag(TopoDS_TShape_Flags_Checked, theIsChecked);
  }

  //! Returns the orientability flag.
  Standard_Boolean Orientable() const { return ((myFlags & TopoDS_TShape_Flags_Orientable) != 0); }

  //! Sets the orientability flag.
  void Orientable(Standard_Boolean theIsOrientable)
  {
    setFlag(TopoDS_TShape_Flags_Orientable, theIsOrientable);
  }

  //! Returns the closedness flag.
  Standard_Boolean Closed() const { return ((myFlags & TopoDS_TShape_Flags_Closed) != 0); }

  //! Sets the closedness flag.
  void Closed(Standard_Boolean theIsClosed) { setFlag(TopoDS_TShape_Flags_Closed, theIsClosed); }

  //! Returns the infinity flag.
  Standard_Boolean Infinite() const { return ((myFlags & TopoDS_TShape_Flags_Infinite) != 0); }

  //! Sets the infinity flag.
  void Infinite(Standard_Boolean theIsInfinite)
  {
    setFlag(TopoDS_TShape_Flags_Infinite, theIsInfinite);
  }

  //! Returns the convexness flag.
  Standard_Boolean Convex() const { return ((myFlags & TopoDS_TShape_Flags_Convex) != 0); }

  //! Sets the convexness flag.
  void Convex(Standard_Boolean theIsConvex) { setFlag(TopoDS_TShape_Flags_Convex, theIsConvex); }

  //! Returns the type as a term of the ShapeEnum enum :
  //! VERTEX, EDGE, WIRE, FACE, ....
  Standard_EXPORT virtual TopAbs_ShapeEnum ShapeType() const = 0;

  //! Returns a copy  of the  TShape  with no sub-shapes.
  Standard_EXPORT virtual Handle(TopoDS_TShape) EmptyCopy() const = 0;

  //! Returns the number of direct sub-shapes (children).
  //! @sa TopoDS_Iterator for accessing sub-shapes
  Standard_Integer NbChildren() const { return myShapes.Size(); }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const;

  friend class TopoDS_Iterator;
  friend class TopoDS_Builder;

  DEFINE_STANDARD_RTTIEXT(TopoDS_TShape, Standard_Transient)

protected:
  //! Constructs an empty TShape.
  //! Free       : True
  //! Modified   : True
  //! Checked    : False
  //! Orientable : True
  //! Closed     : False
  //! Infinite   : False
  //! Convex     : False
  TopoDS_TShape()
      : myFlags(TopoDS_TShape_Flags_Free | TopoDS_TShape_Flags_Modified
                | TopoDS_TShape_Flags_Orientable)
  {
  }

private:
  // Defined mask values
  enum TopoDS_TShape_Flags
  {
    TopoDS_TShape_Flags_Free       = 0x001,
    TopoDS_TShape_Flags_Modified   = 0x002,
    TopoDS_TShape_Flags_Checked    = 0x004,
    TopoDS_TShape_Flags_Orientable = 0x008,
    TopoDS_TShape_Flags_Closed     = 0x010,
    TopoDS_TShape_Flags_Infinite   = 0x020,
    TopoDS_TShape_Flags_Convex     = 0x040,
    TopoDS_TShape_Flags_Locked     = 0x080
  };

  //! Set bit flag.
  void setFlag(TopoDS_TShape_Flags theFlag, Standard_Boolean theIsOn)
  {
    if (theIsOn)
      myFlags |= (Standard_Integer)theFlag;
    else
      myFlags &= ~(Standard_Integer)theFlag;
  }

private:
  TopoDS_ListOfShape myShapes;
  Standard_Integer   myFlags;
};

DEFINE_STANDARD_HANDLE(TopoDS_TShape, Standard_Transient)

#endif // _TopoDS_TShape_HeaderFile
