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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_ListOfShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>

class TopoDS_Iterator;
class TopoDS_Builder;

// resolve name collisions with X11 headers
#ifdef Convex
  #undef Convex
#endif

class TopoDS_TShape;
DEFINE_STANDARD_HANDLE(TopoDS_TShape, Standard_Transient)

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
  
  //! Returns the type as a term of the ShapeEnum enum :
  //! VERTEX, EDGE, WIRE, FACE, ....
  Standard_EXPORT virtual TopAbs_ShapeEnum ShapeType() const = 0;
  
  //! Returns a copy  of the  TShape  with no sub-shapes.
  Standard_EXPORT virtual Handle(TopoDS_TShape) EmptyCopy() const = 0;


friend class TopoDS_Iterator;
friend class TopoDS_Builder;


  DEFINE_STANDARD_RTTIEXT(TopoDS_TShape,Standard_Transient)

protected:

  
  //! Constructs an empty TShape.
  //! Free       : True
  //! Modified   : True
  //! Checked    : False
  //! Orientable : True
  //! Closed     : False
  //! Infinite   : False
  //! Convex     : False
    TopoDS_TShape();



private:

  
    const TopoDS_ListOfShape& Shapes() const;
  
    TopoDS_ListOfShape& ChangeShapes();

  TopoDS_ListOfShape myShapes;
  Standard_Integer myFlags;


};


#include <TopoDS_TShape.lxx>





#endif // _TopoDS_TShape_HeaderFile
