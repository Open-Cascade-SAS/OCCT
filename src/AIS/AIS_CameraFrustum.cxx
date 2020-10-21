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
  static const Standard_ShortReal THE_DEFAULT_TRANSPARENCY = 0.7f;
  static const Quantity_Color     THE_DEFAULT_COLOR = Quantity_NOC_WHITE;
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
AIS_CameraFrustum::AIS_CameraFrustum()
: myPoints (0, Graphic3d_Camera::FrustumVerticesNB)
{
  myDrawer->SetLineAspect (new Prs3d_LineAspect (THE_DEFAULT_COLOR, Aspect_TOL_SOLID, 1.0));

  Handle(Prs3d_ShadingAspect) aShadingAspect = new Prs3d_ShadingAspect();
  aShadingAspect->SetMaterial (Graphic3d_NameOfMaterial_Plastified);
  aShadingAspect->Aspect()->SetAlphaMode (Graphic3d_AlphaMode_Blend);
  aShadingAspect->SetTransparency (THE_DEFAULT_TRANSPARENCY);
  aShadingAspect->SetColor (THE_DEFAULT_COLOR);
  myDrawer->SetShadingAspect (aShadingAspect);

  myDrawer->SetTransparency (THE_DEFAULT_TRANSPARENCY);
  SetDisplayMode (AIS_Shaded);
}

//=======================================================================
//function : AcceptDisplayMode
//purpose  :
//=======================================================================
Standard_Boolean AIS_CameraFrustum::AcceptDisplayMode (const Standard_Integer theMode) const
{
  return theMode == AIS_Shaded || theMode == AIS_WireFrame;
}

//=======================================================================
//function : SetCameraFrustum
//purpose  :
//=======================================================================
void AIS_CameraFrustum::SetCameraFrustum (const Handle(Graphic3d_Camera)& theCamera)
{
  if (theCamera.IsNull())
  {
    return;
  }

  theCamera->FrustumPoints (myPoints);

  fillTriangles();
  fillBorders();

  SetToUpdate();
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_CameraFrustum::SetColor (const Quantity_Color& theColor)
{
  AIS_InteractiveObject::SetColor (theColor);
  myDrawer->ShadingAspect()->SetColor (theColor);
  myDrawer->LineAspect()->SetColor (theColor);
  SynchronizeAspects();
}

//=======================================================================
//function : UnsetColor
//purpose  :
//=======================================================================
void AIS_CameraFrustum::UnsetColor()
{
  if (!HasColor())
  {
    return;
  }

  AIS_InteractiveObject::UnsetColor();

  myDrawer->ShadingAspect()->SetColor (THE_DEFAULT_COLOR);
  myDrawer->LineAspect()->SetColor (THE_DEFAULT_COLOR);
  SynchronizeAspects();
}

//=======================================================================
//function : UnsetColor
//purpose  :
//=======================================================================
void AIS_CameraFrustum::UnsetTransparency()
{
  myDrawer->ShadingAspect()->SetTransparency (0.0f);
  myDrawer->SetTransparency (0.0f);
  SynchronizeAspects();
}

//=======================================================================
//function : fillTriangles
//purpose  :
//=======================================================================
void AIS_CameraFrustum::fillTriangles()
{
  if (myTriangles.IsNull())
  {
    const Standard_Integer aPlaneTriangleVertsNb = 2 * 3;
    const Standard_Integer aPlanesNb             = 3 * 2;

    myTriangles = new Graphic3d_ArrayOfTriangles (Graphic3d_Camera::FrustumVerticesNB, aPlaneTriangleVertsNb * aPlanesNb);
    myTriangles->SetVertice (Graphic3d_Camera::FrustumVerticesNB, gp_Pnt (0.0, 0.0, 0.0));

    // Triangles go in order (clockwise vertices traversing for correct normal):
    // (0, 2, 1), (3, 1, 2)
    const Standard_Integer aLookup1_clockwise[]     = { 0, 1, 0, 1, 0, 1 };
    const Standard_Integer aLookup2_clockwise[]     = { 0, 0, 1, 1, 1, 0 };
    // Triangles go in order (counterclockwise vertices traversing for correct normal):
    // (1, 2, 0), (2, 1, 3)
    const Standard_Integer aLookup1_anticlockwise[] = { 0, 1, 0, 1, 0, 1 };
    const Standard_Integer aLookup2_anticlockwise[] = { 1, 0, 0, 0, 1, 1 };
    Standard_Integer aShifts[]        = { 0, 0, 0 };

    // Planes go in order:
    // LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
    for (Standard_Integer aFaceIdx = 0; aFaceIdx < 3; ++aFaceIdx)
    {
      for (Standard_Integer i = 0; i < 2; ++i)
      {
        for (Standard_Integer aPntIter = 0; aPntIter < aPlaneTriangleVertsNb; ++aPntIter)
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

          Standard_Integer anIndex = aShifts[0] * 2 * 2 + aShifts[1] * 2 + aShifts[2];
          myTriangles->AddEdge (anIndex + 1);
        }
      }
    }
  }

  for (Standard_Integer aPointIter = 0; aPointIter < Graphic3d_Camera::FrustumVerticesNB; ++aPointIter)
  {
    const Graphic3d_Vec3d aPnt = myPoints[aPointIter];
    myTriangles->SetVertice (aPointIter + 1, gp_Pnt (aPnt.x(), aPnt.y(), aPnt.z()));
  }
}

