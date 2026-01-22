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

class TopoDS_Shape;

// resolve name collisions with X11 headers
#ifdef Convex
  #undef Convex
#endif

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
//! are stored in a dynamic array within each derived class.
//!
//! A TShape contains the following boolean flags:
//!
//! - Free       : Free or Frozen.
//! - Modified   : Has been modified.
//! - Checked    : Has been checked.
//! - Orientable : Can be oriented.
//! - Closed     : Is closed (note that only Wires and Shells may be closed).
//! - Infinite   : Is infinite.
//! - Convex     : Is convex.
//! - Locked     : Is locked against modifications.
//!
//! Users have no direct access to the classes derived
//! from TShape. They handle them with the classes
//! derived from Shape.
class TopoDS_TShape : public Standard_Transient
{
public:
  //! Bit layout for compact state storage.
  //! Bits 0-3 store the TopAbs_ShapeEnum value (0-8).
  //! Bits 4-11 store boolean flags.
  //! Bits 12-15 are reserved for future use.
  enum BitLayout : uint16_t
  {
    Bits_ShapeType_Mask  = 0x000F, //!< bits 0-3: shape type mask
    Bits_ShapeType_Shift = 0,      //!< shift for shape type
    Bit_Free             = 0x0010, //!< bit 4: free flag
    Bit_Modified         = 0x0020, //!< bit 5: modified flag
    Bit_Checked          = 0x0040, //!< bit 6: checked flag
    Bit_Orientable       = 0x0080, //!< bit 7: orientable flag
    Bit_Closed           = 0x0100, //!< bit 8: closed flag
    Bit_Infinite         = 0x0200, //!< bit 9: infinite flag
    Bit_Convex           = 0x0400, //!< bit 10: convex flag
    Bit_Locked           = 0x0800, //!< bit 11: locked flag
    Bits_Reserved        = 0xF000  //!< bits 12-15: reserved
  };

public:
  //! Returns the free flag.
  bool Free() const { return (myState & Bit_Free) != 0; }

  //! Sets the free flag.
  void Free(bool theIsFree) { setBit(Bit_Free, theIsFree); }

  //! Returns the locked flag.
  bool Locked() const { return (myState & Bit_Locked) != 0; }

  //! Sets the locked flag.
  void Locked(bool theIsLocked) { setBit(Bit_Locked, theIsLocked); }

  //! Returns the modification flag.
  bool Modified() const { return (myState & Bit_Modified) != 0; }

  //! Sets the modification flag.
  void Modified(bool theIsModified)
  {
    setBit(Bit_Modified, theIsModified);
    if (theIsModified)
    {
      // when a TShape is modified it is also unchecked
      setBit(Bit_Checked, false);
    }
  }

  //! Returns the checked flag.
  bool Checked() const { return (myState & Bit_Checked) != 0; }

  //! Sets the checked flag.
  void Checked(bool theIsChecked) { setBit(Bit_Checked, theIsChecked); }

  //! Returns the orientability flag.
  bool Orientable() const { return (myState & Bit_Orientable) != 0; }

  //! Sets the orientability flag.
  void Orientable(bool theIsOrientable) { setBit(Bit_Orientable, theIsOrientable); }

  //! Returns the closedness flag.
  bool Closed() const { return (myState & Bit_Closed) != 0; }

  //! Sets the closedness flag.
  void Closed(bool theIsClosed) { setBit(Bit_Closed, theIsClosed); }

  //! Returns the infinity flag.
  bool Infinite() const { return (myState & Bit_Infinite) != 0; }

  //! Sets the infinity flag.
  void Infinite(bool theIsInfinite) { setBit(Bit_Infinite, theIsInfinite); }

  //! Returns the convexness flag.
  bool Convex() const { return (myState & Bit_Convex) != 0; }

  //! Sets the convexness flag.
  void Convex(bool theIsConvex) { setBit(Bit_Convex, theIsConvex); }

  //! Returns the type as a term of the ShapeEnum enum:
  //! VERTEX, EDGE, WIRE, FACE, SHELL, SOLID, COMPSOLID, COMPOUND.
  //! The type is embedded in the lower 4 bits of the state.
  TopAbs_ShapeEnum ShapeType() const
  {
    return static_cast<TopAbs_ShapeEnum>(myState & Bits_ShapeType_Mask);
  }

  //! Returns a copy of the TShape with no sub-shapes.
  Standard_EXPORT virtual occ::handle<TopoDS_TShape> EmptyCopy() const = 0;

  //! Returns the number of direct sub-shapes (children).
  //! @sa TopoDS_Iterator for accessing sub-shapes
  virtual int NbChildren() const = 0;

  //! Returns the child shape at the given index (0-based).
  //! @param theIndex the 0-based index of the child
  //! @return the child shape at the given index
  virtual const TopoDS_Shape& GetChild(size_t theIndex) const = 0;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  friend class TopoDS_Iterator;
  friend class TopoDS_Builder;

  DEFINE_STANDARD_RTTIEXT(TopoDS_TShape, Standard_Transient)

protected:
  //! Constructs a TShape with the given shape type.
  //! Default flags: Free = true, Modified = true, Orientable = true.
  //! @param theType the shape type to embed in the state
  TopoDS_TShape(TopAbs_ShapeEnum theType)
      : myState(static_cast<uint16_t>(theType) | Bit_Free | Bit_Modified | Bit_Orientable)
  {
  }

  //! Set a bit flag.
  //! @param theBit the bit to set
  //! @param theIsOn true to set, false to clear
  void setBit(uint16_t theBit, bool theIsOn)
  {
    if (theIsOn)
      myState |= theBit;
    else
      myState &= ~theBit;
  }

protected:
  uint16_t
    myState; //!< Compact state: shape type (bits 0-3) + flags (bits 4-11) + reserved (bits 12-15)
};

#endif // _TopoDS_TShape_HeaderFile
