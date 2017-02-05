// Created on: 2003-11-12
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

#define _POLYGONES_

// if define _POLYGONES_ ColorPrsBuilder use ArrayOfPolygons for drawing faces

#include <Aspect_SequenceOfColor.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Image_PixMap.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_NodalColorPrsBuilder.hxx>
#include <MeshVS_SymmetricPairHasher.hxx>
#include <MeshVS_Tool.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Quantity_Array1OfColor.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_NodalColorPrsBuilder,MeshVS_PrsBuilder)

/*
  Class       : MeshVS_ImageTexture2D
  Description : Texture for nodal presentation
*/
class MeshVS_ImageTexture2D : public Graphic3d_Texture2D
{
public:

  MeshVS_ImageTexture2D (const Handle(Image_PixMap)& theImg) : Graphic3d_Texture2D (theImg, Graphic3d_TOT_2D)
  {
    myParams->SetModulate (Standard_True);
    myParams->SetFilter   (Graphic3d_TOTF_BILINEAR);
  }

public:

  DEFINE_STANDARD_RTTI_INLINE(MeshVS_ImageTexture2D,Graphic3d_Texture2D)
};

DEFINE_STANDARD_HANDLE    (MeshVS_ImageTexture2D, Graphic3d_Texture2D)


//================================================================
// Function : getNearestPow2
// Purpose  : Returns the nearest power of two greater than the
//            argument value
//================================================================
static inline Standard_Integer getNearestPow2( Standard_Integer theValue )
{
  // Precaution against overflow
  Standard_Integer aHalfMax = IntegerLast() >> 1, aRes = 1;
  if ( theValue > aHalfMax ) theValue = aHalfMax;
  while ( aRes < theValue ) aRes <<= 1;
  return aRes;
}

/*
  Class       : MeshVS_NodalColorPrsBuilder
  Description : This class provides methods to create presentation of
                nodes with assigned color (See hxx for more description )
*/

//================================================================
// Function : Constructor MeshVS_NodalColorPrsBuilder
// Purpose  :
//================================================================
MeshVS_NodalColorPrsBuilder::MeshVS_NodalColorPrsBuilder ( const Handle(MeshVS_Mesh)& Parent,
                                                           const MeshVS_DisplayModeFlags& Flags,
                                                           const Handle (MeshVS_DataSource)& DS,
                                                           const Standard_Integer Id,
                                                           const MeshVS_BuilderPriority& Priority )
: MeshVS_PrsBuilder ( Parent, Flags, DS, Id, Priority ),
  myUseTexture( Standard_False ),
  myInvalidColor( Quantity_NOC_GRAY )
{
  SetExcluding ( Standard_True );
}