//=======================================================================
//function : fillBorders
//purpose  :
//=======================================================================
void AIS_CameraFrustum::fillBorders()
{
  if (myBorders.IsNull())
  {
    const Standard_Integer aPlaneSegmVertsNb = 2 * 4;
    const Standard_Integer aPlanesNb         = 3 * 2;
    myBorders = new Graphic3d_ArrayOfSegments (Graphic3d_Camera::FrustumVerticesNB, aPlaneSegmVertsNb * aPlanesNb);
    myBorders->SetVertice (Graphic3d_Camera::FrustumVerticesNB, gp_Pnt (0.0, 0.0, 0.0));

    // Segments go in order:
    // (0, 2), (2, 3), (3, 1), (1, 0)
    const Standard_Integer aLookup1[] = { 0, 1, 1, 1, 1, 0, 0, 0 };
    const Standard_Integer aLookup2[] = { 0, 0, 0, 1, 1, 1, 1, 0 };
    Standard_Integer aShifts[] = { 0, 0, 0 };

    // Planes go in order:
    // LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
    for (Standard_Integer aFaceIdx = 0; aFaceIdx < 3; ++aFaceIdx)
    {
      for (Standard_Integer i = 0; i < 2; ++i)
      {
        for (Standard_Integer aSegmVertIter = 0; aSegmVertIter < aPlaneSegmVertsNb; ++aSegmVertIter)
        {
          aShifts[aFaceIdx] = i;
          aShifts[(aFaceIdx + 1) % 3] = aLookup1[aSegmVertIter];
          aShifts[(aFaceIdx + 2) % 3] = aLookup2[aSegmVertIter];

          Standard_Integer anIndex = aShifts[0] * 2 * 2 + aShifts[1] * 2 + aShifts[2];
          myBorders->AddEdge (anIndex + 1);
        }
      }
    }
  }

  for (Standard_Integer aPointIter = 0; aPointIter < Graphic3d_Camera::FrustumVerticesNB; ++aPointIter)
  {
    const Graphic3d_Vec3d aPnt = myPoints[aPointIter];
    myBorders->SetVertice (aPointIter + 1, gp_Pnt (aPnt.x(), aPnt.y(), aPnt.z()));
  }
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_CameraFrustum::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                                 const Handle(Prs3d_Presentation)&           thePrs,
                                 const Standard_Integer                      theMode)
{
  thePrs->SetInfiniteState (true);
  if (myTriangles.IsNull())
  {
    return;
  }

  switch (theMode)
  {
    case AIS_Shaded:
    {
      Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
      aGroup->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
      aGroup->AddPrimitiveArray (myTriangles);
    }
    Standard_FALLTHROUGH
    case AIS_WireFrame:
    {
      Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
      aGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());
      aGroup->AddPrimitiveArray (myBorders);
      break;
    }
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  :
//=======================================================================
void AIS_CameraFrustum::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                          const Standard_Integer             theMode)
{
  Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this);
  switch (theMode)
  {
    case SelectionMode_Edges:
    {
      Handle(Select3D_SensitiveGroup) aSensitiveEntity = new Select3D_SensitiveGroup (anOwner);
      for (Standard_Integer anIter = 1; anIter <= myBorders->EdgeNumber(); anIter += 2)
      {
        aSensitiveEntity->Add (new Select3D_SensitiveSegment (anOwner, myBorders->Vertice (myBorders->Edge (anIter)), myBorders->Vertice(myBorders->Edge (anIter + 1))));
      }
      theSelection->Add (aSensitiveEntity);
      break;
    }
    case SelectionMode_Volume:
    {
      Handle(Select3D_SensitivePrimitiveArray) aSelArray = new Select3D_SensitivePrimitiveArray (anOwner);
      aSelArray->InitTriangulation (myTriangles->Attributes(), myTriangles->Indices(), TopLoc_Location());
      theSelection->Add (aSelArray);
      break;
    }
  }
}
