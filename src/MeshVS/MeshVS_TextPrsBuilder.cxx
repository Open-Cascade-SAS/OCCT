// Created on: 2003-09-22
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


#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Text.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_TextPrsBuilder.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_TextPrsBuilder,MeshVS_PrsBuilder)

//================================================================
// Function : Constructor MeshVS_TextPrsBuilder
// Purpose  :
//================================================================
MeshVS_TextPrsBuilder::MeshVS_TextPrsBuilder (  const Handle(MeshVS_Mesh)& Parent,
                                                const Standard_Real Height,
                                                const Quantity_Color& Color,
                                                const MeshVS_DisplayModeFlags& Flags,
                                                const Handle (MeshVS_DataSource)& DS,
                                                const Standard_Integer Id,
                                                const MeshVS_BuilderPriority& Priority )
                                              : MeshVS_PrsBuilder ( Parent, Flags, DS, Id, Priority )
{
  Handle ( MeshVS_Drawer ) aDrawer = GetDrawer();
  if ( !aDrawer.IsNull() )
  {
    aDrawer->SetDouble ( MeshVS_DA_TextHeight, Height );
    aDrawer->SetColor  ( MeshVS_DA_TextColor, Color );
  }
}

//================================================================
// Function : GetTexts
// Purpose  :
//================================================================
const MeshVS_DataMapOfIntegerAsciiString& MeshVS_TextPrsBuilder::GetTexts ( const Standard_Boolean IsElements ) const
{
  if ( IsElements )
    return myElemTextMap;
  else
    return myNodeTextMap;
}

//================================================================
// Function : SetTexts
// Purpose  :
//================================================================
void MeshVS_TextPrsBuilder::SetTexts ( const Standard_Boolean IsElements,
                                       const MeshVS_DataMapOfIntegerAsciiString& Map )
{
  if ( IsElements )
    myElemTextMap = Map;
  else
    myNodeTextMap = Map;
}

//================================================================
// Function : HasTexts
// Purpose  :
//================================================================
Standard_Boolean MeshVS_TextPrsBuilder::HasTexts ( const Standard_Boolean IsElement ) const
{
  Standard_Boolean aRes = (myNodeTextMap.Extent()>0);
  if ( IsElement )
    aRes = (myElemTextMap.Extent()>0);
  return aRes;
}

//================================================================
// Function : GetText
// Purpose  :
//================================================================
Standard_Boolean MeshVS_TextPrsBuilder::GetText ( const Standard_Boolean IsElement,
                                                  const Standard_Integer theID,
                                                  TCollection_AsciiString& theStr ) const
{
  const MeshVS_DataMapOfIntegerAsciiString* aMap = &myNodeTextMap;
  if ( IsElement )
    aMap = &myElemTextMap;

  Standard_Boolean aRes = aMap->IsBound ( theID );
  if ( aRes )
    theStr = aMap->Find ( theID );

  return aRes;
}

//================================================================
// Function : SetText
// Purpose  :
//================================================================
void MeshVS_TextPrsBuilder::SetText ( const Standard_Boolean IsElement,
                                      const Standard_Integer ID,
                                      const TCollection_AsciiString& Text )
{
  MeshVS_DataMapOfIntegerAsciiString* aMap = &myNodeTextMap;
  if ( IsElement )
    aMap = &myElemTextMap;

  Standard_Boolean aRes = aMap->IsBound ( ID );
  if ( aRes )
    aMap->ChangeFind ( ID ) = Text;
  else
    aMap->Bind ( ID, Text );
}