//================================================================
// Function : Build
// Purpose  :
//================================================================
void MeshVS_NodalColorPrsBuilder::Build ( const Handle(Prs3d_Presentation)& Prs,
                                          const TColStd_PackedMapOfInteger& IDs,
                                          TColStd_PackedMapOfInteger& IDsToExclude,
                                          const Standard_Boolean IsElement,
                                          const Standard_Integer DisplayMode) const
{
  Handle (MeshVS_DataSource) aSource = GetDataSource();
  Handle (MeshVS_Drawer)     aDrawer = GetDrawer();
  if ( aSource.IsNull() || aDrawer.IsNull() )
    return;

  Standard_Integer aMaxFaceNodes;
  if ( !aDrawer->GetInteger ( MeshVS_DA_MaxFaceNodes, aMaxFaceNodes ) || aMaxFaceNodes <= 0 )
    return;

  MeshVS_Buffer aCoordsBuf (3*aMaxFaceNodes*sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords ( aCoordsBuf, 1, 3 * aMaxFaceNodes );
  Standard_Integer NbNodes;
  MeshVS_EntityType aType;

  if ( !( DisplayMode & GetFlags() ) || !IsElement )
    return;

  if ( (myUseTexture && ( !myTextureCoords.Extent() || !myTextureColorMap.Length() )) ||
       (!myUseTexture && !myNodeColorMap.Extent()) )
    return;

  // subtract the hidden elements and ids to exclude (to minimize allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
  if ( !aHiddenElems.IsNull() )
    anIDs.Subtract( aHiddenElems->Map() );
  anIDs.Subtract( IDsToExclude );

  Standard_Boolean IsReflect = Standard_False, IsMeshSmoothShading = Standard_False;
  aDrawer->GetBoolean( MeshVS_DA_ColorReflection, IsReflect );
  aDrawer->GetBoolean( MeshVS_DA_SmoothShading,   IsMeshSmoothShading );

  // Following parameter are used for texture presentation only
  int nbColors = 0; // Number of colors from color map
  int nbTextureColors = 0; // Number of colors in texture (it will be pow of 2)
  if ( myUseTexture )
  {
    nbColors = myTextureColorMap.Length();
    nbTextureColors = getNearestPow2( nbColors );
  }

  Standard_Integer aSize = anIDs.Extent();

  // Calculate maximum possible number of vertices and bounds
  Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;
  Standard_Integer PolygonVerticesFor3D = 0, PolygonBoundsFor3D = 0;
  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);
  for( ; it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();
    if ( aSource->Get3DGeom( aKey, NbNodes, aTopo ) )
      MeshVS_MeshPrsBuilder::HowManyPrimitives
      ( aTopo, Standard_True, Standard_False, NbNodes,
      PolygonVerticesFor3D, PolygonBoundsFor3D );
  }

  // Draw faces with nodal color
  // OCC20644 Use "plastic" material as it is "non-physic" and so it is easier
  // to get the required colors (see TelUpdateMaterial() function in OpenGl_attri.c)
  Graphic3d_MaterialAspect aMaterial[ 2 ];
  aMaterial[ 0 ] = Graphic3d_MaterialAspect( Graphic3d_NOM_PLASTIC );
  aMaterial[ 1 ] = Graphic3d_MaterialAspect( Graphic3d_NOM_PLASTIC );
  Standard_Integer i;
  for ( i = 0; i < 2; i++ )
  {
    if ( !IsReflect )
    {
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_SPECULAR );
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_AMBIENT );
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_DIFFUSE );
      aMaterial[ i ].SetReflectionModeOff( Graphic3d_TOR_EMISSION );
    }
    else{
      // OCC20644 Using the material with reflection properties same as in
      // ElementalColorPrsBuilder, to get the same colors.
      // Additionally, ambient and diffuse coefficients are used below to scale incoming colors,
      // to simulate TelUpdateMaterial() function from OpenGl_attri.c.
      // This is mandatory, as these "scaled" colors are then passed directly to OpenGL
      // as ambient and diffuse colors of the current material using glColorMaterial().
      // In ElementalColorPrsBuilder we do not need to do scale the colors, as this
      // is done by TelUpdateMaterial().
      // 0.5 is used to have the colors in 3D maximally similar to those in the color scale.
      // This is possible when the sum of all coefficient is equal to 1.
      aMaterial[i].SetAmbient( .5 );
      aMaterial[i].SetDiffuse( .5 );
      aMaterial[i].SetSpecular( 0. );
      aMaterial[i].SetEmissive( 0. );
    }

 }


  // Create array of polygons for interior presentation of faces and volumes
  Handle(Graphic3d_ArrayOfPolygons) aCPolyArr = new Graphic3d_ArrayOfPolygons
    ( aMaxFaceNodes * aSize + PolygonVerticesFor3D, aSize + PolygonBoundsFor3D,
    0, myUseTexture || IsReflect, !myUseTexture, Standard_False, myUseTexture );

    Standard_Integer aNbFacePrimitives = 0;
    Standard_Integer aNbVolmPrimitives = 0;
    Standard_Integer aNbEdgePrimitives = 0;
    Standard_Integer aNbLinkPrimitives = 0;

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

            aNbEdgePrimitives += aFaceNodes.Length();     // add edge segments
            aNbVolmPrimitives += aFaceNodes.Length() - 2; // add volumetric cell triangles
          }
        }
      }
      else if (aType == MeshVS_ET_Link)
      {
        aNbLinkPrimitives += aNbNodes - 1; // add link segments
      }
      else if (aType == MeshVS_ET_Face)
      {
        aNbEdgePrimitives += aNbNodes;     // add edge segments
        aNbFacePrimitives += aNbNodes - 2; // add face triangles
      }
    }

  // Here we do not use indices arrays because they are not effective for some mesh
  // drawing modes: shrinking mode (displaces the vertices inside the polygon), 3D
  // cell rendering (normal interpolation is not always applicable - flat shading),
  // elemental coloring (color interpolation is impossible)

  // Create array of polygons for interior presentation of faces and volumes
  Handle(Graphic3d_ArrayOfTriangles) aFaceTriangles = new Graphic3d_ArrayOfTriangles
    ( (aNbFacePrimitives + aNbVolmPrimitives) * 3, 0, myUseTexture || IsReflect, !myUseTexture, myUseTexture );

  // Create array of polylines for presentation of edges
  Handle(Graphic3d_ArrayOfSegments) anEdgeSegments = new Graphic3d_ArrayOfSegments
    (aNbEdgePrimitives * 2);

  gp_Pnt P, Start;
  Standard_Real aMin = gp::Resolution() * gp::Resolution();
  gp_Dir aDefNorm( 0., 0., 1. );

  // Prepare for scaling the incoming colors
  Standard_Real anColorRatio = aMaterial[0].Ambient();

  for (it.Reset(); it.More(); it.Next())
  {
    Standard_Integer aKey = it.Key();

    if (aSource->GetGeom (aKey, Standard_True, aCoords, NbNodes, aType))
    {
      TColStd_Array1OfInteger aNodes (1, NbNodes);
      
      if (!aSource->GetNodesByElement (aKey, aNodes, NbNodes))
        continue;

      Quantity_Color aNColor;

      Standard_Boolean isValid = Standard_True;
      
      if (myUseTexture)
      {
        for (Standard_Integer k = 1; k <= NbNodes && isValid; ++k)
          isValid = myTextureCoords.IsBound (aNodes (k));
      }
      else
      {
        for (Standard_Integer k = 1; k <= NbNodes && isValid; ++k)
          isValid = GetColor (aNodes (k), aNColor);
      }

      if (!isValid)
        continue;

      // Preparing normal(s) to show reflections if requested
      Handle(TColStd_HArray1OfReal) aNormals;

      Standard_Boolean hasNormals =
        (IsReflect && aSource->GetNormalsByElement (aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals));

      if (aType == MeshVS_ET_Face)
      {
        for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes - 2; ++aNodeIdx) // triangulate polygon
        {
          for (Standard_Integer aSubIdx = 0; aSubIdx < 3; ++aSubIdx) // generate sub-triangle
          {
            gp_XYZ aPnt (aCoords (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 1),
                         aCoords (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 2),
                         aCoords (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 3));

            gp_Vec aNorm = aDefNorm;

            if (hasNormals)
            {
              gp_Vec aTestNorm (aNormals->Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 1),
                                aNormals->Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 2),
                                aNormals->Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 3));

              if (aTestNorm.SquareMagnitude() > aMin)
              {
                aNorm = gp_Dir (aTestNorm);
              }
            }

            if (myUseTexture)
            {
              const Standard_Real aTexCoord = myTextureCoords (aNodes (aSubIdx == 0 ? 1 : (aNodeIdx + aSubIdx + 1)));

              // Transform texture coordinate in accordance with number of colors specified
              // by upper level and real size of OpenGL texture. The OpenGL texture has border
              // colors interpolated with the colors from the color map, thats why we need to
              // shrink texture coordinates around the middle point to exclude areas where the
              // map colors are interpolated with the borders color
              aFaceTriangles->AddVertex (aPnt, aNorm, gp_Pnt2d (
                (aTexCoord * (nbColors - 1.0) + 0.5) / nbTextureColors, aTexCoord < 0 || aTexCoord > 1 ? 0.25 : 0.75));
            }
            else
            {
              GetColor (aNodes (aSubIdx == 0 ? 1 : (aNodeIdx + aSubIdx + 1)), aNColor);
              
              if (IsReflect)
              {
                aNColor.SetValues (anColorRatio * aNColor.Red(),
                                   anColorRatio * aNColor.Green(),
                                   anColorRatio * aNColor.Blue(),
                                   Quantity_TOC_RGB);

                aFaceTriangles->AddVertex (aPnt, aNorm, aNColor);
              }
              else
              {
                aFaceTriangles->AddVertex (aPnt, aNColor);
              }
            }
          }
        }

        for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes; ++aNodeIdx) // border segmentation
        {
          const Standard_Integer aNextIdx = (aNodeIdx + 1) % NbNodes;

          anEdgeSegments->AddVertex (aCoords (3 * aNodeIdx + 1),
                                     aCoords (3 * aNodeIdx + 2),
                                     aCoords (3 * aNodeIdx + 3));

          anEdgeSegments->AddVertex (aCoords (3 * aNextIdx + 1),
                                     aCoords (3 * aNextIdx + 2),
                                     aCoords (3 * aNextIdx + 3));
        }

        // if IsExcludingOn then presentation must not be built by other builders
        if (IsExcludingOn())
        {
          IDsToExclude.Add (aKey);
        }
      }
      else if (aType == MeshVS_ET_Volume)
      {
        if (!aSource->Get3DGeom (aKey, NbNodes, aTopo))
          continue;

        AddVolumePrs (aTopo, aNodes, aCoords, aFaceTriangles,
          IsReflect, nbColors, nbTextureColors, anColorRatio);
        
        AddVolumePrs (aTopo, aNodes, aCoords, anEdgeSegments,
          IsReflect, nbColors, nbTextureColors, anColorRatio);

        // if IsExcludingOn then presentation must not be built by other builders
        if (IsExcludingOn())
          IDsToExclude.Add (aKey);
      }
    }
  } // for ( ...

  Handle(Graphic3d_AspectFillArea3d) anAsp;

