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

#include <gp.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <Precision.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_B2d.hxx>

#include <BRepMesh_SelectorOfDataStructureOfDelaun.hxx>

#include <BRepMesh_Edge.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_Triangle.hxx>

#include <NCollection_Vector.hxx>

#include <algorithm>

const Standard_Real AngDeviation1Deg  = M_PI/180.;
const Standard_Real AngDeviation90Deg = 90 * AngDeviation1Deg;
const Standard_Real Angle2PI          = 2 * M_PI;

const Standard_Real Precision    = Precision::PConfusion();
const Standard_Real Precision2   = Precision * Precision;

namespace {
  //! Sort two points in projection on vector (1,1)
  struct ComparatorOfVertexOfDelaun 
  {
    bool operator() (const BRepMesh_Vertex& theLeft, const BRepMesh_Vertex& theRight)
    {
      return theLeft.Coord().X() + theLeft.Coord().Y() < theRight.Coord().X() + theRight.Coord().Y();
    }
  };

  //! Sort two points in projection on vector (1,1)
  struct ComparatorOfIndexedVertexOfDelaun
  {
  public:
    ComparatorOfIndexedVertexOfDelaun (const Handle(BRepMesh_DataStructureOfDelaun)& theDS)
      : myStructure(theDS) {}
  
    bool operator() (Standard_Integer theLeft, Standard_Integer theRight)
    {
      const BRepMesh_Vertex& aLeft  = myStructure->GetNode(theLeft);
      const BRepMesh_Vertex& aRight = myStructure->GetNode(theRight);
      return ComparatorOfVertexOfDelaun() (aLeft, aRight);
    }

  private:
    Handle(BRepMesh_DataStructureOfDelaun) myStructure;
  };

  inline void UpdateBndBox(const gp_XY& thePnt1, const gp_XY& thePnt2, Bnd_B2d& theBox)
  {
    theBox.Add( thePnt1 );
    theBox.Add( thePnt2 );
    theBox.Enlarge(Precision);
  }
} // anonymous namespace

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with an empty Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun(BRepMesh::Array1OfVertexOfDelaun& theVertices)
: myCircles (theVertices.Length(), new NCollection_IncAllocator(
             BRepMesh::MEMORY_BLOCK_SIZE_HUGE))
{
  if ( theVertices.Length() > 2 )
  {
    myMeshData = new BRepMesh_DataStructureOfDelaun(
      new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE),
      theVertices.Length() );
    Init( theVertices );
  }
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun(
  const Handle( BRepMesh_DataStructureOfDelaun )& theOldMesh,
  BRepMesh::Array1OfVertexOfDelaun&               theVertices)
: myMeshData( theOldMesh ),
  myCircles ( theVertices.Length(), theOldMesh->Allocator() )
{
  if ( theVertices.Length() > 2 )
    Init( theVertices );
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun(
  const Handle( BRepMesh_DataStructureOfDelaun )& theOldMesh, 
  BRepMesh::Array1OfInteger&                      theVertexIndices)
: myMeshData( theOldMesh ),
  myCircles ( theVertexIndices.Length(), theOldMesh->Allocator() )
{
  if ( theVertexIndices.Length() > 2 )
  {
    Bnd_Box2d aBox;
    Standard_Integer anIndex = theVertexIndices.Lower();
    Standard_Integer anUpper = theVertexIndices.Upper();
    for ( ; anIndex <= anUpper; ++anIndex )
      aBox.Add( gp_Pnt2d( GetVertex( theVertexIndices( anIndex) ).Coord() ) );

    perform( aBox, theVertexIndices );
  }
}

//=======================================================================
//function : Init
//purpose  : Initializes the triangulation with an Array of Vertex
//=======================================================================
void BRepMesh_Delaun::Init(BRepMesh::Array1OfVertexOfDelaun& theVertices)
{
  Bnd_Box2d aBox;
  Standard_Integer aLowerIdx  = theVertices.Lower();
  Standard_Integer anUpperIdx = theVertices.Upper();
  BRepMesh::Array1OfInteger aVertexIndexes( aLowerIdx, anUpperIdx );
  
  Standard_Integer anIndex = aLowerIdx;
  for ( ; anIndex <= anUpperIdx; ++anIndex )
  {
    aBox.Add( gp_Pnt2d( theVertices( anIndex ).Coord() ) );
    aVertexIndexes( anIndex ) = myMeshData->AddNode( theVertices( anIndex ) );
  }

  perform( aBox, aVertexIndexes );
}

//=======================================================================
//function : perform
//purpose  : Create super mesh and run triangulation procedure
//=======================================================================
void BRepMesh_Delaun::perform(Bnd_Box2d&                 theBndBox,
                              BRepMesh::Array1OfInteger& theVertexIndexes)
{
  theBndBox.Enlarge( Precision );
  superMesh( theBndBox );

  ComparatorOfIndexedVertexOfDelaun aCmp(myMeshData);
  std::make_heap(theVertexIndexes.begin(), theVertexIndexes.end(), aCmp);
  std::sort_heap(theVertexIndexes.begin(), theVertexIndexes.end(), aCmp);

  compute( theVertexIndexes );
}

//=======================================================================
//function : superMesh
//purpose  : Build the super mesh
//=======================================================================
void BRepMesh_Delaun::superMesh( const Bnd_Box2d& theBox )
{
  Standard_Real aMinX, aMinY, aMaxX, aMaxY;
  theBox.Get( aMinX, aMinY, aMaxX, aMaxY );
  Standard_Real aDeltaX = aMaxX - aMinX;
  Standard_Real aDeltaY = aMaxY - aMinY;

  Standard_Real aDeltaMin = Min( aDeltaX, aDeltaY );
  Standard_Real aDeltaMax = Max( aDeltaX, aDeltaY );
  Standard_Real aDelta    = aDeltaX + aDeltaY;
  
  myCircles.SetMinMaxSize( gp_XY( aMinX, aMinY ), gp_XY( aMaxX, aMaxY ) );

  Standard_Integer aScaler = 2;
  if ( myMeshData->NbNodes() > 100 )
    aScaler = 5;
  else if( myMeshData->NbNodes() > 1000 )
    aScaler = 7;

  myCircles.SetCellSize( aDeltaX / aScaler,
                         aDeltaY / aScaler );

  mySupVert[0] = myMeshData->AddNode(
    BRepMesh_Vertex( ( aMinX + aMaxX ) / 2, aMaxY + aDeltaMax, BRepMesh_Free ) );
    
  mySupVert[1] = myMeshData->AddNode(
    BRepMesh_Vertex( aMinX - aDelta, aMinY - aDeltaMin, BRepMesh_Free ) );
    
  mySupVert[2] = myMeshData->AddNode(
    BRepMesh_Vertex( aMaxX + aDelta, aMinY - aDeltaMin, BRepMesh_Free ) );

  Standard_Integer e[3];
  Standard_Boolean o[3];
  for (Standard_Integer aNodeId = 0; aNodeId < 3; ++aNodeId)
  {
    Standard_Integer aFirstNode = aNodeId;
    Standard_Integer aLastNode  = (aNodeId + 1) % 3;
    Standard_Integer aLinkIndex = myMeshData->AddLink( BRepMesh_Edge( 
      mySupVert[aFirstNode], mySupVert[aLastNode], BRepMesh_Free ) );

    e[aNodeId] = Abs(aLinkIndex);
    o[aNodeId] = (aLinkIndex > 0);
  }
  
  mySupTrian = BRepMesh_Triangle(e, o, BRepMesh_Free);
}

//=======================================================================
//function : deleteTriangle
//purpose  : Deletes the triangle with the given index and adds the free
//           edges into the map.
//           When an edge is suppressed more than one time it is destroyed.
//=======================================================================
void BRepMesh_Delaun::deleteTriangle(const Standard_Integer         theIndex, 
                                     BRepMesh::MapOfIntegerInteger& theLoopEdges )
{
  myCircles.Delete( theIndex );

  Standard_Integer e[3];
  Standard_Boolean o[3];
  GetTriangle( theIndex ).Edges( e, o );
  
  myMeshData->RemoveElement( theIndex );

  for ( Standard_Integer i = 0; i < 3; ++i )
  {
    if ( !theLoopEdges.Bind( e[i], o[i] ) ) 
    {
      theLoopEdges.UnBind( e[i] );
      myMeshData->RemoveLink( e[i] );
    }
  }
}

//=======================================================================
//function : compute
//purpose  : Computes the triangulation and add the vertices edges and 
//           triangles to the Mesh data structure
//=======================================================================
void BRepMesh_Delaun::compute(BRepMesh::Array1OfInteger& theVertexIndexes)
{
  // Insertion of edges of super triangles in the list of free edges: 
  BRepMesh::MapOfIntegerInteger aLoopEdges(10, myMeshData->Allocator());
  Standard_Integer e[3];
  Standard_Boolean o[3];
  mySupTrian.Edges( e, o );
                    
  aLoopEdges.Bind( e[0], Standard_True );
  aLoopEdges.Bind( e[1], Standard_True );
  aLoopEdges.Bind( e[2], Standard_True );

  if ( theVertexIndexes.Length() > 0 )
  {
    // Creation of 3 trianglers with the first node and the edges of the super triangle:
    Standard_Integer anVertexIdx = theVertexIndexes.Lower();
    createTriangles( theVertexIndexes( anVertexIdx ), aLoopEdges );

    // Add other nodes to the mesh
    createTrianglesOnNewVertices( theVertexIndexes );
  }

  // Destruction of triangles containing a top of the super triangle
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector( myMeshData );
  for (Standard_Integer aSupVertId = 0; aSupVertId < 3; ++aSupVertId)
    aSelector.NeighboursOfNode( mySupVert[aSupVertId] );
  
  aLoopEdges.Clear();
  BRepMesh::MapOfInteger::Iterator aFreeTriangles( aSelector.Elements() );
  for ( ; aFreeTriangles.More(); aFreeTriangles.Next() )
    deleteTriangle( aFreeTriangles.Key(), aLoopEdges );

  // All edges that remain free are removed from aLoopEdges;
  // only the boundary edges of the triangulation remain there
  BRepMesh::MapOfIntegerInteger::Iterator aFreeEdges( aLoopEdges );
  for ( ; aFreeEdges.More(); aFreeEdges.Next() )
  {
    if ( myMeshData->ElementsConnectedTo( aFreeEdges.Key() ).IsEmpty() )
      myMeshData->RemoveLink( aFreeEdges.Key() );
  }

  // The tops of the super triangle are destroyed
  for (Standard_Integer aSupVertId = 0; aSupVertId < 3; ++aSupVertId)
    myMeshData->RemoveNode( mySupVert[aSupVertId] );
}

//=======================================================================
//function : createTriangles
//purpose  : Creates the triangles beetween the node and the polyline.
//=======================================================================
void BRepMesh_Delaun::createTriangles(const Standard_Integer         theVertexIndex,  
                                      BRepMesh::MapOfIntegerInteger& thePoly)
{
  BRepMesh::ListOfInteger aLoopEdges, anExternalEdges;
  const gp_XY& aVertexCoord = myMeshData->GetNode( theVertexIndex ).Coord();
  
  BRepMesh::MapOfIntegerInteger::Iterator anEdges( thePoly );
  for ( ; anEdges.More(); anEdges.Next() )
  {
    Standard_Integer     anEdgeId = anEdges.Key();
    const BRepMesh_Edge& anEdge   = GetEdge( anEdgeId );

    const Standard_Boolean isPositive = thePoly (anEdgeId) != 0;

    Standard_Integer aNodes[3];
    if ( isPositive )
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

    const BRepMesh_Vertex& aFirstVertex = GetVertex( aNodes[0] );
    const BRepMesh_Vertex& aLastVertex  = GetVertex( aNodes[2] );

    gp_XY anEdgeDir( aLastVertex.Coord() - aFirstVertex.Coord() );
    Standard_Real anEdgeLen = anEdgeDir.Modulus();
    if ( anEdgeLen < Precision )
      continue;

    anEdgeDir.SetCoord( anEdgeDir.X() / anEdgeLen,
                        anEdgeDir.Y() / anEdgeLen );

    gp_XY aFirstLinkDir( aFirstVertex.Coord() - aVertexCoord );
    gp_XY aLastLinkDir ( aVertexCoord         - aLastVertex.Coord() );
                      
    Standard_Real aDist12 = aFirstLinkDir ^ anEdgeDir;
    Standard_Real aDist23 = anEdgeDir     ^ aLastLinkDir;
    if ( Abs( aDist12 ) < Precision || 
         Abs( aDist23 ) < Precision )
    {
      continue;
    }

    BRepMesh_Edge aFirstLink( aNodes[1], aNodes[0], BRepMesh_Free );
    BRepMesh_Edge aLastLink ( aNodes[2], aNodes[1], BRepMesh_Free );

    Standard_Integer anEdgesInfo[3] = {
      myMeshData->AddLink( aFirstLink ),
      isPositive ? anEdgeId : -anEdgeId,
      myMeshData->AddLink( aLastLink ) };

    Standard_Boolean isSensOK = (aDist12 > 0. && aDist23 > 0.);
    if (isSensOK)
    {
      Standard_Integer anEdgeIds[3];
      Standard_Boolean anEdgesOri[3];
      for ( Standard_Integer aTriLinkIt = 0; aTriLinkIt < 3; ++aTriLinkIt )
      {
        const Standard_Integer& anEdgeInfo = anEdgesInfo[aTriLinkIt];
        anEdgeIds[aTriLinkIt]    = Abs( anEdgeInfo );
        anEdgesOri[aTriLinkIt] = anEdgeInfo > 0;
      }

      addTriangle(anEdgeIds, anEdgesOri, aNodes );
    }
    else
    {
      if ( isPositive )
        aLoopEdges.Append(  anEdges.Key() );
      else
        aLoopEdges.Append( -anEdges.Key() );
        
      if ( aFirstLinkDir.SquareModulus() > aLastLinkDir.SquareModulus() )
        anExternalEdges.Append( Abs( anEdgesInfo[0] ) );
      else
        anExternalEdges.Append( Abs( anEdgesInfo[2] ) );
    }
  }
  
  thePoly.Clear();
  while ( !anExternalEdges.IsEmpty() )
  {
    const BRepMesh_PairOfIndex& aPair = 
      myMeshData->ElementsConnectedTo( Abs( anExternalEdges.First() ) );
    
    
    if ( !aPair.IsEmpty() )
      deleteTriangle( aPair.FirstIndex(), thePoly );
      
    anExternalEdges.RemoveFirst();
  }

  for ( anEdges.Initialize( thePoly ); anEdges.More(); anEdges.Next() )
  {
    if ( myMeshData->ElementsConnectedTo( anEdges.Key() ).IsEmpty() )
      myMeshData->RemoveLink( anEdges.Key() );
  }

  while ( !aLoopEdges.IsEmpty() )
  {
    const BRepMesh_Edge& anEdge = GetEdge( Abs( aLoopEdges.First() ) );
    if ( anEdge.Movability() != BRepMesh_Deleted )
    {
      Standard_Integer anEdgeIdx = aLoopEdges.First();
      meshLeftPolygonOf( Abs( anEdgeIdx ), ( anEdgeIdx > 0 ) );
    }
      
    aLoopEdges.RemoveFirst();
  }
}

//=======================================================================
//function : createTrianglesOnNewVertices
//purpose  : Creation of triangles from the new nodes
//=======================================================================
void BRepMesh_Delaun::createTrianglesOnNewVertices(
  BRepMesh::Array1OfInteger& theVertexIndexes)
{
  Handle(NCollection_IncAllocator) aAllocator =
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);

  Standard_Real aTolU, aTolV;
  myMeshData->Data()->GetTolerance(aTolU, aTolV);
  const Standard_Real aSqTol = aTolU * aTolU + aTolV * aTolV;

  // Insertion of nodes :
  Standard_Boolean isModify = Standard_True;
  
  Standard_Integer anIndex = theVertexIndexes.Lower();
  Standard_Integer anUpper = theVertexIndexes.Upper();
  for( ; anIndex <= anUpper; ++anIndex ) 
  {
    aAllocator->Reset(Standard_False);
    BRepMesh::MapOfIntegerInteger aLoopEdges(10, aAllocator);
    
    Standard_Integer aVertexIdx = theVertexIndexes( anIndex );    
    const BRepMesh_Vertex& aVertex = GetVertex( aVertexIdx );

    // Iterator in the list of indexes of circles containing the node
    BRepMesh::ListOfInteger& aCirclesList = myCircles.Select( aVertex.Coord() );
    
    Standard_Integer onEgdeId = 0, aTriangleId = 0;
    BRepMesh::ListOfInteger::Iterator aCircleIt( aCirclesList );
    for ( ; aCircleIt.More(); aCircleIt.Next() )
    {
      // To add a node in the mesh it is necessary to check conditions: 
      // - the node should be within the boundaries of the mesh and so in an existing triangle
      // - all adjacent triangles should belong to a component connected with this triangle
      if ( Contains( aCircleIt.Value(), aVertex, aSqTol, onEgdeId ) )
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
        aCirclesList.Remove( aCircleIt );
        break;
      }
    }

    if ( aTriangleId > 0 )
    {
      deleteTriangle( aTriangleId, aLoopEdges );

      isModify = Standard_True;    
      while ( isModify && !aCirclesList.IsEmpty() )
      {
        isModify = Standard_False;
        BRepMesh::ListOfInteger::Iterator aCircleIt1( aCirclesList );
        for ( ; aCircleIt1.More(); aCircleIt1.Next() )
        {
          Standard_Integer e[3];
          Standard_Boolean o[3];
          GetTriangle( aCircleIt1.Value() ).Edges( e, o );
                                                   
          if ( aLoopEdges.IsBound( e[0] ) || 
               aLoopEdges.IsBound( e[1] ) || 
               aLoopEdges.IsBound( e[2] ) )
          {
            isModify = Standard_True;
            deleteTriangle( aCircleIt1.Value(), aLoopEdges );
            aCirclesList.Remove( aCircleIt1 );
            break;
          }
        }
      }

      // Creation of triangles with the current node and free edges
      // and removal of these edges from the list of free edges
      createTriangles( aVertexIdx, aLoopEdges );
    }
  }

  insertInternalEdges();

  // Adjustment of meshes to boundary edges
  frontierAdjust();
}

