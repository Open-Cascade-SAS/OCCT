// Created on: 1993-05-12
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BRepMesh_Delaun.ixx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Precision.hxx>
#include <Bnd_Box2d.hxx>
#include <gp.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <BRepMesh_MapOfIntegerInteger.hxx>
#include <BRepMesh_HeapSortIndexedVertexOfDelaun.hxx>
#include <BRepMesh_ComparatorOfIndexedVertexOfDelaun.hxx>
#include <BRepMesh_HeapSortIndexedVertexOfDelaun.hxx>
#include <BRepMesh_SelectorOfDataStructureOfDelaun.hxx>
#include <BRepMesh_HeapSortVertexOfDelaun.hxx>
#include <BRepMesh_ComparatorOfVertexOfDelaun.hxx>
#include <TColgp_Array1OfXY.hxx>
#include <TColStd_Array1OfReal.hxx>

typedef TColStd_ListIteratorOfListOfInteger  IteratorOnListOfInteger;
typedef TColStd_ListOfInteger                ListOfInteger;

const Standard_Real EPSEPS = Precision::PConfusion() * Precision::PConfusion();
const gp_XY SortingDirection(M_SQRT1_2, M_SQRT1_2);

//=======================================================================
//function : fillBndBox
//purpose  : Add boundig box for edge defined by start & end point to
//           the given vector of bounding boxes for triangulation edges
//=======================================================================
static void fillBndBox( NCollection_Vector <Bnd_Box2d>& theVB,
                        const BRepMesh_Vertex& theV1,
                        const BRepMesh_Vertex& theV2 )
{
  Bnd_Box2d aBox;      
  aBox.Add( gp_Pnt2d( theV1.Coord() ) );
  aBox.Add( gp_Pnt2d( theV2.Coord() ) );
  theVB.Append( aBox );
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with an empty Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun( BRepMesh_Array1OfVertexOfDelaun& theVertices,
                                  const Standard_Boolean isPositive )
: myPositiveOrientation( isPositive ),
  myCircles( theVertices.Length(), new NCollection_IncAllocator() )
{
  if ( theVertices.Length() > 2 )
  {
    myMeshData = new BRepMesh_DataStructureOfDelaun( new NCollection_IncAllocator(),
                                                     theVertices.Length() );
    Init( theVertices );
  }
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun ( const Handle( BRepMesh_DataStructureOfDelaun )& theOldMesh, 
                                   BRepMesh_Array1OfVertexOfDelaun& theVertices,
                                   const Standard_Boolean isPositive )
 : myPositiveOrientation( isPositive ),
   myCircles( theVertices.Length(), theOldMesh->Allocator() )
{
  myMeshData = theOldMesh;
  if ( theVertices.Length() > 2 )
    Init( theVertices );
}

//=======================================================================
//function : BRepMesh_Delaun
//purpose  : Creates the triangulation with and existent Mesh data structure
//=======================================================================
BRepMesh_Delaun::BRepMesh_Delaun( const Handle( BRepMesh_DataStructureOfDelaun )& theOldMesh, 
                                  TColStd_Array1OfInteger& theVertexIndexes,
                                  const Standard_Boolean isPositive )
 : myPositiveOrientation( isPositive ),
   myCircles( theVertexIndexes.Length(), theOldMesh->Allocator() )
{
  myMeshData = theOldMesh;
  if ( theVertexIndexes.Length() > 2 )
  {
    Bnd_Box2d aBox;
    Standard_Integer anIndex = theVertexIndexes.Lower();
    Standard_Integer anUpper = theVertexIndexes.Upper();
    for ( ; anIndex <= anUpper; ++anIndex )
      aBox.Add( gp_Pnt2d( GetVertex( theVertexIndexes( anIndex) ).Coord() ) );

    Perform( aBox, theVertexIndexes );
  }
}

//=======================================================================
//function : Init
//purpose  : Initializes the triangulation with an Array of Vertex
//=======================================================================
void BRepMesh_Delaun::Init( BRepMesh_Array1OfVertexOfDelaun& theVertices )
{
  Bnd_Box2d aBox;
  Standard_Integer aLowerIdx  = theVertices.Lower();
  Standard_Integer anUpperIdx = theVertices.Upper();
  TColStd_Array1OfInteger aVertexIndexes( aLowerIdx, anUpperIdx );
  
  Standard_Integer anIndex = aLowerIdx;
  for ( ; anIndex <= anUpperIdx; ++anIndex )
  {
    aBox.Add( gp_Pnt2d( theVertices( anIndex ).Coord() ) );
    aVertexIndexes( anIndex ) = myMeshData->AddNode( theVertices( anIndex ) );
  }

  Perform( aBox, aVertexIndexes );
}

//=======================================================================
//function : Perform
//purpose  : Create super mesh and run triangulation procedure
//=======================================================================
void BRepMesh_Delaun::Perform( Bnd_Box2d& theBndBox,
                               TColStd_Array1OfInteger& theVertexIndexes )
{
  theBndBox.Enlarge( Precision::PConfusion() );
  SuperMesh( theBndBox );

  BRepMesh_HeapSortIndexedVertexOfDelaun::Sort( theVertexIndexes, 
      BRepMesh_ComparatorOfIndexedVertexOfDelaun( SortingDirection,
        Precision::PConfusion(), myMeshData ) );

  Compute( theVertexIndexes );
}

//=======================================================================
//function : SuperMesh
//purpose  : Build the super mesh
//=======================================================================
void BRepMesh_Delaun::SuperMesh( const Bnd_Box2d& theBox )
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

  mySupVert1 = myMeshData->AddNode(
    BRepMesh_Vertex( ( aMinX + aMaxX ) / 2, aMaxY + aDeltaMax, BRepMesh_Free ) );
    
  mySupVert2 = myMeshData->AddNode(
    BRepMesh_Vertex( aMinX - aDelta, aMinY - aDeltaMin, BRepMesh_Free ) );
    
  mySupVert3 = myMeshData->AddNode(
    BRepMesh_Vertex( aMaxX + aDelta, aMinY - aDeltaMin, BRepMesh_Free ) );

  if ( !myPositiveOrientation )
  {
    Standard_Integer aTmp;
    aTmp       = mySupVert2;
    mySupVert2 = mySupVert3;
    mySupVert3 = aTmp;
  }

  Standard_Integer anEdge1, anEdge2, anEdge3;
  
  anEdge1 = myMeshData->AddLink( BRepMesh_Edge( mySupVert1, mySupVert2, BRepMesh_Free ) );
  anEdge2 = myMeshData->AddLink( BRepMesh_Edge( mySupVert2, mySupVert3, BRepMesh_Free ) );
  anEdge3 = myMeshData->AddLink( BRepMesh_Edge( mySupVert3, mySupVert1, BRepMesh_Free ) );
  
  mySupTrian = BRepMesh_Triangle( Abs( anEdge1 ), Abs( anEdge2 ), Abs( anEdge3 ), 
    ( anEdge1 > 0 ), ( anEdge2 > 0 ), ( anEdge1 > 0), BRepMesh_Free);
}

//=======================================================================
//function : DeleteTriangle
//purpose : The concerned triangles are deleted and the freed edges are added in 
//           <theLoopEdges>. If an edge is added twice, it does not exist and
//          it is necessary to destroy it. This corresponds to the destruction of two
//          connected triangles.
//=======================================================================
void  BRepMesh_Delaun::DeleteTriangle( const Standard_Integer theIndex, 
                                       BRepMesh_MapOfIntegerInteger& theLoopEdges )
{
  myCircles.Delete( theIndex );

  Standard_Integer e[3];
  Standard_Boolean o[3];
  GetTriangle( theIndex ).Edges( e[0], e[1], e[2],
                                 o[0], o[1], o[2] );
                                 
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
//function : Compute
//purpose  : Computes the triangulation and add the vertices edges and 
//           triangles to the Mesh data structure
//=======================================================================
void BRepMesh_Delaun::Compute( TColStd_Array1OfInteger& theVertexIndexes )
{
  // Insertion of edges of super triangles in the list of free edges: 
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  Standard_Integer e[3];
  Standard_Boolean o[3];
  mySupTrian.Edges( e[0], e[1], e[2],
                    o[0], o[1], o[2] );
                    
  aLoopEdges.Bind( e[0], Standard_True );
  aLoopEdges.Bind( e[1], Standard_True );
  aLoopEdges.Bind( e[2], Standard_True );

  if ( theVertexIndexes.Length() > 0 )
  {
    // Creation of 3 trianglers with the first node and the edges of the super triangle:
    Standard_Integer anVertexIdx = theVertexIndexes.Lower();
    CreateTriangles( theVertexIndexes( anVertexIdx ), aLoopEdges );

    // Add other nodes to the mesh
    CreateTrianglesOnNewVertices( theVertexIndexes );
  }

  // Destruction of triangles containing a top of the super triangle
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector( myMeshData );
  aSelector.NeighboursOfNode( mySupVert1 );
  aSelector.NeighboursOfNode( mySupVert2 );
  aSelector.NeighboursOfNode( mySupVert3 );
  
  aLoopEdges.Clear();
  BRepMesh_MapOfInteger::Iterator aFreeTriangles( aSelector.Elements() );
  for ( ; aFreeTriangles.More(); aFreeTriangles.Next() )
    DeleteTriangle( aFreeTriangles.Key(), aLoopEdges );

  // All edges that remain free are removed from aLoopEdges;
  // only the boundary edges of the triangulation remain there
  BRepMesh_MapOfIntegerInteger::Iterator aFreeEdges( aLoopEdges );
  for ( ; aFreeEdges.More(); aFreeEdges.Next() )
  {
    if ( myMeshData->ElemConnectedTo( aFreeEdges.Key() ).IsEmpty() )
      myMeshData->RemoveLink( aFreeEdges.Key() );
  }

  // The tops of the super triangle are destroyed
  myMeshData->RemoveNode( mySupVert1 );
  myMeshData->RemoveNode( mySupVert2 );
  myMeshData->RemoveNode( mySupVert3 );
}

//=======================================================================
//function : CreateTriangles
//purpose  : Creates the triangles beetween the node and the polyline.
//=======================================================================
void BRepMesh_Delaun::CreateTriangles ( const Standard_Integer theVertexIndex,  
                                        BRepMesh_MapOfIntegerInteger& thePoly )
{
  ListOfInteger aLoopEdges, anExternalEdges;
  const gp_XY& aVertexCoord = myMeshData->GetNode( theVertexIndex ).Coord();
  
  BRepMesh_MapOfIntegerInteger::Iterator anEdges( thePoly );
  for ( ; anEdges.More(); anEdges.Next() )
  {
    const BRepMesh_Edge& anEdge = GetEdge( anEdges.Key() );
    Standard_Integer aFirstNode = anEdge.FirstNode();
    Standard_Integer aLastNode  = anEdge.LastNode();
    Standard_Boolean isPositive = (Standard_Boolean)thePoly( anEdges.Key() );
    if ( !isPositive )
    {
      Standard_Integer aTmp;
      aTmp       = aFirstNode;
      aFirstNode = aLastNode;
      aLastNode  = aTmp;
    }

    const BRepMesh_Vertex& aFirstVertex = GetVertex( aFirstNode );
    const BRepMesh_Vertex& aLastVertex  = GetVertex( aLastNode  );

    gp_XY aVEdge1, aVEdge2, aVEdge3;
    aVEdge1 = aFirstVertex.Coord();
    aVEdge1.Subtract( aVertexCoord );
    
    aVEdge2 = aLastVertex.Coord();
    aVEdge2.Subtract( aFirstVertex.Coord() );
    
    aVEdge3 = aVertexCoord;
    aVEdge3.Subtract( aLastVertex.Coord() );

    Standard_Real aDist12 = 0., aDist23 = 0.;
    Standard_Real aV2Len  = aVEdge2.Modulus();
    if ( aV2Len > Precision::PConfusion() )
    {
      aVEdge2.SetCoord( aVEdge2.X() / aV2Len,
                        aVEdge2.Y() / aV2Len );
                        
      aDist12 = aVEdge1 ^ aVEdge2;
      aDist23 = aVEdge2 ^ aVEdge3;
    }

    if ( Abs( aDist12 ) >= Precision::PConfusion() && 
         Abs( aDist23 ) >= Precision::PConfusion() )
    {
      Standard_Boolean isSensOK;
      if ( myPositiveOrientation )
        isSensOK = ( aDist12 > 0. && aDist23 > 0.);
      else
        isSensOK = ( aDist12 < 0. && aDist23 < 0.);
        
      Standard_Integer aNewEdge1, aNewEdge3, aNewTriangle;
      if ( isSensOK )
      {
        aNewEdge1 = myMeshData->AddLink( 
          BRepMesh_Edge( theVertexIndex, aFirstNode, BRepMesh_Free ) );
          
        aNewEdge3 = myMeshData->AddLink(
          BRepMesh_Edge( aLastNode, theVertexIndex, BRepMesh_Free ) );
          
        aNewTriangle = myMeshData->AddElement(
          BRepMesh_Triangle( Abs( aNewEdge1 ), anEdges.Key(), Abs( aNewEdge3 ), 
                             ( aNewEdge1 > 0 ), isPositive, ( aNewEdge3 > 0 ),
                             BRepMesh_Free) );

        Standard_Boolean isCircleCreated = 
          myCircles.Add( aVertexCoord, aFirstVertex.Coord(), aLastVertex.Coord(),
                         aNewTriangle );
                         
        if ( !isCircleCreated )
          myMeshData->RemoveElement( aNewTriangle );
      }
      else
      {
        if ( isPositive )
          aLoopEdges.Append(  anEdges.Key() );
        else
          aLoopEdges.Append( -anEdges.Key() );
          
        if ( aVEdge1.SquareModulus() > aVEdge3.SquareModulus() )
        {
          aNewEdge1 = myMeshData->AddLink(
            BRepMesh_Edge( theVertexIndex, aFirstNode, BRepMesh_Free ) );
            
          anExternalEdges.Append( Abs( aNewEdge1) );
        }
        else
        {
          aNewEdge1 = myMeshData->AddLink(
            BRepMesh_Edge( aLastNode, theVertexIndex, BRepMesh_Free ) );
            
          anExternalEdges.Append( Abs( aNewEdge3 ) );
        }
      }
    }
  }
  
  thePoly.Clear();
  while ( !anExternalEdges.IsEmpty() )
  {
    const BRepMesh_PairOfIndex& aPair = 
      myMeshData->ElemConnectedTo( Abs( anExternalEdges.First() ) );
    
    
    if ( !aPair.IsEmpty() )
      DeleteTriangle( aPair.FirstIndex(), thePoly );
      
    anExternalEdges.RemoveFirst();
  }

  for ( anEdges.Initialize( thePoly ); anEdges.More(); anEdges.Next() )
  {
    if ( myMeshData->ElemConnectedTo( anEdges.Key() ).IsEmpty() )
      myMeshData->RemoveLink( anEdges.Key() );
  }

  while ( !aLoopEdges.IsEmpty() )
  {
    const BRepMesh_Edge& anEdge = GetEdge( Abs( aLoopEdges.First() ) );
    if ( anEdge.Movability() != BRepMesh_Deleted )
    {
      Standard_Integer anEdgeIdx = aLoopEdges.First();
      MeshLeftPolygonOf( Abs( anEdgeIdx ), ( anEdgeIdx > 0 ) );
    }
      
    aLoopEdges.RemoveFirst();
  }
}

//=======================================================================
//function : CreateTrianglesOnNewVertices
//purpose  : Creation of triangles from the new nodes
//=======================================================================
void BRepMesh_Delaun::CreateTrianglesOnNewVertices( TColStd_Array1OfInteger& theVertexIndexes )
{
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );

  // Insertion of nodes :
  Standard_Boolean isModify = Standard_True;
  
  Standard_Integer anIndex = theVertexIndexes.Lower();
  Standard_Integer anUpper = theVertexIndexes.Upper();
  for( ; anIndex <= anUpper; ++anIndex ) 
  {
    aLoopEdges.Clear();
    
    Standard_Integer aVertexIdx = theVertexIndexes( anIndex );    
    const BRepMesh_Vertex& aVertex = GetVertex( aVertexIdx );

    // Iterator in the list of indexes of circles containing the node
    BRepMesh_ListOfInteger& aCirclesList = myCircles.Select( aVertex.Coord() );
    
    Standard_Integer onEgdeId = 0, aTriangleId = 0;
    BRepMesh_ListOfInteger::Iterator aCircleIt( aCirclesList );
    for ( ; aCircleIt.More(); aCircleIt.Next() )
    {
      // To add a node in the mesh it is necessary to check conditions: 
      // - the node should be within the boundaries of the mesh and so in an existing triangle
      // - all adjacent triangles should belong to a component connected with this triangle
      if ( Contains( aCircleIt.Value(), aVertex, onEgdeId ) )
      {
        if ( onEgdeId == 0 )
        {
          aTriangleId = aCircleIt.Value();
          aCirclesList.Remove( aCircleIt );
          break;
        }
        else if ( GetEdge( onEgdeId ).Movability() == BRepMesh_Free )
        {
          aTriangleId = aCircleIt.Value();
          aCirclesList.Remove( aCircleIt );
          break;
        }
      }
    }

    if ( aTriangleId > 0 )
    {
      DeleteTriangle( aTriangleId, aLoopEdges );

      isModify = Standard_True;    
      while ( isModify && !aCirclesList.IsEmpty() )
      {
        isModify = Standard_False;
        BRepMesh_ListOfInteger::Iterator aCircleIt1( aCirclesList );
        for ( ; aCircleIt1.More(); aCircleIt1.Next() )
        {
          Standard_Integer e[3];
          Standard_Boolean o[3];
          GetTriangle( aCircleIt1.Value() ).Edges( e[0], e[1], e[2],
                                                   o[0], o[1], o[2] );
                                                   
          if ( aLoopEdges.IsBound( e[0] ) || 
               aLoopEdges.IsBound( e[1] ) || 
               aLoopEdges.IsBound( e[2] ) )
          {
            isModify = Standard_True;
            DeleteTriangle( aCircleIt1.Value(), aLoopEdges );
            aCirclesList.Remove( aCircleIt1 );
            break;
          }
        }
      }

      // Creation of triangles with the current node and free edges
      // and removal of these edges from the list of free edges
      CreateTriangles( aVertexIdx, aLoopEdges );
    }
  }
  // Check that internal edges are not crossed by triangles
  BRepMesh_MapOfInteger::Iterator anInernalEdgesIt( InternalEdges() );

  // Destruction of triancles intersecting internal edges 
  // and their replacement by makeshift triangles
  anInernalEdgesIt.Reset();
  for ( ; anInernalEdgesIt.More(); anInernalEdgesIt.Next() )
  {
    Standard_Integer aNbC;
    aNbC = myMeshData->ElemConnectedTo( anInernalEdgesIt.Key() ).Extent();
    if ( aNbC == 0 )
    {
      MeshLeftPolygonOf( anInernalEdgesIt.Key(), Standard_True  ); 
      MeshLeftPolygonOf( anInernalEdgesIt.Key(), Standard_False ); 
    }
  }

  // Adjustment of meshes to boundary edges
  FrontierAdjust();
}

//=======================================================================
//function : CleanupMesh
//purpose  : Cleanup mesh from the free triangles
//=======================================================================
void BRepMesh_Delaun::CleanupMesh()
{
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  ListOfInteger aTrianglesList;

  while ( Standard_True )
  {
    aTrianglesList.Clear();
    aLoopEdges.Clear();

    BRepMesh_MapOfInteger::Iterator aFreeEdgesIt( FreeEdges() );
    for ( ; aFreeEdgesIt.More(); aFreeEdgesIt.Next() )
    {
      const BRepMesh_Edge& anEdge = GetEdge( aFreeEdgesIt.Key() );
      if ( anEdge.Movability() != BRepMesh_Frontier )
      {
        Standard_Integer aFrontierNb = 0;
        if ( myMeshData->ElemConnectedTo( aFreeEdgesIt.Key() ).IsEmpty() ) 
          aLoopEdges.Bind( aFreeEdgesIt.Key(), Standard_True );
        else
        {
          BRepMesh_ListOfInteger::Iterator aConnectedLinksIt( 
            myMeshData->LinkNeighboursOf( anEdge.FirstNode() ) );
            
          for ( ; aConnectedLinksIt.More(); aConnectedLinksIt.Next() )
          {
            if ( GetEdge( aConnectedLinksIt.Value() ).Movability() == BRepMesh_Frontier )
            {
              aFrontierNb++;
              if ( aFrontierNb > 1 )
                break;
            }
          }
          
          if ( aFrontierNb == 2 )
          {
            const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( aFreeEdgesIt.Key() );
            for ( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j )
            {
              const Standard_Integer anElemId = aPair.Index(j);
              if ( anElemId < 0 )
                continue;
                
              Standard_Integer e[3];
              Standard_Boolean o[3];
              GetTriangle( anElemId ).Edges( e[0], e[1], e[2],
                                             o[0], o[1], o[2] );
              
              Standard_Boolean isTriangleToDelete = Standard_True;
              for ( Standard_Integer k = 0; k < 3; ++k )
              {
                if ( GetEdge( e[k] ).Movability() != BRepMesh_Free )
                {
                  isTriangleToDelete = Standard_False;
                  break;
                }
              }

              if ( isTriangleToDelete )
                aTrianglesList.Append( anElemId );
            }
          }
        }
      }
    }

    // Destruction of triangles :
    Standard_Integer aDeletedTrianglesNb = 0;
    for ( ; !aTrianglesList.IsEmpty(); aTrianglesList.RemoveFirst() )
    {
      DeleteTriangle( aTrianglesList.First(), aLoopEdges );
      aDeletedTrianglesNb++;
    }

    // Destruction of remaining hanging edges
    BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
    for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
    {
      if ( myMeshData->ElemConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
        myMeshData->RemoveLink( aLoopEdgesIt.Key() );
    }

    if ( aDeletedTrianglesNb == 0 )
      break;
  }
}

//=======================================================================
//function : FrontierAdjust
//purpose  : Adjust the mesh on the frontier
//=======================================================================
void BRepMesh_Delaun::FrontierAdjust()
{
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  ListOfInteger aTrianglesList;

  for ( Standard_Integer aPass = 1; aPass <= 2; ++aPass )
  {      
    // 1 pass): find external triangles on boundary edges
    // 2 pass): find external triangles on boundary edges
    // because in comlex curved boundaries external triangles can appear  
    // after "mesh left aPolygonon"
    
    BRepMesh_MapOfInteger::Iterator aFrontierIt( Frontier() );
    for ( ; aFrontierIt.More(); aFrontierIt.Next() )
    {
      const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( aFrontierIt.Key() );
      for( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j )
      {
        const Standard_Integer aPriorElemId = aPair.Index(j);
        if( aPriorElemId < 0 )
            continue;
            
        Standard_Integer e[3];
        Standard_Boolean o[3];
        GetTriangle( aPriorElemId ).Edges( e[0], e[1], e[2],
                                           o[0], o[1], o[2] );

        for ( Standard_Integer n = 0; n < 3; ++n )
        {
          if ( aFrontierIt.Key() == e[n] && !o[n] )
          {
            aTrianglesList.Append( aPriorElemId );
            break;
          }
        }
      }
    }

    // destruction  of external triangles on boundary edges
    for ( ; !aTrianglesList.IsEmpty(); aTrianglesList.RemoveFirst() )
      DeleteTriangle( aTrianglesList.First(), aLoopEdges );

    // destrucrion of remaining hanging edges :
    BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );
    for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
    {
      if (myMeshData->ElemConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
        myMeshData->RemoveLink( aLoopEdgesIt.Key() );
    }
      

    // destruction of triangles crossing the boundary edges and 
    // their replacement by makeshift triangles
    if ( aPass == 1 )
      aFrontierIt.Reset();
    else
      // in some cases there remain unused boundaries check
      aFrontierIt.Initialize( Frontier() );

    NCollection_Vector<Bnd_Box2d> aFrontBoxes;
    for ( ; aFrontierIt.More(); aFrontierIt.Next() )
    {
      const BRepMesh_Edge& anEdge = GetEdge( aFrontierIt.Key() );
      const BRepMesh_Vertex& aV1  = GetVertex( anEdge.FirstNode() );
      const BRepMesh_Vertex& aV2  = GetVertex( anEdge.LastNode()  );
      fillBndBox( aFrontBoxes, aV1, aV2 );

      if ( myMeshData->ElemConnectedTo( aFrontierIt.Key() ).IsEmpty() )
        MeshLeftPolygonOf( aFrontierIt.Key(), Standard_True ); 
    }

    if ( aPass == 1 ) 
      continue;

    CleanupMesh();
  }
}

//=======================================================================
//function : KillInternalTriangles
//purpose  : Removes triangles within polygon
//=======================================================================
void BRepMesh_Delaun::KillInternalTriangles( Standard_Integer theEdgeId, 
                                             const TColStd_MapOfInteger& theIgnoredEdges,
                                             BRepMesh_MapOfIntegerInteger& theLoopEdges )
{
  if ( theIgnoredEdges.Contains( theEdgeId ) )
    return;   

  const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( theEdgeId );
  for ( Standard_Integer i = 1; i <= aPair.Extent(); ++i )
  {
    Standard_Integer anElemId = aPair.Index(i);
    if( anElemId < 0 )
      continue;

    Standard_Integer e[3];
    Standard_Boolean o[3];
    GetTriangle( anElemId ).Edges( e[0], e[1], e[2],
                                   o[0], o[1], o[2] );

    for ( Standard_Integer anIndex = 0; anIndex < 3; ++anIndex )
    {
      if ( e[anIndex] == theEdgeId )
      {
        DeleteTriangle( anElemId, theLoopEdges );
        for ( Standard_Integer n = 0; n < 2; ++n )
        {
          Standard_Integer aCurEdge = e[(anIndex + n + 1) % 3];
          KillInternalTriangles( aCurEdge, theIgnoredEdges, theLoopEdges );
        }
      }
    }
  }
}

//=======================================================================
//function : RemovePivotTriangles
//purpose  : Removes triangles around the given pivot node
//=======================================================================
void BRepMesh_Delaun::RemovePivotTriangles( const Standard_Integer theEdgeInfo,
                                            const Standard_Integer thePivotNode,
                                            TColStd_MapOfInteger& theInfectedEdges,
                                            BRepMesh_MapOfIntegerInteger& theLoopEdges,
                                            const Standard_Boolean isFirstPass )
{
  Standard_Integer e[3];
  Standard_Boolean o[3];
  Standard_Integer aGeneralEdgeId = -1;

  Standard_Integer anMainEdgeId = Abs( theEdgeInfo );
  Standard_Boolean anIsForward = theEdgeInfo > 0;
  const BRepMesh_PairOfIndex& aPair = myMeshData->ElemConnectedTo( anMainEdgeId );
  for ( Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j )
  {
    Standard_Integer anElemId = aPair.Index(j);
    if( anElemId < 0 )
      continue;

    GetTriangle( anElemId ).Edges( e[0], e[1], e[2],
                                   o[0], o[1], o[2] );

    Standard_Boolean isFind = Standard_False;
    for ( Standard_Integer anIndex = 0; anIndex < 3; ++anIndex )
    {
      if ( e[anIndex] == anMainEdgeId && o[anIndex] == anIsForward )
      {
        // triangle detected
        DeleteTriangle( anElemId, theLoopEdges );
        aGeneralEdgeId = anIndex;
        break;
      }
    }
       
    if ( aGeneralEdgeId != -1 )
      break;
  }

  if ( aGeneralEdgeId != -1 )
  {
    // delete triangles around of aPivotNode starting from the bounding one
    // define edge connected to vertes
    Standard_Integer anEdgeId = -1;
    for ( Standard_Integer i = 0; i < 2; ++i )
    {
      Standard_Integer aTmpEdgeId = e[(aGeneralEdgeId + i + 1) % 3];
      const BRepMesh_Edge& anEdge = GetEdge( aTmpEdgeId );
      if ( anEdge.FirstNode() == thePivotNode || 
           anEdge.LastNode()  == thePivotNode )
      {
        anEdgeId = aTmpEdgeId;
      }

      if ( theInfectedEdges.Contains( aTmpEdgeId ) )
        theInfectedEdges.Remove( aTmpEdgeId );
      else
        theInfectedEdges.Add( aTmpEdgeId );
    }

    if ( isFirstPass )
      return;

    while ( anEdgeId > 0 )
    {
      const BRepMesh_PairOfIndex& aFreePair = myMeshData->ElemConnectedTo( anEdgeId );
      Standard_Integer anOldEdge = anEdgeId;
      anEdgeId = -1;

      for ( Standard_Integer aPairIndex = 1; aPairIndex <= aFreePair.Extent(); ++aPairIndex )
      {
        Standard_Integer anElemId = aFreePair.Index( aPairIndex );
        if( anElemId < 0 )
          continue;
          
        Standard_Integer e1[3];
        Standard_Boolean o1[3];
        GetTriangle( anElemId ).Edges( e1[0], e1[1], e1[2],
                                       o1[0], o1[1], o1[2] );
        
        DeleteTriangle( anElemId, theLoopEdges );

        for ( Standard_Integer i = 0; i < 3; ++i )
        {
          if ( e1[i] == anOldEdge )
          {
            for ( Standard_Integer i = 0; i < 2; ++i )
            {
              Standard_Integer aTmpEdgeId = e1[(i + 1) % 3];
              const BRepMesh_Edge& anEdge = GetEdge( aTmpEdgeId );
              if ( anEdge.FirstNode() == thePivotNode || 
                   anEdge.LastNode()  == thePivotNode )
              {
                anEdgeId = aTmpEdgeId;
              }
            
              if ( theInfectedEdges.Contains( aTmpEdgeId ) )
                theInfectedEdges.Remove( aTmpEdgeId );
              else
                theInfectedEdges.Add( aTmpEdgeId );
            }
            break;
          }
        }
      }
    }
  }
}

//=======================================================================
//function : CleanupPolygon
//purpose  : Remove internal triangles from the given polygon
//=======================================================================
void BRepMesh_Delaun::CleanupPolygon( const TColStd_SequenceOfInteger& thePolygon,
                                      TColStd_MapOfInteger& theInfectedEdges,
                                      BRepMesh_MapOfIntegerInteger& theLoopEdges )
{
  Standard_Integer aPolyLen = thePolygon.Length();

  TColStd_MapOfInteger anIgnoredEdges;
  NCollection_Map<Standard_Integer> aPolyVertices;
  for ( Standard_Integer i = 1; i <= aPolyLen; ++i )
  {
    Standard_Integer aPolyEdgeId = Abs( thePolygon(i) );
    anIgnoredEdges.Add( aPolyEdgeId );

    const BRepMesh_Edge& aPolyEdge = GetEdge( aPolyEdgeId );
    aPolyVertices.Add( aPolyEdge.FirstNode() );
    aPolyVertices.Add( aPolyEdge.LastNode() );

    if ( theInfectedEdges.Contains( aPolyEdgeId ) )
      theInfectedEdges.Remove( aPolyEdgeId );
  }

  Standard_Real aRefTotalAngle = 2 * M_PI;
  TColStd_MapIteratorOfMapOfInteger anInfectedIt( theInfectedEdges );
  for ( ; anInfectedIt.More(); anInfectedIt.Next() )
  {
    Standard_Integer anEdgeId = anInfectedIt.Key();
    const BRepMesh_Edge& anInfectedEdge = GetEdge( anEdgeId );
    Standard_Integer anEdgeVertices[2] = { anInfectedEdge.FirstNode(),
                                           anInfectedEdge.LastNode() };

    Standard_Boolean isToExclude = Standard_False;
    for ( Standard_Integer i = 0; i < 2; ++i )
    {
      Standard_Integer aCurVertex = anEdgeVertices[i];
      if ( aPolyVertices.Contains( aCurVertex ) )
        continue;

      gp_XY aCenterPointXY = GetVertex( aCurVertex ).Coord();
      Standard_Real aTotalAng = 0.0;

      for ( Standard_Integer i = 1; i <= aPolyLen; ++i )
      {
        Standard_Integer aPolyEdgeId = thePolygon(i);
        const BRepMesh_Edge& aPolyEdge = GetEdge( Abs( aPolyEdgeId ) );
        Standard_Integer aStartPoint, anEndPoint;
        if ( aPolyEdgeId >= 0 )
        {
          aStartPoint = aPolyEdge.FirstNode();
          anEndPoint  = aPolyEdge.LastNode();
        }
        else
        {
          aStartPoint = aPolyEdge.LastNode();
          anEndPoint  = aPolyEdge.FirstNode();
        }

        gp_XY aStartV = GetVertex( aStartPoint ).Coord() - aCenterPointXY;
        gp_XY anEndV  = GetVertex( anEndPoint ).Coord()  - aCenterPointXY;

        Standard_Real anAngle = gp_Vec2d( anEndV ).Angle( gp_Vec2d( aStartV ) );
        aTotalAng += anAngle;
      }
      
      if ( Abs( aRefTotalAngle - aTotalAng ) > Precision::Angular() )
        isToExclude = Standard_True;
    }

    if ( isToExclude )
      anIgnoredEdges.Add( anEdgeId );
  }


  anInfectedIt.Initialize( theInfectedEdges );
  for ( ; anInfectedIt.More(); anInfectedIt.Next() )
  {
    Standard_Integer anEdgeId = anInfectedIt.Key();
    KillInternalTriangles( anEdgeId, anIgnoredEdges, theLoopEdges);
  }

  BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( theLoopEdges );
  for ( ; aLoopEdgesIt.More(); aLoopEdgesIt.Next() )
  {
    if ( myMeshData->ElemConnectedTo( aLoopEdgesIt.Key() ).IsEmpty() )
      myMeshData->RemoveLink( aLoopEdgesIt.Key() );
  }
}

//=======================================================================
//function : MeshLeftPolygonOf
//purpose  : Triangulation of the aPolygonon to the left of <theEdgeIndex>.(material side)
//=======================================================================
void BRepMesh_Delaun::MeshLeftPolygonOf( const Standard_Integer theEdgeIndex,
                                         const Standard_Boolean isForward )
{
  const BRepMesh_Edge& anEdge = GetEdge( theEdgeIndex );
  NCollection_Map<Standard_Integer> aDealLinks;
  TColStd_SequenceOfInteger aPolygon;
  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );
  TColStd_MapOfInteger anUsedEdges;
  TColStd_MapOfInteger anInfectedEdges;
  
  // Find the aPolygonon
  anUsedEdges.Add( theEdgeIndex );
  Standard_Integer aFirstNode, aStartNode, aPivotNode;
  
  if ( isForward )
  {
    aPolygon.Append( theEdgeIndex );
    aStartNode = anEdge.FirstNode();
    aPivotNode = anEdge.LastNode();
  }
  else
  {
    aPolygon.Append( -theEdgeIndex );
    aStartNode = anEdge.LastNode();
    aPivotNode = anEdge.FirstNode();
  }
  aFirstNode = aStartNode;

  const BRepMesh_Vertex& aStartVertex = GetVertex( aFirstNode );
  const BRepMesh_Vertex& anEndVertex  = GetVertex( aPivotNode );

  Standard_Integer aRefOtherNode = 0, anOtherNode = 0;
  // Check the presence of the previous edge <theEdgeIndex> :
  BRepMesh_ListOfInteger::Iterator aLinkIt( myMeshData->LinkNeighboursOf( aStartNode ) );
  for ( ; aLinkIt.More(); aLinkIt.Next() )
  {
    if ( aLinkIt.Value() != theEdgeIndex )
    {
      const BRepMesh_Edge& aNextEdge = GetEdge( aLinkIt.Value() );
      anOtherNode = aNextEdge.LastNode();
      if ( anOtherNode == aStartNode )
        anOtherNode = aNextEdge.FirstNode();
      break;
    }
  }
  if ( anOtherNode == 0 )
    return;


  gp_XY aVEdge( anEndVertex.Coord() );
  aVEdge.Subtract( aStartVertex.Coord() );
  gp_XY aPrevVEdge( aVEdge );
  gp_XY aRefCurrVEdge, aCurrVEdge;
  
  Standard_Integer aCurrEdgeId = theEdgeIndex;
  Standard_Integer aNextEdgeId;

  // Find the nearest to <theEdgeIndex> closed aPolygonon :
  Standard_Boolean isInMesh, isNotInters;
  Standard_Real anAngle, aRefAngle;

  NCollection_Vector <Bnd_Box2d> aBoxes;
  fillBndBox( aBoxes, aStartVertex, anEndVertex );
    
  while ( aPivotNode != aFirstNode )
  {
    aNextEdgeId = 0;
    if ( myPositiveOrientation )
      aRefAngle = RealFirst();
    else
      aRefAngle = RealLast();

    const BRepMesh_Vertex& aPivotVertex = GetVertex( aPivotNode );

    // Find the next edge with the greatest angle with <theEdgeIndex>
    // and non intersecting <theEdgeIndex> :
    
    aLinkIt.Init( myMeshData->LinkNeighboursOf( aPivotNode ) );
    for ( ; aLinkIt.More(); aLinkIt.Next() )
    {
      Standard_Integer aNextLink = aLinkIt.Value();
      Standard_Integer aNextLinkId = Abs( aNextLink );
      if ( aDealLinks.Contains( aNextLinkId ) )
        continue;

      if ( aNextLinkId != aCurrEdgeId )
      {
        isNotInters = Standard_True;
        const BRepMesh_Edge& aNextEdge = GetEdge( aNextLinkId );

        isInMesh = Standard_True;
        
        //define if link exist in mesh      
        if ( aNextEdge.Movability() == BRepMesh_Free )
        {
          if ( myMeshData->ElemConnectedTo( aLinkIt.Value() ).IsEmpty() ) 
            isInMesh = Standard_False;
        }

        if ( isInMesh )
        {
          anOtherNode = aNextEdge.FirstNode();
          if ( anOtherNode == aPivotNode )
            anOtherNode = aNextEdge.LastNode();

          aCurrVEdge = GetVertex( anOtherNode ).Coord();
          aCurrVEdge.Subtract( aPivotVertex.Coord() );
          
          if ( aCurrVEdge.Modulus() >= gp::Resolution() &&
               aPrevVEdge.Modulus() >= gp::Resolution() )
          {
            anAngle = gp_Vec2d( aPrevVEdge ).Angle( gp_Vec2d( aCurrVEdge ) );

            if ( ( myPositiveOrientation && anAngle > aRefAngle ) ||
                ( !myPositiveOrientation && anAngle < aRefAngle ) )
            {
              Bnd_Box2d aBox;
              aBox.Add( gp_Pnt2d( GetVertex( aNextEdge.FirstNode() ).Coord() ) );
              aBox.Add( gp_Pnt2d( GetVertex( aNextEdge.LastNode()  ).Coord() ) );
              
              for ( Standard_Integer k = 0, aLen = aPolygon.Length(); k < aLen; ++k )
              {
                if ( !aBox.IsOut( aBoxes.Value( k ) ) )
                {
                  // intersection is possible...
                  if ( IntSegSeg( aNextEdge, GetEdge( Abs( aPolygon( k + 1 ) ) ) ) )
                  {
                    isNotInters = Standard_False;
                    break;
                  }
                }
              }
              
              if( isNotInters )
              {              
                aRefAngle = anAngle;
                aRefCurrVEdge = aCurrVEdge;
                
                if ( aNextEdge.FirstNode() == aPivotNode )
                  aNextEdgeId =  aLinkIt.Value();
                else
                  aNextEdgeId = -aLinkIt.Value();
                  
                aRefOtherNode = anOtherNode;
              }
            }
          }
        }
      }
    }
    if ( aNextEdgeId != 0 )
    {
      if ( Abs( aNextEdgeId ) != theEdgeIndex && Abs( aNextEdgeId ) != aCurrEdgeId )
      {
        aCurrEdgeId = Abs( aNextEdgeId );

        if ( !anUsedEdges.Add( aCurrEdgeId ) )
        {
          //if this edge has already been added to the aPolygon, 
          //there is a risk of looping (attention to open contours)
          //remove last edge and continue
          
          aDealLinks.Add( aCurrEdgeId );

          //roll back
          continue;
        }        

        aPolygon.Append( aNextEdgeId );

        const BRepMesh_Edge& aCurrEdge = GetEdge( aCurrEdgeId );
        Standard_Integer aVert1 = aCurrEdge.FirstNode();
        Standard_Integer aVert2 = aCurrEdge.LastNode();
        fillBndBox( aBoxes, GetVertex( aVert1 ), GetVertex( aVert2 ) );

        RemovePivotTriangles( aNextEdgeId, aPivotNode,
          anInfectedEdges, aLoopEdges, (aPolygon.Length() == 2) );
      }
    }
    else
    {
      //hanging end
      if ( aPolygon.Length() == 1 )
        return;

      Standard_Integer aDeadEdgeId = Abs( aPolygon.Last() );
      const BRepMesh_Edge& aDeadEdge = GetEdge( aDeadEdgeId );

      aDealLinks.Add( aDeadEdgeId );

      anUsedEdges.Remove( aDeadEdgeId );
      aPolygon.Remove( aPolygon.Length() );

      // return to previous point
      Standard_Integer aLastValidEdge = aPolygon.Last();
      const BRepMesh_Edge& aLastEdge = GetEdge( Abs( aLastValidEdge ) );

      if( aLastValidEdge > 0 )
      {
        aStartNode = aLastEdge.FirstNode();
        aPivotNode = aLastEdge.LastNode();
      }
      else
      {
        aStartNode = aLastEdge.LastNode();
        aPivotNode = aLastEdge.FirstNode();
      }
      
      const BRepMesh_Vertex& dV = GetVertex( aStartNode );
      const BRepMesh_Vertex& fV = GetVertex( aPivotNode );
      aPrevVEdge = fV.Coord() - dV.Coord();
      continue;
    }
    
    aStartNode = aPivotNode;
    aPivotNode = aRefOtherNode;
    aPrevVEdge = aRefCurrVEdge;
  }

  CleanupPolygon( aPolygon, anInfectedEdges, aLoopEdges );
  MeshPolygon( aPolygon );
}

//=======================================================================
//function : MeshPolygon
//purpose  : Triangulatiion of a closed aPolygonon described by the list of indexes of
//           its edges in the structure. (negative index == reversed)
//=======================================================================
void BRepMesh_Delaun::MeshPolygon( TColStd_SequenceOfInteger& thePoly )
{
  Standard_Integer aPivotNode, aVertex3 = 0;
  Standard_Integer aFirstNode, aLastNode;
  Standard_Integer aTriId;

  if ( thePoly.Length() == 3 )
  {
    aTriId = myMeshData->AddElement( BRepMesh_Triangle( 
      Abs( thePoly(1) ), Abs( thePoly(2) ), Abs( thePoly(3) ), 
      thePoly(1) > 0,    thePoly(2) > 0,    thePoly(3) > 0,
      BRepMesh_Free ) );
      
    myCircles.MocAdd( aTriId );
    const BRepMesh_Edge& anEdge1 = GetEdge( Abs( thePoly(1) ) );
    const BRepMesh_Edge& anEdge2 = GetEdge( Abs( thePoly(2) ) );
    
    if ( thePoly(1) > 0)
    {
      aFirstNode = anEdge1.FirstNode();
      aLastNode  = anEdge1.LastNode();
    }
    else
    {
      aFirstNode = anEdge1.LastNode();
      aLastNode  = anEdge1.FirstNode();
    }
    
    if ( thePoly(2) > 0 ) 
      aVertex3 = anEdge2.LastNode();
    else
      aVertex3 = anEdge2.FirstNode();

    Standard_Boolean isAdded = myCircles.Add( GetVertex( aFirstNode ).Coord(), 
      GetVertex( aLastNode ).Coord(), GetVertex( aVertex3 ).Coord(), aTriId );
      
    if ( !isAdded )
      myMeshData->RemoveElement( aTriId );
  }
  else if ( thePoly.Length() > 3 )
  {
    NCollection_Vector <Bnd_Box2d> aBoxes;
    Standard_Integer aPolyIdx, aUsedIdx = 0;
    Standard_Integer aPolyLen = thePoly.Length();

    for ( aPolyIdx = 1; aPolyIdx <= aPolyLen; ++aPolyIdx )
    {
      const BRepMesh_Edge& anEdge = GetEdge( Abs( thePoly( aPolyIdx ) ) );
      aFirstNode = anEdge.FirstNode();
      aLastNode  = anEdge.LastNode();
      fillBndBox( aBoxes, GetVertex( aFirstNode ), GetVertex( aLastNode ) );
    }
    
    const BRepMesh_Edge& anEdge = GetEdge( Abs( thePoly(1) ) );
    Standard_Real aMinDist = RealLast();

    if ( thePoly(1) > 0 )
    {
      aFirstNode = anEdge.FirstNode();
      aLastNode  = anEdge.LastNode();
    }
    else
    {
      aFirstNode = anEdge.LastNode();
      aLastNode  = anEdge.FirstNode();
    }
    
    gp_XY aVEdge( GetVertex( aLastNode  ).Coord() -
                  GetVertex( aFirstNode ).Coord() );
                  
    Standard_Real aVEdgeLen = aVEdge.Modulus();
    if ( aVEdgeLen > 0.)
    {
      aVEdge.SetCoord( aVEdge.X() / aVEdgeLen,
                       aVEdge.Y() / aVEdgeLen );

      for ( aPolyIdx = 3; aPolyIdx <= aPolyLen; ++aPolyIdx )
      {
        const BRepMesh_Edge& aNextEdge = GetEdge( Abs( thePoly( aPolyIdx ) ) );
        if ( thePoly( aPolyIdx ) > 0)
          aPivotNode = aNextEdge.FirstNode();
        else
          aPivotNode = aNextEdge.LastNode();
          
        gp_XY aVEdgePivot( GetVertex( aPivotNode ).Coord() -
                           GetVertex( aLastNode  ).Coord() );

        Standard_Real aDist = aVEdge ^ aVEdgePivot;
        if ( Abs( aDist ) > Precision::PConfusion() )
        {
          if ( ( aDist > 0. &&  myPositiveOrientation ) || 
               ( aDist < 0. && !myPositiveOrientation ) )
          { 
            if ( Abs( aDist ) < aMinDist )
            {
              Bnd_Box2d aBoxFirst, aBoxLast;
              aBoxFirst.Add( gp_Pnt2d( GetVertex( aPivotNode ).Coord() ) );
              aBoxFirst.Add( gp_Pnt2d( GetVertex( aLastNode  ).Coord() ) );
              
              aBoxLast.Add( gp_Pnt2d( GetVertex( aPivotNode ).Coord() ) );
              aBoxLast.Add( gp_Pnt2d( GetVertex( aFirstNode ).Coord() ) );
              
              BRepMesh_Edge aCheckE1( aLastNode,  aPivotNode, BRepMesh_Free );
              BRepMesh_Edge aCheckE2( aFirstNode, aPivotNode, BRepMesh_Free );
              
              Standard_Boolean isIntersect = Standard_False;
              for ( Standard_Integer aPolyIdx1 = 2; aPolyIdx1 <= aPolyLen; ++aPolyIdx1 )
              {
                if( aPolyIdx1 == aPolyIdx )
                  continue;
                
                const BRepMesh_Edge& aNextEdge1 = GetEdge( Abs( thePoly( aPolyIdx1 ) ) );
                if ( !aBoxFirst.IsOut( aBoxes.Value( aPolyIdx1 - 1 ) ) )
                {                    
                  // intersection is possible...                  
                  if( IntSegSeg( aCheckE1, aNextEdge1 ) )
                  {
                    isIntersect = Standard_True;
                    break;
                  }
                }
                
                if ( !aBoxLast.IsOut( aBoxes.Value( aPolyIdx1 - 1 ) ) &&
                     !aCheckE2.IsEqual( aNextEdge1 ) )
                {
                  // intersection is possible...                  
                  if( IntSegSeg( aCheckE2, aNextEdge1 ) )
                  {
                    isIntersect = Standard_True;
                    break;
                  }
                }
              }
              
              if( !isIntersect )
              {
                aMinDist = aDist;
                aVertex3 = aPivotNode;
                aUsedIdx = aPolyIdx;
              }
            }
          }
        }
      }
    }

    Standard_Integer aNewEdge2, aNewEdge3;
    if ( aMinDist < RealLast() )
    {
      aNewEdge2 = myMeshData->AddLink( BRepMesh_Edge( aLastNode, aVertex3,   BRepMesh_Free ) );
      aNewEdge3 = myMeshData->AddLink( BRepMesh_Edge( aVertex3,  aFirstNode, BRepMesh_Free ) );
      aTriId    = myMeshData->AddElement( BRepMesh_Triangle( 
        Abs( thePoly(1) ), Abs( aNewEdge2 ), Abs( aNewEdge3 ), 
        thePoly(1) > 0,    aNewEdge2 > 0,    aNewEdge3 > 0,
        BRepMesh_Free ) );

      Standard_Boolean isAdded = myCircles.Add( GetVertex( aFirstNode ).Coord(), 
        GetVertex( aLastNode ).Coord(), GetVertex( aVertex3 ).Coord(), aTriId );
        
      if ( !isAdded )
        myMeshData->RemoveElement( aTriId );

      if ( aUsedIdx < aPolyLen )
      {
        TColStd_SequenceOfInteger aSuitePoly;
        thePoly.Split( aUsedIdx, aSuitePoly );
        aSuitePoly.Prepend( -aNewEdge3 );
        MeshPolygon( aSuitePoly );
      }
      else 
        thePoly.Remove( aPolyLen );

      if ( aUsedIdx > 3 )
      {
        thePoly.SetValue( 1, -aNewEdge2 );
        MeshPolygon( thePoly );
      }
    }
  }
}

//=======================================================================
//function : RemoveVertex
//purpose  : Removes a vertex from the triangulation
//=======================================================================
void  BRepMesh_Delaun::RemoveVertex( const BRepMesh_Vertex& theVertex )
{
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector( myMeshData );
  aSelector.NeighboursOf( theVertex );

  BRepMesh_MapOfIntegerInteger aLoopEdges( 10, myMeshData->Allocator() );

  // Loop on triangles to be destroyed :
  BRepMesh_MapOfInteger::Iterator aTriangleIt( aSelector.Elements() );
  for ( ; aTriangleIt.More(); aTriangleIt.Next() )
    DeleteTriangle( aTriangleIt.Key(), aLoopEdges );

  TColStd_SequenceOfInteger aPolygon;
  Standard_Integer aLoopEdgesCount = aLoopEdges.Extent();
  BRepMesh_MapOfIntegerInteger::Iterator aLoopEdgesIt( aLoopEdges );

  if ( aLoopEdgesIt.More() )
  {
    const BRepMesh_Edge& anEdge = GetEdge( aLoopEdgesIt.Key() );
    Standard_Integer aFirstNode = anEdge.FirstNode();
    Standard_Integer aLastNode;
    Standard_Integer aPivotNode = anEdge.LastNode();
    Standard_Integer anEdgeId   = aLoopEdgesIt.Key();
    
    Standard_Boolean isPositive = (Standard_Boolean)aLoopEdges( anEdgeId );
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

    aLoopEdges.UnBind( anEdgeId );
    
    aLastNode = aFirstNode;
    while ( aPivotNode != aLastNode )
    {
      BRepMesh_ListOfInteger::Iterator aLinkIt( myMeshData->LinkNeighboursOf( aPivotNode ) );
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
            
          aPivotNode = aCurrentNode;
          aLoopEdges.UnBind( anEdgeId );
          break;
        }
      }
      
      if ( aLoopEdgesCount <= 0 )
        break;
      --aLoopEdgesCount;
    }
    
    MeshPolygon( aPolygon );
  }
}