//  Aspect_InteriorStyle  aStyle;
//  Standard_Integer      aStyleInt;
  Aspect_TypeOfLine     anEdgeType = Aspect_TOL_SOLID;
  Standard_Integer      anEdgeInt;
  Standard_Real         anEdgeWidth;
  Quantity_Color        anInteriorColor;
  Quantity_Color        anEdgeColor, aLineColor;
  Standard_Boolean      aShowEdges = Standard_True;

  aDrawer->GetColor  ( MeshVS_DA_InteriorColor, anInteriorColor );
  aDrawer->GetColor  ( MeshVS_DA_EdgeColor, anEdgeColor );
  aDrawer->GetColor  ( MeshVS_DA_BeamColor, aLineColor );
  aDrawer->GetDouble ( MeshVS_DA_EdgeWidth, anEdgeWidth );
  aDrawer->GetBoolean( MeshVS_DA_ShowEdges, aShowEdges );

  if ( aDrawer->GetInteger ( MeshVS_DA_EdgeType, anEdgeInt ) )
    anEdgeType = (Aspect_TypeOfLine) anEdgeInt;

  if ( myUseTexture )
  {
    Handle(Prs3d_Drawer) aPrsDrawer =  myParentMesh->Attributes();
    if ( aPrsDrawer.IsNull() )
      return;

    aPrsDrawer->SetShadingAspect( new Prs3d_ShadingAspect() );
    anAsp = aPrsDrawer->ShadingAspect()->Aspect();
    if ( anAsp.IsNull() )
      return;

    anAsp->SetFrontMaterial( aMaterial[ 0 ] );
    anAsp->SetBackMaterial( aMaterial[ 1 ] );


    Handle(Graphic3d_Texture2D) aTexture = CreateTexture();
    if ( aTexture.IsNull() )
      return;

    anAsp->SetTextureMapOn();
    anAsp->SetTextureMap( aTexture );
    anAsp->SetInteriorColor( Quantity_NOC_WHITE );
  }
  else
  {
//    if ( aDrawer->GetInteger ( MeshVS_DA_InteriorStyle, aStyleInt ) )
//      aStyle = (Aspect_InteriorStyle)aStyleInt;

    anAsp = new Graphic3d_AspectFillArea3d (
      Aspect_IS_SOLID, Quantity_NOC_GRAY, anEdgeColor,
      anEdgeType, anEdgeWidth, aMaterial[ 0 ], aMaterial[ 1 ] );
  }

  anAsp->SetDistinguishOff();
  anAsp->SetEdgeOff();

  Handle(Graphic3d_AspectLine3d) anLAsp =
    new Graphic3d_AspectLine3d( anEdgeColor, anEdgeType, anEdgeWidth );

  Handle(Graphic3d_Group) aGroup1 = Prs3d_Root::NewGroup (Prs);

  Standard_Boolean toSupressBackFaces = Standard_False;
  aDrawer->GetBoolean (MeshVS_DA_SupressBackFaces, toSupressBackFaces);
  aGroup1->SetClosed (toSupressBackFaces == Standard_True);

  aGroup1->SetPrimitivesAspect( anAsp );
  aGroup1->AddPrimitiveArray( aFaceTriangles /*aCPolyArr*/ );
  //aGroup1->AddPrimitiveArray( aCPolyArr );

  if (aShowEdges)
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle(Graphic3d_Group) aGroup2 = Prs3d_Root::CurrentGroup ( Prs );

    Handle(Graphic3d_AspectFillArea3d) anAspCopy = new Graphic3d_AspectFillArea3d (*anAsp);
    anAspCopy->SetTextureMapOff();
    aGroup2->SetPrimitivesAspect( anAspCopy );
    aGroup2->SetPrimitivesAspect( anLAsp );
    aGroup2->AddPrimitiveArray( anEdgeSegments );
  }
}

