// Created on: 2020-09-07
// Created by: Maria KRYLOVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <AIS_LightSource.hxx>

#include <AIS_InteractiveContext.hxx>
#include <gp_Quaternion.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_ToolCylinder.hxx>
#include <Prs3d_ToolSphere.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSphere.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_LightSource, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_LightSourceOwner, SelectMgr_EntityOwner)

//=================================================================================================

AIS_LightSourceOwner::AIS_LightSourceOwner(const occ::handle<AIS_LightSource>& theObject,
                                           int                                 thePriority)
    : SelectMgr_EntityOwner((const occ::handle<SelectMgr_SelectableObject>&)theObject, thePriority)
{
  //
}

//=================================================================================================

bool AIS_LightSourceOwner::HandleMouseClick(const NCollection_Vec2<int>&,
                                            Aspect_VKeyMouse theKey,
                                            Aspect_VKeyFlags theFlags,
                                            bool)
{
  AIS_LightSource* aLightSource = dynamic_cast<AIS_LightSource*>(mySelectable);
  if (aLightSource != NULL && aLightSource->ToSwitchOnClick()
      && theKey == Aspect_VKeyMouse_LeftButton && theFlags == Aspect_VKeyFlags_NONE)
  {
    aLightSource->Light()->SetEnabled(!aLightSource->Light()->IsEnabled());
    aLightSource->updateLightAspects();
    return true;
  }
  return false;
}

//=================================================================================================

void AIS_LightSourceOwner::HilightWithColor(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                            const occ::handle<Prs3d_Drawer>&               theStyle,
                                            const int                                      theMode)
{
  occ::handle<AIS_LightSource> aLightSource = occ::down_cast<AIS_LightSource>(mySelectable);
  if (aLightSource.IsNull())
  {
    return;
  }

  if (aLightSource->Light()->Type() == Graphic3d_TypeOfLightSource_Directional
      && aLightSource->myIsDraggable)
  {
    occ::handle<Prs3d_Presentation> aPrs = aLightSource->GetHilightPresentation(thePM);
    const Graphic3d_ZLayerId        aZLayer =
      theStyle->ZLayer() != -1
               ? theStyle->ZLayer()
               : (thePM->IsImmediateModeOn() ? Graphic3d_ZLayerId_Top : aLightSource->ZLayer());
    aPrs->Clear();
    if (aPrs->GetZLayer() != aZLayer)
    {
      aPrs->SetZLayer(aZLayer);
    }
    occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(1);
    const gp_Pnt                         aDetPnt = aLightSource->mySensSphere->LastDetectedPoint();
    if (aDetPnt.X() == RealLast())
    {
      return;
    }
    aPoints->AddVertex(aDetPnt);
    occ::handle<Graphic3d_Group>         aGroup = aPrs->NewGroup();
    const occ::handle<Prs3d_PointAspect> aPointAspect =
      new Prs3d_PointAspect(Aspect_TOM_O_POINT, theStyle->Color(), 3.0f);
    aGroup->SetGroupPrimitivesAspect(aPointAspect->Aspect());
    aGroup->AddPrimitiveArray(aPoints);

    const double               aRadius = aLightSource->Size() * 0.5;
    const int                  aNbPnts = int(aLightSource->ArcSize() * 180 / (M_PI * aRadius));
    NCollection_Array1<gp_Pnt> aCircPoints(0, aNbPnts);
    const gp_Dir               aDirNorm(gp_Vec(gp::Origin(), aDetPnt));
    gp_Dir                     aDirNormToPln(gp::DY());
    if (!gp::DX().IsParallel(aDirNorm, Precision::Angular()))
    {
      aDirNormToPln = gp::DX().Crossed(aDirNorm);
    }
    for (int aStep = 0; aStep < aNbPnts; ++aStep)
    {
      aCircPoints.SetValue(
        aStep,
        (aDetPnt.Rotated(gp_Ax1(gp::Origin(), aDirNormToPln), M_PI / 90 * (aStep - aNbPnts / 2))));
    }

    occ::handle<Graphic3d_Group>            aCircGroup = aPrs->NewGroup();
    occ::handle<Graphic3d_ArrayOfPolylines> aPolylines =
      new Graphic3d_ArrayOfPolylines(aNbPnts * 2, 2);
    aPolylines->AddBound(aNbPnts);

    for (int anIdx = 0; anIdx < aNbPnts; ++anIdx)
    {
      aPolylines->AddVertex(
        aCircPoints.Value(anIdx).Rotated(gp_Ax1(gp::Origin(), aDirNorm), M_PI / 2));
    }
    aPolylines->AddBound(aNbPnts);
    for (int anIdx = 0; anIdx < aNbPnts; ++anIdx)
    {
      aPolylines->AddVertex(aCircPoints.Value(anIdx));
    }
    aCircGroup->AddPrimitiveArray(aPolylines, false);
    aCircGroup->SetGroupPrimitivesAspect(theStyle->ArrowAspect()->Aspect());
    if (thePM->IsImmediateModeOn())
    {
      thePM->AddToImmediateList(aPrs);
    }
    else
    {
      aPrs->Display();
    }
  }
  else
  {
    base_type::HilightWithColor(thePM, theStyle, theMode);
    ;
  }
}

