// Created on: 1998-06-03
// Created by: data exchange team
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

#ifndef _ShapeExtend_WireData_HeaderFile
#define _ShapeExtend_WireData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopTools_HSequenceOfShape.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class TopoDS_Wire;
class TopoDS_Edge;
class TopoDS_Shape;
class TopoDS_Face;


class ShapeExtend_WireData;
DEFINE_STANDARD_HANDLE(ShapeExtend_WireData, Standard_Transient)

//! This class provides a data structure necessary for work with the wire as with
//! ordered list of edges, what is required for many algorithms. The advantage of
//! this class is that it allows to work with wires which are not correct.
//! The object of the class ShapeExtend_WireData can be initialized by
//! TopoDS_Wire, and converted back to TopoDS_Wire.
//! An edge in the wire is defined by its rank number. Operations of accessing,
//! adding and removing edge at the given rank number are provided. On the whole
//! wire, operations of circular permutation and reversing (both orientations of
//! all edges and order of edges) are provided as well.
//! This class also provides a method to check if the edge in the wire is a seam
//! (if the wire lies on a face).
//! This class is handled by reference. Such an approach gives the following advantages:
//! 1.    Sharing the object of this class strongly optimizes the processes of
//! analysis and fixing performed in parallel on the wire stored in the form
//! of this class. Fixing tool (e.g. ShapeFix_Wire) fixes problems one by
//! one using analyzing tool (e.g. ShapeAnalysis_Wire). Sharing allows not
//! to reinitialize each time the analyzing tool with modified
//! ShapeExtend_WireData what consumes certain time.
//! 2.    No copying of contents. The object of ShapeExtend_WireData class has
//! quite big size, returning it as a result of the function would cause
//! additional copying of contents if this class were one handled by value.
//! Moreover, this class is stored as a field in other classes which are
//! they returned as results of functions, storing only a handle to
//! ShapeExtend_WireData saves time and memory.
class ShapeExtend_WireData : public Standard_Transient
{

public:

  
  //! Empty constructor, creates empty wire with no edges
  Standard_EXPORT ShapeExtend_WireData();
  
  //! Constructor initializing the data from TopoDS_Wire. Calls Init(wire,chained).
  Standard_EXPORT ShapeExtend_WireData(const TopoDS_Wire& wire, const Standard_Boolean chained = Standard_True, const Standard_Boolean theManifoldMode = Standard_True);
  
  //! Copies data from another WireData
  Standard_EXPORT void Init (const Handle(ShapeExtend_WireData)& other);
  
  //! Loads an already existing wire
  //! If <chained> is True (default), edges are added in the
  //! sequence as they are explored by TopoDS_Iterator
  //! Else, if <chained> is False, wire is explored by
  //! BRepTools_WireExplorer and it is guaranteed that edges will
  //! be sequencially connected.
  //! Remark : In the latter case it can happen that not all edges
  //! will be found (because of limitations of
  //! BRepTools_WireExplorer for disconnected wires and wires
  //! with seam edges).
  Standard_EXPORT Standard_Boolean Init (const TopoDS_Wire& wire, const Standard_Boolean chained = Standard_True, const Standard_Boolean theManifoldMode = Standard_True);
  
  //! Clears data about Wire.
  Standard_EXPORT void Clear();
  
  //! Computes the list of seam edges
  //! By default (direct call), computing is enforced
  //! For indirect call (from IsSeam) it is redone only if not yet
  //! already done or if the list of edges has changed
  //! Remark : A Seam Edge is an Edge present twice in the list, once as
  //! FORWARD and once as REVERSED
  //! Each sense has its own PCurve, the one for FORWARD
  //! must be set in first
  Standard_EXPORT void ComputeSeams (const Standard_Boolean enforce = Standard_True);
  
  //! Does a circular permutation in order to set <num>th edge last
  Standard_EXPORT void SetLast (const Standard_Integer num);
  
  //! When the wire contains at least one degenerated edge, sets it
  //! as last one
  //! Note   : It is useful to process pcurves, for instance, while the pcurve
  //! of a DGNR may not be computed from its 3D part (there is none)
  //! it is computed after the other edges have been computed and
  //! chained.
  Standard_EXPORT void SetDegeneratedLast();
  
  //! Adds an edge to a wire, being defined (not yet ended)
  //! This is the plain, basic, function to add an edge
  //! <num> = 0 (D): Appends at end
  //! <num> = 1: Preprends at start
  //! else, Insert before <num>
  //! Remark : Null Edge is simply ignored
  Standard_EXPORT void Add (const TopoDS_Edge& edge, const Standard_Integer atnum = 0);
  
  //! Adds an entire wire, considered as a list of edges
  //! Remark : The wire is assumed to be ordered (TopoDS_Iterator
  //! is used)
  Standard_EXPORT void Add (const TopoDS_Wire& wire, const Standard_Integer atnum = 0);
  
  //! Adds a wire in the form of WireData
  Standard_EXPORT void Add (const Handle(ShapeExtend_WireData)& wire, const Standard_Integer atnum = 0);
  
