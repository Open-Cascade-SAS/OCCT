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
#include <NCollection_Array1.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <NCollection_PackedMapAlgo.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_TextPrsBuilder, MeshVS_PrsBuilder)

//=================================================================================================

MeshVS_TextPrsBuilder::MeshVS_TextPrsBuilder(const occ::handle<MeshVS_Mesh>&       Parent,
                                             const double                          Height,
                                             const Quantity_Color&                 Color,
                                             const MeshVS_DisplayModeFlags&        Flags,
                                             const occ::handle<MeshVS_DataSource>& DS,
                                             const int                             Id,
                                             const MeshVS_BuilderPriority&         Priority)
    : MeshVS_PrsBuilder(Parent, Flags, DS, Id, Priority)
{
  occ::handle<MeshVS_Drawer> aDrawer = GetDrawer();
  if (!aDrawer.IsNull())
  {
    aDrawer->SetDouble(MeshVS_DA_TextHeight, Height);
    aDrawer->SetColor(MeshVS_DA_TextColor, Color);
  }
}

//=================================================================================================

const NCollection_DataMap<int, TCollection_AsciiString>& MeshVS_TextPrsBuilder::GetTexts(
  const bool IsElements) const
{
  if (IsElements)
    return myElemTextMap;
  else
    return myNodeTextMap;
}

//=================================================================================================

void MeshVS_TextPrsBuilder::SetTexts(const bool IsElements,
                                     const NCollection_DataMap<int, TCollection_AsciiString>& Map)
{
  if (IsElements)
    myElemTextMap = Map;
  else
    myNodeTextMap = Map;
}

//=================================================================================================

bool MeshVS_TextPrsBuilder::HasTexts(const bool IsElement) const
{
  bool aRes = (myNodeTextMap.Extent() > 0);
  if (IsElement)
    aRes = (myElemTextMap.Extent() > 0);
  return aRes;
}

//=================================================================================================

bool MeshVS_TextPrsBuilder::GetText(const bool               IsElement,
                                    const int                theID,
                                    TCollection_AsciiString& theStr) const
{
  const NCollection_DataMap<int, TCollection_AsciiString>* aMap = &myNodeTextMap;
  if (IsElement)
    aMap = &myElemTextMap;

  bool aRes = aMap->IsBound(theID);
  if (aRes)
    theStr = aMap->Find(theID);

  return aRes;
}

//=================================================================================================

void MeshVS_TextPrsBuilder::SetText(const bool                     IsElement,
                                    const int                      ID,
                                    const TCollection_AsciiString& Text)
{
  NCollection_DataMap<int, TCollection_AsciiString>* aMap = &myNodeTextMap;
  if (IsElement)
    aMap = &myElemTextMap;

  bool aRes = aMap->IsBound(ID);
  if (aRes)
    aMap->ChangeFind(ID) = Text;
  else
    aMap->Bind(ID, Text);
}

//=================================================================================================

