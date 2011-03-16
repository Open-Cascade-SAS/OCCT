// File:	MeshVS_MeshPrsBuilder.cxx
// Created:	Tue Sep 16 2003
// Author:	Alexander SOLOVYOV
// Copyright:	 Open CASCADE 2003

#include <MeshVS_MeshPrsBuilder.ixx>

#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>

#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Group.hxx>

#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Select3D_SensitivePoint.hxx>

#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_Tool.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_MapOfTwoNodes.hxx>
#include <MeshVS_Buffer.hxx>

//================================================================
// Function : Constructor MeshVS_MeshPrsBuilder
// Purpose  :
//================================================================
MeshVS_MeshPrsBuilder::MeshVS_MeshPrsBuilder ( const Handle(MeshVS_Mesh)& Parent,
                                               const Standard_Integer& DisplayModeMask,
                                               const Handle (MeshVS_DataSource)& DS,
                                               const Standard_Integer Id,
                                               const MeshVS_BuilderPriority& Priority )
: MeshVS_PrsBuilder ( Parent, DisplayModeMask, DS, Id, Priority )
{
}

//================================================================
// Function : Build
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::Build ( const Handle(Prs3d_Presentation)& Prs,
                                    const TColStd_PackedMapOfInteger& IDs,
                                    TColStd_PackedMapOfInteger& IDsToExclude,
                                    const Standard_Boolean IsElement,
                                    const Standard_Integer DisplayMode ) const
{
  if ( DisplayMode <= 0 )
    return;

  Standard_Boolean HasHilightFlag = ( ( DisplayMode & MeshVS_DMF_HilightPrs ) != 0 );
  Standard_Integer Extent = IDs.Extent();

  if ( HasHilightFlag && Extent == 1)
    BuildHilightPrs ( Prs, IDs, IsElement );
  else if ( IsElement )
    BuildElements ( Prs, IDs, IDsToExclude, DisplayMode );
  else
    BuildNodes ( Prs, IDs, IDsToExclude, DisplayMode );
}

//================================================================
// Function : BuildNodes
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::BuildNodes ( const Handle(Prs3d_Presentation)& Prs,
                                         const TColStd_PackedMapOfInteger& IDs,
                                         TColStd_PackedMapOfInteger& IDsToExclude,
                                         const Standard_Integer DisplayMode ) const
{
  Handle( MeshVS_DataSource ) aSource = GetDataSource();
  Handle( MeshVS_Drawer )     aDrawer = GetDrawer();
  Handle (Graphic3d_AspectMarker3d) aNodeMark =
    MeshVS_Tool::CreateAspectMarker3d( GetDrawer() );
  if ( aSource.IsNull() || aDrawer.IsNull() || aNodeMark.IsNull() )
    return;

  Standard_Boolean DisplayFreeNodes = Standard_True;
  aDrawer->GetBoolean( MeshVS_DA_DisplayNodes, DisplayFreeNodes );
  Standard_Boolean HasSelectFlag = ( ( DisplayMode & MeshVS_DMF_SelectionPrs ) != 0 );
  Standard_Boolean HasHilightFlag = ( ( DisplayMode & MeshVS_DMF_HilightPrs ) != 0 );

  Standard_Real aCoordsBuf[ 3 ];
  TColStd_Array1OfReal aCoords( *aCoordsBuf, 1, 3 );
  Standard_Integer NbNodes;
  MeshVS_EntityType aType;

  if ( !DisplayFreeNodes )
    return;

  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  if ( !HasSelectFlag && !HasHilightFlag )
  {
    // subtract the hidden nodes and ids to exclude (to minimise allocated memory)
    Handle(TColStd_HPackedMapOfInteger) aHiddenNodes = myParentMesh->GetHiddenNodes();
    if ( !aHiddenNodes.IsNull() )
      anIDs.Subtract( aHiddenNodes->Map() );
  }
  anIDs.Subtract( IDsToExclude );

  Standard_Integer upper = anIDs.Extent();
  if ( upper<=0 )
    return;

  Graphic3d_Array1OfVertex aNodePoints ( 1, upper );
  Standard_Integer k=0;
  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);
  for( ; it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();
    if ( aSource->GetGeom ( aKey, Standard_False, aCoords, NbNodes, aType ) )
    {
      if ( IsExcludingOn() )
        IDsToExclude.Add (aKey);
       k++;
      aNodePoints.SetValue ( k, Graphic3d_Vertex ( aCoords(1), aCoords(2), aCoords(3) ) );
    }
  }

  if ( k>0 )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aNodeGroup = Prs3d_Root::CurrentGroup ( Prs );
    aNodeGroup->SetPrimitivesAspect ( aNodeMark );

    aNodeGroup->BeginPrimitives();
    aNodeGroup->MarkerSet ( aNodePoints );
    aNodeGroup->EndPrimitives();
  }
}

