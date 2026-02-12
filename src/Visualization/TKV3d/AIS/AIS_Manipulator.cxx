// Created on: 2015-12-23
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <AIS_Manipulator.hxx>

#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ManipulatorOwner.hxx>
#include <Extrema_ExtElC.hxx>
#include <gce_MakeDir.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_ToolDisk.hxx>
#include <Prs3d_ToolSector.hxx>
#include <Prs3d_ToolSphere.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <V3d_View.hxx>
class SelectMgr_EntityOwner;

#ifndef M_SQRT2
  #define M_SQRT2 1.41421356237309504880168872420969808
#endif

IMPLEMENT_STANDARD_HANDLE(AIS_Manipulator, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_Manipulator, AIS_InteractiveObject)

namespace
{
//! Return Ax1 for specified direction of Ax2.
static gp_Ax1 getAx1FromAx2Dir(const gp_Ax2& theAx2, int theIndex)
{
  switch (theIndex)
  {
    case 0:
      return gp_Ax1(theAx2.Location(), theAx2.XDirection());
    case 1:
      return gp_Ax1(theAx2.Location(), theAx2.YDirection());
    case 2:
      return theAx2.Axis();
  }
  throw Standard_ProgramError("AIS_Manipulator - Invalid axis index");
}

//! Auxiliary tool for filtering picking ray.
class ManipSensRotation
{
public:
  //! Main constructor.
  ManipSensRotation(const gp_Dir& thePlaneNormal)
      : myPlaneNormal(thePlaneNormal),
        myAngleTol(10.0 * M_PI / 180.0)
  {
  }

  //! Checks if picking ray can be used for detection.
  bool isValidRay(const SelectBasics_SelectingVolumeManager& theMgr) const
  {
    if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
    {
      return false;
    }

    const gp_Dir aRay = theMgr.GetViewRayDirection();
    return !aRay.IsNormal(myPlaneNormal, myAngleTol);
  }

private:
  gp_Dir myPlaneNormal;
  double myAngleTol;
};

//! Sensitive circle with filtering picking ray.
class ManipSensCircle : public Select3D_SensitiveCircle, public ManipSensRotation
{
public:
  //! Main constructor.
  ManipSensCircle(const occ::handle<SelectMgr_EntityOwner>& theOwnerId, const gp_Circ& theCircle)
      : Select3D_SensitiveCircle(theOwnerId, theCircle, false),
        ManipSensRotation(theCircle.Position().Direction())
  {
  }

  //! Checks whether the circle overlaps current selecting volume
  bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
               SelectBasics_PickResult&             thePickResult) override
  {
    return isValidRay(theMgr) && Select3D_SensitiveCircle::Matches(theMgr, thePickResult);
  }
};

//! Sensitive triangulation with filtering picking ray.
class ManipSensTriangulation : public Select3D_SensitiveTriangulation, public ManipSensRotation
{
public:
  ManipSensTriangulation(const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
                         const occ::handle<Poly_Triangulation>&    theTrg,
                         const gp_Dir&                             thePlaneNormal)
      : Select3D_SensitiveTriangulation(theOwnerId, theTrg, TopLoc_Location(), true),
        ManipSensRotation(thePlaneNormal)
  {
  }

  //! Checks whether the circle overlaps current selecting volume
  bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
               SelectBasics_PickResult&             thePickResult) override
  {
    return isValidRay(theMgr) && Select3D_SensitiveTriangulation::Matches(theMgr, thePickResult);
  }
};
} // namespace

//=================================================================================================

void AIS_Manipulator::init()
{
  // Create axis in the default coordinate system. The custom position is applied in local
  // transformation.
  myAxes[0] = Axis(gp::OX(), Quantity_NOC_RED);
  myAxes[1] = Axis(gp::OY(), Quantity_NOC_GREEN);
  myAxes[2] = Axis(gp::OZ(), Quantity_NOC_BLUE1);

  Graphic3d_MaterialAspect aShadingMaterial;
  aShadingMaterial.SetSpecularColor(Quantity_NOC_BLACK);
  aShadingMaterial.SetMaterialType(Graphic3d_MATERIAL_ASPECT);

  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
  myDrawer->ShadingAspect()->SetColor(Quantity_NOC_WHITE);
  myDrawer->ShadingAspect()->SetMaterial(aShadingMaterial);

  myHighlightAspect = new Prs3d_ShadingAspect();
  myHighlightAspect->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
  myHighlightAspect->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myHighlightAspect->SetColor(Quantity_NOC_AZURE);

  myDraggerHighlight = new Prs3d_ShadingAspect();
  myDraggerHighlight->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
  myDraggerHighlight->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myDraggerHighlight->SetTransparency(0.5);

  SetSize(100);
  SetZLayer(Graphic3d_ZLayerId_Topmost);
}

//=================================================================================================

occ::handle<Prs3d_Presentation> AIS_Manipulator::getHighlightPresentation(
  const occ::handle<SelectMgr_EntityOwner>& theOwner) const
{
  occ::handle<Prs3d_Presentation>   aDummyPrs;
  occ::handle<AIS_ManipulatorOwner> anOwner = occ::down_cast<AIS_ManipulatorOwner>(theOwner);
  if (anOwner.IsNull())
  {
    return aDummyPrs;
  }

  switch (anOwner->Mode())
  {
    case AIS_MM_Translation:
      return myAxes[anOwner->Index()].TranslatorHighlightPrs();
    case AIS_MM_Rotation:
      return myAxes[anOwner->Index()].RotatorHighlightPrs();
    case AIS_MM_Scaling:
      return myAxes[anOwner->Index()].ScalerHighlightPrs();
    case AIS_MM_TranslationPlane:
      return myAxes[anOwner->Index()].DraggerHighlightPrs();
    case AIS_MM_None:
      break;
  }

  return aDummyPrs;
}

//=================================================================================================

occ::handle<Graphic3d_Group> AIS_Manipulator::getGroup(const int                 theIndex,
                                                       const AIS_ManipulatorMode theMode) const
{
  occ::handle<Graphic3d_Group> aDummyGroup;

  if (theIndex < 0 || theIndex > 2)
  {
    return aDummyGroup;
  }

  switch (theMode)
  {
    case AIS_MM_Translation:
      return myAxes[theIndex].TranslatorGroup();
    case AIS_MM_Rotation:
      return myAxes[theIndex].RotatorGroup();
    case AIS_MM_Scaling:
      return myAxes[theIndex].ScalerGroup();
    case AIS_MM_TranslationPlane:
      return myAxes[theIndex].DraggerGroup();
    case AIS_MM_None:
      break;
  }

  return aDummyGroup;
}

//=================================================================================================

AIS_Manipulator::AIS_Manipulator()
    : myPosition(gp::XOY()),
      myCurrentIndex(-1),
      myCurrentMode(AIS_MM_None),
      mySkinMode(ManipulatorSkin_Shaded),
      myIsActivationOnDetection(false),
      myIsZoomPersistentMode(true),
      myHasStartedTransformation(false),
      myStartPosition(gp::XOY()),
      myStartPick(0.0, 0.0, 0.0),
      myPrevState(0.0)
{
  SetInfiniteState();
  SetMutable(true);
  SetDisplayMode(AIS_Shaded);
  init();
}

//=================================================================================================

AIS_Manipulator::AIS_Manipulator(const gp_Ax2& thePosition)
    : myPosition(thePosition),
      myCurrentIndex(-1),
      myCurrentMode(AIS_MM_None),
      mySkinMode(ManipulatorSkin_Shaded),
      myIsActivationOnDetection(false),
      myIsZoomPersistentMode(true),
      myHasStartedTransformation(false),
      myStartPosition(gp::XOY()),
      myStartPick(0.0, 0.0, 0.0),
      myPrevState(0.0)
{
  SetInfiniteState();
  SetMutable(true);
  SetDisplayMode(AIS_Shaded);
  init();
}

//=================================================================================================

void AIS_Manipulator::SetPart(const int                 theAxisIndex,
                              const AIS_ManipulatorMode theMode,
                              const bool                theIsEnabled)
{
  Standard_ProgramError_Raise_if(
    theAxisIndex < 0 || theAxisIndex > 2,
    "AIS_Manipulator::SetMode(): axis index should be between 0 and 2");
  switch (theMode)
  {
    case AIS_MM_Translation:
      myAxes[theAxisIndex].SetTranslation(theIsEnabled);
      break;

    case AIS_MM_Rotation:
      myAxes[theAxisIndex].SetRotation(theIsEnabled);
      break;

    case AIS_MM_Scaling:
      myAxes[theAxisIndex].SetScaling(theIsEnabled);
      break;

    case AIS_MM_TranslationPlane:
      myAxes[theAxisIndex].SetDragging(theIsEnabled);
      break;

    case AIS_MM_None:
      break;
  }
}

//=================================================================================================

void AIS_Manipulator::SetPart(const AIS_ManipulatorMode theMode, const bool theIsEnabled)
{
  for (int anIt = 0; anIt < 3; ++anIt)
  {
    SetPart(anIt, theMode, theIsEnabled);
  }
}

//=================================================================================================