//=================================================================================================

bool AIS_LightSourceOwner::IsForcedHilight() const
{
  occ::handle<AIS_LightSource> aLightSource = occ::down_cast<AIS_LightSource>(mySelectable);
  if (aLightSource.IsNull())
  {
    return false;
  }
  if (aLightSource->Light()->Type() == Graphic3d_TypeOfLightSource_Directional)
  {
    return true;
  }
  return false;
}

//=================================================================================================

AIS_LightSource::AIS_LightSource(const occ::handle<Graphic3d_CLight>& theLight)
    : myLightSource(theLight),
      myCodirMarkerType(Aspect_TOM_X),
      myOpposMarkerType(Aspect_TOM_O_POINT),
      mySize(50),
      myNbArrows(5),
      myNbSplitsQuadric(theLight->Type() == Graphic3d_TypeOfLightSource_Ambient ? 10 : 30),
      myNbSplitsArrow(20),
      mySensSphereArcSize(25),
      myIsZoomable(theLight->Type() == Graphic3d_TypeOfLightSource_Positional
                   || theLight->Type() == Graphic3d_TypeOfLightSource_Spot),
      myIsDraggable(theLight->Type() == Graphic3d_TypeOfLightSource_Directional),
      myToDisplayName(true),
      myToDisplayRange(true),
      myToSwitchOnClick(true)
{
  myMarkerTypes[0] = Aspect_TOM_O_X;
  myMarkerTypes[1] = Aspect_TOM_O_POINT;

  myInfiniteState = true;

  const Quantity_Color aColor = theLight->Color();
  myDrawer->SetPointAspect(new Prs3d_PointAspect(myMarkerTypes[1], aColor, 3.0f));
  myDisabledMarkerAspect = new Graphic3d_AspectMarker3d(Aspect_TOM_EMPTY, aColor, 3.0f);

  Graphic3d_MaterialAspect aMat(Graphic3d_NameOfMaterial_UserDefined);
  aMat.SetColor(aColor);
  myDrawer->SetArrowAspect(new Prs3d_ArrowAspect());
  myDrawer->ArrowAspect()->SetColor(aColor);
  myDrawer->ArrowAspect()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myDrawer->ArrowAspect()->Aspect()->ChangeFrontMaterial() = aMat;
  myDrawer->ArrowAspect()->Aspect()->SetMarkerType(Aspect_TOM_EMPTY);
  myDrawer->ArrowAspect()->Aspect()->SetMarkerScale(2.0f);
  myArrowLineAspectShadow = new Graphic3d_AspectLine3d(
    Quantity_NOC_BLACK,
    Aspect_TOL_SOLID,
    theLight->Type() != Graphic3d_TypeOfLightSource_Ambient ? 3.0f : 1.0f);

  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->SetColor(aColor);
  myDrawer->ShadingAspect()->SetMaterial(aMat);
  myDrawer->ShadingAspect()->SetTransparency(0.5f);
  myDrawer->ShadingAspect()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);

  myDrawer->SetTextAspect(new Prs3d_TextAspect());
  myDrawer->TextAspect()->Aspect()->SetDisplayType(Aspect_TODT_SHADOW);
  myDrawer->TextAspect()->Aspect()->SetColorSubTitle(Quantity_NOC_BLACK);
  myDrawer->TextAspect()->SetHorizontalJustification(Graphic3d_HTA_LEFT);
  myDrawer->TextAspect()->SetVerticalJustification(Graphic3d_VTA_TOPFIRSTLINE);

  updateLightTransformPersistence();

  myDrawer->SetDisplayMode(0);
  myDynHilightDrawer = new Prs3d_Drawer();
  myDynHilightDrawer->Link(myDrawer);
  myDynHilightDrawer->SetDisplayMode(1);
  myDynHilightDrawer->SetColor(Quantity_NOC_CYAN1);

  if (!myTransformPersistence.IsNull() && myTransformPersistence->IsTrihedronOr2d())
  {
    myDrawer->SetZLayer(Graphic3d_ZLayerId_Topmost);
    myDynHilightDrawer->SetZLayer(Graphic3d_ZLayerId_Topmost);
    myDrawer->TextAspect()->SetHorizontalJustification(Graphic3d_HTA_CENTER);
    myDrawer->TextAspect()->SetVerticalJustification(Graphic3d_VTA_TOP);
  }
}

