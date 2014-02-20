// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
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

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_CView.hxx>

Standard_Integer OpenGl_GraphicDriver::InquireLightLimit ()
{
  return (myGlDisplay.IsNull() ? 0 : myGlDisplay->Facilities().MaxLights);
}

void OpenGl_GraphicDriver::InquireMat (const Graphic3d_CView& ACView, TColStd_Array2OfReal& AMatO, TColStd_Array2OfReal& AMatM)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->View->GetMatrices(AMatO,AMatM,ACView.Orientation.IsCustomMatrix);
}

Standard_Integer OpenGl_GraphicDriver::InquireViewLimit ()
{
  return (myGlDisplay.IsNull() ? 0 : myGlDisplay->Facilities().MaxViews);
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