void AIS_Manipulator::EnableMode(const AIS_ManipulatorMode theMode)
{
  if (!IsAttached())
  {
    return;
  }

  const occ::handle<AIS_InteractiveContext>& aContext = GetContext();
  if (aContext.IsNull())
  {
    return;
  }

  aContext->Activate(this, theMode);
}

//=================================================================================================

void AIS_Manipulator::attachToPoint(const gp_Pnt& thePoint)
{
  gp_Ax2 aPosition = gp::XOY();
  aPosition.SetLocation(thePoint);
  SetPosition(aPosition);
}

//=================================================================================================

void AIS_Manipulator::attachToBox(const Bnd_Box& theBox)
{
  if (theBox.IsVoid())
  {
    return;
  }

  double anXmin = 0.0, anYmin = 0.0, aZmin = 0.0, anXmax = 0.0, anYmax = 0.0, aZmax = 0.0;
  theBox.Get(anXmin, anYmin, aZmin, anXmax, anYmax, aZmax);

  gp_Ax2 aPosition = gp::XOY();
  aPosition.SetLocation(
    gp_Pnt((anXmin + anXmax) * 0.5, (anYmin + anYmax) * 0.5, (aZmin + aZmax) * 0.5));
  SetPosition(aPosition);
}

//=================================================================================================

void AIS_Manipulator::adjustSize(const Bnd_Box& theBox)
{
  double aXmin = 0., aYmin = 0., aZmin = 0., aXmax = 0., aYmax = 0., aZmax = 0.0;
  theBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  double aXSize = aXmax - aXmin;
  double aYSize = aYmax - aYmin;
  double aZSize = aZmax - aZmin;

  SetSize((float)(std::max(aXSize, std::max(aYSize, aZSize)) * 0.5));
}

//=================================================================================================

void AIS_Manipulator::Attach(const occ::handle<AIS_InteractiveObject>& theObject,
                             const OptionsForAttach&                   theOptions)
{
  if (theObject->IsKind(STANDARD_TYPE(AIS_Manipulator)))
  {
    return;
  }

  occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>> aSeq =
    new NCollection_HSequence<occ::handle<AIS_InteractiveObject>>();
  aSeq->Append(theObject);
  Attach(aSeq, theOptions);
}

//=================================================================================================

void AIS_Manipulator::Attach(
  const occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>>& theObjects,
  const OptionsForAttach&                                                       theOptions)
{
  if (theObjects->Size() < 1)
  {
    return;
  }

  SetOwner(theObjects);
  Bnd_Box                                   aBox;
  const occ::handle<AIS_InteractiveObject>& aCurObject = theObjects->Value(theObjects->Lower());
  aCurObject->BoundingBox(aBox);

  if (theOptions.AdjustPosition)
  {
    const occ::handle<Graphic3d_TransformPers>& aTransPers = aCurObject->TransformPersistence();
    if (!aTransPers.IsNull() && (aTransPers->IsZoomOrRotate() || aTransPers->IsAxial()))
    {
      attachToPoint(aTransPers->AnchorPoint());
    }
    else
    {
      attachToBox(aBox);
    }
  }

  if (theOptions.AdjustSize)
  {
    adjustSize(aBox);
  }

  const occ::handle<AIS_InteractiveContext>& aContext = Object()->GetContext();
  if (!aContext.IsNull())
  {
    if (!aContext->IsDisplayed(this))
    {
      aContext->Display(this, false);
    }
    else
    {
      aContext->Update(this, false);
      aContext->RecomputeSelectionOnly(this);
    }

    aContext->Load(this);
  }

  if (theOptions.EnableModes)
  {
    EnableMode(AIS_MM_Rotation);
    EnableMode(AIS_MM_Translation);
    EnableMode(AIS_MM_Scaling);
    EnableMode(AIS_MM_TranslationPlane);
  }
}

//=================================================================================================

