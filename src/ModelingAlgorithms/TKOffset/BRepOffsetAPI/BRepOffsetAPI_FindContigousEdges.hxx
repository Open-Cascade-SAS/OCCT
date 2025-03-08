// Created on: 1995-05-02
// Created by: Jing Cheng MEI
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepOffsetAPI_FindContigousEdges_HeaderFile
#define _BRepOffsetAPI_FindContigousEdges_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
class BRepBuilderAPI_Sewing;
class TopoDS_Shape;
class TopoDS_Edge;

//! Provides methods to identify contiguous boundaries for continuity control (C0, C1, ...)
//!
//! Use this function as following:
//! - create an object
//! - default tolerance 1.E-06
//! - with analysis of degenerated faces on
//! - define if necessary a new tolerance
//! - set if necessary analysis of degenerated shapes off
//! - add shapes to be controlled -> Add
//! - compute -> Perform
//! - output couples of connected edges for control
//! - output the problems if any
class BRepOffsetAPI_FindContigousEdges
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes an algorithm for identifying contiguous edges
  //! on shapes with tolerance as the tolerance of contiguity
  //! (defaulted to 1.0e-6). This tolerance value is used to
  //! determine whether two edges or sections of edges are coincident.
  //! Use the function Add to define the shapes to be checked.
  //! Set option to false. This argument (defaulted to true) will
  //! serve in subsequent software releases for performing an
  //! analysis of degenerated shapes.
  Standard_EXPORT BRepOffsetAPI_FindContigousEdges(const Standard_Real    tolerance = 1.0e-06,
                                                   const Standard_Boolean option = Standard_True);

  //! Initializes this algorithm for identifying contiguous edges
  //! on shapes using the tolerance of contiguity tolerance.
  //! This tolerance value is used to determine whether two
  //! edges or sections of edges are coincident.
  //! Use the function Add to define the shapes to be checked.
  //! Sets <option> to false.
  Standard_EXPORT void Init(const Standard_Real tolerance, const Standard_Boolean option);

  //! Adds the shape shape to the list of shapes to be
  //! checked by this algorithm.
  //! Once all the shapes to be checked have been added,
  //! use the function Perform to find the contiguous edges
  //! and the function ContigousEdge to return these edges.
  Standard_EXPORT void Add(const TopoDS_Shape& shape);

  //! Finds coincident parts of edges of two or more shapes
  //! added to this algorithm and breaks down these edges
  //! into contiguous and non-contiguous sections on copies
  //! of the initial shapes.
  //! The function ContigousEdge returns contiguous
  //! edges. The function Modified can be used to return
  //! modified copies of the initial shapes where one or more
  //! edges were broken down into contiguous and non-contiguous sections.
  //! Warning
  //! This function must be used once all the shapes to be
  //! checked have been added. It is not possible to add
  //! further shapes subsequently and then to repeat the call to Perform.
  Standard_EXPORT void Perform();

  //! Gives the number of edges (free edges + contiguous edges + multiple edge)
  Standard_EXPORT Standard_Integer NbEdges() const;

  //! Returns the number of contiguous edges found by the
  //! function Perform on the shapes added to this algorithm.
  Standard_EXPORT Standard_Integer NbContigousEdges() const;

  //! Returns the contiguous edge of index index found by
  //! the function Perform on the shapes added to this algorithm.
  //! Exceptions
  //! Standard_OutOfRange if:
  //! - index is less than 1, or
  //! - index is greater than the number of contiguous
  //! edges found by the function Perform on the shapes added to this algorithm.
  Standard_EXPORT const TopoDS_Edge& ContigousEdge(const Standard_Integer index) const;

  //! Returns a list of edges coincident with the contiguous
  //! edge of index index found by the function Perform.
  //! There are as many edges in the list as there are faces
  //! adjacent to this contiguous edge.
  //! Exceptions
  //! Standard_OutOfRange if:
  //! - index is less than 1, or
  //! - index is greater than the number of contiguous edges
  //! found by the function Perform on the shapes added to this algorithm.
  Standard_EXPORT const TopTools_ListOfShape& ContigousEdgeCouple(
    const Standard_Integer index) const;

  //! Returns the edge on the initial shape, of which the
  //! modified copy contains the edge section.
  //! section is coincident with a contiguous edge found by
  //! the function Perform. Use the function
  //! ContigousEdgeCouple to obtain a valid section.
  //! This information is useful for verification purposes, since
  //! it provides a means of determining the surface to which
  //! the contiguous edge belongs.
  //! Exceptions
  //! Standard_NoSuchObject if section is not coincident
  //! with a contiguous edge. Use the function
  //! ContigousEdgeCouple to obtain a valid section.
  Standard_EXPORT const TopoDS_Edge& SectionToBoundary(const TopoDS_Edge& section) const;

  //! Gives the number of degenerated shapes
  Standard_EXPORT Standard_Integer NbDegeneratedShapes() const;

  //! Gives a degenerated shape
  Standard_EXPORT const TopoDS_Shape& DegeneratedShape(const Standard_Integer index) const;

  //! Indicates if a input shape is degenerated
  Standard_EXPORT Standard_Boolean IsDegenerated(const TopoDS_Shape& shape) const;

  //! Returns true if the copy of the initial shape shape was
  //! modified by the function Perform (i.e. if one or more of
  //! its edges was broken down into contiguous and non-contiguous sections).
  //! Warning
  //! Returns false if shape is not one of the initial shapes
  //! added to this algorithm.
  Standard_EXPORT Standard_Boolean IsModified(const TopoDS_Shape& shape) const;

  //! Gives a modifieded shape
  //! Raises   NoSuchObject if shape has not been modified
  Standard_EXPORT const TopoDS_Shape& Modified(const TopoDS_Shape& shape) const;

  //! Dump properties of resulting shape.
  Standard_EXPORT void Dump() const;

protected:
private:
  Handle(BRepBuilderAPI_Sewing) mySewing;
};

#endif // _BRepOffsetAPI_FindContigousEdges_HeaderFile
