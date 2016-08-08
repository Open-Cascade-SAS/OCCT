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


#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfColorMapOfInteger.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfIntegerColor.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfIntegerTwoColors.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfTwoColorsMapOfInteger.hxx>
#include <MeshVS_DataMapOfColorMapOfInteger.hxx>
#include <MeshVS_DataMapOfTwoColorsMapOfInteger.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_ElementalColorPrsBuilder.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_ElementalColorPrsBuilder,MeshVS_PrsBuilder)

//================================================================
// Function : Constructor MeshVS_ElementalColorPrsBuilder
// Purpose  :
//================================================================
MeshVS_ElementalColorPrsBuilder::MeshVS_ElementalColorPrsBuilder
  ( const Handle(MeshVS_Mesh)& Parent,
    const MeshVS_DisplayModeFlags& Flags,
    const Handle (MeshVS_DataSource)& DS,
    const Standard_Integer Id,
    const MeshVS_BuilderPriority& Priority )
: MeshVS_PrsBuilder ( Parent, Flags, DS, Id, Priority )
{
  SetExcluding ( Standard_True );
}

//================================================================
// Function : Build
// Purpose  :
//================================================================
void MeshVS_ElementalColorPrsBuilder::Build ( const Handle(Prs3d_Presentation)& Prs,
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
  if ( !aDrawer->GetInteger ( MeshVS_DA_MaxFaceNodes, aMaxFaceNodes ) && aMaxFaceNodes<=0 )
    return;

  MeshVS_DataMapOfIntegerColor* anElemColorMap = (MeshVS_DataMapOfIntegerColor*) &myElemColorMap1;
  MeshVS_DataMapOfIntegerTwoColors* anElemTwoColorsMap = (MeshVS_DataMapOfIntegerTwoColors*)&myElemColorMap2;

  MeshVS_DataMapOfColorMapOfInteger     aColorsOfElements;
  MeshVS_DataMapOfTwoColorsMapOfInteger aTwoColorsOfElements;

  MeshVS_Buffer aCoordsBuf (3*aMaxFaceNodes*sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords (aCoordsBuf, 1, 3*aMaxFaceNodes);
  Standard_Integer NbNodes;
  MeshVS_EntityType aType;

  if ( !( DisplayMode & GetFlags() ) || !IsElement ||
       ( myElemColorMap1.IsEmpty() && myElemColorMap2.IsEmpty() ) )
    return;

  // subtract the hidden elements and ids to exclude (to minimise allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
  if ( !aHiddenElems.IsNull() )
    anIDs.Subtract( aHiddenElems->Map() );
  anIDs.Subtract( IDsToExclude );

  // STEP 0: We looking for two colored elements, who has equal two colors and move it
  // to map of elements with one assigned color
  TColStd_ListOfInteger aColorOne;
  for ( MeshVS_DataMapIteratorOfDataMapOfIntegerTwoColors anIter ( *anElemTwoColorsMap ); anIter.More(); anIter.Next () )
  {
    Standard_Integer aKey   = anIter.Key ();
    MeshVS_TwoColors aValue = anIter.Value ();
    Quantity_Color   aCol1, aCol2;
    ExtractColors ( aValue, aCol1, aCol2 );
    if ( aCol1 == aCol2 )
    {
      aColorOne.Append ( aKey );
      anElemColorMap->Bind ( aKey, aCol1 );
    }
  }

  for ( TColStd_ListIteratorOfListOfInteger aLIter ( aColorOne ); aLIter.More(); aLIter.Next() )
    anElemTwoColorsMap->UnBind ( aLIter.Value() );

  // The map is to resort itself by colors.
  // STEP 1: We start sorting elements with one assigned color
  for ( MeshVS_DataMapIteratorOfDataMapOfIntegerColor anIterM ( *anElemColorMap ); anIterM.More(); anIterM.Next () )
  {
    Standard_Integer aMKey = anIterM.Key ();
    // The ID of current element
    Standard_Boolean IsExist = Standard_False;
    for ( MeshVS_DataMapIteratorOfDataMapOfColorMapOfInteger anIterC ( aColorsOfElements );
          anIterC.More() && !IsExist; anIterC.Next () )
      if ( anIterC.Key()==anIterM.Value() )
      {
        TColStd_MapOfInteger& aChangeValue = (TColStd_MapOfInteger&) anIterC.Value();
        aChangeValue.Add ( aMKey );
        IsExist = Standard_True;
      }

    if ( !IsExist )
    {
      TColStd_MapOfInteger aNewMap; aNewMap.Add ( aMKey );
      aColorsOfElements.Bind ( anIterM.Value(), aNewMap );
    }
  }

  // STEP 2: We start sorting elements with two assigned colors
  for ( MeshVS_DataMapIteratorOfDataMapOfIntegerTwoColors anIterM2 ( *anElemTwoColorsMap ); anIterM2.More();
        anIterM2.Next () )
  {
    Standard_Integer aMKey = anIterM2.Key ();
    // The ID of current element
    Standard_Boolean IsExist = Standard_False;
    for ( MeshVS_DataMapIteratorOfDataMapOfTwoColorsMapOfInteger anIterC2 ( aTwoColorsOfElements );
          anIterC2.More() && !IsExist; anIterC2.Next () )
      if ( IsEqual ( anIterC2.Key(), anIterM2.Value() ) )
      {
        TColStd_MapOfInteger& aChangeValue = (TColStd_MapOfInteger&) anIterC2.Value();
        aChangeValue.Add ( aMKey );
        IsExist = Standard_True;
      }

    if ( !IsExist )
    {
      TColStd_MapOfInteger aNewMap; aNewMap.Add ( aMKey );
      aTwoColorsOfElements.Bind ( anIterM2.Value(), aNewMap );
    }
  }

  //Now we are ready to draw faces with equal colors
  Aspect_TypeOfLine    anEdgeType = Aspect_TOL_SOLID;
  Aspect_TypeOfLine    aLineType = Aspect_TOL_SOLID;
  Standard_Integer     anEdgeInt, aLineInt;
  Standard_Real        anEdgeWidth, aLineWidth;
  Quantity_Color       anInteriorColor;
  Quantity_Color       anEdgeColor, aLineColor;
  Standard_Boolean     anEdgeOn = Standard_True, IsReflect = Standard_False,
                       IsMeshSmoothShading = Standard_False;
  Standard_Boolean toSupressBackFaces = Standard_False;

  aDrawer->GetColor  ( MeshVS_DA_InteriorColor, anInteriorColor );
  aDrawer->GetColor  ( MeshVS_DA_EdgeColor, anEdgeColor );
  aDrawer->GetColor  ( MeshVS_DA_BeamColor, aLineColor );
  aDrawer->GetDouble ( MeshVS_DA_EdgeWidth, anEdgeWidth );
  aDrawer->GetDouble ( MeshVS_DA_BeamWidth, aLineWidth );
  aDrawer->GetBoolean( MeshVS_DA_ShowEdges, anEdgeOn );
  aDrawer->GetBoolean( MeshVS_DA_ColorReflection, IsReflect );
  aDrawer->GetBoolean( MeshVS_DA_SmoothShading, IsMeshSmoothShading );
  aDrawer->GetBoolean (MeshVS_DA_SupressBackFaces, toSupressBackFaces);

  if ( aDrawer->GetInteger ( MeshVS_DA_EdgeType, anEdgeInt) )
    anEdgeType = (Aspect_TypeOfLine) anEdgeInt;

  if ( aDrawer->GetInteger ( MeshVS_DA_BeamType, aLineInt) )
    aLineType = (Aspect_TypeOfLine) aLineInt;

  Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;
  Standard_Integer PolygonVerticesFor3D = 0, PolygonBoundsFor3D = 0;
  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);
  for( ; it.More(); it.Next() )
  {
    Standard_Integer aKey = it.Key();
    if( aSource->Get3DGeom( aKey, NbNodes, aTopo ) )
        MeshVS_MeshPrsBuilder::HowManyPrimitives
          ( aTopo, Standard_True, Standard_False, NbNodes,
            PolygonVerticesFor3D, PolygonBoundsFor3D );
  }

  Graphic3d_MaterialAspect aMaterial[2];
  for (Standard_Integer i = 0; i < 2; i++)
  {
    // OCC20644 "plastic" is most suitable here, as it is "non-physic"
    // so TelUpdateMaterial() from OpenGl_attri.c uses the interior
    // color from AspectFillArea3d to calculate all material colors
    aMaterial[i] = Graphic3d_MaterialAspect ( Graphic3d_NOM_PLASTIC );

    // OCC21720 For single-colored elements turning all material components off is a good idea,
    // as anyhow the normals are not computed and the lighting will be off,
    // the element color will be taken from Graphic3d_AspectFillArea3d's interior color,
    // and there is no need to spend time on updating material properties 
    if ( !IsReflect )
    {
      aMaterial[i].SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
      aMaterial[i].SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
      aMaterial[i].SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
      aMaterial[i].SetReflectionModeOff(Graphic3d_TOR_EMISSION);
    }
    else
    {
      // OCC20644 This stuff is important in order for elemental and nodal colors
      // to produce similar visual impression and also to make colors match
      // those in the color scale most exactly (the sum of all reflection 
      // coefficients is equal to 1). See also MeshVS_NodalColorPrsBuilder
      // class for more explanations.
      aMaterial[i].SetAmbient( .5 );
      aMaterial[i].SetDiffuse( .5 );
      aMaterial[i].SetSpecular( 0. );
      aMaterial[i].SetEmissive( 0. );
    }
  }

  // Draw elements with one color
  Handle(Graphic3d_Group) aGGroup, aGroup2, aLGroup, aSGroup;
  if (!aTwoColorsOfElements.IsEmpty())
  {
    aGroup2 = Prs3d_Root::NewGroup (Prs);
  }
  if (!aColorsOfElements.IsEmpty())
  {
    Handle(Graphic3d_AspectFillArea3d) aGroupFillAspect = new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID, anInteriorColor, anEdgeColor,
                                                                                          anEdgeType, anEdgeWidth, aMaterial[0], aMaterial[1]);
    aGGroup = Prs3d_Root::NewGroup (Prs);
    aLGroup = Prs3d_Root::NewGroup (Prs);
    aGGroup->SetClosed (toSupressBackFaces == Standard_True);
    aGGroup->SetGroupPrimitivesAspect (aGroupFillAspect);
  }

  if (anEdgeOn)
  {
    Handle(Graphic3d_AspectLine3d) anEdgeAspect = new Graphic3d_AspectLine3d (anEdgeColor, anEdgeType, anEdgeWidth);
    aSGroup = Prs3d_Root::NewGroup (Prs);
    aSGroup->SetGroupPrimitivesAspect (anEdgeAspect);
  }

  for (MeshVS_DataMapIteratorOfDataMapOfColorMapOfInteger aColIter (aColorsOfElements);
       aColIter.More(); aColIter.Next())
  {
    if (aColIter.Value().IsEmpty())
    {
      continue;
    }

    TColStd_PackedMapOfInteger aCustomElements;

    Standard_Integer aNbFacePrimitives = 0;
    Standard_Integer aNbVolmPrimitives = 0;
    Standard_Integer aNbEdgePrimitives = 0;
    Standard_Integer aNbLinkPrimitives = 0;

    for (it.Reset(); it.More(); it.Next())
    {
      Standard_Integer aNbNodes = 0;

      if (!aColIter.Value().Contains (it.Key()))
        continue;

      if (!aSource->GetGeom (it.Key(), Standard_True, aCoords, aNbNodes, aType))
        continue;

      if (aType == MeshVS_ET_Volume)
      {
        if (aSource->Get3DGeom (it.Key(), aNbNodes, aTopo))
        {
          for (Standard_Integer aFaceIdx = aTopo->Lower(); aFaceIdx <= aTopo->Upper(); ++aFaceIdx)
          {
            const TColStd_SequenceOfInteger& aFaceNodes = aTopo->Value (aFaceIdx);

            if (anEdgeOn) // add edge segments
            {
              aNbEdgePrimitives += aFaceNodes.Length();
            }

            aNbVolmPrimitives += aFaceNodes.Length() - 2;
          }
        }
      }
      else if (aType == MeshVS_ET_Link)
      {
        if (anEdgeOn)
        {
          aNbLinkPrimitives += aNbNodes - 1; // add link segments
        }
      }
      else if (aType == MeshVS_ET_Face)
      {
        if (anEdgeOn)
        {
          aNbEdgePrimitives += aNbNodes; // add edge segments
        }
          
        aNbFacePrimitives += aNbNodes - 2; // add face triangles
      }
    }

    // Here we do not use indices arrays because they are not effective for some mesh
    // drawing modes: shrinking mode (displaces the vertices inside the polygon), 3D
    // cell rendering (normal interpolation is not always applicable - flat shading),
    // elemental coloring (color interpolation is impossible)

    Handle (Graphic3d_ArrayOfTriangles) aFaceTriangles = new Graphic3d_ArrayOfTriangles (
     (aNbFacePrimitives + aNbVolmPrimitives) * 3, 0, IsReflect );
    Standard_Boolean IsPolyG = Standard_False;

    Handle (Graphic3d_ArrayOfSegments) anEdgeSegments = new Graphic3d_ArrayOfSegments (aNbEdgePrimitives * 2);
    Handle (Graphic3d_ArrayOfSegments) aLinkSegments = new Graphic3d_ArrayOfSegments (aNbLinkPrimitives * 2);
    Standard_Boolean IsPolyL = Standard_False;

    // OCC20644 NOTE: aColIter.Key() color is then scaled by TelUpdateMaterial() in OpenGl_attri.c
    // using the material reflection coefficients. This affects the visual result.
    Handle(Graphic3d_AspectFillArea3d) aFillAspect =
      new Graphic3d_AspectFillArea3d ( Aspect_IS_SOLID, aColIter.Key(), anEdgeColor,
                                       anEdgeType, anEdgeWidth, aMaterial[0], aMaterial[1] );

    Handle(Graphic3d_AspectLine3d) aLinkAspect =
      new Graphic3d_AspectLine3d ( aColIter.Key(), aLineType, aLineWidth );

    aFillAspect->SetDistinguishOff ();
    aFillAspect->SetInteriorColor ( aColIter.Key() );
    aFillAspect->SetEdgeOff();

    for (it.Reset(); it.More(); it.Next())
    {
      Standard_Integer aKey = it.Key();
      
      if (aColIter.Value().Contains (aKey))
      {
        if (!aSource->GetGeom (aKey, Standard_True, aCoords, NbNodes, aType))
          continue;
        
        if (aType != MeshVS_ET_Face && aType != MeshVS_ET_Link && aType != MeshVS_ET_Volume)
        {
          aCustomElements.Add (aKey);
          continue;
        }
        
        if (IsExcludingOn())
          IDsToExclude.Add (aKey);
          
        if (aType == MeshVS_ET_Volume)
        {
          if (!aSource->Get3DGeom (aKey, NbNodes, aTopo))
          {
            continue;
          }
          
          MeshVS_MeshPrsBuilder::AddVolumePrs (aTopo, aCoords,
            NbNodes, aFaceTriangles, IsReflect, Standard_False, Standard_False, 1.0);

          if (anEdgeOn)
          {
            MeshVS_MeshPrsBuilder::AddVolumePrs (aTopo, aCoords,
              NbNodes, anEdgeSegments, IsReflect, Standard_False, Standard_False, 1.0);
          }

          IsPolyG = Standard_True;
        }
        else if (aType == MeshVS_ET_Face)
        {
          // Preparing normals
          Handle(TColStd_HArray1OfReal) aNormals;
          Standard_Boolean aHasNormals = IsReflect && aSource->GetNormalsByElement (aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals);

          for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes - 2; ++aNodeIdx)
          {
            for (Standard_Integer anIdx = 0; anIdx < 3; ++anIdx)
            {
              if (IsReflect)
              {
                aFaceTriangles->AddVertex (aCoords (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 1),
                                           aCoords (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 2),
                                           aCoords (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 3),
                                           aHasNormals ? aNormals->Value (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 1) : 0.0,
                                           aHasNormals ? aNormals->Value (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 2) : 0.0,
                                           aHasNormals ? aNormals->Value (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 3) : 1.0);
              }
              else
              {
                aFaceTriangles->AddVertex (aCoords (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 1),
                                           aCoords (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 2),
                                           aCoords (3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 3));
              }
            }
          }

          if (anEdgeOn)
          {
            for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes; ++aNodeIdx)
            {
              const Standard_Integer aNextIdx = (aNodeIdx + 1) % NbNodes;

              anEdgeSegments->AddVertex (aCoords (3 * aNodeIdx + 1),
                                         aCoords (3 * aNodeIdx + 2),
                                         aCoords (3 * aNodeIdx + 3));
              
              anEdgeSegments->AddVertex (aCoords (3 * aNextIdx + 1),
                                         aCoords (3 * aNextIdx + 2),
                                         aCoords (3 * aNextIdx + 3));
            }
          }

          IsPolyG = Standard_True;
        }
        else if (aType == MeshVS_ET_Link)
        {
          for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes - 1; ++aNodeIdx)
          {
            const Standard_Integer aNextIdx = aNodeIdx + 1;

            aLinkSegments->AddVertex (aCoords (3 * aNodeIdx + 1),
                                      aCoords (3 * aNodeIdx + 2),
                                      aCoords (3 * aNodeIdx + 3));

            aLinkSegments->AddVertex (aCoords (3 * aNextIdx + 1),
                                      aCoords (3 * aNextIdx + 2),
                                      aCoords (3 * aNextIdx + 3));

            IsPolyL = Standard_True;
          }
        }
      }
    }

    if (IsPolyG)
    {
      aGGroup->SetPrimitivesAspect (aFillAspect);
      aGGroup->AddPrimitiveArray (aFaceTriangles);
      aGGroup->SetClosed (toSupressBackFaces == Standard_True);
      
      if (anEdgeOn)
      {
        aSGroup->AddPrimitiveArray (anEdgeSegments);
      }
    }
    if (IsPolyL)
    {
      aLGroup->SetPrimitivesAspect (aFillAspect);
      aLGroup->SetPrimitivesAspect (aLinkAspect);
      aLGroup->AddPrimitiveArray (aLinkSegments);
    }

    if (!aCustomElements.IsEmpty())
      CustomBuild(Prs, aCustomElements, IDsToExclude, DisplayMode);
  }

  Graphic3d_MaterialAspect aMaterial2[2];
  for (Standard_Integer i = 0; i < 2; i++)
  {
    // OCC20644 "plastic" is most suitable here, as it is "non-physic"
    // so TelUpdateMaterial() from OpenGl_attri.c uses the interior
    // color from AspectFillArea3d to calculate all material colors
    aMaterial2[i] = Graphic3d_MaterialAspect ( Graphic3d_NOM_PLASTIC );

    if ( !IsReflect )
    {
      // OCC21720 Cannot turn ALL material components off, as such a material
      // would be ignored by TelUpdateMaterial(), but we need it in order
      // to have different materials for front and back sides!
      // Instead, trying to make material color "nondirectional" with 
      // only ambient component on.
      aMaterial2[i].SetReflectionModeOn ( Graphic3d_TOR_AMBIENT );
      aMaterial2[i].SetReflectionModeOff( Graphic3d_TOR_DIFFUSE );
      aMaterial2[i].SetReflectionModeOff( Graphic3d_TOR_SPECULAR );
      aMaterial2[i].SetReflectionModeOff( Graphic3d_TOR_EMISSION );
      aMaterial2[i].SetAmbient ( 1. );
      aMaterial2[i].SetDiffuse ( 0. );
      aMaterial2[i].SetSpecular( 0. );
      aMaterial2[i].SetEmissive( 0. );
    }
    else
    {
      // OCC20644 This stuff is important in order for elemental and nodal colors
      // to produce similar visual impression and also to make colors match
      // those in the color scale most exactly (the sum of all reflection 
      // coefficients is equal to 1). See also MeshVS_NodalColorPrsBuilder
      // class for more explanations.
      aMaterial2[i].SetAmbient( .5 );
      aMaterial2[i].SetDiffuse( .5 );
      aMaterial2[i].SetSpecular( 0. );
      aMaterial2[i].SetEmissive( 0. );
    }
  }

  // Draw faces with two color
  if (!aTwoColorsOfElements.IsEmpty())
  {
    Handle(Graphic3d_AspectFillArea3d) aGroupFillAspect2 = new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID, anInteriorColor, anEdgeColor,
                                                                                          anEdgeType, anEdgeWidth, aMaterial2[0], aMaterial2[1]);
    aGroup2->SetClosed (Standard_False); // ignore toSupressBackFaces
    aGroup2->SetGroupPrimitivesAspect (aGroupFillAspect2);
  }
  for ( MeshVS_DataMapIteratorOfDataMapOfTwoColorsMapOfInteger aColIter2 ( aTwoColorsOfElements );
        aColIter2.More(); aColIter2.Next() )
  {
    if (aColIter2.Value().IsEmpty())
    {
      continue;
    }

    Standard_Integer aNbFacePrimitives = 0;
    Standard_Integer aNbEdgePrimitives = 0;
    
    for (it.Reset(); it.More(); it.Next())
    {
      Standard_Integer aNbNodes = 0;

      if (!aColIter2.Value().Contains (it.Key()))
        continue;

      if (!aSource->GetGeom (it.Key(), Standard_True, aCoords, aNbNodes, aType))
        continue;

      if ( aType == MeshVS_ET_Face && aNbNodes > 0 )
      {
        if (anEdgeOn)
        {
          aNbEdgePrimitives += aNbNodes; // add edge segments
        }

        aNbFacePrimitives += aNbNodes - 2; // add face triangles
      }
    }

    Handle (Graphic3d_ArrayOfTriangles) aFaceTriangles = new Graphic3d_ArrayOfTriangles
      (aNbFacePrimitives * 3, 0, IsReflect);

    Handle (Graphic3d_ArrayOfSegments) anEdgeSegments = new Graphic3d_ArrayOfSegments
      (aNbEdgePrimitives * 2);

    MeshVS_TwoColors aTC = aColIter2.Key();
    Quantity_Color aMyIntColor, aMyBackColor;
    ExtractColors ( aTC, aMyIntColor, aMyBackColor );

    // OCC20644 NOTE: aMyIntColor color is then scaled by TelUpdateMaterial() in OpenGl_attri.c
    // using the material reflection coefficients. This affects the visual result.
    Handle(Graphic3d_AspectFillArea3d) anAsp =
      new Graphic3d_AspectFillArea3d ( Aspect_IS_SOLID, aMyIntColor, anEdgeColor,
                                         anEdgeType, anEdgeWidth, aMaterial2[0], aMaterial2[1] );
    anAsp->SetDistinguishOn ();
    anAsp->SetInteriorColor ( aMyIntColor );
    anAsp->SetBackInteriorColor ( aMyBackColor );
    /*if (anEdgeOn)
      anAsp->SetEdgeOn();
    else
      anAsp->SetEdgeOff();*/
    aGroup2->SetPrimitivesAspect (anAsp);

    for( it.Reset(); it.More(); it.Next() )
    {
      Standard_Integer aKey = it.Key();
      if( aColIter2.Value().Contains( aKey ) )
      {
        if ( !aSource->GetGeom ( aKey, Standard_True, aCoords, NbNodes, aType ) )
          continue;

        if( IsExcludingOn() )
          IDsToExclude.Add( aKey );

        if (aType == MeshVS_ET_Face && NbNodes > 0)
        {
          // Preparing normal(s) to show reflections if requested
          Handle(TColStd_HArray1OfReal) aNormals;
          // OCC21720 Always passing normals to OpenGL to make materials work
          // For OpenGL: "No normals" -> "No lighting" -> "no materials taken into account"
          Standard_Boolean aHasNormals = /*IsReflect &&*/
            aSource->GetNormalsByElement (aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals);

          for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes - 2; ++aNodeIdx)
          {
            for (Standard_Integer anIdx = 0; anIdx < 3; ++anIdx)
            {
              if (IsReflect)
              {
                aFaceTriangles->AddVertex (aCoords (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 1),
                                           aCoords (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 2),
                                           aCoords (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 3),
                                           aHasNormals ? aNormals->Value (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 1) : 0.0,
                                           aHasNormals ? aNormals->Value (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 2) : 0.0,
                                           aHasNormals ? aNormals->Value (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 3) : 1.0);
              }
              else
              {
                aFaceTriangles->AddVertex (aCoords (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 1),
                                           aCoords (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 2),
                                           aCoords (3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 3));
              }
            }
          }

          if (anEdgeOn)
          {
            for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes; ++aNodeIdx)
            {
              const Standard_Integer aNextIdx = (aNodeIdx + 1) % NbNodes;

              anEdgeSegments->AddVertex (aCoords (3 * aNodeIdx + 1),
                                         aCoords (3 * aNodeIdx + 2),
                                         aCoords (3 * aNodeIdx + 3));

              anEdgeSegments->AddVertex (aCoords (3 * aNextIdx + 1),
                                         aCoords (3 * aNextIdx + 2),
                                         aCoords (3 * aNextIdx + 3));
            }
          }
        }
      }
    }

    aGroup2->AddPrimitiveArray (aFaceTriangles);
    aSGroup->AddPrimitiveArray (anEdgeSegments);
  }
}