void AIS_Manipulator::Detach()
{
  DeactivateCurrentMode();

  if (!IsAttached())
  {
    return;
  }

  occ::handle<AIS_InteractiveObject>         anObject = Object();
  const occ::handle<AIS_InteractiveContext>& aContext = anObject->GetContext();
  if (!aContext.IsNull())
  {
    aContext->Remove(this, false);
  }

  SetOwner(nullptr);
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>> AIS_Manipulator::Objects()
  const
{
  return occ::down_cast<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>>(GetOwner());
}

//=================================================================================================

occ::handle<AIS_InteractiveObject> AIS_Manipulator::Object(const int theIndex) const
{
  occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>> anOwner =
    occ::down_cast<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>>(GetOwner());

  Standard_ProgramError_Raise_if(theIndex < anOwner->Lower() || theIndex > anOwner->Upper(),
                                 "AIS_Manipulator::Object(): wrong index value");

  if (anOwner.IsNull() || anOwner->IsEmpty())
  {
    return nullptr;
  }

  return anOwner->Value(theIndex);
}

//=================================================================================================

occ::handle<AIS_InteractiveObject> AIS_Manipulator::Object() const
{
  return Object(1);
}

//=================================================================================================

bool AIS_Manipulator::ObjectTransformation(const int                    theMaxX,
                                           const int                    theMaxY,
                                           const occ::handle<V3d_View>& theView,
                                           gp_Trsf&                     theTrsf)
{
  // Initialize start reference data
  if (!myHasStartedTransformation)
  {
    myStartTrsfs.Clear();
    occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>> anObjects = Objects();
    for (NCollection_HSequence<occ::handle<AIS_InteractiveObject>>::Iterator anObjIter(*anObjects);
         anObjIter.More();
         anObjIter.Next())
    {
      myStartTrsfs.Append(anObjIter.Value()->LocalTransformation());
    }
    myStartPosition = myPosition;
  }

  // Get 3d point with projection vector
  NCollection_Vec3<double> anInputPoint, aProj;
  theView->ConvertWithProj(theMaxX,
                           theMaxY,
                           anInputPoint.x(),
                           anInputPoint.y(),
                           anInputPoint.z(),
                           aProj.x(),
                           aProj.y(),
                           aProj.z());
  const gp_Lin anInputLine(gp_Pnt(anInputPoint.x(), anInputPoint.y(), anInputPoint.z()),
                           gp_Dir(aProj.x(), aProj.y(), aProj.z()));
  switch (myCurrentMode)
  {
    case AIS_MM_Translation:
    case AIS_MM_Scaling: {
      const gp_Lin aLine(myStartPosition.Location(), myAxes[myCurrentIndex].Position().Direction());
      Extrema_ExtElC anExtrema(anInputLine, aLine, Precision::Angular());
      if (!anExtrema.IsDone() || anExtrema.IsParallel() || anExtrema.NbExt() != 1)
      {
        // translation cannot be done co-directed with camera
        return false;
      }

      Extrema_POnCurv anExPnts[2];
      anExtrema.Points(1, anExPnts[0], anExPnts[1]);
      const gp_Pnt aNewPosition = anExPnts[1].Value();
      if (!myHasStartedTransformation)
      {
        myStartPick                = aNewPosition;
        myHasStartedTransformation = true;
        return true;
      }
      else if (aNewPosition.Distance(myStartPick) < Precision::Confusion())
      {
        return false;
      }

      gp_Trsf aNewTrsf;
      if (myCurrentMode == AIS_MM_Translation)
      {
        aNewTrsf.SetTranslation(gp_Vec(myStartPick, aNewPosition));
        theTrsf *= aNewTrsf;
      }
      else if (myCurrentMode == AIS_MM_Scaling)
      {
        if (aNewPosition.Distance(myStartPosition.Location()) < Precision::Confusion())
        {
          return false;
        }

        double aCoeff = myStartPosition.Location().Distance(aNewPosition)
                        / myStartPosition.Location().Distance(myStartPick);
        aNewTrsf.SetScale(myPosition.Location(), aCoeff);
        theTrsf = aNewTrsf;
      }
      return true;
    }
    case AIS_MM_Rotation: {
      const gp_Pnt        aPosLoc   = myStartPosition.Location();
      const gp_Ax1        aCurrAxis = getAx1FromAx2Dir(myStartPosition, myCurrentIndex);
      IntAna_IntConicQuad aIntersector(anInputLine,
                                       gp_Pln(aPosLoc, aCurrAxis.Direction()),
                                       Precision::Angular(),
                                       Precision::Intersection());
      if (!aIntersector.IsDone() || aIntersector.IsParallel() || aIntersector.NbPoints() < 1)
      {
        return false;
      }

      const gp_Pnt aNewPosition = aIntersector.Point(1);
      if (!myHasStartedTransformation)
      {
        myStartPick                = aNewPosition;
        myHasStartedTransformation = true;
        gp_Dir aStartAxis          = gce_MakeDir(aPosLoc, myStartPick);
        myPrevState =
          aStartAxis.AngleWithRef(gce_MakeDir(aPosLoc, aNewPosition), aCurrAxis.Direction());
        return true;
      }

      if (aNewPosition.Distance(myStartPick) < Precision::Confusion())
      {
        return false;
      }

      gp_Dir aStartAxis =
        aPosLoc.IsEqual(myStartPick, Precision::Confusion())
          ? getAx1FromAx2Dir(myStartPosition, (myCurrentIndex + 1) % 3).Direction()
          : gce_MakeDir(aPosLoc, myStartPick);

      gp_Dir aCurrentAxis = gce_MakeDir(aPosLoc, aNewPosition);
      double anAngle      = aStartAxis.AngleWithRef(aCurrentAxis, aCurrAxis.Direction());

      if (std::abs(anAngle) < Precision::Confusion())
      {
        return false;
      }

      // Draw a sector indicating the rotation angle
      if (mySkinMode == ManipulatorSkin_Flat)
      {
        const gp_Ax1& anAxis = myAxes[myCurrentIndex].ReferenceAxis();
        const gp_Dir  aRotationStart =
          anAxis.Direction().Z() > 0 ? myStartPosition.YDirection() : myStartPosition.Direction();
        double aRotationAngle = aRotationStart.AngleWithRef(aCurrentAxis, aCurrAxis.Direction());
        aRotationAngle -= (anAngle > 0) ? anAngle * 2.0 : anAngle;
        if (anAxis.Direction().Z() > 0)
        {
          aRotationAngle += M_PI_2;
        }

        gp_Trsf aTrsf;
        aTrsf.SetRotation(anAxis, aRotationAngle);

        occ::handle<Prs3d_ShadingAspect> anAspect = new Prs3d_ShadingAspect();
        anAspect->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
        anAspect->SetMaterial(myDrawer->ShadingAspect()->Material());
        anAspect->SetTransparency(0.5);
        anAspect->SetColor(myAxes[myCurrentIndex].Color());

        mySector.Init(0.0f, myAxes[myCurrentIndex].InnerRadius(), anAxis, std::abs(anAngle));
        mySectorGroup->Clear();
        mySectorGroup->SetPrimitivesAspect(anAspect->Aspect());
        mySectorGroup->AddPrimitiveArray(mySector.Array());
        mySectorGroup->SetTransformation(aTrsf);
      }

      // Change value of an angle if it should have different sign.
      if (anAngle * myPrevState < 0 && std::abs(anAngle) < M_PI_2)
      {
        double aSign = myPrevState > 0 ? -1.0 : 1.0;
        anAngle      = aSign * (M_PI * 2 - anAngle);
      }

      gp_Trsf aNewTrsf;
      aNewTrsf.SetRotation(aCurrAxis, anAngle);
      theTrsf *= aNewTrsf;
      myPrevState = anAngle;
      return true;
    }
    case AIS_MM_TranslationPlane: {
      const gp_Pnt        aPosLoc   = myStartPosition.Location();
      const gp_Ax1        aCurrAxis = getAx1FromAx2Dir(myStartPosition, myCurrentIndex);
      IntAna_IntConicQuad aIntersector(anInputLine,
                                       gp_Pln(aPosLoc, aCurrAxis.Direction()),
                                       Precision::Angular(),
                                       Precision::Intersection());
      if (!aIntersector.IsDone() || aIntersector.NbPoints() < 1)
      {
        return false;
      }

      const gp_Pnt aNewPosition = aIntersector.Point(1);
      if (!myHasStartedTransformation)
      {
        myStartPick                = aNewPosition;
        myHasStartedTransformation = true;
        return true;
      }

      if (aNewPosition.Distance(myStartPick) < Precision::Confusion())
      {
        return false;
      }

      gp_Trsf aNewTrsf;
      aNewTrsf.SetTranslation(gp_Vec(myStartPick, aNewPosition));
      theTrsf *= aNewTrsf;
      return true;
    }
    case AIS_MM_None: {
      return false;
    }
  }
  return false;
}

//=================================================================================================

bool AIS_Manipulator::ProcessDragging(const occ::handle<AIS_InteractiveContext>& aCtx,
                                      const occ::handle<V3d_View>&               theView,
                                      const occ::handle<SelectMgr_EntityOwner>&,
                                      const NCollection_Vec2<int>& theDragFrom,
                                      const NCollection_Vec2<int>& theDragTo,
                                      const AIS_DragAction         theAction)
{
  switch (theAction)
  {
    case AIS_DragAction_Start: {
      if (HasActiveMode())
      {
        StartTransform(theDragFrom.x(), theDragFrom.y(), theView);
        return true;
      }
      break;
    }
    case AIS_DragAction_Confirmed: {
      return true;
    }
    case AIS_DragAction_Update: {
      Transform(theDragTo.x(), theDragTo.y(), theView);
      return true;
    }
    case AIS_DragAction_Abort: {
      StopTransform(false);
      return true;
    }
    case AIS_DragAction_Stop: {
      StopTransform(true);
      if (mySkinMode == ManipulatorSkin_Flat)
      {
        mySectorGroup->Clear();
      }
      else if (aCtx->IsDisplayed(this))
      {
        // at the end of transformation redisplay for updating sensitive areas
        aCtx->Redisplay(this, true);
      }
      return true;
    }
    break;
  }
  return false;
}

//=================================================================================================

void AIS_Manipulator::StartTransform(const int                    theX,
                                     const int                    theY,
                                     const occ::handle<V3d_View>& theView)
{
  if (myHasStartedTransformation)
  {
    return;
  }

  gp_Trsf aTrsf;
  ObjectTransformation(theX, theY, theView, aTrsf);
}

//=================================================================================================

void AIS_Manipulator::StopTransform(const bool theToApply)
{
  if (!IsAttached() || !myHasStartedTransformation)
  {
    return;
  }

  myHasStartedTransformation = false;
  if (theToApply)
  {
    return;
  }

  occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>> anObjects = Objects();
  NCollection_HSequence<occ::handle<AIS_InteractiveObject>>::Iterator    anObjIter(*anObjects);
  NCollection_Sequence<gp_Trsf>::Iterator                                aTrsfIter(myStartTrsfs);
  for (; anObjIter.More(); anObjIter.Next(), aTrsfIter.Next())
  {
    anObjIter.ChangeValue()->SetLocalTransformation(aTrsfIter.Value());
  }
  SetPosition(myStartPosition);
}

//=================================================================================================

void AIS_Manipulator::RecomputeTransformation(const occ::handle<Graphic3d_Camera>& theCamera)
{
  if (mySkinMode == ManipulatorSkin_Shaded)
  {
    return;
  }

  bool isRecomputedTranslation = false;
  bool isRecomputedRotation    = false;
  bool isRecomputedDragging    = false;
  bool isRecomputedScaling     = false;

  // Remove transformation from dragger group
  for (int anIt = 0; anIt < 3; ++anIt)
  {
    if (myAxes[anIt].HasDragging())
    {
      myAxes[anIt].DraggerGroup()->SetTransformation(gp_Trsf());
      isRecomputedDragging = true;
    }
  }

  const gp_Dir& aCameraDir = theCamera->Direction();
  for (int anIt = 0; anIt < 3; ++anIt)
  {
    Axis&         anAxis   = myAxes[anIt];
    const gp_Ax1& aRefAxis = anAxis.ReferenceAxis();
    gp_Dir        anAxisDir, aNormal;

    if (aRefAxis.Direction().X() > 0)
    {
      aNormal   = myPosition.YDirection().Reversed();
      anAxisDir = myPosition.XDirection();
    }
    else if (aRefAxis.Direction().Y() > 0)
    {
      aNormal   = myPosition.XDirection().Crossed(myPosition.YDirection()).Reversed();
      anAxisDir = myPosition.YDirection();
    }
    else
    {
      aNormal   = myPosition.XDirection().Reversed();
      anAxisDir = myPosition.XDirection().Crossed(myPosition.YDirection());
    }

    const gp_Dir aCameraProj =
      std::abs(std::abs(anAxisDir.Dot(aCameraDir)) - 1.0) <= gp::Resolution()
        ? aCameraDir
        : anAxisDir.Crossed(aCameraDir).Crossed(anAxisDir);
    const bool isReversed = anAxisDir.Dot(aCameraDir) > 0;
    double     anAngle    = aNormal.AngleWithRef(aCameraProj, anAxisDir);
    if (aRefAxis.Direction().X() > 0)
      anAngle -= M_PI_2;

    if (anAxis.HasTranslation())
    {
      occ::handle<Prs3d_ShadingAspect> anAspect = new Prs3d_ShadingAspect();
      anAspect->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);

      Quantity_Color aColor =
        isReversed ? Quantity_Color(anAxis.Color().Rgb() * 0.1f) : anAxis.Color();
      anAspect->Aspect()->SetInteriorColor(aColor);

      gp_Trsf aTranslatorTrsf;
      aTranslatorTrsf.SetRotation(aRefAxis, anAngle);
      if (isReversed)
      {
        const double aLength = anAxis.AxisLength() + anAxis.Indent() * 4.0f;
        aTranslatorTrsf.SetTranslationPart(aRefAxis.Direction().XYZ().Reversed() * aLength);
      }

      anAxis.TranslatorGroup()->SetGroupPrimitivesAspect(anAspect->Aspect());
      anAxis.TranslatorGroup()->SetTransformation(aTranslatorTrsf);
      anAxis.TranslatorHighlightPrs()->CurrentGroup()->SetTransformation(aTranslatorTrsf);
      isRecomputedTranslation = true;
    }

    if (anAxis.HasRotation())
    {
      gp_Trsf aRotatorTrsf;
      aRotatorTrsf.SetRotation(aRefAxis, anAngle - M_PI_2);
      anAxis.RotatorGroup()->SetTransformation(aRotatorTrsf);
      anAxis.RotatorHighlightPrs()->CurrentGroup()->SetTransformation(aRotatorTrsf);
      isRecomputedRotation = true;
    }

    if (anAxis.HasDragging() && isReversed)
    {
      for (int anIndexIter = 0; anIndexIter < 3; ++anIndexIter)
      {
        gp_Vec aTranslation =
          (anIndexIter == anIt)
            ? aRefAxis.Direction().XYZ() * myAxes[anIndexIter].AxisRadius() * 2.0f
            : aRefAxis.Direction().XYZ().Reversed() * myAxes[anIndexIter].AxisLength();
        gp_Trsf aDraggerTrsf;
        aDraggerTrsf.SetTranslation(aTranslation);

        const occ::handle<Graphic3d_Group>& aDraggerGroup = myAxes[anIndexIter].DraggerGroup();
        aDraggerTrsf *= aDraggerGroup->Transformation();
        aDraggerGroup->SetTransformation(aDraggerTrsf);
      }
    }

    if (anAxis.HasScaling())
    {
      gp_Trsf aScalerTrsf;
      if (aRefAxis.Direction().X() > 0)
      {
        anAngle += M_PI_2;
      }
      aScalerTrsf.SetRotation(aRefAxis, anAngle);
      if (isReversed)
      {
        float aLength = anAxis.AxisLength() * 2.0f + anAxis.BoxSize() + anAxis.Indent() * 4.0f;
        aScalerTrsf.SetTranslationPart(gp_Vec(aRefAxis.Direction().XYZ().Reversed() * aLength));
      }
      anAxis.ScalerGroup()->SetTransformation(aScalerTrsf);
      anAxis.ScalerHighlightPrs()->CurrentGroup()->SetTransformation(aScalerTrsf);
      isRecomputedScaling = true;
    }
  }

  if (isRecomputedRotation)
  {
    const gp_Dir aXDir  = gp::DX();
    const gp_Dir anYDir = gp::DY();
    const gp_Dir aZDir  = gp::DZ();

    const gp_Dir aCameraProjection =
      std::abs(aXDir.Dot(aCameraDir)) <= gp::Resolution()
          || std::abs(anYDir.Dot(aCameraDir)) <= gp::Resolution()
        ? aCameraDir
        : aXDir.XYZ() * (aXDir.Dot(aCameraDir)) + anYDir.XYZ() * (anYDir.Dot(aCameraDir));
    const bool isReversed = aZDir.Dot(aCameraDir) > 0;

    const double anAngle = M_PI_2 - aCameraDir.Angle(aCameraProjection);
    gp_Dir aRotAxis = std::abs(std::abs(aCameraProjection.Dot(aZDir)) - 1.0) <= gp::Resolution()
                        ? aZDir
                        : aCameraProjection.Crossed(aZDir);
    if (isReversed)
    {
      aRotAxis.Reverse();
    }

    gp_Trsf aRotationTrsf;
    aRotationTrsf.SetRotation(gp_Ax1(gp::Origin(), aRotAxis), anAngle);

    gp_Ax3 aToSystem(gp::Origin(),
                     myPosition.XDirection().Crossed(myPosition.YDirection()),
                     myPosition.XDirection());
    gp_Ax3 aFromSystem(gp::XOY());
    aFromSystem.Transform(aRotationTrsf);

    gp_Trsf aTrsf;
    aTrsf.SetTransformation(aFromSystem, aToSystem);
    myCircleGroup->SetTransformation(aTrsf);
  }

  if (isRecomputedDragging)
  {
    for (int anIt = 0; anIt < 3; ++anIt)
    {
      myAxes[anIt].DraggerHighlightPrs()->CurrentGroup()->SetTransformation(
        myAxes[anIt].DraggerGroup()->Transformation());
    }
  }

  if (isRecomputedTranslation)
  {
    RecomputeSelection(AIS_MM_Translation);
  };
  if (isRecomputedRotation)
  {
    RecomputeSelection(AIS_MM_Rotation);
  };
  if (isRecomputedDragging)
  {
    RecomputeSelection(AIS_MM_TranslationPlane);
  };
  if (isRecomputedScaling)
  {
    RecomputeSelection(AIS_MM_Scaling);
  };

  Object()->GetContext()->RecomputeSelectionOnly(this);
}