//=======================================================================
//function : insertInternalEdges
//purpose  : 
//=======================================================================
void BRepMesh_Delaun::insertInternalEdges()
{
  BRepMesh::HMapOfInteger anInternalEdges = InternalEdges();

  // Destruction of triancles intersecting internal edges 
  // and their replacement by makeshift triangles
  Standard_Integer e[3];
  Standard_Boolean o[3];
  BRepMesh::MapOfInteger::Iterator anInernalEdgesIt( *anInternalEdges );
  for ( ; anInernalEdgesIt.More(); anInernalEdgesIt.Next() )
  {
    const Standard_Integer aLinkIndex = anInernalEdgesIt.Key();
    const BRepMesh_PairOfIndex& aPair = myMeshData->ElementsConnectedTo(aLinkIndex);

    // Check both sides of link for adjusted triangle.
    Standard_Boolean isGo[2] = { Standard_True, Standard_True };
    for (Standard_Integer aTriangleIt = 1; aTriangleIt <= aPair.Extent(); ++aTriangleIt)
    {
      GetTriangle(aPair.Index(aTriangleIt)).Edges(e, o);
      for (Standard_Integer i = 0; i < 3; ++i)
      {
        if (e[i] == aLinkIndex)
        {
          isGo[o[i] ? 0 : 1] = Standard_False;
          break;
        }
      }
    }

    if (isGo[0])
    {
      meshLeftPolygonOf(aLinkIndex, Standard_True);
    }

    if (isGo[1])
    {
      meshLeftPolygonOf(aLinkIndex, Standard_False);
    }
  }
}

//=======================================================================
//function : isBoundToFrontier
//purpose  : Goes through the neighbour triangles around the given node 
//           started from the given link, returns TRUE if some triangle 
//           has a bounding frontier edge or FALSE elsewhere.
//           Stop link is used to prevent cycles.
//           Previous element Id is used to identify next neighbor element.
//=======================================================================
Standard_Boolean BRepMesh_Delaun::isBoundToFrontier(
  const Standard_Integer theRefNodeId,
  const Standard_Integer theRefLinkId,
  const Standard_Integer theStopLinkId,
  const Standard_Integer thePrevElementId)
{
  const BRepMesh_PairOfIndex& aPair = 
    myMeshData->ElementsConnectedTo( theRefLinkId );
  if ( aPair.IsEmpty() )
    return Standard_False;

  Standard_Integer aNbElements = aPair.Extent();
  for ( Standard_Integer anElemIt = 1; anElemIt <= aNbElements; ++anElemIt )
  {
    const Standard_Integer aTriId = aPair.Index(anElemIt);
    if ( aTriId < 0 || aTriId == thePrevElementId )
      continue;

    Standard_Integer anEdges[3];
    Standard_Boolean anEdgesOri[3];
    GetTriangle( aTriId ).Edges( anEdges, anEdgesOri );

    for ( Standard_Integer anEdgeIt = 0; anEdgeIt < 3; ++anEdgeIt )
    {
      const Standard_Integer anEdgeId = anEdges[anEdgeIt];
      if ( anEdgeId == theRefLinkId )
        continue;

      if ( anEdgeId == theStopLinkId )
        return Standard_False;

      const BRepMesh_Edge& anEdge = GetEdge( anEdgeId );
      if ( anEdge.FirstNode() != theRefNodeId &&
           anEdge.LastNode()  != theRefNodeId )
      {
        continue;
      }

      if ( anEdge.Movability() != BRepMesh_Free )
        return Standard_True;

      return isBoundToFrontier( theRefNodeId, anEdgeId, theStopLinkId, aTriId );
    }
  }

  return Standard_False;
}

