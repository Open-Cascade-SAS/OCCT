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
#include <SelectMgr_SequenceOfOwner.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_HANDLE (AIS_Manipulator, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_Manipulator, AIS_InteractiveObject)

IMPLEMENT_HSEQUENCE(AIS_ManipulatorObjectSequence)

namespace
{
  //! Return Ax1 for specified direction of Ax2.
  static gp_Ax1 getAx1FromAx2Dir (const gp_Ax2& theAx2,
                                  int theIndex)
  {
    switch (theIndex)
    {
      case 0: return gp_Ax1 (theAx2.Location(), theAx2.XDirection());
      case 1: return gp_Ax1 (theAx2.Location(), theAx2.YDirection());
      case 2: return theAx2.Axis();
    }
    throw Standard_ProgramError ("AIS_Manipulator - Invalid axis index");
  }

  //! Auxiliary tool for filtering picking ray.
  class ManipSensRotation
  {
  public:
    //! Main constructor.
    ManipSensRotation (const gp_Dir& thePlaneNormal) : myPlaneNormal (thePlaneNormal), myAngleTol (10.0 * M_PI / 180.0) {}

    //! Checks if picking ray can be used for detection.
    Standard_Boolean isValidRay (const SelectBasics_SelectingVolumeManager& theMgr) const
    {
      if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
      {
        return Standard_False;
      }

      const gp_Dir aRay = theMgr.GetViewRayDirection();
      return !aRay.IsNormal (myPlaneNormal, myAngleTol);
    }
  private:
    gp_Dir        myPlaneNormal;
    Standard_Real myAngleTol;
  };

  //! Sensitive circle with filtering picking ray.
  class ManipSensCircle : public Select3D_SensitiveCircle, public ManipSensRotation
  {
  public:
    //! Main constructor.
    ManipSensCircle (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                     const gp_Circ& theCircle)
    : Select3D_SensitiveCircle (theOwnerId, theCircle, Standard_False),
      ManipSensRotation (theCircle.Position().Direction()) {}

    //! Checks whether the circle overlaps current selecting volume
    virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                      SelectBasics_PickResult& thePickResult) Standard_OVERRIDE
    {
      return isValidRay (theMgr)
          && Select3D_SensitiveCircle::Matches (theMgr, thePickResult);
    }
  };

  //! Sensitive triangulation with filtering picking ray.
  class ManipSensTriangulation : public Select3D_SensitiveTriangulation, public ManipSensRotation
  {
  public:
    ManipSensTriangulation (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                            const Handle(Poly_Triangulation)& theTrg,
                            const gp_Dir& thePlaneNormal)
    : Select3D_SensitiveTriangulation (theOwnerId, theTrg, TopLoc_Location(), Standard_True),
      ManipSensRotation (thePlaneNormal) {}

    //! Checks whether the circle overlaps current selecting volume
    virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                      SelectBasics_PickResult& thePickResult) Standard_OVERRIDE
    {
      return isValidRay (theMgr)
          && Select3D_SensitiveTriangulation::Matches (theMgr, thePickResult);
    }
  };
}

//=======================================================================
//function : init
//purpose  : 
//=======================================================================
void AIS_Manipulator::init()
{
  // Create axis in the default coordinate system. The custom position is applied in local transformation.
  myAxes[0] = Axis (gp::OX(), Quantity_NOC_RED);
  myAxes[1] = Axis (gp::OY(), Quantity_NOC_GREEN);
  myAxes[2] = Axis (gp::OZ(), Quantity_NOC_BLUE1);

  Graphic3d_MaterialAspect aShadingMaterial;
  aShadingMaterial.SetSpecularColor(Quantity_NOC_BLACK);
  aShadingMaterial.SetMaterialType (Graphic3d_MATERIAL_ASPECT);

  myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
  myDrawer->ShadingAspect()->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
  myDrawer->ShadingAspect()->SetColor (Quantity_NOC_WHITE);
  myDrawer->ShadingAspect()->SetMaterial (aShadingMaterial);

  Graphic3d_MaterialAspect aHilightMaterial;
  aHilightMaterial.SetColor (Quantity_NOC_AZURE);
  aHilightMaterial.SetAmbientColor (Quantity_NOC_BLACK);
  aHilightMaterial.SetDiffuseColor (Quantity_NOC_BLACK);
  aHilightMaterial.SetSpecularColor(Quantity_NOC_BLACK);
  aHilightMaterial.SetEmissiveColor(Quantity_NOC_BLACK);
  aHilightMaterial.SetMaterialType (Graphic3d_MATERIAL_ASPECT);

  myHighlightAspect = new Prs3d_ShadingAspect();
  myHighlightAspect->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
  myHighlightAspect->SetMaterial (aHilightMaterial);

  Graphic3d_MaterialAspect aDraggerMaterial;
  aDraggerMaterial.SetAmbientColor (Quantity_NOC_BLACK);
  aDraggerMaterial.SetDiffuseColor (Quantity_NOC_BLACK);
  aDraggerMaterial.SetSpecularColor(Quantity_NOC_BLACK);
  aDraggerMaterial.SetMaterialType(Graphic3d_MATERIAL_ASPECT);

  myDraggerHighlight = new Prs3d_ShadingAspect();
  myDraggerHighlight->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
  myDraggerHighlight->SetMaterial(aDraggerMaterial);

  myDraggerHighlight->SetTransparency(0.5);

  SetSize (100);
  SetZLayer (Graphic3d_ZLayerId_Topmost);
}

//=======================================================================
//function : getHighlightPresentation
//purpose  : 
//=======================================================================
Handle(Prs3d_Presentation) AIS_Manipulator::getHighlightPresentation (const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Handle(Prs3d_Presentation) aDummyPrs;
  Handle(AIS_ManipulatorOwner) anOwner = Handle(AIS_ManipulatorOwner)::DownCast (theOwner);
  if (anOwner.IsNull())
  {
    return aDummyPrs;
  }

  switch (anOwner->Mode())
  {
    case AIS_MM_Translation     : return myAxes[anOwner->Index()].TranslatorHighlightPrs();
    case AIS_MM_Rotation        : return myAxes[anOwner->Index()].RotatorHighlightPrs();
    case AIS_MM_Scaling         : return myAxes[anOwner->Index()].ScalerHighlightPrs();
    case AIS_MM_TranslationPlane: return myAxes[anOwner->Index()].DraggerHighlightPrs();
    case AIS_MM_None            : break;
  }

  return aDummyPrs;
}

