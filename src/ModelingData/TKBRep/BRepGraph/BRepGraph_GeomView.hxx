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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_PCurveContext.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
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

  //! Return the PCurve NodeId for a given PCurve context.
  //! Convenience overload that unpacks the context into (EdgeDef, FaceDef, Orientation)
  //! and delegates to the 3-argument PCurveOf.
  //! @param[in] theContext  composite key identifying edge, face and orientation
  Standard_EXPORT BRepGraph_NodeId PCurveOf(const BRepGraph_PCurveContext& theContext) const;

  //! Return pointer to the surface geometry for a face definition,
  //! or nullptr if the face has no valid surface or the handle is null.
  //! @param[in] theFaceDefIdx zero-based face definition index
  Standard_EXPORT const BRepGraph_GeomNode::Surf* FaceSurface(int theFaceDefIdx) const;

  //! Return pointer to the PCurve geometry for an edge on a face,
  //! or nullptr if no PCurve exists or Curve2d is null.
  //! @param[in] theEdgeDef edge definition NodeId
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT const BRepGraph_GeomNode::PCurve* EdgePCurve(
    BRepGraph_NodeId theEdgeDef, BRepGraph_NodeId theFaceDef) const;

  //! Overload with explicit orientation for seam edges.
  //! @param[in] theEdgeDef edge definition NodeId
  //! @param[in] theFaceDef face definition NodeId
  //! @param[in] theEdgeOri edge orientation on the face
  Standard_EXPORT const BRepGraph_GeomNode::PCurve* EdgePCurve(
    BRepGraph_NodeId   theEdgeDef,
    BRepGraph_NodeId   theFaceDef,
    TopAbs_Orientation theEdgeOri) const;

  //! Build a GeomAdaptor_TransformedCurve for an edge definition.
  //! Uses the edge's 3D curve if available; falls back to pcurve-on-surface.
  //! The transform comes from the edge's first usage GlobalLocation.
  //! @param[in] theEdgeDef edge definition NodeId
  //! @return curve adaptor ready for evaluation, sampling, or projection
  Standard_EXPORT GeomAdaptor_TransformedCurve CurveAdaptor(BRepGraph_NodeId theEdgeDef) const;

  //! Build a GeomAdaptor_TransformedCurve for a specific edge usage.
  //! Uses the usage's GlobalLocation as the transform.
  //! @param[in] theEdgeDef   edge definition NodeId
  //! @param[in] theEdgeUsage edge usage id (determines the transform)
  //! @return curve adaptor ready for evaluation
  Standard_EXPORT GeomAdaptor_TransformedCurve CurveAdaptor(BRepGraph_NodeId  theEdgeDef,
                                                            BRepGraph_UsageId theEdgeUsage) const;

  //! Build a curve-on-surface adaptor from edge's PCurve on a face.
  //! Looks up the PCurve node via PCurveOf(), creates Geom2dAdaptor_Curve
  //! from PCurve.Curve2d, creates GeomAdaptor_Surface from Surf.Surface,
  //! and combines into Adaptor3d_CurveOnSurface.
  //! @param[in] theEdgeDef edge definition NodeId
  //! @param[in] theFaceDef face definition NodeId
  //! @return curve-on-surface adaptor, or null handle if data is missing
  Standard_EXPORT Handle(Adaptor3d_CurveOnSurface)
    CurveOnSurfaceAdaptor(BRepGraph_NodeId theEdgeDef,
                          BRepGraph_NodeId theFaceDef) const;

  //! Overload with explicit edge orientation for seam edges.
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theEdgeOrientation   edge orientation on the face (FORWARD or REVERSED)
  //! @return curve-on-surface adaptor, or null handle if data is missing
  Standard_EXPORT Handle(Adaptor3d_CurveOnSurface)
    CurveOnSurfaceAdaptor(BRepGraph_NodeId   theEdgeDef,
                          BRepGraph_NodeId   theFaceDef,
                          TopAbs_Orientation theEdgeOrientation) const;

  //! Build a GeomAdaptor_TransformedSurface for a face definition.
  //! Uses the face's Geom_Surface directly with the surface location as transform,
  //! avoiding TopoDS_Face reconstruction.
  //! @param[in] theFaceDef face definition NodeId
  //! @return surface adaptor ready for evaluation, or empty adaptor if surface is null
  Standard_EXPORT GeomAdaptor_TransformedSurface SurfaceAdaptor(BRepGraph_NodeId theFaceDef) const;

  //! Build a GeomAdaptor_TransformedSurface with explicit UV bounds for a face definition.
  //! @param[in] theFaceDef face definition NodeId
  //! @param[in] theUFirst  minimum U parameter
  //! @param[in] theULast   maximum U parameter
  //! @param[in] theVFirst  minimum V parameter
  //! @param[in] theVLast   maximum V parameter
  //! @return surface adaptor with bounds, or empty adaptor if surface is null
  Standard_EXPORT GeomAdaptor_TransformedSurface SurfaceAdaptor(BRepGraph_NodeId theFaceDef,
                                                                double           theUFirst,
                                                                double           theULast,
                                                                double           theVFirst,
                                                                double           theVLast) const;

private:
  friend class BRepGraph;
  explicit GeomView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::GeomView BRepGraph::Geom() const { return GeomView(this); }

#endif // _BRepGraph_GeomView_HeaderFile