//=======================================================================
//function : cleanupMesh
//purpose  : Cleanup mesh from the free triangles
//=======================================================================
void BRepMesh_Delaun::cleanupMesh()
{
  Handle(NCollection_IncAllocator) aAllocator =
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);

  for(;;)
  {
    aAllocator->Reset(Standard_False);
    BRepMesh::MapOfIntegerInteger aLoopEdges(10, aAllocator);
    BRepMesh::MapOfInteger aDelTriangles(10, aAllocator);

    BRepMesh::HMapOfInteger aFreeEdges = FreeEdges();
    BRepMesh::MapOfInteger::Iterator aFreeEdgesIt( *aFreeEdges );
    for ( ; aFreeEdgesIt.More(); aFreeEdgesIt.Next() )
    {
      const Standard_Integer& aFreeEdgeId = aFreeEdgesIt.Key();
      const BRepMesh_Edge&    anEdge      = GetEdge( aFreeEdgeId );
      if ( anEdge.Movability() == BRepMesh_Frontier )
        continue;

      const BRepMesh_PairOfIndex& aPair = 
        myMeshData->ElementsConnectedTo( aFreeEdgeId );
      if ( aPair.IsEmpty() )
      {
        aLoopEdges.Bind( aFreeEdgeId, Standard_True );
        continue;
      }

      Standard_Integer aTriId = aPair.FirstIndex();

      // Check that the connected triangle is not surrounded by another triangles
      Standard_Integer anEdges[3];
      Standard_Boolean anEdgesOri[3];
      GetTriangle( aTriId ).Edges( anEdges, anEdgesOri );

      Standard_Boolean isCanNotBeRemoved = Standard_True;
      for ( Standard_Integer aCurEdgeIdx = 0; aCurEdgeIdx < 3; ++aCurEdgeIdx )
      {
        if ( anEdges[aCurEdgeIdx] != aFreeEdgeId )
          continue;

        for ( Standard_Integer anOtherEdgeIt = 1; anOtherEdgeIt <= 2; ++anOtherEdgeIt )
        {
          Standard_Integer anOtherEdgeId = ( aCurEdgeIdx + anOtherEdgeIt ) % 3;
          const BRepMesh_PairOfIndex& anOtherEdgePair = 
            myMeshData->ElementsConnectedTo( anEdges[anOtherEdgeId] );

          if ( anOtherEdgePair.Extent() < 2 )
          {
            isCanNotBeRemoved = Standard_False;
            break;
          }
        }

        break;
      }

      if ( isCanNotBeRemoved )
        continue;

      Standard_Boolean isConnected[2] = { Standard_False, Standard_False };
      for ( Standard_Integer aLinkNodeIt = 0; aLinkNodeIt < 2; ++aLinkNodeIt )
      {
        isConnected[aLinkNodeIt] = isBoundToFrontier( ( aLinkNodeIt == 0 ) ? 
          anEdge.FirstNode() : anEdge.LastNode(), 
          aFreeEdgeId, aFreeEdgeId, -1);
      }

      if ( !isConnected[0] || !isConnected[1] )
        aDelTriangles.Add( aTriId );
    }

    // Destruction of triangles :
    Standard_Integer aDeletedTrianglesNb = 0;
    BRepMesh::MapOfInteger::Iterator aDelTrianglesIt( aDelTriangles );
    for ( ; aDelTrianglesIt.More(); aDelTrianglesIt.Next() )
    {
      deleteTriangle( aDelTrianglesIt.Key(), aLoopEdges );
      aDeletedTrianglesNb++;
    }

    // Destruction of remaining hanging edges
    BRepMesh::MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
    for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
    {
      if ( myMeshData->ElementsConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
        myMeshData->RemoveLink( aLoopEdgesIt.Key() );
    }

    if ( aDeletedTrianglesNb == 0 )
      break;
  }
}

//=======================================================================
//function : frontierAdjust
//purpose  : Adjust the mesh on the frontier
//=======================================================================
void BRepMesh_Delaun::frontierAdjust()
{
  BRepMesh::HMapOfInteger        aFrontier = Frontier();

  Handle(NCollection_IncAllocator) aAllocator =
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);

  BRepMesh::VectorOfInteger      aFailedFrontiers(256, aAllocator);
  BRepMesh::MapOfIntegerInteger  aLoopEdges(10, aAllocator);
  BRepMesh::HMapOfInteger        aIntFrontierEdges = 
    new BRepMesh::MapOfInteger(10, aAllocator);

  for ( Standard_Integer aPass = 1; aPass <= 2; ++aPass )
  {      
    // 1 pass): find external triangles on boundary edges;
    // 2 pass): find external triangles on boundary edges appeared 
    //          during triangles replacement.
    
    BRepMesh::MapOfInteger::Iterator aFrontierIt( *aFrontier );
    for ( ; aFrontierIt.More(); aFrontierIt.Next() )
    {
      Standard_Integer aFrontierId = aFrontierIt.Key();
      const BRepMesh_PairOfIndex& aPair = myMeshData->ElementsConnectedTo( aFrontierId );
      Standard_Integer aNbElem = aPair.Extent();
      for( Standard_Integer aElemIt = 1; aElemIt <= aNbElem; ++aElemIt )
      {
        const Standard_Integer aPriorElemId = aPair.Index( aElemIt );
        if( aPriorElemId < 0 )
          continue;
            
        Standard_Integer e[3];
        Standard_Boolean o[3];
        GetTriangle( aPriorElemId ).Edges( e, o );

        Standard_Boolean isTriangleFound = Standard_False;
        for ( Standard_Integer n = 0; n < 3; ++n )
        {
          if ( aFrontierId == e[n] && !o[n] )
          {
            // Destruction  of external triangles on boundary edges
            isTriangleFound = Standard_True;
            deleteTriangle( aPriorElemId, aLoopEdges );
            break;
          }
        }

        if ( isTriangleFound )
          break;
      }
    }

    // destrucrion of remaining hanging edges :
    BRepMesh::MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
    for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
    {
      Standard_Integer aLoopEdgeId = aLoopEdgesIt.Key();
      if (myMeshData->ElementsConnectedTo( aLoopEdgeId ).IsEmpty() )
        myMeshData->RemoveLink( aLoopEdgeId );
    }

    // destruction of triangles crossing the boundary edges and 
    // their replacement by makeshift triangles
    for ( aFrontierIt.Reset(); aFrontierIt.More(); aFrontierIt.Next() )
    {
      Standard_Integer aFrontierId = aFrontierIt.Key();
      if ( !myMeshData->ElementsConnectedTo( aFrontierId ).IsEmpty() )
        continue;

      Standard_Boolean isSuccess = 
        meshLeftPolygonOf( aFrontierId, Standard_True, aIntFrontierEdges );

      if ( aPass == 2 && !isSuccess )
        aFailedFrontiers.Append( aFrontierId );
    }
  }

  cleanupMesh();

  // When the mesh has been cleaned up, try to process frontier edges 
  // once again to fill the possible gaps that might be occured in case of "saw" -
  // situation when frontier edge has a triangle at a right side, but its free 
  // links cross another frontieres  and meshLeftPolygonOf itself can't collect 
  // a closed polygon.
  BRepMesh::VectorOfInteger::Iterator aFailedFrontiersIt( aFailedFrontiers );
  for ( ; aFailedFrontiersIt.More(); aFailedFrontiersIt.Next() )
  {
    Standard_Integer aFrontierId = aFailedFrontiersIt.Value();
    if ( !myMeshData->ElementsConnectedTo( aFrontierId ).IsEmpty() )
      continue;

    meshLeftPolygonOf( aFrontierId, Standard_True, aIntFrontierEdges );
  }
}

//=======================================================================
//function : fillBndBox
//purpose  : Add boundig box for edge defined by start & end point to
//           the given vector of bounding boxes for triangulation edges
//=======================================================================
void BRepMesh_Delaun::fillBndBox(BRepMesh::SequenceOfBndB2d& theBoxes,
                                 const BRepMesh_Vertex&      theV1,
                                 const BRepMesh_Vertex&      theV2)
{
  Bnd_B2d aBox;
  UpdateBndBox(theV1.Coord(), theV2.Coord(), aBox);
  theBoxes.Append( aBox );
}

//=======================================================================
//function : meshLeftPolygonOf
//purpose  : Collect the polygon at the left of the given edge (material side)
//=======================================================================
Standard_Boolean BRepMesh_Delaun::meshLeftPolygonOf( 
  const Standard_Integer  theStartEdgeId,
  const Standard_Boolean  isForward,
  BRepMesh::HMapOfInteger theSkipped )
{
  if ( !theSkipped.IsNull() && theSkipped->Contains( theStartEdgeId ) )
    return Standard_True;

  const BRepMesh_Edge& aRefEdge = GetEdge( theStartEdgeId );

  BRepMesh::SequenceOfInteger aPolygon;
  Standard_Integer aStartNode, aPivotNode;
  if ( isForward )
  {
    aPolygon.Append( theStartEdgeId );
    aStartNode = aRefEdge.FirstNode();
    aPivotNode = aRefEdge.LastNode();
  }
  else
  {
    aPolygon.Append( -theStartEdgeId );
    aStartNode = aRefEdge.LastNode();
    aPivotNode = aRefEdge.FirstNode();
  }


  const BRepMesh_Vertex& aStartEdgeVertexS = GetVertex( aStartNode );
  BRepMesh_Vertex        aPivotVertex      = GetVertex( aPivotNode );

  gp_Vec2d aRefLinkDir( aPivotVertex.Coord() - 
    aStartEdgeVertexS.Coord() );

  if ( aRefLinkDir.SquareMagnitude() < Precision2 )
    return Standard_True;

  // Auxilary structures.
  // Bounding boxes of polygon links to be used for preliminary
  // analysis of intersections
  BRepMesh::SequenceOfBndB2d aBoxes;
  fillBndBox( aBoxes, aStartEdgeVertexS, aPivotVertex );

  // Hanging ends
  BRepMesh::MapOfInteger aDeadLinks;

  // Links are temporarily excluded from consideration
  BRepMesh::MapOfInteger aLeprousLinks;
  aLeprousLinks.Add( theStartEdgeId );

  Standard_Boolean isSkipLeprous = Standard_True;
  Standard_Integer aFirstNode    = aStartNode;
  while ( aPivotNode != aFirstNode )
  {
    Bnd_B2d          aNextLinkBndBox;
    gp_Vec2d         aNextLinkDir;
    Standard_Integer aNextPivotNode = 0;

    Standard_Integer aNextLinkId = findNextPolygonLink(
      aFirstNode,
      aPivotNode,     aPivotVertex,  aRefLinkDir, 
      aBoxes,         aPolygon,      theSkipped,
      isSkipLeprous,  aLeprousLinks, aDeadLinks, 
      aNextPivotNode, aNextLinkDir,  aNextLinkBndBox );

    if ( aNextLinkId != 0 )
    {
      aStartNode   = aPivotNode;
      aRefLinkDir  = aNextLinkDir;

      aPivotNode   = aNextPivotNode;
      aPivotVertex = GetVertex( aNextPivotNode );

      aBoxes.Append  ( aNextLinkBndBox );
      aPolygon.Append( aNextLinkId );

      isSkipLeprous = Standard_True;
    }
    else
    {
      // Nothing to do
      if ( aPolygon.Length() == 1 )
        return Standard_False;

      // Return to the previous point
      Standard_Integer aDeadLinkId = Abs( aPolygon.Last() );
      aDeadLinks.Add      ( aDeadLinkId );

      aLeprousLinks.Remove( aDeadLinkId );
      aPolygon.Remove     ( aPolygon.Length() );
      aBoxes.Remove       ( aBoxes.Length() );

      Standard_Integer aPrevLinkInfo = aPolygon.Last();
      const BRepMesh_Edge& aPrevLink = GetEdge( Abs( aPrevLinkInfo ) );

      if( aPrevLinkInfo > 0 )
      {
        aStartNode = aPrevLink.FirstNode();
        aPivotNode = aPrevLink.LastNode();
      }
      else
      {
        aStartNode = aPrevLink.LastNode();
        aPivotNode = aPrevLink.FirstNode();
      }
      
      aPivotVertex = GetVertex( aPivotNode );
      aRefLinkDir = 
        aPivotVertex.Coord() - GetVertex( aStartNode ).Coord();

      isSkipLeprous = Standard_False;
    }
  }

  if ( aPolygon.Length() < 3 )
    return Standard_False;

  cleanupPolygon( aPolygon, aBoxes );
  meshPolygon   ( aPolygon, aBoxes, theSkipped );

  return Standard_True;
}