//=======================================================================
//function : getGroup
//purpose  : 
//=======================================================================
Handle(Graphic3d_Group) AIS_Manipulator::getGroup (const Standard_Integer theIndex, const AIS_ManipulatorMode theMode) const
{
  Handle(Graphic3d_Group) aDummyGroup;

  if (theIndex < 0 || theIndex > 2)
  {
    return aDummyGroup;
  }

  switch (theMode)
  {
    case AIS_MM_Translation     : return myAxes[theIndex].TranslatorGroup();
    case AIS_MM_Rotation        : return myAxes[theIndex].RotatorGroup();
    case AIS_MM_Scaling         : return myAxes[theIndex].ScalerGroup();
    case AIS_MM_TranslationPlane: return myAxes[theIndex].DraggerGroup();
    case AIS_MM_None            : break;
  }

  return aDummyGroup;
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_Manipulator::AIS_Manipulator()
: myPosition (gp::XOY()),
  myCurrentIndex (-1),
  myCurrentMode (AIS_MM_None),
  myIsActivationOnDetection (Standard_False),
  myIsZoomPersistentMode (Standard_True),
  myHasStartedTransformation (Standard_False),
  myStartPosition (gp::XOY()),
  myStartPick (0.0, 0.0, 0.0),
  myPrevState (0.0)
{
  SetInfiniteState();
  SetMutable (Standard_True);
  SetDisplayMode (AIS_Shaded);
  init();
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_Manipulator::AIS_Manipulator (const gp_Ax2& thePosition)
: myPosition (thePosition),
  myCurrentIndex (-1),
  myCurrentMode (AIS_MM_None),
  myIsActivationOnDetection (Standard_False),
  myIsZoomPersistentMode (Standard_True),
  myHasStartedTransformation (Standard_False),
  myStartPosition (gp::XOY()),
  myStartPick (0.0, 0.0, 0.0),
  myPrevState (0.0)
{
  SetInfiniteState();
  SetMutable (Standard_True);
  SetDisplayMode (AIS_Shaded);
  init();
}

//=======================================================================
//function : SetPart
//purpose  : 
//=======================================================================
void AIS_Manipulator::SetPart (const Standard_Integer theAxisIndex, const AIS_ManipulatorMode theMode, const Standard_Boolean theIsEnabled)
{
  Standard_ProgramError_Raise_if (theAxisIndex < 0 || theAxisIndex > 2, "AIS_Manipulator::SetMode(): axis index should be between 0 and 2");
  switch (theMode)
  {
    case AIS_MM_Translation:
      myAxes[theAxisIndex].SetTranslation (theIsEnabled);
      break;

    case AIS_MM_Rotation:
      myAxes[theAxisIndex].SetRotation (theIsEnabled);
      break;

    case AIS_MM_Scaling:
      myAxes[theAxisIndex].SetScaling (theIsEnabled);
      break;

    case AIS_MM_TranslationPlane:
      myAxes[theAxisIndex].SetDragging(theIsEnabled);
      break;

    case AIS_MM_None:
      break;
  }
}

//=======================================================================
//function : SetPart
//purpose  : 
//=======================================================================
void AIS_Manipulator::SetPart (const AIS_ManipulatorMode theMode, const Standard_Boolean theIsEnabled)
{
  for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
  {
    SetPart (anIt, theMode, theIsEnabled);
  }
}

//=======================================================================
//function : EnableMode
//purpose  : 
//=======================================================================
void AIS_Manipulator::EnableMode (const AIS_ManipulatorMode theMode)
{
  if (!IsAttached())
  {
    return;
  }

  const Handle(AIS_InteractiveContext)& aContext = GetContext();
  if (aContext.IsNull())
  {
    return;
  }

  aContext->Activate (this, theMode);
}

//=======================================================================
//function : attachToBox
//purpose  : 
//=======================================================================
void AIS_Manipulator::attachToBox (const Bnd_Box& theBox)
{
  if (theBox.IsVoid())
  {
    return;
  }

  Standard_Real anXmin = 0.0, anYmin = 0.0, aZmin = 0.0, anXmax = 0.0, anYmax = 0.0, aZmax = 0.0;
  theBox.Get (anXmin, anYmin, aZmin, anXmax, anYmax, aZmax);

  gp_Ax2 aPosition = gp::XOY();
  aPosition.SetLocation (gp_Pnt ((anXmin + anXmax) * 0.5, (anYmin + anYmax) * 0.5, (aZmin + aZmax) * 0.5));
  SetPosition (aPosition);
}

//=======================================================================
//function : adjustSize
//purpose  : 
//=======================================================================
void AIS_Manipulator::adjustSize (const Bnd_Box& theBox)
{
  Standard_Real aXmin = 0., aYmin = 0., aZmin = 0., aXmax = 0., aYmax = 0., aZmax = 0.0;
  theBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  Standard_Real aXSize = aXmax - aXmin;
  Standard_Real aYSize = aYmax - aYmin;
  Standard_Real aZSize  = aZmax  - aZmin;

  SetSize ((Standard_ShortReal) (Max (aXSize, Max (aYSize, aZSize)) * 0.5));
}

//=======================================================================
//function : Attach
//purpose  : 
//=======================================================================
void AIS_Manipulator::Attach (const Handle(AIS_InteractiveObject)& theObject, const OptionsForAttach& theOptions)
{
  if (theObject->IsKind (STANDARD_TYPE(AIS_Manipulator)))
  {
    return;
  }

  Handle(AIS_ManipulatorObjectSequence) aSeq = new AIS_ManipulatorObjectSequence();
  aSeq->Append (theObject);
  Attach (aSeq, theOptions);
}

//=======================================================================
//function : Attach
//purpose  : 
//=======================================================================
void AIS_Manipulator::Attach (const Handle(AIS_ManipulatorObjectSequence)& theObjects, const OptionsForAttach& theOptions)
{
  if (theObjects->Size() < 1)
  {
    return;
  }

  SetOwner (theObjects);
  Bnd_Box aBox;
  const Handle(AIS_InteractiveObject)& aCurObject = theObjects->Value (theObjects->Lower());
  aCurObject->BoundingBox (aBox);

  if (theOptions.AdjustPosition)
  {
    attachToBox (aBox);
  }

  if (theOptions.AdjustSize)
  {
    adjustSize (aBox);
  }

  const Handle(AIS_InteractiveContext)& aContext = Object()->GetContext();
  if (!aContext.IsNull())
  {
    if (!aContext->IsDisplayed (this))
    {
      aContext->Display (this, Standard_False);
    }
    else
    {
      aContext->Update (this, Standard_False);
      aContext->RecomputeSelectionOnly (this);
    }

    aContext->Load (this);
  }

  if (theOptions.EnableModes)
  {
    EnableMode (AIS_MM_Rotation);
    EnableMode (AIS_MM_Translation);
    EnableMode (AIS_MM_Scaling);
    EnableMode (AIS_MM_TranslationPlane);
  }
}

//=======================================================================
//function : Detach
//purpose  : 
//=======================================================================
void AIS_Manipulator::Detach()
{
  DeactivateCurrentMode();

  if (!IsAttached())
  {
    return;
  }

  Handle(AIS_InteractiveObject) anObject = Object();
  const Handle(AIS_InteractiveContext)& aContext = anObject->GetContext();
  if (!aContext.IsNull())
  {
    aContext->Remove (this, Standard_False);
  }

  SetOwner (NULL);
}

//=======================================================================
//function : Objects
//purpose  : 
//=======================================================================
Handle(AIS_ManipulatorObjectSequence) AIS_Manipulator::Objects() const
{
  return Handle(AIS_ManipulatorObjectSequence)::DownCast (GetOwner());
}

//=======================================================================
//function : Object
//purpose  : 
//=======================================================================
Handle(AIS_InteractiveObject) AIS_Manipulator::Object (const Standard_Integer theIndex) const
{
  Handle(AIS_ManipulatorObjectSequence) anOwner = Handle(AIS_ManipulatorObjectSequence)::DownCast (GetOwner());

  Standard_ProgramError_Raise_if (theIndex < anOwner->Lower() || theIndex > anOwner->Upper(), "AIS_Manipulator::Object(): wrong index value");

  if (anOwner.IsNull() || anOwner->IsEmpty())
  {
    return NULL;
  }

  return anOwner->Value (theIndex);
}

//=======================================================================
//function : Object
//purpose  : 
//=======================================================================
Handle(AIS_InteractiveObject) AIS_Manipulator::Object() const
{
  return Object (1);
}

//=======================================================================
//function : ObjectTransformation
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Manipulator::ObjectTransformation (const Standard_Integer theMaxX, const Standard_Integer theMaxY,
                                                        const Handle(V3d_View)& theView, gp_Trsf& theTrsf)
{
  // Initialize start reference data
  if (!myHasStartedTransformation)
  {
    myStartTrsfs.Clear();
    Handle(AIS_ManipulatorObjectSequence) anObjects = Objects();
    for (AIS_ManipulatorObjectSequence::Iterator anObjIter (*anObjects); anObjIter.More(); anObjIter.Next())
    {
      myStartTrsfs.Append (anObjIter.Value()->LocalTransformation());
    }
    myStartPosition = myPosition;
  }

  // Get 3d point with projection vector
  Graphic3d_Vec3d anInputPoint, aProj;
  theView->ConvertWithProj (theMaxX, theMaxY, anInputPoint.x(), anInputPoint.y(), anInputPoint.z(), aProj.x(), aProj.y(), aProj.z());
  const gp_Lin anInputLine (gp_Pnt (anInputPoint.x(), anInputPoint.y(), anInputPoint.z()), gp_Dir (aProj.x(), aProj.y(), aProj.z()));
  switch (myCurrentMode)
  {
    case AIS_MM_Translation:
    case AIS_MM_Scaling:
    {
      const gp_Lin aLine (myStartPosition.Location(), myAxes[myCurrentIndex].Position().Direction());
      Extrema_ExtElC anExtrema (anInputLine, aLine, Precision::Angular());
      if (!anExtrema.IsDone()
        || anExtrema.IsParallel()
        || anExtrema.NbExt() != 1)
      {
        // translation cannot be done co-directed with camera
        return Standard_False;
      }

      Extrema_POnCurv anExPnts[2];
      anExtrema.Points (1, anExPnts[0], anExPnts[1]);
      const gp_Pnt aNewPosition = anExPnts[1].Value();
      if (!myHasStartedTransformation)
      {
        myStartPick = aNewPosition;
        myHasStartedTransformation = Standard_True;
        return Standard_True;
      }
      else if (aNewPosition.Distance (myStartPick) < Precision::Confusion())
      {
        return Standard_False;
      }

      gp_Trsf aNewTrsf;
      if (myCurrentMode == AIS_MM_Translation)
      {
        aNewTrsf.SetTranslation (gp_Vec(myStartPick, aNewPosition));
        theTrsf *= aNewTrsf;
      }
      else if (myCurrentMode == AIS_MM_Scaling)
      {
        if (aNewPosition.Distance (myStartPosition.Location()) < Precision::Confusion())
        {
          return Standard_False;
        }

        Standard_Real aCoeff = myStartPosition.Location().Distance (aNewPosition)
                             / myStartPosition.Location().Distance (myStartPick);
        aNewTrsf.SetScale (myPosition.Location(), aCoeff);
        theTrsf = aNewTrsf;
      }
      return Standard_True;
    }
    case AIS_MM_Rotation:
    {
      const gp_Pnt aPosLoc   = myStartPosition.Location();
      const gp_Ax1 aCurrAxis = getAx1FromAx2Dir (myStartPosition, myCurrentIndex);
      IntAna_IntConicQuad aIntersector (anInputLine, gp_Pln (aPosLoc, aCurrAxis.Direction()), Precision::Angular(), Precision::Intersection());
      if (!aIntersector.IsDone()
        || aIntersector.IsParallel()
        || aIntersector.NbPoints() < 1)
      {
        return Standard_False;
      }

      const gp_Pnt aNewPosition = aIntersector.Point (1);
      if (!myHasStartedTransformation)
      {
        myStartPick = aNewPosition;
        myHasStartedTransformation = Standard_True;
        gp_Dir aStartAxis = gce_MakeDir (aPosLoc, myStartPick);
        myPrevState = aStartAxis.AngleWithRef (gce_MakeDir(aPosLoc, aNewPosition), aCurrAxis.Direction());
        return Standard_True;
      }

      if (aNewPosition.Distance (myStartPick) < Precision::Confusion())
      {
        return Standard_False;
      }

      gp_Dir aStartAxis = aPosLoc.IsEqual (myStartPick, Precision::Confusion())
        ? getAx1FromAx2Dir (myStartPosition, (myCurrentIndex + 1) % 3).Direction()
        : gce_MakeDir (aPosLoc, myStartPick);

      gp_Dir aCurrentAxis = gce_MakeDir (aPosLoc, aNewPosition);
      Standard_Real anAngle = aStartAxis.AngleWithRef (aCurrentAxis, aCurrAxis.Direction());

      // Change value of an angle if it should have different sign.
      if (anAngle * myPrevState < 0 && Abs (anAngle) < M_PI_2)
      {
        Standard_Real aSign = myPrevState > 0 ? -1.0 : 1.0;
        anAngle = aSign * (M_PI * 2 - anAngle);
      }

      if (Abs (anAngle) < Precision::Confusion())
      {
        return Standard_False;
      }

      gp_Trsf aNewTrsf;
      aNewTrsf.SetRotation (aCurrAxis, anAngle);
      theTrsf *= aNewTrsf;
      myPrevState = anAngle;
      return Standard_True;
    }
    case AIS_MM_TranslationPlane:
    {
      const gp_Pnt aPosLoc = myStartPosition.Location();
      const gp_Ax1 aCurrAxis = getAx1FromAx2Dir(myStartPosition, myCurrentIndex);
      IntAna_IntConicQuad aIntersector(anInputLine, gp_Pln(aPosLoc, aCurrAxis.Direction()), Precision::Angular(), Precision::Intersection());
      if (!aIntersector.IsDone() || aIntersector.NbPoints() < 1)
      {
        return Standard_False;
      }

      const gp_Pnt aNewPosition = aIntersector.Point(1);
      if (!myHasStartedTransformation)
      {
        myStartPick = aNewPosition;
        myHasStartedTransformation = Standard_True;
        return Standard_True;
      }

      if (aNewPosition.Distance(myStartPick) < Precision::Confusion())
      {
        return Standard_False;
      }

      gp_Trsf aNewTrsf;
      aNewTrsf.SetTranslation(gp_Vec(myStartPick, aNewPosition));
      theTrsf *= aNewTrsf;
      return Standard_True;
    }
    case AIS_MM_None:
    {
      return Standard_False;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : ProcessDragging
//purpose  :
//=======================================================================
Standard_Boolean AIS_Manipulator::ProcessDragging (const Handle(AIS_InteractiveContext)&,
                                                   const Handle(V3d_View)& theView,
                                                   const Handle(SelectMgr_EntityOwner)&,
                                                   const Graphic3d_Vec2i& theDragFrom,
                                                   const Graphic3d_Vec2i& theDragTo,
                                                   const AIS_DragAction theAction)
{
  switch (theAction)
  {
    case AIS_DragAction_Start:
    {
      if (HasActiveMode())
      {
        StartTransform (theDragFrom.x(), theDragFrom.y(), theView);
        return Standard_True;
      }
      break;
    }
    case AIS_DragAction_Confirmed:
    {
      return Standard_True;
    }
    case AIS_DragAction_Update:
    {
      Transform (theDragTo.x(), theDragTo.y(), theView);
      return Standard_True;
    }
    case AIS_DragAction_Abort:
    {
      StopTransform (false);
      return Standard_True;
    }
    case AIS_DragAction_Stop:
      break;
  }
  return Standard_False;
}

//=======================================================================
//function : StartTransform
//purpose  : 
//=======================================================================
void AIS_Manipulator::StartTransform (const Standard_Integer theX, const Standard_Integer theY, const Handle(V3d_View)& theView)
{
  if (myHasStartedTransformation)
  {
    return;
  }

  gp_Trsf aTrsf;
  ObjectTransformation (theX, theY, theView, aTrsf);
}

//=======================================================================
//function : StopTransform
//purpose  : 
//=======================================================================
void AIS_Manipulator::StopTransform (const Standard_Boolean theToApply)
{
  if (!IsAttached() || !myHasStartedTransformation)
  {
    return;
  }

  myHasStartedTransformation = Standard_False;
  if (theToApply)
  {
    return;
  }

  Handle(AIS_ManipulatorObjectSequence) anObjects = Objects();
  AIS_ManipulatorObjectSequence::Iterator anObjIter (*anObjects);
  NCollection_Sequence<gp_Trsf>::Iterator aTrsfIter (myStartTrsfs);
  for (; anObjIter.More(); anObjIter.Next(), aTrsfIter.Next())
  {
    anObjIter.ChangeValue()->SetLocalTransformation (aTrsfIter.Value());
  }
  SetPosition (myStartPosition);
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================
void AIS_Manipulator::Transform (const gp_Trsf& theTrsf)
{
  if (!IsAttached() || !myHasStartedTransformation)
  {
    return;
  }

  {
    Handle(AIS_ManipulatorObjectSequence) anObjects = Objects();
    AIS_ManipulatorObjectSequence::Iterator anObjIter (*anObjects);
    NCollection_Sequence<gp_Trsf>::Iterator aTrsfIter (myStartTrsfs);
    for (; anObjIter.More(); anObjIter.Next(), aTrsfIter.Next())
    {
      const Handle(AIS_InteractiveObject)& anObj = anObjIter.ChangeValue();
      const gp_Trsf& anOldTrsf = aTrsfIter.Value();
      const Handle(TopLoc_Datum3D)& aParentTrsf = anObj->CombinedParentTransformation();
      if (!aParentTrsf.IsNull()
        && aParentTrsf->Form() != gp_Identity)
      {
        // recompute local transformation relative to parent transformation
        const gp_Trsf aNewLocalTrsf = aParentTrsf->Trsf().Inverted() * theTrsf * aParentTrsf->Trsf() * anOldTrsf;
        anObj->SetLocalTransformation (aNewLocalTrsf);
      }
      else
      {
        anObj->SetLocalTransformation (theTrsf * anOldTrsf);
      }
    }
  }

  if ((myCurrentMode == AIS_MM_Translation      && myBehaviorOnTransform.FollowTranslation)
   || (myCurrentMode == AIS_MM_Rotation         && myBehaviorOnTransform.FollowRotation)
   || (myCurrentMode == AIS_MM_TranslationPlane && myBehaviorOnTransform.FollowDragging))
  {
    gp_Pnt aPos  = myStartPosition.Location().Transformed (theTrsf);
    gp_Dir aVDir = myStartPosition.Direction().Transformed (theTrsf);
    gp_Dir aXDir = myStartPosition.XDirection().Transformed (theTrsf);
    SetPosition (gp_Ax2 (aPos, aVDir, aXDir));
  }
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================
gp_Trsf AIS_Manipulator::Transform (const Standard_Integer thePX, const Standard_Integer thePY,
                                    const Handle(V3d_View)& theView)
{
  gp_Trsf aTrsf;
  if (ObjectTransformation (thePX, thePY, theView, aTrsf))
  {
    Transform (aTrsf);
  }

  return aTrsf;
}

//=======================================================================
//function : SetPosition
//purpose  : 
//=======================================================================
void AIS_Manipulator::SetPosition (const gp_Ax2& thePosition)
{
  if (!myPosition.Location().IsEqual (thePosition.Location(), Precision::Confusion())
   || !myPosition.Direction().IsEqual (thePosition.Direction(), Precision::Angular())
   || !myPosition.XDirection().IsEqual (thePosition.XDirection(), Precision::Angular()))
  {
    myPosition = thePosition;
    myAxes[0].SetPosition (getAx1FromAx2Dir (thePosition, 0));
    myAxes[1].SetPosition (getAx1FromAx2Dir (thePosition, 1));
    myAxes[2].SetPosition (getAx1FromAx2Dir (thePosition, 2));
    updateTransformation();
  }
}

//=======================================================================
//function : updateTransformation
//purpose  : set local transformation to avoid graphics recomputation
//=======================================================================
void AIS_Manipulator::updateTransformation()
{
  gp_Trsf aTrsf;

  if (!myIsZoomPersistentMode)
  {
    aTrsf.SetTransformation (myPosition, gp::XOY());
  }
  else
  {
    const gp_Dir& aVDir = myPosition.Direction();
    const gp_Dir& aXDir = myPosition.XDirection();
    aTrsf.SetTransformation (gp_Ax2 (gp::Origin(), aVDir, aXDir), gp::XOY());
  }

  Handle(TopLoc_Datum3D) aGeomTrsf = new TopLoc_Datum3D (aTrsf);
  // we explicitly call here setLocalTransformation() of the base class
  // since AIS_Manipulator::setLocalTransformation() implementation throws exception
  // as protection from external calls
  AIS_InteractiveObject::setLocalTransformation (aGeomTrsf);
  for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].Transform (aGeomTrsf);
  }

  if (myIsZoomPersistentMode)
  {
    if (TransformPersistence().IsNull()
    ||  TransformPersistence()->Mode() != Graphic3d_TMF_ZoomPers
    || !TransformPersistence()->AnchorPoint().IsEqual (myPosition.Location(), 0.0))
    {
      setTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_ZoomPers, myPosition.Location()));
    }
  }
}

//=======================================================================
//function : SetSize
//purpose  : 
//=======================================================================
void AIS_Manipulator::SetSize (const Standard_ShortReal theSideLength)
{
  for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].SetSize (theSideLength);
  }

  SetToUpdate();
}

//=======================================================================
//function : SetGap
//purpose  : 
//=======================================================================
void AIS_Manipulator::SetGap (const Standard_ShortReal theValue)
{
  for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].SetIndent (theValue);
  }

  SetToUpdate();
}