//================================================================
// Function : AddVolumePrs
// Purpose  :
//================================================================
void MeshVS_NodalColorPrsBuilder::AddVolumePrs (const Handle(MeshVS_HArray1OfSequenceOfInteger)& theTopo,
                                                const TColStd_Array1OfInteger&                   theNodes,
                                                const TColStd_Array1OfReal&                      theCoords,
                                                const Handle(Graphic3d_ArrayOfPrimitives)&       theArray,
                                                const Standard_Boolean                           theIsShaded,
                                                const Standard_Integer                           theNbColors,
                                                const Standard_Integer                           theNbTexColors,
                                                const Standard_Real                              theColorRatio) const
{
  Standard_Integer aLow = theCoords.Lower();

  if (theTopo.IsNull() || theArray.IsNull())
    return;

  Standard_Boolean aIsPolygons = theArray->IsKind (STANDARD_TYPE (Graphic3d_ArrayOfTriangles));

  if (aIsPolygons)
  {    
    for (Standard_Integer aFaceIdx = theTopo->Lower(), topoup = theTopo->Upper(); aFaceIdx <= topoup; ++aFaceIdx)
    {
      const TColStd_SequenceOfInteger& aFaceNodes = theTopo->Value (aFaceIdx);
      
      TColStd_Array1OfReal aPolyNodes (0, 3 * aFaceNodes.Length());

      for (Standard_Integer aNodeIdx = 0; aNodeIdx < aFaceNodes.Length(); ++aNodeIdx)
      {
        Standard_Integer anIdx = aFaceNodes.Value (aNodeIdx + 1);

        Standard_Real aX = theCoords.Value (aLow + 3 * anIdx + 0);
        Standard_Real aY = theCoords.Value (aLow + 3 * anIdx + 1);
        Standard_Real aZ = theCoords.Value (aLow + 3 * anIdx + 2);

        aPolyNodes.SetValue (3 * aNodeIdx + 1, aX);
        aPolyNodes.SetValue (3 * aNodeIdx + 2, aY);
        aPolyNodes.SetValue (3 * aNodeIdx + 3, aZ);
      }
      
      gp_Vec aNorm (0.0, 0.0, 1.0);

      if (theIsShaded)
      {
        aPolyNodes.SetValue (0, aFaceNodes.Length());
        
        if (!MeshVS_Tool::GetAverageNormal (aPolyNodes, aNorm))
        {
          aNorm.SetCoord (0.0, 0.0, 1.0);
        }
      }

      for (Standard_Integer aNodeIdx = 0; aNodeIdx < aFaceNodes.Length() - 2; ++aNodeIdx) // triangulate polygon
      {
        for (Standard_Integer aSubIdx = 0; aSubIdx < 3; ++aSubIdx) // generate sub-triangle
        {
          gp_Pnt aPnt (aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 1),
                       aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 2),
                       aPolyNodes.Value (3 * (aSubIdx == 0 ? 0 : (aNodeIdx + aSubIdx)) + 3));

          if (myUseTexture)
          {
            const Standard_Real aTexCoord = myTextureCoords (theNodes (aFaceNodes (aSubIdx == 0 ? 1 : (aNodeIdx + aSubIdx + 1)) + 1));

            theArray->AddVertex (aPnt, aNorm, gp_Pnt2d (
              (aTexCoord * (theNbColors - 1.0) + 0.5) / theNbTexColors, aTexCoord < 0 || aTexCoord > 1 ? 0.25 : 0.75));
          }
          else
          {
            Quantity_Color aNColor;
            GetColor (theNodes ((aFaceNodes (aSubIdx == 0 ? 1 : (aNodeIdx + aSubIdx + 1)) + 1)), aNColor);

            if (theIsShaded)
            {
              aNColor.SetValues (theColorRatio * aNColor.Red(),
                                 theColorRatio * aNColor.Green(),
                                 theColorRatio * aNColor.Blue(),
                                 Quantity_TOC_RGB);

              theArray->AddVertex (aPnt, aNorm, aNColor);
            }
            else
            {
              theArray->AddVertex (aPnt, aNColor);
            }
          }
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

      Standard_Real aX[] = { theCoords.Value (anIdx1 + 0), theCoords.Value (anIdx2 + 0) };
      Standard_Real aY[] = { theCoords.Value (anIdx1 + 1), theCoords.Value (anIdx2 + 1) };
      Standard_Real aZ[] = { theCoords.Value (anIdx1 + 2), theCoords.Value (anIdx2 + 2) };

      theArray->AddVertex (aX[0], aY[0], aZ[0]);
      theArray->AddVertex (aX[1], aY[1], aZ[1]);
    }
  }
}

