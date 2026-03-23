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

#ifndef _BRepGraph_HeaderFile
#define _BRepGraph_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_UsageId.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_GeomNode.hxx>
#include <BRepGraph_RelEdge.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_SubGraph.hxx>
#include <BRepGraph_UserAttribute.hxx>

#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_DataMap.hxx>

#include <functional>
#include <memory>
#include <utility>

struct BRepGraph_Data;
class NCollection_BaseAllocator;
class TCollection_AsciiString;
class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
class Poly_Triangulation;

class BRepGraph_Builder;
class BRepGraph_History;
class BRepGraph_Analyze;
class BRepGraph_Reconstruct;
class BRepGraph_Mutator;

//! @brief Bidirectional topology-geometry graph over TopoDS / BRep.
//!
//! Two-layer architecture: Definitions (one per unique TShape) hold intrinsic
//! data (geometry links, tolerances).  Usages (one per occurrence) hold
//! context-specific data (orientation, location, parent).
//!
//! ## Grouped View API
//! Related methods are grouped behind lightweight view objects returned by
//! value.  Each view is a zero-cost inner class holding a single pointer.
//! Include the corresponding header (e.g. BRepGraph_DefsView.hxx) to use.
//! All 92 flat methods remain available unchanged.
//!
//! ## Thread safety
//! All const query methods (including lazy cache Gets) are thread-safe.
//! Build() is internally parallel when requested.
//! ApplyModification() requires exclusive access.
class BRepGraph
{
public:
  DEFINE_STANDARD_ALLOC

  BRepGraph(const BRepGraph&)            = delete;
  BRepGraph& operator=(const BRepGraph&) = delete;

  Standard_EXPORT BRepGraph();
  Standard_EXPORT explicit BRepGraph(const Handle(NCollection_BaseAllocator)& theAlloc);
  Standard_EXPORT ~BRepGraph();
  Standard_EXPORT BRepGraph(BRepGraph&&) noexcept;
  Standard_EXPORT BRepGraph& operator=(BRepGraph&&) noexcept;

  //! Build the full graph from a TopoDS_Shape.
  Standard_EXPORT void Build(const TopoDS_Shape& theShape, bool theParallel = false);

  //! Return true if the graph was successfully built.
  Standard_EXPORT bool IsDone() const;

  //! Number of solid definitions in the graph.
  Standard_EXPORT int NbSolidDefs() const;

  //! Number of shell definitions in the graph.
  Standard_EXPORT int NbShellDefs() const;

  //! Number of face definitions in the graph.
  Standard_EXPORT int NbFaceDefs() const;

  //! Number of wire definitions in the graph.
  Standard_EXPORT int NbWireDefs() const;

  //! Number of edge definitions in the graph.
  Standard_EXPORT int NbEdgeDefs() const;

  //! Number of vertex definitions in the graph.
  Standard_EXPORT int NbVertexDefs() const;

  //! Number of compound definitions in the graph.
  Standard_EXPORT int NbCompoundDefs() const;

  //! Number of compsolid definitions in the graph.
  Standard_EXPORT int NbCompSolidDefs() const;

  //! Number of surface geometry nodes in the graph.
  Standard_EXPORT int NbSurfaces() const;

  //! Number of 3D curve geometry nodes in the graph.
  Standard_EXPORT int NbCurves() const;

  //! Number of 2D PCurve geometry nodes in the graph.
  Standard_EXPORT int NbPCurves() const;

  //! Number of solid usage nodes in the graph.
  Standard_EXPORT int NbSolidUsages() const;

  //! Number of shell usage nodes in the graph.
  Standard_EXPORT int NbShellUsages() const;

  //! Number of face usage nodes in the graph.
  Standard_EXPORT int NbFaceUsages() const;

  //! Number of wire usage nodes in the graph.
  Standard_EXPORT int NbWireUsages() const;

  //! Number of edge usage nodes in the graph.
  Standard_EXPORT int NbEdgeUsages() const;

  //! Number of vertex usage nodes in the graph.
  Standard_EXPORT int NbVertexUsages() const;

