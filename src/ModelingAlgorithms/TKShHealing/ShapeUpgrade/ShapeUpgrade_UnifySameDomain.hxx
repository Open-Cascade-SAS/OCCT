// Copyright: Open CASCADE 2014
// Created on: 2012-06-09
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_UnifySameDomain_HeaderFile
#define _ShapeUpgrade_UnifySameDomain_HeaderFile

#include <BRepTools_History.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
class ShapeBuild_ReShape;

//! This tool tries to unify faces and edges of the shape which lie on the same geometry.
//! Faces/edges are considering as 'same-domain' if a group of neighbouring faces/edges
//! are lying on coincident surfaces/curves.
//! In this case these faces/edges can be unified into one face/edge.
//! ShapeUpgrade_UnifySameDomain is initialized by a shape and the next optional parameters:
//! UnifyFaces - tries to unify all possible faces
//! UnifyEdges - tries to unify all possible edges
//! ConcatBSplines - if this flag is set to true then all neighbouring edges, which lay
//! on BSpline or Bezier curves with C1 continuity on their common vertices,
//! will be merged into one common edge.
//!
//! The input shape can be of any type containing faces or edges - compsolid, solid, shell,
//! wire, compound of any kind of shapes. The algorithm preserves the structure of compsolids,
//! solids, shells and wires. E.g., if two shells have a common edge and the faces sharing
//! this edge lie on the same surface the algorithm will not unify these faces, otherwise
//! the structure of shells would be broken. However, if such faces belong to different
//! compounds of faces they will be unified.
//!
//! The output result of the tool is the unified shape.
//!
//! All the modifications of initial shape are recorded during unifying.
//! Methods History are intended to:
//! - set a place holder for the history of modifications of sub-shapes of
//!   the initial shape;
//! - get the collected history.
//! The algorithm provides a place holder for the history and collects the
//! history by default.
//! To avoid collecting of the history the place holder should be set to null handle.
class ShapeUpgrade_UnifySameDomain : public Standard_Transient
{

public:
  typedef NCollection_DataMap<TopoDS_Shape, occ::handle<Geom_Plane>, TopTools_ShapeMapHasher>
    DataMapOfFacePlane;
  typedef NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>, TopTools_ShapeMapHasher>
    DataMapOfShapeMapOfShape;

  //! Empty constructor
  Standard_EXPORT ShapeUpgrade_UnifySameDomain();

  //! Constructor defining input shape and necessary flags.
  //! It does not perform unification.
  Standard_EXPORT ShapeUpgrade_UnifySameDomain(
    const TopoDS_Shape&    aShape,
    const bool UnifyEdges     = true,
    const bool UnifyFaces     = true,
    const bool ConcatBSplines = false);

  //! Initializes with a shape and necessary flags.
  //! It does not perform unification.
  //! If you intend to nullify the History place holder do it after
  //! initialization.
  Standard_EXPORT void Initialize(const TopoDS_Shape&    aShape,
                                  const bool UnifyEdges     = true,
                                  const bool UnifyFaces     = true,
                                  const bool ConcatBSplines = false);

  //! Sets the flag defining whether it is allowed to create
  //! internal edges inside merged faces in the case of non-manifold
  //! topology. Without this flag merging through multi connected edge
  //! is forbidden. Default value is false.
  Standard_EXPORT void AllowInternalEdges(const bool theValue);

  //! Sets the shape for avoid merging of the faces/edges.
  //! This shape can be vertex or edge.
  //! If the shape is a vertex it forbids merging of connected edges.
  //! If the shape is a edge it forbids merging of connected faces.
  //! This method can be called several times to keep several shapes.
  Standard_EXPORT void KeepShape(const TopoDS_Shape& theShape);

  //! Sets the map of shapes for avoid merging of the faces/edges.
  //! It allows passing a ready to use map instead of calling many times
  //! the method KeepShape.
  Standard_EXPORT void KeepShapes(const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theShapes);

