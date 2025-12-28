// Created on: 2018-12-12
// Created by: Olga SURYANINOVA
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <AIS_CameraFrustum.hxx>

#include <AIS_DisplayMode.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_CameraFrustum, AIS_InteractiveObject)

namespace
{
constexpr float THE_DEFAULT_TRANSPARENCY = 0.7f;
const Quantity_Color         THE_DEFAULT_COLOR        = Quantity_NOC_WHITE;
} // namespace

//=================================================================================================

AIS_CameraFrustum::AIS_CameraFrustum()
    : myPoints(0, Graphic3d_Camera::FrustumVerticesNB)
{
  myDrawer->SetLineAspect(new Prs3d_LineAspect(THE_DEFAULT_COLOR, Aspect_TOL_SOLID, 1.0));

  occ::handle<Prs3d_ShadingAspect> aShadingAspect = new Prs3d_ShadingAspect();
  aShadingAspect->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
  aShadingAspect->Aspect()->SetAlphaMode(Graphic3d_AlphaMode_Blend);
  aShadingAspect->SetTransparency(THE_DEFAULT_TRANSPARENCY);
  aShadingAspect->SetColor(THE_DEFAULT_COLOR);
  myDrawer->SetShadingAspect(aShadingAspect);

  myDrawer->SetTransparency(THE_DEFAULT_TRANSPARENCY);
  SetDisplayMode(AIS_Shaded);
}

//=================================================================================================

bool AIS_CameraFrustum::AcceptDisplayMode(const int theMode) const
{
  return theMode == AIS_Shaded || theMode == AIS_WireFrame;
}

//=================================================================================================

void AIS_CameraFrustum::SetCameraFrustum(const occ::handle<Graphic3d_Camera>& theCamera)
{
  if (theCamera.IsNull())
  {
    return;
  }

  theCamera->FrustumPoints(myPoints);

  fillTriangles();
  fillBorders();

  SetToUpdate();
}

//=================================================================================================