//================================================================
// Function : Build
// Purpose  :
//================================================================
void MeshVS_TextPrsBuilder::Build ( const Handle(Prs3d_Presentation)& Prs,
                                    const TColStd_PackedMapOfInteger& IDs,
                                    TColStd_PackedMapOfInteger& IDsToExclude,
                                    const Standard_Boolean IsElement,
                                    const Standard_Integer theDisplayMode ) const
{
  Handle (MeshVS_DataSource) aSource = GetDataSource();
  Handle (MeshVS_Drawer) aDrawer = GetDrawer();
  if ( aSource.IsNull() || aDrawer.IsNull() || !HasTexts( IsElement ) ||
    ( theDisplayMode & GetFlags() ) == 0 )
    return;

  Standard_Integer aMaxFaceNodes;
  Standard_Real aHeight;
  if ( !aDrawer->GetInteger ( MeshVS_DA_MaxFaceNodes, aMaxFaceNodes ) ||
    aMaxFaceNodes <= 0 ||
    !aDrawer->GetDouble  ( MeshVS_DA_TextHeight, aHeight )    )
    return;

  Handle(Graphic3d_Group) aTextGroup = Prs->NewGroup();

  Quantity_Color           AColor           = Quantity_NOC_YELLOW;
  Standard_CString         AFont            = Font_NOF_ASCII_MONO;
  Standard_Real            AExpansionFactor = 1.0;
  Standard_Real            ASpace           = 0.0;
  Aspect_TypeOfStyleText   ATextStyle       = Aspect_TOST_ANNOTATION;
  Aspect_TypeOfDisplayText ADisplayType     = Aspect_TODT_NORMAL;
  // Bold font is used by default for better text readability
  Font_FontAspect           AFontAspectType  = Font_FA_Bold;

  aDrawer->GetColor  ( MeshVS_DA_TextColor, AColor );
  aDrawer->GetDouble ( MeshVS_DA_TextExpansionFactor, AExpansionFactor );
  aDrawer->GetDouble ( MeshVS_DA_TextSpace, ASpace );

  TCollection_AsciiString AFontString = Font_NOF_ASCII_MONO;
  if ( aDrawer->GetAsciiString ( MeshVS_DA_TextFont, AFontString ) )
    AFont = AFontString.ToCString();

  Standard_Integer AStyleInt = Aspect_TOST_ANNOTATION;
  if ( aDrawer->GetInteger ( MeshVS_DA_TextStyle, AStyleInt ) )
    ATextStyle = (Aspect_TypeOfStyleText) AStyleInt;

  Standard_Integer ADispInt = Aspect_TODT_NORMAL;
  if ( aDrawer->GetInteger ( MeshVS_DA_TextDisplayType, ADispInt ) )
    ADisplayType = (Aspect_TypeOfDisplayText) ADispInt;

  Standard_Integer AAspect = Font_FA_Bold;
  if ( aDrawer->GetInteger ( MeshVS_DA_TextFontAspect, AAspect ) )
    AFontAspectType = (Font_FontAspect)AAspect;

  Handle (Graphic3d_AspectText3d) aTextAspect = new Graphic3d_AspectText3d ( AColor, AFont, AExpansionFactor, ASpace,
    ATextStyle, ADisplayType );
  aTextAspect->SetTextFontAspect( AFontAspectType );
  aTextGroup->SetGroupPrimitivesAspect( aTextAspect );

  MeshVS_Buffer aCoordsBuf (3*aMaxFaceNodes*sizeof(Standard_Real));
  TColStd_Array1OfReal aCoords (aCoordsBuf, 1, 3*aMaxFaceNodes);
  Standard_Integer NbNodes;
  TCollection_AsciiString aStr;
  MeshVS_EntityType aType;
  TColStd_PackedMapOfInteger aCustomElements;

  Standard_Real X, Y, Z;

  // subtract the hidden elements and ids to exclude (to minimise allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign( IDs );
  if ( IsElement )
  {
    Handle(TColStd_HPackedMapOfInteger) aHiddenElems = myParentMesh->GetHiddenElems();
    if ( !aHiddenElems.IsNull() )
      anIDs.Subtract( aHiddenElems->Map() );
  }
  anIDs.Subtract( IDsToExclude );

  NCollection_Sequence<Graphic3d_Vec3> aPnts;
  for (TColStd_MapIteratorOfPackedMapOfInteger it (anIDs); it.More(); it.Next())
  {
    Standard_Integer aKey = it.Key();
    if( GetText ( IsElement, aKey, aStr ) )
    {
      if( aSource->GetGeom ( aKey, IsElement, aCoords, NbNodes, aType ) )
      {
        if( aType == MeshVS_ET_Node )
        {
          X = aCoords(1);
          Y = aCoords(2);
          Z = aCoords(3);
        }
        else if( aType == MeshVS_ET_Link || 
          aType == MeshVS_ET_Face || 
          aType == MeshVS_ET_Volume )
        {
          if( IsElement && IsExcludingOn() )
            IDsToExclude.Add( aKey );
          X = Y = Z = 0;
          for ( Standard_Integer i=1; i<=NbNodes; i++ )
          {
            X += aCoords (3*i-2);
            Y += aCoords (3*i-1);
            Z += aCoords (3*i);
          }
          X /= Standard_Real ( NbNodes );
          Y /= Standard_Real ( NbNodes );
          Z /= Standard_Real ( NbNodes );
        }
        else 
        {
          aCustomElements.Add( aKey );
          continue;
        }

        aPnts.Append (Graphic3d_Vec3 ((float )X, (float )Y, (float )Z));

        Handle(Graphic3d_Text) aText = new Graphic3d_Text ((Standard_ShortReal)aHeight);
        aText->SetText (aStr);
        aText->SetPosition (gp_Pnt (X, Y, Z));
        aTextGroup->AddText(aText);
      }
    }
  }

  if (!aPnts.IsEmpty())
  {
    Handle(Graphic3d_Group) aMarkerGroup = Prs->NewGroup();
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (aPnts.Size());
    for (NCollection_Sequence<Graphic3d_Vec3>::Iterator aPntIter (aPnts); aPntIter.More(); aPntIter.Next())
    {
      const Graphic3d_Vec3& aPnt = aPntIter.Value();
      anArrayOfPoints->AddVertex (aPnt.x(), aPnt.y(), aPnt.z());
    }
    Handle (Graphic3d_AspectMarker3d) anAspectMarker3d = new Graphic3d_AspectMarker3d (Aspect_TOM_POINT, Quantity_NOC_GRAY, 1.0);
    aMarkerGroup->SetGroupPrimitivesAspect (anAspectMarker3d);
    aMarkerGroup->AddPrimitiveArray (anArrayOfPoints);
  }

  if (!aCustomElements.IsEmpty())
    CustomBuild ( Prs, aCustomElements, IDsToExclude, theDisplayMode );
}
