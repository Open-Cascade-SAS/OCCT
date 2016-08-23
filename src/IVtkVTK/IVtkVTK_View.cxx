// Created on: 2011-10-14 
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


#include <IVtkVTK_View.hxx>
#include <vtkAutoInit.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkTransform.h>

IMPLEMENT_STANDARD_RTTIEXT(IVtkVTK_View,IVtk_IView)

// Initialization of VTK object factories.
// Since VTK 6 the factory methods require "auto-initialization" depending on
// what modules are enabled at VTK configure time.
// Some defines are needed in order to make the factories work properly.
#ifdef VTK_OPENGL2_BACKEND
VTK_MODULE_INIT(vtkRenderingOpenGL2)
#else
VTK_MODULE_INIT(vtkRenderingOpenGL)
#endif
VTK_MODULE_INIT(vtkInteractionStyle)

// Handle implementation


//================================================================
// Function : Constructor
// Purpose  : 
//================================================================
IVtkVTK_View::IVtkVTK_View (vtkRenderer* theRenderer)
: myRenderer (theRenderer)
{ }

//================================================================
// Function : Destructor
// Purpose  : 
//================================================================
IVtkVTK_View::~IVtkVTK_View()
{ }

//================================================================
// Function : IsPerspective
// Purpose  : 
//================================================================
bool IVtkVTK_View::IsPerspective() const
{
  return !myRenderer->GetActiveCamera()->GetParallelProjection();
}

//================================================================
// Function : GetDistance
// Purpose  : 
//================================================================
double IVtkVTK_View::GetDistance() const
{
  return myRenderer->GetActiveCamera()->GetDistance();
}

//================================================================
// Function : GetPosition
// Purpose  : 
//================================================================
void IVtkVTK_View::GetPosition (double& theX, double& theY, double& theZ) const
{
  myRenderer->GetActiveCamera()->GetFocalPoint (theX, theY, theZ);
}

//================================================================
// Function : GetViewUp
// Purpose  : 
//================================================================
void IVtkVTK_View::GetViewUp (double& theDx, double& theDy, double& theDz) const
{
  myRenderer->GetActiveCamera()->OrthogonalizeViewUp();
  myRenderer->GetActiveCamera()->GetViewUp (theDx, theDy, theDz);
}

//================================================================
// Function : GetDirectionOfProjection
// Purpose  : 
//================================================================
void IVtkVTK_View::GetDirectionOfProjection (double& theDx,
                                             double& theDy,
                                             double& theDz) const
{
  myRenderer->GetActiveCamera()->GetDirectionOfProjection (theDx, theDy, theDz);
  theDx = -theDx;
  theDy = -theDy;
  theDz = -theDz;
}

//================================================================
// Function : GetScale
// Purpose  : 
//================================================================
void IVtkVTK_View::GetScale (double& theX, double& theY, double& theZ) const
{
  double aScale[3];
  myRenderer->GetActiveCamera()->GetViewTransformObject()->GetScale (aScale);
  theX = aScale[0];
  theY = aScale[1];
  theZ = aScale[2];
}

//================================================================
// Function : GetParallelScale
// Purpose  : 
//================================================================
double IVtkVTK_View::GetParallelScale() const
{
  return myRenderer->GetActiveCamera()->GetParallelScale();
}

//================================================================
// Function : GetViewAngle
// Purpose  : 
//================================================================
double IVtkVTK_View::GetViewAngle() const
{
  return myRenderer->GetActiveCamera()->GetViewAngle();
}

//================================================================
// Function : GetViewCenter
// Purpose  : 
//================================================================
void IVtkVTK_View::GetViewCenter (double& theX, double& theY) const
{
  double* aCenter = myRenderer->GetCenter();
  theX = aCenter[0];
  theY = aCenter[1];
}

//================================================================
// Function : DisplayToWorld
// Purpose  : 
//================================================================
bool IVtkVTK_View::DisplayToWorld (const gp_XY& theDisplayPnt, gp_XYZ& theWorldPnt) const
{
  // Convert the selection point into world coordinates.
  myRenderer->SetDisplayPoint (theDisplayPnt.X(), theDisplayPnt.Y(), 0.0);
  myRenderer->DisplayToWorld();

  double* const aCoords = myRenderer->GetWorldPoint();
  if (aCoords[3] == 0.0) // Point at infinity in homogeneous coordinates
  {
    return false;
  }

  theWorldPnt = gp_XYZ (aCoords[0] / aCoords[3], 
    aCoords[1] / aCoords[3], aCoords[2] / aCoords[3]);

  return true;
}

//================================================================
// Function : GetWindowSize
// Purpose  :
//================================================================
void IVtkVTK_View::GetWindowSize (int& theX, int& theY) const
{
  int* aSize = myRenderer->GetRenderWindow()->GetSize();
  theX = aSize[0];
  theY = aSize[1];
}

//================================================================
// Function : GetCamera
// Purpose  :
//================================================================
void IVtkVTK_View::GetCamera (Graphic3d_Mat4d& theProj,
                              Graphic3d_Mat4d& theOrient,
                              Standard_Boolean& theIsOrtho) const
{
  theIsOrtho = !IsPerspective();

  vtkMatrix4x4* aCompositeProj =
    myRenderer->GetActiveCamera()->
    GetCompositeProjectionTransformMatrix (myRenderer->GetTiledAspectRatio(),
                                           0,
                                           1);
  for (Standard_Integer aRow = 0; aRow < 4; ++aRow)
  {
    for (Standard_Integer aCol = 0; aCol < 4; ++aCol)
    {
      theProj.SetValue (aRow, aCol, aCompositeProj->GetElement (aRow, aCol));
    }
  }

  theOrient.InitIdentity();
}

//================================================================
// Function : GetViewport
// Purpose  :
//================================================================
void IVtkVTK_View::GetViewport (Standard_Real& theX,
                                Standard_Real& theY,
                                Standard_Real& theWidth,
                                Standard_Real& theHeight) const
{
  Standard_Real aViewport[4];
  myRenderer->GetViewport (aViewport);
  theX = aViewport[0];
  theY = aViewport[1];
  theWidth  = aViewport[2];
  theHeight = aViewport[3];
}
