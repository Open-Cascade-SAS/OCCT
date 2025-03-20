// Created on: 1998-06-03
// Created by: Daniel RISSER
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

#ifndef _ShapeAnalysis_FreeBounds_HeaderFile
#define _ShapeAnalysis_FreeBounds_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Compound.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
class TopoDS_Shape;

//! This class is intended to output free bounds of the shape.
//!
//! Free bounds are the wires consisting of edges referenced by the faces of the shape
//! only once; these are the edges composing the outer boundary of the face or shell
//! (as opposed to internal edges between the faces in the shell or seam edges on closed faces).
//!
//! This class works on two distinct types of shapes when analyzing
//! their free bounds:
//! 1. compound of faces.
//! Analyzer of sewing algorithm (BRepAlgo_Sewing) is used for
//! for forecasting free bounds that would be obtained after
//! performing sewing
//! 2. compound of shells.
//! Actual free bounds (edges shared by the only face in the shell)
//! are output in this case. ShapeAnalysis_Shell is used for that.
//!
//! When connecting edges into the wires algorithm tries to build
//! wires of maximum length. Two options are provided for a user
//! to extract closed sub-contours out of closed and/or open contours.
//!
//! Free bounds are returned as two compounds, one for closed and one
//! for open wires.
//!
//! This class also provides some static methods for advanced use:
//! connecting edges/wires to wires, extracting closed sub-wires out
//! of wires, dispatching wires into compounds for closed and open
//! wires.
//! NOTE. Ends of the edge or wire mean hereafter their end vertices.
class ShapeAnalysis_FreeBounds
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_FreeBounds();

  //! Builds forecasting free bounds of the <shape>.
  //! <shape> should be a compound of faces.
  //! This constructor is to be used for forecasting free edges
  //! with help of sewing analyzer BRepAlgo_Sewing which is called
  //! with tolerance <toler>.
  //! Free edges are connected into wires only when their ends are
  //! at distance less than <toler>.
  //! If <splitclosed> is True extracts closed sub-wires out of
  //! built closed wires.
  //! If <splitopen> is True extracts closed sub-wires out of
  //! built open wires.
  Standard_EXPORT ShapeAnalysis_FreeBounds(const TopoDS_Shape&    shape,
                                           const Standard_Real    toler,
                                           const Standard_Boolean splitclosed = Standard_False,
                                           const Standard_Boolean splitopen   = Standard_True);

  //! Builds actual free bounds of the <shape>.
  //! <shape> should be a compound of shells.
  //! This constructor is to be used for getting free edges (ones
  //! referenced by the only face) with help of analyzer
  //! ShapeAnalysis_Shell.
  //! Free edges are connected into wires only when they share the
  //! same vertex.
  //! If <splitclosed> is True extracts closed sub-wires out of
  //! built closed wires.
  //! If <splitopen> is True extracts closed sub-wires out of
  //! built open wires.
  Standard_EXPORT ShapeAnalysis_FreeBounds(
    const TopoDS_Shape&    shape,
    const Standard_Boolean splitclosed        = Standard_False,
    const Standard_Boolean splitopen          = Standard_True,
    const Standard_Boolean checkinternaledges = Standard_False);

  //! Returns compound of closed wires out of free edges.
  const TopoDS_Compound& GetClosedWires() const;

  //! Returns compound of open wires out of free edges.
  const TopoDS_Compound& GetOpenWires() const;

  //! Builds sequence of <wires> out of sequence of not sorted
  //! <edges>.
  //! Tries to build wires of maximum length. Building a wire is
  //! stopped when no edges can be connected to it at its head or
  //! at its tail.
  //!
  //! Orientation of the edge can change when connecting.
  //! If <shared> is True connection is performed only when
  //! adjacent edges share the same vertex.
  //! If <shared> is False connection is performed only when
  //! ends of adjacent edges are at distance less than <toler>.
  Standard_EXPORT static void ConnectEdgesToWires(Handle(TopTools_HSequenceOfShape)& edges,
                                                  const Standard_Real                toler,
                                                  const Standard_Boolean             shared,
                                                  Handle(TopTools_HSequenceOfShape)& wires);

  Standard_EXPORT static void ConnectWiresToWires(Handle(TopTools_HSequenceOfShape)& iwires,
                                                  const Standard_Real                toler,
                                                  const Standard_Boolean             shared,
                                                  Handle(TopTools_HSequenceOfShape)& owires);

  //! Builds sequence of <owires> out of sequence of not sorted
  //! <iwires>.
  //! Tries to build wires of maximum length. Building a wire is
  //! stopped when no wires can be connected to it at its head or
  //! at its tail.
  //!
  //! Orientation of the wire can change when connecting.
  //! If <shared> is True connection is performed only when
  //! adjacent wires share the same vertex.
  //! If <shared> is False connection is performed only when
  //! ends of adjacent wires are at distance less than <toler>.
  //! Map <vertices> stores the correspondence between original
  //! end vertices of the wires and new connecting vertices.
  Standard_EXPORT static void ConnectWiresToWires(Handle(TopTools_HSequenceOfShape)& iwires,
                                                  const Standard_Real                toler,
                                                  const Standard_Boolean             shared,
                                                  Handle(TopTools_HSequenceOfShape)& owires,
                                                  TopTools_DataMapOfShapeShape&      vertices);

  //! Extracts closed sub-wires out of <wires> and adds them
  //! to <closed>, open wires remained after extraction are put
  //! into <open>.
  //! If <shared> is True extraction is performed only when
  //! edges share the same vertex.
  //! If <shared> is False connection is performed only when
  //! ends of the edges are at distance less than <toler>.
  Standard_EXPORT static void SplitWires(const Handle(TopTools_HSequenceOfShape)& wires,
                                         const Standard_Real                      toler,
                                         const Standard_Boolean                   shared,
                                         Handle(TopTools_HSequenceOfShape)&       closed,
                                         Handle(TopTools_HSequenceOfShape)&       open);

  //! Dispatches sequence of <wires> into two compounds
  //! <closed> for closed wires and <open> for open wires.
  //! If a compound is not empty wires are added into it.
  Standard_EXPORT static void DispatchWires(const Handle(TopTools_HSequenceOfShape)& wires,
                                            TopoDS_Compound&                         closed,
                                            TopoDS_Compound&                         open);

protected:
private:
  Standard_EXPORT void SplitWires();

  TopoDS_Compound  myWires;
  TopoDS_Compound  myEdges;
  Standard_Real    myTolerance;
  Standard_Boolean myShared;
  Standard_Boolean mySplitClosed;
  Standard_Boolean mySplitOpen;
};

#include <ShapeAnalysis_FreeBounds.lxx>

#endif // _ShapeAnalysis_FreeBounds_HeaderFile
