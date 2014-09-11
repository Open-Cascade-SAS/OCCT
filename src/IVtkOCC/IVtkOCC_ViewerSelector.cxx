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
#include <gp_Quaternion.hxx>
#include <Graphic3d_Camera.hxx>

IMPLEMENT_STANDARD_HANDLE( IVtkOCC_ViewerSelector, SelectMgr_ViewerSelector )
IMPLEMENT_STANDARD_RTTIEXT( IVtkOCC_ViewerSelector, SelectMgr_ViewerSelector )

//============================================================================
// Method:  Constructor
// Purpose:
//============================================================================
IVtkOCC_ViewerSelector::IVtkOCC_ViewerSelector()
: SelectMgr_ViewerSelector(),
myPixTol(2),
myToUpdateTol(Standard_True)
{
  for (Standard_Integer i=0;i<=13;i++) {myCoeff [i] = 0.;myPrevCoeff[i]=0.0;} 
  for (Standard_Integer j=0;j<2;j++) {myCenter [j] = 0.;myPrevCenter[j]=0.0;} 
}

//============================================================================
// Method:  Convert
// Purpose: Projects all sensitive entities from the given selection container
//          to 2D space
//============================================================================
void IVtkOCC_ViewerSelector::Convert (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    if(theSelection->Sensitive()->NeedsConversion())
    {
      Handle(Select3D_SensitiveEntity) aSensEntity = 
        *((Handle(Select3D_SensitiveEntity)*) &(theSelection->Sensitive()));
      aSensEntity->Project (myPrj);
      if (!tosort)
      {
          tosort = Standard_True;
      }
    }
  }
}

//============================================================================
// Method:  Pick
// Purpose: Implements point picking
//============================================================================
void IVtkOCC_ViewerSelector::Pick (const Standard_Integer theXPix,
                                   const Standard_Integer theYPix,
                                   const IVtk_IView::Handle&    theView)
{
  myclip.SetVoid();
  Update (theView);
  gp_XY aDispPnt (theXPix, theYPix);
  gp_XYZ aWorldPnt;
  gp_Pnt2d aP2d;
  theView->DisplayToWorld (aDispPnt, aWorldPnt);
  myPrj->Project (gp_Pnt (aWorldPnt), aP2d);
  InitSelect (aP2d.X(), aP2d.Y());
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
    gp_XYZ aWorldPnt1, aWorldPnt2;
    gp_XY aDispPnt1 (0.0, 0.0);
    gp_XY aDispPnt2 (myPixTol, 0.0);
    theView->DisplayToWorld (aDispPnt1, aWorldPnt1);
    theView->DisplayToWorld (aDispPnt2, aWorldPnt2);
    gp_Pnt aPnt1 (aWorldPnt1);
    gp_Pnt aPnt2 (aWorldPnt2);
    SetSensitivity (aPnt2.Distance (aPnt1));
    myToUpdateTol = Standard_False;
  }
  Update (theView);

  gp_XY aDispPnt1 (theXMin, theYMin);
  gp_XY aDispPnt2 (theXMax, theYMax);
  gp_XYZ aWorldPnt1, aWorldPnt2;

  gp_Pnt2d aP2d_1, aP2d_2;
  theView->DisplayToWorld (aDispPnt1, aWorldPnt1);
  theView->DisplayToWorld (aDispPnt2, aWorldPnt2);

  myPrj->Project (gp_Pnt (aWorldPnt1), aP2d_1);
  myPrj->Project (gp_Pnt (aWorldPnt2), aP2d_2);

  InitSelect (Min (aP2d_1.X(), aP2d_2.X()),
              Min (aP2d_1.Y(), aP2d_2.Y()),
              Max (aP2d_1.X(), aP2d_2.X()),
              Max (aP2d_1.Y(), aP2d_2.Y()));
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
    gp_XYZ aWorldPnt1, aWorldPnt2;
    gp_XY aDispPnt1 (0.0, 0.0);
    gp_XY aDispPnt2 (myPixTol, 0.0);
    theView->DisplayToWorld (aDispPnt1, aWorldPnt1);
    theView->DisplayToWorld (aDispPnt2, aWorldPnt2);
    gp_Pnt aPnt1 (aWorldPnt1);
    gp_Pnt aPnt2 (aWorldPnt2);
    SetSensitivity (aPnt2.Distance (aPnt1));
    myToUpdateTol = Standard_False;
  }

  Update (theView);

  // Build TColgp_Array1OfPnt2d from input array of doubles
  gp_XYZ aWorldPnt;

  for (Standard_Integer anIt = 0; anIt < theNbPoints; anIt++)
  {
    gp_XY aDispPnt = thePoly[anIt][2] != 0 ? gp_XY (thePoly[anIt][0] / thePoly[anIt][2], thePoly[anIt][1] / thePoly[anIt][2])
                                           : gp_XY (thePoly[anIt][0], thePoly[anIt][1]);
    gp_Pnt2d aP2d;
    theView->DisplayToWorld (aDispPnt, aWorldPnt);
    myPrj->Project (gp_Pnt (aWorldPnt), aP2d);
    aPolyline.SetValue (anIt + 1, aP2d);
  }

  InitSelect (aPolyline);
}