  //! Adds an edge or a wire invoking corresponding method Add
  Standard_EXPORT void Add (const TopoDS_Shape& shape, const Standard_Integer atnum = 0);
  
  //! Adds an edge to start or end of <me>, according to <mode>
  //! 0: at end, as direct
  //! 1: at end, as reversed
  //! 2: at start, as direct
  //! 3: at start, as reversed
  //! < 0: no adding
  Standard_EXPORT void AddOriented (const TopoDS_Edge& edge, const Standard_Integer mode);
  
  //! Adds a wire to start or end of <me>, according to <mode>
  //! 0: at end, as direct
  //! 1: at end, as reversed
  //! 2: at start, as direct
  //! 3: at start, as reversed
  //! < 0: no adding
  Standard_EXPORT void AddOriented (const TopoDS_Wire& wire, const Standard_Integer mode);
  
  //! Adds an edge or a wire invoking corresponding method
  //! AddOriented
  Standard_EXPORT void AddOriented (const TopoDS_Shape& shape, const Standard_Integer mode);
  
  //! Removes an Edge, given its rank. By default removes the last edge.
  Standard_EXPORT void Remove (const Standard_Integer num = 0);
  
  //! Replaces an edge at the given
  //! rank number <num> with new one. Default is last edge (<num> = 0).
  Standard_EXPORT void Set (const TopoDS_Edge& edge, const Standard_Integer num = 0);
  
  //! Reverses the sense of the list and the orientation of each Edge
  //! This method should be called when either wire has no seam edges
  //! or face is not available
  Standard_EXPORT void Reverse();
  
  //! Reverses the sense of the list and the orientation of each Edge
  //! The face is necessary for swapping pcurves for seam edges
  //! (first pcurve corresponds to orientation FORWARD, and second to
  //! REVERSED; when edge is reversed, pcurves must be swapped)
  //! If face is NULL, no swapping is performed
  Standard_EXPORT void Reverse (const TopoDS_Face& face);
  
  //! Returns the count of currently recorded edges
  Standard_EXPORT Standard_Integer NbEdges() const;
  
  //! Returns the count of currently recorded non-manifold edges
  Standard_EXPORT Standard_Integer NbNonManifoldEdges() const;
  
  //! Returns <num>th nonmanifold Edge
  Standard_EXPORT TopoDS_Edge NonmanifoldEdge (const Standard_Integer num) const;
  
  //! Returns sequence of non-manifold edges
  //! This sequence can be not empty if wire data set in manifold mode but
  //! initial wire has INTERNAL orientation or contains INTERNAL edges
  Standard_EXPORT Handle(TopTools_HSequenceOfShape) NonmanifoldEdges() const;
  
  //! Returns mode defining manifold wire data or not.
  //! If manifold that nonmanifold edges will not be not
  //! consider during operations(previous behaviour)
  //! and they will be added only in result wire
  //! else non-manifold edges will consider during operations
  Standard_EXPORT Standard_Boolean& ManifoldMode();
  
  //! Returns <num>th Edge
  Standard_EXPORT TopoDS_Edge Edge (const Standard_Integer num) const;
  
  //! Returns the index of the edge
  //! If the edge is a seam the orientation is also checked
  //! Returns 0 if the edge is not found in the list
  Standard_EXPORT Standard_Integer Index (const TopoDS_Edge& edge);
  
  //! Tells if an Edge is seam (see ComputeSeams)
  //! An edge is considered as seam if it presents twice in
  //! the edge list, once as FORWARD and once as REVERSED.
  Standard_EXPORT Standard_Boolean IsSeam (const Standard_Integer num);
  
  //! Makes TopoDS_Wire using
  //! BRep_Builder (just creates the TopoDS_Wire object and adds
  //! all edges into it). This method should be called when
  //! the wire is correct (for example, after successful
  //! fixes by ShapeFix_Wire) and adjacent edges share common
  //! vertices. In case if adjacent edges do not share the same
  //! vertices the resulting TopoDS_Wire will be invalid.
  Standard_EXPORT TopoDS_Wire Wire() const;
  
  //! Makes TopoDS_Wire using
  //! BRepAPI_MakeWire. Class BRepAPI_MakeWire merges
  //! geometrically coincided vertices and can disturb
  //! correct order of edges in the wire. If this class fails,
  //! null shape is returned.
  Standard_EXPORT TopoDS_Wire WireAPIMake() const;




  DEFINE_STANDARD_RTTIEXT(ShapeExtend_WireData,Standard_Transient)

protected:




private:


  Handle(TopTools_HSequenceOfShape) myEdges;
  Handle(TopTools_HSequenceOfShape) myNonmanifoldEdges;
  Handle(TColStd_HSequenceOfInteger) mySeams;
  Standard_Integer mySeamF;
  Standard_Integer mySeamR;
  Standard_Boolean myManifoldMode;


};







#endif // _ShapeExtend_WireData_HeaderFile
