// Created on: 1993-05-12
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepMesh_Delaun.hxx>

#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <Precision.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_B2.hxx>

#include <BRepMesh_SelectorOfDataStructureOfDelaun.hxx>

#include <BRepMesh_Edge.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_Triangle.hxx>

#include <NCollection_Vector.hxx>

#include <algorithm>
#include <stack>
#include <vector>

const double AngDeviation1Deg  = M_PI / 180.;
const double AngDeviation90Deg = 90 * AngDeviation1Deg;
const double Angle2PI          = 2 * M_PI;

const double Precision  = Precision::PConfusion();
const double Precision2 = Precision * Precision;

namespace
{
//! Sort two points in projection on vector (1,1)
struct ComparatorOfVertexOfDelaun
{
  bool operator()(const BRepMesh_Vertex& theLeft, const BRepMesh_Vertex& theRight)
  {
    return theLeft.Coord().X() + theLeft.Coord().Y() < theRight.Coord().X() + theRight.Coord().Y();
  }
};

//! Sort two points in projection on vector (1,1)
struct ComparatorOfIndexedVertexOfDelaun
{
public:
  ComparatorOfIndexedVertexOfDelaun(const occ::handle<BRepMesh_DataStructureOfDelaun>& theDS)
      : myStructure(theDS)
  {
  }

  bool operator()(int theLeft, int theRight)
  {
    const BRepMesh_Vertex& aLeft  = myStructure->GetNode(theLeft);
    const BRepMesh_Vertex& aRight = myStructure->GetNode(theRight);
    return ComparatorOfVertexOfDelaun()(aLeft, aRight);
  }

private:
  occ::handle<BRepMesh_DataStructureOfDelaun> myStructure;
};

void UpdateBndBox(const gp_XY& thePnt1, const gp_XY& thePnt2, Bnd_B2d& theBox)
{
  theBox.Add(thePnt1);
  theBox.Add(thePnt2);
  theBox.Enlarge(Precision);
}

// Class representing a stack of frames. Each frame is a range of elements to be processed.
// The stack allows to process elements in depth-first order meaning that when new elements
// are added to the stack, they will be processed before the remaining elements of the
// current frame.
// Frames are processed in LIFO order while elements inside a frame are processed in FIFO order.
class StackOfFrames
{
private:
  // A frame is a range of elements to be processed.
  class Frame
  {
  public:
    // Construct a frame for the given range of elements.
    // Note that the range is [theFrameStart, theFrameEnd).
    Frame(const int theFrameStart, const int theFrameEnd)
        : CurrentElement(theFrameStart),
          FrameEnd(theFrameEnd)
    {
    }

    // Return the index of the current element and advance to the next one.
    inline int Advance() { return CurrentElement++; }

    // Check if all elements in the frame have been processed.
    inline bool IsEmpty() const { return CurrentElement == FrameEnd; }

  private:
    int CurrentElement; // Index of the current element of the frame.
    int FrameEnd;       // Index of the last element + 1 of the frame.
  };

public:
  // Adds a new frame for the given range of elements.
  // Note that the range is [theFrameStart, theFrameEnd).
  inline void PushFrame(const int theFrameStart, const int theFrameEnd)
  {
    myFrames.emplace_back(theFrameStart, theFrameEnd);
  }

  // Returns the index of the current element of the top frame
  // and advances to the next element. If all elements of the top
  // frame have been processed, the frame is removed from the stack.
  // Precondition: the stack is not empty.
  inline int PopElement()
  {
    Frame&    aFrame = myFrames.back();
    const int anElem = aFrame.Advance();
    if (aFrame.IsEmpty())
      myFrames.pop_back();

    return anElem;
  }

  // Check if the stack is empty.
  inline bool IsEmpty() const { return myFrames.empty(); }

private:
  std::vector<Frame, NCollection_Allocator<Frame>> myFrames; // Container of frames.
};
} // anonymous namespace

//=================================================================================================