//=================================================================================================

void AIS_Manipulator::Transform(const gp_Trsf& theTrsf)
{
  if (!IsAttached() || !myHasStartedTransformation)
  {
    return;
  }

  {
    occ::handle<NCollection_HSequence<occ::handle<AIS_InteractiveObject>>> anObjects = Objects();
    NCollection_HSequence<occ::handle<AIS_InteractiveObject>>::Iterator    anObjIter(*anObjects);
    NCollection_Sequence<gp_Trsf>::Iterator                                aTrsfIter(myStartTrsfs);
    for (; anObjIter.More(); anObjIter.Next(), aTrsfIter.Next())
    {
      const occ::handle<AIS_InteractiveObject>&   anObj      = anObjIter.ChangeValue();
      const occ::handle<Graphic3d_TransformPers>& aTransPers = anObj->TransformPersistence();
      if (!aTransPers.IsNull() && (aTransPers->IsZoomOrRotate() || aTransPers->IsAxial()))
      {
        gp_XYZ aNewAnchorPoint = aTransPers->AnchorPoint().XYZ() - myPosition.Location().XYZ();
        aNewAnchorPoint += myStartPosition.Location().Transformed(theTrsf).XYZ();
        aTransPers->SetAnchorPoint(aNewAnchorPoint);
        continue;
      }

      const gp_Trsf&                     anOldTrsf   = aTrsfIter.Value();
      const occ::handle<TopLoc_Datum3D>& aParentTrsf = anObj->CombinedParentTransformation();
      if (!aParentTrsf.IsNull() && aParentTrsf->Form() != gp_Identity)
      {
        // recompute local transformation relative to parent transformation
        const gp_Trsf aNewLocalTrsf =
          aParentTrsf->Trsf().Inverted() * theTrsf * aParentTrsf->Trsf() * anOldTrsf;
        anObj->SetLocalTransformation(aNewLocalTrsf);
      }
      else
      {
        anObj->SetLocalTransformation(theTrsf * anOldTrsf);
      }
    }
  }

  if ((myCurrentMode == AIS_MM_Translation && myBehaviorOnTransform.FollowTranslation)
      || (myCurrentMode == AIS_MM_Rotation && myBehaviorOnTransform.FollowRotation)
      || (myCurrentMode == AIS_MM_TranslationPlane && myBehaviorOnTransform.FollowDragging))
  {
    gp_Pnt aPos  = myStartPosition.Location().Transformed(theTrsf);
    gp_Dir aVDir = myStartPosition.Direction().Transformed(theTrsf);
    gp_Dir aXDir = myStartPosition.XDirection().Transformed(theTrsf);
    SetPosition(gp_Ax2(aPos, aVDir, aXDir));
  }
}

//=================================================================================================

gp_Trsf AIS_Manipulator::Transform(const int                    thePX,
                                   const int                    thePY,
                                   const occ::handle<V3d_View>& theView)
{
  gp_Trsf aTrsf;
  if (ObjectTransformation(thePX, thePY, theView, aTrsf))
  {
    Transform(aTrsf);
  }

  return aTrsf;
}

//=================================================================================================

void AIS_Manipulator::SetPosition(const gp_Ax2& thePosition)
{
  if (!myPosition.Location().IsEqual(thePosition.Location(), Precision::Confusion())
      || !myPosition.Direction().IsEqual(thePosition.Direction(), Precision::Angular())
      || !myPosition.XDirection().IsEqual(thePosition.XDirection(), Precision::Angular()))
  {
    myPosition = thePosition;
    myAxes[0].SetPosition(getAx1FromAx2Dir(thePosition, 0));
    myAxes[1].SetPosition(getAx1FromAx2Dir(thePosition, 1));
    myAxes[2].SetPosition(getAx1FromAx2Dir(thePosition, 2));
    updateTransformation();
  }
}