//================================================================
// Function : BuildElements
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::BuildElements( const Handle(Prs3d_Presentation)& Prs,
                                           const TColStd_PackedMapOfInteger& IDs,
                                           TColStd_PackedMapOfInteger& IDsToExclude,
                                           const Standard_Integer DisplayMode ) const
{
  Standard_Integer maxnodes;   

  Handle (MeshVS_DataSource) aSource = GetDataSource();
  if ( aSource.IsNull() )
    return;

  Handle( MeshVS_Drawer ) aDrawer = GetDrawer();
  if ( aDrawer.IsNull() || !aDrawer->GetInteger ( MeshVS_DA_MaxFaceNodes, maxnodes ) ||
       maxnodes <= 0 )
    return;

  //----------- extract useful display mode flags ----------
  Standard_Integer aDispMode = ( DisplayMode & GetFlags() );
  if ( ( aDispMode & MeshVS_DMF_DeformedMask) != 0 )
  {
    aDispMode /= MeshVS_DMF_DeformedPrsWireFrame;
    // This transformation turns deformed mesh flags to real display modes
  }
  aDispMode &= MeshVS_DMF_OCCMask;
  //--------------------------------------------------------

  Standard_Real aShrinkCoef;
  aDrawer->GetDouble ( MeshVS_DA_ShrinkCoeff, aShrinkCoef );

  Standard_Boolean IsWireFrame    = ( aDispMode==MeshVS_DMF_WireFrame ),
                   IsShading      = ( aDispMode==MeshVS_DMF_Shading ),
                   IsShrink       = ( aDispMode==MeshVS_DMF_Shrink ),
                   HasHilightFlag = ( ( DisplayMode & MeshVS_DMF_HilightPrs )   != 0 ),
                   HasSelectFlag  = ( ( DisplayMode & MeshVS_DMF_SelectionPrs ) != 0 ),
                   IsMeshReflect, IsMeshAllowOverlap, IsReflect, IsMeshSmoothShading = Standard_False;

  aDrawer->GetBoolean  ( MeshVS_DA_Reflection, IsMeshReflect );
  aDrawer->GetBoolean  ( MeshVS_DA_IsAllowOverlapped, IsMeshAllowOverlap );
  IsReflect      = ( IsMeshReflect && !HasHilightFlag );
  aDrawer->GetBoolean  ( MeshVS_DA_SmoothShading, IsMeshSmoothShading );

  // display mode for hilighted prs of groups
  IsShrink       = ( IsShrink && !HasHilightFlag );
  IsShading      = ( IsShading || HasHilightFlag );

  //---------- Creating AspectFillArea3d and AspectLine3d -------------
  Graphic3d_MaterialAspect AMat;
  aDrawer->GetMaterial ( MeshVS_DA_FrontMaterial, AMat );
  if ( !IsReflect )
  {
    AMat.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
    AMat.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
    AMat.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
    AMat.SetReflectionModeOff(Graphic3d_TOR_EMISSION);
  }
  Handle( Graphic3d_AspectFillArea3d ) aFill = MeshVS_Tool::CreateAspectFillArea3d( GetDrawer(), AMat );
  Handle( Graphic3d_AspectLine3d ) aBeam = MeshVS_Tool::CreateAspectLine3d ( GetDrawer() );
  //-------------------------------------------------------------------

  Standard_Boolean IsOverlapControl =
    !IsMeshAllowOverlap && ( IsWireFrame || IsShading ) && !HasSelectFlag;

  Handle (Graphic3d_ArrayOfPolygons)  aPolygons, aVolumes;
  Handle (Graphic3d_ArrayOfPolylines) aPolylines, aLinkPolylines;

  Standard_Integer PolygonVerticesFor3D = 0, PolygonBoundsFor3D = 0,
                   PolylineVerticesFor3D = 0, PolylineBoundsFor3D = 0,
                   NbNodes;
    
  // subtract the hidden elements and ids to exclude (to minimise allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
  if ( !aHiddenElems.IsNull() )
    anIDs.Subtract( aHiddenElems->Map() );
  anIDs.Subtract( IDsToExclude );

  Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;
  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);
  for( ; it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();
    if( aSource->Get3DGeom( aKey, NbNodes, aTopo ) )
    {
      if( !HasSelectFlag )
        HowManyPrimitives( aTopo, Standard_True,  HasSelectFlag, NbNodes,
                           PolygonVerticesFor3D, PolygonBoundsFor3D );      
      HowManyPrimitives( aTopo, Standard_False, HasSelectFlag, NbNodes,
                         PolylineVerticesFor3D, PolylineBoundsFor3D );
    }
  }

  Standard_Integer Extent = anIDs.Extent();

  if ( IsReflect )
  {
    aPolygons = new Graphic3d_ArrayOfPolygons
      ( maxnodes*Extent, Extent, 0, Standard_True );
    aVolumes = new Graphic3d_ArrayOfPolygons
      ( PolygonVerticesFor3D, PolygonBoundsFor3D, 0, Standard_True );
  }
  else
  {
    aPolygons = new Graphic3d_ArrayOfPolygons( maxnodes*Extent, Extent );
    aVolumes = new Graphic3d_ArrayOfPolygons ( PolygonVerticesFor3D, PolygonBoundsFor3D );
  }

  Standard_Integer howMany = 1;
  if ( IsOverlapControl )
    howMany = 2;

  Standard_Boolean showEdges = Standard_True;
  aDrawer->GetBoolean( MeshVS_DA_ShowEdges, showEdges );

  showEdges = IsWireFrame || showEdges;
  if ( showEdges )
  {
    aPolylines = new Graphic3d_ArrayOfPolylines ( ( maxnodes+1 )*Extent + PolylineVerticesFor3D,
                                                  howMany * Extent + PolylineBoundsFor3D );
    aLinkPolylines = new Graphic3d_ArrayOfPolylines ( 2*Extent, Extent );
  }

  MeshVS_Buffer aCoordsBuf (3*maxnodes*sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords( aCoordsBuf, 1, 3*maxnodes );  
  MeshVS_EntityType aType;

  TColStd_PackedMapOfInteger aCustomElements;

  Quantity_Color       anOldEdgeColor;
  Aspect_InteriorStyle anIntType;
  Quantity_Color       anIntColor, anEdgeColor;
  Aspect_TypeOfLine    aLine;
  Standard_Real        aWidth;

  MeshVS_TwoNodes aTwoNodes;
  aFill->Values ( anIntType, anIntColor, anEdgeColor, aLine, aWidth );
  
  MeshVS_MapOfTwoNodes aLinkNodes;
  if ( showEdges && IsOverlapControl )
    // Forbid drawings of edges, which overlap with some links
    for( it.Reset(); it.More(); it.Next() )
    {
      Standard_Integer aKey = it.Key();
      if ( aSource->GetGeomType ( aKey, Standard_True, aType ) &&
           aType == MeshVS_ET_Link )
      {
        MeshVS_Buffer aNodesBuf (maxnodes * sizeof(Standard_Integer));
        TColStd_Array1OfInteger aNodes (aNodesBuf, 1, maxnodes);
        Standard_Integer nbNodes;
        if ( aSource->GetNodesByElement( aKey, aNodes, nbNodes ) &&
             nbNodes == 2 )
        {
          aTwoNodes.First = aNodes(1);
          aTwoNodes.Second = aNodes(2);
          aLinkNodes.Add ( aTwoNodes );
        }
      }
    }

  for( it.Reset(); it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();

    if ( ! aSource->GetGeom ( aKey, Standard_True, aCoords, NbNodes, aType ) )
      continue;

    switch ( aType )
    {
      case MeshVS_ET_Volume :
        if( IsExcludingOn() )
          IDsToExclude.Add (aKey);
        if( aSource->Get3DGeom( aKey, NbNodes, aTopo ) )
        {
          // !!! TO DO: Overlap control
          // add wireframe presentation (draw edges for shading mode as well)
          if ( showEdges )
            AddVolumePrs ( aTopo, aCoords, NbNodes, aPolylines, IsReflect, IsShrink, HasSelectFlag, aShrinkCoef );

          // add shading presentation
          if ( ( IsShading || IsShrink ) && !HasSelectFlag )
            AddVolumePrs ( aTopo, aCoords, NbNodes, aVolumes, IsReflect, IsShrink, HasSelectFlag, aShrinkCoef );

          /*
          Handle( Graphic3d_ArrayOfPrimitives ) anArr = aVolumes;
          if( IsWireFrame || HasSelectFlag )
            anArr = aPolylines;

          AddVolumePrs ( aTopo, aCoords, NbNodes, anArr, IsReflect, IsShrink, HasSelectFlag, aShrinkCoef );
          */
        }
        break;

      case MeshVS_ET_Link :
        if( IsExcludingOn() )
          IDsToExclude.Add (aKey);
        if ( showEdges )
          AddLinkPrs ( aCoords, aLinkPolylines, IsShrink || HasSelectFlag, aShrinkCoef );
        break;

      case MeshVS_ET_Face :
        if( IsExcludingOn() )
          IDsToExclude.Add (aKey);
        if ( showEdges && IsOverlapControl )
        {
          MeshVS_Buffer aNodesBuf (NbNodes * sizeof(Standard_Integer));
          TColStd_Array1OfInteger aNodes (aNodesBuf, 1, NbNodes);
          if ( !aSource->GetNodesByElement( aKey, aNodes, NbNodes ) )
            continue;

          Standard_Integer Last = 0;
          for ( Standard_Integer i=1; i<=NbNodes; i++ )
          {
            if ( i==1 )
              aTwoNodes.First = aNodes(i);
            else
              aTwoNodes.First = aTwoNodes.Second;

            aTwoNodes.Second = ( i<NbNodes ? aNodes(i+1) : aNodes(1) );
            if ( aLinkNodes.Contains ( aTwoNodes ) )
              {
                aPolylines->AddBound ( i-Last );
                for (Standard_Integer j = Last+1; j<=i; j++)
                  aPolylines->AddVertex ( aCoords(3*j-2), aCoords(3*j-1), aCoords(3*j) );
                Last = i;
              }
          }
          if ( NbNodes-Last > 0 )
          {
            aPolylines->AddBound ( NbNodes-Last+1 );
            for (Standard_Integer j = Last+1; j<=NbNodes; j++)
              aPolylines->AddVertex ( aCoords(3*j-2), aCoords(3*j-1), aCoords(3*j) );

            aPolylines->AddVertex ( aCoords(1), aCoords(2), aCoords(3) );
          }
        }

        if ( !IsOverlapControl || IsShading )
        {
          if ( !IsOverlapControl && showEdges )
            AddFaceWirePrs(aCoords, NbNodes, aPolylines,
                           IsShrink || HasSelectFlag, aShrinkCoef );
          if ( ( IsShading || IsShrink ) && !HasSelectFlag )
            AddFaceSolidPrs(aKey, aCoords, NbNodes, maxnodes, aPolygons,
                            IsReflect, IsShrink || HasSelectFlag,
                            aShrinkCoef, IsMeshSmoothShading );
        }
        break;

      default:
        aCustomElements.Add( aKey );
        break;
    }
  }

  if ( IsShrink )
  {
    anOldEdgeColor = anEdgeColor;
    aFill->SetEdgeColor ( Quantity_NOC_BLACK );
  }

  DrawArrays ( Prs, aPolygons, aPolylines, aLinkPolylines, aVolumes,
    !showEdges, HasSelectFlag, aFill, aBeam );

  if ( !aCustomElements.IsEmpty() )
    CustomBuild ( Prs, aCustomElements, IDsToExclude, DisplayMode );

  if( IsShrink )
    aFill->SetEdgeColor( anOldEdgeColor );
}