//=======================================================================
//function : DeactivateCurrentMode
//purpose  : 
//=======================================================================
void AIS_Manipulator::DeactivateCurrentMode()
{
  if (!myIsActivationOnDetection)
  {
    Handle(Graphic3d_Group) aGroup = getGroup (myCurrentIndex, myCurrentMode);
    if (aGroup.IsNull())
    {
      return;
    }

    Handle(Prs3d_ShadingAspect) anAspect = new Prs3d_ShadingAspect();
    anAspect->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
    anAspect->SetMaterial (myDrawer->ShadingAspect()->Material());
    if (myCurrentMode == AIS_MM_TranslationPlane)
      anAspect->SetTransparency(1.0);
    else
    {
      anAspect->SetTransparency(myDrawer->ShadingAspect()->Transparency());
      anAspect->SetColor(myAxes[myCurrentIndex].Color());
    }

    aGroup->SetGroupPrimitivesAspect (anAspect->Aspect());
  }

  myCurrentIndex = -1;
  myCurrentMode = AIS_MM_None;

  if (myHasStartedTransformation)
  {
    myHasStartedTransformation = Standard_False;
  }
}

//=======================================================================
//function : SetZoomPersistence
//purpose  : 
//=======================================================================
void AIS_Manipulator::SetZoomPersistence (const Standard_Boolean theToEnable)
{
  if (myIsZoomPersistentMode != theToEnable)
  {
    SetToUpdate();
  }

  myIsZoomPersistentMode = theToEnable;

  if (!theToEnable)
  {
    setTransformPersistence (Handle(Graphic3d_TransformPers)());
  }

  updateTransformation();
}

