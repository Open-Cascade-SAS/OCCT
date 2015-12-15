// Created on: 2013-09-05
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_Clipping.hxx>

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Context.hxx>

#if defined(GL_ES_VERSION_2_0)
  // id does not matter for GLSL-based clipping, just for consistency
  #define GL_CLIP_PLANE0 0x3000
#endif

// =======================================================================
// function : OpenGl_ClippingState
// purpose  :
// =======================================================================
OpenGl_Clipping::OpenGl_Clipping ()
: myEmptyPlaneIds (new Aspect_GenId (GL_CLIP_PLANE0, GL_CLIP_PLANE0 + 5)),
  myNbClipping (0),
  myNbCapping  (0)
{}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Clipping::Init (const Standard_Integer theMaxPlanes)
{
  myPlanes.Clear();
  myPlaneStates.Clear();
  myNbClipping = 0;
  myNbCapping  = 0;
  Standard_Integer aLowerId = GL_CLIP_PLANE0;
  Standard_Integer aUpperId = GL_CLIP_PLANE0 + theMaxPlanes - 1;
  myEmptyPlaneIds = new Aspect_GenId (aLowerId, aUpperId);
}

// =======================================================================
// function : add
// purpose  :
// =======================================================================
void OpenGl_Clipping::add (const Handle(OpenGl_Context)&   theGlCtx,
                           const EquationCoords&           theCoordSpace,
                           Graphic3d_SequenceOfHClipPlane& thePlanes)
{
  const bool toUseFfp = theGlCtx->core11 != NULL
                     && theGlCtx->caps->ffpEnable;
  if (!toUseFfp)
  {
    addLazy (theGlCtx, theCoordSpace, thePlanes);
    return;
  }

  if (EquationCoords_View == theCoordSpace)
  {
    theGlCtx->WorldViewState.Push();
    theGlCtx->WorldViewState.SetIdentity();
  }

  // Set either identity or pure view matrix.
  theGlCtx->ApplyWorldViewMatrix();

  addLazy (theGlCtx, theCoordSpace, thePlanes);

  if (EquationCoords_View == theCoordSpace)
  {
    theGlCtx->WorldViewState.Pop();
  }

  // Restore combined model-view matrix.
  theGlCtx->ApplyModelViewMatrix();
}

// =======================================================================
// function : addLazy
// purpose  :
// =======================================================================
void OpenGl_Clipping::addLazy (const Handle(OpenGl_Context)&   theGlCtx,
                               const EquationCoords&           theCoordSpace,
                               Graphic3d_SequenceOfHClipPlane& thePlanes)
{
#if !defined(GL_ES_VERSION_2_0)
  const bool toUseFfp = theGlCtx->core11 != NULL
                     && theGlCtx->caps->ffpEnable;
#else
  (void )theGlCtx;
#endif

  Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (thePlanes);
  while (aPlaneIt.More() && myEmptyPlaneIds->HasFree())
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

  #if !defined(GL_ES_VERSION_2_0)
    if (toUseFfp)
    {
      ::glEnable ((GLenum)anID);
      theGlCtx->core11->glClipPlane ((GLenum)anID, aPlane->GetEquation());
    }
  #endif
    if (aPlane->IsCapping())
    {
      ++myNbCapping;
    }
    else
    {
      ++myNbClipping;
    }

    aPlaneIt.Next();
  }

  if (!myEmptyPlaneIds->HasFree())
  {
    while (aPlaneIt.More())
    {
      thePlanes.Remove (aPlaneIt);
    }
  }
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void OpenGl_Clipping::Remove (const Handle(OpenGl_Context)&         theGlCtx,
                              const Graphic3d_SequenceOfHClipPlane& thePlanes)
{
#if !defined(GL_ES_VERSION_2_0)
  const bool toUseFfp = theGlCtx->core11 != NULL
                     && theGlCtx->caps->ffpEnable;
#else
  (void )theGlCtx;
#endif

  Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (thePlanes);
  for (; aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (!Contains (aPlane))
    {
      continue;
    }

    Standard_Integer anID = myPlaneStates.Find (aPlane).ContextID;
    PlaneProps& aProps = myPlaneStates.ChangeFind (aPlane);
    if (aProps.IsEnabled)
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (toUseFfp)
      {
        ::glDisable ((GLenum)anID);
      }
    #endif
      if (aPlane->IsCapping())
      {
        --myNbCapping;
      }
      else
      {
        --myNbClipping;
      }
    }

    myEmptyPlaneIds->Free (anID);
    myPlaneStates.UnBind (aPlane);
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
void OpenGl_Clipping::SetEnabled (const Handle(OpenGl_Context)&      theGlCtx,
                                  const Handle(Graphic3d_ClipPlane)& thePlane,
                                  const Standard_Boolean             theIsEnabled)
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

#if !defined(GL_ES_VERSION_2_0)
  GLenum anID = (GLenum)aProps.ContextID;
  const bool toUseFfp = theGlCtx->core11 != NULL
                     && theGlCtx->caps->ffpEnable;
#else
  (void )theGlCtx;
#endif
  if (theIsEnabled)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (toUseFfp)
    {
      ::glEnable (anID);
    }
  #endif
    if (thePlane->IsCapping())
    {
      ++myNbCapping;
    }
    else
    {
      ++myNbClipping;
    }
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (toUseFfp)
    {
      ::glDisable (anID);
    }
  #endif
    if (thePlane->IsCapping())
    {
      --myNbCapping;
    }
    else
    {
      --myNbClipping;
    }
  }

  aProps.IsEnabled = theIsEnabled;
}
