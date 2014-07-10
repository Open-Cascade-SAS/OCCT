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

#ifndef _BRepMesh_FastDiscret_HeaderFile
#define _BRepMesh_FastDiscret_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <BRepMesh_Status.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MutexForShapeProvider.hxx>
#include <Standard_Transient.hxx>
#include <Handle_BRepAdaptor_HSurface.hxx>
#include <Handle_Geom2d_Curve.hxx>
#include <BRepMesh_Delaun.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_FaceAttribute.hxx>
#include <BRepMesh_Collections.hxx>

class BRepMesh_DataStructureOfDelaun;
class Bnd_Box;
class TopoDS_Shape;
class TopoDS_Face;
class TopTools_IndexedDataMapOfShapeListOfShape;
class TopoDS_Edge;
class BRepAdaptor_HSurface;
class Geom2d_Curve;
class TopoDS_Vertex;
class BRepMesh_Edge;
class BRepMesh_Vertex;
class gp_Pnt;
class BRepMesh_FaceAttribute;
class TopTools_DataMapOfShapeReal;


//! Algorithm to mesh a shape with respect of the <br>
//! frontier the deflection and by option the shared <br>
//! components. <br>
class BRepMesh_FastDiscret : public Standard_Transient
{
public:
  
  Standard_EXPORT BRepMesh_FastDiscret(const Standard_Real defle,
                                       const Standard_Real angle,
                                       const Bnd_Box& B,
                                       const Standard_Boolean withShare = Standard_True,
                                       const Standard_Boolean inshape = Standard_False,
                                       const Standard_Boolean relative = Standard_False,
                                       const Standard_Boolean shapetrigu = Standard_False,
                                       const Standard_Boolean isInParallel = Standard_False);

  //! if the boolean <relative> is True, the <br>
  //! deflection used for the polygonalisation of <br>
  //! each edge will be <defle> * Size of Edge. <br>
  //! the deflection used for the faces will be the maximum <br>
  //! deflection of their edges. <br>
  //! <br>
  //! if <shapetrigu> is True, the triangulation, if exists <br>
  //! with a correct deflection, can be used to re-triangulate <br>
  //! the shape. <br>
  //! <br>
  //! if <inshape> is True, the calculated <br>
  //! triangulation will be stored in the shape. <br>
  Standard_EXPORT BRepMesh_FastDiscret(const TopoDS_Shape& shape,
                                       const Standard_Real defle,
                                       const Standard_Real angle,
                                       const Bnd_Box& B,
                                       const Standard_Boolean withShare = Standard_True,
                                       const Standard_Boolean inshape = Standard_False,
                                       const Standard_Boolean relative = Standard_False,
                                       const Standard_Boolean shapetrigu = Standard_False,
                                       const Standard_Boolean isInParallel = Standard_False);

  //! Build triangulation on the whole shape <br>
  Standard_EXPORT void Perform(const TopoDS_Shape& shape);

  //! Record a face for further processing. <br>
  Standard_EXPORT void Add(const TopoDS_Face& face,
                           const TopTools_IndexedDataMapOfShapeListOfShape& ancestor) ;

  //! Triangulate a face previously recorded for <br>
  //! processing by call to Add(). Can be executed in <br>
  //! parallel threads. <br>
  Standard_EXPORT void Process(const TopoDS_Face& face) const;

  void operator ()(const TopoDS_Face& face) const
  {
    Process(face);
  }
  
  Standard_EXPORT BRepMesh_Status CurrentFaceStatus() const;
  
  //! Request algorithm to launch in multiple threads <br>
  //! to improve performance (should be supported by plugin). <br>
  Standard_EXPORT void SetParallel(const Standard_Boolean theInParallel);
  
  //! Returns the multi-threading usage flag. <br>
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Creates mutexes for each sub-shape of type theType in theShape. <br>
  //! Used to avoid data races. <br>
  Standard_EXPORT void CreateMutexesForSubShapes(const TopoDS_Shape& theShape,
                                                 const TopAbs_ShapeEnum theType);
  
  //! Removes all created mutexes <br>
  Standard_EXPORT void RemoveAllMutexes();

  //! Gives the number of built triangles. <br>
  Standard_EXPORT Standard_Integer NbTriangles() const;

  //! Gives the triangle of <Index>. <br>
  Standard_EXPORT const BRepMesh_Triangle& Triangle(const Standard_Integer Index) const;

  //! Gives the number of built Edges <br>
  Standard_EXPORT Standard_Integer NbEdges() const;

  //! Gives the edge of index <Index>. <br>
  Standard_EXPORT const BRepMesh_Edge& Edge(const Standard_Integer Index) const;

  //! Gives the number of built Vertices. <br>
  Standard_EXPORT Standard_Integer NbVertices() const;

  //! Gives the vertex of <Index>. <br>
  Standard_EXPORT const BRepMesh_Vertex& Vertex(const Standard_Integer Index) const;

  //! Gives the nodes of triangle with the given index.
  Standard_EXPORT void TriangleNodes(const Standard_Integer theIndex,
                                     Standard_Integer       (&theNodes)[3]) const
  {
    myStructure->ElementNodes(Triangle(theIndex), theNodes);
  }