//================================================================
// Function : SetColors
// Purpose  :
//================================================================
void MeshVS_ElementalColorPrsBuilder::SetColors1 ( const MeshVS_DataMapOfIntegerColor& theColorMap )
{
  myElemColorMap1 = theColorMap;
}

//================================================================
// Function : GetColors
// Purpose  :
//================================================================
const MeshVS_DataMapOfIntegerColor& MeshVS_ElementalColorPrsBuilder::GetColors1 () const
{
  return myElemColorMap1;
}

//================================================================
// Function : HasColors1
// Purpose  :
//================================================================
Standard_Boolean MeshVS_ElementalColorPrsBuilder::HasColors1 () const
{
  return ( myElemColorMap1.Extent() >0 );
}

//================================================================
// Function : GetColor1
// Purpose  :
//================================================================
Standard_Boolean MeshVS_ElementalColorPrsBuilder::GetColor1 ( const Standard_Integer ID,
                                                              Quantity_Color& theColor ) const
{
  Standard_Boolean aRes = myElemColorMap1.IsBound ( ID );
  if ( aRes )
    theColor = myElemColorMap1.Find ( ID );

  return aRes;
}

//================================================================
// Function : SetColor1
// Purpose  :
//================================================================
void MeshVS_ElementalColorPrsBuilder::SetColor1 ( const Standard_Integer theID,
                                                  const Quantity_Color& theCol )
{
  Standard_Boolean aRes = myElemColorMap1.IsBound ( theID );
  if ( aRes )
    myElemColorMap1.ChangeFind ( theID ) = theCol;
  else
    myElemColorMap1.Bind ( theID, theCol );
}

