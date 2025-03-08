// Created on: 2011-10-20
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <IVtkOCC_ViewerSelector.hxx>

#include <Select3D_SensitiveBox.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Graphic3d_Camera.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtkOCC_ViewerSelector, SelectMgr_ViewerSelector)

//============================================================================
// Method:  Constructor
// Purpose:
//============================================================================
IVtkOCC_ViewerSelector::IVtkOCC_ViewerSelector()
    : SelectMgr_ViewerSelector(),
      myPixTol(2),
      myToUpdateTol(Standard_True)
{
}

//============================================================================
// Method:  Destructor
// Purpose:
//============================================================================
IVtkOCC_ViewerSelector::~IVtkOCC_ViewerSelector() {}

//============================================================================
// Method: ConvertVtkToOccCamera
// Purpose:
//============================================================================
Handle(Graphic3d_Camera) IVtkOCC_ViewerSelector::ConvertVtkToOccCamera(
  const IVtk_IView::Handle& theView)
{
  Handle(Graphic3d_Camera) aCamera = new Graphic3d_Camera();
  aCamera->SetZeroToOneDepth(true);
  Standard_Boolean isOrthographic = !theView->IsPerspective();
  aCamera->SetProjectionType(isOrthographic ? Graphic3d_Camera::Projection_Orthographic
                                            : Graphic3d_Camera::Projection_Perspective);
  if (isOrthographic)
  {
    aCamera->SetScale(2 * theView->GetParallelScale());
  }
  else
  {
    aCamera->SetFOVy(theView->GetViewAngle());
  }
  Standard_Real aZNear = 0.0, aZFar = 0.0;
  theView->GetClippingRange(aZNear, aZFar);
  aCamera->SetZRange(aZNear, aZFar);
  aCamera->SetAspect(theView->GetAspectRatio());
  gp_XYZ anAxialScale;
  theView->GetScale(anAxialScale.ChangeCoord(1),
                    anAxialScale.ChangeCoord(2),
                    anAxialScale.ChangeCoord(3));
  aCamera->SetAxialScale(anAxialScale);

  gp_XYZ anUp, aDir, anEyePos;
  theView->GetViewUp(anUp.ChangeCoord(1), anUp.ChangeCoord(2), anUp.ChangeCoord(3));
  theView->GetDirectionOfProjection(aDir.ChangeCoord(1), aDir.ChangeCoord(2), aDir.ChangeCoord(3));
  theView->GetEyePosition(anEyePos.ChangeCoord(1),
                          anEyePos.ChangeCoord(2),
                          anEyePos.ChangeCoord(3));

  aCamera->SetDistance(theView->GetDistance());
  aCamera->SetUp(anUp);
  aCamera->SetDirectionFromEye(aDir.Reversed());
  aCamera->MoveEyeTo(anEyePos);

  return aCamera;
}

