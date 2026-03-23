// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraph_AnalyzeView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_UsagesView.hxx>

#include <BRepGraph_Analyze.hxx>

// ============================================================
// DefsView
// ============================================================

//=================================================================================================

int BRepGraph::DefsView::NbSolids() const { return myGraph->NbSolidDefs(); }

//=================================================================================================

int BRepGraph::DefsView::NbShells() const { return myGraph->NbShellDefs(); }

//=================================================================================================

int BRepGraph::DefsView::NbFaces() const { return myGraph->NbFaceDefs(); }

//=================================================================================================

int BRepGraph::DefsView::NbWires() const { return myGraph->NbWireDefs(); }

//=================================================================================================

int BRepGraph::DefsView::NbEdges() const { return myGraph->NbEdgeDefs(); }

//=================================================================================================

int BRepGraph::DefsView::NbVertices() const { return myGraph->NbVertexDefs(); }

//=================================================================================================

int BRepGraph::DefsView::NbCompounds() const { return myGraph->NbCompoundDefs(); }

//=================================================================================================

int BRepGraph::DefsView::NbCompSolids() const { return myGraph->NbCompSolidDefs(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidDef& BRepGraph::DefsView::Solid(int theIdx) const
{ return myGraph->SolidDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::ShellDef& BRepGraph::DefsView::Shell(int theIdx) const
{ return myGraph->ShellDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::FaceDef& BRepGraph::DefsView::Face(int theIdx) const
{ return myGraph->FaceDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::WireDef& BRepGraph::DefsView::Wire(int theIdx) const
{ return myGraph->WireDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef& BRepGraph::DefsView::Edge(int theIdx) const
{ return myGraph->EdgeDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::VertexDef& BRepGraph::DefsView::Vertex(int theIdx) const
{ return myGraph->VertexDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::CompoundDef& BRepGraph::DefsView::Compound(int theIdx) const
{ return myGraph->CompoundDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::DefsView::CompSolid(int theIdx) const
{ return myGraph->CompSolidDefinition(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::DefsView::TopoDef(BRepGraph_NodeId theId) const
{ return myGraph->TopoDef(theId); }

//=================================================================================================

size_t BRepGraph::DefsView::NbNodes() const { return myGraph->NbNodes(); }

// ============================================================
// UsagesView
// ============================================================

//=================================================================================================

int BRepGraph::UsagesView::NbSolids() const { return myGraph->NbSolidUsages(); }

//=================================================================================================

int BRepGraph::UsagesView::NbShells() const { return myGraph->NbShellUsages(); }

//=================================================================================================

int BRepGraph::UsagesView::NbFaces() const { return myGraph->NbFaceUsages(); }

//=================================================================================================

int BRepGraph::UsagesView::NbWires() const { return myGraph->NbWireUsages(); }

//=================================================================================================

int BRepGraph::UsagesView::NbEdges() const { return myGraph->NbEdgeUsages(); }

//=================================================================================================

int BRepGraph::UsagesView::NbVertices() const { return myGraph->NbVertexUsages(); }

//=================================================================================================

int BRepGraph::UsagesView::NbCompounds() const { return myGraph->NbCompoundUsages(); }

//=================================================================================================

int BRepGraph::UsagesView::NbCompSolids() const { return myGraph->NbCompSolidUsages(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidUsage& BRepGraph::UsagesView::Solid(int theIdx) const
{ return myGraph->SolidUsageNode(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::ShellUsage& BRepGraph::UsagesView::Shell(int theIdx) const
{ return myGraph->ShellUsageNode(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::FaceUsage& BRepGraph::UsagesView::Face(int theIdx) const
{ return myGraph->FaceUsageNode(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::WireUsage& BRepGraph::UsagesView::Wire(int theIdx) const
{ return myGraph->WireUsageNode(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::EdgeUsage& BRepGraph::UsagesView::Edge(int theIdx) const
{ return myGraph->EdgeUsageNode(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::VertexUsage& BRepGraph::UsagesView::Vertex(int theIdx) const
{ return myGraph->VertexUsageNode(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::CompoundUsage& BRepGraph::UsagesView::Compound(int theIdx) const
{ return myGraph->CompoundUsageNode(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::CompSolidUsage& BRepGraph::UsagesView::CompSolid(int theIdx) const
{ return myGraph->CompSolidUsageNode(theIdx); }

// ============================================================
// GeomView
// ============================================================

//=================================================================================================

int BRepGraph::GeomView::NbSurfaces() const { return myGraph->NbSurfaces(); }

//=================================================================================================

int BRepGraph::GeomView::NbCurves() const { return myGraph->NbCurves(); }

//=================================================================================================

int BRepGraph::GeomView::NbPCurves() const { return myGraph->NbPCurves(); }

//=================================================================================================

const BRepGraph_GeomNode::Surf& BRepGraph::GeomView::Surface(int theIdx) const
{ return myGraph->SurfNode(theIdx); }

//=================================================================================================

const BRepGraph_GeomNode::Curve& BRepGraph::GeomView::Curve(int theIdx) const
{ return myGraph->CurveNode(theIdx); }

//=================================================================================================

const BRepGraph_GeomNode::PCurve& BRepGraph::GeomView::PCurve(int theIdx) const
{ return myGraph->PCurveNode(theIdx); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::SurfaceOf(BRepGraph_NodeId theFaceDef) const
{ return myGraph->SurfaceOf(theFaceDef); }

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::GeomView::FacesOnSurface(
  BRepGraph_NodeId theSurf) const
{ return myGraph->FacesOnSurface(theSurf); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::CurveOf(BRepGraph_NodeId theEdgeDef) const
{ return myGraph->CurveOf(theEdgeDef); }

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::GeomView::EdgesOnCurve(
  BRepGraph_NodeId theCurve) const
{ return myGraph->EdgesOnCurve(theCurve); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::PCurveOf(BRepGraph_NodeId theEdgeDef,
                                               BRepGraph_NodeId theFaceDef) const
{ return myGraph->PCurveOf(theEdgeDef, theFaceDef); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::GeomView::PCurveOf(BRepGraph_NodeId   theEdgeDef,
                                               BRepGraph_NodeId   theFaceDef,
                                               TopAbs_Orientation theEdgeOrientation) const
{ return myGraph->PCurveOf(theEdgeDef, theFaceDef, theEdgeOrientation); }

// ============================================================
// UIDsView
// ============================================================

//=================================================================================================

bool BRepGraph::UIDsView::IsEnabled() const { return myGraph->IsUIDEnabled(); }

//=================================================================================================

BRepGraph_UID BRepGraph::UIDsView::Of(BRepGraph_NodeId theNode) const
{ return myGraph->UIDOf(theNode); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::UIDsView::NodeIdFrom(const BRepGraph_UID& theUID) const
{ return myGraph->NodeIdFromUID(theUID); }

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_UID& theUID) const
{ return myGraph->HasUID(theUID); }

//=================================================================================================

uint32_t BRepGraph::UIDsView::Generation() const { return myGraph->Generation(); }

// ============================================================
// RelEdgesView
// ============================================================

//=================================================================================================

int BRepGraph::RelEdgesView::NbFrom(BRepGraph_NodeId theNode) const
{ return myGraph->NbRelEdgesFrom(theNode); }

//=================================================================================================

int BRepGraph::RelEdgesView::NbTo(BRepGraph_NodeId theNode) const
{ return myGraph->NbRelEdgesTo(theNode); }

//=================================================================================================

const NCollection_Vector<BRepGraph_RelEdge>* BRepGraph::RelEdgesView::OutOf(
  BRepGraph_NodeId theNode) const
{ return myGraph->OutRelEdgesOf(theNode); }

//=================================================================================================

const NCollection_Vector<BRepGraph_RelEdge>* BRepGraph::RelEdgesView::InOf(
  BRepGraph_NodeId theNode) const
{ return myGraph->InRelEdgesOf(theNode); }

//=================================================================================================

int BRepGraph::RelEdgesView::FaceCountForEdge(int theEdgeDefIdx) const
{ return myGraph->FaceCountForEdge(theEdgeDefIdx); }

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::RelEdgesView::WiresOfEdge(int theEdgeDefIdx) const
{ return myGraph->WiresOfEdge(theEdgeDefIdx); }

// ============================================================
// SpatialView
// ============================================================

//=================================================================================================

gp_Trsf BRepGraph::SpatialView::GlobalTransform(BRepGraph_UsageId theUsage) const
{ return myGraph->GlobalTransform(theUsage); }

//=================================================================================================

gp_Trsf BRepGraph::SpatialView::GlobalTransform(BRepGraph_NodeId theDefId) const
{ return myGraph->GlobalTransform(theDefId); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SameDomainFaces(
  BRepGraph_NodeId theFaceDef) const
{ return myGraph->SameDomainFaces(theFaceDef); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::FacesOfEdge(
  BRepGraph_NodeId theEdgeDef) const
{ return myGraph->FacesOfEdge(theEdgeDef); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SharedEdges(
  BRepGraph_NodeId theFaceA,
  BRepGraph_NodeId theFaceB) const
{ return myGraph->SharedEdges(theFaceA, theFaceB); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::AdjacentFaces(
  BRepGraph_NodeId theFaceDef) const
{ return myGraph->AdjacentFaces(theFaceDef); }

// ============================================================
// MutView
// ============================================================

//=================================================================================================

BRepGraph_TopoNode::EdgeDef& BRepGraph::MutView::EdgeDef(int theIdx)
{ return myGraph->MutableEdgeDefinition(theIdx); }

//=================================================================================================

BRepGraph_TopoNode::WireDef& BRepGraph::MutView::WireDef(int theIdx)
{ return myGraph->MutableWireDefinition(theIdx); }

//=================================================================================================

BRepGraph_TopoNode::VertexDef& BRepGraph::MutView::VertexDef(int theIdx)
{ return myGraph->MutableVertexDefinition(theIdx); }

//=================================================================================================

BRepGraph_TopoNode::FaceDef& BRepGraph::MutView::FaceDef(int theIdx)
{ return myGraph->MutableFaceDefinition(theIdx); }

//=================================================================================================

BRepGraph_TopoNode::ShellDef& BRepGraph::MutView::ShellDef(int theIdx)
{ return myGraph->MutableShellDefinition(theIdx); }

//=================================================================================================

BRepGraph_TopoNode::SolidDef& BRepGraph::MutView::SolidDef(int theIdx)
{ return myGraph->MutableSolidDefinition(theIdx); }

//=================================================================================================

BRepGraph_TopoNode::CompoundDef& BRepGraph::MutView::CompoundDef(int theIdx)
{ return myGraph->MutableCompoundDefinition(theIdx); }

//=================================================================================================

BRepGraph_TopoNode::CompSolidDef& BRepGraph::MutView::CompSolidDef(int theIdx)
{ return myGraph->MutableCompSolidDefinition(theIdx); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::MutView::AddPCurveToEdge(
  BRepGraph_NodeId            theEdgeDef,
  BRepGraph_NodeId            theFaceDef,
  const Handle(Geom2d_Curve)& theCurve2d,
  double                      theFirst,
  double                      theLast,
  TopAbs_Orientation          theEdgeOrientation)
{ return myGraph->AddPCurveToEdge(theEdgeDef, theFaceDef, theCurve2d, theFirst, theLast, theEdgeOrientation); }

//=================================================================================================

void BRepGraph::MutView::ReplaceEdgeInWire(int              theWireDefIdx,
                                           BRepGraph_NodeId theOldEdgeDef,
                                           BRepGraph_NodeId theNewEdgeDef,
                                           bool             theReversed)
{ myGraph->ReplaceEdgeInWire(theWireDefIdx, theOldEdgeDef, theNewEdgeDef, theReversed); }

//=================================================================================================

void BRepGraph::MutView::SplitEdge(BRepGraph_NodeId  theEdgeDef,
                                   BRepGraph_NodeId  theSplitVertex,
                                   double            theSplitParam,
                                   BRepGraph_NodeId& theSubA,
                                   BRepGraph_NodeId& theSubB)
{ myGraph->SplitEdge(theEdgeDef, theSplitVertex, theSplitParam, theSubA, theSubB); }

//=================================================================================================

int BRepGraph::MutView::AddRelEdge(BRepGraph_NodeId  theFrom,
                                   BRepGraph_NodeId  theTo,
                                   BRepGraph_RelKind theKind)
{ return myGraph->AddRelEdge(theFrom, theTo, theKind); }

//=================================================================================================

void BRepGraph::MutView::RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                        BRepGraph_NodeId  theTo,
                                        BRepGraph_RelKind theKind)
{ myGraph->RemoveRelEdges(theFrom, theTo, theKind); }

// ============================================================
// BuilderView
// ============================================================

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddVertexDef(const gp_Pnt& thePoint, double theTolerance)
{ return myGraph->AddVertexDef(thePoint, theTolerance); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddEdgeDef(BRepGraph_NodeId          theStartVtx,
                                                    BRepGraph_NodeId          theEndVtx,
                                                    const Handle(Geom_Curve)& theCurve,
                                                    double                    theFirst,
                                                    double                    theLast,
                                                    double                    theTolerance)
{ return myGraph->AddEdgeDef(theStartVtx, theEndVtx, theCurve, theFirst, theLast, theTolerance); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddWireDef(
  const NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry>& theEdges)
{ return myGraph->AddWireDef(theEdges); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddFaceDef(
  const Handle(Geom_Surface)&                 theSurface,
  BRepGraph_NodeId                            theOuterWire,
  const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
  double                                      theTolerance)
{ return myGraph->AddFaceDef(theSurface, theOuterWire, theInnerWires, theTolerance); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddShellDef()
{ return myGraph->AddShellDef(); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddSolidDef()
{ return myGraph->AddSolidDef(); }

//=================================================================================================

BRepGraph_UsageId BRepGraph::BuilderView::AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                                         BRepGraph_NodeId   theFaceDef,
                                                         TopAbs_Orientation theOri)
{ return myGraph->AddFaceToShell(theShellDef, theFaceDef, theOri); }

//=================================================================================================

BRepGraph_UsageId BRepGraph::BuilderView::AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                                          BRepGraph_NodeId   theShellDef,
                                                          TopAbs_Orientation theOri)
{ return myGraph->AddShellToSolid(theSolidDef, theShellDef, theOri); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompoundDef(
  const NCollection_Vector<BRepGraph_NodeId>& theChildDefs)
{ return myGraph->AddCompoundDef(theChildDefs); }

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompSolidDef(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs)
{ return myGraph->AddCompSolidDef(theSolidDefs); }

//=================================================================================================

void BRepGraph::BuilderView::AppendShape(const TopoDS_Shape& theShape, bool theParallel)
{ myGraph->AppendShape(theShape, theParallel); }

//=================================================================================================

void BRepGraph::BuilderView::RemoveNode(BRepGraph_NodeId theNode)
{ myGraph->RemoveNode(theNode); }

//=================================================================================================

void BRepGraph::BuilderView::RemoveSubgraph(BRepGraph_NodeId theNode)
{ myGraph->RemoveSubgraph(theNode); }

//=================================================================================================

bool BRepGraph::BuilderView::IsRemoved(BRepGraph_NodeId theNode) const
{ return myGraph->IsRemoved(theNode); }

//=================================================================================================

int BRepGraph::BuilderView::FaceCountForEdge(int theEdgeDefIdx) const
{ return myGraph->FaceCountForEdge(theEdgeDefIdx); }

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::BuilderView::WiresOfEdge(int theEdgeDefIdx) const
{ return myGraph->WiresOfEdge(theEdgeDefIdx); }

// ============================================================
// AnalyzeView
// ============================================================

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::AnalyzeView::FreeEdges() const
{ return BRepGraph_Analyze::FreeEdges(*myGraph); }

//=================================================================================================

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
  BRepGraph::AnalyzeView::MissingPCurves() const
{ return BRepGraph_Analyze::MissingPCurves(*myGraph); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId>
  BRepGraph::AnalyzeView::ToleranceConflicts(double theThreshold) const
{ return BRepGraph_Analyze::ToleranceConflicts(*myGraph, theThreshold); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::AnalyzeView::DegenerateWires() const
{ return BRepGraph_Analyze::DegenerateWires(*myGraph); }

//=================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph::AnalyzeView::Decompose() const
{ return BRepGraph_Analyze::Decompose(*myGraph); }

//=================================================================================================

void BRepGraph::AnalyzeView::ParallelForEachFace(
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int faceIdx)>& theLambda) const
{ BRepGraph_Analyze::ParallelForEachFace(*myGraph, theSub, theLambda); }

//=================================================================================================

void BRepGraph::AnalyzeView::ParallelForEachEdge(
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int edgeIdx)>& theLambda) const
{ BRepGraph_Analyze::ParallelForEachEdge(*myGraph, theSub, theLambda); }
