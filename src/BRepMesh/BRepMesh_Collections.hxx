// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_Collections_HeaderFile
#define _BRepMesh_Collections_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <gp_XY.hxx>
#include <Bnd_B2d.hxx>
#include <Bnd_Box2d.hxx>
#include <Standard.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_CellFilter.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_EBTree.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_PairOfPolygon.hxx>
#include <BRepMesh_PairOfIndex.hxx>
#include <BRepMesh_Circle.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <Handle_Poly_Triangulation.hxx>
#include <TopoDS_Face.hxx>

#include <vector>

class BRepMesh_Vertex;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Vertex;
class Handle_BRepMesh_FaceAttribute;
class BRepMesh_VertexInspector;
class BRepMesh_CircleInspector;
class BRepMesh_Classifier;
class Poly_Triangulation;

//! Short names for collections
#define N_SEQUENCE    NCollection_Sequence
#define N_VECTOR      NCollection_Vector
#define N_ARRAY1      NCollection_Array1
#define N_LIST        NCollection_List
#define N_MAP         NCollection_Map
#define N_HANDLE      NCollection_Handle
#define N_DATAMAP     NCollection_DataMap
#define N_IMAP        NCollection_IndexedMap
#define N_IDMAP       NCollection_IndexedDataMap
#define N_CELLFILTER  NCollection_CellFilter

namespace BRepMeshCol
{
  //! Structure keeping parameters of segment.
  struct Segment
  {
    gp_XY StartPnt;
    gp_XY EndPnt;
  };

  //! Memory allocators
  typedef Handle(NCollection_IncAllocator)                                      Allocator;

  //! Short names for hashers
  typedef TopTools_ShapeMapHasher                                               ShapeMapHasher;

  //! Sequences
  typedef N_SEQUENCE<Bnd_B2d>                                                   SequenceOfBndB2d;
  typedef N_SEQUENCE<Standard_Integer>                                          SequenceOfInteger;
  typedef N_SEQUENCE<Standard_Real>                                             SequenceOfReal;

  //! Vectors
  typedef N_VECTOR<BRepMesh_Vertex>                                             VectorOfVertex;
  typedef N_VECTOR<Standard_Integer>                                            VectorOfInteger;
  typedef N_VECTOR<BRepMesh_Circle>                                             VectorOfCircle;

  //! Trees
  typedef NCollection_EBTree<Standard_Integer, Bnd_Box2d>                       BndBox2dTree;
  typedef NCollection_UBTreeFiller<Standard_Integer, Bnd_Box2d>                 BndBox2dTreeFiller;

  //! Arrays
  typedef N_ARRAY1<BRepMesh_Vertex>                                             Array1OfVertexOfDelaun;
  typedef N_ARRAY1<Standard_Integer>                                            Array1OfInteger;
  typedef N_ARRAY1<Standard_Real>                                               Array1OfReal;
  typedef std::vector<Segment>                                                  Array1OfSegments;

  //! Lists
  typedef N_LIST<gp_XY>                                                         ListOfXY;
  typedef N_LIST<BRepMesh_Vertex>                                               ListOfVertex;
  typedef N_LIST<Standard_Integer>                                              ListOfInteger;

  //! Maps
  typedef N_MAP<Standard_Integer>                                               MapOfInteger;
  typedef N_MAP<Handle(Poly_Triangulation)>                                     MapOfTriangulation;
  typedef N_MAP<TopoDS_Shape, ShapeMapHasher>                                   MapOfShape;

  typedef N_DATAMAP<Standard_Integer, Standard_Integer>                         MapOfIntegerInteger;
  typedef N_DATAMAP<TopoDS_Vertex, Standard_Integer, ShapeMapHasher>            DMapOfVertexInteger;
  typedef N_DATAMAP<TopoDS_Face, Handle_BRepMesh_FaceAttribute, ShapeMapHasher> DMapOfFaceAttribute;
  typedef N_DATAMAP<TopoDS_Shape, BRepMesh_PairOfPolygon, ShapeMapHasher>       DMapOfShapePairOfPolygon;
  typedef N_DATAMAP<Standard_Integer, gp_Pnt>                                   DMapOfIntegerPnt;
  typedef N_DATAMAP<Standard_Integer, ListOfXY>                                 DMapOfIntegerListOfXY;
  typedef N_DATAMAP<Standard_Integer, ListOfInteger>                            DMapOfIntegerListOfInteger;
  typedef N_DATAMAP<TopoDS_Edge, MapOfTriangulation, ShapeMapHasher>            DMapOfEdgeListOfTriangulation;

  typedef N_IMAP<Standard_Integer>                                              IMapOfInteger;
  typedef N_IMAP<Standard_Real>                                                 IMapOfReal;
  typedef N_IMAP<BRepMesh_Triangle>                                             IMapOfElement;
  typedef N_IDMAP<BRepMesh_Edge, BRepMesh_PairOfIndex>                          IDMapOfLink;

  //! CellFilters
  typedef N_CELLFILTER<BRepMesh_CircleInspector>                                CircleCellFilter;
  typedef N_CELLFILTER<BRepMesh_VertexInspector>                                VertexCellFilter;

  //! Handles
  typedef N_HANDLE<MapOfInteger>                                                HMapOfInteger;
  typedef N_HANDLE<IMapOfInteger>                                               HIMapOfInteger;
  typedef N_HANDLE<DMapOfShapePairOfPolygon>                                    HDMapOfShapePairOfPolygon;
  typedef N_HANDLE<DMapOfIntegerPnt>                                            HDMapOfIntegerPnt;
  typedef N_HANDLE<BRepMesh_Classifier>                                         HClassifier;
  typedef N_HANDLE<BndBox2dTree>                                                HBndBox2dTree;
  typedef N_HANDLE<Array1OfSegments>                                            HArray1OfSegments;

  //! Other data structures
  typedef std::pair<HArray1OfSegments, HBndBox2dTree>                           SegmentsTree;
  typedef std::vector<SegmentsTree>                                             Array1OfSegmentsTree;
};

#endif