//=================================================================================================

bool AIS_LightSource::ProcessDragging(const occ::handle<AIS_InteractiveContext>& theCtx,
                                      const occ::handle<V3d_View>&               theView,
                                      const occ::handle<SelectMgr_EntityOwner>&  theOwner,
                                      const NCollection_Vec2<int>&               theDragFrom,
                                      const NCollection_Vec2<int>&               theDragTo,
                                      const AIS_DragAction                       theAction)
{
  if (Light()->Type() != Graphic3d_TypeOfLightSource_Directional)
  {
    return false;
  }

  switch (theAction)
  {
    case AIS_DragAction_Start: {
      myLocTrsfStart = LocalTransformation();
      return true;
    }
    case AIS_DragAction_Confirmed: {
      return true;
    }
    case AIS_DragAction_Update: {
      mySensSphere->ResetLastDetectedPoint();
      SetLocalTransformation(myLocTrsfStart);
      theCtx->MainSelector()->Pick(theDragFrom.x(), theDragFrom.y(), theView);
      gp_Pnt aStartPosition = mySensSphere->LastDetectedPoint();

      mySensSphere->ResetLastDetectedPoint();
      theCtx->MainSelector()->Pick(theDragTo.x(), theDragTo.y(), theView);
      gp_Pnt aCurrPosition = mySensSphere->LastDetectedPoint();
      if (aCurrPosition.X() != RealLast()
          && aStartPosition.Distance(aCurrPosition) > Precision::Confusion())
      {
        gp_Quaternion aQRot;
        aQRot.SetRotation(gp_Vec(gp_Pnt(0, 0, 0), aStartPosition),
                          gp_Vec(gp_Pnt(0, 0, 0), aCurrPosition));
        gp_Trsf aTrsf;
        aTrsf.SetRotation(aQRot);
        SetLocalTransformation(myLocTrsfStart * aTrsf);
        const int aHiMod = HasHilightMode() ? HilightMode() : 0;
        theOwner->UpdateHighlightTrsf(theCtx->CurrentViewer(), theCtx->MainPrsMgr(), aHiMod);
      }
      return true;
    }
    case AIS_DragAction_Abort: {
      return true;
    }
    case AIS_DragAction_Stop: {
      GetHilightPresentation(theCtx->MainPrsMgr())->Clear();
      break;
    }
  }
  return false;
}

//=================================================================================================

void AIS_LightSource::updateLightAspects()
{
  const Quantity_Color aBaseColor = myLightSource->Color();
  const Quantity_Color aDimColor(aBaseColor.Rgb() * 0.3f);
  const Quantity_Color aColor = myLightSource->IsEnabled() ? aBaseColor : aDimColor;
  myDrawer->PointAspect()->SetColor(aColor);
  myDrawer->PointAspect()->Aspect()->SetMarkerType(MarkerType(myLightSource->IsEnabled()));
  myDrawer->PointAspect()->Aspect()->SetMarkerImage(MarkerImage(myLightSource->IsEnabled()));

  myDisabledMarkerAspect->SetColor(aColor);
  myDisabledMarkerAspect->SetMarkerScale(myDrawer->PointAspect()->Aspect()->MarkerScale());
  myDisabledMarkerAspect->SetMarkerType(myLightSource->IsEnabled() ? Aspect_TOM_EMPTY
                                                                   : MarkerType(false));
  myDisabledMarkerAspect->SetMarkerImage(MarkerImage(false));

  myDrawer->ShadingAspect()->SetColor(aColor);
  myDrawer->ArrowAspect()->SetColor(aColor);
  myDrawer->ArrowAspect()->Aspect()->ChangeFrontMaterial().SetColor(aColor);

  if (myLightSource->Type() == Graphic3d_TypeOfLightSource_Directional)
  {
    const double        anAngleTol = 2.0 * M_PI / 180.0;
    Aspect_TypeOfMarker aDirMark   = Aspect_TOM_EMPTY;
    if (myLightSource->IsEnabled() && myLightSource->IsHeadlight()
        && myLightSource->Direction().IsParallel(gp::DZ(), anAngleTol))
    {
      aDirMark = myLightSource->Direction().IsOpposite(-gp::DZ(), anAngleTol) ? myOpposMarkerType
                                                                              : myCodirMarkerType;
    }
    myDrawer->ArrowAspect()->Aspect()->SetMarkerType(aDirMark);
  }
  SynchronizeAspects();
}