  //! Number of compound usage nodes in the graph.
  Standard_EXPORT int NbCompoundUsages() const;

  //! Number of compsolid usage nodes in the graph.
  Standard_EXPORT int NbCompSolidUsages() const;

  //! Access solid definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::SolidDef& SolidDefinition(int theIdx) const;

  //! Access shell definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::ShellDef& ShellDefinition(int theIdx) const;

  //! Access face definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::FaceDef& FaceDefinition(int theIdx) const;

  //! Access wire definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::WireDef& WireDefinition(int theIdx) const;

  //! Access edge definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::EdgeDef& EdgeDefinition(int theIdx) const;

  //! Access vertex definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::VertexDef& VertexDefinition(int theIdx) const;

  //! Access compound definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::CompoundDef& CompoundDefinition(int theIdx) const;

  //! Access compsolid definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidDef& CompSolidDefinition(int theIdx) const;

  //! Access surface geometry node by index.
  //! @param[in] theIdx zero-based surface index
  Standard_EXPORT const BRepGraph_GeomNode::Surf& SurfNode(int theIdx) const;

  //! Access 3D curve geometry node by index.
  //! @param[in] theIdx zero-based curve index
  Standard_EXPORT const BRepGraph_GeomNode::Curve& CurveNode(int theIdx) const;

  //! Access PCurve geometry node by index.
  //! @param[in] theIdx zero-based PCurve index
  Standard_EXPORT const BRepGraph_GeomNode::PCurve& PCurveNode(int theIdx) const;

  //! Access solid usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::SolidUsage& SolidUsageNode(int theIdx) const;

  //! Access shell usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::ShellUsage& ShellUsageNode(int theIdx) const;

  //! Access face usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::FaceUsage& FaceUsageNode(int theIdx) const;

  //! Access wire usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::WireUsage& WireUsageNode(int theIdx) const;

  //! Access edge usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::EdgeUsage& EdgeUsageNode(int theIdx) const;

  //! Access vertex usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::VertexUsage& VertexUsageNode(int theIdx) const;

  //! Access compound usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::CompoundUsage& CompoundUsageNode(int theIdx) const;

  //! Access compsolid usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidUsage& CompSolidUsageNode(int theIdx) const;

  //! Return all usages of a given definition.
  Standard_EXPORT const NCollection_Vector<BRepGraph_UsageId>& UsagesOf(
    BRepGraph_NodeId theDefId) const;

  //! Return the definition NodeId for a given usage.
  Standard_EXPORT BRepGraph_NodeId DefOf(BRepGraph_UsageId theUsageId) const;

  //! Generic topology definition lookup by NodeId.
  //! @return pointer to BaseDef or nullptr if invalid
  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(BRepGraph_NodeId theId) const;

  //! Total number of nodes in the graph (all kinds).
  Standard_EXPORT size_t NbNodes() const;

  //! Enable or disable UID generation for newly created nodes.
  Standard_EXPORT void SetUIDEnabled(bool theVal);

  //! Check if UID generation is enabled.
  Standard_EXPORT bool IsUIDEnabled() const;

  //! Return the UID assigned to a node (empty if UIDs are disabled).
  Standard_EXPORT BRepGraph_UID UIDOf(BRepGraph_NodeId theNode) const;

  //! Resolve a UID back to a NodeId.
  Standard_EXPORT BRepGraph_NodeId NodeIdFromUID(const BRepGraph_UID& theUID) const;

  //! Check if a UID is valid and exists in this graph generation.
  Standard_EXPORT bool HasUID(const BRepGraph_UID& theUID) const;

  //! Current generation counter (incremented on each Build).
  Standard_EXPORT uint32_t Generation() const;

  //! Add a directed relation edge between two nodes.
  //! @return index of the new edge in the outgoing vector of theFrom
  Standard_EXPORT int AddRelEdge(BRepGraph_NodeId  theFrom,
                                 BRepGraph_NodeId  theTo,
                                 BRepGraph_RelKind theKind);