//================================================================
// Function : SetColors
// Purpose  :
//================================================================
void MeshVS_NodalColorPrsBuilder::SetColors (
  const MeshVS_DataMapOfIntegerColor& theColorMap )
{
  myNodeColorMap = theColorMap;
}

//================================================================
// Function : GetColors
// Purpose  :
//================================================================
const MeshVS_DataMapOfIntegerColor& MeshVS_NodalColorPrsBuilder::GetColors() const
{
  return myNodeColorMap;
}

//================================================================
// Function : HasColors
// Purpose  :
//================================================================
Standard_Boolean MeshVS_NodalColorPrsBuilder::HasColors () const
{
  return ( myNodeColorMap.Extent() >0 );
}

//================================================================
// Function : GetColor
// Purpose  :
//================================================================
Standard_Boolean MeshVS_NodalColorPrsBuilder::GetColor ( const Standard_Integer ID,
                                                         Quantity_Color& theColor ) const
{
  Standard_Boolean aRes = myNodeColorMap.IsBound ( ID );
  if ( aRes )
    theColor = myNodeColorMap.Find ( ID );
  return aRes;
}

//================================================================
// Function : SetColor
// Purpose  :
//================================================================
void MeshVS_NodalColorPrsBuilder::SetColor ( const Standard_Integer theID,
                                             const Quantity_Color& theCol )
{
  Standard_Boolean aRes = myNodeColorMap.IsBound ( theID );
  if ( aRes )
    myNodeColorMap.ChangeFind ( theID ) = theCol;
  else
    myNodeColorMap.Bind ( theID, theCol );
}

