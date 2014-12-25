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

#ifndef _BRepMesh_FastDiscretFace_HeaderFile
#define _BRepMesh_FastDiscretFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <BRepMesh_FastDiscretFace.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_MutexForShapeProvider.hxx>
#include <Handle_BRepAdaptor_HSurface.hxx>
#include <Handle_Poly_Triangulation.hxx>
#include <BRepMesh_Delaun.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Classifier.hxx>
#include <ElSLib.hxx>

class BRepMesh_DataStructureOfDelaun;
class BRepMesh_FaceAttribute;
class TopoDS_Face;
class TopTools_DataMapOfShapeReal;
class TopoDS_Vertex;
class BRepAdaptor_HSurface;
class TopoDS_Edge;
class Poly_Triangulation;
class TopLoc_Location;
class gp_XY;
class gp_Pnt2d;
class BRepMesh_Edge;
class BRepMesh_Vertex;
class gp_Pnt;

//! Algorithm to mesh a face with respect of the frontier 
//! the deflection and by option the shared components.
class BRepMesh_FastDiscretFace : public Standard_Transient 
{
public:
  
  //! Constructor.
  //! @param theAngle deviation angle to be used for surface tessellation.
  //! @param isInternalVerticesMode flag enabling/disabling internal 
  //! vertices mode.
  //! @param isControlSurfaceDeflection enables/disables adaptive 
  //! reconfiguration of mesh.
  Standard_EXPORT BRepMesh_FastDiscretFace(
    const Standard_Real    theAngle,
    const Standard_Real    theMinSize,
    const Standard_Boolean isInternalVerticesMode,
    const Standard_Boolean isControlSurfaceDeflection);

  Standard_EXPORT void Perform(const Handle(BRepMesh_FaceAttribute)& theAttribute);

  DEFINE_STANDARD_RTTI(BRepMesh_FastDiscretFace)

private:

  void add(const Handle(BRepMesh_FaceAttribute)& theAttribute);
  void add(const TopoDS_Vertex& theVertex);

  Standard_Real control(BRepMesh::ListOfVertex&  theNewVertices,
                        BRepMesh_Delaun&         theMeshBuilder,
                        const Standard_Boolean   theIsFirst);

  //! Registers the given nodes in mesh data structure and
  //! performs refinement of existing mesh.
  //! @param theVertices nodes to be inserted.
  //! @param theMeshBuilder initialized tool refining mesh 
  //! in respect to inserting nodes.
  //! @return TRUE if vertices were been inserted, FALSE elewhere.
  Standard_Boolean addVerticesToMesh(
    const BRepMesh::ListOfVertex& theVertices,
    BRepMesh_Delaun&              theMeshBuilder);

  //! Calculates nodes lying on face's surface and inserts them to a mesh.
  //! @param theNewVertices list of vertices to be extended and added to mesh.
  //! @param theMeshBuilder initialized tool refining mesh 
  //! in respect to inserting nodes.
  void insertInternalVertices(BRepMesh::ListOfVertex&  theNewVertices,
                              BRepMesh_Delaun&         theMeshBuilder);

  //! Calculates nodes lying on spherical surface.
  //! @param theNewVertices list of vertices to be extended and added to mesh.
  void insertInternalVerticesSphere(BRepMesh::ListOfVertex& theNewVertices);

  //! Calculates nodes lying on cylindrical surface.
  //! @param theNewVertices list of vertices to be extended and added to mesh.
  void insertInternalVerticesCylinder(BRepMesh::ListOfVertex& theNewVertices);

  //! Calculates nodes lying on conical surface.
  //! @param theNewVertices list of vertices to be extended and added to mesh.
  void insertInternalVerticesCone(BRepMesh::ListOfVertex& theNewVertices);

  //! Calculates nodes lying on toroidal surface.
  //! @param theNewVertices list of vertices to be extended and added to mesh.
  void insertInternalVerticesTorus(BRepMesh::ListOfVertex& theNewVertices);

  //! Calculates nodes lying on Bezier/BSpline surface.
  //! @param theNewVertices list of vertices to be extended and added to mesh.
  void insertInternalVerticesBSpline(BRepMesh::ListOfVertex& theNewVertices);