//=======================================================================
//function : findNextPolygonLink
//purpose  : Find next link starting from the given node and has maximum
//           angle respect the given reference link.
//           Each time the next link is found other neighbor links at the 
//           pivot node are marked as leprous and will be excluded from 
//           consideration next time until a hanging end is occured.
//=======================================================================
Standard_Integer BRepMesh_Delaun::findNextPolygonLink(
  const Standard_Integer&            theFirstNode,
  const Standard_Integer&            thePivotNode,
  const BRepMesh_Vertex&             thePivotVertex,
  const gp_Vec2d&                    theRefLinkDir,
  const BRepMesh::SequenceOfBndB2d&  theBoxes,
  const BRepMesh::SequenceOfInteger& thePolygon,
  const BRepMesh::HMapOfInteger      theSkipped,
  const Standard_Boolean&            isSkipLeprous,
  BRepMesh::MapOfInteger&            theLeprousLinks,
  BRepMesh::MapOfInteger&            theDeadLinks,
  Standard_Integer&                  theNextPivotNode,
  gp_Vec2d&                          theNextLinkDir,
  Bnd_B2d&                           theNextLinkBndBox )
{
  // Find the next link having the greatest angle 
  // respect to a direction of a reference one
  Standard_Real aMaxAngle = RealFirst();

  Standard_Integer aNextLinkId = 0;
  BRepMesh::ListOfInteger::Iterator aLinkIt( myMeshData->LinksConnectedTo( thePivotNode ) );
  for ( ; aLinkIt.More(); aLinkIt.Next() )
  {
    const Standard_Integer& aNeighbourLinkInfo = aLinkIt.Value();
    Standard_Integer        aNeighbourLinkId   = Abs( aNeighbourLinkInfo );

    if ( theDeadLinks.Contains( aNeighbourLinkId ) ||
       ( !theSkipped.IsNull() && theSkipped->Contains( aNeighbourLinkId ) ) )
    {
      continue;
    }
      
    Standard_Boolean isLeprous = theLeprousLinks.Contains( aNeighbourLinkId );
    if ( isSkipLeprous && isLeprous )
      continue;

    const BRepMesh_Edge& aNeighbourLink = GetEdge( aNeighbourLinkId );

    // Determine whether the link belongs to the mesh
    if ( aNeighbourLink.Movability() == BRepMesh_Free &&
         myMeshData->ElementsConnectedTo( aNeighbourLinkInfo ).IsEmpty() )
    {
      theDeadLinks.Add( aNeighbourLinkId );
      continue;
    }

    Standard_Integer anOtherNode = aNeighbourLink.FirstNode();
    if ( anOtherNode == thePivotNode )
      anOtherNode = aNeighbourLink.LastNode();

    gp_Vec2d aCurLinkDir( GetVertex( anOtherNode ).Coord() - 
      thePivotVertex.Coord() );

    if ( aCurLinkDir.SquareMagnitude() < Precision2 )
    {
      theDeadLinks.Add( aNeighbourLinkId );
      continue;
    }

    if ( !isLeprous )
      theLeprousLinks.Add( aNeighbourLinkId ); 

    Standard_Real    anAngle    = theRefLinkDir.Angle( aCurLinkDir );
    Standard_Boolean isFrontier = 
      ( aNeighbourLink.Movability() == BRepMesh_Frontier );

    Standard_Boolean isCheckPointOnEdge = Standard_True;
    if ( isFrontier )
    {
      if ( Abs( Abs(anAngle) - M_PI ) < Precision::Angular() )
      {
        // Glued constrains - don't check intersection
        isCheckPointOnEdge = Standard_False;
        anAngle            = Abs( anAngle );
      }
    }

    if (anAngle <= aMaxAngle)
      continue;

    Standard_Boolean isCheckEndPoints = ( anOtherNode != theFirstNode );

    Bnd_B2d aBox;
    Standard_Boolean isNotIntersect =
      checkIntersection( aNeighbourLink, thePolygon, theBoxes, 
      isCheckEndPoints, isCheckPointOnEdge, Standard_True, aBox );
    
    if( isNotIntersect )
    {
      aMaxAngle         = anAngle;

      theNextLinkDir    = aCurLinkDir;
      theNextPivotNode  = anOtherNode;
      theNextLinkBndBox = aBox;

      aNextLinkId       = ( aNeighbourLink.FirstNode() == thePivotNode ) ?
        aNeighbourLinkId : -aNeighbourLinkId;
    }
  }

  return aNextLinkId;
}

//=======================================================================
//function : checkIntersection
//purpose  : Check is the given link intersects the polygon boundaries.
//           Returns bounding box for the given link trough the 
//           <theLinkBndBox> parameter.
//=======================================================================
Standard_Boolean BRepMesh_Delaun::checkIntersection( 
  const BRepMesh_Edge&               theLink,
  const BRepMesh::SequenceOfInteger& thePolygon,
  const BRepMesh::SequenceOfBndB2d&  thePolyBoxes,
  const Standard_Boolean             isConsiderEndPointTouch,
  const Standard_Boolean             isConsiderPointOnEdge,
  const Standard_Boolean             isSkipLastEdge,
  Bnd_B2d&                           theLinkBndBox ) const
{
  UpdateBndBox(GetVertex(theLink.FirstNode()).Coord(),
    GetVertex(theLink.LastNode()).Coord(), theLinkBndBox);

  Standard_Integer aPolyLen = thePolygon.Length();
  // Don't check intersection with the last link
  if ( isSkipLastEdge )
    --aPolyLen;

  Standard_Boolean isFrontier = 
    ( theLink.Movability() == BRepMesh_Frontier );

  for ( Standard_Integer aPolyIt = 1; aPolyIt <= aPolyLen; ++aPolyIt )
  {
    if ( !theLinkBndBox.IsOut( thePolyBoxes.Value( aPolyIt ) ) )
    {
      // intersection is possible...
      Standard_Integer aPolyLinkId   = Abs( thePolygon( aPolyIt ) );
      const BRepMesh_Edge& aPolyLink = GetEdge( aPolyLinkId );

      // skip intersections between frontier edges
      if ( aPolyLink.Movability() == BRepMesh_Frontier && isFrontier )
        continue;

      gp_Pnt2d anIntPnt;
      BRepMesh_GeomTool::IntFlag aIntFlag = intSegSeg( theLink, aPolyLink, 
        isConsiderEndPointTouch, isConsiderPointOnEdge, anIntPnt );

      if ( aIntFlag != BRepMesh_GeomTool::NoIntersection )
        return Standard_False;
    }
  }

  // Ok, no intersection
  return Standard_True;
}

//=======================================================================
//function : addTriangle
//purpose  : Add a triangle based on the given oriented edges into mesh
//=======================================================================
void BRepMesh_Delaun::addTriangle( const Standard_Integer (&theEdgesId)[3],
                                   const Standard_Boolean (&theEdgesOri)[3],
                                   const Standard_Integer (&theNodesId)[3])
{
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    const BRepMesh_PairOfIndex& aPair = myMeshData->ElementsConnectedTo(theEdgesId[i]);
    if (aPair.Extent() == 2)
      // it is forbidden to have more than two triangles connected to the same link
      return;
  }
  Standard_Integer aNewTriangleId = 
    myMeshData->AddElement(BRepMesh_Triangle(theEdgesId, 
      theEdgesOri, BRepMesh_Free));

  Standard_Boolean isAdded = myCircles.Bind( 
    aNewTriangleId,
    GetVertex( theNodesId[0] ).Coord(), 
    GetVertex( theNodesId[1] ).Coord(),
    GetVertex( theNodesId[2] ).Coord() );
    
  if ( !isAdded )
    myMeshData->RemoveElement( aNewTriangleId );
}

//=======================================================================
//function : cleanupPolygon
//purpose  : Remove internal triangles from the given polygon
//=======================================================================
void BRepMesh_Delaun::cleanupPolygon(const BRepMesh::SequenceOfInteger& thePolygon,
                                     const BRepMesh::SequenceOfBndB2d&  thePolyBoxes )
{
  Standard_Integer aPolyLen = thePolygon.Length();
  if ( aPolyLen < 3 )
    return;

  Handle(NCollection_IncAllocator) aAllocator =
    new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE);

  BRepMesh::MapOfIntegerInteger aLoopEdges(10, aAllocator);
  BRepMesh::MapOfInteger    anIgnoredEdges(10, aAllocator);
  BRepMesh::MapOfInteger    aPolyVerticesFindMap(10, aAllocator);
  BRepMesh::VectorOfInteger aPolyVertices(256, aAllocator);
  // Collect boundary vertices of the polygon
  for ( Standard_Integer aPolyIt = 1; aPolyIt <= aPolyLen; ++aPolyIt )
  {
    Standard_Integer aPolyEdgeInfo = thePolygon( aPolyIt );
    Standard_Integer aPolyEdgeId   = Abs( aPolyEdgeInfo );
    anIgnoredEdges.Add( aPolyEdgeId );

    Standard_Boolean isForward = ( aPolyEdgeInfo > 0 );
    const BRepMesh_PairOfIndex& aPair = 
      myMeshData->ElementsConnectedTo( aPolyEdgeId );

    Standard_Integer anElemIt = 1;
    for ( ; anElemIt <= aPair.Extent(); ++anElemIt )
    {
      Standard_Integer anElemId = aPair.Index( anElemIt );
      if ( anElemId < 0 )
        continue;

      Standard_Integer anEdges[3];
      Standard_Boolean anEdgesOri[3];
      GetTriangle( anElemId ).Edges(anEdges, anEdgesOri);

      Standard_Integer isTriangleFound = Standard_False;
      for ( Standard_Integer anEdgeIt = 0; anEdgeIt < 3; ++anEdgeIt )
      {
        if ( anEdges[anEdgeIt]    == aPolyEdgeId && 
             anEdgesOri[anEdgeIt] == isForward )
        {
          isTriangleFound = Standard_True;
          deleteTriangle( anElemId, aLoopEdges );
          break;
        }
      }

      if ( isTriangleFound )
        break;
    }

    // Skip a neighbor link to extract unique vertices each time
    if ( aPolyIt % 2 )
    {
      const BRepMesh_Edge& aPolyEdge = GetEdge( aPolyEdgeId );
      Standard_Integer aFirstVertex  = aPolyEdge.FirstNode();
      Standard_Integer aLastVertex   = aPolyEdge.LastNode();

      aPolyVerticesFindMap.Add( aFirstVertex );
      aPolyVerticesFindMap.Add( aLastVertex );

      if ( aPolyEdgeInfo > 0 )
      {
        aPolyVertices.Append( aFirstVertex );
        aPolyVertices.Append( aLastVertex );
      }
      else
      {
        aPolyVertices.Append( aLastVertex );
        aPolyVertices.Append( aFirstVertex );
      }
    }
  }

  // Make closed sequence
  if ( aPolyVertices.First() != aPolyVertices.Last() )
    aPolyVertices.Append( aPolyVertices.First() );

  BRepMesh::MapOfInteger aSurvivedLinks( anIgnoredEdges );

  Standard_Integer aPolyVertIt          = 0;
  Standard_Integer anUniqueVerticesNum  = aPolyVertices.Length() - 1;
  for ( ; aPolyVertIt < anUniqueVerticesNum; ++aPolyVertIt )
  {
    killTrianglesAroundVertex( aPolyVertices( aPolyVertIt ),
      aPolyVertices, aPolyVerticesFindMap, thePolygon,
      thePolyBoxes, aSurvivedLinks, aLoopEdges );
  }

  BRepMesh::MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
  for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
  {
    const Standard_Integer& aLoopEdgeId = aLoopEdgesIt.Key();
    if ( anIgnoredEdges.Contains( aLoopEdgeId ) )
      continue;

    if ( myMeshData->ElementsConnectedTo( aLoopEdgeId ).IsEmpty() )
      myMeshData->RemoveLink( aLoopEdgesIt.Key() );
  }
}