//================================================================
// Function : SetColors2
// Purpose  :
//================================================================
void MeshVS_ElementalColorPrsBuilder::SetColors2 ( const MeshVS_DataMapOfIntegerTwoColors& theColorMap )
{
  myElemColorMap2 = theColorMap;
}

//================================================================
// Function : GetColors2
// Purpose  :
//================================================================
const MeshVS_DataMapOfIntegerTwoColors& MeshVS_ElementalColorPrsBuilder::GetColors2 () const
{
  return myElemColorMap2;
}

//================================================================
// Function : HasColors2
// Purpose  :
//================================================================
Standard_Boolean MeshVS_ElementalColorPrsBuilder::HasColors2 () const
{
  return (myElemColorMap2.Extent()>0);
}

//================================================================
// Function : GetColor2
// Purpose  :
//================================================================
Standard_Boolean MeshVS_ElementalColorPrsBuilder::GetColor2 ( const Standard_Integer ID,
                                                              MeshVS_TwoColors& theColor ) const
{
  Standard_Boolean aRes = myElemColorMap2.IsBound ( ID );
  if ( aRes )
    theColor = myElemColorMap2.Find ( ID );

  return aRes;
}

//================================================================
// Function : GetColor2
// Purpose  :
//================================================================
Standard_Boolean MeshVS_ElementalColorPrsBuilder::GetColor2 ( const Standard_Integer ID,
                                                              Quantity_Color& theColor1,
                                                              Quantity_Color& theColor2 ) const
{
  MeshVS_TwoColors aTC;
  Standard_Boolean aRes = GetColor2 ( ID, aTC );
  if ( aRes)
    ExtractColors ( aTC, theColor1, theColor2 );
  return aRes;
}

//================================================================
// Function : SetColor2
// Purpose  :
//================================================================
void MeshVS_ElementalColorPrsBuilder::SetColor2 ( const Standard_Integer theID,
                                                  const Quantity_Color& theCol1,
                                                  const Quantity_Color& theCol2 )
{
  SetColor2 ( theID, BindTwoColors ( theCol1, theCol2 ) );
}

//================================================================
// Function : SetColor2
// Purpose  :
//================================================================
void MeshVS_ElementalColorPrsBuilder::SetColor2 ( const Standard_Integer theID,
                                                  const MeshVS_TwoColors& theCol )
{
  Standard_Boolean aRes = myElemColorMap2.IsBound ( theID );
  if ( aRes )
    myElemColorMap2.ChangeFind ( theID ) = theCol;
  else
    myElemColorMap2.Bind ( theID, theCol );
}