  //! Calculates nodes lying on custom-type surface.
  //! @param theNewVertices list of vertices to be extended and added to mesh.
  void insertInternalVerticesOther(BRepMesh::ListOfVertex& theNewVertices);
  
  //! Template method trying to insert new internal vertex corresponded to
  //! the given 2d point. Calculates 3d position analytically using the given
  //! surface.
  //! @param thePnt2d 2d point to be inserted to the list.
  //! @param theAnalyticSurface analytic surface to calculate 3d point.
  //! @param[out] theVertices list of vertices to be updated.
  template<class AnalyticSurface>
  void tryToInsertAnalyticVertex(const gp_Pnt2d&         thePnt2d,
                                 const AnalyticSurface&  theAnalyticSurface,
                                 BRepMesh::ListOfVertex& theVertices)
  {
    const BRepMesh::HClassifier& aClassifier = myAttribute->ChangeClassifier();
    if (aClassifier->Perform(thePnt2d) != TopAbs_IN)
      return;

    gp_Pnt aPnt;
    ElSLib::D0(thePnt2d.X(), thePnt2d.Y(), theAnalyticSurface, aPnt);
    insertVertex(aPnt, thePnt2d.Coord(), theVertices);
  }

  //! Creates new vertex with the given parameters.
  //! @param thePnt3d 3d point corresponded to the vertex.
  //! @param theUV UV point corresponded to the vertex.
  //! @param[out] theVertices list of vertices to be updated.
  void insertVertex(const gp_Pnt&           thePnt3d,
                    const gp_XY&            theUV,
                    BRepMesh::ListOfVertex& theVertices);

  //! Stores mesh into the face (without internal edges).
  void commitSurfaceTriangulation();

  //! Performs initialization of data structure using existing data.
  void initDataStructure();

  //! Adds new link to the mesh data structure.
  //! Movability of the link and order of nodes depend on orientation parameter.
  void addLinkToMesh(const Standard_Integer   theFirstNodeId,
                     const Standard_Integer   theLastNodeId,
                     const TopAbs_Orientation theOrientation);

  //! Inserts new node into a mesh in case if smoothed region build 
  //! using the given node has better deflection metrics than source state.
  //! @param thePnt3d 3d point corresponded to the vertex.
  //! @param theUV UV point corresponded to the vertex.
  //! @param isDeflectionCheckOnly if TRUE new node will not be added to a mesh
  //! even if deflection parameter is better.
  //! @param theTriangleDeflection deflection of a triangle from real geometry.
  //! @param theFaceDeflection deflection to be achieved.
  //! @param theCircleTool tool used for fast extraction of triangles 
  //! touched by the given point.
  //! @param[out] theVertices list of vertices to be updated.
  //! @param[in out] theMaxTriangleDeflection maximal deflection of a mesh.
  //! @return TRUE in case if the given deflection of triangle is fine and
  //! there is no necessity to insert new node or new node was being inserted
  //! successfully, FALSE in case if new configuration is better but 
  //! isDeflectionCheckOnly flag is set.
  Standard_Boolean checkDeflectionAndInsert(
    const gp_Pnt&              thePnt3d,
    const gp_XY&               theUV,
    const Standard_Boolean     isDeflectionCheckOnly,
    const Standard_Real        theTriangleDeflection,
    const Standard_Real        theFaceDeflection,
    const BRepMesh_CircleTool& theCircleTool,
    BRepMesh::ListOfVertex&    theVertices,
    Standard_Real&             theMaxTriangleDeflection);

private:

  Standard_Real                          myAngle;
  Standard_Boolean                       myInternalVerticesMode;
  BRepMesh::IMapOfReal                   myUParam;
  BRepMesh::IMapOfReal                   myVParam;

  // Fast access to attributes of current face
  Handle(BRepMesh_FaceAttribute)         myAttribute;
  Handle(BRepMesh_DataStructureOfDelaun) myStructure;

  Standard_Real                          myMinSize;
  Standard_Boolean                       myIsControlSurfaceDeflection;
};

DEFINE_STANDARD_HANDLE (BRepMesh_FastDiscretFace, Standard_Transient)

#endif
