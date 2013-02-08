// Created on: 2011-05-20
// Created by: Anton POLETAEV
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
#include <OpenGl_PrinterContext.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_PrinterContext, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_PrinterContext, Standard_Transient)

//=======================================================================
//function : OpenGl_PrinterContext
//purpose  :
//=======================================================================
OpenGl_PrinterContext::OpenGl_PrinterContext()
: myProjTransform  (0, 3, 0, 3),
  myLayerViewportX (0),
  myLayerViewportY (0),
  myScaleX (1.0f),
  myScaleY (1.0f)
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
// function : LoadProjTransformation
// purpose  :
// =======================================================================
void OpenGl_PrinterContext::LoadProjTransformation()
{
  glLoadMatrixf ((GLfloat* )myProjMatrixGl);
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