BRepMesh_Delaun::BRepMesh_Delaun(const occ::handle<BRepMesh_DataStructureOfDelaun>& theOldMesh,
                                 const int                        theCellsCountU,
                                 const int                        theCellsCountV,
                                 const bool                        isFillCircles)
    : myMeshData(theOldMesh),
      myCircles(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
      mySupVert(3),
      myInitCircles(false)
{
  if (isFillCircles)
  {
    InitCirclesTool(theCellsCountU, theCellsCountV);
  }
}

//=======================================================================
// function : BRepMesh_Delaun
// purpose  : Creates the triangulation with an empty Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun(IMeshData::Array1OfVertexOfDelaun& theVertices)
    : myCircles(theVertices.Length(),
                new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
      mySupVert(3),
      myInitCircles(false)
{
  if (theVertices.Length() > 2)
  {
    myMeshData = new BRepMesh_DataStructureOfDelaun(
      new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE),
      theVertices.Length());
    Init(theVertices);
  }
}

//=======================================================================
// function : BRepMesh_Delaun
// purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun(const occ::handle<BRepMesh_DataStructureOfDelaun>& theOldMesh,
                                 IMeshData::Array1OfVertexOfDelaun&            theVertices)
    : myMeshData(theOldMesh),
      myCircles(theVertices.Length(),
                new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
      mySupVert(3),
      myInitCircles(false)
{
  if (theVertices.Length() > 2)
  {
    Init(theVertices);
  }
}

//=======================================================================
// function : BRepMesh_Delaun
// purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun(const occ::handle<BRepMesh_DataStructureOfDelaun>& theOldMesh,
                                 IMeshData::VectorOfInteger&                   theVertexIndices)
    : myMeshData(theOldMesh),
      myCircles(theVertexIndices.Length(),
                new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
      mySupVert(3),
      myInitCircles(false)
{
  perform(theVertexIndices);
}

//=======================================================================
// function : BRepMesh_Delaun
// purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun(const occ::handle<BRepMesh_DataStructureOfDelaun>& theOldMesh,
                                 IMeshData::VectorOfInteger&                   theVertexIndices,
                                 const int                        theCellsCountU,
                                 const int                        theCellsCountV)
    : myMeshData(theOldMesh),
      myCircles(theVertexIndices.Length(),
                new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
      mySupVert(3),
      myInitCircles(false)
{
  perform(theVertexIndices, theCellsCountU, theCellsCountV);
}

//=======================================================================
// function : Init
// purpose  : Initializes the triangulation with an Array of Vertex
//=======================================================================
void BRepMesh_Delaun::Init(IMeshData::Array1OfVertexOfDelaun& theVertices)
{
  int           aLowerIdx  = theVertices.Lower();
  int           anUpperIdx = theVertices.Upper();
  IMeshData::VectorOfInteger aVertexIndexes(theVertices.Size());

  int anIndex = aLowerIdx;
  for (; anIndex <= anUpperIdx; ++anIndex)
  {
    aVertexIndexes.Append(myMeshData->AddNode(theVertices(anIndex)));
  }

  perform(aVertexIndexes);
}

//=================================================================================================

void BRepMesh_Delaun::InitCirclesTool(const int theCellsCountU,
                                      const int theCellsCountV)
{
  Bnd_Box2d aBox;
  for (int aNodeIt = 1; aNodeIt <= myMeshData->NbNodes(); ++aNodeIt)
  {
    aBox.Add(gp_Pnt2d(GetVertex(aNodeIt).Coord()));
  }
  aBox.Enlarge(Precision);

  initCirclesTool(aBox, theCellsCountU, theCellsCountV);

  IMeshData::IteratorOfMapOfInteger aTriangleIt(myMeshData->ElementsOfDomain());
  for (; aTriangleIt.More(); aTriangleIt.Next())
  {
    int         aNodesIndices[3];
    const BRepMesh_Triangle& aTriangle = myMeshData->GetElement(aTriangleIt.Key());
    myMeshData->ElementNodes(aTriangle, aNodesIndices);
    myCircles.Bind(aTriangleIt.Key(),
                   GetVertex(aNodesIndices[0]).Coord(),
                   GetVertex(aNodesIndices[1]).Coord(),
                   GetVertex(aNodesIndices[2]).Coord());
  }
}

//=================================================================================================

void BRepMesh_Delaun::initCirclesTool(const Bnd_Box2d&       theBox,
                                      const int theCellsCountU,
                                      const int theCellsCountV)
{
  double aMinX, aMinY, aMaxX, aMaxY;
  theBox.Get(aMinX, aMinY, aMaxX, aMaxY);
  const double aDeltaX = aMaxX - aMinX;
  const double aDeltaY = aMaxY - aMinY;

  int aScaler = 2;
  if (myMeshData->NbNodes() > 100)
  {
    aScaler = 5;
  }
  else if (myMeshData->NbNodes() > 1000)
  {
    aScaler = 7;
  }

  myCircles.SetMinMaxSize(gp_XY(aMinX, aMinY), gp_XY(aMaxX, aMaxY));
  myCircles.SetCellSize(aDeltaX / std::max(theCellsCountU, aScaler),
                        aDeltaY / std::max(theCellsCountV, aScaler));

  myInitCircles = true;
}

//=======================================================================
// function : perform
// purpose  : Create super mesh and run triangulation procedure
//=======================================================================
void BRepMesh_Delaun::perform(IMeshData::VectorOfInteger& theVertexIndices,
                              const int      theCellsCountU /* = -1 */,
                              const int      theCellsCountV /* = -1 */)
{
  if (theVertexIndices.Length() <= 2)
  {
    return;
  }

  Bnd_Box2d        aBox;
  int anIndex = theVertexIndices.Lower();
  int anUpper = theVertexIndices.Upper();
  for (; anIndex <= anUpper; ++anIndex)
  {
    aBox.Add(gp_Pnt2d(GetVertex(theVertexIndices(anIndex)).Coord()));
  }

  aBox.Enlarge(Precision);

  initCirclesTool(aBox, theCellsCountU, theCellsCountV);
  superMesh(aBox);

  ComparatorOfIndexedVertexOfDelaun aCmp(myMeshData);
  std::make_heap(theVertexIndices.begin(), theVertexIndices.end(), aCmp);
  std::sort_heap(theVertexIndices.begin(), theVertexIndices.end(), aCmp);

  compute(theVertexIndices);
}

//=================================================================================================

void BRepMesh_Delaun::superMesh(const Bnd_Box2d& theBox)
{
  double aMinX, aMinY, aMaxX, aMaxY;
  theBox.Get(aMinX, aMinY, aMaxX, aMaxY);
  double aDeltaX = aMaxX - aMinX;
  double aDeltaY = aMaxY - aMinY;

  double aDeltaMin = std::min(aDeltaX, aDeltaY);
  double aDeltaMax = std::max(aDeltaX, aDeltaY);
  double aDelta    = aDeltaX + aDeltaY;

  mySupVert.Append(
    myMeshData->AddNode(BRepMesh_Vertex((aMinX + aMaxX) / 2, aMaxY + aDeltaMax, BRepMesh_Free)));

  mySupVert.Append(
    myMeshData->AddNode(BRepMesh_Vertex(aMinX - aDelta, aMinY - aDeltaMin, BRepMesh_Free)));

  mySupVert.Append(
    myMeshData->AddNode(BRepMesh_Vertex(aMaxX + aDelta, aMinY - aDeltaMin, BRepMesh_Free)));

  int e[3];
  bool o[3];
  for (int aNodeId = 0; aNodeId < 3; ++aNodeId)
  {
    int aFirstNode = aNodeId;
    int aLastNode  = (aNodeId + 1) % 3;
    int aLinkIndex = myMeshData->AddLink(
      BRepMesh_Edge(mySupVert[aFirstNode], mySupVert[aLastNode], BRepMesh_Free));

    e[aNodeId] = std::abs(aLinkIndex);
    o[aNodeId] = (aLinkIndex > 0);
  }

  mySupTrian = BRepMesh_Triangle(e, o, BRepMesh_Free);
}

//=======================================================================
// function : deleteTriangle
// purpose  : Deletes the triangle with the given index and adds the free
//           edges into the map.
//           When an edge is suppressed more than one time it is destroyed.
//=======================================================================
void BRepMesh_Delaun::deleteTriangle(const int          theIndex,
                                     IMeshData::MapOfIntegerInteger& theLoopEdges)
{
  if (myInitCircles)
  {
    myCircles.Delete(theIndex);
  }

  const BRepMesh_Triangle& aElement = GetTriangle(theIndex);
  const int(&e)[3]     = aElement.myEdges;
  const bool(&o)[3]     = aElement.myOrientations;

  myMeshData->RemoveElement(theIndex);

  for (int i = 0; i < 3; ++i)
  {
    if (!theLoopEdges.Bind(e[i], o[i]))
    {
      theLoopEdges.UnBind(e[i]);
      myMeshData->RemoveLink(e[i]);
    }
  }
}

//=======================================================================
// function : compute
// purpose  : Computes the triangulation and add the vertices edges and
//           triangles to the Mesh data structure
//=======================================================================
void BRepMesh_Delaun::compute(IMeshData::VectorOfInteger& theVertexIndexes)
{
  // Insertion of edges of super triangles in the list of free edges:
  occ::handle<NCollection_IncAllocator> aAllocator =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  IMeshData::MapOfIntegerInteger aLoopEdges(10, aAllocator);
  const int(&e)[3] = mySupTrian.myEdges;

  aLoopEdges.Bind(e[0], true);
  aLoopEdges.Bind(e[1], true);
  aLoopEdges.Bind(e[2], true);

  if (theVertexIndexes.Length() > 0)
  {
    // Creation of 3 trianglers with the first node and the edges of the super triangle:
    int anVertexIdx = theVertexIndexes.Lower();
    createTriangles(theVertexIndexes(anVertexIdx), aLoopEdges);

    // Add other nodes to the mesh
    createTrianglesOnNewVertices(theVertexIndexes, Message_ProgressRange());
  }

  RemoveAuxElements();
}

//=================================================================================================

void BRepMesh_Delaun::RemoveAuxElements()
{
  occ::handle<NCollection_IncAllocator> aAllocator =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  IMeshData::MapOfIntegerInteger aLoopEdges(10, aAllocator);

  // Destruction of triangles containing a top of the super triangle
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector(myMeshData);
  for (int aSupVertId = 0; aSupVertId < mySupVert.Size(); ++aSupVertId)
    aSelector.NeighboursOfNode(mySupVert[aSupVertId]);

  IMeshData::IteratorOfMapOfInteger aFreeTriangles(aSelector.Elements());
  for (; aFreeTriangles.More(); aFreeTriangles.Next())
    deleteTriangle(aFreeTriangles.Key(), aLoopEdges);

  // All edges that remain free are removed from aLoopEdges;
  // only the boundary edges of the triangulation remain there
  IMeshData::MapOfIntegerInteger::Iterator aFreeEdges(aLoopEdges);
  for (; aFreeEdges.More(); aFreeEdges.Next())
  {
    if (myMeshData->ElementsConnectedTo(aFreeEdges.Key()).IsEmpty())
      myMeshData->RemoveLink(aFreeEdges.Key());
  }

  // The tops of the super triangle are destroyed
  for (int aSupVertId = 0; aSupVertId < mySupVert.Size(); ++aSupVertId)
    myMeshData->RemoveNode(mySupVert[aSupVertId]);
}

//=======================================================================
// function : createTriangles
// purpose  : Creates the triangles between the node and the polyline.
//=======================================================================
void BRepMesh_Delaun::createTriangles(const int          theVertexIndex,
                                      IMeshData::MapOfIntegerInteger& thePoly)
{
  IMeshData::ListOfInteger aLoopEdges, anExternalEdges;
  const gp_XY&             aVertexCoord = myMeshData->GetNode(theVertexIndex).Coord();

  IMeshData::MapOfIntegerInteger::Iterator anEdges(thePoly);
  for (; anEdges.More(); anEdges.Next())
  {
    int     anEdgeId = anEdges.Key();
    const BRepMesh_Edge& anEdge   = GetEdge(anEdgeId);

    bool isPositive = thePoly(anEdgeId) != 0;

    int aNodes[3];
    if (isPositive)
    {
      aNodes[0] = anEdge.FirstNode();
      aNodes[2] = anEdge.LastNode();
    }
    else
    {
      aNodes[0] = anEdge.LastNode();
      aNodes[2] = anEdge.FirstNode();
    }
    aNodes[1] = theVertexIndex;

    const BRepMesh_Vertex& aFirstVertex = GetVertex(aNodes[0]);
    const BRepMesh_Vertex& aLastVertex  = GetVertex(aNodes[2]);

    gp_XY         anEdgeDir(aLastVertex.Coord() - aFirstVertex.Coord());
    double anEdgeLen = anEdgeDir.Modulus();
    if (anEdgeLen < Precision)
      continue;

    anEdgeDir.SetCoord(anEdgeDir.X() / anEdgeLen, anEdgeDir.Y() / anEdgeLen);

    gp_XY aFirstLinkDir(aFirstVertex.Coord() - aVertexCoord);
    gp_XY aLastLinkDir(aVertexCoord - aLastVertex.Coord());

    double aDist12 = aFirstLinkDir ^ anEdgeDir;
    double aDist23 = anEdgeDir ^ aLastLinkDir;
    if (std::abs(aDist12) < Precision || std::abs(aDist23) < Precision)
    {
      continue;
    }

    BRepMesh_Edge aFirstLink(aNodes[1], aNodes[0], BRepMesh_Free);
    BRepMesh_Edge aLastLink(aNodes[2], aNodes[1], BRepMesh_Free);

    int anEdgesInfo[3] = {myMeshData->AddLink(aFirstLink),
                                       isPositive ? anEdgeId : -anEdgeId,
                                       myMeshData->AddLink(aLastLink)};

    bool isSensOK = (aDist12 > 0. && aDist23 > 0.);
    if (isSensOK)
    {
      int anEdgeIds[3];
      bool anEdgesOri[3];
      for (int aTriLinkIt = 0; aTriLinkIt < 3; ++aTriLinkIt)
      {
        const int& anEdgeInfo = anEdgesInfo[aTriLinkIt];
        anEdgeIds[aTriLinkIt]              = std::abs(anEdgeInfo);
        anEdgesOri[aTriLinkIt]             = anEdgeInfo > 0;
      }

      addTriangle(anEdgeIds, anEdgesOri, aNodes);
    }
    else
    {
      if (isPositive)
        aLoopEdges.Append(anEdges.Key());
      else
        aLoopEdges.Append(-anEdges.Key());

      if (aFirstLinkDir.SquareModulus() > aLastLinkDir.SquareModulus())
        anExternalEdges.Append(std::abs(anEdgesInfo[0]));
      else
        anExternalEdges.Append(std::abs(anEdgesInfo[2]));
    }
  }

  thePoly.Clear();
  while (!anExternalEdges.IsEmpty())
  {
    const BRepMesh_PairOfIndex& aPair =
      myMeshData->ElementsConnectedTo(std::abs(anExternalEdges.First()));

    if (!aPair.IsEmpty())
      deleteTriangle(aPair.FirstIndex(), thePoly);

    anExternalEdges.RemoveFirst();
  }

  for (anEdges.Initialize(thePoly); anEdges.More(); anEdges.Next())
  {
    if (myMeshData->ElementsConnectedTo(anEdges.Key()).IsEmpty())
      myMeshData->RemoveLink(anEdges.Key());
  }

  while (!aLoopEdges.IsEmpty())
  {
    const BRepMesh_Edge& anEdge = GetEdge(std::abs(aLoopEdges.First()));
    if (anEdge.Movability() != BRepMesh_Deleted)
    {
      int anEdgeIdx = aLoopEdges.First();
      meshLeftPolygonOf(std::abs(anEdgeIdx), (anEdgeIdx > 0));
    }

    aLoopEdges.RemoveFirst();
  }
}

//=======================================================================
// function : createTrianglesOnNewVertices
// purpose  : Creation of triangles from the new nodes
//=======================================================================
void BRepMesh_Delaun::createTrianglesOnNewVertices(IMeshData::VectorOfInteger&  theVertexIndexes,
                                                   const Message_ProgressRange& theRange)
{
  occ::handle<NCollection_IncAllocator> aAllocator =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  double aTolU, aTolV;
  myMeshData->Data()->GetTolerance(aTolU, aTolV);
  const double aSqTol = aTolU * aTolU + aTolV * aTolV;

  // Insertion of nodes :
  bool isModify = true;

  int      anIndex = theVertexIndexes.Lower();
  int      anUpper = theVertexIndexes.Upper();
  Message_ProgressScope aPS(theRange, "Create triangles on new vertices", anUpper);
  for (; anIndex <= anUpper; ++anIndex, aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    aAllocator->Reset(false);
    IMeshData::MapOfIntegerInteger aLoopEdges(10, aAllocator);

    int       aVertexIdx = theVertexIndexes(anIndex);
    const BRepMesh_Vertex& aVertex    = GetVertex(aVertexIdx);

    // Iterator in the list of indexes of circles containing the node
    IMeshData::ListOfInteger& aCirclesList = myCircles.Select(aVertex.Coord());

    int                   onEgdeId = 0, aTriangleId = 0;
    IMeshData::ListOfInteger::Iterator aCircleIt(aCirclesList);
    for (; aCircleIt.More(); aCircleIt.Next())
    {
      // To add a node in the mesh it is necessary to check conditions:
      // - the node should be within the boundaries of the mesh and so in an existing triangle
      // - all adjacent triangles should belong to a component connected with this triangle
      if (Contains(aCircleIt.Value(), aVertex, aSqTol, onEgdeId))
      {
        if (onEgdeId != 0 && GetEdge(onEgdeId).Movability() != BRepMesh_Free)
        {
          // We can skip free vertex too close to the frontier edge.
          if (aVertex.Movability() == BRepMesh_Free)
            continue;

          // However, we should add vertex that have neighboring frontier edges.
        }

        // Remove triangle even if it contains frontier edge in order
        // to prevent appearance of incorrect configurations like free
        // edge glued with frontier #26407
        aTriangleId = aCircleIt.Value();
        aCirclesList.Remove(aCircleIt);
        break;
      }
    }

    if (aTriangleId > 0)
    {
      deleteTriangle(aTriangleId, aLoopEdges);

      isModify = true;
      while (isModify && !aCirclesList.IsEmpty())
      {
        isModify = false;
        IMeshData::ListOfInteger::Iterator aCircleIt1(aCirclesList);
        for (; aCircleIt1.More(); aCircleIt1.Next())
        {
          const BRepMesh_Triangle& aElement = GetTriangle(aCircleIt1.Value());
          const int(&e)[3]     = aElement.myEdges;

          if (aLoopEdges.IsBound(e[0]) || aLoopEdges.IsBound(e[1]) || aLoopEdges.IsBound(e[2]))
          {
            isModify = true;
            deleteTriangle(aCircleIt1.Value(), aLoopEdges);
            aCirclesList.Remove(aCircleIt1);
            break;
          }
        }
      }

      // Creation of triangles with the current node and free edges
      // and removal of these edges from the list of free edges
      createTriangles(aVertexIdx, aLoopEdges);
    }
  }

  ProcessConstraints();
}

//=================================================================================================

void BRepMesh_Delaun::insertInternalEdges()
{
  Handle(IMeshData::MapOfInteger) anInternalEdges = InternalEdges();

  // Destruction of triangles intersecting internal edges
  // and their replacement by makeshift triangles
  IMeshData::IteratorOfMapOfInteger anInernalEdgesIt(*anInternalEdges);
  for (; anInernalEdgesIt.More(); anInernalEdgesIt.Next())
  {
    const int      aLinkIndex = anInernalEdgesIt.Key();
    const BRepMesh_PairOfIndex& aPair      = myMeshData->ElementsConnectedTo(aLinkIndex);

    // Check both sides of link for adjusted triangle.
    bool isGo[2] = {true, true};
    for (int aTriangleIt = 1; aTriangleIt <= aPair.Extent(); ++aTriangleIt)
    {
      const BRepMesh_Triangle& aElement = GetTriangle(aPair.Index(aTriangleIt));
      const int(&e)[3]     = aElement.myEdges;
      const bool(&o)[3]     = aElement.myOrientations;

      for (int i = 0; i < 3; ++i)
      {
        if (e[i] == aLinkIndex)
        {
          isGo[o[i] ? 0 : 1] = false;
          break;
        }
      }
    }

    if (isGo[0])
    {
      meshLeftPolygonOf(aLinkIndex, true);
    }

    if (isGo[1])
    {
      meshLeftPolygonOf(aLinkIndex, false);
    }
  }
}

//=================================================================================================

bool BRepMesh_Delaun::isBoundToFrontier(const int theRefNodeId,
                                                    const int theRefLinkId)
{
  std::stack<int> aLinkStack;
  TColStd_PackedMapOfInteger   aVisitedLinks;

  aLinkStack.push(theRefLinkId);
  while (!aLinkStack.empty())
  {
    const int aCurrentLinkId = aLinkStack.top();
    aLinkStack.pop();

    const BRepMesh_PairOfIndex& aPair = myMeshData->ElementsConnectedTo(aCurrentLinkId);
    if (aPair.IsEmpty())
      return false;

    const int aNbElements = aPair.Extent();
    for (int anElemIt = 1; anElemIt <= aNbElements; ++anElemIt)
    {
      const int aTriId = aPair.Index(anElemIt);
      if (aTriId < 0)
        continue;

      const BRepMesh_Triangle& aElement   = GetTriangle(aTriId);
      const int(&anEdges)[3] = aElement.myEdges;

      for (int anEdgeIt = 0; anEdgeIt < 3; ++anEdgeIt)
      {
        const int anEdgeId = anEdges[anEdgeIt];
        if (anEdgeId == aCurrentLinkId)
          continue;

        const BRepMesh_Edge& anEdge = GetEdge(anEdgeId);
        if (anEdge.FirstNode() != theRefNodeId && anEdge.LastNode() != theRefNodeId)
        {
          continue;
        }

        if (anEdge.Movability() != BRepMesh_Free)
          return true;

        if (aVisitedLinks.Add(anEdgeId))
        {
          aLinkStack.push(anEdgeId);
        }
      }
    }
  }

  return false;
}

//=======================================================================
// function : cleanupMesh
// purpose  : Cleanup mesh from the free triangles
//=======================================================================
void BRepMesh_Delaun::cleanupMesh()
{
  occ::handle<NCollection_IncAllocator> aAllocator =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  for (;;)
  {
    aAllocator->Reset(false);
    IMeshData::MapOfIntegerInteger aLoopEdges(10, aAllocator);
    IMeshData::MapOfInteger        aDelTriangles;

    Handle(IMeshData::MapOfInteger)   aFreeEdges = FreeEdges();
    IMeshData::IteratorOfMapOfInteger aFreeEdgesIt(*aFreeEdges);
    for (; aFreeEdgesIt.More(); aFreeEdgesIt.Next())
    {
      const int& aFreeEdgeId = aFreeEdgesIt.Key();
      const BRepMesh_Edge&    anEdge      = GetEdge(aFreeEdgeId);
      if (anEdge.Movability() == BRepMesh_Frontier)
        continue;

      const BRepMesh_PairOfIndex& aPair = myMeshData->ElementsConnectedTo(aFreeEdgeId);
      if (aPair.IsEmpty())
      {
        aLoopEdges.Bind(aFreeEdgeId, true);
        continue;
      }

      int aTriId = aPair.FirstIndex();

      // Check that the connected triangle is not surrounded by another triangles
      const BRepMesh_Triangle& aElement   = GetTriangle(aTriId);
      const int(&anEdges)[3] = aElement.myEdges;

      bool isCanNotBeRemoved = true;
      for (int aCurEdgeIdx = 0; aCurEdgeIdx < 3; ++aCurEdgeIdx)
      {
        if (anEdges[aCurEdgeIdx] != aFreeEdgeId)
          continue;

        for (int anOtherEdgeIt = 1; anOtherEdgeIt <= 2 && isCanNotBeRemoved;
             ++anOtherEdgeIt)
        {
          int            anOtherEdgeId = (aCurEdgeIdx + anOtherEdgeIt) % 3;
          const BRepMesh_PairOfIndex& anOtherEdgePair =
            myMeshData->ElementsConnectedTo(anEdges[anOtherEdgeId]);

          if (anOtherEdgePair.Extent() < 2)
          {
            isCanNotBeRemoved = false;
          }
          else
          {
            for (int aTriIdx = 1; aTriIdx <= anOtherEdgePair.Extent() && isCanNotBeRemoved;
                 ++aTriIdx)
            {
              if (anOtherEdgePair.Index(aTriIdx) == aTriId)
                continue;

              int         v[3];
              const BRepMesh_Triangle& aCurTriangle = GetTriangle(anOtherEdgePair.Index(aTriIdx));
              myMeshData->ElementNodes(aCurTriangle, v);
              for (int aNodeIdx = 0; aNodeIdx < 3 && isCanNotBeRemoved; ++aNodeIdx)
              {
                if (isSupVertex(v[aNodeIdx]))
                {
                  isCanNotBeRemoved = false;
                }
              }
            }
          }
        }

        break;
      }

      if (isCanNotBeRemoved)
        continue;

      bool isConnected[2] = {false, false};
      for (int aLinkNodeIt = 0; aLinkNodeIt < 2; ++aLinkNodeIt)
      {
        isConnected[aLinkNodeIt] =
          isBoundToFrontier((aLinkNodeIt == 0) ? anEdge.FirstNode() : anEdge.LastNode(),
                            aFreeEdgeId);
      }

      if (!isConnected[0] || !isConnected[1])
        aDelTriangles.Add(aTriId);
    }

    // Destruction of triangles :
    int                  aDeletedTrianglesNb = 0;
    IMeshData::IteratorOfMapOfInteger aDelTrianglesIt(aDelTriangles);
    for (; aDelTrianglesIt.More(); aDelTrianglesIt.Next())
    {
      deleteTriangle(aDelTrianglesIt.Key(), aLoopEdges);
      aDeletedTrianglesNb++;
    }

    // Destruction of remaining hanging edges
    IMeshData::MapOfIntegerInteger::Iterator aLoopEdgesIt(aLoopEdges);
    for (; aLoopEdgesIt.More(); aLoopEdgesIt.Next())
    {
      if (myMeshData->ElementsConnectedTo(aLoopEdgesIt.Key()).IsEmpty())
        myMeshData->RemoveLink(aLoopEdgesIt.Key());
    }

    if (aDeletedTrianglesNb == 0)
      break;
  }
}

//=======================================================================
// function : frontierAdjust
// purpose  : Adjust the mesh on the frontier
//=======================================================================
void BRepMesh_Delaun::frontierAdjust()
{
  Handle(IMeshData::MapOfInteger) aFrontier = Frontier();

  occ::handle<NCollection_IncAllocator> aAllocator =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  IMeshData::VectorOfInteger      aFailedFrontiers(256, aAllocator);
  IMeshData::MapOfIntegerInteger  aLoopEdges(10, aAllocator);
  Handle(IMeshData::MapOfInteger) aIntFrontierEdges = new IMeshData::MapOfInteger;

  for (int aPass = 1; aPass <= 2; ++aPass)
  {
    // 1 pass): find external triangles on boundary edges;
    // 2 pass): find external triangles on boundary edges appeared
    //          during triangles replacement.

    IMeshData::IteratorOfMapOfInteger aFrontierIt(*aFrontier);
    for (; aFrontierIt.More(); aFrontierIt.Next())
    {
      int            aFrontierId = aFrontierIt.Key();
      const BRepMesh_PairOfIndex& aPair       = myMeshData->ElementsConnectedTo(aFrontierId);
      int            aNbElem     = aPair.Extent();
      for (int aElemIt = 1; aElemIt <= aNbElem; ++aElemIt)
      {
        const int aPriorElemId = aPair.Index(aElemIt);
        if (aPriorElemId < 0)
          continue;

        const BRepMesh_Triangle& aElement = GetTriangle(aPriorElemId);
        const int(&e)[3]     = aElement.myEdges;
        const bool(&o)[3]     = aElement.myOrientations;

        bool isTriangleFound = false;
        for (int n = 0; n < 3; ++n)
        {
          if (aFrontierId == e[n] && !o[n])
          {
            // Destruction  of external triangles on boundary edges
            isTriangleFound = true;
            deleteTriangle(aPriorElemId, aLoopEdges);
            break;
          }
        }

        if (isTriangleFound)
          break;
      }
    }

    // destrucrion of remaining hanging edges :
    IMeshData::MapOfIntegerInteger::Iterator aLoopEdgesIt(aLoopEdges);
    for (; aLoopEdgesIt.More(); aLoopEdgesIt.Next())
    {
      int aLoopEdgeId = aLoopEdgesIt.Key();
      if (myMeshData->ElementsConnectedTo(aLoopEdgeId).IsEmpty())
        myMeshData->RemoveLink(aLoopEdgeId);
    }

    // destruction of triangles crossing the boundary edges and
    // their replacement by makeshift triangles
    for (aFrontierIt.Reset(); aFrontierIt.More(); aFrontierIt.Next())
    {
      int aFrontierId = aFrontierIt.Key();
      if (!myMeshData->ElementsConnectedTo(aFrontierId).IsEmpty())
        continue;

      bool isSuccess = meshLeftPolygonOf(aFrontierId, true, aIntFrontierEdges);

      if (aPass == 2 && !isSuccess)
        aFailedFrontiers.Append(aFrontierId);
    }
  }

  cleanupMesh();

  // When the mesh has been cleaned up, try to process frontier edges
  // once again to fill the possible gaps that might be occurred in case of "saw" -
  // situation when frontier edge has a triangle at a right side, but its free
  // links cross another frontieres  and meshLeftPolygonOf itself can't collect
  // a closed polygon.
  IMeshData::VectorOfInteger::Iterator aFailedFrontiersIt(aFailedFrontiers);
  for (; aFailedFrontiersIt.More(); aFailedFrontiersIt.Next())
  {
    int aFrontierId = aFailedFrontiersIt.Value();
    if (!myMeshData->ElementsConnectedTo(aFrontierId).IsEmpty())
      continue;

    meshLeftPolygonOf(aFrontierId, true, aIntFrontierEdges);
  }
}

//=======================================================================
// function : fillBndBox
// purpose  : Add bounding box for edge defined by start & end point to
//           the given vector of bounding boxes for triangulation edges
//=======================================================================
void BRepMesh_Delaun::fillBndBox(IMeshData::SequenceOfBndB2d& theBoxes,
                                 const BRepMesh_Vertex&       theV1,
                                 const BRepMesh_Vertex&       theV2)
{
  Bnd_B2d aBox;
  UpdateBndBox(theV1.Coord(), theV2.Coord(), aBox);
  theBoxes.Append(aBox);
}

//=======================================================================
// function : meshLeftPolygonOf
// purpose  : Collect the polygon at the left of the given edge (material side)
//=======================================================================
bool BRepMesh_Delaun::meshLeftPolygonOf(const int          theStartEdgeId,
                                                    const bool          isForward,
                                                    Handle(IMeshData::MapOfInteger) theSkipped)
{
  if (!theSkipped.IsNull() && theSkipped->Contains(theStartEdgeId))
    return true;

  const BRepMesh_Edge& aRefEdge = GetEdge(theStartEdgeId);

  IMeshData::SequenceOfInteger aPolygon;
  int             aStartNode, aPivotNode;
  if (isForward)
  {
    aPolygon.Append(theStartEdgeId);
    aStartNode = aRefEdge.FirstNode();
    aPivotNode = aRefEdge.LastNode();
  }
  else
  {
    aPolygon.Append(-theStartEdgeId);
    aStartNode = aRefEdge.LastNode();
    aPivotNode = aRefEdge.FirstNode();
  }

  const BRepMesh_Vertex& aStartEdgeVertexS = GetVertex(aStartNode);
  BRepMesh_Vertex        aPivotVertex      = GetVertex(aPivotNode);

  gp_Vec2d aRefLinkDir(aPivotVertex.Coord() - aStartEdgeVertexS.Coord());

  if (aRefLinkDir.SquareMagnitude() < Precision2)
    return true;

  // Auxiliary structures.
  // Bounding boxes of polygon links to be used for preliminary
  // analysis of intersections
  IMeshData::SequenceOfBndB2d aBoxes;
  fillBndBox(aBoxes, aStartEdgeVertexS, aPivotVertex);

  // Hanging ends
  IMeshData::MapOfInteger aDeadLinks;

  // Links are temporarily excluded from consideration
  IMeshData::MapOfInteger aLeprousLinks;
  aLeprousLinks.Add(theStartEdgeId);

  bool isSkipLeprous = true;
  int aFirstNode    = aStartNode;
  while (aPivotNode != aFirstNode)
  {
    Bnd_B2d          aNextLinkBndBox;
    gp_Vec2d         aNextLinkDir;
    int aNextPivotNode = 0;

    int aNextLinkId = findNextPolygonLink(aFirstNode,
                                                       aPivotNode,
                                                       aPivotVertex,
                                                       aRefLinkDir,
                                                       aBoxes,
                                                       aPolygon,
                                                       theSkipped,
                                                       isSkipLeprous,
                                                       aLeprousLinks,
                                                       aDeadLinks,
                                                       aNextPivotNode,
                                                       aNextLinkDir,
                                                       aNextLinkBndBox);

    if (aNextLinkId != 0)
    {
      aStartNode  = aPivotNode;
      aRefLinkDir = aNextLinkDir;

      aPivotNode   = aNextPivotNode;
      aPivotVertex = GetVertex(aNextPivotNode);

      aBoxes.Append(aNextLinkBndBox);
      aPolygon.Append(aNextLinkId);

      isSkipLeprous = true;
    }
    else
    {
      // Nothing to do
      if (aPolygon.Length() == 1)
        return false;

      // Return to the previous point
      int aDeadLinkId = std::abs(aPolygon.Last());
      aDeadLinks.Add(aDeadLinkId);

      aLeprousLinks.Remove(aDeadLinkId);
      aPolygon.Remove(aPolygon.Length());
      aBoxes.Remove(aBoxes.Length());

      int     aPrevLinkInfo = aPolygon.Last();
      const BRepMesh_Edge& aPrevLink     = GetEdge(std::abs(aPrevLinkInfo));

      if (aPrevLinkInfo > 0)
      {
        aStartNode = aPrevLink.FirstNode();
        aPivotNode = aPrevLink.LastNode();
      }
      else
      {
        aStartNode = aPrevLink.LastNode();
        aPivotNode = aPrevLink.FirstNode();
      }

      aPivotVertex = GetVertex(aPivotNode);
      aRefLinkDir  = aPivotVertex.Coord() - GetVertex(aStartNode).Coord();

      isSkipLeprous = false;
    }
  }

  if (aPolygon.Length() < 3)
    return false;

  cleanupPolygon(aPolygon, aBoxes);
  meshPolygon(aPolygon, aBoxes, theSkipped);

  return true;
}

//=======================================================================
// function : findNextPolygonLink
// purpose  : Find next link starting from the given node and has maximum
//           angle respect the given reference link.
//           Each time the next link is found other neighbor links at the
//           pivot node are marked as leprous and will be excluded from
//           consideration next time until a hanging end is occurred.
//=======================================================================
int BRepMesh_Delaun::findNextPolygonLink(
  const int&                theFirstNode,
  const int&                thePivotNode,
  const BRepMesh_Vertex&                 thePivotVertex,
  const gp_Vec2d&                        theRefLinkDir,
  const IMeshData::SequenceOfBndB2d&     theBoxes,
  const IMeshData::SequenceOfInteger&    thePolygon,
  const Handle(IMeshData::MapOfInteger)& theSkipped,
  const bool&                isSkipLeprous,
  IMeshData::MapOfInteger&               theLeprousLinks,
  IMeshData::MapOfInteger&               theDeadLinks,
  int&                      theNextPivotNode,
  gp_Vec2d&                              theNextLinkDir,
  Bnd_B2d&                               theNextLinkBndBox)
{
  // Find the next link having the greatest angle
  // respect to a direction of a reference one
  double aMaxAngle = RealFirst();

  int                   aNextLinkId = 0;
  IMeshData::ListOfInteger::Iterator aLinkIt(myMeshData->LinksConnectedTo(thePivotNode));
  for (; aLinkIt.More(); aLinkIt.Next())
  {
    const int& aNeighbourLinkInfo = aLinkIt.Value();
    int        aNeighbourLinkId   = std::abs(aNeighbourLinkInfo);

    if (theDeadLinks.Contains(aNeighbourLinkId)
        || (!theSkipped.IsNull() && theSkipped->Contains(aNeighbourLinkId)))
    {
      continue;
    }

    bool isLeprous = theLeprousLinks.Contains(aNeighbourLinkId);
    if (isSkipLeprous && isLeprous)
      continue;

    const BRepMesh_Edge& aNeighbourLink = GetEdge(aNeighbourLinkId);

    // Determine whether the link belongs to the mesh
    if (aNeighbourLink.Movability() == BRepMesh_Free
        && myMeshData->ElementsConnectedTo(aNeighbourLinkInfo).IsEmpty())
    {
      theDeadLinks.Add(aNeighbourLinkId);
      continue;
    }

    int anOtherNode = aNeighbourLink.FirstNode();
    if (anOtherNode == thePivotNode)
      anOtherNode = aNeighbourLink.LastNode();

    gp_Vec2d aCurLinkDir(GetVertex(anOtherNode).Coord() - thePivotVertex.Coord());

    if (aCurLinkDir.SquareMagnitude() < Precision2)
    {
      theDeadLinks.Add(aNeighbourLinkId);
      continue;
    }

    if (!isLeprous)
      theLeprousLinks.Add(aNeighbourLinkId);

    double    anAngle    = theRefLinkDir.Angle(aCurLinkDir);
    bool isFrontier = (aNeighbourLink.Movability() == BRepMesh_Frontier);

    bool isCheckPointOnEdge = true;
    if (isFrontier)
    {
      if (std::abs(std::abs(anAngle) - M_PI) < Precision::Angular())
      {
        // Glued constrains - don't check intersection
        isCheckPointOnEdge = false;
        anAngle            = std::abs(anAngle);
      }
    }

    if (anAngle <= aMaxAngle)
      continue;

    bool isCheckEndPoints = (anOtherNode != theFirstNode);

    Bnd_B2d          aBox;
    bool isNotIntersect = checkIntersection(aNeighbourLink,
                                                        thePolygon,
                                                        theBoxes,
                                                        isCheckEndPoints,
                                                        isCheckPointOnEdge,
                                                        true,
                                                        aBox);

    if (isNotIntersect)
    {
      aMaxAngle = anAngle;

      theNextLinkDir    = aCurLinkDir;
      theNextPivotNode  = anOtherNode;
      theNextLinkBndBox = aBox;

      aNextLinkId =
        (aNeighbourLink.FirstNode() == thePivotNode) ? aNeighbourLinkId : -aNeighbourLinkId;
    }
  }

  return aNextLinkId;
}

//=======================================================================
// function : checkIntersection
// purpose  : Check is the given link intersects the polygon boundaries.
//           Returns bounding box for the given link through the
//           <theLinkBndBox> parameter.
//=======================================================================
bool BRepMesh_Delaun::checkIntersection(const BRepMesh_Edge&                theLink,
                                                    const IMeshData::SequenceOfInteger& thePolygon,
                                                    const IMeshData::SequenceOfBndB2d& thePolyBoxes,
                                                    const bool isConsiderEndPointTouch,
                                                    const bool isConsiderPointOnEdge,
                                                    const bool isSkipLastEdge,
                                                    Bnd_B2d&               theLinkBndBox) const
{
  UpdateBndBox(GetVertex(theLink.FirstNode()).Coord(),
               GetVertex(theLink.LastNode()).Coord(),
               theLinkBndBox);

  int aPolyLen = thePolygon.Length();
  // Don't check intersection with the last link
  if (isSkipLastEdge)
    --aPolyLen;

  bool isFrontier = (theLink.Movability() == BRepMesh_Frontier);

  for (int aPolyIt = 1; aPolyIt <= aPolyLen; ++aPolyIt)
  {
    if (!theLinkBndBox.IsOut(thePolyBoxes.Value(aPolyIt)))
    {
      // intersection is possible...
      int     aPolyLinkId = std::abs(thePolygon(aPolyIt));
      const BRepMesh_Edge& aPolyLink   = GetEdge(aPolyLinkId);

      // skip intersections between frontier edges
      if (aPolyLink.Movability() == BRepMesh_Frontier && isFrontier)
        continue;

      gp_Pnt2d                   anIntPnt;
      BRepMesh_GeomTool::IntFlag aIntFlag =
        intSegSeg(theLink, aPolyLink, isConsiderEndPointTouch, isConsiderPointOnEdge, anIntPnt);

      if (aIntFlag != BRepMesh_GeomTool::NoIntersection)
        return false;
    }
  }

  // Ok, no intersection
  return true;
}

//=======================================================================
// function : addTriangle
// purpose  : Add a triangle based on the given oriented edges into mesh
//=======================================================================
void BRepMesh_Delaun::addTriangle(const int (&theEdgesId)[3],
                                  const bool (&theEdgesOri)[3],
                                  const int (&theNodesId)[3])
{
  int aNewTriangleId =
    myMeshData->AddElement(BRepMesh_Triangle(theEdgesId, theEdgesOri, BRepMesh_Free));

  bool isAdded = true;
  if (myInitCircles)
  {
    isAdded = myCircles.Bind(aNewTriangleId,
                             GetVertex(theNodesId[0]).Coord(),
                             GetVertex(theNodesId[1]).Coord(),
                             GetVertex(theNodesId[2]).Coord());
  }

  if (!isAdded)
    myMeshData->RemoveElement(aNewTriangleId);
}

//=======================================================================
// function : cleanupPolygon
// purpose  : Remove internal triangles from the given polygon
//=======================================================================
void BRepMesh_Delaun::cleanupPolygon(const IMeshData::SequenceOfInteger& thePolygon,
                                     const IMeshData::SequenceOfBndB2d&  thePolyBoxes)
{
  int aPolyLen = thePolygon.Length();
  if (aPolyLen < 3)
    return;

  occ::handle<NCollection_IncAllocator> aAllocator =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  IMeshData::MapOfIntegerInteger aLoopEdges(10, aAllocator);
  IMeshData::MapOfInteger        anIgnoredEdges;
  IMeshData::MapOfInteger        aPolyVerticesFindMap;
  IMeshData::VectorOfInteger     aPolyVertices(256, aAllocator);
  // Collect boundary vertices of the polygon
  for (int aPolyIt = 1; aPolyIt <= aPolyLen; ++aPolyIt)
  {
    int aPolyEdgeInfo = thePolygon(aPolyIt);
    int aPolyEdgeId   = std::abs(aPolyEdgeInfo);
    anIgnoredEdges.Add(aPolyEdgeId);

    bool            isForward = (aPolyEdgeInfo > 0);
    const BRepMesh_PairOfIndex& aPair     = myMeshData->ElementsConnectedTo(aPolyEdgeId);

    int anElemIt = 1;
    for (; anElemIt <= aPair.Extent(); ++anElemIt)
    {
      int anElemId = aPair.Index(anElemIt);
      if (anElemId < 0)
        continue;

      const BRepMesh_Triangle& aElement      = GetTriangle(anElemId);
      const int(&anEdges)[3]    = aElement.myEdges;
      const bool(&anEdgesOri)[3] = aElement.myOrientations;

      int isTriangleFound = false;
      for (int anEdgeIt = 0; anEdgeIt < 3; ++anEdgeIt)
      {
        if (anEdges[anEdgeIt] == aPolyEdgeId && anEdgesOri[anEdgeIt] == isForward)
        {
          isTriangleFound = true;
          deleteTriangle(anElemId, aLoopEdges);
          break;
        }
      }

      if (isTriangleFound)
        break;
    }

    // Skip a neighbor link to extract unique vertices each time
    if (aPolyIt % 2)
    {
      const BRepMesh_Edge& aPolyEdge    = GetEdge(aPolyEdgeId);
      int     aFirstVertex = aPolyEdge.FirstNode();
      int     aLastVertex  = aPolyEdge.LastNode();

      aPolyVerticesFindMap.Add(aFirstVertex);
      aPolyVerticesFindMap.Add(aLastVertex);

      if (aPolyEdgeInfo > 0)
      {
        aPolyVertices.Append(aFirstVertex);
        aPolyVertices.Append(aLastVertex);
      }
      else
      {
        aPolyVertices.Append(aLastVertex);
        aPolyVertices.Append(aFirstVertex);
      }
    }
  }

  // Make closed sequence
  if (aPolyVertices.First() != aPolyVertices.Last())
    aPolyVertices.Append(aPolyVertices.First());

  IMeshData::MapOfInteger aSurvivedLinks(anIgnoredEdges);

  for (int aPolyVertIt = 0; aPolyVertIt < aPolyVertices.Length() - 1; ++aPolyVertIt)
  {
    StackOfFrames              aStackFames;
    IMeshData::VectorOfInteger aStackData;
    for (int aCurrentVictim = aPolyVertices(aPolyVertIt); aCurrentVictim != -1;
         aCurrentVictim     = aStackFames.IsEmpty() ? -1 : aStackData(aStackFames.PopElement()))
    {
      const int aPrevStackDataSize = aStackData.Length();
      killTrianglesAroundVertex(aCurrentVictim,
                                aPolyVertices,
                                aPolyVerticesFindMap,
                                thePolygon,
                                thePolyBoxes,
                                aSurvivedLinks,
                                aLoopEdges,
                                aStackData);
      const int aNewStackDataSize = aStackData.Length();
      if (aNewStackDataSize > aPrevStackDataSize)
      {
        aStackFames.PushFrame(aPrevStackDataSize, aNewStackDataSize);
      }
    }
  }

  IMeshData::MapOfIntegerInteger::Iterator aLoopEdgesIt(aLoopEdges);
  for (; aLoopEdgesIt.More(); aLoopEdgesIt.Next())
  {
    const int& aLoopEdgeId = aLoopEdgesIt.Key();
    if (anIgnoredEdges.Contains(aLoopEdgeId))
      continue;

    if (myMeshData->ElementsConnectedTo(aLoopEdgeId).IsEmpty())
      myMeshData->RemoveLink(aLoopEdgesIt.Key());
  }
}

//=======================================================================
// function : killTrianglesAroundVertex
// purpose  : Remove all triangles and edges that are placed
//           inside the polygon or crossed it.
//=======================================================================
void BRepMesh_Delaun::killTrianglesAroundVertex(
  const int              theZombieNodeId,
  const IMeshData::VectorOfInteger&   thePolyVertices,
  const IMeshData::MapOfInteger&      thePolyVerticesFindMap,
  const IMeshData::SequenceOfInteger& thePolygon,
  const IMeshData::SequenceOfBndB2d&  thePolyBoxes,
  IMeshData::MapOfInteger&            theSurvivedLinks,
  IMeshData::MapOfIntegerInteger&     theLoopEdges,
  IMeshData::VectorOfInteger&         theVictimNodes)
{
  IMeshData::ListOfInteger::Iterator aNeighborsIt = myMeshData->LinksConnectedTo(theZombieNodeId);

  // Try to infect neighbor nodes
  for (; aNeighborsIt.More(); aNeighborsIt.Next())
  {
    const int& aNeighborLinkId = aNeighborsIt.Value();
    if (theSurvivedLinks.Contains(aNeighborLinkId))
      continue;

    const BRepMesh_Edge& aNeighborLink = GetEdge(aNeighborLinkId);
    if (aNeighborLink.Movability() == BRepMesh_Frontier)
    {
      // Though, if it lies onto the polygon boundary -
      // take its triangles
      Bnd_B2d          aBox;
      bool isNotIntersect = checkIntersection(aNeighborLink,
                                                          thePolygon,
                                                          thePolyBoxes,
                                                          false,
                                                          true,
                                                          false,
                                                          aBox);

      if (isNotIntersect)
      {
        // Don't touch it
        theSurvivedLinks.Add(aNeighborLinkId);
        continue;
      }
    }
    else
    {
      int anOtherNode = aNeighborLink.FirstNode();
      if (anOtherNode == theZombieNodeId)
        anOtherNode = aNeighborLink.LastNode();

      // Possible sacrifice
      if (!thePolyVerticesFindMap.Contains(anOtherNode))
      {
        if (isVertexInsidePolygon(anOtherNode, thePolyVertices))
        {
          // Got you!
          theVictimNodes.Append(anOtherNode);
        }
        else
        {
          // Lucky. But it may intersect the polygon boundary.
          // Let's check it!
          killTrianglesOnIntersectingLinks(aNeighborLinkId,
                                           aNeighborLink,
                                           anOtherNode,
                                           thePolygon,
                                           thePolyBoxes,
                                           theSurvivedLinks,
                                           theLoopEdges);

          continue;
        }
      }
    }

    // Add link to the survivors to avoid cycling
    theSurvivedLinks.Add(aNeighborLinkId);
    killLinkTriangles(aNeighborLinkId, theLoopEdges);
  }
}

//=======================================================================
// function : isVertexInsidePolygon
// purpose  : Checks is the given vertex lies inside the polygon
//=======================================================================
bool BRepMesh_Delaun::isVertexInsidePolygon(
  const int&           theVertexId,
  const IMeshData::VectorOfInteger& thePolygonVertices) const
{
  int aPolyLen = thePolygonVertices.Length();
  if (aPolyLen < 3)
    return false;

  const gp_XY aCenterPointXY = GetVertex(theVertexId).Coord();

  const BRepMesh_Vertex& aFirstVertex = GetVertex(thePolygonVertices(0));
  gp_Vec2d               aPrevVertexDir(aFirstVertex.Coord() - aCenterPointXY);
  if (aPrevVertexDir.SquareMagnitude() < Precision2)
    return true;

  double aTotalAng = 0.0;
  for (int aPolyIt = 1; aPolyIt < aPolyLen; ++aPolyIt)
  {
    const BRepMesh_Vertex& aPolyVertex = GetVertex(thePolygonVertices(aPolyIt));

    gp_Vec2d aCurVertexDir(aPolyVertex.Coord() - aCenterPointXY);
    if (aCurVertexDir.SquareMagnitude() < Precision2)
      return true;

    aTotalAng += aCurVertexDir.Angle(aPrevVertexDir);
    aPrevVertexDir = aCurVertexDir;
  }

  if (std::abs(std::abs(aTotalAng) - Angle2PI) > Precision::Angular())
    return false;

  return true;
}

//=======================================================================
// function : killTrianglesOnIntersectingLinks
// purpose  : Checks is the given link crosses the polygon boundary.
//           If yes, kills its triangles and checks neighbor links on
//           boundary intersection. Does nothing elsewhere.
//=======================================================================
void BRepMesh_Delaun::killTrianglesOnIntersectingLinks(
  const int&             theLinkToCheckId,
  const BRepMesh_Edge&                theLinkToCheck,
  const int&             theEndPoint,
  const IMeshData::SequenceOfInteger& thePolygon,
  const IMeshData::SequenceOfBndB2d&  thePolyBoxes,
  IMeshData::MapOfInteger&            theSurvivedLinks,
  IMeshData::MapOfIntegerInteger&     theLoopEdges)
{
  if (theSurvivedLinks.Contains(theLinkToCheckId))
    return;

  Bnd_B2d          aBox;
  bool isNotIntersect = checkIntersection(theLinkToCheck,
                                                      thePolygon,
                                                      thePolyBoxes,
                                                      false,
                                                      false,
                                                      false,
                                                      aBox);

  theSurvivedLinks.Add(theLinkToCheckId);

  if (isNotIntersect)
    return;

  killLinkTriangles(theLinkToCheckId, theLoopEdges);

  IMeshData::ListOfInteger::Iterator aNeighborsIt(myMeshData->LinksConnectedTo(theEndPoint));

  for (; aNeighborsIt.More(); aNeighborsIt.Next())
  {
    const int& aNeighborLinkId = aNeighborsIt.Value();
    const BRepMesh_Edge&    aNeighborLink   = GetEdge(aNeighborLinkId);
    int        anOtherNode     = aNeighborLink.FirstNode();
    if (anOtherNode == theEndPoint)
      anOtherNode = aNeighborLink.LastNode();

    killTrianglesOnIntersectingLinks(aNeighborLinkId,
                                     aNeighborLink,
                                     anOtherNode,
                                     thePolygon,
                                     thePolyBoxes,
                                     theSurvivedLinks,
                                     theLoopEdges);
  }
}

//=======================================================================
// function : killLinkTriangles
// purpose  : Kill triangles bound to the given link.
//=======================================================================
void BRepMesh_Delaun::killLinkTriangles(const int&         theLinkId,
                                        IMeshData::MapOfIntegerInteger& theLoopEdges)
{
  const BRepMesh_PairOfIndex& aPair = myMeshData->ElementsConnectedTo(theLinkId);

  int anElemNb = aPair.Extent();
  for (int aPairIt = 1; aPairIt <= anElemNb; ++aPairIt)
  {
    int anElemId = aPair.FirstIndex();
    if (anElemId < 0)
      continue;

    deleteTriangle(anElemId, theLoopEdges);
  }
}

//=======================================================================
// function : getOrientedNodes
// purpose  : Returns start and end nodes of the given edge in respect to
//           its orientation.
//=======================================================================
void BRepMesh_Delaun::getOrientedNodes(const BRepMesh_Edge&   theEdge,
                                       const bool isForward,
                                       int*      theNodes) const
{
  if (isForward)
  {
    theNodes[0] = theEdge.FirstNode();
    theNodes[1] = theEdge.LastNode();
  }
  else
  {
    theNodes[0] = theEdge.LastNode();
    theNodes[1] = theEdge.FirstNode();
  }
}

//=======================================================================
// function : processLoop
// purpose  : Processes loop within the given polygon formed by range of
//           its links specified by start and end link indices.
//=======================================================================
void BRepMesh_Delaun::processLoop(const int              theLinkFrom,
                                  const int              theLinkTo,
                                  const IMeshData::SequenceOfInteger& thePolygon,
                                  const IMeshData::SequenceOfBndB2d&  thePolyBoxes)
{
  int aNbOfLinksInLoop = theLinkTo - theLinkFrom - 1;
  if (aNbOfLinksInLoop < 3)
    return;

  IMeshData::SequenceOfInteger aPolygon;
  IMeshData::SequenceOfBndB2d  aPolyBoxes;
  for (; aNbOfLinksInLoop > 0; --aNbOfLinksInLoop)
  {
    int aLoopLinkIndex = theLinkFrom + aNbOfLinksInLoop;
    aPolygon.Prepend(thePolygon(aLoopLinkIndex));
    aPolyBoxes.Prepend(thePolyBoxes(aLoopLinkIndex));
  }
  meshPolygon(aPolygon, aPolyBoxes);
}

//=======================================================================
// function : createAndReplacePolygonLink
// purpose  : Creates new link based on the given nodes and updates the
//           given polygon.
//=======================================================================
int BRepMesh_Delaun::createAndReplacePolygonLink(
  const int*       theNodes,
  const gp_Pnt2d*               thePnts,
  const int        theRootIndex,
  const ReplaceFlag             theReplaceFlag,
  IMeshData::SequenceOfInteger& thePolygon,
  IMeshData::SequenceOfBndB2d&  thePolyBoxes)
{
  int aNewEdgeId =
    myMeshData->AddLink(BRepMesh_Edge(theNodes[0], theNodes[1], BRepMesh_Free));

  Bnd_B2d aNewBox;
  UpdateBndBox(thePnts[0].Coord(), thePnts[1].Coord(), aNewBox);

  switch (theReplaceFlag)
  {
    case BRepMesh_Delaun::Replace:
      thePolygon.SetValue(theRootIndex, aNewEdgeId);
      thePolyBoxes.SetValue(theRootIndex, aNewBox);
      break;

    case BRepMesh_Delaun::InsertAfter:
      thePolygon.InsertAfter(theRootIndex, aNewEdgeId);
      thePolyBoxes.InsertAfter(theRootIndex, aNewBox);
      break;

    case BRepMesh_Delaun::InsertBefore:
      thePolygon.InsertBefore(theRootIndex, aNewEdgeId);
      thePolyBoxes.InsertBefore(theRootIndex, aNewBox);
      break;
  }

  return aNewEdgeId;
}

//=================================================================================================

void BRepMesh_Delaun::meshPolygon(IMeshData::SequenceOfInteger&   thePolygon,
                                  IMeshData::SequenceOfBndB2d&    thePolyBoxes,
                                  Handle(IMeshData::MapOfInteger) theSkipped)
{
  // Check is the source polygon elementary
  if (meshElementaryPolygon(thePolygon))
    return;

  // Check and correct boundary edges
  int    aPolyLen       = thePolygon.Length();
  const double aPolyArea      = std::abs(polyArea(thePolygon, 1, aPolyLen));
  const double aSmallLoopArea = 0.001 * aPolyArea;
  for (int aPolyIt = 1; aPolyIt < aPolyLen; ++aPolyIt)
  {
    int     aCurEdgeInfo = thePolygon(aPolyIt);
    int     aCurEdgeId   = std::abs(aCurEdgeInfo);
    const BRepMesh_Edge* aCurEdge     = &GetEdge(aCurEdgeId);
    if (aCurEdge->Movability() != BRepMesh_Frontier)
      continue;

    int aCurNodes[2];
    getOrientedNodes(*aCurEdge, aCurEdgeInfo > 0, aCurNodes);

    gp_Pnt2d aCurPnts[2] = {GetVertex(aCurNodes[0]).Coord(), GetVertex(aCurNodes[1]).Coord()};

    gp_Vec2d aCurVec(aCurPnts[0], aCurPnts[1]);

    // check further links
    int aNextPolyIt = aPolyIt + 1;
    for (; aNextPolyIt <= aPolyLen; ++aNextPolyIt)
    {
      int     aNextEdgeInfo = thePolygon(aNextPolyIt);
      int     aNextEdgeId   = std::abs(aNextEdgeInfo);
      const BRepMesh_Edge* aNextEdge     = &GetEdge(aNextEdgeId);
      if (aNextEdge->Movability() != BRepMesh_Frontier)
        continue;

      int aNextNodes[2];
      getOrientedNodes(*aNextEdge, aNextEdgeInfo > 0, aNextNodes);

      gp_Pnt2d aNextPnts[2] = {GetVertex(aNextNodes[0]).Coord(), GetVertex(aNextNodes[1]).Coord()};

      gp_Pnt2d                   anIntPnt;
      BRepMesh_GeomTool::IntFlag aIntFlag =
        intSegSeg(*aCurEdge, *aNextEdge, false, true, anIntPnt);

      if (aIntFlag == BRepMesh_GeomTool::NoIntersection)
        continue;

      bool isRemoveFromFirst  = false;
      bool isAddReplacingEdge = true;
      int aIndexToRemoveTo   = aNextPolyIt;
      if (aIntFlag == BRepMesh_GeomTool::Cross)
      {
        double aLoopArea = polyArea(thePolygon, aPolyIt + 1, aNextPolyIt);
        gp_Vec2d      aVec1(anIntPnt, aCurPnts[1]);
        gp_Vec2d      aVec2(anIntPnt, aNextPnts[0]);

        aLoopArea += (aVec1 ^ aVec2) / 2.;
        if (std::abs(aLoopArea) > aSmallLoopArea)
        {
          aNextNodes[1] = aCurNodes[0];
          aNextPnts[1]  = aCurPnts[0];

          aNextEdgeId = std::abs(createAndReplacePolygonLink(aNextNodes,
                                                             aNextPnts,
                                                             aNextPolyIt,
                                                             BRepMesh_Delaun::Replace,
                                                             thePolygon,
                                                             thePolyBoxes));

          processLoop(aPolyIt, aNextPolyIt, thePolygon, thePolyBoxes);
          return;
        }

        double aDist1 = anIntPnt.SquareDistance(aNextPnts[0]);
        double aDist2 = anIntPnt.SquareDistance(aNextPnts[1]);

        // Choose node with lower distance
        const bool isCloseToStart = (aDist1 < aDist2);
        const int aEndPointIndex = isCloseToStart ? 0 : 1;
        aCurNodes[1]                          = aNextNodes[aEndPointIndex];
        aCurPnts[1]                           = aNextPnts[aEndPointIndex];

        if (isCloseToStart)
          --aIndexToRemoveTo;

        // In this context only intersections between frontier edges
        // are possible. If intersection between edges of different
        // types occurred - treat this case as invalid (i.e. result
        // might not reflect the expectations).
        if (!theSkipped.IsNull())
        {
          int aSkippedLinkIt = aPolyIt;
          for (; aSkippedLinkIt <= aIndexToRemoveTo; ++aSkippedLinkIt)
            theSkipped->Add(std::abs(thePolygon(aSkippedLinkIt)));
        }
      }
      else if (aIntFlag == BRepMesh_GeomTool::PointOnSegment)
      {
        // Identify chopping link
        bool isFirstChopping = false;
        int aCheckPointIt   = 0;
        for (; aCheckPointIt < 2; ++aCheckPointIt)
        {
          gp_Pnt2d& aRefPoint = aCurPnts[aCheckPointIt];
          // Check is second link touches the first one
          gp_Vec2d aVec1(aRefPoint, aNextPnts[0]);
          gp_Vec2d aVec2(aRefPoint, aNextPnts[1]);
          if (std::abs(aVec1 ^ aVec2) < Precision)
          {
            isFirstChopping = true;
            break;
          }
        }

        if (isFirstChopping)
        {
          // Split second link
          isAddReplacingEdge = false;
          isRemoveFromFirst  = (aCheckPointIt == 0);

          int aSplitLink[3] = {aNextNodes[0], aCurNodes[aCheckPointIt], aNextNodes[1]};

          gp_Pnt2d aSplitPnts[3] = {aNextPnts[0], aCurPnts[aCheckPointIt], aNextPnts[1]};

          int aSplitLinkIt = 0;
          for (; aSplitLinkIt < 2; ++aSplitLinkIt)
          {
            createAndReplacePolygonLink(&aSplitLink[aSplitLinkIt],
                                        &aSplitPnts[aSplitLinkIt],
                                        aNextPolyIt,
                                        (aSplitLinkIt == 0) ? BRepMesh_Delaun::Replace
                                                            : BRepMesh_Delaun::InsertAfter,
                                        thePolygon,
                                        thePolyBoxes);
          }

          processLoop(aPolyIt + aCheckPointIt, aIndexToRemoveTo, thePolygon, thePolyBoxes);
        }
        else
        {
          // Split first link
          int aSplitLinkNodes[2] = {aNextNodes[1], aCurNodes[1]};

          gp_Pnt2d aSplitLinkPnts[2] = {aNextPnts[1], aCurPnts[1]};
          createAndReplacePolygonLink(aSplitLinkNodes,
                                      aSplitLinkPnts,
                                      aPolyIt,
                                      BRepMesh_Delaun::InsertAfter,
                                      thePolygon,
                                      thePolyBoxes);

          aCurNodes[1] = aNextNodes[1];
          aCurPnts[1]  = aNextPnts[1];
          ++aIndexToRemoveTo;

          processLoop(aPolyIt + 1, aIndexToRemoveTo, thePolygon, thePolyBoxes);
        }
      }
      else if (aIntFlag == BRepMesh_GeomTool::Glued)
      {
        if (aCurNodes[1] == aNextNodes[0])
        {
          aCurNodes[1] = aNextNodes[1];
          aCurPnts[1]  = aNextPnts[1];
        }
        // TODO: Non-adjacent glued links within the polygon
      }
      else if (aIntFlag == BRepMesh_GeomTool::Same)
      {
        processLoop(aPolyIt, aNextPolyIt, thePolygon, thePolyBoxes);

        isRemoveFromFirst  = true;
        isAddReplacingEdge = false;
      }
      else
        continue; // Not supported type

      if (isAddReplacingEdge)
      {
        aCurEdgeId = std::abs(createAndReplacePolygonLink(aCurNodes,
                                                          aCurPnts,
                                                          aPolyIt,
                                                          BRepMesh_Delaun::Replace,
                                                          thePolygon,
                                                          thePolyBoxes));

        aCurEdge = &GetEdge(aCurEdgeId);
        aCurVec  = gp_Vec2d(aCurPnts[0], aCurPnts[1]);
      }

      int aIndexToRemoveFrom = isRemoveFromFirst ? aPolyIt : aPolyIt + 1;

      thePolygon.Remove(aIndexToRemoveFrom, aIndexToRemoveTo);
      thePolyBoxes.Remove(aIndexToRemoveFrom, aIndexToRemoveTo);

      aPolyLen = thePolygon.Length();
      if (isRemoveFromFirst)
      {
        --aPolyIt;
        break;
      }

      aNextPolyIt = aPolyIt;
    }
  }

  IMeshData::SequenceOfInteger* aPolygon1   = &thePolygon;
  IMeshData::SequenceOfBndB2d*  aPolyBoxes1 = &thePolyBoxes;

  Handle(IMeshData::SequenceOfInteger) aPolygon2   = new IMeshData::SequenceOfInteger;
  Handle(IMeshData::SequenceOfBndB2d)  aPolyBoxes2 = new IMeshData::SequenceOfBndB2d;

  NCollection_Sequence<Handle(IMeshData::SequenceOfInteger)> aPolyStack;
  NCollection_Sequence<Handle(IMeshData::SequenceOfBndB2d)>  aPolyBoxStack;
  for (;;)
  {
    decomposeSimplePolygon(*aPolygon1, *aPolyBoxes1, *aPolygon2, *aPolyBoxes2);
    if (!aPolygon2->IsEmpty())
    {
      aPolyStack.Append(aPolygon2);
      aPolyBoxStack.Append(aPolyBoxes2);

      aPolygon2   = new IMeshData::SequenceOfInteger;
      aPolyBoxes2 = new IMeshData::SequenceOfBndB2d;
    }

    if (aPolygon1->IsEmpty())
    {
      if (!aPolyStack.IsEmpty() && aPolygon1 == &(*aPolyStack.First()))
      {
        aPolyStack.Remove(1);
        aPolyBoxStack.Remove(1);
      }

      if (aPolyStack.IsEmpty())
        break;

      aPolygon1   = &(*aPolyStack.ChangeFirst());
      aPolyBoxes1 = &(*aPolyBoxStack.ChangeFirst());
    }
  }
}

//=======================================================================
// function : meshElementaryPolygon
// purpose  : Triangulation of closed polygon containing only three edges.
//=======================================================================
bool BRepMesh_Delaun::meshElementaryPolygon(
  const IMeshData::SequenceOfInteger& thePolygon)
{
  int aPolyLen = thePolygon.Length();
  if (aPolyLen < 3)
    return true;
  else if (aPolyLen > 3)
    return false;

  // Just create a triangle
  int anEdges[3];
  bool anEdgesOri[3];

  for (int anEdgeIt = 0; anEdgeIt < 3; ++anEdgeIt)
  {
    int anEdgeInfo = thePolygon(anEdgeIt + 1);
    anEdges[anEdgeIt]           = std::abs(anEdgeInfo);
    anEdgesOri[anEdgeIt]        = (anEdgeInfo > 0);
  }

  const BRepMesh_Edge& anEdge1 = GetEdge(anEdges[0]);
  const BRepMesh_Edge& anEdge2 = GetEdge(anEdges[1]);

  int aNodes[3] = {anEdge1.FirstNode(), anEdge1.LastNode(), anEdge2.FirstNode()};
  if (aNodes[2] == aNodes[0] || aNodes[2] == aNodes[1])
  {
    aNodes[2] = anEdge2.LastNode();
  }

  addTriangle(anEdges, anEdgesOri, aNodes);
  return true;
}

//=================================================================================================

void BRepMesh_Delaun::decomposeSimplePolygon(IMeshData::SequenceOfInteger& thePolygon,
                                             IMeshData::SequenceOfBndB2d&  thePolyBoxes,
                                             IMeshData::SequenceOfInteger& thePolygonCut,
                                             IMeshData::SequenceOfBndB2d&  thePolyBoxesCut)
{
  // Check is the given polygon elementary
  if (meshElementaryPolygon(thePolygon))
  {
    thePolygon.Clear();
    thePolyBoxes.Clear();
    return;
  }

  // Polygon contains more than 3 links
  int     aFirstEdgeInfo = thePolygon(1);
  const BRepMesh_Edge& aFirstEdge     = GetEdge(std::abs(aFirstEdgeInfo));

  int aNodes[3];
  getOrientedNodes(aFirstEdge, aFirstEdgeInfo > 0, aNodes);

  gp_Pnt2d aRefVertices[3];
  aRefVertices[0] = GetVertex(aNodes[0]).Coord();
  aRefVertices[1] = GetVertex(aNodes[1]).Coord();

  gp_Vec2d aRefEdgeDir(aRefVertices[0], aRefVertices[1]);

  double aRefEdgeLen = aRefEdgeDir.Magnitude();
  if (aRefEdgeLen < Precision)
  {
    thePolygon.Clear();
    thePolyBoxes.Clear();
    return;
  }

  aRefEdgeDir /= aRefEdgeLen;

  // Find a point with minimum distance respect
  // the end of reference link
  int aUsedLinkId = 0;
  double    aOptAngle   = 0.0;
  double    aMinDist    = RealLast();
  int aPivotNode  = aNodes[1];
  int aPolyLen    = thePolygon.Length();
  for (int aLinkIt = 3; aLinkIt <= aPolyLen; ++aLinkIt)
  {
    int     aLinkInfo = thePolygon(aLinkIt);
    const BRepMesh_Edge& aNextEdge = GetEdge(std::abs(aLinkInfo));

    aPivotNode = aLinkInfo > 0 ? aNextEdge.FirstNode() : aNextEdge.LastNode();

    // We have end points touch case in the polygon - ignore it
    if (aPivotNode == aNodes[1])
      continue;

    gp_Pnt2d aPivotVertex = GetVertex(aPivotNode).Coord();
    gp_Vec2d aDistanceDir(aRefVertices[1], aPivotVertex);

    double aDist     = aRefEdgeDir ^ aDistanceDir;
    double aAngle    = std::abs(aRefEdgeDir.Angle(aDistanceDir));
    double anAbsDist = std::abs(aDist);
    if (anAbsDist < Precision || aDist < 0.)
      continue;

    if ((anAbsDist >= aMinDist) && (aAngle <= aOptAngle || aAngle > AngDeviation90Deg))
    {
      continue;
    }

    // Check is the test link crosses the polygon boundaries
    bool isIntersect = false;
    for (int aRefLinkNodeIt = 0; aRefLinkNodeIt < 2; ++aRefLinkNodeIt)
    {
      const int& aLinkFirstNode   = aNodes[aRefLinkNodeIt];
      const gp_Pnt2d&         aLinkFirstVertex = aRefVertices[aRefLinkNodeIt];

      Bnd_B2d aBox;
      UpdateBndBox(aLinkFirstVertex.Coord(), aPivotVertex.Coord(), aBox);

      BRepMesh_Edge aCheckLink(aLinkFirstNode, aPivotNode, BRepMesh_Free);

      int aCheckLinkIt = 2;
      for (; aCheckLinkIt <= aPolyLen; ++aCheckLinkIt)
      {
        if (aCheckLinkIt == aLinkIt)
          continue;

        if (!aBox.IsOut(thePolyBoxes.Value(aCheckLinkIt)))
        {
          const BRepMesh_Edge& aPolyLink = GetEdge(std::abs(thePolygon(aCheckLinkIt)));

          if (aCheckLink.IsEqual(aPolyLink))
            continue;

          // intersection is possible...
          gp_Pnt2d                   anIntPnt;
          BRepMesh_GeomTool::IntFlag aIntFlag =
            intSegSeg(aCheckLink, aPolyLink, false, false, anIntPnt);

          if (aIntFlag != BRepMesh_GeomTool::NoIntersection)
          {
            isIntersect = true;
            break;
          }
        }
      }

      if (isIntersect)
        break;
    }

    if (isIntersect)
      continue;

    aOptAngle       = aAngle;
    aMinDist        = anAbsDist;
    aNodes[2]       = aPivotNode;
    aRefVertices[2] = aPivotVertex;
    aUsedLinkId     = aLinkIt;
  }

  if (aUsedLinkId == 0)
  {
    thePolygon.Clear();
    thePolyBoxes.Clear();
    return;
  }

  BRepMesh_Edge aNewEdges[2] = {BRepMesh_Edge(aNodes[1], aNodes[2], BRepMesh_Free),
                                BRepMesh_Edge(aNodes[2], aNodes[0], BRepMesh_Free)};

  int aNewEdgesInfo[3] = {aFirstEdgeInfo,
                                       myMeshData->AddLink(aNewEdges[0]),
                                       myMeshData->AddLink(aNewEdges[1])};

  int anEdges[3];
  bool anEdgesOri[3];
  for (int aTriEdgeIt = 0; aTriEdgeIt < 3; ++aTriEdgeIt)
  {
    const int& anEdgeInfo = aNewEdgesInfo[aTriEdgeIt];
    anEdges[aTriEdgeIt]                = std::abs(anEdgeInfo);
    anEdgesOri[aTriEdgeIt]             = anEdgeInfo > 0;
  }
  addTriangle(anEdges, anEdgesOri, aNodes);

  if (aUsedLinkId == 3)
  {
    thePolygon.Remove(1);
    thePolyBoxes.Remove(1);

    thePolygon.SetValue(1, -aNewEdgesInfo[2]);

    Bnd_B2d aBox;
    UpdateBndBox(aRefVertices[0].Coord(), aRefVertices[2].Coord(), aBox);
    thePolyBoxes.SetValue(1, aBox);
  }
  else
  {
    // Create triangle and split the source polygon on two
    // parts (if possible) and mesh each part as independent
    // polygon.
    if (aUsedLinkId < aPolyLen)
    {
      thePolygon.Split(aUsedLinkId, thePolygonCut);
      thePolygonCut.Prepend(-aNewEdgesInfo[2]);
      thePolyBoxes.Split(aUsedLinkId, thePolyBoxesCut);

      Bnd_B2d aBox;
      UpdateBndBox(aRefVertices[0].Coord(), aRefVertices[2].Coord(), aBox);
      thePolyBoxesCut.Prepend(aBox);
    }
    else
    {
      thePolygon.Remove(aPolyLen);
      thePolyBoxes.Remove(aPolyLen);
    }

    thePolygon.SetValue(1, -aNewEdgesInfo[1]);

    Bnd_B2d aBox;
    UpdateBndBox(aRefVertices[1].Coord(), aRefVertices[2].Coord(), aBox);
    thePolyBoxes.SetValue(1, aBox);
  }
}

//=======================================================================
// function : RemoveVertex
// purpose  : Removes a vertex from the triangulation
//=======================================================================
void BRepMesh_Delaun::RemoveVertex(const BRepMesh_Vertex& theVertex)
{
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector(myMeshData);
  aSelector.NeighboursOf(theVertex);

  IMeshData::MapOfIntegerInteger aLoopEdges; //( 10, myMeshData->Allocator() );

  // Loop on triangles to be destroyed :
  IMeshData::IteratorOfMapOfInteger aTriangleIt(aSelector.Elements());
  for (; aTriangleIt.More(); aTriangleIt.Next())
    deleteTriangle(aTriangleIt.Key(), aLoopEdges);

  IMeshData::SequenceOfBndB2d              aBoxes;
  IMeshData::SequenceOfInteger             aPolygon;
  int                         aLoopEdgesCount = aLoopEdges.Extent();
  IMeshData::MapOfIntegerInteger::Iterator aLoopEdgesIt(aLoopEdges);

  if (aLoopEdgesIt.More())
  {
    const BRepMesh_Edge& anEdge     = GetEdge(aLoopEdgesIt.Key());
    int     aFirstNode = anEdge.FirstNode();
    int     aLastNode;
    int     aPivotNode = anEdge.LastNode();
    int     anEdgeId   = aLoopEdgesIt.Key();

    bool isPositive = aLoopEdges(anEdgeId) != 0;
    if (!isPositive)
    {
      int aTmp;
      aTmp       = aFirstNode;
      aFirstNode = aPivotNode;
      aPivotNode = aTmp;

      aPolygon.Append(-anEdgeId);
    }
    else
      aPolygon.Append(anEdgeId);

    fillBndBox(aBoxes, GetVertex(aFirstNode), GetVertex(aPivotNode));

    aLoopEdges.UnBind(anEdgeId);

    aLastNode = aFirstNode;
    while (aPivotNode != aLastNode)
    {
      IMeshData::ListOfInteger::Iterator aLinkIt(myMeshData->LinksConnectedTo(aPivotNode));
      for (; aLinkIt.More(); aLinkIt.Next())
      {
        if (aLinkIt.Value() != anEdgeId && aLoopEdges.IsBound(aLinkIt.Value()))
        {
          int aCurrentNode;
          anEdgeId                     = aLinkIt.Value();
          const BRepMesh_Edge& anEdge1 = GetEdge(anEdgeId);

          aCurrentNode = anEdge1.LastNode();
          if (aCurrentNode != aPivotNode)
          {
            aCurrentNode = anEdge1.FirstNode();
            aPolygon.Append(-anEdgeId);
          }
          else
            aPolygon.Append(anEdgeId);

          fillBndBox(aBoxes, GetVertex(aCurrentNode), GetVertex(aPivotNode));

          aPivotNode = aCurrentNode;
          aLoopEdges.UnBind(anEdgeId);
          break;
        }
      }

      if (aLoopEdgesCount <= 0)
        break;
      --aLoopEdgesCount;
    }

    meshPolygon(aPolygon, aBoxes);
  }
}

//=======================================================================
// function : AddVertices
// purpose  : Adds some vertices in the triangulation.
//=======================================================================
void BRepMesh_Delaun::AddVertices(IMeshData::VectorOfInteger&  theVertices,
                                  const Message_ProgressRange& theRange)
{
  ComparatorOfIndexedVertexOfDelaun aCmp(myMeshData);
  std::make_heap(theVertices.begin(), theVertices.end(), aCmp);
  std::sort_heap(theVertices.begin(), theVertices.end(), aCmp);

  createTrianglesOnNewVertices(theVertices, theRange);
}

//=======================================================================
// function : UseEdge
// purpose  : Modify mesh to use the edge. Return True if done
//=======================================================================
bool BRepMesh_Delaun::UseEdge(const int /*theIndex*/)
{
  /*
  const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( theIndex );
  if ( aPair.Extent() == 0 )
  {
    const BRepMesh_Edge& anEdge = GetEdge( theIndex );

    int aStartNode, aPivotNode, anOtherNode;
    aStartNode = anEdge.FirstNode();
    aPivotNode = anEdge.LastNode();

    const BRepMesh_ListOfInteger& aStartNodeNeighbors = myMeshData->LinkNeighboursOf( aStartNode );
    const BRepMesh_ListOfInteger& aPivotNodeNeighbors = myMeshData->LinkNeighboursOf( aPivotNode );

    if ( aStartNodeNeighbors.Extent() > 0 &&
         aPivotNodeNeighbors.Extent() > 0 )
    {
      const BRepMesh_Vertex& aStartVertex = GetVertex( aStartNode );
      const BRepMesh_Vertex& aPivotVertex = GetVertex( aPivotNode );

      gp_XY aVEdge   ( aPivotVertex.Coord() );
      aVEdge.Subtract( aStartVertex.Coord() );

      double    anAngle    = 0.;
      double    anAngleMin = RealLast();
      double    anAngleMax = RealFirst();
      int aLeftEdge  = 0, aRightEdge = 0;

      BRepMesh_ListOfInteger::Iterator aNeighborIt( aPivotNodeNeighbors );
      for ( ; aNeighborIt.More(); aNeighborIt.Next() )
      {
        int anEdgeId = aNeighborIt.Value();
        if ( anEdgeId != theIndex )
        {
          const BRepMesh_Edge& aNextEdge = GetEdge( anEdgeId );

          bool isInMesh = true;
          if ( aNextEdge.Movability() == BRepMesh_Free )
          {
            if ( myMeshData->ElemConnectedTo( anEdgeId ).IsEmpty() )
              isInMesh = false;
          }

          if ( isInMesh )
          {
            anOtherNode = aNextEdge.FirstNode();
            if ( anOtherNode == aPivotNode )
              anOtherNode = aNextEdge.LastNode();

            gp_XY aVEdgeCur = GetVertex( anOtherNode ).Coord();
            aVEdgeCur.Subtract( aPivotVertex.Coord() );

            anAngle = gp_Vec2d( aVEdge ).Angle( gp_Vec2d( aVEdgeCur ) );
          }

          if ( anAngle > anAngleMax )
          {
            anAngleMax = anAngle;
            aLeftEdge  = anEdgeId;
          }
          if ( anAngle < anAngleMin )
          {
            anAngleMin = anAngle;
            aRightEdge = anEdgeId;
          }
        }
      }

      if ( aLeftEdge > 0 )
      {
        if (aLeftEdge==aRightEdge)
        {
        }
        else
        {
        }
      }
    }
  }
  */
  return false;
}

//=======================================================================
// function : getEdgesByType
// purpose  : Gives the list of edges with type defined by input parameter
//=======================================================================
Handle(IMeshData::MapOfInteger) BRepMesh_Delaun::getEdgesByType(
  const BRepMesh_DegreeOfFreedom theEdgeType) const
{
  Handle(IMeshData::MapOfInteger)   aResult = new IMeshData::MapOfInteger;
  IMeshData::IteratorOfMapOfInteger anEdgeIt(myMeshData->LinksOfDomain());

  for (; anEdgeIt.More(); anEdgeIt.Next())
  {
    int anEdge  = anEdgeIt.Key();
    bool isToAdd = (theEdgeType == BRepMesh_Free)
                                 ? (myMeshData->ElementsConnectedTo(anEdge).Extent() <= 1)
                                 : (GetEdge(anEdge).Movability() == theEdgeType);

    if (isToAdd)
      aResult->Add(anEdge);
  }

  return aResult;
}

//=======================================================================
// function : calculateDist
// purpose  : Calculates distances between the given point and edges of
//           triangle
//=======================================================================
double BRepMesh_Delaun::calculateDist(const gp_XY            theVEdges[3],
                                             const gp_XY            thePoints[3],
                                             const BRepMesh_Vertex& theVertex,
                                             double          theDistance[3],
                                             double          theSqModulus[3],
                                             int&      theEdgeOn) const
{
  double aMinDist = RealLast();
  for (int i = 0; i < 3; ++i)
  {
    theSqModulus[i] = theVEdges[i].SquareModulus();
    if (theSqModulus[i] <= Precision2)
      return -1;

    theDistance[i] = theVEdges[i] ^ (theVertex.Coord() - thePoints[i]);

    double aDist = theDistance[i] * theDistance[i];
    aDist /= theSqModulus[i];

    if (aDist < aMinDist)
    {
      theEdgeOn = i;
      aMinDist  = aDist;
    }
  }

  return aMinDist;
}

//=======================================================================
// function : Contains
// purpose  : Test if triangle of index <TrianIndex> contains geometricaly
//           <theVertex>. If <theEdgeOn> is != 0 then theVertex is on Edge
//           of  index <theEdgeOn>
//=======================================================================
bool BRepMesh_Delaun::Contains(const int theTriangleId,
                                           const BRepMesh_Vertex& theVertex,
                                           const double    theSqTolerance,
                                           int&      theEdgeOn) const
{
  theEdgeOn = 0;

  int p[3];

  const BRepMesh_Triangle& aElement = GetTriangle(theTriangleId);
  const int(&e)[3]     = aElement.myEdges;

  const BRepMesh_Edge* anEdges[3] = {&GetEdge(e[0]), &GetEdge(e[1]), &GetEdge(e[2])};

  myMeshData->ElementNodes(aElement, p);

  gp_XY aPoints[3];
  aPoints[0] = GetVertex(p[0]).Coord();
  aPoints[1] = GetVertex(p[1]).Coord();
  aPoints[2] = GetVertex(p[2]).Coord();

  gp_XY aVEdges[3];
  aVEdges[0] = aPoints[1];
  aVEdges[0].Subtract(aPoints[0]);

  aVEdges[1] = aPoints[2];
  aVEdges[1].Subtract(aPoints[1]);

  aVEdges[2] = aPoints[0];
  aVEdges[2].Subtract(aPoints[2]);

  double aDistance[3];
  double aSqModulus[3];

  double    aSqMinDist;
  int aEdgeOnId;
  aSqMinDist = calculateDist(aVEdges, aPoints, theVertex, aDistance, aSqModulus, aEdgeOnId);
  if (aSqMinDist < 0)
    return false;

  const bool isNotFree = (anEdges[aEdgeOnId]->Movability() != BRepMesh_Free);
  if (aSqMinDist > theSqTolerance)
  {
    if (isNotFree && aDistance[aEdgeOnId] < (aSqModulus[aEdgeOnId] / 5.))
      theEdgeOn = e[aEdgeOnId];
  }
  else if (isNotFree)
    return false;
  else
    theEdgeOn = e[aEdgeOnId];

  return (aDistance[0] >= 0. && aDistance[1] >= 0. && aDistance[2] >= 0.);
}

//=============================================================================
// function : intSegSeg
// purpose  : Checks intersection between the two segments.
//=============================================================================
BRepMesh_GeomTool::IntFlag BRepMesh_Delaun::intSegSeg(
  const BRepMesh_Edge&   theEdg1,
  const BRepMesh_Edge&   theEdg2,
  const bool isConsiderEndPointTouch,
  const bool isConsiderPointOnEdge,
  gp_Pnt2d&              theIntPnt) const
{
  gp_XY p1, p2, p3, p4;
  p1 = GetVertex(theEdg1.FirstNode()).Coord();
  p2 = GetVertex(theEdg1.LastNode()).Coord();
  p3 = GetVertex(theEdg2.FirstNode()).Coord();
  p4 = GetVertex(theEdg2.LastNode()).Coord();

  return BRepMesh_GeomTool::IntSegSeg(p1,
                                      p2,
                                      p3,
                                      p4,
                                      isConsiderEndPointTouch,
                                      isConsiderPointOnEdge,
                                      theIntPnt);
}

//=============================================================================
// function : polyArea
// purpose  : Returns area of the loop of the given polygon defined by indices
//           of its start and end links.
//=============================================================================
double BRepMesh_Delaun::polyArea(const IMeshData::SequenceOfInteger& thePolygon,
                                        const int              theStartIndex,
                                        const int              theEndIndex) const
{
  double    aArea    = 0.0;
  int aPolyLen = thePolygon.Length();
  if (theStartIndex >= theEndIndex || theStartIndex > aPolyLen)
  {
    return aArea;
  }
  int     aCurEdgeInfo = thePolygon(theStartIndex);
  int     aCurEdgeId   = std::abs(aCurEdgeInfo);
  const BRepMesh_Edge* aCurEdge     = &GetEdge(aCurEdgeId);

  int aNodes[2];
  getOrientedNodes(*aCurEdge, aCurEdgeInfo > 0, aNodes);

  gp_Pnt2d         aRefPnt = GetVertex(aNodes[0]).Coord();
  int aPolyIt = theStartIndex + 1;
  for (; aPolyIt <= theEndIndex; ++aPolyIt)
  {
    aCurEdgeInfo = thePolygon(aPolyIt);
    aCurEdgeId   = std::abs(aCurEdgeInfo);
    aCurEdge     = &GetEdge(aCurEdgeId);

    getOrientedNodes(*aCurEdge, aCurEdgeInfo > 0, aNodes);
    gp_Vec2d aVec1(aRefPnt, GetVertex(aNodes[0]).Coord());
    gp_Vec2d aVec2(aRefPnt, GetVertex(aNodes[1]).Coord());

    aArea += aVec1 ^ aVec2;
  }

  return aArea / 2.;
}

#ifdef OCCT_DEBUG
//=================================================================================================

  #include <TopoDS_Compound.hxx>
  #include <BRep_Builder.hxx>
  #include <Standard_ErrorHandler.hxx>
  #include <BRepBuilderAPI_MakeEdge.hxx>
  #include <BRepTools.hxx>

const char* BRepMesh_DumpPoly(void*            thePolygon,
                                   void*            theMeshHandlePtr,
                                   const char* theFileNameStr)
{
  if (thePolygon == 0 || theFileNameStr == 0)
  {
    return "Error: file name or polygon data is null";
  }

  IMeshData::SequenceOfInteger& aPolygon = *(IMeshData::SequenceOfInteger*)thePolygon;

  occ::handle<BRepMesh_DataStructureOfDelaun> aMeshData =
    *(occ::handle<BRepMesh_DataStructureOfDelaun>*)theMeshHandlePtr;

  if (aMeshData.IsNull())
    return "Error: mesh data is empty";

  TopoDS_Compound aMesh;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aMesh);

  try
  {
    OCC_CATCH_SIGNALS

    IMeshData::SequenceOfInteger::Iterator aLinksIt(aPolygon);
    for (; aLinksIt.More(); aLinksIt.Next())
    {
      const BRepMesh_Edge& aLink = aMeshData->GetLink(std::abs(aLinksIt.Value()));

      gp_Pnt aPnt[2];
      for (int i = 0; i < 2; ++i)
      {
        const int aNodeId = (i == 0) ? aLink.FirstNode() : aLink.LastNode();

        const gp_XY& aNode = aMeshData->GetNode(aNodeId).Coord();
        aPnt[i]            = gp_Pnt(aNode.X(), aNode.Y(), 0.);
      }

      if (aPnt[0].SquareDistance(aPnt[1]) < Precision::SquareConfusion())
        continue;

      aBuilder.Add(aMesh, BRepBuilderAPI_MakeEdge(aPnt[0], aPnt[1]));
    }

    if (!BRepTools::Write(aMesh, theFileNameStr))
      return "Error: write failed";
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }

  return theFileNameStr;
}
#endif