  //! Remove all relation edges of a given kind between two nodes.
  Standard_EXPORT void RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                      BRepGraph_NodeId  theTo,
                                      BRepGraph_RelKind theKind);

  //! Number of outgoing relation edges from a node.
  Standard_EXPORT int NbRelEdgesFrom(BRepGraph_NodeId theNode) const;

  //! Number of incoming relation edges to a node.
  Standard_EXPORT int NbRelEdgesTo(BRepGraph_NodeId theNode) const;

  //! Access outgoing RelEdge vector for a node.
  //! @return pointer to the vector or nullptr if none
  Standard_EXPORT const NCollection_Vector<BRepGraph_RelEdge>* OutRelEdgesOf(
    BRepGraph_NodeId theNode) const;

  //! Access incoming RelEdge vector for a node.
  //! @return pointer to the vector or nullptr if none
  Standard_EXPORT const NCollection_Vector<BRepGraph_RelEdge>* InRelEdgesOf(
    BRepGraph_NodeId theNode) const;

  //! Invoke a callback for each outgoing RelEdge of a given kind.
  //! @param[in] theNodeId source node
  //! @param[in] theKind   relation kind to filter by
  //! @param[in] theCallback callback receiving each matching BRepGraph_RelEdge
  template <typename Func>
  void ForEachOutEdgeOfKind(BRepGraph_NodeId  theNodeId,
                            BRepGraph_RelKind theKind,
                            const Func&       theCallback) const
  {
    const NCollection_Vector<BRepGraph_RelEdge>* aEdges = OutRelEdgesOf(theNodeId);
    if (aEdges == nullptr)
      return;
    for (int anIdx = 0; anIdx < aEdges->Length(); ++anIdx)
    {
      const BRepGraph_RelEdge& anEdge = aEdges->Value(anIdx);
      if (anEdge.Kind == theKind)
        theCallback(anEdge);
    }
  }

  //! Invoke a callback for each incoming RelEdge of a given kind.
  //! @param[in] theNodeId target node
  //! @param[in] theKind   relation kind to filter by
  //! @param[in] theCallback callback receiving each matching BRepGraph_RelEdge
  template <typename Func>
  void ForEachInEdgeOfKind(BRepGraph_NodeId  theNodeId,
                           BRepGraph_RelKind theKind,
                           const Func&       theCallback) const
  {
    const NCollection_Vector<BRepGraph_RelEdge>* aEdges = InRelEdgesOf(theNodeId);
    if (aEdges == nullptr)
      return;
    for (int anIdx = 0; anIdx < aEdges->Length(); ++anIdx)
    {
      const BRepGraph_RelEdge& anEdge = aEdges->Value(anIdx);
      if (anEdge.Kind == theKind)
        theCallback(anEdge);
    }
  }

  //! Return the surface definition NodeId for a face definition.
  Standard_EXPORT BRepGraph_NodeId SurfaceOf(BRepGraph_NodeId theFaceDef) const;

  //! Return all face definitions that reference a given surface.
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& FacesOnSurface(
    BRepGraph_NodeId theSurf) const;

  //! Return the 3D curve definition NodeId for an edge definition.
  Standard_EXPORT BRepGraph_NodeId CurveOf(BRepGraph_NodeId theEdgeDef) const;

  //! Return all edge definitions that reference a given 3D curve.
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& EdgesOnCurve(
    BRepGraph_NodeId theCurve) const;

  //! Return the PCurve NodeId for an edge on a given face.
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId theEdgeDef,
                                            BRepGraph_NodeId theFaceDef) const;

  //! Return the PCurve NodeId for an edge/face/orientation triple.
  //! For seam edges two PCurve nodes share the same FaceDefId but differ in orientation;
  //! use this overload to distinguish FORWARD (C1) from REVERSED (C2).
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId   theEdgeDef,
                                            BRepGraph_NodeId   theFaceDef,
                                            TopAbs_Orientation theEdgeOrientation) const;

  //! Accumulated global transform for a usage.
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_UsageId theUsage) const;

  //! Accumulated global transform for a definition (from first usage).
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_NodeId theDefId) const;

  //! Return all face definitions sharing the same surface as the given face.
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    BRepGraph_NodeId theFaceDef) const;

  //! Return all face definition NodeIds that reference this edge (via their wires).
  //! @param[in] theEdgeDef edge definition NodeId
  //! @return vector of face definition NodeIds
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FacesOfEdge(
    BRepGraph_NodeId theEdgeDef) const;

  //! Return all edges shared between two faces.
  //! @param[in] theFaceA first face definition NodeId
  //! @param[in] theFaceB second face definition NodeId
  //! @return vector of edge definition NodeIds
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SharedEdges(
    BRepGraph_NodeId theFaceA,
    BRepGraph_NodeId theFaceB) const;

  //! Return all faces adjacent to a face (sharing at least one edge).
  //! @param[in] theFaceDef face definition NodeId
  //! @return vector of adjacent face definition NodeIds
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentFaces(
    BRepGraph_NodeId theFaceDef) const;

  //! Compute or return cached axis-aligned bounding box for a node.
  Standard_EXPORT Bnd_Box BoundingBox(BRepGraph_NodeId theNode) const;

  //! Compute or return cached centroid for a node.
  Standard_EXPORT gp_Pnt Centroid(BRepGraph_NodeId theNode) const;

  //! Invalidate cached values (bounding box, centroid) for a single node.
  Standard_EXPORT void Invalidate(BRepGraph_NodeId theNode);

  //! Invalidate cached values for a node and all its descendants.
  Standard_EXPORT void InvalidateSubgraph(BRepGraph_NodeId theNode);

  //! Attach a user attribute to a node.
  Standard_EXPORT void SetUserAttribute(BRepGraph_NodeId             theNode,
                                        int                          theKey,
                                        const BRepGraph_UserAttrPtr& theAttr);

  //! Retrieve a user attribute from a node.
  //! @return shared pointer to the attribute, or nullptr if not set
  Standard_EXPORT BRepGraph_UserAttrPtr GetUserAttribute(BRepGraph_NodeId theNode,
                                                         int              theKey) const;

  //! Remove a user attribute from a node.
  //! @return true if the attribute was found and removed
  Standard_EXPORT bool RemoveUserAttribute(BRepGraph_NodeId theNode, int theKey);

  //! Invalidate (but do not remove) a user attribute on a node.
  Standard_EXPORT void InvalidateUserAttribute(BRepGraph_NodeId theNode, int theKey);

  //! Number of history records stored in the graph.
  Standard_EXPORT int NbHistoryRecords() const;

  //! Access a history record by index.
  Standard_EXPORT const BRepGraph_HistoryRecord& HistoryRecord(int theIdx) const;

  //! Find the original node that was modified to produce the given node.
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;

  //! Find all nodes transitively derived from an original node.
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(
    BRepGraph_NodeId theOriginal) const;

  //! Apply a modification operation and record history.
  //! @param[in] theTarget node to modify
  //! @param[in] theModifier callback that performs the modification and returns replacements
  //! @param[in] theOpLabel label for the history record
  Standard_EXPORT void ApplyModification(
    BRepGraph_NodeId                                                                    theTarget,
    std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
    const TCollection_AsciiString&                                                      theOpLabel);

  //! Return or reconstruct a TopoDS_Shape for a node.
  Standard_EXPORT TopoDS_Shape Shape(BRepGraph_NodeId theNode) const;

  //! Check if the node has an original shape from Build().
  Standard_EXPORT bool HasOriginalShape(BRepGraph_NodeId theNode) const;

  //! Return the original TopoDS_Shape stored during Build().
  //! @exception Standard_ProgramError if no original shape exists
  Standard_EXPORT const TopoDS_Shape& OriginalOf(BRepGraph_NodeId theNode) const;

  //! Reconstruct a TopoDS_Shape from a definition node.
  Standard_EXPORT TopoDS_Shape ReconstructShape(BRepGraph_NodeId theRoot) const;

  //! Reconstruct a TopoDS_Face from a face definition index.
  Standard_EXPORT TopoDS_Shape ReconstructFace(int theFaceDefIdx) const;

  //! Reconstruct a TopoDS_Shape from a usage node.
  Standard_EXPORT TopoDS_Shape ReconstructFromUsage(BRepGraph_UsageId theRoot) const;

  //! Return mutable edge definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::EdgeDef& MutableEdgeDefinition(int theIdx);

  //! Return mutable wire definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::WireDef& MutableWireDefinition(int theIdx);

  //! Return mutable vertex definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::VertexDef& MutableVertexDefinition(int theIdx);

  //! Return mutable face definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::FaceDef& MutableFaceDefinition(int theIdx);

  //! Return mutable shell definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::ShellDef& MutableShellDefinition(int theIdx);

  //! Return mutable solid definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::SolidDef& MutableSolidDefinition(int theIdx);

  //! Return mutable compound definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::CompoundDef& MutableCompoundDefinition(int theIdx);

  //! Return mutable compsolid definition (marks node as modified).
  Standard_EXPORT BRepGraph_TopoNode::CompSolidDef& MutableCompSolidDefinition(int theIdx);

  //! Attach a PCurve to an edge for a given face context.
  //! @return NodeId of the newly created PCurve node
  Standard_EXPORT BRepGraph_NodeId AddPCurveToEdge(
    BRepGraph_NodeId            theEdgeDef,
    BRepGraph_NodeId            theFaceDef,
    const Handle(Geom2d_Curve)& theCurve2d,
    double                      theFirst,
    double                      theLast,
    TopAbs_Orientation          theEdgeOrientation = TopAbs_FORWARD);

  //! Replace an edge in a wire definition.
  Standard_EXPORT void ReplaceEdgeInWire(int              theWireDefIdx,
                                         BRepGraph_NodeId theOldEdgeDef,
                                         BRepGraph_NodeId theNewEdgeDef,
                                         bool             theReversed);

  //! Split a single edge definition at a vertex and 3D-curve parameter.
  //! Creates two new EdgeDef slots, splits all PCurve nodes at the corresponding
  //! 2D parameter, and updates every wire that contained the original edge
  //! (inserting the two sub-edges in traversal order).
  //! The original EdgeDef index remains valid but is no longer referenced by any wire.
  //! @param[in]  theEdgeDef      edge to split (must not be degenerate)
  //! @param[in]  theSplitVertex  vertex definition at the split point (already in graph)
  //! @param[in]  theSplitParam   parameter on the 3D curve at the split point
  //! @param[out] theSubA    sub-edge: StartVertex -> SplitVertex, [First, SplitParam]
  //! @param[out] theSubB    sub-edge: SplitVertex -> EndVertex,   [SplitParam, Last]
  Standard_EXPORT void SplitEdge(BRepGraph_NodeId  theEdgeDef,
                                  BRepGraph_NodeId  theSplitVertex,
                                  double            theSplitParam,
                                  BRepGraph_NodeId& theSubA,
                                  BRepGraph_NodeId& theSubB);

  //! Number of distinct faces referencing a given edge definition.
  Standard_EXPORT int FaceCountForEdge(int theEdgeDefIdx) const;

  //! Return all wire definition indices that contain a given edge.
  Standard_EXPORT const NCollection_Vector<int>& WiresOfEdge(int theEdgeDefIdx) const;

  //! Record a history entry mapping an original node to its replacements.
  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&                theOpLabel,
                                     BRepGraph_NodeId                              theOriginal,
                                     const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  //! Add a vertex definition to the graph.
  //! @param[in] thePoint     3D coordinates
  //! @param[in] theTolerance vertex tolerance
  //! @return NodeId of the new vertex definition
  Standard_EXPORT BRepGraph_NodeId AddVertexDef(const gp_Pnt& thePoint,
                                                double        theTolerance);

  //! Add an edge definition to the graph.
  //! @param[in] theStartVtx  start vertex def NodeId
  //! @param[in] theEndVtx    end vertex def NodeId
  //! @param[in] theCurve     3D curve (may be null for degenerate edges)
  //! @param[in] theFirst     first curve parameter
  //! @param[in] theLast      last curve parameter
  //! @param[in] theTolerance edge tolerance
  //! @return NodeId of the new edge definition
  Standard_EXPORT BRepGraph_NodeId AddEdgeDef(BRepGraph_NodeId          theStartVtx,
                                              BRepGraph_NodeId          theEndVtx,
                                              const Handle(Geom_Curve)& theCurve,
                                              double                    theFirst,
                                              double                    theLast,
                                              double                    theTolerance);

  //! Add a wire definition to the graph.
  //! @param[in] theEdges ordered edge entries
  //! @return NodeId of the new wire definition
  Standard_EXPORT BRepGraph_NodeId AddWireDef(
    const NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry>& theEdges);

  //! Add a face definition to the graph.
  //! @param[in] theSurface    surface geometry
  //! @param[in] theOuterWire  outer wire def NodeId
  //! @param[in] theInnerWires inner wire def NodeIds
  //! @param[in] theTolerance  face tolerance
  //! @return NodeId of the new face definition
  Standard_EXPORT BRepGraph_NodeId AddFaceDef(const Handle(Geom_Surface)&              theSurface,
                                              BRepGraph_NodeId                         theOuterWire,
                                              const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
                                              double                                   theTolerance);

  //! Add an empty shell definition to the graph.
  //! @return NodeId of the new shell definition
  Standard_EXPORT BRepGraph_NodeId AddShellDef();

  //! Add an empty solid definition to the graph.
  //! @return NodeId of the new solid definition
  Standard_EXPORT BRepGraph_NodeId AddSolidDef();

  //! Link a face to a shell (creates FaceUsage under the ShellUsage).
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theFaceDef   face definition NodeId
  //! @param[in] theOri       orientation of the face in the shell
  //! @return UsageId of the created FaceUsage
  Standard_EXPORT BRepGraph_UsageId AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                                   BRepGraph_NodeId   theFaceDef,
                                                   TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Link a shell to a solid (creates ShellUsage under the SolidUsage).
  //! @param[in] theSolidDef  solid definition NodeId
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theOri       orientation of the shell in the solid
  //! @return UsageId of the created ShellUsage
  Standard_EXPORT BRepGraph_UsageId AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                                    BRepGraph_NodeId   theShellDef,
                                                    TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Add a compound definition with child definitions.
  //! @param[in] theChildDefs child definition NodeIds
  //! @return NodeId of the new compound definition
  Standard_EXPORT BRepGraph_NodeId AddCompoundDef(
    const NCollection_Vector<BRepGraph_NodeId>& theChildDefs);

  //! Add a compsolid definition with child solid definitions.
  //! @param[in] theSolidDefs child solid definition NodeIds
  //! @return NodeId of the new compsolid definition
  Standard_EXPORT BRepGraph_NodeId AddCompSolidDef(
    const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs);

  //! Append a shape to the existing graph without clearing.
  //! Uses existing deduplication maps to avoid re-registering shared entities.
  //! @param[in] theShape   shape to add
  //! @param[in] theParallel if true, per-face geometry extraction is parallel
  Standard_EXPORT void AppendShape(const TopoDS_Shape& theShape,
                                   bool                theParallel = false);

  //! Mark a node as removed (soft deletion).
  //! The node remains in storage but is skipped by iterators and queries.
  //! @param[in] theNode node to remove
  Standard_EXPORT void RemoveNode(BRepGraph_NodeId theNode);

  //! Mark a node and all its descendants as removed (cascading soft deletion).
  //! Walks the definition/usage hierarchy and marks all children as removed.
  //! @param[in] theNode root node to remove
  Standard_EXPORT void RemoveSubgraph(BRepGraph_NodeId theNode);

  //! Check if a node has been soft-removed.
  //! @param[in] theNode node to check
  //! @return true if the node was marked as removed
  Standard_EXPORT bool IsRemoved(BRepGraph_NodeId theNode) const;

  //! Replace the internal allocator and re-create all storage with new allocator.
  Standard_EXPORT void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc);

  //! Return the current allocator.
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;

  //! Enable or disable history recording.
  Standard_EXPORT void SetHistoryEnabled(bool theVal);

  //! Check if history recording is enabled.
  Standard_EXPORT bool IsHistoryEnabled() const;