//=======================================================================
//function : AddVertices
//purpose  : Adds some vertices in the triangulation.
//=======================================================================
void  BRepMesh_Delaun::AddVertices( BRepMesh_Array1OfVertexOfDelaun& theVertices )
{
  BRepMesh_HeapSortVertexOfDelaun::Sort( theVertices, 
    BRepMesh_ComparatorOfVertexOfDelaun( SortingDirection, Precision::PConfusion() ) );

  Standard_Integer aLower  = theVertices.Lower();
  Standard_Integer anUpper = theVertices.Upper();
    
  TColStd_Array1OfInteger aVertexIndexes( aLower, anUpper );
  for ( Standard_Integer i = aLower; i <= anUpper; ++i )     
    aVertexIndexes(i) = myMeshData->AddNode( theVertices(i) );

  CreateTrianglesOnNewVertices( aVertexIndexes );
}

//=======================================================================
//function : UseEdge
//purpose  : Modify mesh to use the edge. Return True if done
//=======================================================================
Standard_Boolean BRepMesh_Delaun::UseEdge( const Standard_Integer theIndex )
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
//function : Result
//purpose  : Gives the Mesh data structure
//=======================================================================
const Handle( BRepMesh_DataStructureOfDelaun )& BRepMesh_Delaun::Result() const
{
  return myMeshData;
}