//=======================================================================
//function : SetTransformPersistence
//purpose  :
//=======================================================================
void AIS_Manipulator::SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  Standard_ASSERT_RETURN (!myIsZoomPersistentMode,
    "AIS_Manipulator::SetTransformPersistence: "
    "Custom settings are not allowed by this class in ZoomPersistence mode",);

  setTransformPersistence (theTrsfPers);
}

//=======================================================================
//function : setTransformPersistence
//purpose  : 
//=======================================================================
void AIS_Manipulator::setTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  AIS_InteractiveObject::SetTransformPersistence (theTrsfPers);

  for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].SetTransformPersistence (theTrsfPers);
  }
}

//=======================================================================
//function : setLocalTransformation
//purpose  :
//=======================================================================
void AIS_Manipulator::setLocalTransformation (const Handle(TopLoc_Datum3D)& /*theTrsf*/)
{
  Standard_ASSERT_INVOKE ("AIS_Manipulator::setLocalTransformation: "
                          "Custom transformation is not supported by this class");
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Manipulator::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                               const Handle(Prs3d_Presentation)& thePrs,
                               const Standard_Integer theMode)
{
  if (theMode != AIS_Shaded)
  {
    return;
  }

  thePrs->SetInfiniteState (Standard_True);
  thePrs->SetMutable (Standard_True);
  Handle(Graphic3d_Group) aGroup;
  Handle(Prs3d_ShadingAspect) anAspect = new Prs3d_ShadingAspect();
  anAspect->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
  anAspect->SetMaterial (myDrawer->ShadingAspect()->Material());
  anAspect->SetTransparency (myDrawer->ShadingAspect()->Transparency());

  // Display center
  myCenter.Init (myAxes[0].AxisRadius() * 2.0f, gp::Origin());
  aGroup = thePrs->NewGroup ();
  aGroup->SetPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
  aGroup->AddPrimitiveArray (myCenter.Array());

  for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
  {
    // Display axes
    aGroup = thePrs->NewGroup ();

    Handle(Prs3d_ShadingAspect) anAspectAx = new Prs3d_ShadingAspect (new Graphic3d_AspectFillArea3d(*anAspect->Aspect()));
    anAspectAx->SetColor (myAxes[anIt].Color());
    aGroup->SetGroupPrimitivesAspect (anAspectAx->Aspect());
    myAxes[anIt].Compute (thePrsMgr, thePrs, anAspectAx);
    myAxes[anIt].SetTransformPersistence (TransformPersistence());
  }

  updateTransformation();
}

