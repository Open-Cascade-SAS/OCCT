// Created on: 1998-09-16
// Created by: Roman LYGIN <rln@nnov.matra-dtv.fr>
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeFix_FreeBounds_HeaderFile
#define _ShapeFix_FreeBounds_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

//! This class is intended to output free bounds of the shape
//! (free bounds are the wires consisting of edges referenced by the
//! only face).
//! For building free bounds it uses ShapeAnalysis_FreeBounds class.
//! This class complements it with the feature to reduce the number
//! of open wires.
//! This reduction is performed with help of connecting several
//! adjacent open wires one to another what can lead to:
//! 1. making an open wire with greater length out of several
//! open wires
//! 2. making closed wire out of several open wires
//!
//! The connecting open wires is performed with a user-given
//! tolerance.
//!
//! When connecting several open wires into one wire their previous
//! end vertices are replaced with new connecting vertices. After
//! that all the edges in the shape sharing previous vertices inside
//! the shape are updated with new vertices. Thus source shape can
//! be modified.
//!
//! Since interface of this class is the same as one of
//! ShapeAnalysis_FreeBounds refer to its CDL for details.
class ShapeFix_FreeBounds
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT ShapeFix_FreeBounds();

  //! Builds forecasting free bounds of the <shape> and connects
  //! open wires with tolerance <closetoler>.
  //! <shape> should be a compound of faces.
  //! Tolerance <closetoler> should be greater than tolerance
  //! <sewtoler> used for initializing sewing analyzer, otherwise
  //! connection of open wires is not performed.
  Standard_EXPORT ShapeFix_FreeBounds(const TopoDS_Shape&    shape,
                                      const Standard_Real    sewtoler,
                                      const Standard_Real    closetoler,
                                      const Standard_Boolean splitclosed,
                                      const Standard_Boolean splitopen);

  //! Builds actual free bounds of the <shape> and connects
  //! open wires with tolerance <closetoler>.
  //! <shape> should be a compound of shells.
  Standard_EXPORT ShapeFix_FreeBounds(const TopoDS_Shape&    shape,
                                      const Standard_Real    closetoler,
                                      const Standard_Boolean splitclosed,
                                      const Standard_Boolean splitopen);

  //! Returns compound of closed wires out of free edges.
  const TopoDS_Compound& GetClosedWires() const;

  //! Returns compound of open wires out of free edges.
  const TopoDS_Compound& GetOpenWires() const;

  //! Returns modified source shape.
  const TopoDS_Shape& GetShape() const;

protected:
private:
  Standard_EXPORT Standard_Boolean Perform();

  TopoDS_Compound  myWires;
  TopoDS_Compound  myEdges;
  TopoDS_Shape     myShape;
  Standard_Boolean myShared;
  Standard_Real    mySewToler;
  Standard_Real    myCloseToler;
  Standard_Boolean mySplitClosed;
  Standard_Boolean mySplitOpen;
};

#include <ShapeFix_FreeBounds.lxx>

#endif // _ShapeFix_FreeBounds_HeaderFile
