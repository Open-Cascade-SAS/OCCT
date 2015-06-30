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

#include <OpenGl_CappingAlgo.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_CappingPlaneResource.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Structure.hxx>
#include <Graphic3d_GraphicDriver.hxx>

namespace
{
#if !defined(GL_ES_VERSION_2_0)
  static const GLint THE_FILLPRIM_FROM = GL_TRIANGLES;
  static const GLint THE_FILLPRIM_TO   = GL_POLYGON;
#else
  static const GLint THE_FILLPRIM_FROM = GL_TRIANGLES;
  static const GLint THE_FILLPRIM_TO   = GL_TRIANGLE_FAN;
#endif
}

// =======================================================================
// function : RenderCapping
// purpose  :
// =======================================================================
void OpenGl_CappingAlgo::RenderCapping (const Handle(OpenGl_Workspace)&  theWorkspace,
                                        const Graphic3d_SequenceOfGroup& theGroups)
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // check whether algorithm need to be performed
  Standard_Boolean isCapping = Standard_False;
  const Graphic3d_SequenceOfHClipPlane& aContextPlanes = aContext->Clipping().Planes();
  Graphic3d_SequenceOfHClipPlane::Iterator aCappingIt (aContextPlanes);
  for (; aCappingIt.More(); aCappingIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aCappingIt.Value();
    if (aPlane->IsCapping())
    {
      isCapping = Standard_True;
      break;
    }
  }

  // do not perform algorithm is there is nothing to render
  if (!isCapping)
  {
    return;
  }

  // remember current aspect face defined in workspace
  const OpenGl_AspectFace* aFaceAsp = theWorkspace->AspectFace (Standard_False);

  // replace primitive groups rendering filter
  Handle(OpenGl_RenderFilter) aRenderFilter = theWorkspace->GetRenderFilter();
  theWorkspace->SetRenderFilter (theWorkspace->DefaultCappingAlgoFilter());

  // prepare for rendering the clip planes
  glEnable (GL_STENCIL_TEST);

  // remember current state of depth
  // function and change its value
  GLint aDepthFuncPrev;
  glGetIntegerv (GL_DEPTH_FUNC, &aDepthFuncPrev);
  glDepthFunc (GL_LESS);

  // generate capping for every clip plane
  for (aCappingIt.Init (aContextPlanes); aCappingIt.More(); aCappingIt.Next())
  {
    // get plane being rendered
    const Handle(Graphic3d_ClipPlane)& aRenderPlane = aCappingIt.Value();
    if (!aRenderPlane->IsCapping())
    {
      continue;
    }

    // enable only the rendering plane to generate stencil mask
    Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (aContextPlanes);
    for (; aPlaneIt.More(); aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      const Standard_Boolean isOn = (aPlane == aRenderPlane);
      aContext->ChangeClipping().SetEnabled (aPlane, isOn);
    }

    glClear (GL_STENCIL_BUFFER_BIT);
    glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // override aspects, disable culling
    theWorkspace->SetAspectFace (&theWorkspace->NoneCulling());
    theWorkspace->AspectFace (Standard_True);

    // evaluate number of pair faces
    glDisable (GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);
    glStencilFunc (GL_ALWAYS, 1, 0x01);
    glStencilOp (GL_KEEP, GL_INVERT, GL_INVERT);

    for (OpenGl_Structure::GroupIterator aGroupIt (theGroups); aGroupIt.More(); aGroupIt.Next())
    {
      if (aGroupIt.Value()->IsClosed())
      {
        aGroupIt.Value()->Render (theWorkspace);
      }
    }

    // override material, cull back faces
    theWorkspace->SetAspectFace (&theWorkspace->FrontCulling());
    theWorkspace->AspectFace (Standard_True);

    // enable all clip plane except the rendered one
    for (aPlaneIt.Init (aContextPlanes); aPlaneIt.More(); aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      const Standard_Boolean isOn = (aPlane != aRenderPlane);
      aContext->ChangeClipping().SetEnabled (aPlane, isOn);
    }

    // render capping plane using the generated stencil mask
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask (GL_TRUE);
    glStencilFunc (GL_EQUAL, 1, 0x01);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    glEnable (GL_DEPTH_TEST);

    RenderPlane (theWorkspace, aRenderPlane);
  }

  // restore previous application state
  glClear (GL_STENCIL_BUFFER_BIT);
  glDepthFunc (aDepthFuncPrev);
  glStencilFunc (GL_ALWAYS, 0, 0xFF);
  glDisable (GL_STENCIL_TEST);

  // enable clipping
  for (aCappingIt.Init (aContextPlanes); aCappingIt.More(); aCappingIt.Next())
  {
    aContext->ChangeClipping().SetEnabled (aCappingIt.Value(), Standard_True);
  }

  // restore rendering aspects
  theWorkspace->SetAspectFace (aFaceAsp);
  theWorkspace->SetRenderFilter (aRenderFilter);
}

// =======================================================================
// function : RenderPlane
// purpose  :
// =======================================================================
void OpenGl_CappingAlgo::RenderPlane (const Handle(OpenGl_Workspace)& theWorkspace,
                                      const Handle(Graphic3d_ClipPlane)& thePlane)
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // get resource for the plane
  TCollection_AsciiString aResId = thePlane->GetId();

  Handle(OpenGl_CappingPlaneResource) aPlaneRes;
  if (!aContext->GetResource (aResId, aPlaneRes))
  {
    // share and register for release once the resource is no longer used
    aPlaneRes = new OpenGl_CappingPlaneResource (thePlane);
    aContext->ShareResource (aResId, aPlaneRes);
  }

  aPlaneRes->Update (aContext);

  const OpenGl_AspectFace* aFaceAspect = theWorkspace->AspectFace (Standard_False);
  const OpenGl_AspectFace* aPlaneAspect = aPlaneRes->AspectFace();
  if (aPlaneAspect != NULL)
  {
    theWorkspace->SetAspectFace (aPlaneAspect);
  }

  // set identity model matrix
  aContext->ModelWorldState.Push();
  aContext->ModelWorldState.SetCurrent (OpenGl_Mat4::Map (*aPlaneRes->Orientation()->mat));
  aContext->ApplyModelViewMatrix();

  aPlaneRes->Primitives().Render (theWorkspace);

  aContext->ModelWorldState.Pop();
  aContext->ApplyModelViewMatrix();

  theWorkspace->SetAspectFace (aFaceAspect);

  // set delayed resource release
  aPlaneRes.Nullify();
  aContext->ReleaseResource (aResId, Standard_True);
}

// =======================================================================
// function : CanRender
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_CappingAlgoFilter::CanRender (const OpenGl_Element* theElement)
{
  const OpenGl_PrimitiveArray* aPArray = dynamic_cast<const OpenGl_PrimitiveArray*> (theElement);
  return aPArray != NULL
      && aPArray->DrawMode() >= THE_FILLPRIM_FROM
      && aPArray->DrawMode() <= THE_FILLPRIM_TO;
}