//=======================================================================
//function : HilightSelected
//purpose  : 
//=======================================================================
void AIS_Manipulator::HilightSelected (const Handle(PrsMgr_PresentationManager)& thePM,
                                       const SelectMgr_SequenceOfOwner& theSeq)
{
  if (theSeq.IsEmpty())
  {
    return;
  }

  if (myIsActivationOnDetection)
  {
    return;
  }

  if (!theSeq (1)->IsKind (STANDARD_TYPE (AIS_ManipulatorOwner)))
  {
    thePM->Color (this, GetContext()->HighlightStyle(), 0);
    return;
  }

  Handle(AIS_ManipulatorOwner) anOwner = Handle(AIS_ManipulatorOwner)::DownCast (theSeq (1));
  myHighlightAspect->Aspect()->SetInteriorColor (GetContext()->HighlightStyle()->Color());
  Handle(Graphic3d_Group) aGroup = getGroup (anOwner->Index(), anOwner->Mode());
  if (aGroup.IsNull())
  {
    return;
  }

  if (anOwner->Mode() == AIS_MM_TranslationPlane)
  {
    myDraggerHighlight->SetColor(myAxes[anOwner->Index()].Color());
    aGroup->SetGroupPrimitivesAspect(myDraggerHighlight->Aspect());
  }
  else
    aGroup->SetGroupPrimitivesAspect(myHighlightAspect->Aspect());

  myCurrentIndex = anOwner->Index();
  myCurrentMode = anOwner->Mode();
}