//================================================================
// Function : UseTexture
// Purpose  : Specify whether texture must be used to build presentation
//================================================================
void MeshVS_NodalColorPrsBuilder::UseTexture( const Standard_Boolean theToUse )
{
  myUseTexture = theToUse;
  if ( myUseTexture )
    myNodeColorMap.Clear();
  else
    myTextureColorMap.Clear();
}

//================================================================
// Function : IsUseTexture
// Purpose  : Verify whether texture is used to build presentation
//================================================================
Standard_Boolean MeshVS_NodalColorPrsBuilder::IsUseTexture() const
{
  return myUseTexture;
}

//================================================================
// Function : SetColorMap
// Purpose  : Set colors to be used for texrture presentation.
//            Generate texture in accordance with given parameters
//================================================================
void MeshVS_NodalColorPrsBuilder::SetColorMap( const Aspect_SequenceOfColor& theColors )
{
  myTextureColorMap = theColors;
}

//================================================================
// Function : GetColorMap
// Purpose  : Return colors used for texrture presentation
//================================================================
const Aspect_SequenceOfColor& MeshVS_NodalColorPrsBuilder::GetColorMap() const
{
  return myTextureColorMap;
}

//================================================================
// Function : SetInvalidColor
// Purpose  : Set color representing invalid texture coordinate
//            (laying outside range [0, 1])
//================================================================
void MeshVS_NodalColorPrsBuilder::SetInvalidColor(
  const Quantity_Color& theInvalidColor )
{
  myInvalidColor = theInvalidColor;
}

