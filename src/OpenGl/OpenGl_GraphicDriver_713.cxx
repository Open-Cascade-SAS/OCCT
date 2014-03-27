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

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Group.hxx>
#include <OpenGl_PrimitiveArray.hxx>

#include <OpenGl_CView.hxx>

void OpenGl_GraphicDriver::SetDepthTestEnabled( const Graphic3d_CView& ACView, const Standard_Boolean isEnabled ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->UseDepthTest() = isEnabled;
}

Standard_Boolean OpenGl_GraphicDriver::IsDepthTestEnabled( const Graphic3d_CView& ACView ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    return aCView->WS->UseDepthTest();
  return Standard_False;
}

void OpenGl_GraphicDriver::ReadDepths( const Graphic3d_CView& ACView,
                                      const Standard_Integer x,
                                      const Standard_Integer y,
                                      const Standard_Integer width,
                                      const Standard_Integer height,
                                      const Standard_Address buffer ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->ReadDepths(x, y, width, height, (float*) buffer);
}

void OpenGl_GraphicDriver::SetGLLightEnabled( const Graphic3d_CView& ACView, const Standard_Boolean isEnabled ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->UseGLLight() = isEnabled;
}

Standard_Boolean OpenGl_GraphicDriver::IsGLLightEnabled( const Graphic3d_CView& ACView ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    return aCView->WS->UseGLLight();
  return Standard_False;
}
