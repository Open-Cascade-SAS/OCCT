// File:      OpenGl_GraphicDriver_2.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

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

