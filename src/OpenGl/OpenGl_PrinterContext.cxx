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


#include <OpenGl_PrinterContext.hxx>

OpenGl_PrinterContext* OpenGl_PrinterContext::g_PrinterContext = NULL;
GLCONTEXT             OpenGl_PrinterContext::g_ContextId      = NULL;

//=======================================================================
//function : OpenGl_PrinterContext
//purpose  : Constructor
//=======================================================================

OpenGl_PrinterContext::OpenGl_PrinterContext (GLCONTEXT theCtx) :
  myCtx (theCtx), myProjTransform (0, 3, 0, 3), myLayerViewportX (0),
  myLayerViewportY (0), myScaleX (1.0f), myScaleY (1.0f)
{
  // assign global instance to the current object
  if (myCtx != NULL)
  {
    g_PrinterContext = this;
    g_ContextId      = myCtx;
  }

  // init projection matrix
  Standard_Real anInitValue = 0.0;
  myProjTransform.Init (anInitValue);
  myProjTransform (0,0)  = 1.0f;
  myProjTransform (1,1)  = 1.0f;
  myProjTransform (2,2)  = 1.0f;
  myProjTransform (3,3)  = 1.0f;
}

//=======================================================================
//function : ~OpenGl_PrinterContext
//purpose  : Destructor
//=======================================================================

OpenGl_PrinterContext::~OpenGl_PrinterContext () 
{
  // unassign global instance
  if (g_PrinterContext == this)
  {
    g_ContextId      = NULL;
    g_PrinterContext = NULL;
  }
}

//=======================================================================
//function : GetProjTransformation
//purpose  : Get view projection transformation matrix.
//=======================================================================

void OpenGl_PrinterContext::GetProjTransformation (GLfloat theMatrix[16])
{
  for (int i = 0, k = 0; i < 4; i++)
    for (int j = 0; j < 4; j++, k++)
      theMatrix[k] = (GLfloat)myProjTransform (i,j);
}

//=======================================================================
//function : SetProjTransformation
//purpose  : Set view projection transformation matrix for printing purposes.
//           theProjTransform parameter should be an 4x4 array.
//=======================================================================

bool OpenGl_PrinterContext::SetProjTransformation (TColStd_Array2OfReal& thePrj)
{
  if (thePrj.RowLength () != 4 || thePrj.ColLength () != 4)
    return false;

  myProjTransform = thePrj;

  return true;
}

//=======================================================================
//function : Deactivate
//purpose  : Deactivate PrinterContext object.
//           Useful when you need to redraw in usual mode the same
//           OpenGl context that you used for printing right after printing,
//           before the OpenGl_PrinterContext instance destroyed
//=======================================================================

void OpenGl_PrinterContext::Deactivate ()
{
  // unassign global instance
  if (g_PrinterContext == this)
  {
    g_ContextId      = NULL;
    g_PrinterContext = NULL;
  }
}


//=======================================================================
//function : GetInstance
//purpose  : Get the PrinterContext instance assigned for OpenGl context.
//           Return NULL, if there is no current printing operation and
//           there is no assigned instance for "theCtx" OpenGl context.
//=======================================================================

OpenGl_PrinterContext* OpenGl_PrinterContext::GetPrinterContext (GLCONTEXT theCtx)
{
  if (g_ContextId == theCtx)
    return g_PrinterContext;
  else
    return NULL;
}
