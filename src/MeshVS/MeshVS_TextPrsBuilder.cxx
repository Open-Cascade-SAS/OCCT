// Created on: 2003-09-22
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


#include <MeshVS_TextPrsBuilder.ixx>

#include <MeshVS_DisplayModeFlags.hxx>

#include <Graphic3d_Group.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Buffer.hxx>

#include <Graphic3d_NameOfFont.hxx>

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

  Prs3d_Root::NewGroup ( Prs );
  Handle (Graphic3d_Group) aTextGroup = Prs3d_Root::CurrentGroup ( Prs );

  Quantity_Color           AColor           = Quantity_NOC_YELLOW;
#ifdef WNT  
  Standard_CString         AFont            = "Courier New";
#else
  Standard_CString         AFont            = "Courier";
#endif
  Standard_Real            AExpansionFactor = 1.0;
  Standard_Real            ASpace           = 0.0;
  Aspect_TypeOfStyleText   ATextStyle       = Aspect_TOST_ANNOTATION;
  Standard_Integer         AStyleInt;
  Aspect_TypeOfDisplayText ADisplayType     = Aspect_TODT_NORMAL;
  TCollection_AsciiString  AFontString;
  Standard_Integer         ADispInt;
  // Bold font is used by default for better text readability
  Font_FontAspect           AFontAspectType  = Font_FA_Bold;
  Standard_Integer         AAspect; 
  

  aDrawer->GetColor  ( MeshVS_DA_TextColor, AColor );
  aDrawer->GetDouble ( MeshVS_DA_TextExpansionFactor, AExpansionFactor );
  aDrawer->GetDouble ( MeshVS_DA_TextSpace, ASpace );

  if ( aDrawer->GetAsciiString ( MeshVS_DA_TextFont, AFontString ) )
    AFont = AFontString.ToCString();
  if ( aDrawer->GetInteger ( MeshVS_DA_TextStyle, AStyleInt ) )
    ATextStyle = (Aspect_TypeOfStyleText) AStyleInt;
  if ( aDrawer->GetInteger ( MeshVS_DA_TextDisplayType, ADispInt ) )
    ADisplayType = (Aspect_TypeOfDisplayText) ADispInt;
  if ( aDrawer->GetInteger ( MeshVS_DA_TextFontAspect, AAspect ) )
    AFontAspectType = (Font_FontAspect)AAspect;         

  Handle (Graphic3d_AspectText3d) aTextAspect = new Graphic3d_AspectText3d ( AColor, AFont, AExpansionFactor, ASpace,
    ATextStyle, ADisplayType );
  aTextAspect->SetTextFontAspect( AFontAspectType );
  Handle (Graphic3d_AspectMarker3d) anAspectMarker3d =
    new Graphic3d_AspectMarker3d( Aspect_TOM_POINT, Quantity_NOC_GRAY, 1. );
  aTextGroup->SetPrimitivesAspect( aTextAspect );
  aTextGroup->SetPrimitivesAspect( anAspectMarker3d );

  aTextGroup->BeginPrimitives();

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

  TColStd_MapIteratorOfPackedMapOfInteger it (anIDs);
  for( ; it.More(); it.Next() )
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

        Graphic3d_Vertex aPoint( X, Y, Z );

        aTextGroup->Marker ( aPoint );
        aTextGroup->Text ( aStr.ToCString(), aPoint, aHeight );
      }
    }
  }

  aTextGroup->EndPrimitives();

  if (!aCustomElements.IsEmpty())
    CustomBuild ( Prs, aCustomElements, IDsToExclude, theDisplayMode );
}