//=================================================================================================

void AIS_LightSource::updateLightTransformPersistence()
{
  occ::handle<Graphic3d_TransformPers> aTrsfPers = myTransformPersistence;
  switch (myLightSource->Type())
  {
    case Graphic3d_TypeOfLightSource_Ambient: {
      if (!myIsZoomable)
      {
        if (aTrsfPers.IsNull() || !aTrsfPers->IsTrihedronOr2d())
        {
          aTrsfPers = new Graphic3d_TransformPers(Graphic3d_TMF_TriedronPers,
                                                  Aspect_TOTP_LEFT_UPPER,
                                                  NCollection_Vec2<int>(50));
        }
      }
      else
      {
        aTrsfPers.Nullify();
      }
      break;
    }
    case Graphic3d_TypeOfLightSource_Directional: {
      Graphic3d_TransModeFlags aMode =
        myLightSource->IsHeadlight() ? Graphic3d_TMF_2d : Graphic3d_TMF_TriedronPers;
      if (myIsZoomable)
      {
        aMode = myLightSource->IsHeadlight() ? Graphic3d_TMF_CameraPers : Graphic3d_TMF_None;
      }
      if (aMode != Graphic3d_TMF_None)
      {
        if (aTrsfPers.IsNull() || aTrsfPers->Mode() != aMode)
        {
          if (aMode == Graphic3d_TMF_CameraPers)
          {
            aTrsfPers = new Graphic3d_TransformPers(Graphic3d_TMF_CameraPers);
          }
          else
          {
            aTrsfPers =
              new Graphic3d_TransformPers(aMode, Aspect_TOTP_LEFT_UPPER, NCollection_Vec2<int>(50));
          }
        }
      }
      else
      {
        aTrsfPers.Nullify();
      }
      break;
    }
    case Graphic3d_TypeOfLightSource_Positional:
    case Graphic3d_TypeOfLightSource_Spot: {
      Graphic3d_TransModeFlags aMode =
        myLightSource->IsHeadlight()
          ? Graphic3d_TMF_CameraPers
          : (!myIsZoomable ? Graphic3d_TMF_ZoomPers : Graphic3d_TMF_None);
      if (aMode != Graphic3d_TMF_None)
      {
        if (aTrsfPers.IsNull() || aTrsfPers->Mode() != aMode)
        {
          if (aMode == Graphic3d_TMF_CameraPers)
          {
            aTrsfPers = new Graphic3d_TransformPers(Graphic3d_TMF_CameraPers);
          }
          else
          {
            aTrsfPers = new Graphic3d_TransformPers(aMode, myLightSource->Position());
          }
        }
        if (aMode == Graphic3d_TMF_ZoomPers)
        {
          aTrsfPers->SetAnchorPoint(myLightSource->Position());
        }
      }
      else
      {
        aTrsfPers.Nullify();
      }
      break;
    }
  }

  SetTransformPersistence(aTrsfPers);
}

//=================================================================================================

