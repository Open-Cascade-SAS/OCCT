// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_Clipping.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Workspace.hxx>

namespace
{
  static const GLdouble OpenGl_DefaultPlaneEq[] = {0.0, 0.0, 0.0, 0.0};
};

// =======================================================================
// function : OpenGl_ClippingState
// purpose  :
// =======================================================================
OpenGl_Clipping::OpenGl_Clipping ()
: myPlanes(),
  myPlaneStates(),
  myEmptyPlaneIds (new Aspect_GenId (GL_CLIP_PLANE0, GL_CLIP_PLANE5))
{}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Clipping::Init (const Standard_Integer theMaxPlanes)
{
  myPlanes.Clear();
  myPlaneStates.Clear();
  Standard_Integer aLowerId = GL_CLIP_PLANE0;
  Standard_Integer aUpperId = GL_CLIP_PLANE0 + theMaxPlanes - 1;
  myEmptyPlaneIds = new Aspect_GenId (aLowerId, aUpperId);
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void OpenGl_Clipping::Add (Graphic3d_SequenceOfHClipPlane& thePlanes,
                           const EquationCoords& theCoordSpace,
                           const Handle(OpenGl_Workspace)& theWS)
{
  GLint aMatrixMode;
  glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);

  OpenGl_Matrix aCurrentMx;
  glGetFloatv (GL_MODELVIEW_MATRIX, (GLfloat*) &aCurrentMx);

  if (aMatrixMode != GL_MODELVIEW)
  {
    glMatrixMode (GL_MODELVIEW);
  }

  switch (theCoordSpace)
  {
    case EquationCoords_View:  glLoadMatrixf ((const GLfloat*) &OpenGl_IdentityMatrix); break;
    case EquationCoords_World: glLoadMatrixf ((const GLfloat*) theWS->ViewMatrix());    break;
  }

  Add (thePlanes, theCoordSpace);

  // restore model-view matrix
  glLoadMatrixf ((GLfloat*) &aCurrentMx);

  // restore context matrix state
  if (aMatrixMode != GL_MODELVIEW)
  {
    glMatrixMode (aMatrixMode);
  }
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void OpenGl_Clipping::Add (Graphic3d_SequenceOfHClipPlane& thePlanes, const EquationCoords& theCoordSpace)
{
  Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (thePlanes);
  while (aPlaneIt.More() && myEmptyPlaneIds->Available() > 0)
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (Contains (aPlane))
    {
      thePlanes.Remove (aPlaneIt);
      continue;
    }

    Standard_Integer anID = myEmptyPlaneIds->Next();
    myPlanes.Append (aPlane);
    myPlaneStates.Bind (aPlane, PlaneProps (theCoordSpace, anID, Standard_True));

    glEnable ((GLenum)anID);
    glClipPlane ((GLenum)anID, aPlane->GetEquation());
    aPlaneIt.Next();
  }

  while (aPlaneIt.More() && myEmptyPlaneIds->Available() == 0)
  {
    thePlanes.Remove (aPlaneIt);
  }
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void OpenGl_Clipping::Remove (const Graphic3d_SequenceOfHClipPlane& thePlanes)
{
  Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (thePlanes);
  for (; aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (!Contains (aPlane))
    {
      continue;
    }

    Standard_Integer anID = myPlaneStates.Find (aPlane).ContextID;
    myEmptyPlaneIds->Free (anID);
    myPlaneStates.UnBind (aPlane);

    glDisable ((GLenum)anID);
  }

  // renew collection of planes
  aPlaneIt.Init (myPlanes);
  while (aPlaneIt.More())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (!myPlaneStates.IsBound (aPlane))
    {
      myPlanes.Remove (aPlaneIt);
    }
    else
    {
      aPlaneIt.Next();
    }
  }
}

// =======================================================================
// function : SetEnabled
// purpose  :
// =======================================================================
void OpenGl_Clipping::SetEnabled (const Handle(Graphic3d_ClipPlane)& thePlane,
                                  const Standard_Boolean theIsEnabled)
{
  if (!Contains (thePlane))
  {
    return;
  }

  PlaneProps& aProps = myPlaneStates.ChangeFind (thePlane);
  if (theIsEnabled == aProps.IsEnabled)
  {
    return;
  }

  GLenum anID = (GLenum)aProps.ContextID;
  if (theIsEnabled)
  {
    glEnable (anID);
  }
  else
  {
    glDisable (anID);
  }

  aProps.IsEnabled = theIsEnabled;
}