//=======================================================================
//function : ClearSelected
//purpose  :
//=======================================================================
void AIS_Manipulator::ClearSelected()
{
  DeactivateCurrentMode();
}

//=======================================================================
//function : HilightOwnerWithColor
//purpose  : 
//=======================================================================
void AIS_Manipulator::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                             const Handle(Prs3d_Drawer)& theStyle,
                                             const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle(AIS_ManipulatorOwner) anOwner = Handle(AIS_ManipulatorOwner)::DownCast (theOwner);
  Handle(Prs3d_Presentation) aPresentation = getHighlightPresentation (anOwner);
  if (aPresentation.IsNull())
  {
    return;
  }

  aPresentation->CStructure()->ViewAffinity = myViewAffinity;

  if (anOwner->Mode() == AIS_MM_TranslationPlane)
  {
    Handle(Prs3d_Drawer) aStyle = new Prs3d_Drawer();
    aStyle->SetColor (myAxes[anOwner->Index()].Color());
    aStyle->SetTransparency (0.5);
    aPresentation->Highlight (aStyle);
  }
  else
  {
    aPresentation->Highlight (theStyle);
  }

  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (aPresentation->Groups());
       aGroupIter.More(); aGroupIter.Next())
  {
    Handle(Graphic3d_Group)& aGrp = aGroupIter.ChangeValue();
    if (!aGrp.IsNull())
    {
      aGrp->SetGroupPrimitivesAspect (myHighlightAspect->Aspect());
    }
  }
  aPresentation->SetZLayer (Graphic3d_ZLayerId_Topmost);
  thePM->AddToImmediateList (aPresentation);

  if (myIsActivationOnDetection)
  {
    if (HasActiveMode())
    {
      DeactivateCurrentMode();
    }

    myCurrentIndex = anOwner->Index();
    myCurrentMode = anOwner->Mode();
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Manipulator::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                        const Standard_Integer theMode)
{
  //Check mode
  const AIS_ManipulatorMode aMode = (AIS_ManipulatorMode) theMode;
  if (aMode == AIS_MM_None)
  {
    return;
  }
  Handle(SelectMgr_EntityOwner) anOwner;

  // Sensitivity calculation for manipulator parts allows to avoid
  // overlapping of sensitive areas when size of manipulator is small.
  // Sensitivity is calculated relative to the default size of the manipulator (100.0f).
  const Standard_ShortReal aSensitivityCoef = myAxes[0].Size() / 100.0f;
  const Standard_Integer aHighSensitivity = Max (Min (RealToInt (aSensitivityCoef * 15), 15), 3); // clamp sensitivity within range [3, 15]
  const Standard_Integer aLowSensitivity  = Max (Min (RealToInt (aSensitivityCoef * 10), 10), 2); // clamp sensitivity within range [2, 10]

  switch (aMode)
  {
  case AIS_MM_Translation:
  {
    for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
    {
      if (!myAxes[anIt].HasTranslation())
      {
        continue;
      }
      const Axis& anAxis = myAxes[anIt];
      anOwner = new AIS_ManipulatorOwner(this, anIt, AIS_MM_Translation, 9);

      // define sensitivity by line
      Handle(Select3D_SensitiveSegment) aLine = new Select3D_SensitiveSegment(anOwner, gp::Origin(), anAxis.TranslatorTipPosition());
      aLine->SetSensitivityFactor (aHighSensitivity);
      theSelection->Add (aLine);

      // enlarge sensitivity by triangulation
      Handle(Select3D_SensitivePrimitiveArray) aTri = new Select3D_SensitivePrimitiveArray(anOwner);
      aTri->InitTriangulation (anAxis.TriangleArray()->Attributes(), anAxis.TriangleArray()->Indices(), TopLoc_Location());
      theSelection->Add (aTri);
    }
    break;
  }
  case AIS_MM_Rotation:
  {
    for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
    {
      if (!myAxes[anIt].HasRotation())
      {
        continue;
      }
      const Axis& anAxis = myAxes[anIt];
      anOwner = new AIS_ManipulatorOwner(this, anIt, AIS_MM_Rotation, 9);

      // define sensitivity by circle
      const gp_Circ aGeomCircle (gp_Ax2(gp::Origin(), anAxis.ReferenceAxis().Direction()), anAxis.RotatorDiskRadius());
      Handle(Select3D_SensitiveCircle) aCircle = new ManipSensCircle(anOwner, aGeomCircle);
      aCircle->SetSensitivityFactor (aLowSensitivity);
      theSelection->Add(aCircle);
      // enlarge sensitivity by triangulation
      Handle(Select3D_SensitiveTriangulation) aTri = new ManipSensTriangulation(anOwner, myAxes[anIt].RotatorDisk().Triangulation(), anAxis.ReferenceAxis().Direction());
      theSelection->Add (aTri);
    }
    break;
  }
  case AIS_MM_Scaling:
  {
    for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
    {
      if (!myAxes[anIt].HasScaling())
      {
        continue;
      }
      anOwner = new AIS_ManipulatorOwner(this, anIt, AIS_MM_Scaling, 9);

      // define sensitivity by point
      Handle(Select3D_SensitivePoint) aPnt = new Select3D_SensitivePoint(anOwner, myAxes[anIt].ScalerCubePosition());
      aPnt->SetSensitivityFactor (aHighSensitivity);
      theSelection->Add (aPnt);
      // enlarge sensitivity by triangulation
      Handle(Select3D_SensitiveTriangulation) aTri = new Select3D_SensitiveTriangulation(anOwner, myAxes[anIt].ScalerCube().Triangulation(), TopLoc_Location(), Standard_True);
      theSelection->Add (aTri);
    }
    break;
  }
  case AIS_MM_TranslationPlane:
  {
    for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
    {
      if (!myAxes[anIt].HasDragging())
      {
        continue;
      }
      anOwner = new AIS_ManipulatorOwner(this, anIt, AIS_MM_TranslationPlane, 9);

      // define sensitivity by two crossed lines
      Standard_Real aSensitivityOffset = ZoomPersistence() ? aHighSensitivity * (0.5 + M_SQRT2) : 0.0;
      gp_Pnt aP1 = myAxes[((anIt + 1) % 3)].TranslatorTipPosition().Translated (myAxes[((anIt + 2) % 3)].ReferenceAxis().Direction().XYZ() * aSensitivityOffset);
      gp_Pnt aP2 = myAxes[((anIt + 2) % 3)].TranslatorTipPosition().Translated (myAxes[((anIt + 1) % 3)].ReferenceAxis().Direction().XYZ() * aSensitivityOffset);
      gp_XYZ aMidP = (aP1.XYZ() + aP2.XYZ()) / 2.0;
      gp_XYZ anOrig = aMidP.Normalized().Multiplied (aSensitivityOffset);

      Handle(Select3D_SensitiveSegment) aLine1 = new Select3D_SensitiveSegment(anOwner, aP1, aP2);
      aLine1->SetSensitivityFactor(aLowSensitivity);
      theSelection->Add (aLine1);
      Handle(Select3D_SensitiveSegment) aLine2 = new Select3D_SensitiveSegment(anOwner, anOrig, aMidP);
      aLine2->SetSensitivityFactor (aLowSensitivity);
      theSelection->Add (aLine2);

      // enlarge sensitivity by triangulation
      Handle(Select3D_SensitiveTriangulation) aTri = new Select3D_SensitiveTriangulation(anOwner, myAxes[anIt].DraggerSector().Triangulation(), TopLoc_Location(), Standard_True); 
      theSelection->Add (aTri);
    }
    break;
  }
  default:
  {
    anOwner = new SelectMgr_EntityOwner(this, 5);
    break;
  }
  }
}

