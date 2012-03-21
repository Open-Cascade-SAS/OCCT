// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <OpenGl_GlCore11.hxx>

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_CView.hxx>

Standard_Integer OpenGl_GraphicDriver::InquireLightLimit ()
{
  return (openglDisplay.IsNull()? 0 : openglDisplay->Facilities().MaxLights);
}

void OpenGl_GraphicDriver::InquireMat (const Graphic3d_CView& ACView, TColStd_Array2OfReal& AMatO, TColStd_Array2OfReal& AMatM)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->GetMatrices(AMatO,AMatM,ACView.Orientation.IsCustomMatrix);
}

Standard_Integer OpenGl_GraphicDriver::InquireViewLimit ()
{
  return (openglDisplay.IsNull()? 0 : openglDisplay->Facilities().MaxViews);
}

Standard_Boolean OpenGl_GraphicDriver::InquireTextureAvailable ()
{
  return Standard_True;
}

Standard_Integer OpenGl_GraphicDriver::InquirePlaneLimit ()
{
  GLint aMaxPlanes = 0;
  if (GET_GL_CONTEXT())
  {
    glGetIntegerv (GL_MAX_CLIP_PLANES, &aMaxPlanes);
    aMaxPlanes -= 2; // NOTE the 2 first planes are reserved for ZClipping
    if (aMaxPlanes < 0)
      aMaxPlanes = 0;
  }
  return aMaxPlanes;
}

