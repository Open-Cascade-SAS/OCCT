// Created on: 2003-11-12
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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


#define _POLYGONES_
// if define _POLYGONES_ ColorPrsBuilder use ArrayOfPolygons for drawing faces

#include <MeshVS_ElementalColorPrsBuilder.ixx>

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>

#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_DataMapOfColorMapOfInteger.hxx>
#include <MeshVS_DataMapOfTwoColorsMapOfInteger.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfIntegerTwoColors.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfIntegerColor.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfTwoColorsMapOfInteger.hxx>
#include <MeshVS_DataMapIteratorOfDataMapOfColorMapOfInteger.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <MeshVS_Buffer.hxx>


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
  Aspect_InteriorStyle aStyle;
  Standard_Integer     aStyleInt;
  Aspect_TypeOfLine    anEdgeType = Aspect_TOL_SOLID;
  Aspect_TypeOfLine    aLineType = Aspect_TOL_SOLID;
  Standard_Integer     anEdgeInt, aLineInt;
  Standard_Real        anEdgeWidth, aLineWidth;
  Quantity_Color       anInteriorColor;
  Quantity_Color       anEdgeColor, aLineColor;
  Standard_Boolean     anEdgeOn = Standard_True, IsReflect = Standard_False,
                       IsMeshSmoothShading = Standard_False;

  aDrawer->GetColor  ( MeshVS_DA_InteriorColor, anInteriorColor );
  aDrawer->GetColor  ( MeshVS_DA_EdgeColor, anEdgeColor );
  aDrawer->GetColor  ( MeshVS_DA_BeamColor, aLineColor );
  aDrawer->GetDouble ( MeshVS_DA_EdgeWidth, anEdgeWidth );
  aDrawer->GetDouble ( MeshVS_DA_BeamWidth, aLineWidth );
  aDrawer->GetBoolean( MeshVS_DA_ShowEdges, anEdgeOn );
  aDrawer->GetBoolean( MeshVS_DA_ColorReflection, IsReflect );
  aDrawer->GetBoolean( MeshVS_DA_SmoothShading, IsMeshSmoothShading );

  if ( aDrawer->GetInteger ( MeshVS_DA_InteriorStyle, aStyleInt) )
    aStyle = (Aspect_InteriorStyle) aStyleInt;

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
  Standard_Integer i;
  for ( i=0; i<2; i++ )
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
  for ( MeshVS_DataMapIteratorOfDataMapOfColorMapOfInteger aColIter ( aColorsOfElements ); aColIter.More();
        aColIter.Next() )
  {
    Standard_Integer aSize = aColIter.Value().Extent();
    if ( aSize<=0 )
      continue;

    TColStd_PackedMapOfInteger aCustomElements;

    Prs3d_Root::NewGroup ( Prs );
    Handle ( Graphic3d_Group ) aGGroup = Prs3d_Root::CurrentGroup ( Prs );
    Prs3d_Root::NewGroup ( Prs );
    Handle ( Graphic3d_Group ) aLGroup = Prs3d_Root::CurrentGroup ( Prs );

    Handle (Graphic3d_ArrayOfPolygons) aPolyGArr = new Graphic3d_ArrayOfPolygons
      ( aMaxFaceNodes*aSize + PolygonVerticesFor3D, aSize + PolygonBoundsFor3D, 0, IsReflect );
    Standard_Boolean IsPolyG = Standard_False;

    Handle (Graphic3d_ArrayOfPolylines) aPolyLArr = new Graphic3d_ArrayOfPolylines
      ( 2*aSize, aSize );
    Standard_Boolean IsPolyL = Standard_False;

    // OCC20644 NOTE: aColIter.Key() color is then scaled by TelUpdateMaterial() in OpenGl_attri.c
    // using the material reflection coefficients. This affects the visual result.
    Handle(Graphic3d_AspectFillArea3d) anAsp =
      new Graphic3d_AspectFillArea3d ( Aspect_IS_SOLID, aColIter.Key(), anEdgeColor,
                                       anEdgeType, anEdgeWidth, aMaterial[0], aMaterial[1] );

    Handle(Graphic3d_AspectLine3d) anLAsp =
      new Graphic3d_AspectLine3d ( aColIter.Key(), aLineType, aLineWidth );

    anAsp->SetDistinguishOff ();
    anAsp->SetInteriorColor ( aColIter.Key() );
    if (anEdgeOn)
      anAsp->SetEdgeOn();
    else
      anAsp->SetEdgeOff();

    for( it.Reset(); it.More(); it.Next() )
    {
      Standard_Integer aKey = it.Key();
      if( aColIter.Value().Contains( aKey ) )
      {
        if ( !aSource->GetGeom  ( aKey, Standard_True, aCoords, NbNodes, aType ) )
          continue;

        if( aType == MeshVS_ET_Face )
        {
          aPolyGArr->AddBound ( NbNodes );
          if( IsExcludingOn() )
            IDsToExclude.Add( aKey );
        }
        else if( aType == MeshVS_ET_Link )
        {
          aPolyLArr->AddBound ( NbNodes );
          if( IsExcludingOn() )
            IDsToExclude.Add( aKey );
        }
        else if( aType == MeshVS_ET_Volume )
        {
          if( IsExcludingOn() )
            IDsToExclude.Add( aKey );
          if( aSource->Get3DGeom( aKey, NbNodes, aTopo ) )
          {
            MeshVS_MeshPrsBuilder::AddVolumePrs( aTopo, aCoords, NbNodes, aPolyGArr, IsReflect, Standard_False, Standard_False, 1.0 );
            IsPolyG = Standard_True;
          }
          else
            continue;
        }
        else
        {
          aCustomElements.Add( aKey );
          continue;
        }

        // Preparing normal(s) to show reflections if requested
        Handle(TColStd_HArray1OfReal) aNormals;
        Standard_Boolean hasNormals = IsReflect && aSource->GetNormalsByElement( aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals );

        // Adding vertices (with normals if necessary)
        for ( i=1; i<=NbNodes; i++ )
          if ( aType == MeshVS_ET_Face )
          {
            if ( IsReflect )
            {
              hasNormals ? aPolyGArr->AddVertex ( aCoords(3 * i - 2), 
                                                  aCoords(3 * i - 1), 
                                                  aCoords(3 * i    ),
                                                  aNormals->Value(3 * i - 2), 
                                                  aNormals->Value(3 * i - 1), 
                                                  aNormals->Value(3 * i    ) ) :
                           aPolyGArr->AddVertex ( aCoords(3 * i - 2), 
                                                  aCoords(3 * i - 1), 
                                                  aCoords(3 * i    ),
                                                  0., 
                                                  0., 
                                                  1. );
            }
            else
              aPolyGArr->AddVertex ( aCoords(3 * i - 2), 
                                     aCoords(3 * i - 1), 
                                     aCoords(3 * i    ) );
            IsPolyG = Standard_True;
          }
          else if ( aType == MeshVS_ET_Link )
          {
            aPolyLArr->AddVertex ( aCoords(3*i-2), aCoords(3*i-1), aCoords(3*i) );
            IsPolyL = Standard_True;
          }
      }
    }

    if ( IsPolyG )
    {
      aGGroup->SetPrimitivesAspect ( anAsp );
      aGGroup->BeginPrimitives();
      aGGroup->AddPrimitiveArray ( aPolyGArr );
      aGGroup->EndPrimitives();
    }
    if ( IsPolyL )
    {
      anAsp->SetEdgeOff();
      aLGroup->SetPrimitivesAspect ( anAsp );
      aLGroup->SetPrimitivesAspect ( anLAsp );
      aLGroup->BeginPrimitives();
      aLGroup->AddPrimitiveArray ( aPolyLArr );
      aLGroup->EndPrimitives();
      if (anEdgeOn)
        anAsp->SetEdgeOn();
      else
        anAsp->SetEdgeOff();
    }

    if( !aCustomElements.IsEmpty() )
      CustomBuild( Prs, aCustomElements, IDsToExclude, DisplayMode );
  }

  Graphic3d_MaterialAspect aMaterial2[2];
  for ( i=0; i<2; i++ )
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
  for ( MeshVS_DataMapIteratorOfDataMapOfTwoColorsMapOfInteger aColIter2 ( aTwoColorsOfElements );
        aColIter2.More(); aColIter2.Next() )
  {
    Prs3d_Root::NewGroup ( Prs );
    Handle ( Graphic3d_Group ) aGroup2 = Prs3d_Root::CurrentGroup ( Prs );

    Standard_Integer aSize = aColIter2.Value().Extent();
    if ( aSize<=0 )
      continue;

#ifdef _POLYGONES_
    Handle (Graphic3d_ArrayOfPolygons) aPolyArr = new Graphic3d_ArrayOfPolygons
      ( aMaxFaceNodes*aSize, aSize, 0, IsReflect );
#endif

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
    if (anEdgeOn)
      anAsp->SetEdgeOn();
    else
      anAsp->SetEdgeOff();

    aGroup2->SetPrimitivesAspect ( anAsp );
    aGroup2->BeginPrimitives();

    for( it.Reset(); it.More(); it.Next() )
    {
      Standard_Integer aKey = it.Key();
      if( aColIter2.Value().Contains( aKey ) )
      {
        if ( !aSource->GetGeom ( aKey, Standard_True, aCoords, NbNodes, aType ) )
          continue;

        if( IsExcludingOn() )
          IDsToExclude.Add( aKey );

        if ( aType == MeshVS_ET_Face && NbNodes > 0 )
        {
          // Preparing normal(s) to show reflections if requested
          Handle(TColStd_HArray1OfReal) aNormals;
          // OCC21720 Always passing normals to OpenGL to make materials work
          // For OpenGL: "No normals" -> "No lighting" -> "no materials taken into account"
          Standard_Boolean hasNormals = /*IsReflect &&*/
            aSource->GetNormalsByElement( aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals );

#ifdef _POLYGONES_
          aPolyArr->AddBound ( NbNodes );
#else
          Graphic3d_Array1OfVertex aVertArr ( 1, NbNodes );
#endif

          for ( i=1; i<=NbNodes; i++ )
          {
#ifdef _POLYGONES_
            if ( IsReflect )
            {
              hasNormals ? aPolyArr->AddVertex ( aCoords(3 * i - 2), 
                                                 aCoords(3 * i - 1), 
                                                 aCoords(3 * i    ),
                                                 aNormals->Value(3 * i - 2), 
                                                 aNormals->Value(3 * i - 1), 
                                                 aNormals->Value(3 * i    ) ) :
                           aPolyArr->AddVertex ( aCoords(3 * i - 2), 
                                                 aCoords(3 * i - 1), 
                                                 aCoords(3 * i    ),
                                                 0., 
                                                 0., 
                                                 1. );
            }
            else
              aPolyArr->AddVertex ( aCoords(3*i-2), 
                                    aCoords(3*i-1), 
                                    aCoords(3*i  ) );
#else
            if ( IsReflect )
            {
              aVertArr (i) = hasNormals ? Graphic3d_VertexN( aCoords(3 * i - 2), 
                                                             aCoords(3 * i - 1), 
                                                             aCoords(3 * i    ),
                                                             aNormals->Value(3 * i - 2), 
                                                             aNormals->Value(3 * i - 1), 
                                                             aNormals->Value(3 * i    ),
                                                             Standard_False ) :
                                            Graphic3d_VertexN( aCoords(3 * i - 2), 
                                                             aCoords(3 * i - 1), 
                                                             aCoords(3 * i    ),
                                                             0., 
                                                             0., 
                                                             1.,
                                                             Standard_False );
										 
            }
            else
              Graphic3d_Vertex ( aCoords(3 * i - 2), 
                                 aCoords(3 * i - 1), 
                                 aCoords(3 * i    ) );
#endif
          }


#ifndef _POLYGONES_
          aGroup2->Polygon ( aVertArr );
#endif
        }
      }
    }
#ifdef _POLYGONES_
    aGroup2->AddPrimitiveArray ( aPolyArr );
#endif
    aGroup2->EndPrimitives();
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
