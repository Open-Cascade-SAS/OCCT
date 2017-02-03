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
#include <TopTools_HArray1OfShape.hxx>
#include <TopTools_DataMapOfIntegerListOfShape.hxx>
#include <TopTools_HArray1OfListOfShape.hxx>
#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <Standard_Real.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <Standard_Address.hxx>
class TopOpeBRepDS_HDataStructure;
class Standard_NoSuchObject;
class TopOpeBRepBuild_HBuilder;
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

//! The Builder  algorithm    constructs   topological
//! objects  from   an    existing  topology  and  new
//! geometries attached to the topology. It is used to
//! construct the result of a topological operation;
//! the existing  topologies are the parts involved in
//! the  topological  operation and the new geometries
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
  Standard_EXPORT virtual void Perform (const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  //! Stores the data structure <HDS>,
  //! Create shapes from the new geometries,
  //! Evaluates if an operation performed on shapes S1,S2
  //! is a particular case.
  Standard_EXPORT virtual void Perform (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  //! returns the DS handled by this builder
  Standard_EXPORT Handle(TopOpeBRepDS_HDataStructure) DataStructure() const;
  
  //! Removes all splits and merges already performed.
  //! Does NOT clear the handled DS.
  Standard_EXPORT virtual void Clear();
  
  //! Merges  the two edges <S1> and <S2> keeping the
  //! parts in each edge of states <TB1> and <TB2>.
  //! Booleans onA, onB, onAB indicate wheter parts of edges
  //! found as state ON respectively on first, second, and both
  //! shapes must be (or not) built.
  Standard_EXPORT void MergeEdges (const TopTools_ListOfShape& L1, const TopAbs_State TB1, const TopTools_ListOfShape& L2, const TopAbs_State TB2, const Standard_Boolean onA = Standard_False, const Standard_Boolean onB = Standard_False, const Standard_Boolean onAB = Standard_False);
  
  //! Merges  the two faces <S1>   and <S2> keeping the
  //! parts in each face of states <TB1> and <TB2>.
  Standard_EXPORT void MergeFaces (const TopTools_ListOfShape& S1, const TopAbs_State TB1, const TopTools_ListOfShape& S2, const TopAbs_State TB2, const Standard_Boolean onA = Standard_False, const Standard_Boolean onB = Standard_False, const Standard_Boolean onAB = Standard_False);
  
  //! Merges  the two solids <S1>   and <S2> keeping the
  //! parts in each solid of states <TB1> and <TB2>.
  Standard_EXPORT void MergeSolids (const TopoDS_Shape& S1, const TopAbs_State TB1, const TopoDS_Shape& S2, const TopAbs_State TB2);
  
  //! Merges the two shapes <S1> and <S2> keeping the
  //! parts of states <TB1>,<TB2> in <S1>,<S2>.
  Standard_EXPORT void MergeShapes (const TopoDS_Shape& S1, const TopAbs_State TB1, const TopoDS_Shape& S2, const TopAbs_State TB2);
  
  Standard_EXPORT void End();
  
  Standard_EXPORT Standard_Boolean Classify() const;
  
  Standard_EXPORT void ChangeClassify (const Standard_Boolean B);
  
  //! Merges the solid <S>  keeping the
  //! parts of state <TB>.
  Standard_EXPORT void MergeSolid (const TopoDS_Shape& S, const TopAbs_State TB);
  
  //! Returns the vertex created on point <I>.
  Standard_EXPORT const TopoDS_Shape& NewVertex (const Standard_Integer I) const;
  
  //! Returns the edges created on curve <I>.
  Standard_EXPORT const TopTools_ListOfShape& NewEdges (const Standard_Integer I) const;
  
  //! Returns the faces created on surface <I>.
  Standard_EXPORT const TopTools_ListOfShape& NewFaces (const Standard_Integer I) const;
  
  //! Returns True if the shape <S> has been split.
  Standard_EXPORT Standard_Boolean IsSplit (const TopoDS_Shape& S, const TopAbs_State TB) const;
  
  //! Returns the split parts <TB> of shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Splits (const TopoDS_Shape& S, const TopAbs_State TB) const;
  
  //! Returns True if the shape <S> has been merged.
  Standard_EXPORT Standard_Boolean IsMerged (const TopoDS_Shape& S, const TopAbs_State TB) const;
  
  //! Returns the merged parts <TB> of shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Merged (const TopoDS_Shape& S, const TopAbs_State TB) const;
  
  Standard_EXPORT void InitSection();
  
  //! create parts ON solid of section edges
  Standard_EXPORT void SplitSectionEdges();
  
  //! create parts ON solid of section edges
  Standard_EXPORT virtual void SplitSectionEdge (const TopoDS_Shape& E);
  
  //! return the section edges built on new curves.
  Standard_EXPORT void SectionCurves (TopTools_ListOfShape& L);
  
  //! return the parts of edges found ON the boundary
  //! of the two arguments S1,S2 of Perform()
  Standard_EXPORT void SectionEdges (TopTools_ListOfShape& L);
  
  //! Fills anAncMap with pairs (edge,ancestor edge) for each
  //! split from the map aMapON for the shape object identified
  //! by ShapeRank
  Standard_EXPORT void FillSecEdgeAncestorMap (const Standard_Integer aShapeRank, const TopTools_MapOfShape& aMapON, TopTools_DataMapOfShapeShape& anAncMap) const;
  
  //! return all section edges.
  Standard_EXPORT void Section (TopTools_ListOfShape& L);
  
  Standard_EXPORT const TopTools_ListOfShape& Section();
  
  //! update the DS by creating new geometries.
  //! create vertices on DS points.
  Standard_EXPORT void BuildVertices (const Handle(TopOpeBRepDS_HDataStructure)& DS);
  
  //! update the DS by creating new geometries.
  //! create shapes from the new geometries.
  Standard_EXPORT void BuildEdges (const Handle(TopOpeBRepDS_HDataStructure)& DS);
  
  Standard_EXPORT const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MSplit (const TopAbs_State s) const;
  
  Standard_EXPORT TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& ChangeMSplit (const TopAbs_State s);
  
  Standard_EXPORT void MakeEdges (const TopoDS_Shape& E, TopOpeBRepBuild_EdgeBuilder& B, TopTools_ListOfShape& L);
  
  Standard_EXPORT void MakeFaces (const TopoDS_Shape& F, TopOpeBRepBuild_FaceBuilder& B, TopTools_ListOfShape& L);
  
  Standard_EXPORT void MakeSolids (TopOpeBRepBuild_SolidBuilder& B, TopTools_ListOfShape& L);
  
  Standard_EXPORT void MakeShells (TopOpeBRepBuild_SolidBuilder& B, TopTools_ListOfShape& L);
  
  //! Returns a ref.on the list of shapes connected to <S> as
  //! <TB> split parts of <S>.
  //! Mark <S> as split in <TB> parts.
  Standard_EXPORT TopTools_ListOfShape& ChangeSplit (const TopoDS_Shape& S, const TopAbs_State TB);
  
  Standard_EXPORT Standard_Boolean Opec12() const;
  
  Standard_EXPORT Standard_Boolean Opec21() const;
  
  Standard_EXPORT Standard_Boolean Opecom() const;
  
  Standard_EXPORT Standard_Boolean Opefus() const;
  
  Standard_EXPORT TopAbs_State ShapePosition (const TopoDS_Shape& S, const TopTools_ListOfShape& LS);
  
  Standard_EXPORT Standard_Boolean KeepShape (const TopoDS_Shape& S, const TopTools_ListOfShape& LS, const TopAbs_State T);
  
  Standard_EXPORT static TopAbs_ShapeEnum TopType (const TopoDS_Shape& S);
  
  Standard_EXPORT static Standard_Boolean Reverse (const TopAbs_State T1, const TopAbs_State T2);
  
  Standard_EXPORT static TopAbs_Orientation Orient (const TopAbs_Orientation O, const Standard_Boolean R);
  
  Standard_EXPORT void FindSameDomain (TopTools_ListOfShape& L1, TopTools_ListOfShape& L2) const;
  
  Standard_EXPORT void FindSameDomainSameOrientation (TopTools_ListOfShape& LSO, TopTools_ListOfShape& LDO) const;
  
  Standard_EXPORT void MapShapes (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  Standard_EXPORT void ClearMaps();
  
  Standard_EXPORT void FindSameRank (const TopTools_ListOfShape& L1, const Standard_Integer R, TopTools_ListOfShape& L2) const;
  
  Standard_EXPORT Standard_Integer ShapeRank (const TopoDS_Shape& S) const;
  
  Standard_EXPORT Standard_Boolean IsShapeOf (const TopoDS_Shape& S, const Standard_Integer I12) const;
  
  Standard_EXPORT static Standard_Boolean Contains (const TopoDS_Shape& S, const TopTools_ListOfShape& L);
  
  Standard_EXPORT Standard_Integer FindIsKPart();
  
  Standard_EXPORT Standard_Integer IsKPart() const;
  
  Standard_EXPORT virtual void MergeKPart();
  
  Standard_EXPORT virtual void MergeKPart (const TopAbs_State TB1, const TopAbs_State TB2);
  
  Standard_EXPORT void MergeKPartiskole();
  
  Standard_EXPORT void MergeKPartiskoletge();
  
  Standard_EXPORT void MergeKPartisdisj();
  
  Standard_EXPORT void MergeKPartisfafa();
  
  Standard_EXPORT void MergeKPartissoso();
  
  Standard_EXPORT Standard_Integer KPiskole();
  
  Standard_EXPORT Standard_Integer KPiskoletge();
  
  Standard_EXPORT Standard_Integer KPisdisj();
  
  Standard_EXPORT Standard_Integer KPisfafa();
  
  Standard_EXPORT Standard_Integer KPissoso();
  
  Standard_EXPORT void KPClearMaps();
  
  Standard_EXPORT Standard_Integer KPlhg (const TopoDS_Shape& S, const TopAbs_ShapeEnum T, TopTools_ListOfShape& L) const;
  
  Standard_EXPORT Standard_Integer KPlhg (const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const;
  
  Standard_EXPORT Standard_Integer KPlhsd (const TopoDS_Shape& S, const TopAbs_ShapeEnum T, TopTools_ListOfShape& L) const;
  
  Standard_EXPORT Standard_Integer KPlhsd (const TopoDS_Shape& S, const TopAbs_ShapeEnum T) const;
  
  Standard_EXPORT TopAbs_State KPclasSS (const TopoDS_Shape& S1, const TopTools_ListOfShape& exceptLS1, const TopoDS_Shape& S2);
  
  Standard_EXPORT TopAbs_State KPclasSS (const TopoDS_Shape& S1, const TopoDS_Shape& exceptS1, const TopoDS_Shape& S2);
  
  Standard_EXPORT TopAbs_State KPclasSS (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  Standard_EXPORT Standard_Boolean KPiskolesh (const TopoDS_Shape& S, TopTools_ListOfShape& LS, TopTools_ListOfShape& LF) const;
  
  Standard_EXPORT Standard_Boolean KPiskoletgesh (const TopoDS_Shape& S, TopTools_ListOfShape& LS, TopTools_ListOfShape& LF) const;
  
  Standard_EXPORT void KPSameDomain (TopTools_ListOfShape& L1, TopTools_ListOfShape& L2) const;
  
  Standard_EXPORT Standard_Integer KPisdisjsh (const TopoDS_Shape& S) const;
  
  Standard_EXPORT Standard_Integer KPisfafash (const TopoDS_Shape& S) const;
  
  Standard_EXPORT Standard_Integer KPissososh (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void KPiskoleanalyse (const TopAbs_State FT1, const TopAbs_State FT2, const TopAbs_State ST1, const TopAbs_State ST2, Standard_Integer& I, Standard_Integer& I1, Standard_Integer& I2) const;
  
  Standard_EXPORT void KPiskoletgeanalyse (const TopOpeBRepDS_Config Conf, const TopAbs_State ST1, const TopAbs_State ST2, Standard_Integer& I) const;
  
  Standard_EXPORT void KPisdisjanalyse (const TopAbs_State ST1, const TopAbs_State ST2, Standard_Integer& I, Standard_Integer& IC1, Standard_Integer& IC2) const;
  
  Standard_EXPORT static Standard_Integer KPls (const TopoDS_Shape& S, const TopAbs_ShapeEnum T, TopTools_ListOfShape& L);
  
  Standard_EXPORT static Standard_Integer KPls (const TopoDS_Shape& S, const TopAbs_ShapeEnum T);
  
  Standard_EXPORT TopAbs_State KPclassF (const TopoDS_Shape& F1, const TopoDS_Shape& F2);
  
  Standard_EXPORT void KPclassFF (const TopoDS_Shape& F1, const TopoDS_Shape& F2, TopAbs_State& T1, TopAbs_State& T2);
  
  Standard_EXPORT Standard_Boolean KPiskoleFF (const TopoDS_Shape& F1, const TopoDS_Shape& F2, TopAbs_State& T1, TopAbs_State& T2);
  
  Standard_EXPORT static Standard_Boolean KPContains (const TopoDS_Shape& S, const TopTools_ListOfShape& L);
  
  Standard_EXPORT TopoDS_Shape KPmakeface (const TopoDS_Shape& F1, const TopTools_ListOfShape& LF2, const TopAbs_State T1, const TopAbs_State T2, const Standard_Boolean R1, const Standard_Boolean R2);
  
  Standard_EXPORT static Standard_Integer KPreturn (const Standard_Integer KP);
  
  Standard_EXPORT void SplitEvisoONperiodicF();
  
  Standard_EXPORT void GMergeSolids (const TopTools_ListOfShape& LSO1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT void GFillSolidsSFS (const TopTools_ListOfShape& LSO1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT virtual void GFillSolidSFS (const TopoDS_Shape& SO1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GFillSurfaceTopologySFS (const TopoDS_Shape& SO1, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GFillSurfaceTopologySFS (const TopOpeBRepDS_SurfaceIterator& IT, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS) const;
  
  Standard_EXPORT virtual void GFillShellSFS (const TopoDS_Shape& SH1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GFillFaceSFS (const TopoDS_Shape& F1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GSplitFaceSFS (const TopoDS_Shape& F1, const TopTools_ListOfShape& LSclass, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GMergeFaceSFS (const TopoDS_Shape& F, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GSplitFace (const TopoDS_Shape& F, const TopOpeBRepBuild_GTopo& G, const TopTools_ListOfShape& LSclass);
  
  Standard_EXPORT void AddONPatchesSFS (const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void FillOnPatches (const TopTools_ListOfShape& anEdgesON, const TopoDS_Shape& aBaseFace, const TopTools_IndexedMapOfOrientedShape& avoidMap);
  
  Standard_EXPORT void FindFacesTouchingEdge (const TopoDS_Shape& aFace, const TopoDS_Shape& anEdge, const Standard_Integer aShRank, TopTools_ListOfShape& aFaces) const;
  
  Standard_EXPORT void GMergeFaces (const TopTools_ListOfShape& LF1, const TopTools_ListOfShape& LF2, const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT void GFillFacesWES (const TopTools_ListOfShape& LF1, const TopTools_ListOfShape& LF2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillFacesWESK (const TopTools_ListOfShape& LF1, const TopTools_ListOfShape& LF2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES, const Standard_Integer K);
  
  Standard_EXPORT void GFillFacesWESMakeFaces (const TopTools_ListOfShape& LF1, const TopTools_ListOfShape& LF2, const TopTools_ListOfShape& LSO, const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT void GFillFaceWES (const TopoDS_Shape& F, const TopTools_ListOfShape& LF2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillCurveTopologyWES (const TopoDS_Shape& F, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillCurveTopologyWES (const TopOpeBRepDS_CurveIterator& IT, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES) const;
  
  Standard_EXPORT void GFillONPartsWES (const TopoDS_Shape& F, const TopOpeBRepBuild_GTopo& G, const TopTools_ListOfShape& LSclass, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillWireWES (const TopoDS_Shape& W, const TopTools_ListOfShape& LF2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillEdgeWES (const TopoDS_Shape& E, const TopTools_ListOfShape& LF2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GSplitEdgeWES (const TopoDS_Shape& E, const TopTools_ListOfShape& LF2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GMergeEdgeWES (const TopoDS_Shape& E, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GSplitEdge (const TopoDS_Shape& E, const TopOpeBRepBuild_GTopo& G, const TopTools_ListOfShape& LSclass);
  
  Standard_EXPORT void GMergeEdges (const TopTools_ListOfShape& LE1, const TopTools_ListOfShape& LE2, const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT void GFillEdgesPVS (const TopTools_ListOfShape& LE1, const TopTools_ListOfShape& LE2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_PaveSet& PVS);
  
  Standard_EXPORT void GFillEdgePVS (const TopoDS_Shape& E, const TopTools_ListOfShape& LE2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_PaveSet& PVS);
  
  Standard_EXPORT void GFillPointTopologyPVS (const TopoDS_Shape& E, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_PaveSet& PVS);
  
  Standard_EXPORT void GFillPointTopologyPVS (const TopoDS_Shape& E, const TopOpeBRepDS_PointIterator& IT, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_PaveSet& PVS) const;
  
  Standard_EXPORT Standard_Boolean GParamOnReference (const TopoDS_Vertex& V, const TopoDS_Edge& E, Standard_Real& P) const;
  
  Standard_EXPORT Standard_Boolean GKeepShape (const TopoDS_Shape& S, const TopTools_ListOfShape& Lref, const TopAbs_State T);
  
  //! return True if S is classified <T> / Lref shapes
  Standard_EXPORT Standard_Boolean GKeepShape1 (const TopoDS_Shape& S, const TopTools_ListOfShape& Lref, const TopAbs_State T, TopAbs_State& pos);
  
  //! add to Lou the shapes of Lin classified <T> / Lref shapes.
  //! Lou is not cleared. (S is a dummy trace argument)
  Standard_EXPORT void GKeepShapes (const TopoDS_Shape& S, const TopTools_ListOfShape& Lref, const TopAbs_State T, const TopTools_ListOfShape& Lin, TopTools_ListOfShape& Lou);
  
  Standard_EXPORT void GSFSMakeSolids (const TopoDS_Shape& SOF, TopOpeBRepBuild_ShellFaceSet& SFS, TopTools_ListOfShape& LOSO);
  
  Standard_EXPORT void GSOBUMakeSolids (const TopoDS_Shape& SOF, TopOpeBRepBuild_SolidBuilder& SOBU, TopTools_ListOfShape& LOSO);
  
  Standard_EXPORT virtual void GWESMakeFaces (const TopoDS_Shape& FF, TopOpeBRepBuild_WireEdgeSet& WES, TopTools_ListOfShape& LOF);
  
  Standard_EXPORT void GFABUMakeFaces (const TopoDS_Shape& FF, TopOpeBRepBuild_FaceBuilder& FABU, TopTools_ListOfShape& LOF, TopTools_DataMapOfShapeInteger& MWisOld);
  
  Standard_EXPORT void RegularizeFaces (const TopoDS_Shape& FF, const TopTools_ListOfShape& lnewFace, TopTools_ListOfShape& LOF);
  
  Standard_EXPORT void RegularizeFace (const TopoDS_Shape& FF, const TopoDS_Shape& newFace, TopTools_ListOfShape& LOF);
  
  Standard_EXPORT void RegularizeSolids (const TopoDS_Shape& SS, const TopTools_ListOfShape& lnewSolid, TopTools_ListOfShape& LOS);
  
  Standard_EXPORT void RegularizeSolid (const TopoDS_Shape& SS, const TopoDS_Shape& newSolid, TopTools_ListOfShape& LOS);
  
  Standard_EXPORT void GPVSMakeEdges (const TopoDS_Shape& EF, TopOpeBRepBuild_PaveSet& PVS, TopTools_ListOfShape& LOE) const;
  
  Standard_EXPORT void GEDBUMakeEdges (const TopoDS_Shape& EF, TopOpeBRepBuild_EdgeBuilder& EDBU, TopTools_ListOfShape& LOE) const;
  
  Standard_EXPORT Standard_Boolean GToSplit (const TopoDS_Shape& S, const TopAbs_State TB) const;
  
  Standard_EXPORT Standard_Boolean GToMerge (const TopoDS_Shape& S) const;
  
  Standard_EXPORT static Standard_Boolean GTakeCommonOfSame (const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT static Standard_Boolean GTakeCommonOfDiff (const TopOpeBRepBuild_GTopo& G);
  
  Standard_EXPORT void GFindSamDom (const TopoDS_Shape& S, TopTools_ListOfShape& L1, TopTools_ListOfShape& L2) const;
  
  Standard_EXPORT void GFindSamDom (TopTools_ListOfShape& L1, TopTools_ListOfShape& L2) const;
  
  Standard_EXPORT void GFindSamDomSODO (const TopoDS_Shape& S, TopTools_ListOfShape& LSO, TopTools_ListOfShape& LDO) const;
  
  Standard_EXPORT void GFindSamDomSODO (TopTools_ListOfShape& LSO, TopTools_ListOfShape& LDO) const;
  
  Standard_EXPORT void GMapShapes (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  Standard_EXPORT void GClearMaps();
  
  Standard_EXPORT void GFindSameRank (const TopTools_ListOfShape& L1, const Standard_Integer R, TopTools_ListOfShape& L2) const;
  
  Standard_EXPORT Standard_Integer GShapeRank (const TopoDS_Shape& S) const;
  
  Standard_EXPORT Standard_Boolean GIsShapeOf (const TopoDS_Shape& S, const Standard_Integer I12) const;
  
  Standard_EXPORT static Standard_Boolean GContains (const TopoDS_Shape& S, const TopTools_ListOfShape& L);
  
  Standard_EXPORT static void GCopyList (const TopTools_ListOfShape& Lin, const Standard_Integer i1, const Standard_Integer i2, TopTools_ListOfShape& Lou);
  
  Standard_EXPORT static void GCopyList (const TopTools_ListOfShape& Lin, TopTools_ListOfShape& Lou);
  
  Standard_EXPORT void GdumpLS (const TopTools_ListOfShape& L) const;
  
  Standard_EXPORT static void GdumpPNT (const gp_Pnt& P);
  
  Standard_EXPORT static void GdumpORIPARPNT (const TopAbs_Orientation o, const Standard_Real p, const gp_Pnt& Pnt);
  
  Standard_EXPORT void GdumpSHA (const TopoDS_Shape& S, const Standard_Address str = NULL) const;
  
  Standard_EXPORT void GdumpSHAORI (const TopoDS_Shape& S, const Standard_Address str = NULL) const;
  
  Standard_EXPORT void GdumpSHAORIGEO (const TopoDS_Shape& S, const Standard_Address str = NULL) const;
  
  Standard_EXPORT void GdumpSHASTA (const Standard_Integer iS, const TopAbs_State T, const TCollection_AsciiString& a = "", const TCollection_AsciiString& b = "") const;
  
  Standard_EXPORT void GdumpSHASTA (const TopoDS_Shape& S, const TopAbs_State T, const TCollection_AsciiString& a = "", const TCollection_AsciiString& b = "") const;
  
  Standard_EXPORT void GdumpSHASTA (const Standard_Integer iS, const TopAbs_State T, const TopOpeBRepBuild_ShapeSet& SS, const TCollection_AsciiString& a = "", const TCollection_AsciiString& b = "", const TCollection_AsciiString& c = "\n") const;
  
  Standard_EXPORT void GdumpEDG (const TopoDS_Shape& S, const Standard_Address str = NULL) const;
  
  Standard_EXPORT void GdumpEDGVER (const TopoDS_Shape& E, const TopoDS_Shape& V, const Standard_Address str = NULL) const;
  
  Standard_EXPORT void GdumpSAMDOM (const TopTools_ListOfShape& L, const Standard_Address str = NULL) const;
  
  Standard_EXPORT void GdumpEXP (const TopOpeBRepTool_ShapeExplorer& E) const;
  
  Standard_EXPORT void GdumpSOBU (TopOpeBRepBuild_SolidBuilder& SB) const;
  
  Standard_EXPORT void GdumpFABU (TopOpeBRepBuild_FaceBuilder& FB) const;
  
  Standard_EXPORT void GdumpEDBU (TopOpeBRepBuild_EdgeBuilder& EB) const;
  
  Standard_EXPORT Standard_Boolean GtraceSPS (const Standard_Integer iS) const;
  
  Standard_EXPORT Standard_Boolean GtraceSPS (const Standard_Integer iS, const Standard_Integer jS) const;
  
  Standard_EXPORT Standard_Boolean GtraceSPS (const TopoDS_Shape& S) const;
  
  Standard_EXPORT Standard_Boolean GtraceSPS (const TopoDS_Shape& S, Standard_Integer& IS) const;
  
  Standard_EXPORT void GdumpSHASETreset();
  
  Standard_EXPORT Standard_Integer GdumpSHASETindex();
  
  Standard_EXPORT static void PrintGeo (const TopoDS_Shape& S);
  
  Standard_EXPORT static void PrintSur (const TopoDS_Face& F);
  
  Standard_EXPORT static void PrintCur (const TopoDS_Edge& E);
  
  Standard_EXPORT static void PrintPnt (const TopoDS_Vertex& V);
  
  Standard_EXPORT static void PrintOri (const TopoDS_Shape& S);
  
  Standard_EXPORT static TCollection_AsciiString StringState (const TopAbs_State S);
  
  Standard_EXPORT static Standard_Boolean GcheckNBOUNDS (const TopoDS_Shape& E);


friend class TopOpeBRepBuild_HBuilder;


protected:

  
  //! update the DS by creating new geometries.
  //! create edges on the new curve <Icurv>.
  Standard_EXPORT void BuildEdges (const Standard_Integer iC, const Handle(TopOpeBRepDS_HDataStructure)& DS);
  
  //! update the DS by creating new geometries.
  //! create faces on the new surface <ISurf>.
  Standard_EXPORT void BuildFaces (const Standard_Integer iS, const Handle(TopOpeBRepDS_HDataStructure)& DS);
  
  //! update the DS by creating new geometries.
  //! create shapes from the new geometries.
  Standard_EXPORT void BuildFaces (const Handle(TopOpeBRepDS_HDataStructure)& DS);
  
  //! Split <E1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitEdge (const TopoDS_Shape& E1, const TopAbs_State TB1, const TopAbs_State TB2);
  
  //! Split <E1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitEdge1 (const TopoDS_Shape& E1, const TopAbs_State TB1, const TopAbs_State TB2);
  
  //! Split <E1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitEdge2 (const TopoDS_Shape& E1, const TopAbs_State TB1, const TopAbs_State TB2);
  
  //! Split <F1> keeping the  parts of state  <TB1>.
  //! Merge faces with same domain, keeping parts  of
  //! state <TB2>.
  Standard_EXPORT void SplitFace (const TopoDS_Shape& F1, const TopAbs_State TB1, const TopAbs_State TB2);
  
  Standard_EXPORT void SplitFace1 (const TopoDS_Shape& F1, const TopAbs_State TB1, const TopAbs_State TB2);
  
  Standard_EXPORT void SplitFace2 (const TopoDS_Shape& F1, const TopAbs_State TB1, const TopAbs_State TB2);
  
  //! Split <S1> keeping the parts of state <TB1>.
  Standard_EXPORT void SplitSolid (const TopoDS_Shape& S1, const TopAbs_State TB1, const TopAbs_State TB2);
  
  //! Explore shapes of given  by explorer <Ex> to split them.
  //! Store  new shapes in the set <SS>.
  //! According to RevOri, reverse or not their orientation.
  Standard_EXPORT void SplitShapes (TopOpeBRepTool_ShapeExplorer& Ex, const TopAbs_State TB1, const TopAbs_State TB2, TopOpeBRepBuild_ShapeSet& SS, const Standard_Boolean RevOri);
  
  //! Split edges of <F1> and store  wires and edges in
  //! the set <WES>. According to RevOri, reverse (or not) orientation.
  Standard_EXPORT void FillFace (const TopoDS_Shape& F1, const TopAbs_State TB1, const TopTools_ListOfShape& LF2, const TopAbs_State TB2, TopOpeBRepBuild_WireEdgeSet& WES, const Standard_Boolean RevOri);
  
  //! Split faces of <S1> and store shells  and faces in
  //! the set <SS>. According to RevOri, reverse (or not) orientation.
  Standard_EXPORT void FillSolid (const TopoDS_Shape& S1, const TopAbs_State TB1, const TopTools_ListOfShape& LS2, const TopAbs_State TB2, TopOpeBRepBuild_ShapeSet& SS, const Standard_Boolean RevOri);
  
  //! Split subshapes of <S1> and store subshapes in
  //! the set <SS>. According to RevOri, reverse (or not) orientation.
  Standard_EXPORT void FillShape (const TopoDS_Shape& S1, const TopAbs_State TB1, const TopTools_ListOfShape& LS2, const TopAbs_State TB2, TopOpeBRepBuild_ShapeSet& SS, const Standard_Boolean RevOri);
  
  //! fills the vertex set PVS with the point iterator IT.
  //! IT accesses a list of interferences which geometry is a point or a vertex.
  //! TB indicates the orientation to give to the geometries
  //! found in interference list accessed by IT.
  Standard_EXPORT void FillVertexSet (TopOpeBRepDS_PointIterator& IT, const TopAbs_State TB, TopOpeBRepBuild_PaveSet& PVS) const;
  
  //! fills vertex set PVS with the current value of IT.
  //! I geometry is a point or a vertex.
  //! TB  indicates the orientation to give to geometries found I
  Standard_EXPORT void FillVertexSetOnValue (const TopOpeBRepDS_PointIterator& IT, const TopAbs_State TB, TopOpeBRepBuild_PaveSet& PVS) const;
  
  //! Returns True if the shape <S> has not already been split
  Standard_EXPORT Standard_Boolean ToSplit (const TopoDS_Shape& S, const TopAbs_State TB) const;
  
  //! add the shape <S> to the map of split shapes.
  //! mark <S> as split/not split on <state>, according to B value.
  Standard_EXPORT void MarkSplit (const TopoDS_Shape& S, const TopAbs_State TB, const Standard_Boolean B = Standard_True);
  
  //! Returns a ref. on the list of shapes connected to <S> as
  //! <TB> merged parts of <S>.
  Standard_EXPORT TopTools_ListOfShape& ChangeMerged (const TopoDS_Shape& S, const TopAbs_State TB);
  
  //! Returns a ref. on the vertex created on point <I>.
  Standard_EXPORT TopoDS_Shape& ChangeNewVertex (const Standard_Integer I);
  
  //! Returns a ref. on the list of edges created on curve <I>.
  Standard_EXPORT TopTools_ListOfShape& ChangeNewEdges (const Standard_Integer I);
  
  //! Returns a ref. on the list of faces created on surface <I>.
  Standard_EXPORT TopTools_ListOfShape& ChangeNewFaces (const Standard_Integer I);
  
  Standard_EXPORT void AddIntersectionEdges (TopoDS_Shape& F, const TopAbs_State TB, const Standard_Boolean RevOri, TopOpeBRepBuild_ShapeSet& ES) const;
  
  Standard_EXPORT void UpdateSplitAndMerged (const TopTools_DataMapOfIntegerListOfShape& mle, const TopTools_DataMapOfIntegerShape& mre, const TopTools_DataMapOfShapeShape& mlf, const TopAbs_State state);


  TopAbs_State myState1;
  TopAbs_State myState2;
  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
  Handle(TopOpeBRepDS_HDataStructure) myDataStructure;
  TopOpeBRepDS_BuildTool myBuildTool;
  Handle(TopTools_HArray1OfShape) myNewVertices;
  TopTools_DataMapOfIntegerListOfShape myNewEdges;
  Handle(TopTools_HArray1OfListOfShape) myNewFaces;
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State mySplitIN;
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State mySplitON;
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State mySplitOUT;
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State myMergedIN;
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State myMergedON;
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State myMergedOUT;
  TopTools_ListOfShape myEmptyShapeList;
  TopTools_ListOfShape myListOfSolid;
  TopTools_ListOfShape myListOfFace;
  TopTools_ListOfShape myListOfEdge;
  TopTools_DataMapOfShapeListOfShape myFSplits;
  TopTools_DataMapOfShapeListOfShape myESplits;
  Standard_Boolean mySectionDone;
  Standard_Boolean mySplitSectionEdgesDone;
  TopTools_ListOfShape mySection;
  TopoDS_Solid mySolidReference;
  TopoDS_Solid mySolidToFill;
  TopTools_ListOfShape myFaceAvoid;
  TopoDS_Face myFaceReference;
  TopoDS_Face myFaceToFill;
  TopTools_ListOfShape myEdgeAvoid;
  TopoDS_Edge myEdgeReference;
  TopoDS_Edge myEdgeToFill;
  TopTools_ListOfShape myVertexAvoid;
  TopTools_IndexedMapOfShape myMAP1;
  TopTools_IndexedMapOfShape myMAP2;
  Standard_Integer myIsKPart;
  TopTools_DataMapOfShapeListOfShape myKPMAPf1f2;
  Standard_Integer mySHASETindex;
  Standard_Boolean myClassifyDef;
  Standard_Boolean myClassifyVal;
  TopOpeBRepTool_ShapeClassifier myShapeClassifier;
  TopTools_MapOfShape myMemoSplit;
  TCollection_AsciiString myEmptyAS;
  Standard_Boolean myProcessON;
  TopTools_IndexedDataMapOfShapeShape myONFacesMap;
  TopTools_IndexedMapOfOrientedShape myONElemMap;


private:





};







#endif // _TopOpeBRepBuild_Builder_HeaderFile
