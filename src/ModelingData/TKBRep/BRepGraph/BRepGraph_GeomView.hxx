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

#ifndef _BRepGraph_GeomView_HeaderFile
#define _BRepGraph_GeomView_HeaderFile

#include <BRepGraph.hxx>
#include <TopAbs_Orientation.hxx>

//! Lightweight const view over geometry nodes of a BRepGraph.
//! Obtained via BRepGraph::Geom().
class BRepGraph::GeomView
{
public:
  //! Number of surface geometry nodes.
  Standard_EXPORT int NbSurfaces() const;

  //! Number of 3D curve geometry nodes.
  Standard_EXPORT int NbCurves() const;

  //! Number of 2D PCurve geometry nodes.
  Standard_EXPORT int NbPCurves() const;

  //! Access surface geometry node by index.
  //! @param[in] theIdx zero-based surface index
  Standard_EXPORT const BRepGraph_GeomNode::Surf& Surface(int theIdx) const;

  //! Access 3D curve geometry node by index.
  //! @param[in] theIdx zero-based curve index
  Standard_EXPORT const BRepGraph_GeomNode::Curve& Curve(int theIdx) const;

  //! Access PCurve geometry node by index.
  //! @param[in] theIdx zero-based PCurve index
  Standard_EXPORT const BRepGraph_GeomNode::PCurve& PCurve(int theIdx) const;

  //! Return the surface definition NodeId for a face definition.
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT BRepGraph_NodeId SurfaceOf(BRepGraph_NodeId theFaceDef) const;

  //! Return all face definitions that reference a given surface.
  //! @param[in] theSurf surface NodeId
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& FacesOnSurface(
    BRepGraph_NodeId theSurf) const;

  //! Return the 3D curve definition NodeId for an edge definition.
  //! @param[in] theEdgeDef edge definition NodeId
  Standard_EXPORT BRepGraph_NodeId CurveOf(BRepGraph_NodeId theEdgeDef) const;

  //! Return all edge definitions that reference a given 3D curve.
  //! @param[in] theCurve curve NodeId
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& EdgesOnCurve(
    BRepGraph_NodeId theCurve) const;

  //! Return the PCurve NodeId for an edge on a given face.
  //! @param[in] theEdgeDef edge definition NodeId
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId theEdgeDef,
                                            BRepGraph_NodeId theFaceDef) const;

  //! Return the PCurve NodeId for an edge/face/orientation triple.
  //! For seam edges two PCurve nodes share the same FaceDefId but differ in orientation;
  //! use this overload to distinguish FORWARD (C1) from REVERSED (C2).
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theEdgeOrientation   edge orientation on the face
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId   theEdgeDef,
                                            BRepGraph_NodeId   theFaceDef,
                                            TopAbs_Orientation theEdgeOrientation) const;

private:
  friend class BRepGraph;
  explicit GeomView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::GeomView BRepGraph::Geom() const { return GeomView(this); }

#endif // _BRepGraph_GeomView_HeaderFile