//=======================================================================
//function : killTrianglesAroundVertex
//purpose  : Remove all triangles and edges that are placed
//           inside the polygon or crossed it.
//=======================================================================
void BRepMesh_Delaun::killTrianglesAroundVertex( 
  const Standard_Integer             theZombieNodeId,
  const BRepMesh::VectorOfInteger&   thePolyVertices,
  const BRepMesh::MapOfInteger&      thePolyVerticesFindMap,
  const BRepMesh::SequenceOfInteger& thePolygon,
  const BRepMesh::SequenceOfBndB2d&  thePolyBoxes,
  BRepMesh::MapOfInteger&            theSurvivedLinks,
  BRepMesh::MapOfIntegerInteger&     theLoopEdges )
{
  BRepMesh::ListOfInteger::Iterator aNeighborsIt = 
    myMeshData->LinksConnectedTo( theZombieNodeId );

  // Try to infect neighbor nodes
  BRepMesh::VectorOfInteger aVictimNodes;
  for ( ; aNeighborsIt.More(); aNeighborsIt.Next() )
  {
    const Standard_Integer& aNeighborLinkId = aNeighborsIt.Value();
    if ( theSurvivedLinks.Contains( aNeighborLinkId ) )
      continue;

    const BRepMesh_Edge& aNeighborLink = GetEdge( aNeighborLinkId );
    if ( aNeighborLink.Movability() == BRepMesh_Frontier )
    {
      // Though, if it lies onto the polygon boundary -
      // take its triangles
      Bnd_B2d aBox;
      Standard_Boolean isNotIntersect =
        checkIntersection( aNeighborLink, thePolygon, 
        thePolyBoxes, Standard_False, Standard_True, 
        Standard_False, aBox );

      if ( isNotIntersect )
      {
        // Don't touch it
        theSurvivedLinks.Add( aNeighborLinkId );
        continue;
      }
    }
    else
    {
      Standard_Integer anOtherNode = aNeighborLink.FirstNode();
      if ( anOtherNode == theZombieNodeId )
        anOtherNode = aNeighborLink.LastNode();

      // Possible sacrifice
      if ( !thePolyVerticesFindMap.Contains( anOtherNode ) )
      {
        if ( isVertexInsidePolygon( anOtherNode, thePolyVertices ) )
        {
          // Got you!
          aVictimNodes.Append( anOtherNode );
        }
        else
        {
          // Lucky. But it may intersect the polygon boundary.
          // Let's check it!
          killTrianglesOnIntersectingLinks( aNeighborLinkId, 
            aNeighborLink, anOtherNode, thePolygon, 
            thePolyBoxes, theSurvivedLinks, theLoopEdges );

          continue;
        }
      }
    }

    // Add link to the survivers to avoid cycling
    theSurvivedLinks.Add( aNeighborLinkId );
    killLinkTriangles( aNeighborLinkId, theLoopEdges );
  }

  // Go and do your job!
  BRepMesh::VectorOfInteger::Iterator aVictimIt( aVictimNodes );
  for ( ; aVictimIt.More(); aVictimIt.Next() )
  {
    killTrianglesAroundVertex( aVictimIt.Value(), thePolyVertices,
      thePolyVerticesFindMap, thePolygon, thePolyBoxes,
      theSurvivedLinks, theLoopEdges );
  }
}