  //! Sets the flag defining the behavior of the algorithm regarding
  //! modification of input shape.
  //! If this flag is equal to True then the input (original) shape can't be
  //! modified during modification process. Default value is true.
  Standard_EXPORT void SetSafeInputMode(bool theValue);

  //! Sets the linear tolerance. It plays the role of chord error when
  //! taking decision about merging of shapes. Default value is Precision::Confusion().
  void SetLinearTolerance(const double theValue) { myLinTol = theValue; }

  //! Sets the angular tolerance. If two shapes form a connection angle greater than
  //! this value they will not be merged. Default value is Precision::Angular().
  void SetAngularTolerance(const double theValue)
  {
    myAngTol = (theValue < Precision::Angular() ? Precision::Angular() : theValue);
  }

  //! Performs unification and builds the resulting shape.
  Standard_EXPORT void Build();

  //! Gives the resulting shape
  const TopoDS_Shape& Shape() const { return myShape; }

  //! Returns the history of the processed shapes.
  const occ::handle<BRepTools_History>& History() const { return myHistory; }

  //! Returns the history of the processed shapes.
  occ::handle<BRepTools_History>& History() { return myHistory; }

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_UnifySameDomain, Standard_Transient)

protected:
  struct SubSequenceOfEdges;

protected:
  //! This method makes if possible a common face from each
  //! group of faces lying on coincident surfaces
  Standard_EXPORT void UnifyFaces();

  //! This method makes if possible a common edge from each
  //! group of smothly connected edges, which are common for the same couple of faces
  Standard_EXPORT void UnifyEdges();

  void IntUnifyFaces(const TopoDS_Shape&                              theInpShape,
                     const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theGMapEdgeFaces,
                     const DataMapOfShapeMapOfShape&                  theGMapFaceShells,
                     const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theFreeBoundMap);

  //! Splits the sequence of edges into the sequence of chains
  bool MergeEdges(NCollection_Sequence<TopoDS_Shape>&                        SeqEdges,
                              const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theVFmap,
                              NCollection_Sequence<SubSequenceOfEdges>&        SeqOfSubSeqOfEdges,
                              const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       NonMergVrt);

  //! Tries to unify the sequence of edges with the set of
  //! another edges which lies on the same geometry
  bool MergeSeq(NCollection_Sequence<TopoDS_Shape>&                        SeqEdges,
                            const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theVFmap,
                            const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       nonMergVert);

  //! Merges a sequence of edges into one edge if possible
  bool MergeSubSeq(const NCollection_Sequence<TopoDS_Shape>&                  theChain,
                               const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theVFmap,
                               TopoDS_Edge&                                     OutEdge);

  //! Unifies the pcurve of the chain into one pcurve of the edge
  void UnionPCurves(const NCollection_Sequence<TopoDS_Shape>& theChain, TopoDS_Edge& theEdge);

  //! Fills the history of the modifications during the operation.
  Standard_EXPORT void FillHistory();

private:
  //! Generates sub-sequences of edges from sequence of edges.
  //! Edges from each subsequences can be merged into the one edge.
  static void generateSubSeq(const NCollection_Sequence<TopoDS_Shape>&                  anInpEdgeSeq,
                             NCollection_Sequence<SubSequenceOfEdges>&        SeqOfSubSeqOfEdges,
                             bool                                 IsClosed,
                             double                                           theAngTol,
                             double                                           theLinTol,
                             const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       AvoidEdgeVrt,
                             const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theVFmap);

private:
  TopoDS_Shape                              myInitShape;
  double                             myLinTol;
  double                             myAngTol;
  bool                          myUnifyFaces;
  bool                          myUnifyEdges;
  bool                          myConcatBSplines;
  bool                          myAllowInternal;
  bool                          mySafeInputMode;
  TopoDS_Shape                              myShape;
  occ::handle<ShapeBuild_ReShape>                myContext;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                       myKeepShapes;
  DataMapOfFacePlane                        myFacePlaneMap;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myEFmap;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>              myFaceNewFace;

  occ::handle<BRepTools_History> myHistory; //!< The history.
};

#endif // _ShapeUpgrade_UnifySameDomain_HeaderFile