void AIS_LightSource::updateLightLocalTransformation()
{
  myLocalTransformation.Nullify();
  switch (myLightSource->Type())
  {
    case Graphic3d_TypeOfLightSource_Ambient: {
      if (myIsZoomable)
      {
        gp_Trsf aTrsf;
        aTrsf.SetTranslation(gp::Origin(), myLightSource->Position());
        myLocalTransformation = new TopLoc_Datum3D(aTrsf);
      }
      break;
    }
    case Graphic3d_TypeOfLightSource_Directional: {
      const gp_Pnt aLightPos = (myIsZoomable && !myLightSource->IsHeadlight())
                                 ? myLightSource->DisplayPosition()
                                 : gp::Origin();
      gp_Trsf      aTrsf;
      const gp_Ax2 anAx2(aLightPos, -myLightSource->Direction());
      aTrsf.SetTransformation(anAx2, gp_Ax3());
      myLocalTransformation = new TopLoc_Datum3D(aTrsf);
      break;
    }
    case Graphic3d_TypeOfLightSource_Positional: {
      if (myIsZoomable)
      {
        gp_Trsf aTrsf;
        aTrsf.SetTranslation(gp::Origin(), myLightSource->Position());
        myLocalTransformation = new TopLoc_Datum3D(aTrsf);
      }
      break;
    }
    case Graphic3d_TypeOfLightSource_Spot: {
      gp_Trsf      aTrsf;
      const gp_Ax2 anAx2(myIsZoomable ? myLightSource->Position() : gp::Origin(),
                         -myLightSource->Direction());
      aTrsf.SetTransformation(anAx2, gp_Ax3());
      myLocalTransformation = new TopLoc_Datum3D(aTrsf);
      break;
    }
  }
  UpdateTransformation();
}

//=================================================================================================

void AIS_LightSource::setLocalTransformation(const occ::handle<TopLoc_Datum3D>& theTrsf)
{
  const gp_Trsf aTrsf = !theTrsf.IsNull() ? theTrsf->Transformation() : gp_Trsf();
  switch (myLightSource->Type())
  {
    case Graphic3d_TypeOfLightSource_Ambient: {
      break;
    }
    case Graphic3d_TypeOfLightSource_Directional: {
      gp_Dir aNewDir = (-gp::DZ()).Transformed(aTrsf);
      myLightSource->SetDirection(aNewDir);
      if (myIsZoomable)
      {
        gp_Pnt aNewPos = gp::Origin().Transformed(aTrsf);
        myLightSource->SetDisplayPosition(aNewPos);
      }
      break;
    }
    case Graphic3d_TypeOfLightSource_Positional: {
      gp_Pnt aNewPos = gp::Origin().Transformed(aTrsf);
      myLightSource->SetPosition(aNewPos);
      break;
    }
    case Graphic3d_TypeOfLightSource_Spot: {
      gp_Pnt aNewPos = gp::Origin().Transformed(aTrsf);
      myLightSource->SetPosition(aNewPos);

      gp_Dir aNewDir = (-gp::DZ()).Transformed(aTrsf);
      myLightSource->SetDirection(aNewDir);
      break;
    }
  }

  base_type::setLocalTransformation(new TopLoc_Datum3D(aTrsf));

  updateLightAspects();
  updateLightTransformPersistence();
}

//=================================================================================================

void AIS_LightSource::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                              const occ::handle<Prs3d_Presentation>& thePrs,
                              const int                              theMode)
{
  thePrs->SetInfiniteState(myInfiniteState);
  if (theMode != 0 && theMode != 1)
  {
    return;
  }

  if (theMode == 0)
  {
    updateLightAspects();
    updateLightTransformPersistence();
    updateLightLocalTransformation();
  }

  switch (myLightSource->Type())
  {
    case Graphic3d_TypeOfLightSource_Ambient:
      computeAmbient(thePrs, theMode);
      break;
    case Graphic3d_TypeOfLightSource_Directional:
      computeDirectional(thePrs, theMode);
      break;
    case Graphic3d_TypeOfLightSource_Positional:
      computePositional(thePrs, theMode);
      break;
    case Graphic3d_TypeOfLightSource_Spot:
      computeSpot(thePrs, theMode);
      break;
  }

  if (myToDisplayName)
  {
    TCollection_AsciiString aPrefix =
      !myTransformPersistence.IsNull() && myTransformPersistence->IsTrihedronOr2d() ? "\n" : "   ";
    TCollection_AsciiString aName = aPrefix + myLightSource->Name();
    Prs3d_Text::Draw(thePrs->NewGroup(), myDrawer->TextAspect(), aName, gp::Origin());
  }
}

//=================================================================================================

