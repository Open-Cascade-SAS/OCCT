// Created on: 2011-05-20
// Created by: Anton POLETAEV
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
#include <OpenGl_PrinterContext.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_PrinterContext, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_PrinterContext, Standard_Transient)

//=======================================================================
//function : OpenGl_PrinterContext
//purpose  :
//=======================================================================
OpenGl_PrinterContext::OpenGl_PrinterContext()
: myProjTransform  (0, 3, 0, 3),
  myScaleX (1.0f),
  myScaleY (1.0f),
  myLayerViewportX (0),
  myLayerViewportY (0)

{
  // identity projection matrix
  Standard_Real anInitValue = 0.0;
  myProjTransform.Init (anInitValue);
  myProjTransform (0, 0)  = 1.0;
  myProjTransform (1, 1)  = 1.0;
  myProjTransform (2, 2)  = 1.0;
  myProjTransform (3, 3)  = 1.0;
  SetProjTransformation (myProjTransform);
}

// =======================================================================
// function : ~OpenGl_PrinterContext
// purpose  :
// =======================================================================
OpenGl_PrinterContext::~OpenGl_PrinterContext()
{
  //
}

// =======================================================================
// function : ProjTransformation
// purpose  :
// =======================================================================
OpenGl_Mat4 OpenGl_PrinterContext::ProjTransformation()
{
  return OpenGl_Mat4::Map (myProjMatrixGl);
}

// =======================================================================
// function : SetProjTransformation
// purpose  : Set view projection transformation matrix for printing purposes.
//            theProjTransform parameter should be an 4x4 array.
// =======================================================================
bool OpenGl_PrinterContext::SetProjTransformation (const TColStd_Array2OfReal& thePrj)
{
  if (thePrj.RowLength () != 4 || thePrj.ColLength () != 4)
  {
    return false;
  }

  myProjTransform = thePrj;
  for (int i = 0, k = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++, k++)
    {
      myProjMatrixGl[k] = (Standard_ShortReal )myProjTransform (i, j);
    }
  }
  return true;
}

// =======================================================================
// function : SetScale
// purpose  :
// =======================================================================
void OpenGl_PrinterContext::SetScale (const Standard_ShortReal theScaleX,
                                      const Standard_ShortReal theScaleY)
{
  myScaleX = theScaleX;
  myScaleY = theScaleY;
}

// =======================================================================
// function : SetLayerViewport
// purpose  :
// =======================================================================
void OpenGl_PrinterContext::SetLayerViewport (const Standard_Integer theViewportX,
                                              const Standard_Integer theViewportY)
{
  myLayerViewportX = theViewportX;
  myLayerViewportY = theViewportY;
}
