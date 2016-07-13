// Created on: 2003-09-16
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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


#include <Aspect_InteriorStyle.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_MapOfTwoNodes.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_SymmetricPairHasher.hxx>
#include <MeshVS_Tool.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>

#ifdef _WIN32
  #include <malloc.h> // for alloca()
#endif

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_MeshPrsBuilder,MeshVS_PrsBuilder)

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
    // subtract the hidden nodes and ids to exclude (to minimize allocated memory)
    Handle(TColStd_HPackedMapOfInteger) aHiddenNodes = myParentMesh->GetHiddenNodes();
    if ( !aHiddenNodes.IsNull() )
      anIDs.Subtract( aHiddenNodes->Map() );
  }
  anIDs.Subtract( IDsToExclude );

  Standard_Integer upper = anIDs.Extent();
  if ( upper<=0 )
    return;

  Handle(Graphic3d_ArrayOfPoints) aNodePoints = new Graphic3d_ArrayOfPoints (upper);
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
      aNodePoints->AddVertex (aCoords(1), aCoords(2), aCoords(3));
    }
  }

  if ( k>0 )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aNodeGroup = Prs3d_Root::CurrentGroup ( Prs );
    aNodeGroup->SetPrimitivesAspect ( aNodeMark );
    aNodeGroup->AddPrimitiveArray (aNodePoints);
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

  Handle(MeshVS_DataSource) aSource = GetDataSource();
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

  // display mode for highlighted prs of groups
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

  // subtract the hidden elements and ids to exclude (to minimize allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
  if ( !aHiddenElems.IsNull() )
    anIDs.Subtract( aHiddenElems->Map() );
  anIDs.Subtract( IDsToExclude );

  Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;
  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);

  Standard_Boolean showEdges = Standard_True;
  aDrawer->GetBoolean( MeshVS_DA_ShowEdges, showEdges );

  showEdges = IsWireFrame || showEdges;

  Standard_Integer* aNodesBuf  = (Standard_Integer*) alloca (maxnodes * sizeof (Standard_Integer));
  Standard_Real*    aCoordsBuf = (Standard_Real*)    alloca (3 * maxnodes * sizeof (Standard_Real));

  TColStd_Array1OfInteger aNodes  (*aNodesBuf, 1, maxnodes);
  TColStd_Array1OfReal    aCoords (*aCoordsBuf, 1, 3 * maxnodes);

  Standard_Integer aNbFacePrimitives = 0;
  Standard_Integer aNbVolmPrimitives = 0;
  Standard_Integer aNbEdgePrimitives = 0;
  Standard_Integer aNbLinkPrimitives = 0;

  MeshVS_EntityType aType;

  for (it.Reset(); it.More(); it.Next())
  {
    Standard_Integer aNbNodes = 0;

    if (!aSource->GetGeom (it.Key(), Standard_True, aCoords, aNbNodes, aType))
      continue;

    if (aType == MeshVS_ET_Volume)
    {
      if (aSource->Get3DGeom (it.Key(), aNbNodes, aTopo))
      {
        for (Standard_Integer aFaceIdx = aTopo->Lower(); aFaceIdx <= aTopo->Upper(); ++aFaceIdx)
        {
          const TColStd_SequenceOfInteger& aFaceNodes = aTopo->Value (aFaceIdx);
          
          if (showEdges) // add edge segments
          {
            aNbEdgePrimitives += aFaceNodes.Length();
          }
          
          if (IsShading || IsShrink) // add volumetric cell triangles
          {
            if (!HasSelectFlag)
              aNbVolmPrimitives += aFaceNodes.Length() - 2;
          }
        }
      }
    }
    else if (aType == MeshVS_ET_Link)
    {
      if (showEdges)
      {
        aNbLinkPrimitives += 1; // add link segment
      }
    }
    else if (aType == MeshVS_ET_Face)
    {
      if (showEdges)
      {
        aNbEdgePrimitives += aNbNodes; // add edge segments
      }

      if (!IsOverlapControl || IsShading)
      {
        if ((IsShading || IsShrink) && !HasSelectFlag)
        {
          aNbFacePrimitives += aNbNodes - 2; // add face triangles
        }
      }
    }
  }

  // Here we do not use indices arrays because they are not effective for some mesh
  // drawing modes: shrinking mode (displaces the vertices inside the polygon), 3D
  // cell rendering (normal interpolation is not always applicable - flat shading),
  // elemental coloring (color interpolation is impossible)
  Handle (Graphic3d_ArrayOfTriangles) aVolmTriangles =
    new Graphic3d_ArrayOfTriangles (aNbVolmPrimitives * 3, 0, IsReflect);
  Handle (Graphic3d_ArrayOfTriangles) aFaceTriangles =
    new Graphic3d_ArrayOfTriangles (aNbFacePrimitives * 3, 0, IsReflect);

  Handle (Graphic3d_ArrayOfSegments) aLinkSegments;
  Handle (Graphic3d_ArrayOfSegments) aEdgeSegments;

  if (showEdges)
  {
    aLinkSegments = new Graphic3d_ArrayOfSegments (aNbLinkPrimitives * 2);
    aEdgeSegments = new Graphic3d_ArrayOfSegments (aNbEdgePrimitives * 2);
  }

  TColStd_PackedMapOfInteger aCustomElements;

  Quantity_Color anOldEdgeColor;
  Quantity_Color anEdgeColor = aFill->EdgeColor();
  MeshVS_MapOfTwoNodes aLinkNodes;
  
  // Forbid drawings of edges which overlap with some links
  if (showEdges && IsOverlapControl)
  {
    for (it.Reset(); it.More(); it.Next())
    {
      if (aSource->GetGeomType (it.Key(), Standard_True, aType) && aType == MeshVS_ET_Link)
      {
        Standard_Integer aNbNodes;

        if (aSource->GetNodesByElement (it.Key(), aNodes, aNbNodes) && aNbNodes == 2)
        {
          aLinkNodes.Add (MeshVS_TwoNodes (aNodes(1), aNodes(2)));
        }
      }
    }
  }

  NCollection_Map<MeshVS_NodePair, MeshVS_SymmetricPairHasher> aSegmentMap;

  for (it.Reset(); it.More(); it.Next())
  {
    const Standard_Integer aKey = it.Key();

    Standard_Integer NbNodes;
    if (!aSource->GetGeom (aKey, Standard_True, aCoords, NbNodes, aType))
      continue;

    if (!aSource->GetNodesByElement (aKey, aNodes, NbNodes))
      continue;

    switch (aType)
    {
      case MeshVS_ET_Volume:
      {
        if (IsExcludingOn())
          IDsToExclude.Add (aKey);

        if (aSource->Get3DGeom (aKey, NbNodes, aTopo))
        {
          // Add wire-frame presentation (draw edges for shading mode as well)
          if (showEdges)
          {
            AddVolumePrs (aTopo, aCoords, NbNodes,
              aEdgeSegments, IsReflect, IsShrink, HasSelectFlag, aShrinkCoef);
          }

          // Add shading presentation
          if ((IsShading || IsShrink) && !HasSelectFlag)
          {
            AddVolumePrs (aTopo, aCoords, NbNodes,
              aVolmTriangles, IsReflect, IsShrink, HasSelectFlag, aShrinkCoef);
          }
        }
      }
      break;

      case MeshVS_ET_Link:
      {
        if (IsExcludingOn())
          IDsToExclude.Add (aKey);

        if (showEdges)
        {
          AddLinkPrs (aCoords, aLinkSegments, IsShrink || HasSelectFlag, aShrinkCoef);
        }
      }
      break;

      case MeshVS_ET_Face:
      {
        if (IsExcludingOn())
          IDsToExclude.Add (aKey);

        if (showEdges && IsOverlapControl)
        {
          Standard_Integer Last = 0;

          MeshVS_TwoNodes aTwoNodes (aNodes (1));

          for (Standard_Integer i = 1; i <= NbNodes; ++i)
          {
            if (i > 1)
              aTwoNodes.First = aTwoNodes.Second;

            aTwoNodes.Second = (i < NbNodes) ? aNodes (i+1) : aNodes (1);

            if (aLinkNodes.Contains (aTwoNodes))
            {
              for (Standard_Integer aNodeIdx = Last + 1; aNodeIdx < i; ++aNodeIdx)
              {
                const Standard_Integer aNextIdx = aNodeIdx + 1;

                aEdgeSegments->AddVertex (
                  aCoords (3 * aNodeIdx - 2), aCoords (3 * aNodeIdx - 1), aCoords (3 * aNodeIdx));
                aEdgeSegments->AddVertex (
                  aCoords (3 * aNextIdx - 2), aCoords (3 * aNextIdx - 1), aCoords (3 * aNextIdx));
              }

              Last = i;
            }
          }

          if (NbNodes - Last > 0)
          {
            for (Standard_Integer aNodeIdx = Last; aNodeIdx < NbNodes; ++aNodeIdx)
            {
              const Standard_Integer aNextIdx = (aNodeIdx + 1) % NbNodes;
              
              const MeshVS_NodePair aSegment (aNodes (aNodeIdx + 1),
                                              aNodes (aNextIdx + 1));

              if (!aSegmentMap.Contains (aSegment))
              {
                aEdgeSegments->AddVertex (aCoords (3 * aNodeIdx + 1),
                                          aCoords (3 * aNodeIdx + 2),
                                          aCoords (3 * aNodeIdx + 3));

                aEdgeSegments->AddVertex (aCoords (3 * aNextIdx + 1),
                                          aCoords (3 * aNextIdx + 2),
                                          aCoords (3 * aNextIdx + 3));

                aSegmentMap.Add (aSegment);
              }
            }
          }
        }

        if (!IsOverlapControl || IsShading)
        {
          if (!IsOverlapControl && showEdges)
          {
            AddFaceWirePrs (aCoords, NbNodes,
              aEdgeSegments, IsShrink || HasSelectFlag, aShrinkCoef);
          }

          if ((IsShading || IsShrink) && !HasSelectFlag)
          {
            AddFaceSolidPrs (aKey, aCoords, NbNodes, maxnodes, aFaceTriangles, IsReflect,
              IsShrink || HasSelectFlag, aShrinkCoef, IsMeshSmoothShading);
          }
        }
      }
      break;

      default:
      {
        aCustomElements.Add (aKey);
      }
    }
  }

  if (IsShrink)
  {
    anOldEdgeColor = anEdgeColor;
    aFill->SetEdgeColor (Quantity_NOC_BLACK);
  }

  //std::cout << "Actual extents: " << std::endl
  //  << "Face tris: " << aFaceTriangles->ItemNumber() << " from " << aNbFacePrimitives << std::endl
  //  << "Volm tris: " << aVolmTriangles->ItemNumber()  << " from " << aNbVolmPrimitives << std::endl
  //  << "Face segs: " << aEdgeSegments->ItemNumber()  << " from " << aNbEdgePrimitives << std::endl
  //  << "Link segs: " << aLinkSegments->ItemNumber()  << " from " << aNbLinkPrimitives << std::endl;

  DrawArrays ( Prs, aFaceTriangles, aEdgeSegments, aLinkSegments, aVolmTriangles,
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
      aHilightGroup->SetPrimitivesAspect (aNodeMark);
      Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
      anArrayOfPoints->AddVertex (aCoords(1), aCoords(2), aCoords(3));
      aHilightGroup->AddPrimitiveArray (anArrayOfPoints);
    }
    break;

    case MeshVS_ET_Link:
    {
      aHilightGroup->SetPrimitivesAspect ( aBeam );
      Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
      aPrims->AddVertex(aCoords(1),aCoords(2),aCoords(3));
      aPrims->AddVertex(aCoords(4),aCoords(5),aCoords(6));
      aHilightGroup->AddPrimitiveArray(aPrims);
    }
    break;

    case MeshVS_ET_Face:
    if ( NbNodes > 0 )
    {
      aHilightGroup->SetPrimitivesAspect ( aFill );
      Handle(Graphic3d_ArrayOfPolygons) aPrims = new Graphic3d_ArrayOfPolygons(NbNodes);
      for ( Standard_Integer k=1; k<=NbNodes; k++)
        aPrims->AddVertex(aCoords(3*k-2),aCoords(3*k-1),aCoords(3*k));
      aHilightGroup->AddPrimitiveArray(aPrims);
    }
    break;

    case MeshVS_ET_Volume:
    if( NbNodes > 0 )
    {
      Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;

      aHilightGroup->SetPrimitivesAspect ( aFill );

      if( aSource->Get3DGeom( ID, NbNodes, aTopo ) )
      {
        const Standard_Integer up = aTopo->Upper();
        const Standard_Integer lo = aTopo->Lower();
        Standard_Integer nbnodes = 0, i, j;
        for( i=lo; i<=up; i++ )
          nbnodes += aTopo->Value( i ).Length();

        Handle(Graphic3d_ArrayOfPolygons) aPrims = new Graphic3d_ArrayOfPolygons(nbnodes,aTopo->Length());
        for( i=lo; i<=up; i++ )
        {
          const TColStd_SequenceOfInteger& aSeq = aTopo->Value( i );
          const Standard_Integer m = aSeq.Length();
          aPrims->AddBound(m);
          for( j=1; j<=m; j++ )
          {
            const Standard_Integer ind = 3*aSeq.Value( j );
            aPrims->AddVertex(aCoords(ind+1),aCoords(ind+2),aCoords(ind+3));
          }
        }
        aHilightGroup->AddPrimitiveArray(aPrims);
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
void MeshVS_MeshPrsBuilder::AddLinkPrs (const TColStd_Array1OfReal&              theCoords,
                                        const Handle(Graphic3d_ArrayOfSegments)& theSegments,
                                        const Standard_Boolean                   theIsShrinked,
                                        const Standard_Real                      theShrinkCoef) const
{
  Standard_Real aX1 = theCoords (1);
  Standard_Real aY1 = theCoords (2);
  Standard_Real aZ1 = theCoords (3);
  Standard_Real aX2 = theCoords (4);
  Standard_Real aY2 = theCoords (5);
  Standard_Real aZ2 = theCoords (6);

  if (theIsShrinked)
  {
    const Standard_Real xG = (aX1 + aX2) * 0.5;
    const Standard_Real yG = (aY1 + aY2) * 0.5;
    const Standard_Real zG = (aZ1 + aZ2) * 0.5;

    aX1 = (aX1 - xG) * theShrinkCoef + xG;
    aY1 = (aY1 - yG) * theShrinkCoef + yG;
    aZ1 = (aZ1 - zG) * theShrinkCoef + zG;

    aX2 = 2.0 * xG - aX1;
    aY2 = 2.0 * yG - aY1;
    aZ2 = 2.0 * zG - aZ1;
  }

  theSegments->AddVertex (aX1, aY1, aZ1);
  theSegments->AddVertex (aX2, aY2, aZ2);
}

//================================================================
// Function : AddFaceWirePrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::AddFaceWirePrs (const TColStd_Array1OfReal&              theCoords,
                                            const Standard_Integer                   theNbNodes,
                                            const Handle(Graphic3d_ArrayOfSegments)& theSegments,
                                            const Standard_Boolean                   theIsShrinked,
                                            const Standard_Real                      theShrinkingCoef) const
{
  Standard_Real aCenterX = 0.0;
  Standard_Real aCenterY = 0.0;
  Standard_Real aCenterZ = 0.0;

  if (theIsShrinked)
  {
    CalculateCenter (theCoords, theNbNodes, aCenterX, aCenterY, aCenterZ);
  }

  NCollection_Vector<gp_XYZ> aNodes (theNbNodes);

  for (Standard_Integer aNodeIdx = 0; aNodeIdx < theNbNodes; ++aNodeIdx)
  {
    gp_XYZ aPnt (theCoords (3 * aNodeIdx + 1),
                 theCoords (3 * aNodeIdx + 2),
                 theCoords (3 * aNodeIdx + 3));

    if (theIsShrinked)
    {
      aPnt.SetX ((aPnt.X() - aCenterX) * theShrinkingCoef + aCenterX);
      aPnt.SetY ((aPnt.Y() - aCenterY) * theShrinkingCoef + aCenterY);
      aPnt.SetZ ((aPnt.Z() - aCenterZ) * theShrinkingCoef + aCenterZ);
    }

    aNodes.Append (aPnt);
  }

  for (Standard_Integer aNodeIdx = 0; aNodeIdx < theNbNodes; ++aNodeIdx)
  {
    theSegments->AddVertex (aNodes.Value (aNodeIdx).X(),
                            aNodes.Value (aNodeIdx).Y(),
                            aNodes.Value (aNodeIdx).Z());

    const Standard_Integer aNextIdx = (aNodeIdx + 1) % theNbNodes;

    theSegments->AddVertex (aNodes.Value (aNextIdx).X(),
                            aNodes.Value (aNextIdx).Y(),
                            aNodes.Value (aNextIdx).Z());
  }
}

//================================================================
// Function : AddFaceSolidPrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::AddFaceSolidPrs (const Standard_Integer                    theID,
                                             const TColStd_Array1OfReal&               theCoords,
                                             const Standard_Integer                    theNbNodes,
                                             const Standard_Integer                    theMaxNodes,
                                             const Handle(Graphic3d_ArrayOfTriangles)& theTriangles,
                                             const Standard_Boolean                    theIsShaded,
                                             const Standard_Boolean                    theIsShrinked,
                                             const Standard_Real                       theShrinkingCoef,
                                             const Standard_Boolean                    theIsSmoothShading) const
{
  Handle(MeshVS_DataSource) aDataSource = myParentMesh->GetDataSource();

  if (aDataSource.IsNull())
    return;

  Standard_Real aCenterX = 0.0;
  Standard_Real aCenterY = 0.0;
  Standard_Real aCenterZ = 0.0;
  Standard_Real aNormalX = 0.0;
  Standard_Real aNormalY = 0.0;
  Standard_Real aNormalZ = 0.0;

  if (theIsShrinked)
  {
    CalculateCenter (theCoords, theNbNodes, aCenterX, aCenterY, aCenterZ);
  }

  NCollection_Vector<gp_XYZ> aVertexNormals (theMaxNodes);
  
  if (theIsShaded)
  {
    if (theIsSmoothShading)
    {
      for (Standard_Integer aNodeIdx = 1; aNodeIdx <= theNbNodes; ++aNodeIdx)
      {
        if (!aDataSource->GetNodeNormal (aNodeIdx, theID, aNormalX, aNormalY, aNormalZ))
          break;

        aVertexNormals.Append (gp_XYZ (aNormalX, aNormalY, aNormalZ));
      }
    }

    if (!theIsSmoothShading || aVertexNormals.Size() != theNbNodes)
    {
      aDataSource->GetNormal (theID, theMaxNodes, aNormalX, aNormalY, aNormalZ);
    }
  }

  NCollection_Vector<gp_XYZ> aNodes (theMaxNodes);

  for (Standard_Integer aNodeIdx = 0; aNodeIdx < theNbNodes; ++aNodeIdx)
  {
    gp_XYZ aPnt (theCoords (3 * aNodeIdx + 1),
                 theCoords (3 * aNodeIdx + 2),
                 theCoords (3 * aNodeIdx + 3));

    if (theIsShrinked)
    {
      aPnt.SetX ((aPnt.X() - aCenterX) * theShrinkingCoef + aCenterX);
      aPnt.SetY ((aPnt.Y() - aCenterY) * theShrinkingCoef + aCenterY);
      aPnt.SetZ ((aPnt.Z() - aCenterZ) * theShrinkingCoef + aCenterZ);
    }

    aNodes.Append (aPnt);
  }

  // Triangulate polygon
  for (Standard_Integer aNodeIdx = 0; aNodeIdx < theNbNodes - 2; ++aNodeIdx)
  {
    for (Standard_Integer aSubIdx = 0; aSubIdx < 3; ++aSubIdx)
    {
      if (theIsShaded)
      {
        if (theIsSmoothShading && aVertexNormals.Size() == theNbNodes)
        {
          aNormalX = aVertexNormals.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).X();
          aNormalY = aVertexNormals.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).Y();
          aNormalZ = aVertexNormals.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).Z();
        }

        theTriangles->AddVertex (aNodes.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).X(),
                                 aNodes.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).Y(),
                                 aNodes.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).Z(),
                                 aNormalX,
                                 aNormalY,
                                 aNormalZ);
      }
      else
      {
        theTriangles->AddVertex (aNodes.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).X(),
                                 aNodes.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).Y(),
                                 aNodes.Value (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)).Z());
      }
    }
  }
}

