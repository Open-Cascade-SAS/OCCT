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

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopAbs.hxx>
#include <TopAbs_ShapeEnum.hxx>

// resolve name collisions with X11 headers
#ifdef Convex
  #undef Convex
#endif

class TopoDS_Shape;

//! A TShape is a topological structure describing a
//! set of points in a 2D or 3D space.
//!
//! A topological shape is a structure made from other
//! shapes. This is a deferred class used to support
//! topological objects.
//!
//! TShapes are defined by their optional domain
//! (geometry) and their components (other TShapes
//! with Locations and Orientations). The components
//! are stored in derived classes using type-specific
//! storage strategies (local slots for common cases,
//! dynamic arrays for overflow).
//!
//! Shape type and flags are stored in a compact 16-bit field:
//! - Bits 0-3:  ShapeType (4 bits for TopAbs_ShapeEnum values 0-8)
//! - Bit 4:     Free (free or frozen)
//! - Bit 5:     Modified (has been modified)
//! - Bit 6:     Checked (has been checked)
//! - Bit 7:     Orientable (can be oriented)
//! - Bit 8:     Closed (is closed, only for Wires and Shells)
//! - Bit 9:     Infinite (is infinite)
//! - Bit 10:    Convex (is convex)
//! - Bit 11:    Locked (is locked for modifications)
//! - Bits 12-15: Reserved for future use
//!
//! Users have no direct access to the classes derived
//! from TShape. They handle them with the classes
//! derived from Shape.
class TopoDS_TShape : public Standard_Transient
{
public:
  //! Bit layout for shape type and flags.
  //! Organized as a 16-bit field for compact storage.
  enum BitLayout : uint16_t
  {
    // Shape type stored in bits 0-3 (4 bits for values 0-8)
    Bits_ShapeType_Mask  = 0x000F, //!< Mask for shape type bits
    Bits_ShapeType_Shift = 0,      //!< Shift for shape type

    // Boolean flags in bits 4-11
    Bit_Free       = 0x0010, //!< Bit 4: Free or Frozen
    Bit_Modified   = 0x0020, //!< Bit 5: Has been modified
    Bit_Checked    = 0x0040, //!< Bit 6: Has been checked
    Bit_Orientable = 0x0080, //!< Bit 7: Can be oriented
    Bit_Closed     = 0x0100, //!< Bit 8: Is closed
    Bit_Infinite   = 0x0200, //!< Bit 9: Is infinite
    Bit_Convex     = 0x0400, //!< Bit 10: Is convex
    Bit_Locked     = 0x0800, //!< Bit 11: Is locked for modifications

    // Bits 12-15 reserved for future use
    Bits_Reserved_Mask = 0xF000
  };

  //! Returns the shape type (non-virtual, direct access).
  TopAbs_ShapeEnum ShapeType() const
  {
    return static_cast<TopAbs_ShapeEnum>((myState & Bits_ShapeType_Mask) >> Bits_ShapeType_Shift);
  }

  //! Returns the free flag.
  bool Free() const { return (myState & Bit_Free) != 0; }

  //! Sets the free flag.
  void Free(bool theIsFree) { setFlag(Bit_Free, theIsFree); }

  //! Returns the locked flag.
  bool Locked() const { return (myState & Bit_Locked) != 0; }

  //! Sets the locked flag.
  void Locked(bool theIsLocked) { setFlag(Bit_Locked, theIsLocked); }

  //! Returns the modification flag.
  bool Modified() const { return (myState & Bit_Modified) != 0; }

  //! Sets the modification flag.
  void Modified(bool theIsModified)
  {
    setFlag(Bit_Modified, theIsModified);
    if (theIsModified)
    {
      myState &= ~Bit_Checked; // when a TShape is modified it is also unchecked
    }
  }

  //! Returns the checked flag.
  bool Checked() const { return (myState & Bit_Checked) != 0; }

  //! Sets the checked flag.
  void Checked(bool theIsChecked) { setFlag(Bit_Checked, theIsChecked); }

  //! Returns the orientability flag.
  bool Orientable() const { return (myState & Bit_Orientable) != 0; }

  //! Sets the orientability flag.
  void Orientable(bool theIsOrientable) { setFlag(Bit_Orientable, theIsOrientable); }

  //! Returns the closedness flag.
  bool Closed() const { return (myState & Bit_Closed) != 0; }

  //! Sets the closedness flag.
  void Closed(bool theIsClosed) { setFlag(Bit_Closed, theIsClosed); }

  //! Returns the infinity flag.
  bool Infinite() const { return (myState & Bit_Infinite) != 0; }

  //! Sets the infinity flag.
  void Infinite(bool theIsInfinite) { setFlag(Bit_Infinite, theIsInfinite); }

  //! Returns the convexness flag.
  bool Convex() const { return (myState & Bit_Convex) != 0; }

  //! Sets the convexness flag.
  void Convex(bool theIsConvex) { setFlag(Bit_Convex, theIsConvex); }

  //! Returns a copy of the TShape with no sub-shapes.
  Standard_EXPORT virtual Handle(TopoDS_TShape) EmptyCopy() const = 0;

  //! Returns the number of direct sub-shapes (children).
  //! @sa TopoDS_Iterator for accessing sub-shapes
  virtual int NbChildren() const = 0;

  //! Returns the child shape at the given index (0-based).
  //! @param theIndex index of the child shape (0 <= theIndex < NbChildren())
  //! @return reference to the child shape
  virtual const TopoDS_Shape& GetChild(int theIndex) const = 0;

  //! Returns the child shape at the given index (0-based) for modification.
  //! @param theIndex index of the child shape (0 <= theIndex < NbChildren())
  //! @return reference to the child shape
  virtual TopoDS_Shape& ChangeChild(int theIndex) = 0;

  //! Adds a child shape.
  //! @param theShape the child shape to add
  virtual void AddChild(const TopoDS_Shape& theShape) = 0;

  //! Removes the child shape at the given index (0-based).
  //! @param theIndex index of the child shape to remove
  virtual void RemoveChild(int theIndex) = 0;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const;

  friend class TopoDS_Iterator;
  friend class TopoDS_Builder;

  DEFINE_STANDARD_RTTIEXT(TopoDS_TShape, Standard_Transient)

protected:
  //! Constructs a TShape with specified shape type.
  //! Default flags: Free=true, Modified=true, Orientable=true, others=false.
  //! @param theShapeType the type of shape (VERTEX, EDGE, WIRE, etc.)
  TopoDS_TShape(TopAbs_ShapeEnum theShapeType)
      : myState(static_cast<uint16_t>(theShapeType) | Bit_Free | Bit_Modified | Bit_Orientable)
  {
  }

  //! Set or clear a flag bit.
  //! @param theBit the bit mask to modify
  //! @param theIsOn true to set, false to clear
  void setFlag(uint16_t theBit, bool theIsOn)
  {
    if (theIsOn)
    {
      myState |= theBit;
    }
    else
    {
      myState &= ~theBit;
    }
  }

protected:
  uint16_t myState; //!< Compact storage: shape type (4 bits) + flags (8 bits) + reserved (4 bits)
};

DEFINE_STANDARD_HANDLE(TopoDS_TShape, Standard_Transient)

#endif // _TopoDS_TShape_HeaderFile