//=======================================================================
//function : Frontier
//purpose  : Gives the list of frontier edges
//=======================================================================
const BRepMesh_MapOfInteger& BRepMesh_Delaun::Frontier()
{
  BRepMesh_MapOfInteger::Iterator anEdgeIt( myMeshData->LinkOfDomain() );

  myMapEdges.Clear();
  for ( ; anEdgeIt.More(); anEdgeIt.Next() )
  {
    Standard_Integer anEdge = anEdgeIt.Key();
    if ( GetEdge( anEdge ).Movability() == BRepMesh_Frontier )
      myMapEdges.Add( anEdge );
  }
  
  return myMapEdges;
}

//=======================================================================
//function : InternalEdges
//purpose  : Gives the list of internal edges
//=======================================================================
const BRepMesh_MapOfInteger& BRepMesh_Delaun::InternalEdges()
{
  BRepMesh_MapOfInteger::Iterator anEdgeIt( myMeshData->LinkOfDomain() );

  myMapEdges.Clear();
  for ( ; anEdgeIt.More(); anEdgeIt.Next() )
  {
    Standard_Integer anEdge = anEdgeIt.Key();
    if ( GetEdge( anEdge ).Movability() == BRepMesh_Fixed )
      myMapEdges.Add( anEdge );
  }
  
  return myMapEdges;
}