//================================================================
// Function : BuildHilightPrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::BuildHilightPrs ( const Handle(Prs3d_Presentation)& Prs,
                                              const TColStd_PackedMapOfInteger& IDs,
                                              const Standard_Boolean IsElement ) const
{
  Standard_Integer maxnodes;

  Handle (MeshVS_DataSource) aSource = GetDataSource();
  if ( aSource.IsNull() || IDs.IsEmpty() )
    return;

  Handle( MeshVS_Drawer ) aDrawer = GetDrawer();
  if ( aDrawer.IsNull() || !aDrawer->GetInteger ( MeshVS_DA_MaxFaceNodes, maxnodes ) ||
       maxnodes <= 0 )
    return;

  MeshVS_Buffer aCoordsBuf (3*maxnodes*sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords (aCoordsBuf, 1, 3*maxnodes);

  Graphic3d_MaterialAspect AMat;
  aDrawer->GetMaterial ( MeshVS_DA_FrontMaterial, AMat );
  AMat.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
  AMat.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
  AMat.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
  AMat.SetReflectionModeOff(Graphic3d_TOR_EMISSION);

  Handle( Graphic3d_AspectFillArea3d ) aFill     = MeshVS_Tool::CreateAspectFillArea3d( GetDrawer(), AMat );
  Handle( Graphic3d_AspectLine3d )     aBeam     = MeshVS_Tool::CreateAspectLine3d( GetDrawer() );
  Handle( Graphic3d_AspectMarker3d )   aNodeMark = MeshVS_Tool::CreateAspectMarker3d( GetDrawer() );

  // Hilight one element or node
  TColStd_MapIteratorOfPackedMapOfInteger it (IDs);
  Standard_Integer ID = it.Key(), NbNodes;
  MeshVS_EntityType aType;

  if ( !aSource->GetGeom ( ID, IsElement, aCoords, NbNodes, aType ) )
    return;

  Prs3d_Root::NewGroup ( Prs );
  Handle (Graphic3d_Group) aHilightGroup = Prs3d_Root::CurrentGroup ( Prs );

  switch ( aType )
  {
    case MeshVS_ET_Node :
    {
      aHilightGroup->SetPrimitivesAspect ( aNodeMark );
      aHilightGroup->Marker ( Graphic3d_Vertex ( aCoords(1), aCoords(2), aCoords(3) ) );
    }
    break;

    case MeshVS_ET_Link:
    {
      aHilightGroup->SetPrimitivesAspect ( aBeam );
      aHilightGroup->Polyline ( Graphic3d_Vertex ( aCoords(1), aCoords(2), aCoords(3) ),
                                Graphic3d_Vertex ( aCoords(4), aCoords(5), aCoords(6) ) );
    }
    break;

    case MeshVS_ET_Face:
    if ( NbNodes > 0 )
    {
      Standard_Real X, Y, Z;
      aHilightGroup->SetPrimitivesAspect ( aFill );
      Graphic3d_Array1OfVertex aVArr ( 1, NbNodes );

      for ( Standard_Integer k=1; k<=NbNodes; k++)
      {
        X = aCoords(3*k-2);
        Y = aCoords(3*k-1);
        Z = aCoords(3*k);
        aVArr.SetValue ( k, Graphic3d_Vertex ( X, Y, Z ) );
      }
      aHilightGroup->Polygon ( aVArr );
    }
    break;

    case MeshVS_ET_Volume:
    if( NbNodes > 0 )
    {
      Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;

      aHilightGroup->SetPrimitivesAspect ( aFill );

      if( aSource->Get3DGeom( ID, NbNodes, aTopo ) )
      {
        Standard_Integer low = aTopo->Lower(), up = aTopo->Upper(), i, j, m, ind;
        for( i=low; i<=up; i++ )
        {
          const TColStd_SequenceOfInteger& aSeq = aTopo->Value( i );
          m = aSeq.Length();
          Graphic3d_Array1OfVertex aVArr( 1, m );
          for( j=1; j<=m; j++ )
          {
            ind = aSeq.Value( j );
            aVArr.SetValue( j, Graphic3d_Vertex( aCoords( 3*ind+1 ),
                                                 aCoords( 3*ind+2 ), 
                                                 aCoords( 3*ind+3 ) ) );
          }
          aHilightGroup->Polygon ( aVArr );
        }
      }
    }
    break;

    default:
      {
        TColStd_PackedMapOfInteger tmp;
        CustomBuild ( Prs, IDs, tmp, MeshVS_DMF_HilightPrs );
      }
    break;
  }
}

//================================================================
// Function : AddLinkPrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::AddLinkPrs (const TColStd_Array1OfReal& theCoords,
                                        const Handle(Graphic3d_ArrayOfPolylines)& theLines,
                                        const Standard_Boolean IsShrinked,
                                        const Standard_Real ShrinkCoef) const
{
  Standard_Real x1 = theCoords(1);
  Standard_Real y1 = theCoords(2);
  Standard_Real z1 = theCoords(3);
  Standard_Real x2 = theCoords(4);
  Standard_Real y2 = theCoords(5);
  Standard_Real z2 = theCoords(6);
  Standard_Real xG, yG, zG;

  if ( IsShrinked )
  {
    xG = (x1+x2)/2.0;
    yG = (y1+y2)/2.0;
    zG = (z1+z2)/2.0;
    x1 = (x1 - xG) * ShrinkCoef + xG;
    x2 = 2.0*xG - x1;
    y1 = (y1 - yG) * ShrinkCoef + yG;
    y2 = 2.0*yG - y1;
    z1 = (z1 - zG) * ShrinkCoef + zG;
    z2 = 2.0*zG - z1;
  }
  theLines->AddBound ( 2 );
  theLines->AddVertex ( x1, y1, z1 );
  theLines->AddVertex ( x2, y2, z2 );
}