void AIS_LightSource::computeAmbient(const occ::handle<Prs3d_Presentation>& thePrs,
                                     const int                              theMode)
{
  const gp_XYZ aLightPos = gp::Origin().XYZ();
  if (theMode == 0)
  {
    occ::handle<Graphic3d_ArrayOfTriangles> aSphereArray =
      Prs3d_ToolSphere::Create(mySize * 0.25, myNbSplitsQuadric, myNbSplitsQuadric, gp_Trsf());
    occ::handle<Graphic3d_Group> aSphereGroup = thePrs->NewGroup();
    aSphereGroup->SetClosed(true);
    aSphereGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
    aSphereGroup->AddPrimitiveArray(aSphereArray);
  }
  if (theMode == 0 || theMode == 1)
  {
    const double aLen        = mySize * 0.25;
    const int    aNbArrows   = 6;
    const gp_Dir aDirList[6] = {-gp::DX(), gp::DX(), -gp::DY(), gp::DY(), -gp::DZ(), gp::DZ()};

    const Prs3d_ToolCylinder                aCylTool(mySize * 0.1,
                                      0.0,
                                      mySize * 0.2,
                                      myNbSplitsArrow,
                                      myNbSplitsArrow);
    occ::handle<Graphic3d_ArrayOfTriangles> aTrisArray =
      new Graphic3d_ArrayOfTriangles(aNbArrows * aCylTool.VerticesNb(),
                                     aNbArrows * aCylTool.TrianglesNb() * 3,
                                     Graphic3d_ArrayFlags_VertexNormal);
    occ::handle<Graphic3d_ArrayOfSegments> aLineArray =
      new Graphic3d_ArrayOfSegments(aNbArrows * 2);
    for (int anArrIter = 0; anArrIter < aNbArrows; ++anArrIter)
    {
      const gp_Dir& aDir = aDirList[anArrIter];
      const gp_XYZ  aPnt = aLightPos + aDir.XYZ() * aLen;
      if (!aLineArray.IsNull())
      {
        aLineArray->AddVertex(aPnt + aDir.XYZ() * aLen * 0.5);
        aLineArray->AddVertex(aPnt + aDir.XYZ() * aLen * 1.5);
      }
      if (!aTrisArray.IsNull())
      {
        const gp_Ax3 aSystem(aPnt + aDir.XYZ() * aLen, -aDir);
        gp_Trsf      aTrsfCone;
        aTrsfCone.SetTransformation(aSystem, gp_Ax3());
        aCylTool.FillArray(aTrisArray, aTrsfCone);
      }
    }

    if (!aLineArray.IsNull())
    {
      occ::handle<Graphic3d_Group> aDirGroupShadow = thePrs->NewGroup();
      aDirGroupShadow->SetGroupPrimitivesAspect(myArrowLineAspectShadow);
      aDirGroupShadow->AddPrimitiveArray(aLineArray);
    }
    if (!aTrisArray.IsNull())
    {
      occ::handle<Graphic3d_Group> anArrowGroup = thePrs->NewGroup();
      anArrowGroup->SetClosed(true);
      anArrowGroup->SetGroupPrimitivesAspect(myDrawer->ArrowAspect()->Aspect());
      anArrowGroup->AddPrimitiveArray(aTrisArray);
    }
  }

  {
    occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(1);
    aPoints->AddVertex(aLightPos);
    occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
    aGroup->SetGroupPrimitivesAspect(theMode == 1 ? myDrawer->PointAspect()->Aspect()
                                                  : myDisabledMarkerAspect);
    aGroup->AddPrimitiveArray(aPoints);
  }
}

//=================================================================================================