public:
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  // -- Grouped View API (inner classes defined in separate headers) --
  class DefsView;
  class UsagesView;
  class GeomView;
  class UIDsView;
  class RelEdgesView;
  class SpatialView;
  class CacheView;
  class AttrsView;
  class ShapesView;
  class MutView;
  class BuilderView;
  class AnalyzeView;

  //! Access topology definitions. Include BRepGraph_DefsView.hxx to use.
  DefsView     Defs()     const;
  //! Access topology usages. Include BRepGraph_UsagesView.hxx to use.
  UsagesView   Usages()   const;
  //! Access geometry nodes and links. Include BRepGraph_GeomView.hxx to use.
  GeomView     Geom()     const;
  //! Access unique identifiers. Include BRepGraph_UIDsView.hxx to use.
  UIDsView     UIDs()     const;
  //! Access relation edges. Include BRepGraph_RelEdgesView.hxx to use.
  RelEdgesView RelEdges() const;
  //! Access spatial and adjacency queries. Include BRepGraph_SpatialView.hxx to use.
  SpatialView  Spatial()  const;
  //! Access cached spatial properties. Include BRepGraph_CacheView.hxx to use.
  CacheView    Cache()    const;
  //! Access user attributes. Include BRepGraph_AttrsView.hxx to use.
  AttrsView    Attrs();
  //! Access shape reconstruction. Include BRepGraph_ShapesView.hxx to use.
  ShapesView   Shapes()   const;
  //! Access mutable definitions and mutation operations. Include BRepGraph_MutView.hxx to use.
  MutView      Mut();
  //! Access programmatic graph construction. Include BRepGraph_BuilderView.hxx to use.
  BuilderView  Builder();
  //! Access analysis queries. Include BRepGraph_AnalyzeView.hxx to use.
  AnalyzeView  Analyze()  const;

  //! Access history subsystem directly.
  Standard_EXPORT BRepGraph_History&       History();
  //! Access history subsystem directly (const).
  Standard_EXPORT const BRepGraph_History& History() const;

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_History;
  friend class BRepGraph_Analyze;
  friend class BRepGraph_Reconstruct;
  friend class BRepGraph_Mutator;

  std::unique_ptr<BRepGraph_Data> myData;

  //! Internal build helpers.
  BRepGraph_NodeId registerSurface(const Handle(Geom_Surface)&       theSurf,
                                   const Handle(Poly_Triangulation)& theTri);
  BRepGraph_NodeId registerCurve(const Handle(Geom_Curve)& theCrv);
  BRepGraph_NodeId createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                    BRepGraph_NodeId            theEdgeDef,
                                    BRepGraph_NodeId            theFaceDef,
                                    double                      theFirst,
                                    double                      theLast);

  void collectVertexPoints(BRepGraph_NodeId theNode, Bnd_Box& theBox) const;
  void invalidateSubgraphImpl(BRepGraph_NodeId theNode);
  BRepGraph_UID allocateUID(BRepGraph_NodeId theNodeId);
  BRepGraph_NodeCache* mutableCache(BRepGraph_NodeId theNode);
  void markModified(BRepGraph_NodeId theDefId);

  //! Dispatch a callback on the def vector matching theNode.Kind.
  //! Defined in BRepGraph.cxx (only used internally).
  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc) const
    -> decltype(theFunc(std::declval<const NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0));

  //! Non-const overload of dispatchDef for mutable access.
  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc)
    -> decltype(theFunc(std::declval<NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0));
};

#endif // _BRepGraph_HeaderFile