//=======================================================================
// function : updateTransformation
// purpose  : set local transformation to avoid graphics recomputation
//=======================================================================
void AIS_Manipulator::updateTransformation()
{
  gp_Trsf aTrsf;

  if (!myIsZoomPersistentMode)
  {
    aTrsf.SetTransformation(myPosition, gp::XOY());
  }
  else
  {
    const gp_Dir& aVDir = myPosition.Direction();
    const gp_Dir& aXDir = myPosition.XDirection();
    aTrsf.SetTransformation(gp_Ax2(gp::Origin(), aVDir, aXDir), gp::XOY());
  }

  occ::handle<TopLoc_Datum3D> aGeomTrsf = new TopLoc_Datum3D(aTrsf);
  // we explicitly call here setLocalTransformation() of the base class
  // since AIS_Manipulator::setLocalTransformation() implementation throws exception
  // as protection from external calls
  AIS_InteractiveObject::setLocalTransformation(aGeomTrsf);
  for (int anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].Transform(aGeomTrsf);
  }

  if (myIsZoomPersistentMode)
  {
    if (TransformPersistence().IsNull()
        || TransformPersistence()->Mode() != Graphic3d_TMF_AxialZoomPers
        || !TransformPersistence()->AnchorPoint().IsEqual(myPosition.Location(), 0.0))
    {
      setTransformPersistence(
        new Graphic3d_TransformPers(Graphic3d_TMF_AxialZoomPers, myPosition.Location()));
    }
  }
  else
  {
    if (TransformPersistence().IsNull()
        || TransformPersistence()->Mode() != Graphic3d_TMF_AxialScalePers)
    {
      setTransformPersistence(
        new Graphic3d_TransformPers(Graphic3d_TMF_AxialScalePers, myPosition.Location()));
    }
  }
}

//=================================================================================================

void AIS_Manipulator::SetSize(const float theSideLength)
{
  for (int anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].SetSize(theSideLength);
  }

  SetToUpdate();
}

//=================================================================================================

void AIS_Manipulator::SetGap(const float theValue)
{
  for (int anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].SetIndent(theValue);
  }

  SetToUpdate();
}

//=================================================================================================

void AIS_Manipulator::DeactivateCurrentMode()
{
  if (!myIsActivationOnDetection)
  {
    occ::handle<Graphic3d_Group> aGroup = getGroup(myCurrentIndex, myCurrentMode);
    if (aGroup.IsNull())
    {
      return;
    }

    occ::handle<Prs3d_ShadingAspect> anAspect = new Prs3d_ShadingAspect();
    if (mySkinMode == ManipulatorSkin_Flat)
    {
      anAspect->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
    }
    anAspect->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
    anAspect->SetMaterial(myDrawer->ShadingAspect()->Material());
    if (myCurrentMode == AIS_MM_TranslationPlane)
      anAspect->SetTransparency(1.0);
    else
    {
      anAspect->SetTransparency(myDrawer->ShadingAspect()->Transparency());
      anAspect->SetColor(myAxes[myCurrentIndex].Color());
    }

    aGroup->SetGroupPrimitivesAspect(anAspect->Aspect());
  }

  myCurrentIndex = -1;
  myCurrentMode  = AIS_MM_None;

  if (myHasStartedTransformation)
  {
    myHasStartedTransformation = false;
  }
}

//=================================================================================================

void AIS_Manipulator::SetZoomPersistence(const bool theToEnable)
{
  if (myIsZoomPersistentMode != theToEnable)
  {
    SetToUpdate();
  }

  myIsZoomPersistentMode = theToEnable;

  if (!theToEnable)
  {
    setTransformPersistence(new (Graphic3d_TransformPers)(Graphic3d_TMF_AxialScalePers));
  }

  updateTransformation();
}

//=================================================================================================

void AIS_Manipulator::SetSkinMode(const ManipulatorSkin theSkinMode)
{
  if (mySkinMode != theSkinMode)
  {
    SetToUpdate();
  }
  mySkinMode = theSkinMode;
}

//=================================================================================================

void AIS_Manipulator::SetTransformPersistence(
  const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
{
  Standard_ASSERT_RETURN(!myIsZoomPersistentMode,
                         "AIS_Manipulator::SetTransformPersistence: "
                         "Custom settings are not allowed by this class in ZoomPersistence mode",
                         Standard_VOID_RETURN);

  setTransformPersistence(theTrsfPers);
}

//=================================================================================================

void AIS_Manipulator::setTransformPersistence(
  const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
{
  AIS_InteractiveObject::SetTransformPersistence(theTrsfPers);

  for (int anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].SetTransformPersistence(theTrsfPers);
  }
}

//=================================================================================================

void AIS_Manipulator::setLocalTransformation(const occ::handle<TopLoc_Datum3D>& /*theTrsf*/)
{
  Standard_ASSERT_INVOKE("AIS_Manipulator::setLocalTransformation: "
                         "Custom transformation is not supported by this class");
}

//=================================================================================================

void AIS_Manipulator::Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                              const occ::handle<Prs3d_Presentation>&         thePrs,
                              const int                                      theMode)
{
  if (theMode != AIS_Shaded)
  {
    return;
  }

  thePrs->SetInfiniteState(true);
  thePrs->SetMutable(true);
  occ::handle<Graphic3d_Group>     aGroup;
  occ::handle<Prs3d_ShadingAspect> anAspect = new Prs3d_ShadingAspect();
  if (mySkinMode == ManipulatorSkin_Flat)
  {
    anAspect->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  }
  anAspect->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
  anAspect->SetMaterial(myDrawer->ShadingAspect()->Material());
  anAspect->SetTransparency(myDrawer->ShadingAspect()->Transparency());

  // Display center
  myCenter.Init(myAxes[0].AxisRadius() * 2.0f, gp::Origin(), mySkinMode);
  aGroup = thePrs->NewGroup();
  aGroup->SetPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
  aGroup->AddPrimitiveArray(myCenter.Array());

  // Display outer circle
  if (mySkinMode == ManipulatorSkin_Flat
      && (myAxes[0].HasRotation() || myAxes[1].HasRotation() || myAxes[2].HasRotation()))
  {
    myCircle.Init(myAxes[0].InnerRadius(),
                  myAxes[0].Size(),
                  gp_Ax1(gp::Origin(), gp::DZ()),
                  2.0f * M_PI,
                  myAxes[0].FacettesNumber() * 4);
    myCircleGroup = thePrs->NewGroup();
    myCircleGroup->SetPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
    myCircleGroup->AddPrimitiveArray(myCircle.Array());

    mySectorGroup = thePrs->NewGroup();
    mySectorGroup->SetGroupPrimitivesAspect(anAspect->Aspect());
  }

  for (int anIt = 0; anIt < 3; ++anIt)
  {
    // Display axes
    aGroup = thePrs->NewGroup();

    occ::handle<Prs3d_ShadingAspect> anAspectAx =
      new Prs3d_ShadingAspect(new Graphic3d_AspectFillArea3d(*anAspect->Aspect()));
    anAspectAx->SetColor(myAxes[anIt].Color());
    aGroup->SetGroupPrimitivesAspect(anAspectAx->Aspect());
    myAxes[anIt].Compute(thePrsMgr, thePrs, anAspectAx, mySkinMode);
    myAxes[anIt].SetTransformPersistence(TransformPersistence());
  }

  updateTransformation();
}

//=================================================================================================

void AIS_Manipulator::HilightSelected(
  const occ::handle<PrsMgr_PresentationManager>&                  thePM,
  const NCollection_Sequence<occ::handle<SelectMgr_EntityOwner>>& theSeq)
{
  if (theSeq.IsEmpty())
  {
    return;
  }

  if (myIsActivationOnDetection)
  {
    return;
  }

  if (!theSeq(1)->IsKind(STANDARD_TYPE(AIS_ManipulatorOwner)))
  {
    thePM->Color(this, GetContext()->HighlightStyle(), 0);
    return;
  }

  occ::handle<AIS_ManipulatorOwner> anOwner = occ::down_cast<AIS_ManipulatorOwner>(theSeq(1));
  myHighlightAspect->Aspect()->SetInteriorColor(GetContext()->HighlightStyle()->Color());
  occ::handle<Graphic3d_Group> aGroup = getGroup(anOwner->Index(), anOwner->Mode());
  if (aGroup.IsNull())
  {
    return;
  }

  if (anOwner->Mode() == AIS_MM_TranslationPlane && mySkinMode == ManipulatorSkin_Shaded)
  {
    myDraggerHighlight->SetColor(myAxes[anOwner->Index()].Color());
    aGroup->SetGroupPrimitivesAspect(myDraggerHighlight->Aspect());
  }
  else
    aGroup->SetGroupPrimitivesAspect(myHighlightAspect->Aspect());

  myCurrentIndex = anOwner->Index();
  myCurrentMode  = anOwner->Mode();
}

//=================================================================================================

void AIS_Manipulator::ClearSelected()
{
  DeactivateCurrentMode();
}

//=================================================================================================

void AIS_Manipulator::HilightOwnerWithColor(const occ::handle<PrsMgr_PresentationManager>& thePM,
                                            const occ::handle<Prs3d_Drawer>&               theStyle,
                                            const occ::handle<SelectMgr_EntityOwner>&      theOwner)
{
  occ::handle<AIS_ManipulatorOwner> anOwner       = occ::down_cast<AIS_ManipulatorOwner>(theOwner);
  occ::handle<Prs3d_Presentation>   aPresentation = getHighlightPresentation(anOwner);
  if (aPresentation.IsNull())
  {
    return;
  }

  aPresentation->CStructure()->ViewAffinity = myViewAffinity;

  if (anOwner->Mode() == AIS_MM_TranslationPlane && mySkinMode == ManipulatorSkin_Shaded)
  {
    occ::handle<Prs3d_Drawer> aStyle = new Prs3d_Drawer();
    aStyle->SetColor(myAxes[anOwner->Index()].Color());
    aStyle->SetTransparency(0.5);
    aPresentation->Highlight(aStyle);
  }
  else
  {
    aPresentation->Highlight(theStyle);
  }

  for (NCollection_Sequence<occ::handle<Graphic3d_Group>>::Iterator aGroupIter(
         aPresentation->Groups());
       aGroupIter.More();
       aGroupIter.Next())
  {
    occ::handle<Graphic3d_Group>& aGrp = aGroupIter.ChangeValue();
    if (!aGrp.IsNull())
    {
      aGrp->SetGroupPrimitivesAspect(myHighlightAspect->Aspect());
    }
  }
  aPresentation->SetZLayer(Graphic3d_ZLayerId_Topmost);
  thePM->AddToImmediateList(aPresentation);

  if (myIsActivationOnDetection)
  {
    if (HasActiveMode())
    {
      DeactivateCurrentMode();
    }

    myCurrentIndex = anOwner->Index();
    myCurrentMode  = anOwner->Mode();
  }
}