//================================================================
// Function : GetInvalidColor
// Purpose  : Return color representing invalid texture coordinate
//            (laying outside range [0, 1])
//================================================================
Quantity_Color MeshVS_NodalColorPrsBuilder::GetInvalidColor() const
{
  return myInvalidColor;
}

//================================================================
// Function : SetTextureCoords
// Purpose  : Specify correspondence between node IDs and texture
//            coordinates (range [0, 1])
//================================================================
void MeshVS_NodalColorPrsBuilder::SetTextureCoords (
  const TColStd_DataMapOfIntegerReal& theMap )
{
  myTextureCoords = theMap;
}

//================================================================
// Function : GetTextureCoords
// Purpose  : Get correspondence between node IDs and texture
//            coordinates (range [0, 1])
//================================================================
const TColStd_DataMapOfIntegerReal& MeshVS_NodalColorPrsBuilder::GetTextureCoords() const
{
  return myTextureCoords;
}

//================================================================
// Function : SetTextureCoord
// Purpose  : Specify correspondence between node ID and texture
//            coordinate (range [0, 1])
//================================================================
void MeshVS_NodalColorPrsBuilder::SetTextureCoord( const Standard_Integer theID,
                                                   const Standard_Real theCoord )
{
  myTextureCoords.Bind( theID, theCoord );
}

//================================================================
// Function : GetTextureCoord
// Purpose  : Return correspondence between node IDs and texture
//            coordinate (range [0, 1])
//================================================================
Standard_Real MeshVS_NodalColorPrsBuilder::GetTextureCoord( const Standard_Integer theID )
{
  return myTextureCoords.IsBound( theID ) ? myTextureCoords( theID ) : -1;
}

//================================================================
// Function : CreateTexture
// Purpose  : Create texture in accordance with myTextureColorMap
//================================================================
Handle(Graphic3d_Texture2D) MeshVS_NodalColorPrsBuilder::CreateTexture() const
{
  const Standard_Integer aColorsNb = myTextureColorMap.Length();
  if (aColorsNb == 0)
  {
    return NULL;
  }

  // create and fill image with colors
  Handle(Image_PixMap) anImage = new Image_PixMap();
  if (!anImage->InitTrash (Image_Format_RGBA, Standard_Size(getNearestPow2 (aColorsNb)), 2))
  {
    return NULL;
  }

  anImage->SetTopDown (false);
  for (Standard_Size aCol = 0; aCol < Standard_Size(aColorsNb); ++aCol)
  {
    const Quantity_Color& aSrcColor = myTextureColorMap.Value (Standard_Integer(aCol) + 1);
    Image_ColorRGBA& aColor = anImage->ChangeValue<Image_ColorRGBA> (0, aCol);
    aColor.r() = Standard_Byte(255.0 * aSrcColor.Red());
    aColor.g() = Standard_Byte(255.0 * aSrcColor.Green());
    aColor.b() = Standard_Byte(255.0 * aSrcColor.Blue());
    aColor.a() = 0xFF;
  }

  // fill padding bytes
  const Quantity_Color& aLastColorSrc = myTextureColorMap.Last();
  const Image_ColorRGBA aLastColor =
  {{
    Standard_Byte(255.0 * aLastColorSrc.Red()),
    Standard_Byte(255.0 * aLastColorSrc.Green()),
    Standard_Byte(255.0 * aLastColorSrc.Blue()),
    0xFF
  }};

  // fill second row
  for (Standard_Size aCol = (Standard_Size )aColorsNb; aCol < anImage->SizeX(); ++aCol)
  {
    anImage->ChangeValue<Image_ColorRGBA> (0, aCol) = aLastColor;
  }

  const Image_ColorRGBA anInvalidColor =
  {{
    Standard_Byte(255.0 * myInvalidColor.Red()),
    Standard_Byte(255.0 * myInvalidColor.Green()),
    Standard_Byte(255.0 * myInvalidColor.Blue()),
    0xFF
  }};
  for (Standard_Size aCol = 0; aCol < anImage->SizeX(); ++aCol)
  {
    anImage->ChangeValue<Image_ColorRGBA> (1, aCol) = anInvalidColor;
  }

  // create texture
  return new MeshVS_ImageTexture2D (anImage);
}