//============================================================================
// Method:  Pick
// Purpose: Implements point picking
//============================================================================
void IVtkOCC_ViewerSelector::Pick(const Standard_Integer    theXPix,
                                  const Standard_Integer    theYPix,
                                  const IVtk_IView::Handle& theView)
{
  gp_Pnt2d aMousePos(static_cast<Standard_Real>(theXPix), static_cast<Standard_Real>(theYPix));
  mySelectingVolumeMgr.InitPointSelectingVolume(aMousePos);

  if (myToUpdateTol)
  {
    // Compute and set a sensitivity tolerance according to the renderer (viewport).
    // TODO: Think if this works well in perspective view...'cause result depends
    // on position on the screen, but we always use the point close to the
    // screen's origin...
    mySelectingVolumeMgr.SetPixelTolerance(myPixTol);

    myToUpdateTol = Standard_False;
  }

  mySelectingVolumeMgr.SetCamera(ConvertVtkToOccCamera(theView));

  Standard_Integer aWidth = 0, aHeight = 0;
  theView->GetWindowSize(aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize(aWidth, aHeight);
  Standard_Real aX = RealLast(), aY = RealLast();
  Standard_Real aVpWidth = RealLast(), aVpHeight = RealLast();
  theView->GetViewport(aX, aY, aVpWidth, aVpHeight);
  mySelectingVolumeMgr.SetViewport(aX, aY, aVpWidth, aVpHeight);

  mySelectingVolumeMgr.BuildSelectingVolume();

  TraverseSensitives(-1);
}

//============================================================================
// Method:  Pick
// Purpose: Picking by rectangle
//============================================================================
void IVtkOCC_ViewerSelector::Pick(const Standard_Integer    theXMin,
                                  const Standard_Integer    theYMin,
                                  const Standard_Integer    theXMax,
                                  const Standard_Integer    theYMax,
                                  const IVtk_IView::Handle& theView)
{
  gp_Pnt2d aMinMousePos(static_cast<Standard_Real>(theXMin), static_cast<Standard_Real>(theYMin));
  gp_Pnt2d aMaxMousePos(static_cast<Standard_Real>(theXMax), static_cast<Standard_Real>(theYMax));
  mySelectingVolumeMgr.InitBoxSelectingVolume(aMinMousePos, aMaxMousePos);

  if (myToUpdateTol)
  {
    // Compute and set a sensitivity tolerance according to the renderer (viewport).
    // TODO: Think if this works well in perspective view...'cause result depends
    // on position on the screen, but we always use the point close to the
    // screen's origin...
    mySelectingVolumeMgr.SetPixelTolerance(myPixTol);

    myToUpdateTol = Standard_False;
  }

  Standard_Integer aWidth = 0, aHeight = 0;
  Graphic3d_Mat4d  aProj, anOrient;
  Standard_Real    aX = RealLast(), aY = RealLast();
  Standard_Real    aVpWidth = RealLast(), aVpHeight = RealLast();

  mySelectingVolumeMgr.SetCamera(ConvertVtkToOccCamera(theView));

  theView->GetWindowSize(aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize(aWidth, aHeight);

  theView->GetViewport(aX, aY, aVpWidth, aVpHeight);
  mySelectingVolumeMgr.SetViewport(aX, aY, aVpWidth, aVpHeight);

  mySelectingVolumeMgr.BuildSelectingVolume();

  TraverseSensitives(-1);
}

//============================================================================
// Method:  Pick
// Purpose:
//============================================================================
void IVtkOCC_ViewerSelector::Pick(double**                  thePoly,
                                  const int                 theNbPoints,
                                  const IVtk_IView::Handle& theView)
{
  // Build TColgp_Array1OfPnt2d from input array of doubles
  TColgp_Array1OfPnt2d aPolyline(1, theNbPoints);
  for (Standard_Integer anIt = 0; anIt < theNbPoints; anIt++)
  {
    gp_XY aDispPnt = thePoly[anIt][2] != 0 ? gp_XY(thePoly[anIt][0] / thePoly[anIt][2],
                                                   thePoly[anIt][1] / thePoly[anIt][2])
                                           : gp_XY(thePoly[anIt][0], thePoly[anIt][1]);
    aPolyline.SetValue(anIt + 1, aDispPnt);
  }
  mySelectingVolumeMgr.InitPolylineSelectingVolume(aPolyline);

  if (myToUpdateTol)
  {
    // Compute and set a sensitivity tolerance according to the renderer (viewport).
    // TODO: Think if this works well in perspective view...'cause result depends
    // on position on the screen, but we always use the point close to the
    // screen's origin...
    mySelectingVolumeMgr.SetPixelTolerance(myPixTol);

    myToUpdateTol = Standard_False;
  }

  Standard_Integer aWidth = 0, aHeight = 0;
  Graphic3d_Mat4d  aProj, anOrient;
  Standard_Real    aX = RealLast(), aY = RealLast();
  Standard_Real    aVpWidth = RealLast(), aVpHeight = RealLast();

  mySelectingVolumeMgr.SetCamera(ConvertVtkToOccCamera(theView));

  theView->GetWindowSize(aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize(aWidth, aHeight);

  theView->GetViewport(aX, aY, aVpWidth, aVpHeight);
  mySelectingVolumeMgr.SetViewport(aX, aY, aVpWidth, aVpHeight);

  mySelectingVolumeMgr.BuildSelectingVolume();

  TraverseSensitives(-1);
}

//============================================================================
// Method:  Activate
// Purpose: Activates the given selection
//============================================================================
void IVtkOCC_ViewerSelector::Activate(const Handle(SelectMgr_Selection)& theSelection)
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    aSelEntIter.Value()->SetActiveForSelection();
  }

  if (theSelection->GetSelectionState() != SelectMgr_SOS_Activated)
  {
    theSelection->SetSelectionState(SelectMgr_SOS_Activated);
    myTolerances.Add(theSelection->Sensitivity());
  }
}

//============================================================================
// Method:  Deactivate
// Purpose: Deactivate the given selection
//============================================================================
void IVtkOCC_ViewerSelector::Deactivate(const Handle(SelectMgr_Selection)& theSelection)
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    aSelEntIter.Value()->ResetSelectionActiveStatus();
  }

  if (theSelection->GetSelectionState() == SelectMgr_SOS_Activated)
  {
    theSelection->SetSelectionState(SelectMgr_SOS_Deactivated);
    myTolerances.Decrement(theSelection->Sensitivity());
  }
}
