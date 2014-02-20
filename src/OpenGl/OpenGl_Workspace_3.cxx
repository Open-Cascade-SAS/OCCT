// Created on: 2011-09-20
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

#include <stdio.h>

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Workspace.hxx>

/*----------------------------------------------------------------------*/
/*  Mode Ajout              */
/*----------------------------------------------------------------------*/

//call_togl_begin_ajout_mode
Standard_Boolean OpenGl_Workspace::BeginAddMode()
{
  if (!Activate())
  {
    return Standard_False;
  }

  NamedStatus |= OPENGL_NS_ADD;

  MakeFrontAndBackBufCurrent();

  //TsmPushAttri();

  return Standard_True;
}

/*----------------------------------------------------------------------*/

//call_togl_end_ajout_mode
void OpenGl_Workspace::EndAddMode ()
{
  if (NamedStatus & OPENGL_NS_ADD)
  {
    OpenGl_Workspace::MakeBackBufCurrent();

    // Clear add mode flag
    NamedStatus &= ~OPENGL_NS_ADD;
  }

  // FMN necessaire pour l'affichage sur WNT
  glFlush();

  //TsmPopAttri();
}

/*----------------------------------------------------------------------*/
/*  Mode Transient              */
/*----------------------------------------------------------------------*/

//call_togl_clear_immediat_mode
void OpenGl_Workspace::ClearImmediatMode (const Graphic3d_CView& theCView,
                                          const Standard_Boolean theToFlush)
{
  if (myIsTransientOpen)
  {
    EndImmediatMode();
  }

  if (!Activate())
  {
    myTransientList.Clear(); // Clear current list contents
    return;
  }

  GLboolean isDoubleBuffer = GL_FALSE;
  glGetBooleanv (GL_DOUBLEBUFFER, &isDoubleBuffer);
  if (!myBackBufferRestored || !myTransientDrawToFront || !isDoubleBuffer)
  {
    Redraw1 (theCView, *((CALL_DEF_LAYER* )theCView.ptrUnderLayer), *((CALL_DEF_LAYER* )theCView.ptrOverLayer), theToFlush);

    // After a redraw,
    // Made the back identical to the front buffer.
    // Always perform full copy (partial update optimization is useless on mordern hardware)!
    if (myRetainMode && myTransientDrawToFront && isDoubleBuffer)
    {
      const Standard_Boolean toCopyFrontToBack = Standard_True;
      CopyBuffers (toCopyFrontToBack);
    }

    myBackBufferRestored = Standard_True;
  }
  else if (!myTransientList.IsEmpty() && isDoubleBuffer)
  {
    // restore pixels from the back buffer
    const Standard_Boolean toCopyFrontToBack = Standard_False;
    CopyBuffers (toCopyFrontToBack);
  }

  myTransientList.Clear(); // clear current list contents
}

/*----------------------------------------------------------------------*/

//call_togl_redraw_immediat_mode
void OpenGl_Workspace::RedrawImmediatMode()
{
  if (!myRetainMode || myTransientList.IsEmpty())
  {
    return;
  }

  GLboolean isDoubleBuffer = GL_FALSE;
  glGetBooleanv (GL_DOUBLEBUFFER, &isDoubleBuffer);
  if (isDoubleBuffer && myTransientDrawToFront)
  {
    MakeFrontBufCurrent();
  }
  else
  {
    myBackBufferRestored = Standard_False;
  }
  glDisable (GL_LIGHTING);

  Handle(OpenGl_Workspace) aWS (this);
  for (Standard_Integer anIter = 1; anIter <= myTransientList.Size(); ++anIter)
  {
    const OpenGl_Structure* aStructure = myTransientList.Value (anIter);
    aStructure->Render (aWS);
  }

  if (isDoubleBuffer && myTransientDrawToFront)
  {
    glFlush(); // FMN necessaire pour l'affichage sur WNT
    MakeBackBufCurrent();
  }
}

/*----------------------------------------------------------------------*/

//call_togl_begin_immediat_mode
Standard_Boolean OpenGl_Workspace::BeginImmediatMode (const Graphic3d_CView& theCView,
                                                      const Standard_Boolean theToUseDepthTest,
                                                      const Standard_Boolean theRetainMode)
{
  if (!Activate())
  {
    return Standard_False;
  }

  OpenGl_Workspace::ClearImmediatMode (theCView, Standard_True);

  NamedStatus |= OPENGL_NS_IMMEDIATE;
  myRetainMode = theRetainMode;

  if (myTransientDrawToFront)
  {
    MakeFrontBufCurrent();
  }

  //TsmPushAttri();

  if (myRetainMode)
  {
    myIsTransientOpen = Standard_True;
  }

  if (theToUseDepthTest)
  {
    glEnable (GL_DEPTH_TEST);
  }
  else
  {
    glDisable (GL_DEPTH_TEST);
  }

  return Standard_True;
}

/*----------------------------------------------------------------------*/

//call_togl_end_immediat_mode
void OpenGl_Workspace::EndImmediatMode()
{
  if (NamedStatus & OPENGL_NS_IMMEDIATE)
  {
    if (myIsTransientOpen)
    {
      myIsTransientOpen = Standard_False;
    }
    if (myTransientDrawToFront)
    {
      MakeBackBufCurrent();
    }

    // Clear immediate mode flag
    NamedStatus &= ~OPENGL_NS_IMMEDIATE;
  }

  if (myTransientDrawToFront)
  {
    // Ajout CAL : pour voir quelque chose avant le prochain begin_immediat_mode
    glFinish();
  }
  else
  {
    GetGlContext()->SwapBuffers();
  }

  //TsmPopAttri();
}

//call_togl_draw_structure
void OpenGl_Workspace::DrawStructure (const OpenGl_Structure* theStructure)
{
  if (NamedStatus & (OPENGL_NS_ADD | OPENGL_NS_IMMEDIATE))
  {
    Handle(OpenGl_Workspace) aWS (this);
    theStructure->Render (aWS);

    if (myIsTransientOpen && myRetainMode)
    {
      myTransientList.Append (theStructure);
    }
  }
}