//=================================================================================================

void AIS_Manipulator::RecomputeSelection(const AIS_ManipulatorMode theMode)
{
  if (theMode == AIS_MM_None)
  {
    return;
  }

  const occ::handle<SelectMgr_Selection>& aSelection = Object()->Selection(theMode);
  if (!aSelection.IsNull())
  {
    aSelection->Clear();
    ComputeSelection(aSelection, theMode);
  }
}

//=================================================================================================

void AIS_Manipulator::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                       const int                               theMode)
{
  // Check mode
  const AIS_ManipulatorMode aMode = (AIS_ManipulatorMode)theMode;
  if (aMode == AIS_MM_None)
  {
    return;
  }
  occ::handle<SelectMgr_EntityOwner> anOwner;

  // Sensitivity calculation for manipulator parts allows to avoid
  // overlapping of sensitive areas when size of manipulator is small.
  // Sensitivity is calculated relative to the default size of the manipulator (100.0f).
  const float aSensitivityCoef = myAxes[0].Size() / 100.0f;
  // clang-format off
  const int aHighSensitivity = std::clamp(static_cast<int>(aSensitivityCoef * 15), 3, 15); // clamp sensitivity within range [3, 15]
  const int aLowSensitivity  = std::clamp(static_cast<int>(aSensitivityCoef * 10), 2, 10); // clamp sensitivity within range [2, 10]
  // clang-format on

  switch (aMode)
  {
    case AIS_MM_Translation: {
      for (int anIt = 0; anIt < 3; ++anIt)
      {
        if (!myAxes[anIt].HasTranslation())
        {
          continue;
        }
        const Axis& anAxis = myAxes[anIt];
        anOwner            = new AIS_ManipulatorOwner(this, anIt, AIS_MM_Translation, 9);

        if (mySkinMode == ManipulatorSkin_Shaded)
        {
          // define sensitivity by line
          occ::handle<Select3D_SensitiveSegment> aLine =
            new Select3D_SensitiveSegment(anOwner, gp::Origin(), anAxis.TranslatorTipPosition());
          aLine->SetSensitivityFactor(aHighSensitivity);
          theSelection->Add(aLine);
        }

        // enlarge sensitivity by triangulation
        occ::handle<Select3D_SensitivePrimitiveArray> aTri =
          new Select3D_SensitivePrimitiveArray(anOwner);
        TopLoc_Location aTrsf =
          !myAxes[anIt].TranslatorGroup().IsNull()
            ? TopLoc_Location(myAxes[anIt].TranslatorGroup()->Transformation())
            : TopLoc_Location();
        aTri->InitTriangulation(anAxis.TriangleArray()->Attributes(),
                                anAxis.TriangleArray()->Indices(),
                                aTrsf);
        theSelection->Add(aTri);
      }
      break;
    }
    case AIS_MM_Rotation: {
      for (int anIt = 0; anIt < 3; ++anIt)
      {
        if (!myAxes[anIt].HasRotation())
        {
          continue;
        }
        const Axis& anAxis = myAxes[anIt];
        anOwner            = new AIS_ManipulatorOwner(this, anIt, AIS_MM_Rotation, 9);

        if (mySkinMode == ManipulatorSkin_Shaded)
        {
          // define sensitivity by circle
          const gp_Circ aGeomCircle(gp_Ax2(gp::Origin(), anAxis.ReferenceAxis().Direction()),
                                    anAxis.RotatorDiskRadius());
          occ::handle<Select3D_SensitiveCircle> aCircle = new ManipSensCircle(anOwner, aGeomCircle);
          aCircle->SetSensitivityFactor(aLowSensitivity);
          theSelection->Add(aCircle);
        }
        // enlarge sensitivity by triangulation
        occ::handle<Select3D_SensitivePrimitiveArray> aTri =
          new Select3D_SensitivePrimitiveArray(anOwner);
        const occ::handle<Graphic3d_Group>& aGroup = myAxes[anIt].RotatorGroup();
        TopLoc_Location                     aTrsf =
          !aGroup.IsNull() ? TopLoc_Location(aGroup->Transformation()) : TopLoc_Location();
        aTri->InitTriangulation(myAxes[anIt].RotatorDisk().Array()->Attributes(),
                                myAxes[anIt].RotatorDisk().Array()->Indices(),
                                aTrsf);
        theSelection->Add(aTri);
      }
      break;
    }
    case AIS_MM_Scaling: {
      for (int anIt = 0; anIt < 3; ++anIt)
      {
        if (!myAxes[anIt].HasScaling())
        {
          continue;
        }
        anOwner = new AIS_ManipulatorOwner(this, anIt, AIS_MM_Scaling, 9);

        if (mySkinMode == ManipulatorSkin_Shaded)
        {
          // define sensitivity by point
          occ::handle<Select3D_SensitivePoint> aPnt =
            new Select3D_SensitivePoint(anOwner, myAxes[anIt].ScalerCubePosition());
          aPnt->SetSensitivityFactor(aHighSensitivity);
          theSelection->Add(aPnt);
        }
        // enlarge sensitivity by triangulation
        occ::handle<Select3D_SensitivePrimitiveArray> aTri =
          new Select3D_SensitivePrimitiveArray(anOwner);
        const occ::handle<Graphic3d_Group>& aGroup = myAxes[anIt].ScalerGroup();
        TopLoc_Location                     aTrsf =
          !aGroup.IsNull() ? TopLoc_Location(aGroup->Transformation()) : TopLoc_Location();
        aTri->InitTriangulation(myAxes[anIt].ScalerCube().Array()->Attributes(),
                                myAxes[anIt].ScalerCube().Array()->Indices(),
                                aTrsf);
        theSelection->Add(aTri);
      }
      break;
    }
    case AIS_MM_TranslationPlane: {
      for (int anIt = 0; anIt < 3; ++anIt)
      {
        if (!myAxes[anIt].HasDragging())
        {
          continue;
        }
        anOwner = new AIS_ManipulatorOwner(this, anIt, AIS_MM_TranslationPlane, 9);

        if (mySkinMode == ManipulatorSkin_Shaded)
        {
          // define sensitivity by two crossed lines
          double aSensitivityOffset = ZoomPersistence() ? aHighSensitivity * (0.5 + M_SQRT2) : 0.0;
          gp_Pnt aP1                = myAxes[((anIt + 1) % 3)].TranslatorTipPosition().Translated(
            myAxes[((anIt + 2) % 3)].ReferenceAxis().Direction().XYZ() * aSensitivityOffset);
          gp_Pnt aP2 = myAxes[((anIt + 2) % 3)].TranslatorTipPosition().Translated(
            myAxes[((anIt + 1) % 3)].ReferenceAxis().Direction().XYZ() * aSensitivityOffset);
          gp_XYZ aMidP  = (aP1.XYZ() + aP2.XYZ()) / 2.0;
          gp_XYZ anOrig = aMidP.Normalized().Multiplied(aSensitivityOffset);

          occ::handle<Select3D_SensitiveSegment> aLine1 =
            new Select3D_SensitiveSegment(anOwner, aP1, aP2);
          aLine1->SetSensitivityFactor(aLowSensitivity);
          theSelection->Add(aLine1);
          occ::handle<Select3D_SensitiveSegment> aLine2 =
            new Select3D_SensitiveSegment(anOwner, anOrig, aMidP);
          aLine2->SetSensitivityFactor(aLowSensitivity);
          theSelection->Add(aLine2);
        }

        // enlarge sensitivity by triangulation
        occ::handle<Select3D_SensitivePrimitiveArray> aTri =
          new Select3D_SensitivePrimitiveArray(anOwner);
        const occ::handle<Graphic3d_Group>& aGroup = myAxes[anIt].DraggerGroup();
        TopLoc_Location                     aTrsf =
          !aGroup.IsNull() ? TopLoc_Location(aGroup->Transformation()) : TopLoc_Location();
        aTri->InitTriangulation(myAxes[anIt].DraggerSector().Array()->Attributes(),
                                myAxes[anIt].DraggerSector().Array()->Indices(),
                                aTrsf);
        theSelection->Add(aTri);
      }
      break;
    }
    default: {
      anOwner = new SelectMgr_EntityOwner(this, 5);
      break;
    }
  }
}

