// Created on: 1990-12-11
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

#ifndef _TopoDS_Shape_HeaderFile
#define _TopoDS_Shape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopLoc_Location.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
class TopoDS_TShape;

// resolve name collisions with X11 headers
#ifdef Convex
  #undef Convex
#endif

//! Describes a shape which
//! - references an underlying shape with the potential
//! to be given a location and an orientation
//! - has a location for the underlying shape, giving its
//! placement in the local coordinate system
//! - has an orientation for the underlying shape, in
//! terms of its geometry (as opposed to orientation in
//! relation to other shapes).
//! Note: A Shape is empty if it references an underlying
//! shape which has an empty list of shapes.
class TopoDS_Shape 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a NULL Shape referring to nothing.
    TopoDS_Shape();
  
  //! Returns true if this shape is null. In other words, it
  //! references no underlying shape with the potential to
  //! be given a location and an orientation.
    Standard_Boolean IsNull() const;
  
  //! Destroys the reference to the underlying shape
  //! stored in this shape. As a result, this shape becomes null.
    void Nullify();
  
  //! Returns the shape local coordinate system.
    const TopLoc_Location& Location() const;
  
  //! Sets the shape local coordinate system.
    void Location (const TopLoc_Location& Loc);
  
  //! Returns a  shape  similar to <me> with   the local
  //! coordinate system set to <Loc>.
    TopoDS_Shape Located (const TopLoc_Location& Loc) const;
  
  //! Returns the shape orientation.
    TopAbs_Orientation Orientation() const;
  
  //! Sets the shape orientation.
    void Orientation (const TopAbs_Orientation Orient);
  
  //! Returns  a    shape  similar  to  <me>   with  the
  //! orientation set to <Or>.
    TopoDS_Shape Oriented (const TopAbs_Orientation Or) const;
  
    const Handle(TopoDS_TShape)& TShape() const;
  
  //! Returns the value of the TopAbs_ShapeEnum
  //! enumeration that corresponds to this shape, for
  //! example VERTEX, EDGE, and so on.
  //! Exceptions
  //! Standard_NullObject if this shape is null.
    TopAbs_ShapeEnum ShapeType() const;
  
  //! Returns the free flag.
    Standard_Boolean Free() const;
  
  //! Sets the free flag.
    void Free (const Standard_Boolean F);
  
  //! Returns the locked flag.
    Standard_Boolean Locked() const;
  
  //! Sets the locked flag.
    void Locked (const Standard_Boolean F);
  
  //! Returns the modification flag.
    Standard_Boolean Modified() const;
  
  //! Sets the modification flag.
    void Modified (const Standard_Boolean M);
  
  //! Returns the checked flag.
    Standard_Boolean Checked() const;
  
  //! Sets the checked flag.
    void Checked (const Standard_Boolean C);
  
  //! Returns the orientability flag.
    Standard_Boolean Orientable() const;
  
  //! Sets the orientability flag.
    void Orientable (const Standard_Boolean C);
  
  //! Returns the closedness flag.
    Standard_Boolean Closed() const;
  
  //! Sets the closedness flag.
    void Closed (const Standard_Boolean C);
  
  //! Returns the infinity flag.
    Standard_Boolean Infinite() const;
  
  //! Sets the infinity flag.
    void Infinite (const Standard_Boolean C);
  
  //! Returns the convexness flag.
    Standard_Boolean Convex() const;
  
  //! Sets the convexness flag.
    void Convex (const Standard_Boolean C);
  
  //! Multiplies the Shape location by <position>.
    void Move (const TopLoc_Location& position);
  
  //! Returns  a shape similar  to  <me> with a location
  //! multiplied  by <position>.
    TopoDS_Shape Moved (const TopLoc_Location& position) const;
  
  //! Reverses the orientation, using the Reverse method
  //! from the TopAbs package.
    void Reverse();
  
  //! Returns    a shape  similar    to  <me>  with  the
  //! orientation  reversed, using  the   Reverse method
  //! from the TopAbs package.
    TopoDS_Shape Reversed() const;
  
  //! Complements the orientation, using the  Complement
  //! method from the TopAbs package.
    void Complement();
  
  //! Returns  a   shape  similar  to   <me>   with  the
  //! orientation complemented,  using   the  Complement
  //! method from the TopAbs package.
    TopoDS_Shape Complemented() const;
  
  //! Updates the  Shape Orientation by composition with
  //! <Orient>, using the Compose method from the TopAbs
  //! package.
    void Compose (const TopAbs_Orientation Orient);
  
  //! Returns  a  shape   similar   to  <me>   with  the
  //! orientation  composed  with <Orient>,   using  the
  //! Compose method from the TopAbs package.
    TopoDS_Shape Composed (const TopAbs_Orientation Orient) const;
  
  //! Returns True if two shapes  are partners, i.e.  if
  //! they   share   the   same  TShape.  Locations  and
  //! Orientations may differ.
    Standard_Boolean IsPartner (const TopoDS_Shape& other) const;
  
  //! Returns True if two shapes are same, i.e.  if they
  //! share  the  same TShape  with the same  Locations.
  //! Orientations may differ.
    Standard_Boolean IsSame (const TopoDS_Shape& other) const;
  
  //! Returns True if two shapes are equal, i.e. if they
  //! share the same TShape with  the same Locations and
  //! Orientations.
    Standard_Boolean IsEqual (const TopoDS_Shape& other) const;
  Standard_Boolean operator == (const TopoDS_Shape& other) const
{
  return IsEqual(other);
}
  
  //! Negation of the IsEqual method.
    Standard_Boolean IsNotEqual (const TopoDS_Shape& other) const;
  Standard_Boolean operator != (const TopoDS_Shape& other) const
{
  return IsNotEqual(other);
}
  
  //! Returns a hashed value  denoting <me>.  This value
  //! is in the range  1..<Upper>.  It is  computed from
  //! the  TShape  and the  Location. The Orientation is
  //! not used.
  Standard_EXPORT Standard_Integer HashCode (const Standard_Integer Upper) const;
  
  //! Replace   <me> by  a  new   Shape with the    same
  //! Orientation and Location and a new TShape with the
  //! same geometry and no sub-shapes.
    void EmptyCopy();
  
  //! Returns a new Shape with the  same Orientation and
  //! Location and  a new TShape  with the same geometry
  //! and no sub-shapes.
    TopoDS_Shape EmptyCopied() const;
  
    void TShape (const Handle(TopoDS_TShape)& T);




protected:





private:



  Handle(TopoDS_TShape) myTShape;
  TopLoc_Location myLocation;
  TopAbs_Orientation myOrient;


};


#include <TopoDS_Shape.lxx>



inline Standard_Integer HashCode(const TopoDS_Shape& me,const Standard_Integer Upper) {
 return me.HashCode(Upper);
}



#endif // _TopoDS_Shape_HeaderFile
