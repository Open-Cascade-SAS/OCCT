// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_HBuilder_HeaderFile
#define _TopOpeBRepBuild_HBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRepBuild_Builder1.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <Standard_Transient.hxx>
#include <TopAbs_State.hxx>
class TopOpeBRepDS_BuildTool;
class TopOpeBRepDS_HDataStructure;
class TopOpeBRepBuild_Builder;

//! The HBuilder algorithm constructs topological
//! objects from an existing topology and new
//! geometries attached to the topology. It is used to
//! construct the result of a topological operation;
//! the existing topologies are the parts involved in
//! the topological operation and the new geometries
//! are the intersection lines and points.
class TopOpeBRepBuild_HBuilder : public Standard_Transient
{

public:
  Standard_EXPORT TopOpeBRepBuild_HBuilder(const TopOpeBRepDS_BuildTool& BT);

  Standard_EXPORT const TopOpeBRepDS_BuildTool& BuildTool() const;

  //! Stores the data structure <HDS>,
  //! Create shapes from the new geometries described in <HDS>.
  Standard_EXPORT void Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);

  //! Same as previous + evaluates if an operation performed on shapes S1,S2
  //! is a particular case.
  Standard_EXPORT void Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                               const TopoDS_Shape&                             S1,
                               const TopoDS_Shape&                             S2);

  //! Removes all split and merge already performed.
  //! Does NOT clear the handled DS.
  Standard_EXPORT void Clear();

  //! returns the DS handled by this builder
  Standard_EXPORT occ::handle<TopOpeBRepDS_HDataStructure> DataStructure() const;

  Standard_EXPORT TopOpeBRepDS_BuildTool& ChangeBuildTool();

  //! Merges the two shapes <S1> and <S2> keeping the
  //! parts of states <TB1>,<TB2> in <S1>,<S2>.
  Standard_EXPORT void MergeShapes(const TopoDS_Shape& S1,
                                   const TopAbs_State  TB1,
                                   const TopoDS_Shape& S2,
                                   const TopAbs_State  TB2);

  //! Merges the two solids <S1> and <S2> keeping the
  //! parts in each solid of states <TB1> and <TB2>.
  Standard_EXPORT void MergeSolids(const TopoDS_Shape& S1,
                                   const TopAbs_State  TB1,
                                   const TopoDS_Shape& S2,
                                   const TopAbs_State  TB2);

  //! Merges the solid <S> keeping the
  //! parts of state <TB>.
  Standard_EXPORT void MergeSolid(const TopoDS_Shape& S, const TopAbs_State TB);

  //! Returns True if the shape <S> has been split.
  Standard_EXPORT bool IsSplit(const TopoDS_Shape& S, const TopAbs_State ToBuild) const;

  //! Returns the split parts <ToBuild> of shape <S>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Splits(const TopoDS_Shape& S,
                                                               const TopAbs_State  ToBuild) const;

  //! Returns True if the shape <S> has been merged.
  Standard_EXPORT bool IsMerged(const TopoDS_Shape& S, const TopAbs_State ToBuild) const;

  //! Returns the merged parts <ToBuild> of shape <S>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Merged(const TopoDS_Shape& S,
                                                               const TopAbs_State  ToBuild) const;

  //! Returns the vertex created on point <I>.
  Standard_EXPORT const TopoDS_Shape& NewVertex(const int I) const;

  //! Returns the edges created on curve <I>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& NewEdges(const int I) const;

  //! Returns the edges created on curve <I>.
  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeNewEdges(const int I);

  //! Returns the faces created on surface <I>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& NewFaces(const int I) const;

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Section();

  Standard_EXPORT void InitExtendedSectionDS(const int k = 3);

  Standard_EXPORT void InitSection(const int k = 3);

  Standard_EXPORT bool MoreSection() const;

  Standard_EXPORT void NextSection();

  Standard_EXPORT const TopoDS_Shape& CurrentSection() const;

  Standard_EXPORT int GetDSEdgeFromSectEdge(const TopoDS_Shape& E, const int rank);

  Standard_EXPORT NCollection_List<int>& GetDSFaceFromDSEdge(const int indexEdg, const int rank);

  Standard_EXPORT int GetDSCurveFromSectEdge(const TopoDS_Shape& SectEdge);

  Standard_EXPORT int GetDSFaceFromDSCurve(const int indexCur, const int rank);

  Standard_EXPORT int GetDSPointFromNewVertex(const TopoDS_Shape& NewVert);

  //! search for the couple of face F1,F2
  //! (from arguments of supra Perform(S1,S2,HDS)) method which
  //! intersection gives section edge E built on an intersection curve.
  //! returns True if F1,F2 have been valued.
  //! returns False if E is not a section edge built
  //! on intersection curve IC.
  Standard_EXPORT bool EdgeCurveAncestors(const TopoDS_Shape& E,
                                          TopoDS_Shape&       F1,
                                          TopoDS_Shape&       F2,
                                          int&                IC);

  //! search for the couple of face F1,F2
  //! (from arguments of supra Perform(S1,S2,HDS)) method which
  //! intersection gives section edge E built on at least one edge.
  //! returns True if F1,F2 have been valued.
  //! returns False if E is not a section edge built
  //! on at least one edge of S1 and/or S2.
  //! LE1,LE2 are edges of S1,S2 which common part is edge E.
  //! LE1 or LE2 may be empty() but not both.
  Standard_EXPORT bool EdgeSectionAncestors(const TopoDS_Shape&             E,
                                            NCollection_List<TopoDS_Shape>& LF1,
                                            NCollection_List<TopoDS_Shape>& LF2,
                                            NCollection_List<TopoDS_Shape>& LE1,
                                            NCollection_List<TopoDS_Shape>& LE2);

  //! Returns 0 is standard operation, != 0 if particular case
  Standard_EXPORT int IsKPart();

  Standard_EXPORT void MergeKPart(const TopAbs_State TB1, const TopAbs_State TB2);

  Standard_EXPORT TopOpeBRepBuild_Builder& ChangeBuilder();

  DEFINE_STANDARD_RTTIEXT(TopOpeBRepBuild_HBuilder, Standard_Transient)

protected:
  TopOpeBRepBuild_Builder1 myBuilder;

private:
  Standard_EXPORT void MakeEdgeAncestorMap();

  Standard_EXPORT void MakeCurveAncestorMap();

  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> mySectEdgeDSEdges1;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> mySectEdgeDSEdges2;
  NCollection_DataMap<int, NCollection_List<int>>                 myDSEdgesDSFaces1;
  NCollection_DataMap<int, NCollection_List<int>>                 myDSEdgesDSFaces2;
  bool                                                            myMakeEdgeAncestorIsDone;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> mySectEdgeDSCurve;
  bool                                                            myMakeCurveAncestorIsDone;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> myNewVertexDSPoint;
  bool                                                            myMakePointAncestorIsDone;
  TopoDS_Shape                                                    myEmptyShape;
  NCollection_List<int>                                           myEmptyIntegerList;
};

#endif // _TopOpeBRepBuild_HBuilder_HeaderFile
