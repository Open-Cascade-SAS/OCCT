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

#ifndef _BRepGraphCheck_HeaderFile
#define _BRepGraphCheck_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraphCheck_Issue.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>

//! Static check functions for BRepGraph-based shape validation.
//!
//! Each function validates a specific aspect of graph topology or geometry
//! and appends any issues found to the caller-provided issue vector.
//! Functions are organized by topology type in separate .cxx files.
//!
//! These functions are thread-safe: they take a const BRepGraph reference
//! and write only to the caller's issue vector.
class BRepGraphCheck
{
public:
  DEFINE_STANDARD_ALLOC

  // ---- Vertex checks (BRepGraphCheck_Vertex.cxx) ----

  //! Check that vertex point lies on the edge's 3D curve within tolerance.
  //! @param[in] theGraph  pre-built graph
  //! @param[in] theVertex typed vertex definition identifier
  //! @param[in] theEdge   typed edge definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckVertexOnEdge(
    const BRepGraph&                          theGraph,
    const BRepGraph_VertexId                  theVertex,
    const BRepGraph_EdgeId                    theEdge,
    NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! Check that vertex point lies on the face's surface within tolerance.
  //! Uses PCurve evaluation for the edge carrying the vertex.
  //! @param[in] theGraph  pre-built graph
  //! @param[in] theVertex typed vertex definition identifier
  //! @param[in] theFace   typed face definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckVertexOnFace(
    const BRepGraph&                          theGraph,
    const BRepGraph_VertexId                  theVertex,
    const BRepGraph_FaceId                    theFace,
    NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  // ---- Edge checks (BRepGraphCheck_Edge.cxx) ----

  //! Minimum edge checks: 3D curve presence, flag consistency, parameter range.
  //! @param[in] theGraph  pre-built graph
  //! @param[in] theEdge   typed edge definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckEdgeMinimum(const BRepGraph&                          theGraph,
                                               const BRepGraph_EdgeId                    theEdge,
                                               NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! Check edge on a specific face: PCurve existence and SameParameter deviation.
  //! @param[in] theGraph   pre-built graph
  //! @param[in] theEdge    typed edge definition identifier
  //! @param[in] theFace    typed face definition identifier
  //! @param[in] theIsExact use exact method for BRepLib_ValidateEdge
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckEdgeOnFace(const BRepGraph&                          theGraph,
                                              const BRepGraph_EdgeId                    theEdge,
                                              const BRepGraph_FaceId                    theFace,
                                              const bool                                theIsExact,
                                              NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! Check edge connectivity within a shell: free edges and multi-connexity.
  //! @param[in] theGraph         pre-built graph
  //! @param[in] theEdge          typed edge definition identifier
  //! @param[in] theShell         typed shell definition identifier (context)
  //! @param[in] theEdgeFaceCount number of faces in this shell referencing the edge
  //! @param[out] theIssues       issues appended here
  Standard_EXPORT static void CheckEdgeInShell(const BRepGraph&        theGraph,
                                               const BRepGraph_EdgeId  theEdge,
                                               const BRepGraph_ShellId theShell,
                                               const int               theEdgeFaceCount,
                                               NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  // ---- Wire checks (BRepGraphCheck_Wire.cxx) ----

  //! Minimum wire checks: emptiness and edge redundancy.
  //! @param[in] theGraph pre-built graph
  //! @param[in] theWire  typed wire definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckWireMinimum(const BRepGraph&                          theGraph,
                                               const BRepGraph_WireId                    theWire,
                                               NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! Check wire on a face: connectivity, closure, and orientation.
  //! @param[in] theGraph        pre-built graph
  //! @param[in] theWire         typed wire definition identifier
  //! @param[in] theFace         typed face definition identifier
  //! @param[in] theGeomControls perform geometric checks (self-intersection)
  //! @param[out] theIssues      issues appended here
  Standard_EXPORT static void CheckWireOnFace(const BRepGraph&       theGraph,
                                              const BRepGraph_WireId theWire,
                                              const BRepGraph_FaceId theFace,
                                              const bool             theGeomControls,
                                              NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  // ---- Face checks (BRepGraphCheck_Face.cxx) ----

  //! Minimum face check: surface presence.
  //! @param[in] theGraph pre-built graph
  //! @param[in] theFace  typed face definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckFaceMinimum(const BRepGraph&                          theGraph,
                                               const BRepGraph_FaceId                    theFace,
                                               NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! Check face wires: intersections, imbrication, orientation consistency.
  //! @param[in] theGraph        pre-built graph
  //! @param[in] theFace         typed face definition identifier
  //! @param[in] theGeomControls perform geometric checks
  //! @param[out] theIssues      issues appended here
  Standard_EXPORT static void CheckFaceWires(const BRepGraph&       theGraph,
                                             const BRepGraph_FaceId theFace,
                                             const bool             theGeomControls,
                                             NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  // ---- Shell checks (BRepGraphCheck_Shell.cxx) ----

  //! Minimum shell check: non-empty.
  //! @param[in] theGraph pre-built graph
  //! @param[in] theShell typed shell definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckShellMinimum(
    const BRepGraph&                          theGraph,
    const BRepGraph_ShellId                   theShell,
    NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! Check shell closure: each edge shared by exactly 2 faces.
  //! @param[in] theGraph pre-built graph
  //! @param[in] theShell typed shell definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckShellClosed(const BRepGraph&                          theGraph,
                                               const BRepGraph_ShellId                   theShell,
                                               NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  //! Check shell orientation: consistent edge orientation between adjacent faces.
  //! @param[in] theGraph pre-built graph
  //! @param[in] theShell typed shell definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckShellOrientation(
    const BRepGraph&                          theGraph,
    const BRepGraph_ShellId                   theShell,
    NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

  // ---- Solid checks (BRepGraphCheck_Solid.cxx) ----

  //! Minimum solid check: shell imbrication and subshape orientation.
  //! @param[in] theGraph pre-built graph
  //! @param[in] theSolid typed solid definition identifier
  //! @param[out] theIssues issues appended here
  Standard_EXPORT static void CheckSolidMinimum(
    const BRepGraph&                          theGraph,
    const BRepGraph_SolidId                   theSolid,
    NCollection_Vector<BRepGraphCheck_Issue>& theIssues);

private:
  BRepGraphCheck() = delete;
};

#endif // _BRepGraphCheck_HeaderFile