//================================================================
// Function : AddFaceWirePrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::AddFaceWirePrs (const TColStd_Array1OfReal& theCoords,
                                            const Standard_Integer NbNodes,
                                            const Handle(Graphic3d_ArrayOfPolylines)& theLines,
                                            const Standard_Boolean IsShrinked,
                                            const Standard_Real ShrinkCoef) const
{
  Standard_Real xG, yG, zG, X, Y, Z, startX=0., startY=0., startZ=0.;
  theLines->AddBound ( NbNodes+1 );
  if ( IsShrinked )
    CalculateCenter( theCoords, NbNodes, xG, yG, zG );

  for ( Standard_Integer k=1; k<=NbNodes; k++)
  {
    X = theCoords(3*k-2);
    Y = theCoords(3*k-1);
    Z = theCoords(3*k);
    if ( IsShrinked )
    {
      X = (X - xG) * ShrinkCoef + xG;
      Y = (Y - yG) * ShrinkCoef + yG;
      Z = (Z - zG) * ShrinkCoef + zG;
    }
    if( k==1 )
    {
      startX = X; startY = Y; startZ = Z;
    }
    theLines->AddVertex ( X, Y, Z );
  }
  theLines->AddVertex( startX, startY, startZ );
}

//================================================================
// Function : AddFaceSolidPrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::AddFaceSolidPrs (const Standard_Integer ID,
                                             const TColStd_Array1OfReal& theCoords,
                                             const Standard_Integer NbNodes,
                                             const Standard_Integer MaxNodes,
                                             const Handle(Graphic3d_ArrayOfPolygons)& thePolygons,
                                             const Standard_Boolean IsReflected,
                                             const Standard_Boolean IsShrinked,
                                             const Standard_Real ShrinkCoef,
                                             const Standard_Boolean IsMeshSmoothShading) const
{
  Handle( MeshVS_DataSource ) aDS = myParentMesh->GetDataSource();
  if ( aDS.IsNull() )
    return;

  Standard_Real xG, yG, zG, X, Y, Z, nx, ny, nz;
  thePolygons->AddBound ( NbNodes );
  if ( IsShrinked )
    CalculateCenter( theCoords, NbNodes, xG, yG, zG );

  Standard_Boolean allNormals = Standard_True;
  Standard_Integer k;

  if( IsReflected )
  {
    if( IsMeshSmoothShading )
      for( k=1; k<=NbNodes && allNormals; k++ )
        allNormals = aDS->GetNodeNormal (k, ID, nx, ny, nz);
    if( !IsMeshSmoothShading || !allNormals )
      aDS->GetNormal( ID, MaxNodes, nx, ny, nz );
  }

  for ( k=1; k<=NbNodes; k++)
  {
    X = theCoords(3*k-2);
    Y = theCoords(3*k-1);
    Z = theCoords(3*k);
    if ( IsShrinked )
    {
      X = (X - xG) * ShrinkCoef + xG;
      Y = (Y - yG) * ShrinkCoef + yG;
      Z = (Z - zG) * ShrinkCoef + zG;
    }

    if ( IsReflected )
    {
      if( IsMeshSmoothShading && allNormals )
        aDS->GetNodeNormal (k, ID, nx, ny, nz);
      thePolygons->AddVertex ( X, Y, Z, nx, ny, nz );
    }
    else
      thePolygons->AddVertex ( X, Y, Z );
  }
}

