// Copyright (c) 2013 OPEN CASCADE SAS
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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepMesh.hxx>

class Poly_Triangulation;
class TopoDS_Face;
class TopoDS_Edge;
class Bnd_Box;
class TopoDS_Vertex;
class gp_XY;
class gp_Pnt2d;

class BRepMesh_ShapeTool
{
public:

  DEFINE_STANDARD_ALLOC
  
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

  //! Returns relative deflection for edge with respect to shape size.
  //! @param theEdge edge for which relative deflection should be computed.
  //! @param theDeflection absolute deflection.
  //! @param theMaxShapeSize maximum size of a shape.
  //! @param theAdjustmentCoefficient coefficient of adjustment between maximum 
  //! size of shape and calculated relative deflection.
  //! @return relative deflection for the edge.
  Standard_EXPORT static Standard_Real RelativeEdgeDeflection(
    const TopoDS_Edge&  theEdge,
    const Standard_Real theDeflection,
    const Standard_Real theMaxShapeSize,
    Standard_Real&      theAdjustmentCoefficient);

  //! Checks 2d representations of 3d point with the 
  //! given index for equality to avoid duplications.
  //! @param theIndexOfPnt3d index of 3d point with which 2d 
  //! representation should be associated.
  //! @param thePnt2d 2d representation of the point with the 
  //! given index.
  //! @param theMinDistance minimum distance between vertices 
  //! regarding which they could be treated as distinct ones.
  //! @param theFaceAttribute attributes contining data calculated
  //! according to face geomtry and define limits of face in parametric 
  //! space. If defined, will be used instead of surface parameter.
  //! @param theLocation2dMap map of 2d representations of 3d points.
  //! @return given 2d point in case if 3d poind does not alredy have 
  //! the similar representation, otherwice 2d point corresponding to 
  //! existing representation will be returned.
  Standard_EXPORT static gp_XY FindUV(
    const Standard_Integer                theIndexOfPnt3d,
    const gp_Pnt2d&                       thePnt2d,
    const Standard_Real                   theMinDistance,
    const Handle(BRepMesh_FaceAttribute)& theFaceAttribute);

  //! Stores the given triangulation into the given face.
  //! @param theFace face to be updated by triangulation.
  //! @param theTriangulation triangulation to be stored into the face.
  Standard_EXPORT static void AddInFace(
    const TopoDS_Face&          theFace,
    Handle(Poly_Triangulation)& theTriangulation);

  //! Nullifies triangulation stored in the face.
  //! @param theFace face to be updated by null triangulation.
  Standard_EXPORT static void NullifyFace(const TopoDS_Face& theFace);

  //! Nullifies polygon on triangulation stored in the edge.
  //! @param theEdge edge to be updated by null polygon.
  //! @param theTriangulation triangulation the given edge is associated to.
  //! @param theLocation face location.
  Standard_EXPORT static void NullifyEdge(
    const TopoDS_Edge&                theEdge,
    const Handle(Poly_Triangulation)& theTriangulation,
    const TopLoc_Location&            theLocation);

  //! Updates the given edge by the given tessellated representation.
  //! @param theEdge edge to be updated.
  //! @param thePolygon tessellated representation of the edge to be stored.
  //! @param theTriangulation triangulation the given edge is associated to.
  //! @param theLocation face location.
  Standard_EXPORT static void UpdateEdge(
    const TopoDS_Edge&                         theEdge,
    const Handle(Poly_PolygonOnTriangulation)& thePolygon,
    const Handle(Poly_Triangulation)&          theTriangulation,
    const TopLoc_Location&                     theLocation);

  //! Updates the given seam edge by the given tessellated representations.
  //! @param theEdge edge to be updated.
  //! @param thePolygon1 tessellated representation corresponding to
  //! forward direction of the seam edge.
  //! @param thePolygon2 tessellated representation corresponding to
  //! reversed direction of the seam edge.
  //! @param theTriangulation triangulation the given edge is associated to.
  //! @param theLocation face location.
  Standard_EXPORT static void UpdateEdge(
    const TopoDS_Edge&                         theEdge,
    const Handle(Poly_PolygonOnTriangulation)& thePolygon1,
    const Handle(Poly_PolygonOnTriangulation)& thePolygon2,
    const Handle(Poly_Triangulation)&          theTriangulation,
    const TopLoc_Location&                     theLocation);

  //! Applies location to the given point and return result.
  //! @param thePnt point to be transformed.
  //! @param theLoc location to be applied.
  Standard_EXPORT static gp_Pnt UseLocation(const gp_Pnt&          thePnt,
                                            const TopLoc_Location& theLoc);

  //! Checks is the given edge degenerated.
  //! Checks geometrical parameters in case if IsDegenerated flag is not set.
  //! @param theEdge edge to be checked.
  //! @param theFace face within which parametric space edge will be checked
  //! for geometrical degenerativity.
  Standard_EXPORT static Standard_Boolean IsDegenerated(
    const TopoDS_Edge& theEdge,
    const TopoDS_Face& theFace);
};

#endif