//=======================================================================
//function : FreeEdges
//purpose  : Gives the list of free edges used only one time
//=======================================================================
const BRepMesh_MapOfInteger& BRepMesh_Delaun::FreeEdges()
{
  BRepMesh_MapOfInteger::Iterator anEdgeIt( myMeshData->LinkOfDomain() );

  myMapEdges.Clear();
  for ( ; anEdgeIt.More(); anEdgeIt.Next() )
  {
    Standard_Integer anEdge = anEdgeIt.Key();
    if ( myMeshData->ElemConnectedTo( anEdge ).Extent() <= 1 )
      myMapEdges.Add( anEdge );
  }
  
  return myMapEdges;
}

//=======================================================================
//function : calculateDist
//purpose  : Calculates distances between the given point and edges of
//           triangle
//=======================================================================
static Standard_Real calculateDist( const gp_XY theVEdges[3],
                                    const gp_XY thePoints[3],
                                    const Standard_Integer theEdgesId[3],
                                    const BRepMesh_Vertex& theVertex,
                                    Standard_Real theDistance[3],
                                    Standard_Real theSqModulus[3],
                                    Standard_Integer& theEdgeOn )
{
  Standard_Real aMinDist = -1;
  if ( !theVEdges || !thePoints || !theEdgesId || 
       !theDistance || !theSqModulus )
    return aMinDist;
    
  for( Standard_Integer i = 0; i < 3; ++i )
  {
    theSqModulus[i] = theVEdges[i].SquareModulus();
    if ( theSqModulus[i] <= EPSEPS )
      return -1;
      
    theDistance[i] = theVEdges[i] ^ ( theVertex.Coord() - thePoints[i] );
    
    Standard_Real aDist = theDistance[i] * theDistance[i];
    aDist /= theSqModulus[i];
    
    if ( aMinDist < 0 || aDist < aMinDist )
    {
      theEdgeOn = theEdgesId[i];
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
                                            Standard_Integer& theEdgeOn ) const
{
  theEdgeOn = 0;
  
  Standard_Integer e[3];
  Standard_Boolean o[3];
  Standard_Integer p[3];

  GetTriangle( theTriangleId ).Edges( e[0], e[1], e[2],
                                      o[0], o[1], o[2] );
                                      
  const BRepMesh_Edge* anEdges[3] = { &GetEdge( e[0] ),
                                      &GetEdge( e[1] ),
                                      &GetEdge( e[2] ) };
  if ( o[0] )
  {
    p[0] = anEdges[0]->FirstNode();
    p[1] = anEdges[0]->LastNode();
  }
  else
  {
    p[1] = anEdges[0]->FirstNode();
    p[0] = anEdges[0]->LastNode();
  }
  
  if ( o[2] )
    p[2] = anEdges[2]->FirstNode();
  else
    p[2] = anEdges[2]->LastNode();

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

  Standard_Real aMinDist;  
  aMinDist = calculateDist( aVEdges, aPoints, e, theVertex, aDistance, aSqModulus, theEdgeOn );
  if ( aMinDist < 0 )
    return Standard_False;
      
  if ( aMinDist > EPSEPS )
  {
    Standard_Integer anEdgeId = theEdgeOn;
    theEdgeOn = 0;
    
    if ( anEdgeId != 0 ) 
    {
      Standard_Integer i = 0;
      for ( ; i < 3; ++i )
      {
        if( e[i] == anEdgeId )
          break;
      }
      
      if( anEdges[i]->Movability() != BRepMesh_Free )
        if ( aDistance[i] < ( aSqModulus[i] / 5. ) )
          theEdgeOn = e[i];
    }
  }

  return ( aDistance[0] + aDistance[1] + aDistance[2] != 0. &&
            ( ( aDistance[0] >= 0. && aDistance[1] >= 0. && aDistance[2] >= 0. ) ||
              ( aDistance[0] <= 0. && aDistance[1] <= 0. && aDistance[2] <= 0. ) ) );
}


//=============================================================================
// Function: classifyPoint
// This function is used for point classifying in case of coincidence of two 
// vectors.
// Returns zero value if point is out of segment and non zero value if point is
// between the first and the second point of segment.
//
// thePoint1       - the start point of a segment (base point)
// thePoint2       - the end point of a segment
// thePointToCheck - the point to classify
//=============================================================================
static Standard_Integer classifyPoint( const gp_XY& thePoint1,
                                       const gp_XY& thePoint2,
                                       const gp_XY& thePointToCheck )
{
  gp_XY aP1 = thePoint2       - thePoint1;
  gp_XY aP2 = thePointToCheck - thePoint1;
  
  Standard_Real aDist = Abs( aP1 ^ aP2 );
  if ( aDist >= Precision::PConfusion() )
  {
    aDist = ( aDist * aDist ) / aP1.SquareModulus();
    if ( aDist >= EPSEPS )
      return 0; //out
  }
    
  gp_XY aMult = aP1.Multiplied( aP2 );
  if ( aMult.X() < 0.0 || aMult.Y() < 0.0 )
    return 0; //out
    
  if ( aP1.SquareModulus() < aP2.SquareModulus() )
    return 0; //out
    
  if ( thePointToCheck.IsEqual( thePoint1, Precision::PConfusion() ) || 
       thePointToCheck.IsEqual( thePoint2, Precision::PConfusion() ) )
    return -1; //end point
    
  return 1;
}

//=============================================================================
// Function: IntSegSeg
//=============================================================================
Standard_Boolean BRepMesh_Delaun::IntSegSeg( const BRepMesh_Edge& theEdg1,
                                             const BRepMesh_Edge& theEdg2 )
{
  gp_XY p1, p2, p3, p4;
  p1 = GetVertex( theEdg1.FirstNode() ).Coord();
  p2 = GetVertex( theEdg1.LastNode()  ).Coord();
  p3 = GetVertex( theEdg2.FirstNode() ).Coord();
  p4 = GetVertex( theEdg2.LastNode()  ).Coord();
  
  Standard_Integer aPoint1 = classifyPoint( p1, p2, p3 );
  Standard_Integer aPoint2 = classifyPoint( p1, p2, p4 );
  Standard_Integer aPoint3 = classifyPoint( p3, p4, p1 );
  Standard_Integer aPoint4 = classifyPoint( p3, p4, p2 );
  
  if ( aPoint1 > 0 || aPoint2 > 0 ||
       aPoint3 > 0 || aPoint4 > 0 )
    return Standard_True;
                    
  gp_XY aPl1 = p2 - p1;
  gp_XY aPl2 = p4 - p3;
  gp_XY aPl3 = p1 - p3;
    
  Standard_Real aCrossD1D2 = aPl1 ^ aPl2;
  Standard_Real aCrossD1D3 = aPl1 ^ aPl3;
  if ( Abs( aCrossD1D2 ) < Precision::PConfusion() )
  {
    if( Abs( aCrossD1D3 ) < Precision::PConfusion() )
    {
      Standard_Integer aPosHash = aPoint1 + aPoint2;
      if ( ( !aPosHash && aPoint3 ) ) //|| aPosHash < -1 )
        return Standard_True;
        
      return Standard_False;
    }
    else
      //parallel case
      return Standard_False;
  }

  Standard_Real aPar = aCrossD1D3 / aCrossD1D2;
  // inrersects out of first segment range
  if( aPar < Precision::Angular() || aPar > 1 - Precision::Angular() )
    return Standard_False;
 
  Standard_Real aCrossD2D3 = aPl2 ^ aPl3;
  aPar = aCrossD2D3 / aCrossD1D2;
  // inrersects out of second segment range
  if( aPar < Precision::Angular() || aPar > 1 - Precision::Angular() )
    return Standard_False;
 
  return Standard_True;
}
