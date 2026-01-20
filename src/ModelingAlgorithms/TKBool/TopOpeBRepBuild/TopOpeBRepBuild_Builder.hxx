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

#ifndef _TopOpeBRepBuild_Builder_HeaderFile
#define _TopOpeBRepBuild_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class TopOpeBRepDS_HDataStructure;
class TopOpeBRepTool_ShapeExplorer;
class TopOpeBRepBuild_ShapeSet;
class TopOpeBRepBuild_EdgeBuilder;
class TopOpeBRepBuild_FaceBuilder;
class TopOpeBRepBuild_SolidBuilder;
class TopOpeBRepBuild_WireEdgeSet;
class TopOpeBRepDS_PointIterator;
class TopOpeBRepBuild_PaveSet;
class TopOpeBRepBuild_GTopo;
class TopOpeBRepBuild_ShellFaceSet;
class TopOpeBRepDS_SurfaceIterator;
class TopOpeBRepDS_CurveIterator;
class TopoDS_Vertex;
class gp_Pnt;

// resolve name collisions with X11 headers
#ifdef FillSolid
  #undef FillSolid
#endif

//! The Builder algorithm constructs topological
//! objects from an existing topology and new
//! geometries attached to the topology. It is used to
//! construct the result of a topological operation;
//! the existing topologies are the parts involved in
//! the topological operation and the new geometries
//! are the intersection lines and points.
class TopOpeBRepBuild_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepBuild_Builder(const TopOpeBRepDS_BuildTool& BT);

  Standard_EXPORT virtual ~TopOpeBRepBuild_Builder();

  Standard_EXPORT TopOpeBRepDS_BuildTool& ChangeBuildTool();

  Standard_EXPORT const TopOpeBRepDS_BuildTool& BuildTool() const;

  //! Stores the data structure <HDS>,
  //! Create shapes from the new geometries.
  Standard_EXPORT virtual void Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);

  //! Stores the data structure <HDS>,
  //! Create shapes from the new geometries,
  //! Evaluates if an operation performed on shapes S1,S2
  //! is a particular case.
  Standard_EXPORT virtual void Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                       const TopoDS_Shape&                        S1,
                                       const TopoDS_Shape&                        S2);

  //! returns the DS handled by this builder
  Standard_EXPORT occ::handle<TopOpeBRepDS_HDataStructure> DataStructure() const;

  //! Removes all splits and merges already performed.
  //! Does NOT clear the handled DS.
  Standard_EXPORT virtual void Clear();

  //! Merges the two edges <S1> and <S2> keeping the
  //! parts in each edge of states <TB1> and <TB2>.
  //! Booleans onA, onB, onAB indicate whether parts of edges
  //! found as state ON respectively on first, second, and both
  //! shapes must be (or not) built.
  Standard_EXPORT void MergeEdges(const NCollection_List<TopoDS_Shape>& L1,
                                  const TopAbs_State          TB1,
                                  const NCollection_List<TopoDS_Shape>& L2,
                                  const TopAbs_State          TB2,
                                  const bool      onA  = false,
                                  const bool      onB  = false,
                                  const bool      onAB = false);

  //! Merges the two faces <S1> and <S2> keeping the
  //! parts in each face of states <TB1> and <TB2>.
  Standard_EXPORT void MergeFaces(const NCollection_List<TopoDS_Shape>& S1,
                                  const TopAbs_State          TB1,
                                  const NCollection_List<TopoDS_Shape>& S2,
                                  const TopAbs_State          TB2,
                                  const bool      onA  = false,
                                  const bool      onB  = false,
                                  const bool      onAB = false);

  //! Merges the two solids <S1> and <S2> keeping the
  //! parts in each solid of states <TB1> and <TB2>.
  Standard_EXPORT void MergeSolids(const TopoDS_Shape& S1,
                                   const TopAbs_State  TB1,
                                   const TopoDS_Shape& S2,
                                   const TopAbs_State  TB2);

  //! Merges the two shapes <S1> and <S2> keeping the
  //! parts of states <TB1>,<TB2> in <S1>,<S2>.
  Standard_EXPORT void MergeShapes(const TopoDS_Shape& S1,
                                   const TopAbs_State  TB1,
                                   const TopoDS_Shape& S2,
                                   const TopAbs_State  TB2);

  Standard_EXPORT void End();

  Standard_EXPORT bool Classify() const;

  Standard_EXPORT void ChangeClassify(const bool B);

  //! Merges the solid <S> keeping the
  //! parts of state <TB>.
  Standard_EXPORT void MergeSolid(const TopoDS_Shape& S, const TopAbs_State TB);

  //! Returns the vertex created on point <I>.
  Standard_EXPORT const TopoDS_Shape& NewVertex(const int I) const;

  //! Returns the edges created on curve <I>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& NewEdges(const int I) const;

  //! Returns the faces created on surface <I>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& NewFaces(const int I) const;

  //! Returns True if the shape <S> has been split.
  Standard_EXPORT bool IsSplit(const TopoDS_Shape& S, const TopAbs_State TB) const;

  //! Returns the split parts <TB> of shape <S>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Splits(const TopoDS_Shape& S,
                                                     const TopAbs_State  TB) const;

  //! Returns True if the shape <S> has been merged.
  Standard_EXPORT bool IsMerged(const TopoDS_Shape& S, const TopAbs_State TB) const;

  //! Returns the merged parts <TB> of shape <S>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Merged(const TopoDS_Shape& S,
                                                     const TopAbs_State  TB) const;

  Standard_EXPORT void InitSection();

  //! create parts ON solid of section edges
  Standard_EXPORT void SplitSectionEdges();

  //! create parts ON solid of section edges
  Standard_EXPORT virtual void SplitSectionEdge(const TopoDS_Shape& E);

  //! return the section edges built on new curves.
  Standard_EXPORT void SectionCurves(NCollection_List<TopoDS_Shape>& L);

  //! return the parts of edges found ON the boundary
  //! of the two arguments S1,S2 of Perform()
  Standard_EXPORT void SectionEdges(NCollection_List<TopoDS_Shape>& L);

  //! Fills anAncMap with pairs (edge,ancestor edge) for each
  //! split from the map aMapON for the shape object identified
  //! by ShapeRank
  Standard_EXPORT void FillSecEdgeAncestorMap(const int        aShapeRank,
                                              const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&    aMapON,
                                              NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& anAncMap) const;

  //! return all section edges.
  Standard_EXPORT void Section(NCollection_List<TopoDS_Shape>& L);

  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Section();

  //! update the DS by creating new geometries.
  //! create vertices on DS points.
  Standard_EXPORT void BuildVertices(const occ::handle<TopOpeBRepDS_HDataStructure>& DS);

  //! update the DS by creating new geometries.
  //! create shapes from the new geometries.
  Standard_EXPORT void BuildEdges(const occ::handle<TopOpeBRepDS_HDataStructure>& DS);

  Standard_EXPORT const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& MSplit(
    const TopAbs_State s) const;

  Standard_EXPORT NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>& ChangeMSplit(
    const TopAbs_State s);

  Standard_EXPORT void MakeEdges(const TopoDS_Shape&          E,
                                 TopOpeBRepBuild_EdgeBuilder& B,
                                 NCollection_List<TopoDS_Shape>&        L);

  Standard_EXPORT void MakeFaces(const TopoDS_Shape&          F,
                                 TopOpeBRepBuild_FaceBuilder& B,
                                 NCollection_List<TopoDS_Shape>&        L);

  Standard_EXPORT void MakeSolids(TopOpeBRepBuild_SolidBuilder& B, NCollection_List<TopoDS_Shape>& L);

  Standard_EXPORT void MakeShells(TopOpeBRepBuild_SolidBuilder& B, NCollection_List<TopoDS_Shape>& L);

  //! Returns a ref.on the list of shapes connected to <S> as
  //! <TB> split parts of <S>.
  //! Mark <S> as split in <TB> parts.
  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeSplit(const TopoDS_Shape& S, const TopAbs_State TB);

  Standard_EXPORT bool Opec12() const;

  Standard_EXPORT bool Opec21() const;

  Standard_EXPORT bool Opecom() const;

  Standard_EXPORT bool Opefus() const;

  Standard_EXPORT TopAbs_State ShapePosition(const TopoDS_Shape& S, const NCollection_List<TopoDS_Shape>& LS);

  Standard_EXPORT bool KeepShape(const TopoDS_Shape&         S,
                                             const NCollection_List<TopoDS_Shape>& LS,
                                             const TopAbs_State          T);

  Standard_EXPORT static TopAbs_ShapeEnum TopType(const TopoDS_Shape& S);

  Standard_EXPORT static bool Reverse(const TopAbs_State T1, const TopAbs_State T2);

  Standard_EXPORT static TopAbs_Orientation Orient(const TopAbs_Orientation O,
                                                   const bool   R);

  Standard_EXPORT void FindSameDomain(NCollection_List<TopoDS_Shape>& L1, NCollection_List<TopoDS_Shape>& L2) const;

  Standard_EXPORT void FindSameDomainSameOrientation(NCollection_List<TopoDS_Shape>& LSO,
                                                     NCollection_List<TopoDS_Shape>& LDO) const;

  Standard_EXPORT void MapShapes(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  Standard_EXPORT void ClearMaps();

  Standard_EXPORT void FindSameRank(const NCollection_List<TopoDS_Shape>& L1,
                                    const int      R,
                                    NCollection_List<TopoDS_Shape>&       L2) const;

  Standard_EXPORT int ShapeRank(const TopoDS_Shape& S) const;

  Standard_EXPORT bool IsShapeOf(const TopoDS_Shape&    S,
                                             const int I12) const;

  Standard_EXPORT static bool Contains(const TopoDS_Shape&         S,
                                                   const NCollection_List<TopoDS_Shape>& L);

  Standard_EXPORT int FindIsKPart();

  Standard_EXPORT int IsKPart() const;

  Standard_EXPORT virtual void MergeKPart();

  Standard_EXPORT virtual void MergeKPart(const TopAbs_State TB1, const TopAbs_State TB2);

  Standard_EXPORT void MergeKPartiskole();

  Standard_EXPORT void MergeKPartiskoletge();

  Standard_EXPORT void MergeKPartisdisj();

  Standard_EXPORT void MergeKPartisfafa();

  Standard_EXPORT void MergeKPartissoso();

  Standard_EXPORT int KPiskole();

  Standard_EXPORT int KPiskoletge();

  Standard_EXPORT int KPisdisj();

  Standard_EXPORT int KPisfafa();

  Standard_EXPORT int KPissoso();

  Standard_EXPORT void KPClearMaps();

  Standard_EXPORT int KPlhg(const TopoDS_Shape&    S,
                                         const TopAbs_ShapeEnum T,
                                         NCollection_List<TopoDS_Shape>&  L) const;

  Standard_EXPORT int KPlhg(const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const;

  Standard_EXPORT int KPlhsd(const TopoDS_Shape&    S,
                                          const TopAbs_ShapeEnum T,
                                          NCollection_List<TopoDS_Shape>&  L) const;

  Standard_EXPORT int KPlhsd(const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const;

  Standard_EXPORT TopAbs_State KPclasSS(const TopoDS_Shape&         S1,
                                        const NCollection_List<TopoDS_Shape>& exceptLS1,
                                        const TopoDS_Shape&         S2);

  Standard_EXPORT TopAbs_State KPclasSS(const TopoDS_Shape& S1,
                                        const TopoDS_Shape& exceptS1,
                                        const TopoDS_Shape& S2);

  Standard_EXPORT TopAbs_State KPclasSS(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  Standard_EXPORT bool KPiskolesh(const TopoDS_Shape&   S,
                                              NCollection_List<TopoDS_Shape>& LS,
                                              NCollection_List<TopoDS_Shape>& LF) const;

  Standard_EXPORT bool KPiskoletgesh(const TopoDS_Shape&   S,
                                                 NCollection_List<TopoDS_Shape>& LS,
                                                 NCollection_List<TopoDS_Shape>& LF) const;

  Standard_EXPORT void KPSameDomain(NCollection_List<TopoDS_Shape>& L1, NCollection_List<TopoDS_Shape>& L2) const;

  Standard_EXPORT int KPisdisjsh(const TopoDS_Shape& S) const;

  Standard_EXPORT int KPisfafash(const TopoDS_Shape& S) const;

  Standard_EXPORT int KPissososh(const TopoDS_Shape& S) const;

  Standard_EXPORT void KPiskoleanalyse(const TopAbs_State FT1,
                                       const TopAbs_State FT2,
                                       const TopAbs_State ST1,
                                       const TopAbs_State ST2,
                                       int&  I,
                                       int&  I1,
                                       int&  I2) const;

  Standard_EXPORT void KPiskoletgeanalyse(const TopOpeBRepDS_Config Conf,
                                          const TopAbs_State        ST1,
                                          const TopAbs_State        ST2,
                                          int&         I) const;

  Standard_EXPORT void KPisdisjanalyse(const TopAbs_State ST1,
                                       const TopAbs_State ST2,
                                       int&  I,
                                       int&  IC1,
                                       int&  IC2) const;

  Standard_EXPORT static int KPls(const TopoDS_Shape&    S,
                                               const TopAbs_ShapeEnum T,
                                               NCollection_List<TopoDS_Shape>&  L);

  Standard_EXPORT static int KPls(const TopoDS_Shape& S, const TopAbs_ShapeEnum T);

  Standard_EXPORT TopAbs_State KPclassF(const TopoDS_Shape& F1, const TopoDS_Shape& F2);

  Standard_EXPORT void KPclassFF(const TopoDS_Shape& F1,
                                 const TopoDS_Shape& F2,
                                 TopAbs_State&       T1,
                                 TopAbs_State&       T2);

  Standard_EXPORT bool KPiskoleFF(const TopoDS_Shape& F1,
                                              const TopoDS_Shape& F2,
                                              TopAbs_State&       T1,
                                              TopAbs_State&       T2);

  Standard_EXPORT static bool KPContains(const TopoDS_Shape&         S,
                                                     const NCollection_List<TopoDS_Shape>& L);

  Standard_EXPORT TopoDS_Shape KPmakeface(const TopoDS_Shape&         F1,
                                          const NCollection_List<TopoDS_Shape>& LF2,
                                          const TopAbs_State          T1,
                                          const TopAbs_State          T2,
                                          const bool      R1,
                                          const bool      R2);

  Standard_EXPORT static int KPreturn(const int KP);

  Standard_EXPORT void SplitEvisoONperiodicF();

  Standard_EXPORT void GMergeSolids(const NCollection_List<TopoDS_Shape>&  LSO1,
                                    const NCollection_List<TopoDS_Shape>&  LSO2,
                                    const TopOpeBRepBuild_GTopo& G);

  Standard_EXPORT void GFillSolidsSFS(const NCollection_List<TopoDS_Shape>&   LSO1,
                                      const NCollection_List<TopoDS_Shape>&   LSO2,
                                      const TopOpeBRepBuild_GTopo&  G,
                                      TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT virtual void GFillSolidSFS(const TopoDS_Shape&           SO1,
                                             const NCollection_List<TopoDS_Shape>&   LSO2,
                                             const TopOpeBRepBuild_GTopo&  G,
                                             TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GFillSurfaceTopologySFS(const TopoDS_Shape&           SO1,
                                               const TopOpeBRepBuild_GTopo&  G,
                                               TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GFillSurfaceTopologySFS(const TopOpeBRepDS_SurfaceIterator& IT,
                                               const TopOpeBRepBuild_GTopo&        G,
                                               TopOpeBRepBuild_ShellFaceSet&       SFS) const;

  Standard_EXPORT virtual void GFillShellSFS(const TopoDS_Shape&           SH1,
                                             const NCollection_List<TopoDS_Shape>&   LSO2,
                                             const TopOpeBRepBuild_GTopo&  G,
                                             TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GFillFaceSFS(const TopoDS_Shape&           F1,
                                    const NCollection_List<TopoDS_Shape>&   LSO2,
                                    const TopOpeBRepBuild_GTopo&  G,
                                    TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GSplitFaceSFS(const TopoDS_Shape&           F1,
                                     const NCollection_List<TopoDS_Shape>&   LSclass,
                                     const TopOpeBRepBuild_GTopo&  G,
                                     TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GMergeFaceSFS(const TopoDS_Shape&           F,
                                     const TopOpeBRepBuild_GTopo&  G,
                                     TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GSplitFace(const TopoDS_Shape&          F,
                                  const TopOpeBRepBuild_GTopo& G,
                                  const NCollection_List<TopoDS_Shape>&  LSclass);

  Standard_EXPORT void AddONPatchesSFS(const TopOpeBRepBuild_GTopo&  G,
                                       TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void FillOnPatches(const NCollection_List<TopoDS_Shape>&               anEdgesON,
                                     const TopoDS_Shape&                       aBaseFace,
                                     const NCollection_IndexedMap<TopoDS_Shape>& avoidMap);

  Standard_EXPORT void FindFacesTouchingEdge(const TopoDS_Shape&    aFace,
                                             const TopoDS_Shape&    anEdge,
                                             const int aShRank,
                                             NCollection_List<TopoDS_Shape>&  aFaces) const;

  Standard_EXPORT void GMergeFaces(const NCollection_List<TopoDS_Shape>&  LF1,
                                   const NCollection_List<TopoDS_Shape>&  LF2,
                                   const TopOpeBRepBuild_GTopo& G);

  Standard_EXPORT void GFillFacesWES(const NCollection_List<TopoDS_Shape>&  LF1,
                                     const NCollection_List<TopoDS_Shape>&  LF2,
                                     const TopOpeBRepBuild_GTopo& G,
                                     TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillFacesWESK(const NCollection_List<TopoDS_Shape>&  LF1,
                                      const NCollection_List<TopoDS_Shape>&  LF2,
                                      const TopOpeBRepBuild_GTopo& G,
                                      TopOpeBRepBuild_WireEdgeSet& WES,
                                      const int       K);

  Standard_EXPORT void GFillFacesWESMakeFaces(const NCollection_List<TopoDS_Shape>&  LF1,
                                              const NCollection_List<TopoDS_Shape>&  LF2,
                                              const NCollection_List<TopoDS_Shape>&  LSO,
                                              const TopOpeBRepBuild_GTopo& G);

  Standard_EXPORT void GFillFaceWES(const TopoDS_Shape&          F,
                                    const NCollection_List<TopoDS_Shape>&  LF2,
                                    const TopOpeBRepBuild_GTopo& G,
                                    TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillCurveTopologyWES(const TopoDS_Shape&          F,
                                             const TopOpeBRepBuild_GTopo& G,
                                             TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillCurveTopologyWES(const TopOpeBRepDS_CurveIterator& IT,
                                             const TopOpeBRepBuild_GTopo&      G,
                                             TopOpeBRepBuild_WireEdgeSet&      WES) const;

  Standard_EXPORT void GFillONPartsWES(const TopoDS_Shape&          F,
                                       const TopOpeBRepBuild_GTopo& G,
                                       const NCollection_List<TopoDS_Shape>&  LSclass,
                                       TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillWireWES(const TopoDS_Shape&          W,
                                    const NCollection_List<TopoDS_Shape>&  LF2,
                                    const TopOpeBRepBuild_GTopo& G,
                                    TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillEdgeWES(const TopoDS_Shape&          E,
                                    const NCollection_List<TopoDS_Shape>&  LF2,
                                    const TopOpeBRepBuild_GTopo& G,
                                    TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GSplitEdgeWES(const TopoDS_Shape&          E,
                                     const NCollection_List<TopoDS_Shape>&  LF2,
                                     const TopOpeBRepBuild_GTopo& G,
                                     TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GMergeEdgeWES(const TopoDS_Shape&          E,
                                     const TopOpeBRepBuild_GTopo& G,
                                     TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GSplitEdge(const TopoDS_Shape&          E,
                                  const TopOpeBRepBuild_GTopo& G,
                                  const NCollection_List<TopoDS_Shape>&  LSclass);

  Standard_EXPORT void GMergeEdges(const NCollection_List<TopoDS_Shape>&  LE1,
                                   const NCollection_List<TopoDS_Shape>&  LE2,
                                   const TopOpeBRepBuild_GTopo& G);

  Standard_EXPORT void GFillEdgesPVS(const NCollection_List<TopoDS_Shape>&  LE1,
                                     const NCollection_List<TopoDS_Shape>&  LE2,
                                     const TopOpeBRepBuild_GTopo& G,
                                     TopOpeBRepBuild_PaveSet&     PVS);

  Standard_EXPORT void GFillEdgePVS(const TopoDS_Shape&          E,
                                    const NCollection_List<TopoDS_Shape>&  LE2,
                                    const TopOpeBRepBuild_GTopo& G,
                                    TopOpeBRepBuild_PaveSet&     PVS);

  Standard_EXPORT void GFillPointTopologyPVS(const TopoDS_Shape&          E,
                                             const TopOpeBRepBuild_GTopo& G,
                                             TopOpeBRepBuild_PaveSet&     PVS);

  Standard_EXPORT void GFillPointTopologyPVS(const TopoDS_Shape&               E,
                                             const TopOpeBRepDS_PointIterator& IT,
                                             const TopOpeBRepBuild_GTopo&      G,
                                             TopOpeBRepBuild_PaveSet&          PVS) const;

  Standard_EXPORT bool GParamOnReference(const TopoDS_Vertex& V,
                                                     const TopoDS_Edge&   E,
                                                     double&       P) const;

  Standard_EXPORT bool GKeepShape(const TopoDS_Shape&         S,
                                              const NCollection_List<TopoDS_Shape>& Lref,
                                              const TopAbs_State          T);

  //! return True if S is classified <T> / Lref shapes
  Standard_EXPORT bool GKeepShape1(const TopoDS_Shape&         S,
                                               const NCollection_List<TopoDS_Shape>& Lref,
                                               const TopAbs_State          T,
                                               TopAbs_State&               pos);

  //! add to Lou the shapes of Lin classified <T> / Lref shapes.
  //! Lou is not cleared. (S is a dummy trace argument)
  Standard_EXPORT void GKeepShapes(const TopoDS_Shape&         S,
                                   const NCollection_List<TopoDS_Shape>& Lref,
                                   const TopAbs_State          T,
                                   const NCollection_List<TopoDS_Shape>& Lin,
                                   NCollection_List<TopoDS_Shape>&       Lou);

  Standard_EXPORT void GSFSMakeSolids(const TopoDS_Shape&           SOF,
                                      TopOpeBRepBuild_ShellFaceSet& SFS,
                                      NCollection_List<TopoDS_Shape>&         LOSO);

  Standard_EXPORT void GSOBUMakeSolids(const TopoDS_Shape&           SOF,
                                       TopOpeBRepBuild_SolidBuilder& SOBU,
                                       NCollection_List<TopoDS_Shape>&         LOSO);

  Standard_EXPORT virtual void GWESMakeFaces(const TopoDS_Shape&          FF,
                                             TopOpeBRepBuild_WireEdgeSet& WES,
                                             NCollection_List<TopoDS_Shape>&        LOF);

  Standard_EXPORT void GFABUMakeFaces(const TopoDS_Shape&             FF,
                                      TopOpeBRepBuild_FaceBuilder&    FABU,
                                      NCollection_List<TopoDS_Shape>&           LOF,
                                      NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>& MWisOld);

  Standard_EXPORT void RegularizeFaces(const TopoDS_Shape&         FF,
                                       const NCollection_List<TopoDS_Shape>& lnewFace,
                                       NCollection_List<TopoDS_Shape>&       LOF);

  Standard_EXPORT void RegularizeFace(const TopoDS_Shape&   FF,
                                      const TopoDS_Shape&   newFace,
                                      NCollection_List<TopoDS_Shape>& LOF);

  Standard_EXPORT void RegularizeSolids(const TopoDS_Shape&         SS,
                                        const NCollection_List<TopoDS_Shape>& lnewSolid,
                                        NCollection_List<TopoDS_Shape>&       LOS);

  Standard_EXPORT void RegularizeSolid(const TopoDS_Shape&   SS,
                                       const TopoDS_Shape&   newSolid,
                                       NCollection_List<TopoDS_Shape>& LOS);

  Standard_EXPORT void GPVSMakeEdges(const TopoDS_Shape&      EF,
                                     TopOpeBRepBuild_PaveSet& PVS,
                                     NCollection_List<TopoDS_Shape>&    LOE) const;

  Standard_EXPORT void GEDBUMakeEdges(const TopoDS_Shape&          EF,
                                      TopOpeBRepBuild_EdgeBuilder& EDBU,
                                      NCollection_List<TopoDS_Shape>&        LOE) const;

  Standard_EXPORT bool GToSplit(const TopoDS_Shape& S, const TopAbs_State TB) const;

  Standard_EXPORT bool GToMerge(const TopoDS_Shape& S) const;

  Standard_EXPORT static bool GTakeCommonOfSame(const TopOpeBRepBuild_GTopo& G);

  Standard_EXPORT static bool GTakeCommonOfDiff(const TopOpeBRepBuild_GTopo& G);

  Standard_EXPORT void GFindSamDom(const TopoDS_Shape&   S,
                                   NCollection_List<TopoDS_Shape>& L1,
                                   NCollection_List<TopoDS_Shape>& L2) const;

  Standard_EXPORT void GFindSamDom(NCollection_List<TopoDS_Shape>& L1, NCollection_List<TopoDS_Shape>& L2) const;

  Standard_EXPORT void GFindSamDomSODO(const TopoDS_Shape&   S,
                                       NCollection_List<TopoDS_Shape>& LSO,
                                       NCollection_List<TopoDS_Shape>& LDO) const;

  Standard_EXPORT void GFindSamDomSODO(NCollection_List<TopoDS_Shape>& LSO, NCollection_List<TopoDS_Shape>& LDO) const;

  Standard_EXPORT void GMapShapes(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  Standard_EXPORT void GClearMaps();

  Standard_EXPORT void GFindSameRank(const NCollection_List<TopoDS_Shape>& L1,
                                     const int      R,
                                     NCollection_List<TopoDS_Shape>&       L2) const;

  Standard_EXPORT int GShapeRank(const TopoDS_Shape& S) const;

  Standard_EXPORT bool GIsShapeOf(const TopoDS_Shape&    S,
                                              const int I12) const;

  Standard_EXPORT static bool GContains(const TopoDS_Shape&         S,
                                                    const NCollection_List<TopoDS_Shape>& L);

  Standard_EXPORT static void GCopyList(const NCollection_List<TopoDS_Shape>& Lin,
                                        const int      i1,
                                        const int      i2,
                                        NCollection_List<TopoDS_Shape>&       Lou);

  Standard_EXPORT static void GCopyList(const NCollection_List<TopoDS_Shape>& Lin, NCollection_List<TopoDS_Shape>& Lou);

  Standard_EXPORT void GdumpLS(const NCollection_List<TopoDS_Shape>& L) const;

  Standard_EXPORT static void GdumpPNT(const gp_Pnt& P);

  Standard_EXPORT static void GdumpORIPARPNT(const TopAbs_Orientation o,
                                             const double      p,
                                             const gp_Pnt&            Pnt);

  Standard_EXPORT void GdumpSHA(const TopoDS_Shape& S, void* const str = NULL) const;

  Standard_EXPORT void GdumpSHAORI(const TopoDS_Shape& S, void* const str = NULL) const;

  Standard_EXPORT void GdumpSHAORIGEO(const TopoDS_Shape&    S,
                                      void* const str = NULL) const;

  Standard_EXPORT void GdumpSHASTA(const int         iS,
                                   const TopAbs_State             T,
                                   const TCollection_AsciiString& a = "",
                                   const TCollection_AsciiString& b = "") const;

  Standard_EXPORT void GdumpSHASTA(const TopoDS_Shape&            S,
                                   const TopAbs_State             T,
                                   const TCollection_AsciiString& a = "",
                                   const TCollection_AsciiString& b = "") const;

  Standard_EXPORT void GdumpSHASTA(const int          iS,
                                   const TopAbs_State              T,
                                   const TopOpeBRepBuild_ShapeSet& SS,
                                   const TCollection_AsciiString&  a = "",
                                   const TCollection_AsciiString&  b = "",
                                   const TCollection_AsciiString&  c = "\n") const;

  Standard_EXPORT void GdumpEDG(const TopoDS_Shape& S, void* const str = NULL) const;

  Standard_EXPORT void GdumpEDGVER(const TopoDS_Shape&    E,
                                   const TopoDS_Shape&    V,
                                   void* const str = NULL) const;

  Standard_EXPORT void GdumpSAMDOM(const NCollection_List<TopoDS_Shape>& L,
                                   void* const      str = NULL) const;

  Standard_EXPORT void GdumpEXP(const TopOpeBRepTool_ShapeExplorer& E) const;

  Standard_EXPORT void GdumpSOBU(TopOpeBRepBuild_SolidBuilder& SB) const;

  Standard_EXPORT void GdumpFABU(TopOpeBRepBuild_FaceBuilder& FB) const;

  Standard_EXPORT void GdumpEDBU(TopOpeBRepBuild_EdgeBuilder& EB) const;

  Standard_EXPORT bool GtraceSPS(const int iS) const;

  Standard_EXPORT bool GtraceSPS(const int iS,
                                             const int jS) const;

  Standard_EXPORT bool GtraceSPS(const TopoDS_Shape& S) const;

  Standard_EXPORT bool GtraceSPS(const TopoDS_Shape& S, int& IS) const;

  Standard_EXPORT void GdumpSHASETreset();

  Standard_EXPORT int GdumpSHASETindex();

  Standard_EXPORT static void PrintGeo(const TopoDS_Shape& S);

  Standard_EXPORT static void PrintSur(const TopoDS_Face& F);

  Standard_EXPORT static void PrintCur(const TopoDS_Edge& E);

  Standard_EXPORT static void PrintPnt(const TopoDS_Vertex& V);

  Standard_EXPORT static void PrintOri(const TopoDS_Shape& S);

  Standard_EXPORT static TCollection_AsciiString StringState(const TopAbs_State S);

  Standard_EXPORT static bool GcheckNBOUNDS(const TopoDS_Shape& E);

  friend class TopOpeBRepBuild_HBuilder;

protected:
  //! update the DS by creating new geometries.
  //! create edges on the new curve <Icurv>.
  Standard_EXPORT void BuildEdges(const int                     iC,
                                  const occ::handle<TopOpeBRepDS_HDataStructure>& DS);

  //! update the DS by creating new geometries.
  //! create faces on the new surface <ISurf>.
  Standard_EXPORT void BuildFaces(const int                     iS,
                                  const occ::handle<TopOpeBRepDS_HDataStructure>& DS);

  //! update the DS by creating new geometries.
  //! create shapes from the new geometries.
  Standard_EXPORT void BuildFaces(const occ::handle<TopOpeBRepDS_HDataStructure>& DS);

  //! Split <E1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitEdge(const TopoDS_Shape& E1,
                                 const TopAbs_State  TB1,
                                 const TopAbs_State  TB2);

  //! Split <E1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitEdge1(const TopoDS_Shape& E1,
                                  const TopAbs_State  TB1,
                                  const TopAbs_State  TB2);

  //! Split <E1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitEdge2(const TopoDS_Shape& E1,
                                  const TopAbs_State  TB1,
                                  const TopAbs_State  TB2);

  //! Split <F1> keeping the parts of state <TB1>.
  //! Merge faces with same domain, keeping parts of
  //! state <TB2>.
  Standard_EXPORT void SplitFace(const TopoDS_Shape& F1,
                                 const TopAbs_State  TB1,
                                 const TopAbs_State  TB2);

  Standard_EXPORT void SplitFace1(const TopoDS_Shape& F1,
                                  const TopAbs_State  TB1,
                                  const TopAbs_State  TB2);

  Standard_EXPORT void SplitFace2(const TopoDS_Shape& F1,
                                  const TopAbs_State  TB1,
                                  const TopAbs_State  TB2);

  //! Split <S1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitSolid(const TopoDS_Shape& S1,
                                  const TopAbs_State  TB1,
                                  const TopAbs_State  TB2);

  //! Explore shapes of given by explorer <Ex> to split them.
  //! Store new shapes in the set <SS>.
  //! According to RevOri, reverse or not their orientation.
  Standard_EXPORT void SplitShapes(TopOpeBRepTool_ShapeExplorer& Ex,
                                   const TopAbs_State            TB1,
                                   const TopAbs_State            TB2,
                                   TopOpeBRepBuild_ShapeSet&     SS,
                                   const bool        RevOri);

  //! Split edges of <F1> and store wires and edges in
  //! the set <WES>. According to RevOri, reverse (or not) orientation.
  Standard_EXPORT void FillFace(const TopoDS_Shape&          F1,
                                const TopAbs_State           TB1,
                                const NCollection_List<TopoDS_Shape>&  LF2,
                                const TopAbs_State           TB2,
                                TopOpeBRepBuild_WireEdgeSet& WES,
                                const bool       RevOri);

  //! Split faces of <S1> and store shells and faces in
  //! the set <SS>. According to RevOri, reverse (or not) orientation.
  Standard_EXPORT void FillSolid(const TopoDS_Shape&         S1,
                                 const TopAbs_State          TB1,
                                 const NCollection_List<TopoDS_Shape>& LS2,
                                 const TopAbs_State          TB2,
                                 TopOpeBRepBuild_ShapeSet&   SS,
                                 const bool      RevOri);

  //! Split subshapes of <S1> and store subshapes in
  //! the set <SS>. According to RevOri, reverse (or not) orientation.
  Standard_EXPORT void FillShape(const TopoDS_Shape&         S1,
                                 const TopAbs_State          TB1,
                                 const NCollection_List<TopoDS_Shape>& LS2,
                                 const TopAbs_State          TB2,
                                 TopOpeBRepBuild_ShapeSet&   SS,
                                 const bool      RevOri);

  //! fills the vertex set PVS with the point iterator IT.
  //! IT accesses a list of interferences which geometry is a point or a vertex.
  //! TB indicates the orientation to give to the geometries
  //! found in interference list accessed by IT.
  Standard_EXPORT void FillVertexSet(TopOpeBRepDS_PointIterator& IT,
                                     const TopAbs_State          TB,
                                     TopOpeBRepBuild_PaveSet&    PVS) const;

  //! fills vertex set PVS with the current value of IT.
  //! I geometry is a point or a vertex.
  //! TB indicates the orientation to give to geometries found I
  Standard_EXPORT void FillVertexSetOnValue(const TopOpeBRepDS_PointIterator& IT,
                                            const TopAbs_State                TB,
                                            TopOpeBRepBuild_PaveSet&          PVS) const;

  //! Returns True if the shape <S> has not already been split
  Standard_EXPORT bool ToSplit(const TopoDS_Shape& S, const TopAbs_State TB) const;

  //! add the shape <S> to the map of split shapes.
  //! mark <S> as split/not split on <state>, according to B value.
  Standard_EXPORT void MarkSplit(const TopoDS_Shape&    S,
                                 const TopAbs_State     TB,
                                 const bool B = true);

  //! Returns a ref. on the list of shapes connected to <S> as
  //! <TB> merged parts of <S>.
  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeMerged(const TopoDS_Shape& S, const TopAbs_State TB);

  //! Returns a ref. on the vertex created on point <I>.
  Standard_EXPORT TopoDS_Shape& ChangeNewVertex(const int I);

  //! Returns a ref. on the list of edges created on curve <I>.
  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeNewEdges(const int I);

  //! Returns a ref. on the list of faces created on surface <I>.
  Standard_EXPORT NCollection_List<TopoDS_Shape>& ChangeNewFaces(const int I);

  Standard_EXPORT void AddIntersectionEdges(TopoDS_Shape&             F,
                                            const TopAbs_State        TB,
                                            const bool    RevOri,
                                            TopOpeBRepBuild_ShapeSet& ES) const;

  Standard_EXPORT void UpdateSplitAndMerged(const NCollection_DataMap<int, NCollection_List<TopoDS_Shape>>& mle,
                                            const NCollection_DataMap<int, TopoDS_Shape>&       mre,
                                            const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&         mlf,
                                            const TopAbs_State                          state);

  TopAbs_State                                   myState1;
  TopAbs_State                                   myState2;
  TopoDS_Shape                                   myShape1;
  TopoDS_Shape                                   myShape2;
  occ::handle<TopOpeBRepDS_HDataStructure>            myDataStructure;
  TopOpeBRepDS_BuildTool                         myBuildTool;
  occ::handle<NCollection_HArray1<TopoDS_Shape>>                myNewVertices;
  NCollection_DataMap<int, NCollection_List<TopoDS_Shape>>           myNewEdges;
  occ::handle<NCollection_HArray1<NCollection_List<TopoDS_Shape>>>          myNewFaces;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher> mySplitIN;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher> mySplitON;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher> mySplitOUT;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher> myMergedIN;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher> myMergedON;
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher> myMergedOUT;
  NCollection_List<TopoDS_Shape>                           myEmptyShapeList;
  NCollection_List<TopoDS_Shape>                           myListOfSolid;
  NCollection_List<TopoDS_Shape>                           myListOfFace;
  NCollection_List<TopoDS_Shape>                           myListOfEdge;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>             myFSplits;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>             myESplits;
  bool                               mySectionDone;
  bool                               mySplitSectionEdgesDone;
  NCollection_List<TopoDS_Shape>                           mySection;
  TopoDS_Solid                                   mySolidReference;
  TopoDS_Solid                                   mySolidToFill;
  NCollection_List<TopoDS_Shape>                           myFaceAvoid;
  TopoDS_Face                                    myFaceReference;
  TopoDS_Face                                    myFaceToFill;
  NCollection_List<TopoDS_Shape>                           myEdgeAvoid;
  TopoDS_Edge                                    myEdgeReference;
  TopoDS_Edge                                    myEdgeToFill;
  NCollection_List<TopoDS_Shape>                           myVertexAvoid;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                     myMAP1;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                     myMAP2;
  int                               myIsKPart;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>             myKPMAPf1f2;
  int                               mySHASETindex;
  bool                               myClassifyDef;
  bool                               myClassifyVal;
  TopOpeBRepTool_ShapeClassifier                 myShapeClassifier;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                            myMemoSplit;
  TCollection_AsciiString                        myEmptyAS;
  bool                               myProcessON;
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>            myONFacesMap;
  NCollection_IndexedMap<TopoDS_Shape>             myONElemMap;

};

#endif // _TopOpeBRepBuild_Builder_HeaderFile