//=======================================================================
//function : isVertexInsidePolygon
//purpose  : Checks is the given vertex lies inside the polygon
//=======================================================================
Standard_Boolean BRepMesh_Delaun::isVertexInsidePolygon( 
  const Standard_Integer&          theVertexId,
  const BRepMesh::VectorOfInteger& thePolygonVertices ) const
{
  Standard_Integer aPolyLen = thePolygonVertices.Length();
  if ( aPolyLen < 3 )
    return Standard_False;


  const gp_XY aCenterPointXY = GetVertex( theVertexId ).Coord();

  const BRepMesh_Vertex& aFirstVertex = GetVertex( thePolygonVertices( 0 ) );
  gp_Vec2d aPrevVertexDir( aFirstVertex.Coord() - aCenterPointXY );
  if ( aPrevVertexDir.SquareMagnitude() < Precision2 )
    return Standard_True;

  Standard_Real aTotalAng = 0.0;
  for ( Standard_Integer aPolyIt = 1; aPolyIt < aPolyLen; ++aPolyIt )
  {
    const BRepMesh_Vertex& aPolyVertex = GetVertex( thePolygonVertices( aPolyIt ) );

    gp_Vec2d aCurVertexDir( aPolyVertex.Coord() - aCenterPointXY );
    if ( aCurVertexDir.SquareMagnitude() < Precision2 )
      return Standard_True;

    aTotalAng     += aCurVertexDir.Angle( aPrevVertexDir );
    aPrevVertexDir = aCurVertexDir;
  }
  
  if ( Abs( Angle2PI - aTotalAng ) > Precision::Angular() )
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : killTrianglesOnIntersectingLinks
//purpose  : Checks is the given link crosses the polygon boundary.
//           If yes, kills its triangles and checks neighbor links on
//           boundary intersection. Does nothing elsewhere.
//=======================================================================
void BRepMesh_Delaun::killTrianglesOnIntersectingLinks( 
  const Standard_Integer&            theLinkToCheckId, 
  const BRepMesh_Edge&               theLinkToCheck,
  const Standard_Integer&            theEndPoint,
  const BRepMesh::SequenceOfInteger& thePolygon,
  const BRepMesh::SequenceOfBndB2d&  thePolyBoxes,
  BRepMesh::MapOfInteger&            theSurvivedLinks,
  BRepMesh::MapOfIntegerInteger&     theLoopEdges )
{
  if ( theSurvivedLinks.Contains( theLinkToCheckId ) )
    return;

  Bnd_B2d aBox;
  Standard_Boolean isNotIntersect =
    checkIntersection( theLinkToCheck, thePolygon, 
      thePolyBoxes, Standard_False, Standard_False, 
      Standard_False, aBox );

  theSurvivedLinks.Add( theLinkToCheckId );

  if ( isNotIntersect )
    return;

  killLinkTriangles( theLinkToCheckId, theLoopEdges );

  BRepMesh::ListOfInteger::Iterator aNeighborsIt(
    myMeshData->LinksConnectedTo(theEndPoint));

  for ( ; aNeighborsIt.More(); aNeighborsIt.Next() )
  {
    const Standard_Integer& aNeighborLinkId = aNeighborsIt.Value();
    const BRepMesh_Edge&    aNeighborLink   = GetEdge( aNeighborLinkId );
    Standard_Integer anOtherNode = aNeighborLink.FirstNode();
    if ( anOtherNode == theEndPoint )
      anOtherNode = aNeighborLink.LastNode();

    killTrianglesOnIntersectingLinks( aNeighborLinkId, 
      aNeighborLink, anOtherNode, thePolygon, 
      thePolyBoxes, theSurvivedLinks, theLoopEdges );
  }
}

//=======================================================================
//function : killLinkTriangles
//purpose  : Kill triangles bound to the given link.
//=======================================================================
void BRepMesh_Delaun::killLinkTriangles( 
  const Standard_Integer&        theLinkId, 
  BRepMesh::MapOfIntegerInteger& theLoopEdges )
{
  const BRepMesh_PairOfIndex& aPair = 
    myMeshData->ElementsConnectedTo( theLinkId );

  Standard_Integer anElemNb = aPair.Extent();
  for ( Standard_Integer aPairIt = 1; aPairIt <= anElemNb; ++aPairIt )
  {
    Standard_Integer anElemId = aPair.FirstIndex();
    if ( anElemId < 0 )
      continue;

    deleteTriangle( anElemId, theLoopEdges );
  }
}

//=======================================================================
//function : getOrientedNodes
//purpose  : Returns start and end nodes of the given edge in respect to 
//           its orientation.
//=======================================================================
void BRepMesh_Delaun::getOrientedNodes(const BRepMesh_Edge&   theEdge,
                                       const Standard_Boolean isForward,
                                       Standard_Integer       *theNodes) const
{
  if ( isForward )
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
//function : processLoop
//purpose  : Processes loop within the given polygon formed by range of 
//           its links specified by start and end link indices.
//=======================================================================
void BRepMesh_Delaun::processLoop(const Standard_Integer             theLinkFrom,
                                  const Standard_Integer             theLinkTo,
                                  const BRepMesh::SequenceOfInteger& thePolygon,
                                  const BRepMesh::SequenceOfBndB2d&  thePolyBoxes)
{
  Standard_Integer aNbOfLinksInLoop = theLinkTo - theLinkFrom - 1;
  if ( aNbOfLinksInLoop < 3 )
    return;

  BRepMesh::SequenceOfInteger aPolygon;
  BRepMesh::SequenceOfBndB2d  aPolyBoxes;
  for ( ; aNbOfLinksInLoop > 0; --aNbOfLinksInLoop )
  {
    Standard_Integer aLoopLinkIndex = theLinkFrom + aNbOfLinksInLoop;
    aPolygon  .Prepend( thePolygon  ( aLoopLinkIndex ) );
    aPolyBoxes.Prepend( thePolyBoxes( aLoopLinkIndex ) );
  }
  meshPolygon( aPolygon, aPolyBoxes );
}

//=======================================================================
//function : createAndReplacePolygonLink
//purpose  : Creates new link based on the given nodes and updates the 
//           given polygon.
//=======================================================================
Standard_Integer BRepMesh_Delaun::createAndReplacePolygonLink(
  const Standard_Integer       *theNodes,
  const gp_Pnt2d               *thePnts,
  const Standard_Integer       theRootIndex,
  const ReplaceFlag            theReplaceFlag,
  BRepMesh::SequenceOfInteger& thePolygon,
  BRepMesh::SequenceOfBndB2d&  thePolyBoxes )
{
  Standard_Integer aNewEdgeId = 
    myMeshData->AddLink( BRepMesh_Edge(
    theNodes[0], theNodes[1], BRepMesh_Free ) );

  Bnd_B2d aNewBox;
  UpdateBndBox(thePnts[0].Coord(), thePnts[1].Coord(), aNewBox);

  switch ( theReplaceFlag )
  {
  case BRepMesh_Delaun::Replace:
    thePolygon  .SetValue( theRootIndex, aNewEdgeId );
    thePolyBoxes.SetValue( theRootIndex, aNewBox );
    break;

  case BRepMesh_Delaun::InsertAfter:
    thePolygon  .InsertAfter( theRootIndex, aNewEdgeId );
    thePolyBoxes.InsertAfter( theRootIndex, aNewBox );
    break;

  case BRepMesh_Delaun::InsertBefore:
    thePolygon  .InsertBefore( theRootIndex, aNewEdgeId );
    thePolyBoxes.InsertBefore( theRootIndex, aNewBox );
    break;
  }

  return aNewEdgeId;
}

//=======================================================================
//function : meshPolygon
//purpose  : 
//=======================================================================
void BRepMesh_Delaun::meshPolygon(BRepMesh::SequenceOfInteger& thePolygon,
                                  BRepMesh::SequenceOfBndB2d&  thePolyBoxes,
                                  BRepMesh::HMapOfInteger      theSkipped )
{
  // Check is the source polygon elementary
  if ( meshElementaryPolygon( thePolygon ) )
    return;

  // Check and correct boundary edges
  Standard_Integer aPolyLen = thePolygon.Length();
  const Standard_Real aPolyArea      = Abs( polyArea( thePolygon, 1, aPolyLen ) );
  const Standard_Real aSmallLoopArea = 0.001 * aPolyArea;
  for ( Standard_Integer aPolyIt = 1; aPolyIt < aPolyLen; ++aPolyIt )
  {
    Standard_Integer aCurEdgeInfo = thePolygon( aPolyIt );
    Standard_Integer aCurEdgeId   = Abs( aCurEdgeInfo );
    const BRepMesh_Edge* aCurEdge = &GetEdge( aCurEdgeId );
    if ( aCurEdge->Movability() != BRepMesh_Frontier )
      continue;

    Standard_Integer aCurNodes[2];
    getOrientedNodes( *aCurEdge, aCurEdgeInfo > 0, aCurNodes );

    gp_Pnt2d aCurPnts[2] = { 
      GetVertex(aCurNodes[0]).Coord(),
      GetVertex(aCurNodes[1]).Coord()
    };

    gp_Vec2d aCurVec( aCurPnts[0], aCurPnts[1] );

    // check further links
    Standard_Integer aNextPolyIt = aPolyIt + 1;
    for ( ; aNextPolyIt <= aPolyLen; ++aNextPolyIt )
    {
      Standard_Integer aNextEdgeInfo = thePolygon( aNextPolyIt );
      Standard_Integer aNextEdgeId   = Abs( aNextEdgeInfo );
      const BRepMesh_Edge* aNextEdge = &GetEdge( aNextEdgeId );
      if ( aNextEdge->Movability() != BRepMesh_Frontier )
        continue;

      Standard_Integer aNextNodes[2];
      getOrientedNodes( *aNextEdge, aNextEdgeInfo > 0, aNextNodes );

      gp_Pnt2d aNextPnts[2] = { 
        GetVertex(aNextNodes[0]).Coord(),
        GetVertex(aNextNodes[1]).Coord() 
      };

      gp_Pnt2d anIntPnt;
      BRepMesh_GeomTool::IntFlag aIntFlag = intSegSeg( *aCurEdge, *aNextEdge, 
        Standard_False, Standard_True, anIntPnt );

      if ( aIntFlag == BRepMesh_GeomTool::NoIntersection )
        continue;

      Standard_Boolean isRemoveFromFirst  = Standard_False;
      Standard_Boolean isAddReplacingEdge = Standard_True;
      Standard_Integer aIndexToRemoveTo   = aNextPolyIt;
      if ( aIntFlag == BRepMesh_GeomTool::Cross )
      {
        Standard_Real aLoopArea = polyArea( thePolygon, aPolyIt + 1, aNextPolyIt );
        gp_Vec2d aVec1( anIntPnt, aCurPnts [1] );
        gp_Vec2d aVec2( anIntPnt, aNextPnts[0] );

        aLoopArea += ( aVec1 ^ aVec2 ) / 2.;
        if ( Abs( aLoopArea ) > aSmallLoopArea )
        {
          aNextNodes[1] = aCurNodes[0];
          aNextPnts [1] = aCurPnts [0];

          aNextEdgeId = Abs( createAndReplacePolygonLink( aNextNodes, aNextPnts,
            aNextPolyIt, BRepMesh_Delaun::Replace, thePolygon, thePolyBoxes ) );

          processLoop( aPolyIt, aNextPolyIt, thePolygon, thePolyBoxes );
          return;
        }

        Standard_Real aDist1 = anIntPnt.SquareDistance(aNextPnts[0]);
        Standard_Real aDist2 = anIntPnt.SquareDistance(aNextPnts[1]);

        // Choose node with lower distance
        const Standard_Boolean isCloseToStart = ( aDist1 < aDist2 );
        const Standard_Integer aEndPointIndex = isCloseToStart ? 0 : 1;
        aCurNodes[1] = aNextNodes[aEndPointIndex];
        aCurPnts [1] = aNextPnts [aEndPointIndex];

        if ( isCloseToStart )
          --aIndexToRemoveTo;

        // In this context only intersections between frontier edges
        // are possible. If intersection between edges of different
        // types occured - treat this case as invalid (i.e. result 
        // might not reflect the expectations).
        if ( !theSkipped.IsNull() )
        {
          Standard_Integer aSkippedLinkIt = aPolyIt;
          for ( ; aSkippedLinkIt <= aIndexToRemoveTo; ++aSkippedLinkIt )
            theSkipped->Add( Abs( thePolygon( aSkippedLinkIt ) ) );
        }
      }
      else if ( aIntFlag == BRepMesh_GeomTool::PointOnSegment )
      {
        // Indentify chopping link 
        Standard_Boolean isFirstChopping = Standard_False;
        Standard_Integer aCheckPointIt = 0;
        for ( ; aCheckPointIt < 2; ++aCheckPointIt )
        {
          gp_Pnt2d& aRefPoint = aCurPnts[aCheckPointIt];
          // Check is second link touches the first one
          gp_Vec2d aVec1( aRefPoint, aNextPnts[0] );
          gp_Vec2d aVec2( aRefPoint, aNextPnts[1] );
          if ( Abs( aVec1 ^ aVec2 ) < Precision )
          {
            isFirstChopping = Standard_True;
            break;
          }
        }
 
        if ( isFirstChopping )
        {
          // Split second link
          isAddReplacingEdge = Standard_False;
          isRemoveFromFirst  = ( aCheckPointIt == 0 );

          Standard_Integer aSplitLink[3] = {
            aNextNodes[0],
            aCurNodes [aCheckPointIt],
            aNextNodes[1]
          };

          gp_Pnt2d aSplitPnts[3] = {
            aNextPnts[0],
            aCurPnts [aCheckPointIt],
            aNextPnts[1]
          };

          Standard_Integer aSplitLinkIt = 0;
          for ( ; aSplitLinkIt < 2; ++aSplitLinkIt )
          {
            createAndReplacePolygonLink( &aSplitLink[aSplitLinkIt],
              &aSplitPnts[aSplitLinkIt], aNextPolyIt, ( aSplitLinkIt == 0 ) ? 
              BRepMesh_Delaun::Replace : BRepMesh_Delaun::InsertAfter,
              thePolygon, thePolyBoxes );
          }

          processLoop( aPolyIt + aCheckPointIt, aIndexToRemoveTo,
            thePolygon, thePolyBoxes );
        }
        else
        {
          // Split first link
          Standard_Integer aSplitLinkNodes[2] = {
            aNextNodes[1],
            aCurNodes [1]
          };

          gp_Pnt2d aSplitLinkPnts[2] = {
            aNextPnts[1],
            aCurPnts [1]
          };
          createAndReplacePolygonLink( aSplitLinkNodes, aSplitLinkPnts,
            aPolyIt, BRepMesh_Delaun::InsertAfter, thePolygon, thePolyBoxes );

          aCurNodes[1] = aNextNodes[1];
          aCurPnts [1] = aNextPnts [1];
          ++aIndexToRemoveTo;

          processLoop( aPolyIt + 1, aIndexToRemoveTo, 
            thePolygon, thePolyBoxes );
        }
      }
      else if ( aIntFlag == BRepMesh_GeomTool::Glued )
      {
        if ( aCurNodes[1] == aNextNodes[0] )
        {
          aCurNodes[1] = aNextNodes[1];
          aCurPnts [1] = aNextPnts [1];
        }
        // TODO: Non-adjacent glued links within the polygon
      }
      else if ( aIntFlag == BRepMesh_GeomTool::Same )
      {
        processLoop( aPolyIt, aNextPolyIt, thePolygon, thePolyBoxes );

        isRemoveFromFirst  = Standard_True;
        isAddReplacingEdge = Standard_False;
      }
      else
        continue; // Not supported type

      if ( isAddReplacingEdge )
      {
        aCurEdgeId = Abs( createAndReplacePolygonLink( aCurNodes, aCurPnts,
          aPolyIt, BRepMesh_Delaun::Replace, thePolygon, thePolyBoxes ) );

        aCurEdge = &GetEdge( aCurEdgeId );
        aCurVec  = gp_Vec2d( aCurPnts[0], aCurPnts[1] );
      }

      Standard_Integer aIndexToRemoveFrom = 
        isRemoveFromFirst ? aPolyIt : aPolyIt + 1;

      thePolygon  .Remove( aIndexToRemoveFrom, aIndexToRemoveTo );
      thePolyBoxes.Remove( aIndexToRemoveFrom, aIndexToRemoveTo );

      aPolyLen = thePolygon.Length();
      if ( isRemoveFromFirst )
      {
        --aPolyIt;
        break;
      }

      aNextPolyIt = aPolyIt;
    }
  }

  BRepMesh::SequenceOfInteger* aPolygon1   = &thePolygon;
  BRepMesh::SequenceOfBndB2d*  aPolyBoxes1 = &thePolyBoxes;

  BRepMesh::HSequenceOfInteger aPolygon2   = new BRepMesh::SequenceOfInteger;
  BRepMesh::HSequenceOfBndB2d  aPolyBoxes2 = new BRepMesh::SequenceOfBndB2d;

  NCollection_Sequence<BRepMesh::HSequenceOfInteger> aPolyStack;
  NCollection_Sequence<BRepMesh::HSequenceOfBndB2d>  aPolyBoxStack;
  for (;;)
  {
    decomposeSimplePolygon(*aPolygon1, *aPolyBoxes1, *aPolygon2, *aPolyBoxes2);
    if (!aPolygon2->IsEmpty())
    {
      aPolyStack.Append(aPolygon2);
      aPolyBoxStack.Append(aPolyBoxes2);
      
      aPolygon2   = new BRepMesh::SequenceOfInteger;
      aPolyBoxes2 = new BRepMesh::SequenceOfBndB2d;
    }

    if (aPolygon1->IsEmpty())
    {
      if (!aPolyStack.IsEmpty() && aPolygon1 == &(*aPolyStack.First()))
      {
        aPolyStack.Remove(1);
        aPolyBoxStack.Remove(1);
      }

      if (aPolyStack.IsEmpty())
        return;

      aPolygon1   = &(*aPolyStack.ChangeFirst());
      aPolyBoxes1 = &(*aPolyBoxStack.ChangeFirst());
    }
  }
}

//=======================================================================
//function : meshElementaryPolygon
//purpose  : Triangulation of closed polygon containing only three edges.
//=======================================================================
inline Standard_Boolean BRepMesh_Delaun::meshElementaryPolygon( 
  const BRepMesh::SequenceOfInteger& thePolygon)
{
  Standard_Integer aPolyLen = thePolygon.Length();
  if ( aPolyLen < 3 )
    return Standard_True;
  else if ( aPolyLen > 3 )
    return Standard_False;

  // Just create a triangle
  Standard_Integer anEdges[3];
  Standard_Boolean anEdgesOri[3];

  for ( Standard_Integer anEdgeIt = 0; anEdgeIt < 3; ++anEdgeIt )
  {
    Standard_Integer anEdgeInfo = thePolygon( anEdgeIt + 1 );
    anEdges[anEdgeIt]           = Abs( anEdgeInfo );
    anEdgesOri[anEdgeIt]        = ( anEdgeInfo > 0 );
  }
  
  const BRepMesh_Edge& anEdge1 = GetEdge( anEdges[0] );
  const BRepMesh_Edge& anEdge2 = GetEdge( anEdges[1] );
  
  Standard_Integer aNodes[3] = { anEdge1.FirstNode(),
                                 anEdge1.LastNode(),
                                 anEdge2.FirstNode() };
  if ( aNodes[2] == aNodes[0] || 
       aNodes[2] == aNodes[1] )
  {
    aNodes[2] = anEdge2.LastNode();
  }

  addTriangle( anEdges, anEdgesOri, aNodes );
  return Standard_True;
}

//=======================================================================
//function : meshSimplePolygon
//purpose  : 
//=======================================================================
void BRepMesh_Delaun::decomposeSimplePolygon(
  BRepMesh::SequenceOfInteger& thePolygon,
  BRepMesh::SequenceOfBndB2d&  thePolyBoxes,
  BRepMesh::SequenceOfInteger& thePolygonCut,
  BRepMesh::SequenceOfBndB2d&  thePolyBoxesCut)
{
  // Check is the given polygon elementary
  if ( meshElementaryPolygon( thePolygon ) )
  {
    thePolygon.Clear();
    thePolyBoxes.Clear();
    return;
  }

  // Polygon contains more than 3 links
  Standard_Integer aFirstEdgeInfo = thePolygon(1);
  const BRepMesh_Edge& aFirstEdge = GetEdge( Abs( aFirstEdgeInfo ) );

  Standard_Integer aNodes[3];
  getOrientedNodes( aFirstEdge, aFirstEdgeInfo > 0, aNodes );
  
  gp_Pnt2d aRefVertices[3];
  aRefVertices[0] = GetVertex( aNodes[0] ).Coord();
  aRefVertices[1] = GetVertex( aNodes[1] ).Coord();

  gp_Vec2d aRefEdgeDir( aRefVertices[0], aRefVertices[1] );

  Standard_Real aRefEdgeLen = aRefEdgeDir.Magnitude();
  if ( aRefEdgeLen < Precision )
  {
    thePolygon.Clear();
    thePolyBoxes.Clear();
    return;
  }

  aRefEdgeDir /= aRefEdgeLen;

  // Find a point with minimum distance respect
  // the end of reference link
  Standard_Integer aUsedLinkId = 0;
  Standard_Real    aOptAngle   = 0.0;
  Standard_Real    aMinDist    = RealLast();
  Standard_Integer aPivotNode  = aNodes[1];
  Standard_Integer aPolyLen    = thePolygon.Length();
  for ( Standard_Integer aLinkIt = 3; aLinkIt <= aPolyLen; ++aLinkIt )
  {
    Standard_Integer aLinkInfo = thePolygon( aLinkIt );
    const BRepMesh_Edge& aNextEdge = GetEdge( Abs( aLinkInfo ) );

    aPivotNode = aLinkInfo > 0 ? 
      aNextEdge.FirstNode() : 
      aNextEdge.LastNode();

    gp_Pnt2d aPivotVertex = GetVertex( aPivotNode ).Coord();
    gp_Vec2d aDistanceDir( aRefVertices[1], aPivotVertex );

    Standard_Real aDist     = aRefEdgeDir ^ aDistanceDir;
    Standard_Real aAngle    = Abs( aRefEdgeDir.Angle(aDistanceDir) );
    Standard_Real anAbsDist = Abs( aDist );
    if (anAbsDist < Precision || aDist < 0.)
      continue;

    if ( ( anAbsDist >= aMinDist                             ) &&
         ( aAngle <= aOptAngle || aAngle > AngDeviation90Deg ) )
    {
      continue;
    }

    // Check is the test link crosses the polygon boudaries
    Standard_Boolean isIntersect = Standard_False;
    for ( Standard_Integer aRefLinkNodeIt = 0; aRefLinkNodeIt < 2; ++aRefLinkNodeIt )
    {
      const Standard_Integer& aLinkFirstNode   = aNodes[aRefLinkNodeIt];
      const gp_Pnt2d&         aLinkFirstVertex = aRefVertices[aRefLinkNodeIt];

      Bnd_B2d aBox;
      UpdateBndBox(aLinkFirstVertex.Coord(), aPivotVertex.Coord(), aBox);

      BRepMesh_Edge aCheckLink( aLinkFirstNode, aPivotNode, BRepMesh_Free );

      Standard_Integer aCheckLinkIt = 2;
      for ( ; aCheckLinkIt <= aPolyLen; ++aCheckLinkIt )
      {
        if( aCheckLinkIt == aLinkIt )
          continue;
        
        if ( !aBox.IsOut( thePolyBoxes.Value( aCheckLinkIt ) ) )
        {
          const BRepMesh_Edge& aPolyLink = 
            GetEdge( Abs( thePolygon( aCheckLinkIt ) ) );

          if ( aCheckLink.IsEqual( aPolyLink ) )
            continue;

          // intersection is possible...                  
          gp_Pnt2d anIntPnt;
          BRepMesh_GeomTool::IntFlag aIntFlag = intSegSeg( aCheckLink, aPolyLink, 
            Standard_False, Standard_False, anIntPnt );

          if( aIntFlag != BRepMesh_GeomTool::NoIntersection )
          {
            isIntersect = Standard_True;
            break;
          }
        }
      }

      if ( isIntersect )
        break;
    }

    if( isIntersect )
      continue;


    aOptAngle       = aAngle;
    aMinDist        = anAbsDist;
    aNodes[2]       = aPivotNode;
    aRefVertices[2] = aPivotVertex;
    aUsedLinkId     = aLinkIt;
  }

  if ( aUsedLinkId == 0 )
  {
    thePolygon.Clear();
    thePolyBoxes.Clear();
    return;
  }


  BRepMesh_Edge aNewEdges[2] = {
    BRepMesh_Edge( aNodes[1], aNodes[2], BRepMesh_Free ),
    BRepMesh_Edge( aNodes[2], aNodes[0], BRepMesh_Free ) };

  Standard_Integer aNewEdgesInfo[3] = {
    aFirstEdgeInfo,
    myMeshData->AddLink( aNewEdges[0] ),
    myMeshData->AddLink( aNewEdges[1] ) };


  Standard_Integer anEdges[3];
  Standard_Boolean anEdgesOri[3];
  for ( Standard_Integer aTriEdgeIt = 0; aTriEdgeIt < 3; ++aTriEdgeIt )
  {
    const Standard_Integer& anEdgeInfo = aNewEdgesInfo[aTriEdgeIt];
    anEdges[aTriEdgeIt]    = Abs( anEdgeInfo );
    anEdgesOri[aTriEdgeIt] = anEdgeInfo > 0;
  }
  addTriangle( anEdges, anEdgesOri, aNodes );

  // Create triangle and split the source polygon on two 
  // parts (if possible) and mesh each part as independent
  // polygon.
  if ( aUsedLinkId < aPolyLen )
  {
    thePolygon.Split(aUsedLinkId, thePolygonCut);
    thePolygonCut.Prepend( -aNewEdgesInfo[2] );
    thePolyBoxes.Split(aUsedLinkId, thePolyBoxesCut);

    Bnd_B2d aBox;
    UpdateBndBox(aRefVertices[0].Coord(), aRefVertices[2].Coord(), aBox);
    thePolyBoxesCut.Prepend( aBox );
  }
  else
  {
    thePolygon.Remove  ( aPolyLen );
    thePolyBoxes.Remove( aPolyLen );
  }

  if ( aUsedLinkId > 3 )
  {
    thePolygon.SetValue( 1, -aNewEdgesInfo[1] );

    Bnd_B2d aBox;
    UpdateBndBox(aRefVertices[1].Coord(), aRefVertices[2].Coord(), aBox);
    thePolyBoxes.SetValue( 1, aBox );
  }
}

//=======================================================================
//function : RemoveVertex
//purpose  : Removes a vertex from the triangulation
//=======================================================================
void BRepMesh_Delaun::RemoveVertex( const BRepMesh_Vertex& theVertex )
{
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector( myMeshData );
  aSelector.NeighboursOf( theVertex );

  BRepMesh::MapOfIntegerInteger aLoopEdges;//( 10, myMeshData->Allocator() );

  // Loop on triangles to be destroyed :
  BRepMesh::MapOfInteger::Iterator aTriangleIt( aSelector.Elements() );
  for ( ; aTriangleIt.More(); aTriangleIt.Next() )
    deleteTriangle( aTriangleIt.Key(), aLoopEdges );

  BRepMesh::SequenceOfBndB2d  aBoxes;
  BRepMesh::SequenceOfInteger aPolygon;
  Standard_Integer aLoopEdgesCount = aLoopEdges.Extent();
  BRepMesh::MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );

  if ( aLoopEdgesIt.More() )
  {
    const BRepMesh_Edge& anEdge = GetEdge( aLoopEdgesIt.Key() );
    Standard_Integer aFirstNode = anEdge.FirstNode();
    Standard_Integer aLastNode;
    Standard_Integer aPivotNode = anEdge.LastNode();
    Standard_Integer anEdgeId   = aLoopEdgesIt.Key();
    
    Standard_Boolean isPositive = (aLoopEdges (anEdgeId) != 0);
    if ( !isPositive )
    {
      Standard_Integer aTmp;
      aTmp       = aFirstNode;
      aFirstNode = aPivotNode;
      aPivotNode = aTmp;
      
      aPolygon.Append( -anEdgeId );
    }
    else
      aPolygon.Append( anEdgeId );

    fillBndBox( aBoxes, GetVertex( aFirstNode ), GetVertex( aPivotNode ) );

    aLoopEdges.UnBind( anEdgeId );
    
    aLastNode = aFirstNode;
    while ( aPivotNode != aLastNode )
    {
      BRepMesh::ListOfInteger::Iterator aLinkIt( myMeshData->LinksConnectedTo( aPivotNode ) );
      for ( ; aLinkIt.More(); aLinkIt.Next() )
      {
        if ( aLinkIt.Value() != anEdgeId &&
             aLoopEdges.IsBound( aLinkIt.Value() ) )
        {
          Standard_Integer aCurrentNode;
          anEdgeId = aLinkIt.Value();
          const BRepMesh_Edge& anEdge1 = GetEdge( anEdgeId );
          
          aCurrentNode = anEdge1.LastNode();
          if ( aCurrentNode != aPivotNode )
          {
            aCurrentNode = anEdge1.FirstNode();
            aPolygon.Append( -anEdgeId );
          }
          else
            aPolygon.Append( anEdgeId );

          fillBndBox( aBoxes, GetVertex( aCurrentNode ), GetVertex( aPivotNode ) );
            
          aPivotNode = aCurrentNode;
          aLoopEdges.UnBind( anEdgeId );
          break;
        }
      }
      
      if ( aLoopEdgesCount <= 0 )
        break;
      --aLoopEdgesCount;
    }
    
    meshPolygon( aPolygon, aBoxes );
  }
}


//=======================================================================
//function : AddVertices
//purpose  : Adds some vertices in the triangulation.
//=======================================================================
void BRepMesh_Delaun::AddVertices(BRepMesh::Array1OfVertexOfDelaun& theVertices)
{
  std::make_heap(theVertices.begin(), theVertices.end(), ComparatorOfVertexOfDelaun());
  std::sort_heap(theVertices.begin(), theVertices.end(), ComparatorOfVertexOfDelaun());

  Standard_Integer aLower  = theVertices.Lower();
  Standard_Integer anUpper = theVertices.Upper();
    
  BRepMesh::Array1OfInteger aVertexIndexes( aLower, anUpper );
  for ( Standard_Integer i = aLower; i <= anUpper; ++i )     
    aVertexIndexes(i) = myMeshData->AddNode( theVertices(i) );

  createTrianglesOnNewVertices( aVertexIndexes );
}

//=======================================================================
//function : UseEdge
//purpose  : Modify mesh to use the edge. Return True if done
//=======================================================================
Standard_Boolean BRepMesh_Delaun::UseEdge( const Standard_Integer /*theIndex*/ )
{
  /*
  const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( theIndex );
  if ( aPair.Extent() == 0 )
  {
    const BRepMesh_Edge& anEdge = GetEdge( theIndex );
    
    Standard_Integer aStartNode, aPivotNode, anOtherNode;
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

      Standard_Real    anAngle    = 0.;
      Standard_Real    anAngleMin = RealLast();
      Standard_Real    anAngleMax = RealFirst();
      Standard_Integer aLeftEdge  = 0, aRightEdge = 0;

      BRepMesh_ListOfInteger::Iterator aNeighborIt( aPivotNodeNeighbors );
      for ( ; aNeighborIt.More(); aNeighborIt.Next() )
      {
        Standard_Integer anEdgeId = aNeighborIt.Value();
        if ( anEdgeId != theIndex )
        {
          const BRepMesh_Edge& aNextEdge = GetEdge( anEdgeId );

          Standard_Boolean isInMesh = Standard_True;
          if ( aNextEdge.Movability() == BRepMesh_Free )
          {
            if ( myMeshData->ElemConnectedTo( anEdgeId ).IsEmpty() ) 
              isInMesh = Standard_False;
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
  return Standard_False;
}

//=======================================================================
//function : getEdgesByType
//purpose  : Gives the list of edges with type defined by input parameter
//=======================================================================
BRepMesh::HMapOfInteger BRepMesh_Delaun::getEdgesByType(
  const BRepMesh_DegreeOfFreedom theEdgeType ) const
{
  Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
  BRepMesh::HMapOfInteger aResult = new BRepMesh::MapOfInteger(1, anAlloc);
  BRepMesh::MapOfInteger::Iterator anEdgeIt( myMeshData->LinksOfDomain() );

  for ( ; anEdgeIt.More(); anEdgeIt.Next() )
  {
    Standard_Integer anEdge = anEdgeIt.Key();
    Standard_Boolean isToAdd = (theEdgeType == BRepMesh_Free) ?
      (myMeshData->ElementsConnectedTo( anEdge ).Extent() <= 1) :
      (GetEdge( anEdge ).Movability() == theEdgeType);

    if (isToAdd)
      aResult->Add( anEdge );
  }
  
  return aResult;
}

//=======================================================================
//function : calculateDist
//purpose  : Calculates distances between the given point and edges of
//           triangle
//=======================================================================
Standard_Real BRepMesh_Delaun::calculateDist( const gp_XY            theVEdges[3],
                                              const gp_XY            thePoints[3],
                                              const BRepMesh_Vertex& theVertex,
                                              Standard_Real          theDistance[3],
                                              Standard_Real          theSqModulus[3],
                                              Standard_Integer&      theEdgeOn ) const
{
  Standard_Real aMinDist = RealLast();
  for( Standard_Integer i = 0; i < 3; ++i )
  {
    theSqModulus[i] = theVEdges[i].SquareModulus();
    if ( theSqModulus[i] <= Precision2 )
      return -1;
      
    theDistance[i] = theVEdges[i] ^ ( theVertex.Coord() - thePoints[i] );
    
    Standard_Real aDist = theDistance[i] * theDistance[i];
    aDist /= theSqModulus[i];
    
    if ( aDist < aMinDist )
    {
      theEdgeOn = i;
      aMinDist  = aDist;
    }
  }
  
  return aMinDist;
}

//=======================================================================
//function : Contains
//purpose  : Test if triangle of index <TrianIndex> contains geometricaly
//           <theVertex>. If <theEdgeOn> is != 0 then theVertex is on Edge 
//           of  index <theEdgeOn>
//=======================================================================
Standard_Boolean BRepMesh_Delaun::Contains( const Standard_Integer theTriangleId,
                                            const BRepMesh_Vertex& theVertex,
                                            const Standard_Real    theSqTolerance,
                                            Standard_Integer&      theEdgeOn) const
{
  theEdgeOn = 0;
  
  Standard_Integer e[3];
  Standard_Boolean o[3];
  Standard_Integer p[3];

  const BRepMesh_Triangle& aElement = GetTriangle( theTriangleId );
  aElement.Edges(e, o);

  const BRepMesh_Edge* anEdges[3] = { &GetEdge( e[0] ),
                                      &GetEdge( e[1] ),
                                      &GetEdge( e[2] ) };

  myMeshData->ElementNodes(aElement, p);

  gp_XY aPoints[3];
  aPoints[0] = GetVertex( p[0] ).Coord();
  aPoints[1] = GetVertex( p[1] ).Coord();
  aPoints[2] = GetVertex( p[2] ).Coord();
  
  gp_XY aVEdges[3];
  aVEdges[0] = aPoints[1]; 
  aVEdges[0].Subtract( aPoints[0] );
  
  aVEdges[1] = aPoints[2]; 
  aVEdges[1].Subtract( aPoints[1] );
  
  aVEdges[2] = aPoints[0];
  aVEdges[2].Subtract( aPoints[2] );

  Standard_Real aDistance[3];
  Standard_Real aSqModulus[3];

  Standard_Real aSqMinDist;
  Standard_Integer aEdgeOnId;
  aSqMinDist = calculateDist( aVEdges, aPoints, theVertex, aDistance, aSqModulus, aEdgeOnId );
  if ( aSqMinDist < 0 )
    return Standard_False;

  const Standard_Boolean isNotFree = (anEdges[aEdgeOnId]->Movability() != BRepMesh_Free);
  if ( aSqMinDist > theSqTolerance )
  {
    if (isNotFree && aDistance[aEdgeOnId] < ( aSqModulus[aEdgeOnId] / 5. ))
      theEdgeOn = e[aEdgeOnId];
  }
  else if (isNotFree)
    return Standard_False;
  else
    theEdgeOn = e[aEdgeOnId];

  return (aDistance[0] >= 0. && aDistance[1] >= 0. && aDistance[2] >= 0.);
}

//=============================================================================
//function : intSegSeg
//purpose  : Checks intersection between the two segments.
//=============================================================================
BRepMesh_GeomTool::IntFlag BRepMesh_Delaun::intSegSeg( 
  const BRepMesh_Edge&   theEdg1,
  const BRepMesh_Edge&   theEdg2,
  const Standard_Boolean isConsiderEndPointTouch,
  const Standard_Boolean isConsiderPointOnEdge,
  gp_Pnt2d&              theIntPnt) const
{
  gp_XY p1, p2, p3, p4;
  p1 = GetVertex( theEdg1.FirstNode() ).Coord();
  p2 = GetVertex( theEdg1.LastNode()  ).Coord();
  p3 = GetVertex( theEdg2.FirstNode() ).Coord();
  p4 = GetVertex( theEdg2.LastNode()  ).Coord();
  
  return BRepMesh_GeomTool::IntSegSeg(p1, p2, p3, p4,
    isConsiderEndPointTouch, isConsiderPointOnEdge, theIntPnt);
}

//=============================================================================
//function : polyArea
//purpose  : Returns area of the loop of the given polygon defined by indices 
//           of its start and end links.
//=============================================================================
Standard_Real BRepMesh_Delaun::polyArea(const BRepMesh::SequenceOfInteger& thePolygon,
                                        const Standard_Integer             theStartIndex,
                                        const Standard_Integer             theEndIndex) const
{
  Standard_Real aArea = 0.0;
  Standard_Integer aPolyLen = thePolygon.Length();
  if ( theStartIndex >= theEndIndex ||
       theStartIndex > aPolyLen )
  {
    return aArea;
  }
  Standard_Integer aCurEdgeInfo = thePolygon( theStartIndex );
  Standard_Integer aCurEdgeId   = Abs( aCurEdgeInfo );
  const BRepMesh_Edge* aCurEdge = &GetEdge( aCurEdgeId );

  Standard_Integer aNodes[2];
  getOrientedNodes( *aCurEdge, aCurEdgeInfo > 0, aNodes );

  gp_Pnt2d aRefPnt = GetVertex( aNodes[0] ).Coord();
  Standard_Integer aPolyIt = theStartIndex + 1;
  for ( ; aPolyIt <= theEndIndex; ++aPolyIt )
  {
    aCurEdgeInfo = thePolygon( aPolyIt );
    aCurEdgeId   = Abs( aCurEdgeInfo );
    aCurEdge     = &GetEdge( aCurEdgeId );

    getOrientedNodes( *aCurEdge, aCurEdgeInfo > 0, aNodes );
    gp_Vec2d aVec1( aRefPnt, GetVertex( aNodes[0] ).Coord() );
    gp_Vec2d aVec2( aRefPnt, GetVertex( aNodes[1] ).Coord() );

    aArea += aVec1 ^ aVec2;
  }

  return aArea / 2.;
}

#ifdef OCCT_DEBUG
//=======================================================================
//function : BRepMesh_DumpPoly
//purpose  : 
//=======================================================================
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <Standard_ErrorHandler.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepTools.hxx>
Standard_CString BRepMesh_DumpPoly(void*            thePolygon,
                                   void*            theMeshHandlePtr,
                                   Standard_CString theFileNameStr)
{
  if (thePolygon == 0 || theFileNameStr == 0)
  {
    return "Error: file name or polygon data is null";
  }

  BRepMesh::SequenceOfInteger& aPolygon = *(BRepMesh::SequenceOfInteger*)thePolygon;

  Handle(BRepMesh_DataStructureOfDelaun) aMeshData = 
    *(Handle(BRepMesh_DataStructureOfDelaun)*)theMeshHandlePtr;

  if (aMeshData.IsNull())
    return "Error: mesh data is empty";

  TopoDS_Compound aMesh;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aMesh);

  try
  {
    OCC_CATCH_SIGNALS

    BRepMesh::SequenceOfInteger::Iterator aLinksIt(aPolygon);
    for (; aLinksIt.More(); aLinksIt.Next())
    {
      const BRepMesh_Edge& aLink = aMeshData->GetLink(Abs(aLinksIt.Value()));

      gp_Pnt aPnt[2];
      for (Standard_Integer i = 0; i < 2; ++i)
      {
        const Standard_Integer aNodeId = 
          (i == 0) ? aLink.FirstNode() : aLink.LastNode();

        const gp_XY& aNode = aMeshData->GetNode(aNodeId).Coord();
        aPnt[i] = gp_Pnt(aNode.X(), aNode.Y(), 0.);
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
