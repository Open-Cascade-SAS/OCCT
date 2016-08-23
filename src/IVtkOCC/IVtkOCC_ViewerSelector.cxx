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


IMPLEMENT_STANDARD_RTTIEXT(IVtkOCC_ViewerSelector,SelectMgr_ViewerSelector)

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
IVtkOCC_ViewerSelector::~IVtkOCC_ViewerSelector()
{
}

//============================================================================
// Method:  Pick
// Purpose: Implements point picking
//============================================================================
void IVtkOCC_ViewerSelector::Pick (const Standard_Integer theXPix,
                                   const Standard_Integer theYPix,
                                   const IVtk_IView::Handle&    theView)
{
  if (myToUpdateTol)
  {
    // Compute and set a sensitivity tolerance according to the renderer (viewport).
    // TODO: Think if this works well in perspective view...'cause result depends
    // on position on the screen, but we always use the point close to the
    // screen's origin...
    mySelectingVolumeMgr.SetPixelTolerance (myPixTol);

    myToUpdateTol = Standard_False;
  }

  Standard_Integer aWidth = 0, aHeight = 0;
  Graphic3d_Mat4d aProj, anOrient;
  Standard_Boolean isOrthographic = Standard_False;
  Standard_Real aX = RealLast(), aY = RealLast();
  Standard_Real aVpWidth = RealLast(), aVpHeight = RealLast();

  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Point);
  theView->GetCamera (aProj, anOrient, isOrthographic);
  mySelectingVolumeMgr.SetCamera (aProj, anOrient, isOrthographic);

  theView->GetWindowSize (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);

  theView->GetViewport (aX, aY, aVpWidth, aVpHeight);
  mySelectingVolumeMgr.SetViewport (aX, aY, aVpWidth, aVpHeight);

  gp_Pnt2d aMousePos (static_cast<Standard_Real> (theXPix),
                      static_cast<Standard_Real> (theYPix));
  mySelectingVolumeMgr.BuildSelectingVolume (aMousePos);

  TraverseSensitives();
}

//============================================================================
// Method:  Pick
// Purpose: Picking by rectangle
//============================================================================
void IVtkOCC_ViewerSelector::Pick (const Standard_Integer    theXMin,
                                   const Standard_Integer    theYMin,
                                   const Standard_Integer    theXMax,
                                   const Standard_Integer    theYMax,
                                   const IVtk_IView::Handle& theView)
{
  if (myToUpdateTol)
  {
    // Compute and set a sensitivity tolerance according to the renderer (viewport).
    // TODO: Think if this works well in perspective view...'cause result depends
    // on position on the screen, but we always use the point close to the
    // screen's origin...
    mySelectingVolumeMgr.SetPixelTolerance (myPixTol);

    myToUpdateTol = Standard_False;
  }

  Standard_Integer aWidth = 0, aHeight = 0;
  Graphic3d_Mat4d aProj, anOrient;
  Standard_Boolean isOrthographic = Standard_False;
  Standard_Real aX = RealLast(), aY = RealLast();
  Standard_Real aVpWidth = RealLast(), aVpHeight = RealLast();

  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Box);
  theView->GetCamera (aProj, anOrient, isOrthographic);
  mySelectingVolumeMgr.SetCamera (aProj, anOrient, isOrthographic);

  theView->GetWindowSize (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);

  theView->GetViewport (aX, aY, aVpWidth, aVpHeight);
  mySelectingVolumeMgr.SetViewport (aX, aY, aVpWidth, aVpHeight);

  gp_Pnt2d aMinMousePos (static_cast<Standard_Real> (theXMin),
                         static_cast<Standard_Real> (theYMin));
  gp_Pnt2d aMaxMousePos (static_cast<Standard_Real> (theXMax),
                         static_cast<Standard_Real> (theYMax));

  mySelectingVolumeMgr.BuildSelectingVolume (aMinMousePos,
                                             aMaxMousePos);

  TraverseSensitives();
}

//============================================================================
// Method:  Pick
// Purpose:
//============================================================================
void IVtkOCC_ViewerSelector::Pick (double**                  thePoly,
                                   const int                 theNbPoints,
                                   const IVtk_IView::Handle& theView)
{
  TColgp_Array1OfPnt2d aPolyline (1, theNbPoints);

  if (myToUpdateTol)
  {
    // Compute and set a sensitivity tolerance according to the renderer (viewport).
    // TODO: Think if this works well in perspective view...'cause result depends
    // on position on the screen, but we always use the point close to the
    // screen's origin...
    mySelectingVolumeMgr.SetPixelTolerance (myPixTol);

    myToUpdateTol = Standard_False;
  }

  // Build TColgp_Array1OfPnt2d from input array of doubles
  gp_XYZ aWorldPnt;

  for (Standard_Integer anIt = 0; anIt < theNbPoints; anIt++)
  {
    gp_XY aDispPnt = thePoly[anIt][2] != 0 ? gp_XY (thePoly[anIt][0] / thePoly[anIt][2], thePoly[anIt][1] / thePoly[anIt][2])
                                           : gp_XY (thePoly[anIt][0], thePoly[anIt][1]);
    aPolyline.SetValue (anIt + 1, aDispPnt);
  }

  Standard_Integer aWidth = 0, aHeight = 0;
  Graphic3d_Mat4d aProj, anOrient;
  Standard_Boolean isOrthographic = Standard_False;
  Standard_Real aX = RealLast(), aY = RealLast();
  Standard_Real aVpWidth = RealLast(), aVpHeight = RealLast();

  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Polyline);
  theView->GetCamera (aProj, anOrient, isOrthographic);
  mySelectingVolumeMgr.SetCamera (aProj, anOrient, isOrthographic);

  theView->GetWindowSize (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);

  theView->GetViewport (aX, aY, aVpWidth, aVpHeight);
  mySelectingVolumeMgr.SetViewport (aX, aY, aVpWidth, aVpHeight);

  mySelectingVolumeMgr.BuildSelectingVolume (aPolyline);

  TraverseSensitives();
}

//============================================================================
// Method:  Activate
// Purpose: Activates the given selection
//============================================================================
void IVtkOCC_ViewerSelector::Activate (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    theSelection->Sensitive()->SetActiveForSelection();
  }

  theSelection->SetSelectionState (SelectMgr_SOS_Activated);

  myTolerances.Add (theSelection->Sensitivity());
  myToUpdateTolerance = Standard_True;
}

//============================================================================
// Method:  Deactivate
// Purpose: Deactivate the given selection
//============================================================================
void IVtkOCC_ViewerSelector::Deactivate (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    theSelection->Sensitive()->ResetSelectionActiveStatus();
  }

  theSelection->SetSelectionState (SelectMgr_SOS_Deactivated);

  myTolerances.Decrement (theSelection->Sensitivity());
  myToUpdateTolerance = Standard_True;
}