//================================================================
// Function : AddVolumePrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::AddVolumePrs (const Handle( MeshVS_HArray1OfSequenceOfInteger )& Topo,
                                          const TColStd_Array1OfReal& Nodes,
                                          const Standard_Integer NbNodes,
                                          const Handle( Graphic3d_ArrayOfPrimitives )& Array,
                                          const Standard_Boolean IsReflected,
                                          const Standard_Boolean IsShrinked,
                                          const Standard_Boolean IsSelect,
                                          const Standard_Real ShrinkCoef)
{
  Standard_Real c[3]; c[0] = c[1] = c[2] = 0.0;
  Standard_Integer low = Nodes.Lower(), n=NbNodes;

  if( Topo.IsNull() || Array.IsNull() )
    return;

  if( IsShrinked )
  {
    for( Standard_Integer i=0; i<3*n; i++ )
      c[i%3] += Nodes.Value( low + i );

    c[0] /= n;
    c[1] /= n;
    c[2] /= n;
  }

  Standard_Boolean IsPolygons = Array->IsKind( STANDARD_TYPE( Graphic3d_ArrayOfPolygons ) );
  Standard_Real x[2], y[2], z[2];
  Standard_Integer ind;
  gp_Vec norm;  

  if( IsPolygons )
  {    
    for( Standard_Integer i=Topo->Lower(), topoup=Topo->Upper(); i<=topoup; i++ )
    {
      const TColStd_SequenceOfInteger& aSeq = Topo->Value( i );
      
      Standard_Integer m = aSeq.Length();
      Array->AddBound( m );
      
      norm.SetCoord( 0, 0, 0 );
      if( IsReflected )
      {
        MeshVS_Buffer PolyNodesBuf (3*m*sizeof(Standard_Real));
        TColStd_Array1OfReal PolyNodes( PolyNodesBuf, 0, 3*m );
        PolyNodes.SetValue( 0, m );
        for( Standard_Integer j=1; j<=m; j++ )
        {          
          ind = aSeq.Value( j );
          PolyNodes.SetValue( 3*j-2, Nodes.Value( low+3*ind ) );
          PolyNodes.SetValue( 3*j-1, Nodes.Value( low+3*ind+1 ) );
          PolyNodes.SetValue( 3*j,   Nodes.Value( low+3*ind+2 ) );
        }

        // compute normal
        // if( !MeshVS_Tool::GetNormal( PolyNodes, norm ) )
        //   norm.SetCoord( 0, 0, 0 );
        MeshVS_Tool::GetAverageNormal( PolyNodes, norm );
      }

      Standard_Real nx = norm.X(), ny = norm.Y(), nz = norm.Z();
       

      for( Standard_Integer j=1; j<=m; j++ )
      {
        ind = aSeq.Value( j );
        x[0] = Nodes.Value( low + 3*ind   );
        y[0] = Nodes.Value( low + 3*ind+1 );
        z[0] = Nodes.Value( low + 3*ind+2 );

        if( IsShrinked )
        {
          x[0] = c[0] + ShrinkCoef * ( x[0]-c[0] );
          y[0] = c[1] + ShrinkCoef * ( y[0]-c[1] );
          z[0] = c[2] + ShrinkCoef * ( z[0]-c[2] );
        }
    
        if( IsReflected )
          Array->AddVertex( x[0], y[0], z[0], nx, ny, nz );
        else
          Array->AddVertex( x[0], y[0], z[0] );
      }
    }
  }
  else if( IsSelect )
  {
    for( Standard_Integer i=Topo->Lower(), topoup=Topo->Upper(); i<=topoup; i++ )
    {
      const TColStd_SequenceOfInteger& aSeq = Topo->Value( i );
      Standard_Real pc[3]; pc[0] = pc[1] = pc[2] = 0;
      Standard_Integer j,m;
      for( j=1, m=aSeq.Length(); j<=m; j++ )
      {
        ind = aSeq.Value( j );
        for( Standard_Integer k=0; k<3; k++ )
          pc[k] += Nodes.Value( low + 3*ind+k );
      }
      pc[0] /= m;
      pc[1] /= m;
      pc[2] /= m;

      Array->AddBound( m+1 );
      for( j=1, m=aSeq.Length(); j<=m+1; j++ )
      {
        ind = aSeq.Value( (j-1)%m + 1 );
        x[0] = Nodes.Value( low + 3*ind   );
        y[0] = Nodes.Value( low + 3*ind+1 );
        z[0] = Nodes.Value( low + 3*ind+2 );
        x[0] = pc[0] + ShrinkCoef * ( x[0]-pc[0] );
        y[0] = pc[1] + ShrinkCoef * ( y[0]-pc[1] );
        z[0] = pc[2] + ShrinkCoef * ( z[0]-pc[2] );
        Array->AddVertex( x[0], y[0], z[0] );
      }
    }
  }
  else
  {
    Standard_Integer F, S=0, k;

    // Find all pairs of nodes (edges) to draw;
    // map is used to ensure that each edge will be drawn only once
    TColStd_PackedMapOfInteger aMap;
    for( Standard_Integer i=Topo->Lower(), topoup=Topo->Upper(); i<=topoup; i++ )
    {
      const TColStd_SequenceOfInteger& aSeq = Topo->Value( i );
      for( Standard_Integer j=1, m=aSeq.Length(); j<=m; j++ )
      {
        if( j==1 )
          F = aSeq.Value( j );
        else
          F = S;

        S = j<m ? aSeq.Value( j+1 ) : aSeq.Value( 1 );

        if( F<S )
          aMap.Add( F + NbNodes * S );
        else
          aMap.Add( S + NbNodes * F );
      }
    }

    // draw edges
    TColStd_MapIteratorOfPackedMapOfInteger anIt( aMap );
    for( ; anIt.More(); anIt.Next() )
    {      
      F = low + 3*(anIt.Key()%NbNodes);
      S = low + 3*(anIt.Key()/NbNodes);

      x[0] = Nodes.Value( F );
      y[0] = Nodes.Value( F+1 );
      z[0] = Nodes.Value( F+2 );
      x[1] = Nodes.Value( S );
      y[1] = Nodes.Value( S+1 );
      z[1] = Nodes.Value( S+2 );

      if( IsShrinked )
        for( k=0; k<2; k++ )
        {
          x[k] = c[0] + ShrinkCoef * ( x[k]-c[0] );
          y[k] = c[1] + ShrinkCoef * ( y[k]-c[1] );
          z[k] = c[2] + ShrinkCoef * ( z[k]-c[2] );
        }

      Array->AddBound( 2 );
      Array->AddVertex( x[0], y[0], z[0] );
      Array->AddVertex( x[1], y[1], z[1] );
    }
  }
}