void AIS_LightSource::computeDirectional(const occ::handle<Prs3d_Presentation>& thePrs,
                                         const int                              theMode)
{
  const double aDistance = mySize * 0.5;
  const double aStep     = aDistance * 0.5;

  // light source direction is set to local transformation
  const gp_Dir aLightDir = -gp::DZ();
  const gp_XYZ aLightPos = -aStep * aLightDir.XYZ();

  int aNbArrows = 1;
  if (myNbArrows >= 9)
  {
    aNbArrows = 9;
  }
  else if (myNbArrows >= 5)
  {
    aNbArrows = 5;
  }
  else if (myNbArrows >= 3)
  {
    aNbArrows = 3;
  }
  NCollection_Array1<gp_Pnt> aPoints(1, aNbArrows);
  {
    const gp_Ax2 anAxes(gp::Origin(), aLightDir);
    const gp_XYZ aDY  = anAxes.YDirection().XYZ() * aStep;
    const gp_XYZ aDX  = anAxes.XDirection().XYZ() * aStep;
    const gp_XYZ aDXY = aDX + aDY;
    switch (aNbArrows)
    {
      case 9: {
        aPoints.SetValue(6, aLightPos + aDY);
        aPoints.SetValue(7, aLightPos + aDX);
        aPoints.SetValue(8, aLightPos - aDY);
        aPoints.SetValue(9, aLightPos - aDX);
      }
        [[fallthrough]];
      case 5: {
        aPoints.SetValue(4, aLightPos - aDY + aDX);
        aPoints.SetValue(5, aLightPos + aDY - aDX);
      }
        [[fallthrough]];
      case 3: {
        aPoints.SetValue(2, aLightPos + aDXY);
        aPoints.SetValue(3, aLightPos - aDXY);
      }
        [[fallthrough]];
      case 1: {
        aPoints.SetValue(1, aLightPos);
        break;
      }
    }
  }

  const Prs3d_ToolCylinder                aCylTool(aDistance * 0.1,
                                    0.0,
                                    aDistance * 0.2,
                                    myNbSplitsArrow,
                                    myNbSplitsArrow);
  occ::handle<Graphic3d_ArrayOfTriangles> aTrisArray;
  if (theMode == 0)
  {
    aTrisArray = new Graphic3d_ArrayOfTriangles(aNbArrows * aCylTool.VerticesNb(),
                                                aNbArrows * aCylTool.TrianglesNb() * 3,
                                                Graphic3d_ArrayFlags_VertexNormal);
  }
  occ::handle<Graphic3d_ArrayOfPoints>   aPntArray  = new Graphic3d_ArrayOfPoints(aNbArrows);
  occ::handle<Graphic3d_ArrayOfSegments> aLineArray = new Graphic3d_ArrayOfSegments(aNbArrows * 2);
  for (int aPntIter = aPoints.Lower(); aPntIter <= aPoints.Upper(); ++aPntIter)
  {
    const gp_Pnt aPnt = aPoints.Value(aPntIter);
    if (!aPntArray.IsNull())
    {
      aPntArray->AddVertex(aPnt);
    }
    if (!aLineArray.IsNull())
    {
      aLineArray->AddVertex(aPnt);
      aLineArray->AddVertex(gp_Pnt(aPnt.XYZ() + aLightDir.XYZ() * aDistance));
    }
    if (!aTrisArray.IsNull())
    {
      const gp_Ax3 aSystem(aPnt.XYZ() + aLightDir.XYZ() * aDistance, aLightDir);
      gp_Trsf      aTrsfCone;
      aTrsfCone.SetTransformation(aSystem, gp_Ax3());
      aCylTool.FillArray(aTrisArray, aTrsfCone);
    }
  }

  if (!aLineArray.IsNull() && theMode == 0)
  {
    occ::handle<Graphic3d_Group> aDirGroupShadow = thePrs->NewGroup();
    aDirGroupShadow->SetGroupPrimitivesAspect(myArrowLineAspectShadow);
    aDirGroupShadow->AddPrimitiveArray(aLineArray);
  }
  if (!aLineArray.IsNull())
  {
    occ::handle<Graphic3d_Group> aDirGroup = thePrs->NewGroup();
    aDirGroup->SetGroupPrimitivesAspect(myDrawer->ArrowAspect()->Aspect());
    aDirGroup->AddPrimitiveArray(aLineArray);
  }
  if (!aTrisArray.IsNull())
  {
    occ::handle<Graphic3d_Group> anArrowGroup = thePrs->NewGroup();
    anArrowGroup->SetClosed(true);
    anArrowGroup->SetGroupPrimitivesAspect(myDrawer->ArrowAspect()->Aspect());
    anArrowGroup->AddPrimitiveArray(aTrisArray);
  }
  if (!aPntArray.IsNull())
  {
    occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
    aGroup->SetGroupPrimitivesAspect(myDrawer->ArrowAspect()->Aspect());
    aGroup->AddPrimitiveArray(aPntArray);
  }
  {
    occ::handle<Graphic3d_ArrayOfPoints> aPntArray2 = new Graphic3d_ArrayOfPoints(1);
    aPntArray2->AddVertex(aLightPos);
    occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
    aGroup->SetGroupPrimitivesAspect(myDisabledMarkerAspect);
    aGroup->AddPrimitiveArray(aPntArray2);
  }
}

//=================================================================================================

