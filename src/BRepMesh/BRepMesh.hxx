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

#ifndef _BRepMesh_HeaderFile
#define _BRepMesh_HeaderFile

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
#include <TopoDS_Shape.hxx>

#include <vector>

class BRepMesh_Vertex;
class TopoDS_Edge;
class TopoDS_Vertex;
class Handle_BRepMesh_FaceAttribute;
class BRepMesh_VertexInspector;
class BRepMesh_CircleInspector;
class BRepMesh_Classifier;
class Poly_Triangulation;
class BRepMesh_VertexTool;

namespace BRepMesh
{
  //! Default size for memory block allocated by IncAllocator.
  const size_t MEMORY_BLOCK_SIZE_HUGE = 512 * 1024;

  //! Structure keeping parameters of segment.
  struct Segment
  {
    gp_XY StartPnt;
    gp_XY EndPnt;
  };

  //! Sequences
  typedef NCollection_Sequence<Bnd_B2d>                                                             SequenceOfBndB2d;
  typedef NCollection_Sequence<Standard_Integer>                                                    SequenceOfInteger;
  typedef NCollection_Sequence<Standard_Real>                                                       SequenceOfReal;

  //! Vectors
  typedef NCollection_Vector<BRepMesh_Vertex>                                                       VectorOfVertex;
  typedef NCollection_Vector<Standard_Integer>                                                      VectorOfInteger;
  typedef NCollection_Vector<BRepMesh_Circle>                                                       VectorOfCircle;

  //! Trees
  typedef NCollection_EBTree<Standard_Integer, Bnd_Box2d>                                           BndBox2dTree;
  typedef NCollection_UBTreeFiller<Standard_Integer, Bnd_Box2d>                                     BndBox2dTreeFiller;

  //! Arrays
  typedef NCollection_Array1<BRepMesh_Vertex>                                                       Array1OfVertexOfDelaun;
  typedef NCollection_Array1<Standard_Integer>                                                      Array1OfInteger;
  typedef NCollection_Array1<Standard_Real>                                                         Array1OfReal;
  typedef NCollection_Array1<Segment>                                                               Array1OfSegments;

  //! Lists
  typedef NCollection_List<gp_XY>                                                                   ListOfXY;
  typedef NCollection_List<BRepMesh_Vertex>                                                         ListOfVertex;
  typedef NCollection_List<Standard_Integer>                                                        ListOfInteger;

  //! Maps
  typedef NCollection_Map<Standard_Integer>                                                         MapOfInteger;
  typedef NCollection_DataMap<Handle(Poly_Triangulation), Standard_Boolean>                         DMapOfTriangulationBool;
  typedef NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                                    MapOfShape;

  typedef NCollection_DataMap<Standard_Integer, Standard_Integer>                                   MapOfIntegerInteger;
  typedef NCollection_DataMap<TopoDS_Vertex, Standard_Integer, TopTools_ShapeMapHasher>             DMapOfVertexInteger;
  typedef NCollection_DataMap<TopoDS_Face, Handle_BRepMesh_FaceAttribute, TopTools_ShapeMapHasher>  DMapOfFaceAttribute;
  typedef NCollection_DataMap<TopoDS_Shape, BRepMesh_PairOfPolygon, TopTools_ShapeMapHasher>        DMapOfShapePairOfPolygon;
  typedef NCollection_DataMap<Standard_Integer, gp_Pnt>                                             DMapOfIntegerPnt;
  typedef NCollection_DataMap<Standard_Integer, ListOfXY>                                           DMapOfIntegerListOfXY;
  typedef NCollection_DataMap<Standard_Integer, ListOfInteger>                                      DMapOfIntegerListOfInteger;
  typedef NCollection_DataMap<TopoDS_Edge, DMapOfTriangulationBool, TopTools_ShapeMapHasher>        DMapOfEdgeListOfTriangulationBool;

  typedef NCollection_IndexedMap<Standard_Integer>                                                  IMapOfInteger;
  typedef NCollection_IndexedMap<Standard_Real>                                                     IMapOfReal;
  typedef NCollection_IndexedMap<BRepMesh_Triangle>                                                 IMapOfElement;
  typedef NCollection_IndexedDataMap<BRepMesh_Edge, BRepMesh_PairOfIndex>                           IDMapOfLink;

  //! CellFilters
  typedef NCollection_CellFilter<BRepMesh_CircleInspector>                                          CircleCellFilter;
  typedef NCollection_CellFilter<BRepMesh_VertexInspector>                                          VertexCellFilter;

  //! Handles
  typedef NCollection_Handle<VectorOfVertex>                                                        HVectorOfVertex;
  typedef NCollection_Handle<MapOfInteger>                                                          HMapOfInteger;
  typedef NCollection_Handle<IMapOfInteger>                                                         HIMapOfInteger;
  typedef NCollection_Handle<DMapOfShapePairOfPolygon>                                              HDMapOfShapePairOfPolygon;
  typedef NCollection_Handle<DMapOfIntegerPnt>                                                      HDMapOfIntegerPnt;
  typedef NCollection_Handle<BRepMesh_Classifier>                                                   HClassifier;
  typedef NCollection_Handle<BndBox2dTree>                                                          HBndBox2dTree;
  typedef NCollection_Handle<Array1OfSegments>                                                      HArray1OfSegments;
  typedef NCollection_Handle<DMapOfVertexInteger>                                                   HDMapOfVertexInteger;
  typedef NCollection_Handle<DMapOfIntegerListOfXY>                                                 HDMapOfIntegerListOfXY;
  typedef NCollection_Handle<BRepMesh_VertexTool>                                                   HVertexTool;
  typedef NCollection_Handle<SequenceOfBndB2d>                                                      HSequenceOfBndB2d;
  typedef NCollection_Handle<SequenceOfInteger>                                                     HSequenceOfInteger;

  //! Other data structures
  typedef std::pair<HArray1OfSegments, HBndBox2dTree>                                               SegmentsTree;
  typedef NCollection_Array1<SegmentsTree>                                                          Array1OfSegmentsTree;
};

#endif