void MeshVS_TextPrsBuilder::Build(const occ::handle<Prs3d_Presentation>& Prs,
                                  const TColStd_PackedMapOfInteger&      IDs,
                                  TColStd_PackedMapOfInteger&            IDsToExclude,
                                  const bool                             IsElement,
                                  const int                              theDisplayMode) const
{
  occ::handle<MeshVS_DataSource> aSource = GetDataSource();
  occ::handle<MeshVS_Drawer>     aDrawer = GetDrawer();
  if (aSource.IsNull() || aDrawer.IsNull() || !HasTexts(IsElement)
      || (theDisplayMode & GetFlags()) == 0)
    return;

  int    aMaxFaceNodes;
  double aHeight;
  if (!aDrawer->GetInteger(MeshVS_DA_MaxFaceNodes, aMaxFaceNodes) || aMaxFaceNodes <= 0
      || !aDrawer->GetDouble(MeshVS_DA_TextHeight, aHeight))
    return;

  occ::handle<Graphic3d_Group> aTextGroup = Prs->NewGroup();

  Quantity_Color           AColor           = Quantity_NOC_YELLOW;
  const char*              AFont            = Font_NOF_ASCII_MONO;
  double                   AExpansionFactor = 1.0;
  double                   ASpace           = 0.0;
  Aspect_TypeOfStyleText   ATextStyle       = Aspect_TOST_ANNOTATION;
  Aspect_TypeOfDisplayText ADisplayType     = Aspect_TODT_NORMAL;
  // Bold font is used by default for better text readability
  Font_FontAspect AFontAspectType = Font_FA_Bold;

  aDrawer->GetColor(MeshVS_DA_TextColor, AColor);
  aDrawer->GetDouble(MeshVS_DA_TextExpansionFactor, AExpansionFactor);
  aDrawer->GetDouble(MeshVS_DA_TextSpace, ASpace);

  TCollection_AsciiString AFontString = Font_NOF_ASCII_MONO;
  if (aDrawer->GetAsciiString(MeshVS_DA_TextFont, AFontString))
    AFont = AFontString.ToCString();

  int AStyleInt = Aspect_TOST_ANNOTATION;
  if (aDrawer->GetInteger(MeshVS_DA_TextStyle, AStyleInt))
    ATextStyle = (Aspect_TypeOfStyleText)AStyleInt;

  int ADispInt = Aspect_TODT_NORMAL;
  if (aDrawer->GetInteger(MeshVS_DA_TextDisplayType, ADispInt))
    ADisplayType = (Aspect_TypeOfDisplayText)ADispInt;

  int AAspect = Font_FA_Bold;
  if (aDrawer->GetInteger(MeshVS_DA_TextFontAspect, AAspect))
    AFontAspectType = (Font_FontAspect)AAspect;

  occ::handle<Graphic3d_AspectText3d> aTextAspect =
    new Graphic3d_AspectText3d(AColor, AFont, AExpansionFactor, ASpace, ATextStyle, ADisplayType);
  aTextAspect->SetTextFontAspect(AFontAspectType);
  aTextGroup->SetGroupPrimitivesAspect(aTextAspect);

  MeshVS_Buffer              aCoordsBuf(3 * aMaxFaceNodes * sizeof(double));
  NCollection_Array1<double> aCoords(aCoordsBuf, 1, 3 * aMaxFaceNodes);
  int                        NbNodes;
  TCollection_AsciiString    aStr;
  MeshVS_EntityType          aType;
  TColStd_PackedMapOfInteger aCustomElements;

  double X, Y, Z;

  // subtract the hidden elements and ids to exclude (to minimise allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign(IDs);
  if (IsElement)
  {
    occ::handle<TColStd_HPackedMapOfInteger> aHiddenElems = myParentMesh->GetHiddenElems();
    if (!aHiddenElems.IsNull())
      NCollection_PackedMapAlgo::Subtract(anIDs, aHiddenElems->Map());
  }
  NCollection_PackedMapAlgo::Subtract(anIDs, IDsToExclude);

  NCollection_Sequence<NCollection_Vec3<float>> aPnts;
  for (TColStd_PackedMapOfInteger::Iterator it(anIDs); it.More(); it.Next())
  {
    int aKey = it.Key();
    if (GetText(IsElement, aKey, aStr))
    {
      if (aSource->GetGeom(aKey, IsElement, aCoords, NbNodes, aType))
      {
        if (aType == MeshVS_ET_Node)
        {
          X = aCoords(1);
          Y = aCoords(2);
          Z = aCoords(3);
        }
        else if (aType == MeshVS_ET_Link || aType == MeshVS_ET_Face || aType == MeshVS_ET_Volume)
        {
          if (IsElement && IsExcludingOn())
            IDsToExclude.Add(aKey);
          X = Y = Z = 0;
          for (int i = 1; i <= NbNodes; i++)
          {
            X += aCoords(3 * i - 2);
            Y += aCoords(3 * i - 1);
            Z += aCoords(3 * i);
          }
          X /= double(NbNodes);
          Y /= double(NbNodes);
          Z /= double(NbNodes);
        }
        else
        {
          aCustomElements.Add(aKey);
          continue;
        }

        aPnts.Append(NCollection_Vec3<float>((float)X, (float)Y, (float)Z));

        occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)aHeight);
        aText->SetText(aStr);
        aText->SetPosition(gp_Pnt(X, Y, Z));
        aTextGroup->AddText(aText);
      }
    }
  }

  if (!aPnts.IsEmpty())
  {
    occ::handle<Graphic3d_Group>         aMarkerGroup = Prs->NewGroup();
    occ::handle<Graphic3d_ArrayOfPoints> anArrayOfPoints =
      new Graphic3d_ArrayOfPoints(aPnts.Size());
    for (NCollection_Sequence<NCollection_Vec3<float>>::Iterator aPntIter(aPnts); aPntIter.More();
         aPntIter.Next())
    {
      const NCollection_Vec3<float>& aPnt = aPntIter.Value();
      anArrayOfPoints->AddVertex(aPnt.x(), aPnt.y(), aPnt.z());
    }
    occ::handle<Graphic3d_AspectMarker3d> anAspectMarker3d =
      new Graphic3d_AspectMarker3d(Aspect_TOM_POINT, Quantity_NOC_GRAY, 1.0);
    aMarkerGroup->SetGroupPrimitivesAspect(anAspectMarker3d);
    aMarkerGroup->AddPrimitiveArray(anArrayOfPoints);
  }

  if (!aCustomElements.IsEmpty())
    CustomBuild(Prs, aCustomElements, IDsToExclude, theDisplayMode);
}