//=================================================================================================

void AIS_Manipulator::Disk::Init(const float   theInnerRadius,
                                 const float   theOuterRadius,
                                 const gp_Ax1& thePosition,
                                 const double  theAngle,
                                 const int     theSlicesNb,
                                 const int     theStacksNb)
{
  myPosition = thePosition;
  myInnerRad = theInnerRadius;
  myOuterRad = theOuterRadius;

  Prs3d_ToolDisk aTool(theInnerRadius, theOuterRadius, theSlicesNb, theStacksNb);
  aTool.SetAngleRange(0, theAngle);
  gp_Ax3  aSystem(myPosition.Location(), myPosition.Direction());
  gp_Trsf aTrsf;
  aTrsf.SetTransformation(aSystem, gp_Ax3());
  myArray         = aTool.CreateTriangulation(aTrsf);
  myTriangulation = aTool.CreatePolyTriangulation(aTrsf);
}

//=================================================================================================

void AIS_Manipulator::Sphere::Init(const float           theRadius,
                                   const gp_Pnt&         thePosition,
                                   const ManipulatorSkin theSkinMode,
                                   const int             theSlicesNb,
                                   const int             theStacksNb)
{
  myPosition = thePosition;
  myRadius   = theRadius;

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(gp::Origin(), thePosition));

  const double     aRadius = theSkinMode == ManipulatorSkin_Flat ? theRadius * 0.5 : theRadius;
  Prs3d_ToolSphere aTool(aRadius, theSlicesNb, theStacksNb);
  myArray         = aTool.CreateTriangulation(aTrsf);
  myTriangulation = aTool.CreatePolyTriangulation(aTrsf);
}

//=================================================================================================

void AIS_Manipulator::Cube::Init(const gp_Ax1&         thePosition,
                                 const float           theSize,
                                 const ManipulatorSkin theSkinMode)
{
  if (theSkinMode == ManipulatorSkin_Flat)
  {
    gp_Dir aXDirection;
    if (thePosition.Direction().X() > 0)
      aXDirection = gp::DY();
    else if (thePosition.Direction().Y() > 0)
      aXDirection = gp::DZ();
    else
      aXDirection = gp::DX();

    gp_Pnt aLocation =
      thePosition.Location().Translated(gp_Vec(thePosition.Direction().XYZ() * theSize));
    gp_Ax3  aSystem(aLocation, aXDirection, thePosition.Direction());
    gp_Trsf aTrsf;
    aTrsf.SetTransformation(aSystem, gp_Ax3());

    Prs3d_ToolDisk aTool(0.0, theSize, 40, 40);
    myArray         = aTool.CreateTriangulation(aTrsf);
    myTriangulation = aTool.CreatePolyTriangulation(aTrsf);
  }
  else
  {
    myArray = new Graphic3d_ArrayOfTriangles(12 * 3, 0, true);

    NCollection_Array1<Poly_Triangle> aPolyTriangles(1, 12);
    NCollection_Array1<gp_Pnt>        aPoints(1, 36);
    myTriangulation = new Poly_Triangulation(aPoints, aPolyTriangles);

    gp_Ax2 aPln(thePosition.Location(), thePosition.Direction());
    gp_Pnt aBottomLeft = thePosition.Location().XYZ() - aPln.XDirection().XYZ() * theSize * 0.5
                         - aPln.YDirection().XYZ() * theSize * 0.5;
    gp_Pnt aV2 = aBottomLeft.XYZ() + aPln.YDirection().XYZ() * theSize;
    gp_Pnt aV3 =
      aBottomLeft.XYZ() + aPln.YDirection().XYZ() * theSize + aPln.XDirection().XYZ() * theSize;
    gp_Pnt aV4       = aBottomLeft.XYZ() + aPln.XDirection().XYZ() * theSize;
    gp_Pnt aTopRight = thePosition.Location().XYZ() + thePosition.Direction().XYZ() * theSize
                       + aPln.XDirection().XYZ() * theSize * 0.5
                       + aPln.YDirection().XYZ() * theSize * 0.5;
    gp_Pnt aV5 = aTopRight.XYZ() - aPln.YDirection().XYZ() * theSize;
    gp_Pnt aV6 =
      aTopRight.XYZ() - aPln.YDirection().XYZ() * theSize - aPln.XDirection().XYZ() * theSize;
    gp_Pnt aV7 = aTopRight.XYZ() - aPln.XDirection().XYZ() * theSize;

    gp_Dir aRight((gp_Vec(aTopRight, aV7) ^ gp_Vec(aTopRight, aV2)).XYZ());
    gp_Dir aFront((gp_Vec(aV3, aV4) ^ gp_Vec(aV3, aV5)).XYZ());

    // Bottom
    addTriangle(0, aBottomLeft, aV2, aV3, -thePosition.Direction());
    addTriangle(1, aBottomLeft, aV3, aV4, -thePosition.Direction());

    // Front
    addTriangle(2, aV3, aV5, aV4, -aFront);
    addTriangle(3, aV3, aTopRight, aV5, -aFront);

    // Back
    addTriangle(4, aBottomLeft, aV7, aV2, aFront);
    addTriangle(5, aBottomLeft, aV6, aV7, aFront);

    // aTop
    addTriangle(6, aV7, aV6, aV5, thePosition.Direction());
    addTriangle(7, aTopRight, aV7, aV5, thePosition.Direction());

    // Left
    addTriangle(8, aV6, aV4, aV5, aRight);
    addTriangle(9, aBottomLeft, aV4, aV6, aRight);

    // Right
    addTriangle(10, aV3, aV7, aTopRight, -aRight);
    addTriangle(11, aV3, aV2, aV7, -aRight);
  }
}

//=======================================================================
// class    : Cube
// function : addTriangle
// purpose  :
//=======================================================================
void AIS_Manipulator::Cube::addTriangle(const int     theIndex,
                                        const gp_Pnt& theP1,
                                        const gp_Pnt& theP2,
                                        const gp_Pnt& theP3,
                                        const gp_Dir& theNormal)
{
  myTriangulation->SetNode(theIndex * 3 + 1, theP1);
  myTriangulation->SetNode(theIndex * 3 + 2, theP2);
  myTriangulation->SetNode(theIndex * 3 + 3, theP3);

  myTriangulation->SetTriangle(theIndex + 1,
                               Poly_Triangle(theIndex * 3 + 1, theIndex * 3 + 2, theIndex * 3 + 3));
  myArray->AddVertex(theP1, theNormal);
  myArray->AddVertex(theP2, theNormal);
  myArray->AddVertex(theP3, theNormal);
}

//=================================================================================================

void AIS_Manipulator::Sector::Init(const float           theRadius,
                                   const gp_Ax1&         thePosition,
                                   const gp_Dir&         theXDirection,
                                   const ManipulatorSkin theSkinMode,
                                   const int             theSlicesNb,
                                   const int             theStacksNb)
{
  gp_Ax3  aSystem(thePosition.Location(), thePosition.Direction(), theXDirection);
  gp_Trsf aTrsf;
  aTrsf.SetTransformation(aSystem, gp_Ax3());

  if (theSkinMode == ManipulatorSkin_Flat)
  {
    myArray         = new Graphic3d_ArrayOfTriangles(4, 6, Graphic3d_ArrayFlags_VertexNormal);
    myTriangulation = new Poly_Triangulation(4, 2, false);

    const double anIndent = theRadius / 3.0;
    gp_Pnt       aV1      = gp_Pnt(anIndent, anIndent, 0.0).Transformed(aTrsf);
    gp_Pnt       aV2      = gp_Pnt(anIndent, anIndent * 2.0, 0.0).Transformed(aTrsf);
    gp_Pnt       aV3      = gp_Pnt(anIndent * 2.0, anIndent * 2.0, 0.0).Transformed(aTrsf);
    gp_Pnt       aV4      = gp_Pnt(anIndent * 2.0, anIndent, 0.0).Transformed(aTrsf);
    gp_Dir       aNormal  = gp_Dir(gp_Dir::D::NZ).Transformed(aTrsf);

    myArray->AddVertex(aV1, aNormal);
    myArray->AddVertex(aV2, aNormal);
    myArray->AddVertex(aV3, aNormal);
    myArray->AddVertex(aV4, aNormal);
    myArray->AddTriangleEdges(3, 1, 2);
    myArray->AddTriangleEdges(1, 3, 4);

    myTriangulation->SetNode(1, aV1);
    myTriangulation->SetNode(2, aV2);
    myTriangulation->SetNode(3, aV3);
    myTriangulation->SetNode(4, aV4);
    myTriangulation->SetTriangle(1, Poly_Triangle(3, 1, 2));
    myTriangulation->SetTriangle(2, Poly_Triangle(1, 3, 4));
  }
  else
  {
    Prs3d_ToolSector aTool(theRadius, theSlicesNb, theStacksNb);
    myArray         = aTool.CreateTriangulation(aTrsf);
    myTriangulation = aTool.CreatePolyTriangulation(aTrsf);
  }
}

