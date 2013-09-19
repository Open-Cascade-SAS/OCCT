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
// function : Planes
// purpose  :
// =======================================================================
Graphic3d_SetOfHClipPlane OpenGl_ClippingState::Planes() const
{
  Graphic3d_SetOfHClipPlane aRes;
  OpenGl_MapOfContextPlanes::Iterator anIt (myPlanes);
  for (; anIt.More(); anIt.Next())
  {
    aRes.Add (anIt.Key());
  }

  return aRes;
}

// =======================================================================
// function : IsSet
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ClippingState::IsSet (const Handle(Graphic3d_ClipPlane)& thePlane) const
{
  return myPlanes.IsBound (thePlane);
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
void OpenGl_ClippingState::Set (const Graphic3d_SetOfHClipPlane& thePlanes,
                                const Standard_Boolean theToEnable)
{
  Graphic3d_SetOfHClipPlane::Iterator aPlaneIt (thePlanes);
  for (; aPlaneIt.More() && myEmptyPlaneIds->Available() > 0; aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (IsSet (aPlane))
      return;

    Standard_Integer anId = myEmptyPlaneIds->Next();
    myPlanes.Bind (aPlane, anId);
    myPlaneStates.Bind (aPlane, theToEnable);

    const GLenum anOpenGlId = (GLenum)anId;
    if (theToEnable)
    {
      glEnable (anOpenGlId);
    }
    else
    {
      glDisable (anOpenGlId);
    }

    glClipPlane (anOpenGlId, aPlane->GetEquation());
  }
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
void OpenGl_ClippingState::Set (const Graphic3d_SetOfHClipPlane& thePlanes,
                                const OpenGl_Matrix* theViewMatrix,
                                const Standard_Boolean theToEnable)
{
  GLint aMatrixMode;
  glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);

  OpenGl_Matrix aCurrentMat;
  glGetFloatv (GL_MODELVIEW_MATRIX, (GLfloat*)aCurrentMat.mat);

  if (aMatrixMode != GL_MODELVIEW)
  {
    glMatrixMode (GL_MODELVIEW);
  }

  // load equation transform matrices
  glLoadMatrixf ((theViewMatrix != NULL)
    ? (const GLfloat*)theViewMatrix->mat
    : (const GLfloat*)OpenGl_IdentityMatrix.mat);

  Set (thePlanes, theToEnable);

  // restore model-view matrix
  glLoadMatrixf ((GLfloat*)aCurrentMat.mat);

  // restore context matrix state
  if (aMatrixMode != GL_MODELVIEW)
  {
    glMatrixMode (aMatrixMode);
  }
}

// =======================================================================
// function : Unset
// purpose  :
// =======================================================================
void OpenGl_ClippingState::Unset (const Graphic3d_SetOfHClipPlane& thePlanes)
{
  Graphic3d_SetOfHClipPlane::Iterator aPlaneIt (thePlanes);
  for (; aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (!IsSet (aPlane))
      continue;

    Standard_Integer anId = myPlanes.Find (aPlane);
    myEmptyPlaneIds->Free (anId);
    myPlanes.UnBind (aPlane);
    myPlaneStates.UnBind (aPlane);

    const GLenum anOpenGlId = (GLenum)anId;

    glDisable (anOpenGlId);
    glClipPlane (anOpenGlId, OpenGl_DefaultPlaneEq);
  }
}

//
//// =======================================================================
//// function : SetPlane
//// purpose  :
//// =======================================================================
//Standard_Boolean OpenGl_ClippingState::SetPlane (const Handle(Graphic3d_ClipPlane)& thePlane,
//                                                 const Standard_Boolean theToEnable)
//{
//  if (myEmptyPlaneIds->Available() == 0)
//    return Standard_False;
//
//  if (IsPlaneSet (thePlane))
//    return Standard_True;
//
//  Standard_Integer aPlaneId = myEmptyPlaneIds->Next();
//  myPlanes.Bind (thePlane, aPlaneId);
//  myPlaneStates.Bind (thePlane, theToEnable);
//  if (theToEnable)
//    glEnable (aPlaneId);
//  else
//    glDisable (aPlaneId);
//
//  glClipPlane (aPlaneId, thePlane->GetEquation());
//
//  return Standard_True;
//}

//// =======================================================================
//// function : UnsetPlane
//// purpose  :
//// =======================================================================
//void OpenGl_ClippingState::UnsetPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
//{
//  if (!IsPlaneSet (thePlane))
//    return;
//
//  Standard_Integer aPlaneId = myPlanes.Find (thePlane);
//
//  myEmptyPlaneIds->Free (aPlaneId);
//  myPlanes.UnBind (thePlane);
//  myPlaneStates.UnBind (thePlane);
//
//  glDisable (aPlaneId);
//  glClipPlane (aPlaneId, OpenGl_DefaultPlaneEq);
//}

// =======================================================================
// function : IsEnabled
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_ClippingState::IsEnabled (const Handle(Graphic3d_ClipPlane)& thePlane) const
{
  Standard_Boolean isSet;
  return IsSet (thePlane)
      && myPlaneStates.Find (thePlane, isSet)
      && isSet;
}

// =======================================================================
// function : SetEnabled
// purpose  :
// =======================================================================
void OpenGl_ClippingState::SetEnabled (const Handle(Graphic3d_ClipPlane)& thePlane,
                                       const Standard_Boolean theIsEnabled)
{
  if (!IsSet (thePlane))
    return;

  Standard_Boolean& aState = myPlaneStates.ChangeFind (thePlane);
  if (theIsEnabled == aState)
    return;

  Standard_Integer aPlaneId = myPlanes.Find (thePlane);
  if (theIsEnabled)
    glEnable (aPlaneId);
  else
    glDisable (aPlaneId);

  aState = theIsEnabled;
}
