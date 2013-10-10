// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#include <OpenGl_ClippingState.hxx>
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
OpenGl_ClippingState::OpenGl_ClippingState ()
: myPlanes(),
  myPlaneStates(),
  myEmptyPlaneIds (new Aspect_GenId (GL_CLIP_PLANE0, GL_CLIP_PLANE5))
{}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_ClippingState::Init (const Standard_Integer theMaxPlanes)
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
void OpenGl_ClippingState::Add (Graphic3d_SetOfHClipPlane& thePlanes,
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
void OpenGl_ClippingState::Add (Graphic3d_SetOfHClipPlane& thePlanes, const EquationCoords& theCoordSpace)
{
  Graphic3d_SetOfHClipPlane::Iterator aPlaneIt (thePlanes);
  while (aPlaneIt.More() && myEmptyPlaneIds->Available() > 0)
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (Contains (aPlane))
    {
      thePlanes.Remove (aPlaneIt);
      continue;
    }

    Standard_Integer anID = myEmptyPlaneIds->Next();
    myPlanes.Add (aPlane);
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
void OpenGl_ClippingState::Remove (const Graphic3d_SetOfHClipPlane& thePlanes)
{
  Graphic3d_SetOfHClipPlane::Iterator aPlaneIt (thePlanes);
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
void OpenGl_ClippingState::SetEnabled (const Handle(Graphic3d_ClipPlane)& thePlane,
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