  //! Gives the location3d of the vertex of <Index>. <br>
  Standard_EXPORT const gp_Pnt& Pnt(const Standard_Integer Index) const;

  //! Gives the list of indices of the vertices <br>
  Standard_EXPORT void VerticesOfDomain(BRepMeshCol::MapOfInteger& Indices) const;

  //! Gives the list of indices of the edges <br>
  inline void EdgesOfDomain(BRepMeshCol::MapOfInteger& Indices) const
  { 
    Indices = myStructure->LinksOfDomain();
  }

  //! Gives the list of indices of the triangles <br>
  inline void TrianglesOfDomain(BRepMeshCol::MapOfInteger& Indices) const
  { 
    Indices = myStructure->ElementsOfDomain();
  }

  //! Gives the number of different location in 3d space.  
  //! It is different of the number of vertices if there 
  //! is more than one surface. <br>
  //! Even for one surface, the number can be different
  //! if an edge is shared. <br>
  inline Standard_Integer NbPoint3d() const
  {
    return myNbLocat;
  }

  //! Gives the 3d space location of the vertex <Index>. <br>
  inline const gp_Pnt& Point3d(const Standard_Integer Index) const
  {
    return myLocation3d(Index);
  }

  //! returns the deflection value. <br>
  inline Standard_Real GetDeflection() const
  {
    return myDeflection;
  }

  //! returns the deflection value. <br>
  inline Standard_Real GetAngle() const
  {
    return myAngle;
  }
  
  inline Standard_Boolean WithShare() const
  {
    return myWithShare;
  }
  
  inline Standard_Boolean InShape() const
  {
    return myInshape;
  }
  
  inline Standard_Boolean ShapeTrigu() const
  {
    return myShapetrigu;
  }

  //! returns the face deflection value. <br>
  Standard_EXPORT Standard_Boolean GetFaceAttribute(const TopoDS_Face& face,Handle(BRepMesh_FaceAttribute)& fattrib) const;

  //! remove face attribute as useless to free locate memory <br>
  Standard_EXPORT void RemoveFaceAttribute(const TopoDS_Face& face);
  
  inline const TopTools_DataMapOfShapeReal& GetMapOfDefEdge() const
  {
    return myMapdefle;
  }


  DEFINE_STANDARD_RTTI(BRepMesh_FastDiscret)

private: 
  
  void Add(const TopoDS_Edge& edge,
           const TopoDS_Face& face,
           const Handle(BRepAdaptor_HSurface)& S,
           const Handle(Geom2d_Curve)& C,
           const TopTools_IndexedDataMapOfShapeListOfShape& ancestor,
           const Standard_Real defedge,
           const Standard_Real first,
           const Standard_Real last);
  
  void Add(const TopoDS_Vertex& theVert,
           const TopoDS_Face& face,
           const Handle(BRepAdaptor_HSurface)& S);
  
  Standard_Boolean Update(const TopoDS_Edge& Edge,
                          const TopoDS_Face& Face,
                          const Handle(Geom2d_Curve)& C,
                          const Standard_Real defedge,
                          const Standard_Real first,
                          const Standard_Real last);
  
  void InternalVertices(const Handle(BRepAdaptor_HSurface)& caro,
                        BRepMeshCol::ListOfVertex& inter,
                        const Standard_Real defedge,
                        const BRepMeshCol::HClassifier& classifier);
  
  Standard_Real Control(const Handle(BRepAdaptor_HSurface)& caro,
                        const Standard_Real defface,
                        BRepMeshCol::ListOfVertex& inter,
                        BRepMeshCol::ListOfInteger& badTri,
                        BRepMeshCol::ListOfInteger& nulTri,
                        BRepMesh_Delaun& trigu,
                        const Standard_Boolean isfirst);
  
  void AddInShape(const TopoDS_Face& face,
                  const Standard_Real defedge);

private:

  Standard_Real                             myAngle;
  Standard_Real                             myDeflection;
  Standard_Real                             myDtotale;
  Standard_Boolean                          myWithShare;
  Standard_Boolean                          myInParallel;
  BRepMeshCol::DMapOfVertexInteger          myVertices;
  BRepMeshCol::DMapOfShapePairOfPolygon     myEdges;
  BRepMeshCol::DMapOfShapePairOfPolygon     myInternaledges;
  Standard_Integer                          myNbLocat;
  BRepMeshCol::DMapOfIntegerPnt             myLocation3d;
  Handle_BRepMesh_DataStructureOfDelaun     myStructure;
  BRepMeshCol::DMapOfFaceAttribute          myMapattrib;
  TColStd_IndexedMapOfInteger               myVemap;
  BRepMeshCol::DMapOfIntegerListOfXY        myLocation2d;
  Standard_Boolean                          myRelative;
  Standard_Boolean                          myShapetrigu;
  Standard_Boolean                          myInshape;
  BRepMesh_Status                           myFacestate;
  TopTools_DataMapOfShapeReal               myMapdefle;
  TopTools_ListOfShape                      myNottriangulated;
  BRepMeshCol::Allocator                    myAllocator;
  TopTools_MutexForShapeProvider            myMutexProvider;
};

DEFINE_STANDARD_HANDLE(BRepMesh_FastDiscret, Standard_Transient)

#endif