//=======================================================================
//class    : Disk
//function : Init
//purpose  : 
//=======================================================================
void AIS_Manipulator::Disk::Init (const Standard_ShortReal theInnerRadius,
                                  const Standard_ShortReal theOuterRadius,
                                  const gp_Ax1& thePosition,
                                  const Standard_Integer theSlicesNb,
                                  const Standard_Integer theStacksNb)
{
  myPosition = thePosition;
  myInnerRad = theInnerRadius;
  myOuterRad = theOuterRadius;

  Prs3d_ToolDisk aTool (theInnerRadius, theOuterRadius, theSlicesNb, theStacksNb);
  gp_Ax3 aSystem (myPosition.Location(), myPosition.Direction());
  gp_Trsf aTrsf;
  aTrsf.SetTransformation (aSystem, gp_Ax3());
  myArray = aTool.CreateTriangulation (aTrsf);
  myTriangulation = aTool.CreatePolyTriangulation (aTrsf);
}

//=======================================================================
//class    : Sphere
//function : Init
//purpose  : 
//=======================================================================
void AIS_Manipulator::Sphere::Init (const Standard_ShortReal theRadius,
                                    const gp_Pnt& thePosition,
                                    const Standard_Integer theSlicesNb,
                                    const Standard_Integer theStacksNb)
{
  myPosition = thePosition;
  myRadius = theRadius;

  Prs3d_ToolSphere aTool (theRadius, theSlicesNb, theStacksNb);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation (gp_Vec(gp::Origin(), thePosition));
  myArray = aTool.CreateTriangulation (aTrsf);
  myTriangulation = aTool.CreatePolyTriangulation (aTrsf);
}

//=======================================================================
//class    : Cube
//function : Init
//purpose  : 
//=======================================================================
void AIS_Manipulator::Cube::Init (const gp_Ax1& thePosition, const Standard_ShortReal theSize)
{
  myArray = new Graphic3d_ArrayOfTriangles (12 * 3, 0, Standard_True);

  Poly_Array1OfTriangle aPolyTriangles (1, 12);
  TColgp_Array1OfPnt aPoints (1, 36);
  NCollection_Array1<gp_Dir> aNormals (1, 12);
  myTriangulation = new Poly_Triangulation (aPoints, aPolyTriangles);

  gp_Ax2 aPln (thePosition.Location(), thePosition.Direction());
  gp_Pnt aBottomLeft = thePosition.Location().XYZ() - aPln.XDirection().XYZ() * theSize * 0.5 - aPln.YDirection().XYZ() * theSize * 0.5;
  gp_Pnt aV2 = aBottomLeft.XYZ() + aPln.YDirection().XYZ() * theSize;
  gp_Pnt aV3 = aBottomLeft.XYZ() + aPln.YDirection().XYZ() * theSize + aPln.XDirection().XYZ() * theSize;
  gp_Pnt aV4 = aBottomLeft.XYZ() + aPln.XDirection().XYZ() * theSize;
  gp_Pnt aTopRight = thePosition.Location().XYZ() + thePosition.Direction().XYZ() * theSize
    + aPln.XDirection().XYZ() * theSize * 0.5 + aPln.YDirection().XYZ() * theSize * 0.5;
  gp_Pnt aV5 = aTopRight.XYZ() - aPln.YDirection().XYZ() * theSize;
  gp_Pnt aV6 = aTopRight.XYZ() - aPln.YDirection().XYZ() * theSize - aPln.XDirection().XYZ() * theSize;
  gp_Pnt aV7 = aTopRight.XYZ() - aPln.XDirection().XYZ() * theSize;

  gp_Dir aRight ((gp_Vec(aTopRight, aV7) ^ gp_Vec(aTopRight, aV2)).XYZ());
  gp_Dir aFront ((gp_Vec(aV3, aV4) ^ gp_Vec(aV3, aV5)).XYZ());

  // Bottom
  addTriangle (0, aBottomLeft, aV2, aV3, -thePosition.Direction());
  addTriangle (1, aBottomLeft, aV3, aV4, -thePosition.Direction());

  // Front
  addTriangle (2, aV3, aV5, aV4, -aFront);
  addTriangle (3, aV3, aTopRight, aV5, -aFront);

  // Back
  addTriangle (4, aBottomLeft, aV7, aV2, aFront);
  addTriangle (5, aBottomLeft, aV6, aV7, aFront);

  // aTop
  addTriangle (6, aV7, aV6, aV5, thePosition.Direction());
  addTriangle (7, aTopRight, aV7, aV5, thePosition.Direction());

  // Left
  addTriangle (8, aV6, aV4, aV5, aRight);
  addTriangle (9, aBottomLeft, aV4, aV6, aRight);

  // Right
  addTriangle (10, aV3, aV7, aTopRight, -aRight);
  addTriangle (11, aV3, aV2, aV7, -aRight);
}