//=======================================================================
// class    : Axis
// function : Constructor
// purpose  :
//=======================================================================
AIS_Manipulator::Axis::Axis(const gp_Ax1&         theAxis,
                            const Quantity_Color& theColor,
                            const float           theLength)
    : myReferenceAxis(theAxis),
      myPosition(theAxis),
      myColor(theColor),
      myHasTranslation(true),
      myLength(theLength),
      myAxisRadius(0.5f),
      myHasScaling(true),
      myBoxSize(2.0f),
      myHasRotation(true),
      myInnerRadius(myLength + myBoxSize),
      myDiskThickness(myBoxSize * 0.5f),
      myIndent(0.2f),
      myHasDragging(true),
      myFacettesNumber(20),
      myCircleRadius(myLength + myBoxSize + myBoxSize * 0.5f * 0.5f)
{
}

//=================================================================================================

void AIS_Manipulator::Axis::Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                    const occ::handle<Prs3d_Presentation>&         thePrs,
                                    const occ::handle<Prs3d_ShadingAspect>&        theAspect,
                                    const ManipulatorSkin                          theSkinMode)
{
  if (myHasTranslation)
  {
    const double anArrowLength   = 0.25 * myLength;
    const double aCylinderLength = myLength - anArrowLength;
    myArrowTipPos =
      gp_Pnt(0.0, 0.0, 0.0).Translated(myReferenceAxis.Direction().XYZ() * aCylinderLength);

    myTranslatorGroup = thePrs->NewGroup();
    myTranslatorGroup->SetClosed(theSkinMode == ManipulatorSkin_Shaded);
    myTranslatorGroup->SetGroupPrimitivesAspect(theAspect->Aspect());

    if (theSkinMode == ManipulatorSkin_Flat)
    {
      const int aStripsNb = 14;

      myTriangleArray = new Graphic3d_ArrayOfTriangles(aStripsNb * 4,
                                                       aStripsNb * 6,
                                                       Graphic3d_ArrayFlags_VertexNormal);
      occ::handle<Graphic3d_ArrayOfTriangles> aColorlessArr =
        new Graphic3d_ArrayOfTriangles(aStripsNb * 2,
                                       aStripsNb * 3,
                                       Graphic3d_ArrayFlags_VertexNormal);
      occ::handle<Graphic3d_ArrayOfTriangles> aColoredArr = new Graphic3d_ArrayOfTriangles(
        aStripsNb * 2,
        aStripsNb * 3,
        Graphic3d_ArrayFlags_VertexNormal | Graphic3d_ArrayFlags_VertexColor);

      gp_Ax3  aSystem(gp::Origin(), myReferenceAxis.Direction());
      gp_Trsf aTrsf;
      aTrsf.SetTransformation(aSystem, gp_Ax3());

      gp_Dir aNormal = gp_Dir(gp_Dir::D::X).Transformed(aTrsf);
      double aLength = myLength + myIndent * 4.0f;

      const double aStepV = 1.0f / aStripsNb;
      for (int aU = 0; aU <= 1; ++aU)
      {
        for (int aV = 0; aV <= aStripsNb; ++aV)
        {
          gp_Pnt aVertex = gp_Pnt(0.0, myAxisRadius * (1.5f * aU - 0.75f), aLength * aV * aStepV)
                             .Transformed(aTrsf);
          myTriangleArray->AddVertex(aVertex, aNormal);

          if (aV != 0)
          {
            aColorlessArr->AddVertex(aVertex, aNormal);
          }
          if (aV != aStripsNb)
          {
            aColoredArr->AddVertex(aVertex, aNormal, myColor);
          }

          if (aU != 0 && aV != 0)
          {
            int aVertId = myTriangleArray->VertexNumber();
            myTriangleArray->AddTriangleEdges(aVertId, aVertId - aStripsNb - 2, aVertId - 1);
            myTriangleArray->AddTriangleEdges(aVertId - aStripsNb - 2,
                                              aVertId,
                                              aVertId - aStripsNb - 1);

            occ::handle<Graphic3d_ArrayOfTriangles> aSquares =
              aV % 2 == 0 ? aColorlessArr : aColoredArr;

            aVertId = aSquares->VertexNumber();
            aSquares->AddTriangleEdges(aVertId, aVertId - aStripsNb - 1, aVertId - 1);
            aSquares->AddTriangleEdges(aVertId - aStripsNb - 1, aVertId, aVertId - aStripsNb);
          }
        }
      }
      myTranslatorGroup->AddPrimitiveArray(aColoredArr);
      myTranslatorGroup->AddPrimitiveArray(aColorlessArr);
    }
    else
    {
      myTriangleArray = Prs3d_Arrow::DrawShaded(gp_Ax1(gp::Origin(), myReferenceAxis.Direction()),
                                                myAxisRadius,
                                                myLength,
                                                myAxisRadius * 1.5,
                                                anArrowLength,
                                                myFacettesNumber);
      myTranslatorGroup->AddPrimitiveArray(myTriangleArray);
    }

    if (myHighlightTranslator.IsNull())
    {
      myHighlightTranslator = new Prs3d_Presentation(thePrsMgr->StructureManager());
    }
    else
    {
      myHighlightTranslator->Clear();
    }
    {
      occ::handle<Graphic3d_Group> aGroup = myHighlightTranslator->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect(theAspect->Aspect());
      aGroup->AddPrimitiveArray(myTriangleArray);
    }
  }

  if (myHasScaling)
  {
    myCubePos = myReferenceAxis.Direction().XYZ() * (myLength + myIndent);
    const float aBoxSize =
      theSkinMode == ManipulatorSkin_Shaded ? myBoxSize : myBoxSize * 0.5f + myIndent;
    myCube.Init(gp_Ax1(myCubePos, myReferenceAxis.Direction()), aBoxSize, theSkinMode);

    myScalerGroup = thePrs->NewGroup();
    myScalerGroup->SetClosed(theSkinMode == ManipulatorSkin_Shaded);
    myScalerGroup->SetGroupPrimitivesAspect(theAspect->Aspect());
    myScalerGroup->AddPrimitiveArray(myCube.Array());

    if (myHighlightScaler.IsNull())
    {
      myHighlightScaler = new Prs3d_Presentation(thePrsMgr->StructureManager());
    }
    else
    {
      myHighlightScaler->Clear();
    }
    {
      occ::handle<Graphic3d_Group> aGroup = myHighlightScaler->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect(theAspect->Aspect());
      aGroup->AddPrimitiveArray(myCube.Array());
    }
  }

  if (myHasRotation)
  {
    myCircleRadius       = myInnerRadius + myIndent * 2.0f + myDiskThickness * 0.5f;
    const double anAngle = theSkinMode == ManipulatorSkin_Shaded ? M_PI * 2.0f : M_PI;
    myCircle.Init(myInnerRadius + myIndent * 2.0f,
                  Size(),
                  gp_Ax1(gp::Origin(), myReferenceAxis.Direction()),
                  anAngle,
                  myFacettesNumber * 2);
    myRotatorGroup = thePrs->NewGroup();
    myRotatorGroup->SetGroupPrimitivesAspect(theAspect->Aspect());
    myRotatorGroup->AddPrimitiveArray(myCircle.Array());

    if (myHighlightRotator.IsNull())
    {
      myHighlightRotator = new Prs3d_Presentation(thePrsMgr->StructureManager());
    }
    else
    {
      myHighlightRotator->Clear();
    }
    {
      occ::handle<Graphic3d_Group> aGroup = myHighlightRotator->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect(theAspect->Aspect());
      aGroup->AddPrimitiveArray(myCircle.Array());
    }
  }

  if (myHasDragging)
  {
    gp_Dir aXDirection;
    if (myReferenceAxis.Direction().X() > 0)
      aXDirection = gp::DY();
    else if (myReferenceAxis.Direction().Y() > 0)
      aXDirection = gp::DZ();
    else
      aXDirection = gp::DX();

    gp_Pnt aPosition = theSkinMode == ManipulatorSkin_Flat
                         ? gp_Pnt(myReferenceAxis.Direction().Reversed().XYZ() * (myAxisRadius))
                         : gp::Origin();
    float  aRadius = theSkinMode == ManipulatorSkin_Flat ? myLength : myInnerRadius + myIndent * 2;
    mySector.Init(aRadius,
                  gp_Ax1(aPosition, myReferenceAxis.Direction()),
                  aXDirection,
                  theSkinMode,
                  myFacettesNumber * 2);
    myDraggerGroup = thePrs->NewGroup();

    occ::handle<Graphic3d_AspectFillArea3d> aFillArea =
      theSkinMode == ManipulatorSkin_Flat ? theAspect->Aspect() : new Graphic3d_AspectFillArea3d();

    myDraggerGroup->SetGroupPrimitivesAspect(aFillArea);
    myDraggerGroup->AddPrimitiveArray(mySector.Array());

    if (myHighlightDragger.IsNull())
    {
      myHighlightDragger = new Prs3d_Presentation(thePrsMgr->StructureManager());
    }
    else
    {
      myHighlightDragger->Clear();
    }
    {
      occ::handle<Graphic3d_Group> aGroup = myHighlightDragger->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect(aFillArea);
      aGroup->AddPrimitiveArray(mySector.Array());
    }
  }
}
