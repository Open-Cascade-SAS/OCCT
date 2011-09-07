// File:      OpenGl_PrinterContext.cxx
// Created:   20.05.11 10:00:00
// Author:    Anton POLETAEV

#include <OpenGl_PrinterContext.hxx>

OpenGl_PrinterContext* OpenGl_PrinterContext::g_PrinterContext = NULL;
GLCONTEXT              OpenGl_PrinterContext::g_ContextId      = NULL;

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