//=======================================================================
//class    : Cube
//function : addTriangle
//purpose  : 
//=======================================================================
void AIS_Manipulator::Cube::addTriangle (const Standard_Integer theIndex,
                                         const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3,
                                         const gp_Dir& theNormal)
{
  myTriangulation->SetNode (theIndex * 3 + 1, theP1);
  myTriangulation->SetNode (theIndex * 3 + 2, theP2);
  myTriangulation->SetNode (theIndex * 3 + 3, theP3);

  myTriangulation->SetTriangle (theIndex + 1, Poly_Triangle (theIndex * 3 + 1, theIndex * 3 + 2, theIndex * 3 + 3));
  myArray->AddVertex (theP1, theNormal);
  myArray->AddVertex (theP2, theNormal);
  myArray->AddVertex (theP3, theNormal);
}

//=======================================================================
//class    : Sector
//function : Init
//purpose  : 
//=======================================================================
void AIS_Manipulator::Sector::Init (const Standard_ShortReal theRadius,
                                    const gp_Ax1&            thePosition,
                                    const gp_Dir&            theXDirection,
                                    const Standard_Integer   theSlicesNb,
                                    const Standard_Integer   theStacksNb)
{
  Prs3d_ToolSector aTool(theRadius, theSlicesNb, theStacksNb);
  gp_Ax3 aSystem(thePosition.Location(), thePosition.Direction(), theXDirection);
  gp_Trsf aTrsf;
  aTrsf.SetTransformation(aSystem, gp_Ax3());
  myArray = aTool.CreateTriangulation (aTrsf);
  myTriangulation = aTool.CreatePolyTriangulation (aTrsf);
}

//=======================================================================
//class    : Axis
//function : Constructor
//purpose  : 
//=======================================================================
AIS_Manipulator::Axis::Axis (const gp_Ax1& theAxis,
                             const Quantity_Color& theColor,
                             const Standard_ShortReal theLength)
: myReferenceAxis (theAxis),
  myPosition (theAxis),
  myColor (theColor),
  myHasTranslation (Standard_True),
  myLength (theLength),
  myAxisRadius (0.5f),
  myHasScaling (Standard_True),
  myBoxSize (2.0f),
  myHasRotation (Standard_True),
  myInnerRadius (myLength + myBoxSize),
  myDiskThickness (myBoxSize * 0.5f),
  myIndent (0.2f),
  myHasDragging(Standard_True),
  myFacettesNumber (20),
  myCircleRadius (myLength + myBoxSize + myBoxSize * 0.5f * 0.5f)
{
  //
}

//=======================================================================
//class    : Axis
//function : Compute
//purpose  :
//=======================================================================

void AIS_Manipulator::Axis::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                     const Handle(Prs3d_Presentation)& thePrs,
                                     const Handle(Prs3d_ShadingAspect)& theAspect)
{
  if (myHasTranslation)
  {
    const Standard_Real anArrowLength   = 0.25 * myLength;
    const Standard_Real aCylinderLength = myLength - anArrowLength;
    myArrowTipPos = gp_Pnt (0.0, 0.0, 0.0).Translated (myReferenceAxis.Direction().XYZ() * aCylinderLength);

    myTriangleArray = Prs3d_Arrow::DrawShaded (gp_Ax1(gp::Origin(), myReferenceAxis.Direction()),
                                               myAxisRadius,
                                               myLength,
                                               myAxisRadius * 1.5,
                                               anArrowLength,
                                               myFacettesNumber);
    myTranslatorGroup = thePrs->NewGroup();
    myTranslatorGroup->SetClosed (true);
    myTranslatorGroup->SetGroupPrimitivesAspect (theAspect->Aspect());
    myTranslatorGroup->AddPrimitiveArray (myTriangleArray);

    if (myHighlightTranslator.IsNull())
    {
      myHighlightTranslator = new Prs3d_Presentation (thePrsMgr->StructureManager());
    }
    else
    {
      myHighlightTranslator->Clear();
    }
    {
      Handle(Graphic3d_Group) aGroup = myHighlightTranslator->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect (theAspect->Aspect());
      aGroup->AddPrimitiveArray (myTriangleArray);
    }
  }

  if (myHasScaling)
  {
    myCubePos = myReferenceAxis.Direction().XYZ() * (myLength + myIndent);
    myCube.Init (gp_Ax1 (myCubePos, myReferenceAxis.Direction()), myBoxSize);

    myScalerGroup = thePrs->NewGroup();
    myScalerGroup->SetClosed (true);
    myScalerGroup->SetGroupPrimitivesAspect (theAspect->Aspect());
    myScalerGroup->AddPrimitiveArray (myCube.Array());

    if (myHighlightScaler.IsNull())
    {
      myHighlightScaler = new Prs3d_Presentation (thePrsMgr->StructureManager());
    }
    else
    {
      myHighlightScaler->Clear();
    }
    {
      Handle(Graphic3d_Group) aGroup = myHighlightScaler->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect (theAspect->Aspect());
      aGroup->AddPrimitiveArray (myCube.Array());
    }
  }

  if (myHasRotation)
  {
    myCircleRadius = myInnerRadius + myIndent * 2 + myDiskThickness * 0.5f;
    myCircle.Init (myInnerRadius + myIndent * 2, myInnerRadius + myDiskThickness + myIndent * 2, gp_Ax1(gp::Origin(), myReferenceAxis.Direction()), myFacettesNumber * 2);
    myRotatorGroup = thePrs->NewGroup ();
    myRotatorGroup->SetGroupPrimitivesAspect (theAspect->Aspect());
    myRotatorGroup->AddPrimitiveArray (myCircle.Array());

    if (myHighlightRotator.IsNull())
    {
      myHighlightRotator = new Prs3d_Presentation (thePrsMgr->StructureManager());
    }
    else
    {
      myHighlightRotator->Clear();
    }
    {
      Handle(Graphic3d_Group) aGroup = myHighlightRotator->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect (theAspect->Aspect());
      aGroup->AddPrimitiveArray (myCircle.Array());
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

    mySector.Init(myInnerRadius + myIndent * 2, gp_Ax1(gp::Origin(), myReferenceAxis.Direction()), aXDirection, myFacettesNumber * 2);
    myDraggerGroup = thePrs->NewGroup();

    Handle(Graphic3d_AspectFillArea3d) aFillArea = new Graphic3d_AspectFillArea3d();
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
      Handle(Graphic3d_Group) aGroup = myHighlightDragger->CurrentGroup();
      aGroup->SetGroupPrimitivesAspect(aFillArea);
      aGroup->AddPrimitiveArray(mySector.Array());
    }
  }
}
