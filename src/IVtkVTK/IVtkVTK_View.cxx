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
#include <vtkTransform.h>

// Initialization of VTK object factories.
// Since VTK 6 the factory methods require "auto-initialization" depending on
// what modules are enabled at VTK configure time.
// Some defines are needed in order to make the factories work properly.
VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);

// Handle implementation
IMPLEMENT_STANDARD_HANDLE(IVtkVTK_View, IVtk_IView)
IMPLEMENT_STANDARD_RTTIEXT(IVtkVTK_View, IVtk_IView)

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

  vtkFloatingPointType* const aCoords = myRenderer->GetWorldPoint();
  if (aCoords[3] == 0.0) // Point at infinity in homogeneous coordinates
  {
    return false;
  }

  theWorldPnt = gp_XYZ (aCoords[0] / aCoords[3], aCoords[1] / aCoords[3], aCoords[2] / aCoords[3]);

  return true;
}