//================================================================
// Function : AddVolumePrs
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::AddVolumePrs (const Handle(MeshVS_HArray1OfSequenceOfInteger)& theTopo,
                                          const TColStd_Array1OfReal&                      theNodes,
                                          const Standard_Integer                           theNbNodes,
                                          const Handle(Graphic3d_ArrayOfPrimitives)&       theArray,
                                          const Standard_Boolean                           theIsShaded,
                                          const Standard_Boolean                           theIsShrinked,
                                          const Standard_Boolean                           theIsSelected,
                                          const Standard_Real                              theShrinkCoef)
{
  Standard_Real aCenter[] = { 0.0, 0.0, 0.0 };

  Standard_Integer aLow = theNodes.Lower();

  if (theTopo.IsNull() || theArray.IsNull())
    return;

  if (theIsShrinked)
  {
    for (Standard_Integer aNodeIdx = 0; aNodeIdx < 3 * theNbNodes; ++aNodeIdx)
    {
      aCenter[aNodeIdx % 3] += theNodes.Value (aLow + aNodeIdx);
    }

    aCenter[0] /= theNbNodes;
    aCenter[1] /= theNbNodes;
    aCenter[2] /= theNbNodes;
  }

  Standard_Boolean aIsPolygons = theArray->IsKind (STANDARD_TYPE (Graphic3d_ArrayOfTriangles));

  if (aIsPolygons)
  {
    for (Standard_Integer aFaceIdx = theTopo->Lower(), topoup = theTopo->Upper(); aFaceIdx <= topoup; ++aFaceIdx)
    {
      const TColStd_SequenceOfInteger& aFaceNodes = theTopo->Value (aFaceIdx);
      const Standard_Integer aNbPolyNodes = aFaceNodes.Length();
      
      Standard_Real* aPolyNodesBuf = (Standard_Real*) alloca ((3 * aNbPolyNodes + 1) * sizeof (Standard_Real));
      TColStd_Array1OfReal aPolyNodes (*aPolyNodesBuf, 0, 3 * aNbPolyNodes);

      for (Standard_Integer aNodeIdx = 0; aNodeIdx < aNbPolyNodes; ++aNodeIdx)
      {
        Standard_Integer anIdx = aFaceNodes.Value (aNodeIdx + 1);

        Standard_Real aX = theNodes.Value (aLow + 3 * anIdx + 0);
        Standard_Real aY = theNodes.Value (aLow + 3 * anIdx + 1);
        Standard_Real aZ = theNodes.Value (aLow + 3 * anIdx + 2);

        if (theIsShrinked)
        {
          aX = aCenter[0] + theShrinkCoef * (aX - aCenter[0]);
          aY = aCenter[1] + theShrinkCoef * (aY - aCenter[1]);
          aZ = aCenter[2] + theShrinkCoef * (aZ - aCenter[2]);
        }

        aPolyNodes.SetValue (3 * aNodeIdx + 1, aX);
        aPolyNodes.SetValue (3 * aNodeIdx + 2, aY);
        aPolyNodes.SetValue (3 * aNodeIdx + 3, aZ);
      }
      
      gp_Vec aNorm;

      if (theIsShaded)
      {
        aPolyNodes.SetValue (0, aNbPolyNodes);
        
        if (!MeshVS_Tool::GetAverageNormal (aPolyNodes, aNorm))
        {
          aNorm.SetCoord (0.0, 0.0, 1.0);
        }
      }

      for (Standard_Integer aNodeIdx = 0; aNodeIdx < aNbPolyNodes - 2; ++aNodeIdx) // triangulate polygon
      {
        for (Standard_Integer aSubIdx = 0; aSubIdx < 3; ++aSubIdx) // generate sub-triangle
        {
          if (theIsShaded)
          {
            theArray->AddVertex (aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 1),
                                 aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 2),
                                 aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 3),
                                 aNorm.X(),
                                 aNorm.Y(),
                                 aNorm.Z());
          }
          else
          {
            theArray->AddVertex (aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 1),
                                 aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 2),
                                 aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 3));
          }
        }
      }
    }
  }
  else if (theIsSelected)
  {
    for (Standard_Integer aFaceIdx = theTopo->Lower(), topoup = theTopo->Upper(); aFaceIdx <= topoup; ++aFaceIdx)
    {
      const TColStd_SequenceOfInteger& aFaceNodes = theTopo->Value (aFaceIdx);

      Standard_Real aFaceCenter[] = { 0.0, 0.0, 0.0 };

      for (Standard_Integer aNodeIdx = 1; aNodeIdx <= aFaceNodes.Length(); ++aNodeIdx)
      {
        for (Standard_Integer aAxisIdx = 0; aAxisIdx < 3; ++aAxisIdx)
        {
          aFaceCenter[aAxisIdx] += theNodes.Value (aLow + 3 * aFaceNodes.Value (aNodeIdx) + aAxisIdx);
        }
      }

      aFaceCenter[0] /= aFaceNodes.Length();
      aFaceCenter[1] /= aFaceNodes.Length();
      aFaceCenter[2] /= aFaceNodes.Length();

      for (Standard_Integer aNodeIdx = 0, aNbNodes = aFaceNodes.Length(); aNodeIdx < aNbNodes; ++aNodeIdx)
      {
        for (Standard_Integer aSubIdx = 0; aSubIdx < 2; ++aSubIdx) // add segment
        {
          Standard_Integer anIdx = aFaceNodes.Value ((aNodeIdx + aSubIdx) % aNbNodes + 1);

          Standard_Real aX = theNodes.Value (aLow + 3 * anIdx + 0);
          Standard_Real aY = theNodes.Value (aLow + 3 * anIdx + 1);
          Standard_Real aZ = theNodes.Value (aLow + 3 * anIdx + 2);

          theArray->AddVertex (aFaceCenter[0] + theShrinkCoef * (aX - aFaceCenter[0]),
                               aFaceCenter[1] + theShrinkCoef * (aY - aFaceCenter[1]),
                               aFaceCenter[2] + theShrinkCoef * (aZ - aFaceCenter[2]));
        }
      }
    }
  }
  else
  {
    // Find all pairs of nodes (edges) to draw (will be drawn only once)
    NCollection_Map<MeshVS_NodePair, MeshVS_SymmetricPairHasher> aEdgeMap;

    for (Standard_Integer aFaceIdx = theTopo->Lower(), topoup = theTopo->Upper(); aFaceIdx <= topoup; ++aFaceIdx)
    {
      const TColStd_SequenceOfInteger& aFaceNodes = theTopo->Value (aFaceIdx);
      
      for (Standard_Integer aNodeIdx = 0, aNbNodes = aFaceNodes.Length(); aNodeIdx < aNbNodes; ++aNodeIdx)
      {
        const Standard_Integer aNextIdx = (aNodeIdx + 1) % aNbNodes;

        aEdgeMap.Add (MeshVS_NodePair (aFaceNodes.Value (aNodeIdx + 1),
                                       aFaceNodes.Value (aNextIdx + 1)));
      }
    }

    // Draw edges
    for(NCollection_Map<MeshVS_NodePair, MeshVS_SymmetricPairHasher>::Iterator anIt (aEdgeMap); anIt.More(); anIt.Next())
    {      
      const Standard_Integer anIdx1 = aLow + 3 * anIt.Key().first;
      const Standard_Integer anIdx2 = aLow + 3 * anIt.Key().second;

      Standard_Real aX[] = { theNodes.Value (anIdx1 + 0), theNodes.Value (anIdx2 + 0) };
      Standard_Real aY[] = { theNodes.Value (anIdx1 + 1), theNodes.Value (anIdx2 + 1) };
      Standard_Real aZ[] = { theNodes.Value (anIdx1 + 2), theNodes.Value (anIdx2 + 2) };

      if (theIsShrinked)
      {
        for (Standard_Integer anAxisIdx = 0; anAxisIdx < 2; ++anAxisIdx)
        {
          aX[anAxisIdx] = aCenter[0] + theShrinkCoef * (aX[anAxisIdx] - aCenter[0]);
          aY[anAxisIdx] = aCenter[1] + theShrinkCoef * (aY[anAxisIdx] - aCenter[1]);
          aZ[anAxisIdx] = aCenter[2] + theShrinkCoef * (aZ[anAxisIdx] - aCenter[2]);
        }
      }

      theArray->AddVertex (aX[0], aY[0], aZ[0]);
      theArray->AddVertex (aX[1], aY[1], aZ[1]);
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
  if( !Topo.IsNull() ) {
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
}

//================================================================
// Function : DrawArrays
// Purpose  :
//================================================================
void MeshVS_MeshPrsBuilder::DrawArrays( const Handle(Prs3d_Presentation)& Prs,
                                        const Handle(Graphic3d_ArrayOfPrimitives)& thePolygons,
                                        const Handle(Graphic3d_ArrayOfPrimitives)& theLines,
                                        const Handle(Graphic3d_ArrayOfPrimitives)& theLinkLines,
                                        const Handle(Graphic3d_ArrayOfPrimitives)& theVolumesInShad,
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

  Quantity_Color anIntColor  = theFillAsp->InteriorColor();
  Quantity_Color aBackColor  = theFillAsp->BackInteriorColor();
  Quantity_Color anEdgeColor = theFillAsp->EdgeColor();
  Standard_Real  aWidth      = theFillAsp->EdgeWidth();

  Standard_Boolean isSupressBackFaces = Standard_False;
  Handle(MeshVS_Drawer) aDrawer = GetDrawer();
  if (!aDrawer.IsNull())
  {
    aDrawer->GetBoolean (MeshVS_DA_SupressBackFaces, isSupressBackFaces);
  }

  if ( IsPolygons && theFillAsp->FrontMaterial().Transparency()<0.01 )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup ( Prs );
    aGroup->SetClosed (isSupressBackFaces == Standard_True);
    Handle(Graphic3d_AspectFillArea3d) aFillAsp = new Graphic3d_AspectFillArea3d (*theFillAsp);
    //if ( IsPolygonsEdgesOff )
      aFillAsp->SetEdgeOff ();
    //else
    //  aFillAsp->SetEdgeOn ();

    if( anIntColor!=aBackColor )
      aFillAsp->SetDistinguishOn();
    else
      aFillAsp->SetDistinguishOff();

    aGroup->SetPrimitivesAspect (aFillAsp);

    if( IsFacePolygons )
    {
      aGroup->AddPrimitiveArray ( thePolygons );
    }

    if( IsVolumePolygons )
    {
      aGroup->AddPrimitiveArray ( theVolumesInShad );
    }
  }

  if ( IsPolylines && !IsPolygonsEdgesOff )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aLGroup = Prs3d_Root::CurrentGroup ( Prs );

    if ( IsSelected )
      aLGroup->SetPrimitivesAspect ( theLineAsp );
    else
    {
      aLGroup->SetPrimitivesAspect ( theFillAsp );
      aLGroup->SetPrimitivesAspect ( new Graphic3d_AspectLine3d
        ( anEdgeColor, Aspect_TOL_SOLID, aWidth ) );
    }
    aLGroup->AddPrimitiveArray ( theLines );
  }

  if ( IsLinkPolylines )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aBeamGroup = Prs3d_Root::CurrentGroup ( Prs );
    if ( !IsSelected )
      aBeamGroup->SetPrimitivesAspect ( theFillAsp );
    aBeamGroup->SetPrimitivesAspect ( theLineAsp );
    aBeamGroup->AddPrimitiveArray ( theLinkLines );
  }

  if ( IsPolygons && theFillAsp->FrontMaterial().Transparency()>=0.01 )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup ( Prs );
    aGroup->SetClosed (isSupressBackFaces == Standard_True);
    Handle(Graphic3d_AspectFillArea3d) aFillAsp = new Graphic3d_AspectFillArea3d (*theFillAsp);
    //if ( IsPolygonsEdgesOff )
      aFillAsp->SetEdgeOff ();
    //else
    //  aFillAsp->SetEdgeOn ();

    if( anIntColor!=aBackColor )
      aFillAsp->SetDistinguishOn();
    else
      aFillAsp->SetDistinguishOff();

    aGroup->SetPrimitivesAspect (aFillAsp);

    if( IsFacePolygons )
    {
      aGroup->AddPrimitiveArray ( thePolygons );
    }

    if( IsVolumePolygons )
    {
      aGroup->AddPrimitiveArray ( theVolumesInShad );
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
