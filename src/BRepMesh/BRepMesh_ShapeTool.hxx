// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_ShapeTool_HeaderFile
#define _BRepMesh_ShapeTool_HeaderFile

#include <Standard_Transient.hxx>
#include <IMeshData_Types.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>

class Geom_Curve;
class Geom2d_Curve;
class Poly_Polygon3D;
class TopoDS_Face;
class TopoDS_Edge;
class Bnd_Box;

//! Auxiliary class providing functionality to compute,
//! retrieve and store data to TopoDS and model shape.
class BRepMesh_ShapeTool : public Standard_Transient
{
public:

  //! Returns maximum tolerance of the given face.
  //! Considers tolerances of edges and vertices contained in the given face.
  Standard_EXPORT static Standard_Real MaxFaceTolerance(
    const TopoDS_Face& theFace);

  //! Gets the maximum dimension of the given bounding box.
  //! If the given bounding box is void leaves the resulting value unchanged.
  //! @param theBox bounding box to be processed.
  //! @param theMaxDimension maximum dimension of the given box.
  Standard_EXPORT static void BoxMaxDimension(const Bnd_Box& theBox,
                                              Standard_Real& theMaxDimension);

  //! Checks same parameter, same range and degenerativity attributes
  //! using geometrical data of the given edge and updates edge model
  //! by computed parameters in case of worst case - it can drop flags
  //! same parameter and same range to False but never to True if it is 
  //! already set to False. In contrary, it can also drop degenerated 
  //! flag to True, but never to False if it is already set to True.
  Standard_EXPORT static void CheckAndUpdateFlags (
    const IMeshData::IEdgeHandle&   theEdge,
    const IMeshData::IPCurveHandle& thePCurve);

  //! Stores the given triangulation into the given face.
  //! @param theFace face to be updated by triangulation.
  //! @param theTriangulation triangulation to be stored into the face.
  Standard_EXPORT static void AddInFace(
    const TopoDS_Face&          theFace,
    Handle(Poly_Triangulation)& theTriangulation);

  //! Nullifies triangulation stored in the face.
  //! @param theFace face to be updated by null triangulation.
  Standard_EXPORT static void NullifyFace (const TopoDS_Face& theFace);

  //! Nullifies polygon on triangulation stored in the edge.
  //! @param theEdge edge to be updated by null polygon.
  //! @param theTriangulation triangulation the given edge is associated to.
  //! @param theLocation face location.
  Standard_EXPORT static void NullifyEdge (
    const TopoDS_Edge&                 theEdge,
    const Handle (Poly_Triangulation)& theTriangulation,
    const TopLoc_Location&             theLocation);

  //! Nullifies 3d polygon stored in the edge.
  //! @param theEdge edge to be updated by null polygon.
  //! @param theLocation face location.
  Standard_EXPORT static void NullifyEdge (
    const TopoDS_Edge&     theEdge,
    const TopLoc_Location& theLocation);

  //! Updates the given edge by the given tessellated representation.
  //! @param theEdge edge to be updated.
  //! @param thePolygon tessellated representation of the edge to be stored.
  //! @param theTriangulation triangulation the given edge is associated to.
  //! @param theLocation face location.
  Standard_EXPORT static void UpdateEdge (
    const TopoDS_Edge&                          theEdge,
    const Handle (Poly_PolygonOnTriangulation)& thePolygon,
    const Handle (Poly_Triangulation)&          theTriangulation,
    const TopLoc_Location&                      theLocation);

  //! Updates the given edge by the given tessellated representation.
  //! @param theEdge edge to be updated.
  //! @param thePolygon tessellated representation of the edge to be stored.
  Standard_EXPORT static void UpdateEdge(
    const TopoDS_Edge&            theEdge,
    const Handle(Poly_Polygon3D)& thePolygon);

  //! Updates the given seam edge by the given tessellated representations.
  //! @param theEdge edge to be updated.
  //! @param thePolygon1 tessellated representation corresponding to
  //! forward direction of the seam edge.
  //! @param thePolygon2 tessellated representation corresponding to
  //! reversed direction of the seam edge.
  //! @param theTriangulation triangulation the given edge is associated to.
  //! @param theLocation face location.
  Standard_EXPORT static void UpdateEdge (
    const TopoDS_Edge&                          theEdge,
    const Handle (Poly_PolygonOnTriangulation)& thePolygon1,
    const Handle (Poly_PolygonOnTriangulation)& thePolygon2,
    const Handle (Poly_Triangulation)&          theTriangulation,
    const TopLoc_Location&                      theLocation);

  //! Applies location to the given point and return result.
  //! @param thePnt point to be transformed.
  //! @param theLoc location to be applied.
  Standard_EXPORT static gp_Pnt UseLocation (
    const gp_Pnt&          thePnt,
    const TopLoc_Location& theLoc);

  //! Gets the strict UV locations of the extremities of the edge using pcurve.
  Standard_EXPORT static Standard_Boolean UVPoints (
    const TopoDS_Edge&      theEdge,
    const TopoDS_Face&      theFace,
    gp_Pnt2d&               theFirstPoint2d,
    gp_Pnt2d&               theLastPoint2d,
    const Standard_Boolean  isConsiderOrientation = Standard_False);

  //! Gets the parametric range of the given edge on the given face.
  Standard_EXPORT static Standard_Boolean Range (
    const TopoDS_Edge&      theEdge,
    const TopoDS_Face&      theFace,
    Handle (Geom2d_Curve)&  thePCurve,
    Standard_Real&          theFirstParam,
    Standard_Real&          theLastParam,
    const Standard_Boolean  isConsiderOrientation = Standard_False);

  //! Gets the 3d range of the given edge.
  Standard_EXPORT static Standard_Boolean Range (
    const TopoDS_Edge&      theEdge,
    Handle (Geom_Curve)&    theCurve,
    Standard_Real&          theFirstParam,
    Standard_Real&          theLastParam,
    const Standard_Boolean  isConsiderOrientation = Standard_False);

  DEFINE_STANDARD_RTTIEXT(BRepMesh_ShapeTool, Standard_Transient)
};

#endif