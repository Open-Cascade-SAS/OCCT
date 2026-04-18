// Created on: 2016-04-07
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

#ifndef _IMeshData_Types_HeaderFile
#define _IMeshData_Types_HeaderFile

#include <NCollection_Sequence.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <NCollection_OccAllocator.hxx>
#include <IMeshData_ParametersListArrayAdaptor.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <NCollection_EBTree.hxx>
#include <Bnd_Box2d.hxx>
#include <NCollection_CellFilter.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_IndexedMap.hxx>
#include <BRepMesh_Vertex.hxx>
#include <Bnd_B2.hxx>
#include <BRepMesh_Circle.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_PairOfIndex.hxx>
#include <BRepMesh_Edge.hxx>

#include <memory>
#include <queue>

class IMeshData_Shape;
class IMeshData_Face;
class IMeshData_Wire;
class IMeshData_Edge;
class IMeshData_Curve;
class IMeshData_PCurve;
class IMeshData_Model;
class BRepMesh_VertexInspector;
class BRepMesh_CircleInspector;

#define DEFINE_INC_ALLOC                                                                           \
  DEFINE_NCOLLECTION_ALLOC                                                                         \
  void operator delete(void* /*theAddress*/)                                                       \
  {                                                                                                \
    /*it's inc allocator, nothing to do*/                                                          \
  }

namespace IMeshData
{
//! Default size for memory block allocated by IncAllocator.
/**
 * The idea here is that blocks of the given size are returned to the system
 * rather than retained in the malloc heap, at least on WIN32 and WIN64 platforms.
 */
#ifdef _WIN64
const size_t MEMORY_BLOCK_SIZE_HUGE = 1024 * 1024;
#else
const size_t MEMORY_BLOCK_SIZE_HUGE = 512 * 1024;
#endif

using IEdgePtr = IMeshData_Edge*;
using IFacePtr = IMeshData_Face*;

using IEdgeHandle = occ::handle<IMeshData_Edge>;
using IWireHandle = occ::handle<IMeshData_Wire>;
using IFaceHandle = occ::handle<IMeshData_Face>;
using ICurveHandle = occ::handle<IMeshData_Curve>;
using IPCurveHandle = occ::handle<IMeshData_PCurve>;

using ICurveArrayAdaptor = IMeshData_ParametersListArrayAdaptor<ICurveHandle>;
using ICurveArrayAdaptorHandle = occ::handle<ICurveArrayAdaptor>;

using BndBox2dTree = NCollection_Shared<NCollection_EBTree<int, Bnd_Box2d>>;
using BndBox2dTreeFiller = NCollection_UBTreeFiller<int, Bnd_Box2d>;

// Vectors
using VectorOfIFaceHandles = NCollection_Shared<NCollection_Vector<IFaceHandle>>;
using VectorOfIWireHandles = NCollection_Shared<NCollection_Vector<IWireHandle>>;
using VectorOfIEdgeHandles = NCollection_Shared<NCollection_Vector<IEdgeHandle>>;
using VectorOfIPCurveHandles = NCollection_Shared<NCollection_Vector<IPCurveHandle>>;
using VectorOfIEdgePtrs = NCollection_Shared<NCollection_Vector<IEdgePtr>>;
using VectorOfBoolean = NCollection_Shared<NCollection_Vector<bool>>;
using VectorOfInteger = NCollection_Shared<NCollection_Vector<int>>;
using VectorOfOrientation = NCollection_Shared<NCollection_Vector<TopAbs_Orientation>>;
using VectorOfElements = NCollection_Shared<NCollection_Vector<BRepMesh_Triangle>>;
using VectorOfCircle = NCollection_Shared<NCollection_Vector<BRepMesh_Circle>>;

using Array1OfVertexOfDelaun = NCollection_Shared<NCollection_Array1<BRepMesh_Vertex>>;
using VectorOfVertex = NCollection_Shared<NCollection_Vector<BRepMesh_Vertex>>;

// Sequences
using SequenceOfBndB2d = NCollection_Shared<NCollection_Sequence<Bnd_B2d>>;
using SequenceOfInteger = NCollection_Shared<NCollection_Sequence<int>>;
using SequenceOfReal = NCollection_Shared<NCollection_Sequence<double>>;

namespace Model
{
using SequenceOfPnt = std::deque<gp_Pnt, NCollection_OccAllocator<gp_Pnt>>;
using SequenceOfPnt2d = std::deque<gp_Pnt2d, NCollection_OccAllocator<gp_Pnt2d>>;
using SequenceOfReal = std::deque<double, NCollection_OccAllocator<double>>;
using SequenceOfInteger = std::deque<int, NCollection_OccAllocator<int>>;
} // namespace Model

// Lists
using ListOfInteger = NCollection_Shared<NCollection_List<int>>;
using ListOfPnt2d = NCollection_Shared<NCollection_List<gp_Pnt2d>>;
using ListOfIPCurves = NCollection_Shared<NCollection_List<IPCurveHandle>>;

using MapOfInteger = NCollection_Shared<TColStd_PackedMapOfInteger>;
using IteratorOfMapOfInteger = TColStd_PackedMapOfInteger::Iterator;

using CircleCellFilter = NCollection_CellFilter<BRepMesh_CircleInspector>;
using VertexCellFilter = NCollection_CellFilter<BRepMesh_VertexInspector>;

using DMapOfShapeInteger = NCollection_Shared<NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>>;
using DMapOfIFacePtrsListOfInteger = NCollection_Shared<NCollection_DataMap<IFacePtr, ListOfInteger>>;
using MapOfIEdgePtr = NCollection_Shared<NCollection_Map<IEdgePtr>>;
using MapOfIFacePtr = NCollection_Shared<NCollection_Map<IFacePtr>>;
using MapOfOrientedEdges = NCollection_Shared<NCollection_Map<BRepMesh_OrientedEdge>>;
using MapOfReal = NCollection_Shared<NCollection_Map<double>>;
using IDMapOfIFacePtrsListOfIPCurves = NCollection_Shared<NCollection_IndexedDataMap<IFacePtr, ListOfIPCurves>>;
using DMapOfIFacePtrsMapOfIEdgePtrs = NCollection_Shared<NCollection_DataMap<IFacePtr, occ::handle<MapOfIEdgePtr>>>;
using IDMapOfLink = NCollection_Shared<NCollection_IndexedDataMap<BRepMesh_Edge, BRepMesh_PairOfIndex>>;
using DMapOfIntegerListOfInteger = NCollection_Shared<NCollection_DataMap<int, ListOfInteger>>;
using MapOfIntegerInteger = NCollection_Shared<NCollection_DataMap<int, bool>>;
using IMapOfReal = NCollection_Shared<NCollection_IndexedMap<double>>;

using Array1OfInteger = NCollection_Shared<NCollection_Array1<int>>;
} // namespace IMeshData

#endif