void AIS_LightSource::computePositional(const occ::handle<Prs3d_Presentation>& thePrs,
                                        const int                              theMode)
{
  // light source position is set to local transformation
  const gp_XYZ aLightPos = gp::Origin().XYZ();
  const double aRadius = (myIsZoomable && myLightSource->HasRange()) ? myLightSource->Range() : 0.0;
  if (theMode == 0 && aRadius > 0.0 && myToDisplayRange)
  {
    occ::handle<Graphic3d_ArrayOfTriangles> aPosRangeArray =
      Prs3d_ToolSphere::Create(aRadius, myNbSplitsQuadric, myNbSplitsQuadric, gp_Trsf());
    occ::handle<Graphic3d_Group> aRangeGroup = thePrs->NewGroup();
    aRangeGroup->SetClosed(true);
    aRangeGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
    aRangeGroup->AddPrimitiveArray(aPosRangeArray);
  }
  {
    occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(1);
    aPoints->AddVertex(aLightPos);
    occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
    aGroup->SetGroupPrimitivesAspect(myDrawer->PointAspect()->Aspect());
    aGroup->AddPrimitiveArray(aPoints);
  }
}

//=================================================================================================

void AIS_LightSource::computeSpot(const occ::handle<Prs3d_Presentation>& thePrs, const int theMode)
{
  // light source position and direction are set to local transformation
  const gp_Dir aLightDir = -gp::DZ();
  const gp_XYZ aLightPos = gp::Origin().XYZ();
  const double aDistance =
    (myIsZoomable && myLightSource->HasRange()) ? myLightSource->Range() : mySize;
  {
    occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(1);
    aPoints->AddVertex(aLightPos);

    occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
    aGroup->SetGroupPrimitivesAspect(myDrawer->PointAspect()->Aspect());
    aGroup->AddPrimitiveArray(aPoints);
  }

  {
    occ::handle<Graphic3d_ArrayOfSegments> aDirArray = new Graphic3d_ArrayOfSegments(2);
    aDirArray->AddVertex(aLightPos);
    aDirArray->AddVertex(gp_Pnt(aLightPos + aLightDir.XYZ() * aDistance));

    occ::handle<Graphic3d_Group> aDirGroupShadow = thePrs->NewGroup();
    aDirGroupShadow->SetClosed(true);
    aDirGroupShadow->SetGroupPrimitivesAspect(myArrowLineAspectShadow);
    aDirGroupShadow->AddPrimitiveArray(aDirArray);

    occ::handle<Graphic3d_Group> aDirGroup = thePrs->NewGroup();
    aDirGroup->SetClosed(true);
    aDirGroup->SetGroupPrimitivesAspect(myDrawer->ArrowAspect()->Aspect());
    aDirGroup->AddPrimitiveArray(aDirArray);
  }

  if (theMode == 0 && myToDisplayRange)
  {
    const float  aHalfAngle = myLightSource->Angle() / 2.0f;
    const double aRadius    = aDistance * std::tan(aHalfAngle);
    gp_Ax3       aSystem(aLightPos + aLightDir.XYZ() * aDistance, -aLightDir);
    gp_Trsf      aTrsfCone;
    aTrsfCone.SetTransformation(aSystem, gp_Ax3());
    occ::handle<Graphic3d_ArrayOfTriangles> aSpotRangeArray =
      Prs3d_ToolCylinder::Create(aRadius,
                                 0.0,
                                 aDistance,
                                 myNbSplitsQuadric,
                                 myNbSplitsQuadric,
                                 aTrsfCone);

    occ::handle<Graphic3d_Group> aRangeGroup = thePrs->NewGroup();
    aRangeGroup->SetClosed(true);
    aRangeGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
    aRangeGroup->AddPrimitiveArray(aSpotRangeArray);
  }
}

//=================================================================================================

void AIS_LightSource::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                       const int                               theMode)
{
  if (theMode != 0)
  {
    return;
  }

  occ::handle<AIS_LightSourceOwner> anEntityOwner = new AIS_LightSourceOwner(this, 15);
  {
    if (myLightSource->Type() == Graphic3d_TypeOfLightSource_Directional)
    {
      mySensSphere = new Select3D_SensitiveSphere(anEntityOwner, gp::Origin(), mySize * 0.5);
      theSel->Add(mySensSphere);
    }

    occ::handle<Select3D_SensitivePoint> aSensPosition =
      new Select3D_SensitivePoint(anEntityOwner, gp::Origin());
    aSensPosition->SetSensitivityFactor(12);
    if (!myTransformPersistence.IsNull() && myTransformPersistence->IsTrihedronOr2d())
    {
      aSensPosition->SetSensitivityFactor(std::max(12, int(mySize * 0.5)));
    }
    theSel->Add(aSensPosition);
  }
}