//============================================================================
// Method:  Update
// Purpose:  Checks if some projection parameters have changed,
//      and updates the 2D projections of all sensitive entities if necessary.
//============================================================================
Standard_Boolean  IVtkOCC_ViewerSelector::Update (const IVtk_IView::Handle& theView)
{
  static Standard_Real aZoom (0.0);

  // No focal distance by default
  myPrevCoeff[9] = 0.0;
  // Parallel projection by default
  myPrevCoeff[10] = 0.0;

  // Flag related to perspective or parallel projection 
  Standard_Boolean isPerspective = theView->IsPerspective();

  // For perspective projections only
  if (isPerspective)
  {
    // Flag = 1 if perspective projection
    myPrevCoeff[10] = 1.0;
    // Focal distance
    myPrevCoeff[9] = theView->GetDistance();
  }
  // View point
  // Use (0,0,0) as a view reference point: 

  theView->GetPosition (myPrevCoeff[0], myPrevCoeff[1], myPrevCoeff[2]);

  // Orientation
  theView->GetViewUp (myPrevCoeff[3], myPrevCoeff[4], myPrevCoeff[5]);
  // Projection direction vector
  theView->GetDirectionOfProjection (myPrevCoeff[6], myPrevCoeff[7], myPrevCoeff[8]);

  // 3D Scale
  theView->GetScale (myPrevCoeff[11], myPrevCoeff[12], myPrevCoeff[13]);

  // Return the center of this viewport in display coordinates.
  theView->GetViewCenter (myPrevCenter[0], myPrevCenter[1]); 

  Standard_Integer anIt;

  for (anIt=0; anIt <= 13 && (myPrevCoeff[anIt] == myCoeff[anIt]); anIt++) { }

  if (anIt <= 13 || (myPrevCenter[0] != myCenter[0]) || (myPrevCenter[1] != myCenter[1]))
  {
    toupdate    = Standard_True;
    myToUpdateTol = Standard_True;

    for (Standard_Integer anI = anIt; anI <= 13; anI++)
    {
      myCoeff[anI] = myPrevCoeff[anI];
    }

    for (Standard_Integer aJ = 0; aJ < 2; aJ++)
    {
      myCenter[aJ] = myPrevCenter[aJ];
    }

    // For orthographic view use only direction of projection and up vector
    // Panning, and zooming has no effect on 2D selection sensitives.
    Handle (Graphic3d_Camera) aCamera = new Graphic3d_Camera();

    aCamera->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
    aCamera->SetCenter (gp::Origin());
    aCamera->SetDirection (gp_Dir (-myCoeff[6], -myCoeff[7], -myCoeff[8]));
    aCamera->SetUp (gp_Dir (myCoeff[3], myCoeff[4], myCoeff[5]));
    aCamera->SetDistance (1.0);
    aCamera->SetAxialScale (gp_XYZ (myCoeff[11], myCoeff[12], myCoeff[13]));

    myPrj = new Select3D_Projector (aCamera->OrientationMatrix(), Graphic3d_Mat4d());
  }

  if (isPerspective)
  {
    if (Abs(theView->GetViewAngle() - aZoom) > 1.e-3)
    {
      myToUpdateTol = Standard_True;
      aZoom = theView->GetViewAngle();
    }
  }
  else
  {
    if (Abs (theView->GetParallelScale() - aZoom) > 1.e-3)
    {
      myToUpdateTol = Standard_True;
      aZoom = theView->GetParallelScale();
    }
  }

  if(myToUpdateTol)
  {
    // Compute and set a sensitivity tolerance according to the view
    gp_XYZ aWorldPnt1, aWorldPnt2;
    gp_XY aDispPnt1 (0.0, 0.0);
    gp_XY aDispPnt2 (myPixTol, 0.0);

    theView->DisplayToWorld (aDispPnt1, aWorldPnt1);
    theView->DisplayToWorld (aDispPnt2, aWorldPnt2);
    gp_Pnt aPnt1 (aWorldPnt1);
    gp_Pnt aPnt2 (aWorldPnt2);
    SetSensitivity (aPnt2.Distance (aPnt1));

    myToUpdateTol = Standard_False;
  }

  if(toupdate) UpdateConversion();
  if(tosort) UpdateSort();

  return Standard_True;
}

//============================================================================
// Method:  Activate
// Purpose: Activates the given selection
//============================================================================
void IVtkOCC_ViewerSelector::Activate (const Handle(SelectMgr_Selection)& theSelection,
                                       const Standard_Boolean             theIsAutomaticProj)
{
  tosort = Standard_True;

  if (!myselections.IsBound (theSelection))
  {
    myselections.Bind (theSelection, 0);
  } 
  else if (myselections (theSelection) != 0)
  {
    myselections (theSelection) = 0;
  }
  if (theIsAutomaticProj)
  {
    Convert (theSelection);
  }
}

//============================================================================
// Method:  Deactivate
// Purpose: Deactivate the given selection
//============================================================================
void IVtkOCC_ViewerSelector::Deactivate (const Handle(SelectMgr_Selection)& theSelection)
{
  if (myselections.IsBound (theSelection))
  {
    myselections (theSelection) = 1;
    tosort = Standard_True;
  }
}

//============================================================================
// Method:  PickingLine
// Purpose: Deactivate the given selection
//============================================================================
gp_Lin IVtkOCC_ViewerSelector::PickingLine (const Standard_Real theX,const Standard_Real theY) const
{
  return myPrj->Shoot (theX, theY);
}
