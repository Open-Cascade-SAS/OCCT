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
#include <BRepMesh_Collections.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_MutexForShapeProvider.hxx>
#include <Handle_BRepAdaptor_HSurface.hxx>
#include <Handle_Poly_Triangulation.hxx>
#include <BRepMesh_Delaun.hxx>
#include <BRepMesh_Triangle.hxx>

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
//! the deflection and by option the shared components. <br>
class BRepMesh_FastDiscretFace : public Standard_Transient 
{
public:

  
  Standard_EXPORT BRepMesh_FastDiscretFace(const Standard_Real theAngle,
                                           const Standard_Boolean theWithShare = Standard_True);
  
  Standard_EXPORT void Add(const TopoDS_Face& theFace,
                           const Handle(BRepMesh_FaceAttribute)& theAttrib,
                           const TopTools_DataMapOfShapeReal& theMapDefle,
                           const TopTools_MutexForShapeProvider& theMutexProvider);
  
  Standard_EXPORT Standard_Real Control(const Handle(BRepAdaptor_HSurface)& theCaro,
                                        const Standard_Real theDefFace,
                                        BRepMeshCol::ListOfVertex& theInternalV,
                                        BRepMeshCol::ListOfInteger& theBadTriangles,
                                        BRepMeshCol::ListOfInteger& theNulTriangles,
                                        BRepMesh_Delaun& theTrigu,
                                        const Standard_Boolean theIsFirst);

  //! Gives the triangle of <Index>. <br>
  Standard_EXPORT const BRepMesh_Triangle& Triangle(const Standard_Integer theIndex) const;

  //! Gives the edge of index <Index>. <br>
  Standard_EXPORT const BRepMesh_Edge& Edge(const Standard_Integer theIndex) const;

  //! Gives the vertex of <Index>. <br>
  Standard_EXPORT const BRepMesh_Vertex& Vertex(const Standard_Integer theIndex) const;

  //! Gives the location3d of the vertex of <Index>. <br>
  Standard_EXPORT const gp_Pnt& Pnt(const Standard_Integer theIndex) const;

  DEFINE_STANDARD_RTTI(BRepMesh_FastDiscretFace)

protected:
 
  Standard_Boolean RestoreStructureFromTriangulation(const TopoDS_Edge& theEdge,
                                                     const TopoDS_Face& theFace,
                                                     const Handle(BRepAdaptor_HSurface)& theSurf,
                                                     const Handle(Poly_Triangulation)& theTrigu,
                                                     const Standard_Real theDefEdge,
                                                     const TopLoc_Location& theLoc,
                                                     const TopTools_MutexForShapeProvider& theMutexProvider);

private: 
  
  void Add(const TopoDS_Vertex& theVert,
           const TopoDS_Face& theFace,
           const Handle(BRepAdaptor_HSurface)& theSFace);
  
  void InternalVertices(const Handle(BRepAdaptor_HSurface)& theCaro,
                        BRepMeshCol::ListOfVertex& theInternalV,
                        const Standard_Real theDefFace,
                        const BRepMeshCol::HClassifier& theClassifier);
  
  void AddInShape(const TopoDS_Face& theFace,
                  const Standard_Real theDefFace,
                  const TopTools_MutexForShapeProvider& theMutexProvider);

private:
  Standard_Real                             myAngle;
  Standard_Boolean                          myWithShare;
  BRepMeshCol::DMapOfVertexInteger          myVertices;
  BRepMeshCol::DMapOfShapePairOfPolygon     myInternaledges;
  Standard_Integer                          myNbLocat;
  BRepMeshCol::DMapOfIntegerPnt             myLocation3d;
  Handle_BRepMesh_DataStructureOfDelaun     myStructure;
  BRepMeshCol::ListOfVertex                 myListver;
  BRepMeshCol::IMapOfInteger                myVemap;
  BRepMeshCol::DMapOfIntegerListOfXY        myLocation2d;
  Handle_BRepMesh_FaceAttribute             myAttrib;
  Standard_Boolean                          myInternalVerticesMode;
  BRepMeshCol::IMapOfReal                   myUParam;
  BRepMeshCol::IMapOfReal                   myVParam;
  BRepMeshCol::Allocator                    myAllocator;
};

DEFINE_STANDARD_HANDLE (BRepMesh_FastDiscretFace, Standard_Transient)

#endif
