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

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <MeshVS_Buffer.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <MeshVS_TwoColors.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_ElementalColorPrsBuilder.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshPrsBuilder.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <NCollection_List.hxx>
#include <NCollection_PackedMapAlgo.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_ElementalColorPrsBuilder, MeshVS_PrsBuilder)

//=================================================================================================

MeshVS_ElementalColorPrsBuilder::MeshVS_ElementalColorPrsBuilder(
  const occ::handle<MeshVS_Mesh>&       Parent,
  const MeshVS_DisplayModeFlags&        Flags,
  const occ::handle<MeshVS_DataSource>& DS,
  const int                             Id,
  const MeshVS_BuilderPriority&         Priority)
    : MeshVS_PrsBuilder(Parent, Flags, DS, Id, Priority)
{
  SetExcluding(true);
}

//=================================================================================================

void MeshVS_ElementalColorPrsBuilder::Build(const occ::handle<Prs3d_Presentation>& Prs,
                                            const TColStd_PackedMapOfInteger&      IDs,
                                            TColStd_PackedMapOfInteger&            IDsToExclude,
                                            const bool                             IsElement,
                                            const int DisplayMode) const
{
  occ::handle<MeshVS_DataSource> aSource = GetDataSource();
  occ::handle<MeshVS_Drawer>     aDrawer = GetDrawer();

  if (aSource.IsNull() || aDrawer.IsNull())
    return;

  int aMaxFaceNodes;
  if (!aDrawer->GetInteger(MeshVS_DA_MaxFaceNodes, aMaxFaceNodes) && aMaxFaceNodes <= 0)
    return;

  NCollection_DataMap<int, Quantity_Color>* anElemColorMap =
    (NCollection_DataMap<int, Quantity_Color>*)&myElemColorMap1;
  NCollection_DataMap<int, MeshVS_TwoColors>* anElemTwoColorsMap =
    (NCollection_DataMap<int, MeshVS_TwoColors>*)&myElemColorMap2;

  NCollection_DataMap<Quantity_Color, NCollection_Map<int>>   aColorsOfElements;
  NCollection_DataMap<MeshVS_TwoColors, NCollection_Map<int>> aTwoColorsOfElements;

  MeshVS_Buffer              aCoordsBuf(3 * aMaxFaceNodes * sizeof(double));
  NCollection_Array1<double> aCoords(aCoordsBuf, 1, 3 * aMaxFaceNodes);
  int                        NbNodes;
  MeshVS_EntityType          aType;

  if (!(DisplayMode & GetFlags()) || !IsElement
      || (myElemColorMap1.IsEmpty() && myElemColorMap2.IsEmpty()))
    return;

  // subtract the hidden elements and ids to exclude (to minimise allocated memory)
  TColStd_PackedMapOfInteger anIDs;
  anIDs.Assign(IDs);
  occ::handle<TColStd_HPackedMapOfInteger> aHiddenElems = myParentMesh->GetHiddenElems();
  if (!aHiddenElems.IsNull())
    NCollection_PackedMapAlgo::Subtract(anIDs, aHiddenElems->Map());
  NCollection_PackedMapAlgo::Subtract(anIDs, IDsToExclude);

  // STEP 0: We looking for two colored elements, who has equal two colors and move it
  // to map of elements with one assigned color
  NCollection_List<int> aColorOne;
  for (NCollection_DataMap<int, MeshVS_TwoColors>::Iterator anIter(*anElemTwoColorsMap);
       anIter.More();
       anIter.Next())
  {
    int              aKey   = anIter.Key();
    MeshVS_TwoColors aValue = anIter.Value();
    Quantity_Color   aCol1, aCol2;
    ExtractColors(aValue, aCol1, aCol2);
    if (aCol1 == aCol2)
    {
      aColorOne.Append(aKey);
      anElemColorMap->Bind(aKey, aCol1);
    }
  }

  for (NCollection_List<int>::Iterator aLIter(aColorOne); aLIter.More(); aLIter.Next())
    anElemTwoColorsMap->UnBind(aLIter.Value());

  // The map is to resort itself by colors.
  // STEP 1: We start sorting elements with one assigned color
  for (NCollection_DataMap<int, Quantity_Color>::Iterator anIterM(*anElemColorMap); anIterM.More();
       anIterM.Next())
  {
    int aMKey = anIterM.Key();
    // The ID of current element
    bool IsExist = false;
    for (NCollection_DataMap<Quantity_Color, NCollection_Map<int>>::Iterator anIterC(
           aColorsOfElements);
         anIterC.More() && !IsExist;
         anIterC.Next())
      if (anIterC.Key() == anIterM.Value())
      {
        NCollection_Map<int>& aChangeValue = (NCollection_Map<int>&)anIterC.Value();
        aChangeValue.Add(aMKey);
        IsExist = true;
      }

    if (!IsExist)
    {
      NCollection_Map<int> aNewMap;
      aNewMap.Add(aMKey);
      aColorsOfElements.Bind(anIterM.Value(), aNewMap);
    }
  }

  // STEP 2: We start sorting elements with two assigned colors
  for (NCollection_DataMap<int, MeshVS_TwoColors>::Iterator anIterM2(*anElemTwoColorsMap);
       anIterM2.More();
       anIterM2.Next())
  {
    int aMKey = anIterM2.Key();
    // The ID of current element
    bool IsExist = false;
    for (NCollection_DataMap<MeshVS_TwoColors, NCollection_Map<int>>::Iterator anIterC2(
           aTwoColorsOfElements);
         anIterC2.More() && !IsExist;
         anIterC2.Next())
      if (anIterC2.Key() == anIterM2.Value())
      {
        NCollection_Map<int>& aChangeValue = (NCollection_Map<int>&)anIterC2.Value();
        aChangeValue.Add(aMKey);
        IsExist = true;
      }

    if (!IsExist)
    {
      NCollection_Map<int> aNewMap;
      aNewMap.Add(aMKey);
      aTwoColorsOfElements.Bind(anIterM2.Value(), aNewMap);
    }
  }

  // Now we are ready to draw faces with equal colors
  Aspect_TypeOfLine anEdgeType  = Aspect_TOL_SOLID;
  Aspect_TypeOfLine aLineType   = Aspect_TOL_SOLID;
  double            anEdgeWidth = 1.0, aLineWidth = 1.0;
  Quantity_Color    anInteriorColor;
  Quantity_Color    anEdgeColor, aLineColor;
  bool              anEdgeOn = true, IsReflect = false, IsMeshSmoothShading = false;
  bool              toSupressBackFaces = false;

  aDrawer->GetColor(MeshVS_DA_InteriorColor, anInteriorColor);
  aDrawer->GetColor(MeshVS_DA_EdgeColor, anEdgeColor);
  aDrawer->GetColor(MeshVS_DA_BeamColor, aLineColor);
  aDrawer->GetDouble(MeshVS_DA_EdgeWidth, anEdgeWidth);
  aDrawer->GetDouble(MeshVS_DA_BeamWidth, aLineWidth);
  aDrawer->GetBoolean(MeshVS_DA_ShowEdges, anEdgeOn);
  aDrawer->GetBoolean(MeshVS_DA_ColorReflection, IsReflect);
  aDrawer->GetBoolean(MeshVS_DA_SmoothShading, IsMeshSmoothShading);
  aDrawer->GetBoolean(MeshVS_DA_SupressBackFaces, toSupressBackFaces);

  int anEdgeInt = Aspect_TOL_SOLID;
  if (aDrawer->GetInteger(MeshVS_DA_EdgeType, anEdgeInt))
    anEdgeType = (Aspect_TypeOfLine)anEdgeInt;

  int aLineInt = Aspect_TOL_SOLID;
  if (aDrawer->GetInteger(MeshVS_DA_BeamType, aLineInt))
    aLineType = (Aspect_TypeOfLine)aLineInt;

  occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> aTopo;
  int                                  PolygonVerticesFor3D = 0, PolygonBoundsFor3D = 0;
  TColStd_PackedMapOfInteger::Iterator it(anIDs);
  for (; it.More(); it.Next())
  {
    int aKey = it.Key();
    if (aSource->Get3DGeom(aKey, NbNodes, aTopo))
      MeshVS_MeshPrsBuilder::HowManyPrimitives(aTopo,
                                               true,
                                               false,
                                               NbNodes,
                                               PolygonVerticesFor3D,
                                               PolygonBoundsFor3D);
  }

  Graphic3d_MaterialAspect aMaterial[2] = {Graphic3d_NameOfMaterial_Plastified,
                                           Graphic3d_NameOfMaterial_Plastified};
  for (int i = 0; i < 2; i++)
  {
    // OCC20644 "plastic" is most suitable here, as it is "non-physic"
    // so TelUpdateMaterial() from OpenGl_attri.c uses the interior
    // color from AspectFillArea3d to calculate all material colors
    aMaterial[i].SetSpecularColor(Quantity_NOC_BLACK);
    aMaterial[i].SetEmissiveColor(Quantity_NOC_BLACK);

    // OCC21720 For single-colored elements turning all material components off is a good idea,
    // as anyhow the normals are not computed and the lighting will be off,
    // the element color will be taken from Graphic3d_AspectFillArea3d's interior color,
    // and there is no need to spend time on updating material properties
    if (IsReflect)
    {
      // OCC20644 This stuff is important in order for elemental and nodal colors
      // to produce similar visual impression and also to make colors match
      // those in the color scale most exactly (the sum of all reflection
      // coefficients is equal to 1). See also MeshVS_NodalColorPrsBuilder
      // class for more explanations.
      aMaterial[i].SetAmbientColor(Quantity_Color(NCollection_Vec3<float>(0.5f)));
      aMaterial[i].SetDiffuseColor(Quantity_Color(NCollection_Vec3<float>(0.5f)));
    }
  }

  // Draw elements with one color
  occ::handle<Graphic3d_Group> aGGroup, aGroup2, aLGroup, aSGroup;
  if (!aTwoColorsOfElements.IsEmpty())
  {
    aGroup2 = Prs->NewGroup();
  }
  if (!aColorsOfElements.IsEmpty())
  {
    occ::handle<Graphic3d_AspectFillArea3d> aGroupFillAspect =
      new Graphic3d_AspectFillArea3d(Aspect_IS_SOLID,
                                     anInteriorColor,
                                     anEdgeColor,
                                     anEdgeType,
                                     anEdgeWidth,
                                     aMaterial[0],
                                     aMaterial[1]);
    if (!IsReflect)
      aGroupFillAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);

    aGGroup = Prs->NewGroup();
    aLGroup = Prs->NewGroup();
    aGGroup->SetClosed(toSupressBackFaces);
    aGGroup->SetGroupPrimitivesAspect(aGroupFillAspect);
  }

  if (anEdgeOn)
  {
    occ::handle<Graphic3d_AspectLine3d> anEdgeAspect =
      new Graphic3d_AspectLine3d(anEdgeColor, anEdgeType, anEdgeWidth);
    aSGroup = Prs->NewGroup();
    aSGroup->SetGroupPrimitivesAspect(anEdgeAspect);
  }

  for (NCollection_DataMap<Quantity_Color, NCollection_Map<int>>::Iterator aColIter(
         aColorsOfElements);
       aColIter.More();
       aColIter.Next())
  {
    if (aColIter.Value().IsEmpty())
    {
      continue;
    }

    TColStd_PackedMapOfInteger aCustomElements;

    int aNbFacePrimitives = 0;
    int aNbVolmPrimitives = 0;
    int aNbEdgePrimitives = 0;
    int aNbLinkPrimitives = 0;

    for (it.Reset(); it.More(); it.Next())
    {
      int aNbNodes = 0;

      if (!aColIter.Value().Contains(it.Key()))
        continue;

      if (!aSource->GetGeom(it.Key(), true, aCoords, aNbNodes, aType))
        continue;

      if (aType == MeshVS_ET_Volume)
      {
        if (aSource->Get3DGeom(it.Key(), aNbNodes, aTopo))
        {
          for (int aFaceIdx = aTopo->Lower(); aFaceIdx <= aTopo->Upper(); ++aFaceIdx)
          {
            const NCollection_Sequence<int>& aFaceNodes = aTopo->Value(aFaceIdx);

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

    occ::handle<Graphic3d_ArrayOfTriangles> aFaceTriangles =
      new Graphic3d_ArrayOfTriangles((aNbFacePrimitives + aNbVolmPrimitives) * 3, 0, IsReflect);
    bool IsPolyG = false;

    occ::handle<Graphic3d_ArrayOfSegments> anEdgeSegments =
      new Graphic3d_ArrayOfSegments(aNbEdgePrimitives * 2);
    occ::handle<Graphic3d_ArrayOfSegments> aLinkSegments =
      new Graphic3d_ArrayOfSegments(aNbLinkPrimitives * 2);
    bool IsPolyL = false;

    // OCC20644 NOTE: aColIter.Key() color is then scaled by TelUpdateMaterial() in OpenGl_attri.c
    // using the material reflection coefficients. This affects the visual result.
    occ::handle<Graphic3d_AspectFillArea3d> aFillAspect =
      new Graphic3d_AspectFillArea3d(Aspect_IS_SOLID,
                                     aColIter.Key(),
                                     anEdgeColor,
                                     anEdgeType,
                                     anEdgeWidth,
                                     aMaterial[0],
                                     aMaterial[1]);

    occ::handle<Graphic3d_AspectLine3d> aLinkAspect =
      new Graphic3d_AspectLine3d(aColIter.Key(), aLineType, aLineWidth);

    aFillAspect->SetDistinguishOff();
    aFillAspect->SetInteriorColor(aColIter.Key());
    aFillAspect->SetEdgeOff();
    if (!IsReflect)
      aFillAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);

    for (it.Reset(); it.More(); it.Next())
    {
      int aKey = it.Key();

      if (aColIter.Value().Contains(aKey))
      {
        if (!aSource->GetGeom(aKey, true, aCoords, NbNodes, aType))
          continue;

        if (aType != MeshVS_ET_Face && aType != MeshVS_ET_Link && aType != MeshVS_ET_Volume)
        {
          aCustomElements.Add(aKey);
          continue;
        }

        if (IsExcludingOn())
          IDsToExclude.Add(aKey);

        if (aType == MeshVS_ET_Volume)
        {
          if (!aSource->Get3DGeom(aKey, NbNodes, aTopo))
          {
            continue;
          }

          MeshVS_MeshPrsBuilder::AddVolumePrs(aTopo,
                                              aCoords,
                                              NbNodes,
                                              aFaceTriangles,
                                              IsReflect,
                                              false,
                                              false,
                                              1.0);

          if (anEdgeOn)
          {
            MeshVS_MeshPrsBuilder::AddVolumePrs(aTopo,
                                                aCoords,
                                                NbNodes,
                                                anEdgeSegments,
                                                IsReflect,
                                                false,
                                                false,
                                                1.0);
          }

          IsPolyG = true;
        }
        else if (aType == MeshVS_ET_Face)
        {
          // Preparing normals
          occ::handle<NCollection_HArray1<double>> aNormals;
          bool                                     aHasNormals =
            IsReflect
            && aSource->GetNormalsByElement(aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals);

          for (int aNodeIdx = 0; aNodeIdx < NbNodes - 2; ++aNodeIdx)
          {
            for (int anIdx = 0; anIdx < 3; ++anIdx)
            {
              if (IsReflect)
              {
                aFaceTriangles->AddVertex(
                  aCoords(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 1),
                  aCoords(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 2),
                  aCoords(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 3),
                  aHasNormals ? aNormals->Value(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 1)
                              : 0.0,
                  aHasNormals ? aNormals->Value(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 2)
                              : 0.0,
                  aHasNormals ? aNormals->Value(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 3)
                              : 1.0);
              }
              else
              {
                aFaceTriangles->AddVertex(aCoords(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 1),
                                          aCoords(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 2),
                                          aCoords(3 * (anIdx == 0 ? 0 : (aNodeIdx + anIdx)) + 3));
              }
            }
          }

          if (anEdgeOn)
          {
            for (int aNodeIdx = 0; aNodeIdx < NbNodes; ++aNodeIdx)
            {
              const int aNextIdx = (aNodeIdx + 1) % NbNodes;

              anEdgeSegments->AddVertex(aCoords(3 * aNodeIdx + 1),
                                        aCoords(3 * aNodeIdx + 2),
                                        aCoords(3 * aNodeIdx + 3));

              anEdgeSegments->AddVertex(aCoords(3 * aNextIdx + 1),
                                        aCoords(3 * aNextIdx + 2),
                                        aCoords(3 * aNextIdx + 3));
            }
          }

          IsPolyG = true;
        }
        else if (aType == MeshVS_ET_Link)
        {
          for (int aNodeIdx = 0; aNodeIdx < NbNodes - 1; ++aNodeIdx)
          {
            const int aNextIdx = aNodeIdx + 1;

            aLinkSegments->AddVertex(aCoords(3 * aNodeIdx + 1),
                                     aCoords(3 * aNodeIdx + 2),
                                     aCoords(3 * aNodeIdx + 3));

            aLinkSegments->AddVertex(aCoords(3 * aNextIdx + 1),
                                     aCoords(3 * aNextIdx + 2),
                                     aCoords(3 * aNextIdx + 3));

            IsPolyL = true;
          }
        }
      }
    }

    if (IsPolyG)
    {
      aGGroup->SetPrimitivesAspect(aFillAspect);
      aGGroup->AddPrimitiveArray(aFaceTriangles);
      aGGroup->SetClosed(toSupressBackFaces);

      if (anEdgeOn)
      {
        aSGroup->AddPrimitiveArray(anEdgeSegments);
      }
    }
    if (IsPolyL)
    {
      aLGroup->SetPrimitivesAspect(aFillAspect);
      aLGroup->SetPrimitivesAspect(aLinkAspect);
      aLGroup->AddPrimitiveArray(aLinkSegments);
    }

    if (!aCustomElements.IsEmpty())
      CustomBuild(Prs, aCustomElements, IDsToExclude, DisplayMode);
  }

  Graphic3d_MaterialAspect aMaterial2[2] = {Graphic3d_NameOfMaterial_Plastified,
                                            Graphic3d_NameOfMaterial_Plastified};
  for (int i = 0; i < 2; i++)
  {
    // OCC20644 "plastic" is most suitable here, as it is "non-physic"
    // so TelUpdateMaterial() from OpenGl_attri.c uses the interior
    // color from AspectFillArea3d to calculate all material colors
    aMaterial2[i].SetSpecularColor(Quantity_NOC_BLACK);
    aMaterial2[i].SetEmissiveColor(Quantity_NOC_BLACK);

    if (!IsReflect)
    {
      // OCC21720 Cannot turn ALL material components off, as such a material
      // would be ignored by TelUpdateMaterial(), but we need it in order
      // to have different materials for front and back sides!
      // Instead, trying to make material color "nondirectional" with
      // only ambient component on.
      aMaterial2[i].SetAmbientColor(Quantity_Color(NCollection_Vec3<float>(1.0f)));
      aMaterial2[i].SetDiffuseColor(Quantity_NOC_BLACK);
    }
    else
    {
      // OCC20644 This stuff is important in order for elemental and nodal colors
      // to produce similar visual impression and also to make colors match
      // those in the color scale most exactly (the sum of all reflection
      // coefficients is equal to 1). See also MeshVS_NodalColorPrsBuilder
      // class for more explanations.
      aMaterial2[i].SetAmbientColor(Quantity_Color(NCollection_Vec3<float>(0.5f)));
      aMaterial2[i].SetDiffuseColor(Quantity_Color(NCollection_Vec3<float>(0.5f)));
    }
  }

  // Draw faces with two color
  if (!aTwoColorsOfElements.IsEmpty())
  {
    occ::handle<Graphic3d_AspectFillArea3d> aGroupFillAspect2 =
      new Graphic3d_AspectFillArea3d(Aspect_IS_SOLID,
                                     anInteriorColor,
                                     anEdgeColor,
                                     anEdgeType,
                                     anEdgeWidth,
                                     aMaterial2[0],
                                     aMaterial2[1]);
    aGroup2->SetClosed(false); // ignore toSupressBackFaces
    aGroup2->SetGroupPrimitivesAspect(aGroupFillAspect2);
  }
  for (NCollection_DataMap<MeshVS_TwoColors, NCollection_Map<int>>::Iterator aColIter2(
         aTwoColorsOfElements);
       aColIter2.More();
       aColIter2.Next())
  {
    if (aColIter2.Value().IsEmpty())
    {
      continue;
    }

    int aNbFacePrimitives = 0;
    int aNbEdgePrimitives = 0;

    for (it.Reset(); it.More(); it.Next())
    {
      int aNbNodes = 0;

      if (!aColIter2.Value().Contains(it.Key()))
        continue;

      if (!aSource->GetGeom(it.Key(), true, aCoords, aNbNodes, aType))
        continue;

      if (aType == MeshVS_ET_Face && aNbNodes > 0)
      {
        if (anEdgeOn)
        {
          aNbEdgePrimitives += aNbNodes; // add edge segments
        }

        aNbFacePrimitives += aNbNodes - 2; // add face triangles
      }
    }

    occ::handle<Graphic3d_ArrayOfTriangles> aFaceTriangles =
      new Graphic3d_ArrayOfTriangles(aNbFacePrimitives * 3, 0, IsReflect);

    occ::handle<Graphic3d_ArrayOfSegments> anEdgeSegments =
      new Graphic3d_ArrayOfSegments(aNbEdgePrimitives * 2);

    MeshVS_TwoColors aTC = aColIter2.Key();
    Quantity_Color   aMyIntColor, aMyBackColor;
    ExtractColors(aTC, aMyIntColor, aMyBackColor);

    // OCC20644 NOTE: aMyIntColor color is then scaled by TelUpdateMaterial() in OpenGl_attri.c
    // using the material reflection coefficients. This affects the visual result.
    occ::handle<Graphic3d_AspectFillArea3d> anAsp = new Graphic3d_AspectFillArea3d(Aspect_IS_SOLID,
                                                                                   aMyIntColor,
                                                                                   anEdgeColor,
                                                                                   anEdgeType,
                                                                                   anEdgeWidth,
                                                                                   aMaterial2[0],
                                                                                   aMaterial2[1]);
    anAsp->SetDistinguishOn();
    anAsp->SetInteriorColor(aMyIntColor);
    anAsp->SetBackInteriorColor(aMyBackColor);
    /*if (anEdgeOn)
      anAsp->SetEdgeOn();
    else
      anAsp->SetEdgeOff();*/
    aGroup2->SetPrimitivesAspect(anAsp);

    for (it.Reset(); it.More(); it.Next())
    {
      int aKey = it.Key();
      if (aColIter2.Value().Contains(aKey))
      {
        if (!aSource->GetGeom(aKey, true, aCoords, NbNodes, aType))
          continue;

        if (IsExcludingOn())
          IDsToExclude.Add(aKey);

        if (aType == MeshVS_ET_Face && NbNodes > 0)
        {
          // Preparing normal(s) to show reflections if requested
          occ::handle<NCollection_HArray1<double>> aNormals;
          // OCC21720 Always passing normals to OpenGL to make materials work
          // For OpenGL: "No normals" -> "No lighting" -> "no materials taken into account"
          bool aHasNormals = /*IsReflect &&*/
            aSource->GetNormalsByElement(aKey, IsMeshSmoothShading, aMaxFaceNodes, aNormals);

          for (int aNodeIdx = 0; aNodeIdx < NbNodes - 2; ++aNodeIdx)
          {
            for (int anIdx = 0; anIdx < 3; ++anIdx)
            {
              if (IsReflect)
              {
                aFaceTriangles->AddVertex(
                  aCoords(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 1),
                  aCoords(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 2),
                  aCoords(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 3),
                  aHasNormals ? aNormals->Value(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 1) : 0.0,
                  aHasNormals ? aNormals->Value(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 2) : 0.0,
                  aHasNormals ? aNormals->Value(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 3) : 1.0);
              }
              else
              {
                aFaceTriangles->AddVertex(aCoords(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 1),
                                          aCoords(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 2),
                                          aCoords(3 * (anIdx == 0 ? 0 : aNodeIdx + anIdx) + 3));
              }
            }
          }

          if (anEdgeOn)
          {
            for (int aNodeIdx = 0; aNodeIdx < NbNodes; ++aNodeIdx)
            {
              const int aNextIdx = (aNodeIdx + 1) % NbNodes;

              anEdgeSegments->AddVertex(aCoords(3 * aNodeIdx + 1),
                                        aCoords(3 * aNodeIdx + 2),
                                        aCoords(3 * aNodeIdx + 3));

              anEdgeSegments->AddVertex(aCoords(3 * aNextIdx + 1),
                                        aCoords(3 * aNextIdx + 2),
                                        aCoords(3 * aNextIdx + 3));
            }
          }
        }
      }
    }

    aGroup2->AddPrimitiveArray(aFaceTriangles);
    if (anEdgeOn)
    {
      aSGroup->AddPrimitiveArray(anEdgeSegments);
    }
  }
}

//=================================================================================================

void MeshVS_ElementalColorPrsBuilder::SetColors1(
  const NCollection_DataMap<int, Quantity_Color>& theColorMap)
{
  myElemColorMap1 = theColorMap;
}

//=================================================================================================

const NCollection_DataMap<int, Quantity_Color>& MeshVS_ElementalColorPrsBuilder::GetColors1() const
{
  return myElemColorMap1;
}

//=================================================================================================

bool MeshVS_ElementalColorPrsBuilder::HasColors1() const
{
  return (myElemColorMap1.Extent() > 0);
}

//=================================================================================================

bool MeshVS_ElementalColorPrsBuilder::GetColor1(const int ID, Quantity_Color& theColor) const
{
  bool aRes = myElemColorMap1.IsBound(ID);
  if (aRes)
    theColor = myElemColorMap1.Find(ID);

  return aRes;
}

//=================================================================================================

void MeshVS_ElementalColorPrsBuilder::SetColor1(const int theID, const Quantity_Color& theCol)
{
  bool aRes = myElemColorMap1.IsBound(theID);
  if (aRes)
    myElemColorMap1.ChangeFind(theID) = theCol;
  else
    myElemColorMap1.Bind(theID, theCol);
}

//=================================================================================================

void MeshVS_ElementalColorPrsBuilder::SetColors2(
  const NCollection_DataMap<int, MeshVS_TwoColors>& theColorMap)
{
  myElemColorMap2 = theColorMap;
}

//=================================================================================================

const NCollection_DataMap<int, MeshVS_TwoColors>& MeshVS_ElementalColorPrsBuilder::GetColors2()
  const
{
  return myElemColorMap2;
}

//=================================================================================================

bool MeshVS_ElementalColorPrsBuilder::HasColors2() const
{
  return (myElemColorMap2.Extent() > 0);
}

//=================================================================================================

bool MeshVS_ElementalColorPrsBuilder::GetColor2(const int ID, MeshVS_TwoColors& theColor) const
{
  bool aRes = myElemColorMap2.IsBound(ID);
  if (aRes)
    theColor = myElemColorMap2.Find(ID);

  return aRes;
}

//=================================================================================================

bool MeshVS_ElementalColorPrsBuilder::GetColor2(const int       ID,
                                                Quantity_Color& theColor1,
                                                Quantity_Color& theColor2) const
{
  MeshVS_TwoColors aTC;
  bool             aRes = GetColor2(ID, aTC);
  if (aRes)
    ExtractColors(aTC, theColor1, theColor2);
  return aRes;
}

//=================================================================================================

void MeshVS_ElementalColorPrsBuilder::SetColor2(const int             theID,
                                                const Quantity_Color& theCol1,
                                                const Quantity_Color& theCol2)
{
  SetColor2(theID, BindTwoColors(theCol1, theCol2));
}

//=================================================================================================

void MeshVS_ElementalColorPrsBuilder::SetColor2(const int theID, const MeshVS_TwoColors& theCol)
{
  bool aRes = myElemColorMap2.IsBound(theID);
  if (aRes)
    myElemColorMap2.ChangeFind(theID) = theCol;
  else
    myElemColorMap2.Bind(theID, theCol);
}