//================================================================
// Function : HowManyPrimitives
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::HowManyPrimitives (const Handle(MeshVS_HArray1OfSequenceOfInteger)& Topo,
                                               const Standard_Boolean AsPolygons,
                                               const Standard_Boolean IsSelect,
                                               const Standard_Integer NbNodes,
                                               Standard_Integer& Vertices,
                                               Standard_Integer& Bounds)
{
  if( !Topo.IsNull() )
    if( AsPolygons || IsSelect )
    {
      Standard_Integer B = Topo->Upper()-Topo->Lower()+1;
      Bounds += B;
      for( Standard_Integer i=Topo->Lower(), n=Topo->Upper(); i<=n; i++ )
        Vertices += Topo->Value( i ).Length();      

      if( IsSelect )
        Vertices+=B;
    }
    else
    {
      Standard_Integer F = Topo->Upper()-Topo->Lower()+1,
                       E = NbNodes + F - 2;
      // number of edges is obtained by Euler's expression for polyhedrons
      
      Bounds += E;
      Vertices += 2*E;
    }
}

//================================================================
// Function : DrawArrays
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::DrawArrays( const Handle(Prs3d_Presentation)& Prs,
                                        const Handle(Graphic3d_ArrayOfPolygons)& thePolygons,
                                        const Handle(Graphic3d_ArrayOfPolylines)& theLines,
                                        const Handle(Graphic3d_ArrayOfPolylines)& theLinkLines,
                                        const Handle(Graphic3d_ArrayOfPolygons)& theVolumesInShad,
                                        const Standard_Boolean IsPolygonsEdgesOff,
                                        const Standard_Boolean IsSelected,
                                        const Handle(Graphic3d_AspectFillArea3d)& theFillAsp,
                                        const Handle(Graphic3d_AspectLine3d)& theLineAsp ) const
{
  if ( theFillAsp.IsNull() )
    return;

  Standard_Boolean IsFacePolygons   = ( !thePolygons.IsNull() && thePolygons->ItemNumber() > 0 ),
                   IsVolumePolygons = ( !theVolumesInShad.IsNull() && theVolumesInShad->ItemNumber() > 0 ),
                   IsPolygons       = IsFacePolygons || IsVolumePolygons,
                   IsPolylines      = ( !theLines.IsNull() && theLines->ItemNumber() > 0 ),
                   IsLinkPolylines  = ( !theLinkLines.IsNull() && theLinkLines->ItemNumber() > 0 );

  Aspect_InteriorStyle aStyle;
  Quantity_Color anIntColor, aBackColor, anEdgeColor; 
  Aspect_TypeOfLine aType;
  Standard_Real aWidth;

  theFillAsp->Values( aStyle, anIntColor, aBackColor, anEdgeColor, aType, aWidth );

  if ( IsPolygons && theFillAsp->FrontMaterial().Transparency()<0.01 )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup ( Prs );

    //if ( IsPolygonsEdgesOff )
      theFillAsp->SetEdgeOff ();
    //else
    //  theFillAsp->SetEdgeOn ();

    if( anIntColor!=aBackColor )
      theFillAsp->SetDistinguishOn();
    else
      theFillAsp->SetDistinguishOff();

    if( IsFacePolygons )
    {
      aGroup->SetPrimitivesAspect ( theFillAsp );
      aGroup->BeginPrimitives ();
      aGroup->AddPrimitiveArray ( thePolygons );
      aGroup->EndPrimitives ();
    }

    if( IsVolumePolygons )
    {
      Handle( Graphic3d_AspectFillArea3d ) aCullFillAsp = 
          new Graphic3d_AspectFillArea3d( *( theFillAsp.operator->() ) );

      Standard_Boolean isSupressBackFaces = Standard_False;
      Handle( MeshVS_Drawer ) aDrawer = GetDrawer();
      if (!aDrawer.IsNull())
        aDrawer->GetBoolean  ( MeshVS_DA_SupressBackFaces, isSupressBackFaces );

      if (isSupressBackFaces)
        aCullFillAsp->SuppressBackFace();

      aGroup->SetPrimitivesAspect ( aCullFillAsp );
      aGroup->BeginPrimitives ();
      aGroup->AddPrimitiveArray ( theVolumesInShad );
      aGroup->EndPrimitives ();
    }
  }

  if ( IsPolylines && !IsPolygonsEdgesOff )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aLGroup = Prs3d_Root::CurrentGroup ( Prs );

    theFillAsp->SetEdgeOff();
    if ( IsSelected )
      aLGroup->SetPrimitivesAspect ( theLineAsp );
    else
    {
      aLGroup->SetPrimitivesAspect ( theFillAsp );
      aLGroup->SetPrimitivesAspect ( new Graphic3d_AspectLine3d
        ( anEdgeColor, Aspect_TOL_SOLID, aWidth ) );
    }
    aLGroup->BeginPrimitives ();
    aLGroup->AddPrimitiveArray ( theLines );
    aLGroup->EndPrimitives ();
    theFillAsp->SetEdgeOn();
  }

  if ( IsLinkPolylines )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aBeamGroup = Prs3d_Root::CurrentGroup ( Prs );

    theFillAsp->SetEdgeOff();
    if ( !IsSelected )
      aBeamGroup->SetPrimitivesAspect ( theFillAsp );
    aBeamGroup->SetPrimitivesAspect ( theLineAsp );

    aBeamGroup->BeginPrimitives();
    aBeamGroup->AddPrimitiveArray ( theLinkLines );
    aBeamGroup->EndPrimitives();
    theFillAsp->SetEdgeOn();
  }

  if ( IsPolygons && theFillAsp->FrontMaterial().Transparency()>=0.01 )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup ( Prs );

    //if ( IsPolygonsEdgesOff )
      theFillAsp->SetEdgeOff ();
    //else
    //  theFillAsp->SetEdgeOn ();

    if( anIntColor!=aBackColor )
      theFillAsp->SetDistinguishOn();
    else
      theFillAsp->SetDistinguishOff();

    if( IsFacePolygons )
    {
      aGroup->SetPrimitivesAspect ( theFillAsp );
      aGroup->BeginPrimitives ();
      aGroup->AddPrimitiveArray ( thePolygons );
      aGroup->EndPrimitives ();
    }

    if( IsVolumePolygons )
    {
      Handle( Graphic3d_AspectFillArea3d ) aCullFillAsp = 
          new Graphic3d_AspectFillArea3d( *( theFillAsp.operator->() ) );

      Standard_Boolean isSupressBackFaces = Standard_False;
      Handle( MeshVS_Drawer ) aDrawer = GetDrawer();
      if (!aDrawer.IsNull())
        aDrawer->GetBoolean  ( MeshVS_DA_SupressBackFaces, isSupressBackFaces );

      if (isSupressBackFaces)
        aCullFillAsp->SuppressBackFace();

      aGroup->SetPrimitivesAspect ( aCullFillAsp );
      aGroup->BeginPrimitives ();
      aGroup->AddPrimitiveArray ( theVolumesInShad );
      aGroup->EndPrimitives ();
    }
  }
}

//================================================================
// Function : CalculateCenter
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::CalculateCenter (const TColStd_Array1OfReal& theCoords,
                                             const Standard_Integer NbNodes,
                                             Standard_Real &xG,
                                             Standard_Real &yG,
                                             Standard_Real &zG)
{
  xG = yG = zG = 0;
  if ( NbNodes < 4 )
  {
    for ( Standard_Integer k=1; k<=NbNodes; k++)
    {
      xG += theCoords(3*k-2);
      yG += theCoords(3*k-1);
      zG += theCoords(3*k);
    }
    xG /= Standard_Real(NbNodes);
    yG /= Standard_Real(NbNodes);
    zG /= Standard_Real(NbNodes);
  }
  else
  {
    Standard_Integer a = 1, b = 3;
    xG = ( theCoords( 3*a-2 ) + theCoords( 3*b-2 ) ) / 2.0;
    yG = ( theCoords( 3*a-1 ) + theCoords( 3*b-1 ) ) / 2.0;
    zG = ( theCoords( 3*a )   + theCoords( 3*b   ) ) / 2.0;
  }
}