void AIS_CameraFrustum::SetColor(const Quantity_Color& theColor)
{
  AIS_InteractiveObject::SetColor(theColor);
  myDrawer->ShadingAspect()->SetColor(theColor);
  myDrawer->LineAspect()->SetColor(theColor);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_CameraFrustum::UnsetColor()
{
  if (!HasColor())
  {
    return;
  }

  AIS_InteractiveObject::UnsetColor();

  myDrawer->ShadingAspect()->SetColor(THE_DEFAULT_COLOR);
  myDrawer->LineAspect()->SetColor(THE_DEFAULT_COLOR);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_CameraFrustum::UnsetTransparency()
{
  myDrawer->ShadingAspect()->SetTransparency(0.0f);
  myDrawer->SetTransparency(0.0f);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_CameraFrustum::fillTriangles()
{
  if (myTriangles.IsNull())
  {
    const int aPlaneTriangleVertsNb = 2 * 3;
    const int aPlanesNb             = 3 * 2;

    myTriangles = new Graphic3d_ArrayOfTriangles(Graphic3d_Camera::FrustumVerticesNB,
                                                 aPlaneTriangleVertsNb * aPlanesNb);
    myTriangles->SetVertice(Graphic3d_Camera::FrustumVerticesNB, gp_Pnt(0.0, 0.0, 0.0));

    // Triangles go in order (clockwise vertices traversing for correct normal):
    // (0, 2, 1), (3, 1, 2)
    const int aLookup1_clockwise[] = {0, 1, 0, 1, 0, 1};
    const int aLookup2_clockwise[] = {0, 0, 1, 1, 1, 0};
    // Triangles go in order (counterclockwise vertices traversing for correct normal):
    // (1, 2, 0), (2, 1, 3)
    const int aLookup1_anticlockwise[] = {0, 1, 0, 1, 0, 1};
    const int aLookup2_anticlockwise[] = {1, 0, 0, 0, 1, 1};
    int       aShifts[]                = {0, 0, 0};

    // Planes go in order:
    // LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
    for (int aFaceIdx = 0; aFaceIdx < 3; ++aFaceIdx)
    {
      for (int i = 0; i < 2; ++i)
      {
        for (int aPntIter = 0; aPntIter < aPlaneTriangleVertsNb; ++aPntIter)
        {
          aShifts[aFaceIdx] = i;
          if (i == 0)
          {
            aShifts[(aFaceIdx + 1) % 3] = aLookup1_clockwise[aPntIter];
            aShifts[(aFaceIdx + 2) % 3] = aLookup2_clockwise[aPntIter];
          }
          else
          {
            aShifts[(aFaceIdx + 1) % 3] = aLookup1_anticlockwise[aPntIter];
            aShifts[(aFaceIdx + 2) % 3] = aLookup2_anticlockwise[aPntIter];
          }

          int anIndex = aShifts[0] * 2 * 2 + aShifts[1] * 2 + aShifts[2];
          myTriangles->AddEdge(anIndex + 1);
        }
      }
    }
  }

  for (int aPointIter = 0; aPointIter < Graphic3d_Camera::FrustumVerticesNB;
       ++aPointIter)
  {
    const NCollection_Vec3<double> aPnt = myPoints[aPointIter];
    myTriangles->SetVertice(aPointIter + 1, gp_Pnt(aPnt.x(), aPnt.y(), aPnt.z()));
  }
}

//=================================================================================================

void AIS_CameraFrustum::fillBorders()
{
  if (myBorders.IsNull())
  {
    const int aPlaneSegmVertsNb = 2 * 4;
    const int aPlanesNb         = 3 * 2;
    myBorders = new Graphic3d_ArrayOfSegments(Graphic3d_Camera::FrustumVerticesNB,
                                              aPlaneSegmVertsNb * aPlanesNb);
    myBorders->SetVertice(Graphic3d_Camera::FrustumVerticesNB, gp_Pnt(0.0, 0.0, 0.0));

    // Segments go in order:
    // (0, 2), (2, 3), (3, 1), (1, 0)
    const int aLookup1[] = {0, 1, 1, 1, 1, 0, 0, 0};
    const int aLookup2[] = {0, 0, 0, 1, 1, 1, 1, 0};
    int       aShifts[]  = {0, 0, 0};

    // Planes go in order:
    // LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
    for (int aFaceIdx = 0; aFaceIdx < 3; ++aFaceIdx)
    {
      for (int i = 0; i < 2; ++i)
      {
        for (int aSegmVertIter = 0; aSegmVertIter < aPlaneSegmVertsNb; ++aSegmVertIter)
        {
          aShifts[aFaceIdx]           = i;
          aShifts[(aFaceIdx + 1) % 3] = aLookup1[aSegmVertIter];
          aShifts[(aFaceIdx + 2) % 3] = aLookup2[aSegmVertIter];

          int anIndex = aShifts[0] * 2 * 2 + aShifts[1] * 2 + aShifts[2];
          myBorders->AddEdge(anIndex + 1);
        }
      }
    }
  }

  for (int aPointIter = 0; aPointIter < Graphic3d_Camera::FrustumVerticesNB;
       ++aPointIter)
  {
    const NCollection_Vec3<double> aPnt = myPoints[aPointIter];
    myBorders->SetVertice(aPointIter + 1, gp_Pnt(aPnt.x(), aPnt.y(), aPnt.z()));
  }
}

//=================================================================================================

void AIS_CameraFrustum::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                const occ::handle<Prs3d_Presentation>& thePrs,
                                const int            theMode)
{
  thePrs->SetInfiniteState(true);
  if (myTriangles.IsNull())
  {
    return;
  }

  switch (theMode)
  {
    case AIS_Shaded: {
      occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
      aGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
      aGroup->AddPrimitiveArray(myTriangles);
    }
      [[fallthrough]];
    case AIS_WireFrame: {
      occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
      aGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
      aGroup->AddPrimitiveArray(myBorders);
      break;
    }
  }
}

//=================================================================================================

void AIS_CameraFrustum::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                         const int             theMode)
{
  occ::handle<SelectMgr_EntityOwner> anOwner = new SelectMgr_EntityOwner(this);
  switch (theMode)
  {
    case SelectionMode_Edges: {
      occ::handle<Select3D_SensitiveGroup> aSensitiveEntity = new Select3D_SensitiveGroup(anOwner);
      for (int anIter = 1; anIter <= myBorders->EdgeNumber(); anIter += 2)
      {
        aSensitiveEntity->Add(
          new Select3D_SensitiveSegment(anOwner,
                                        myBorders->Vertice(myBorders->Edge(anIter)),
                                        myBorders->Vertice(myBorders->Edge(anIter + 1))));
      }
      theSelection->Add(aSensitiveEntity);
      break;
    }
    case SelectionMode_Volume: {
      occ::handle<Select3D_SensitivePrimitiveArray> aSelArray =
        new Select3D_SensitivePrimitiveArray(anOwner);
      aSelArray->InitTriangulation(myTriangles->Attributes(),
                                   myTriangles->Indices(),
                                   TopLoc_Location());
      theSelection->Add(aSelArray);
      break;
    }
  }
}
