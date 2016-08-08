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
#include <OpenGl_ShaderManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_CappingAlgoFilter,OpenGl_RenderFilter)

namespace
{
#if !defined(GL_ES_VERSION_2_0)
  static const GLint THE_FILLPRIM_FROM = GL_TRIANGLES;
  static const GLint THE_FILLPRIM_TO   = GL_POLYGON;
#else
  static const GLint THE_FILLPRIM_FROM = GL_TRIANGLES;
  static const GLint THE_FILLPRIM_TO   = GL_TRIANGLE_FAN;
#endif

  //! Render infinite capping plane.
  //! @param theWorkspace [in] the GL workspace, context state.
  //! @param thePlane [in] the graphical plane, for which the capping surface is rendered.
  static void renderPlane (const Handle(OpenGl_Workspace)& theWorkspace,
                           const Handle(OpenGl_CappingPlaneResource)& thePlane,
                           const OpenGl_AspectFace* theAspectFace)
  {
    const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
    thePlane->Update (aContext, theAspectFace != NULL ? theAspectFace->Aspect() : Handle(Graphic3d_AspectFillArea3d)());

    const OpenGl_AspectFace* aFaceAspect = theWorkspace->AspectFace();
    theWorkspace->SetAspectFace (thePlane->AspectFace());

    // set identity model matrix
    aContext->ModelWorldState.Push();
    aContext->ModelWorldState.SetCurrent (OpenGl_Mat4::Map (*thePlane->Orientation()->mat));
    aContext->ApplyModelViewMatrix();

    thePlane->Primitives().Render (theWorkspace);

    aContext->ModelWorldState.Pop();
    aContext->ApplyModelViewMatrix();

    theWorkspace->SetAspectFace (aFaceAspect);
  }

  //! Render capping for specific structure.
  static void renderCappingForStructure (const Handle(OpenGl_Workspace)& theWorkspace,
                                         const OpenGl_Structure&         theStructure,
                                         const Handle(OpenGl_CappingPlaneResource)& thePlane)
  {
    const Handle(OpenGl_Context)&         aContext       = theWorkspace->GetGlContext();
    const Graphic3d_SequenceOfHClipPlane& aContextPlanes = aContext->Clipping().Planes();
    const Handle(Graphic3d_ClipPlane)&    aRenderPlane   = thePlane->Plane();
    for (OpenGl_Structure::GroupIterator aGroupIter (theStructure.Groups()); aGroupIter.More(); aGroupIter.Next())
    {
      if (!aGroupIter.Value()->IsClosed())
      {
        continue;
      }

      // enable only the rendering plane to generate stencil mask
      for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (aContextPlanes); aPlaneIt.More(); aPlaneIt.Next())
      {
        const Standard_Boolean isOn = (aPlaneIt.Value() == aRenderPlane);
        aContext->ChangeClipping().SetEnabled (aContext, aPlaneIt.Value(), isOn);
      }
      aContext->ShaderManager()->UpdateClippingState();

      glClear (GL_STENCIL_BUFFER_BIT);
      glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

      // override aspects, disable culling
      theWorkspace->SetAspectFace (&theWorkspace->NoneCulling());
      theWorkspace->ApplyAspectFace();

      // evaluate number of pair faces
      glDisable (GL_DEPTH_TEST);
      glDepthMask (GL_FALSE);
      glStencilFunc (GL_ALWAYS, 1, 0x01);
      glStencilOp (GL_KEEP, GL_INVERT, GL_INVERT);

      // render closed primitives
      if (aRenderPlane->ToUseObjectProperties())
      {
        aGroupIter.Value()->Render (theWorkspace);
      }
      else
      {
        for (; aGroupIter.More(); aGroupIter.Next())
        {
          if (aGroupIter.Value()->IsClosed())
          {
            aGroupIter.Value()->Render (theWorkspace);
          }
        }
      }

      // override material, cull back faces
      theWorkspace->SetAspectFace (&theWorkspace->FrontCulling());
      theWorkspace->ApplyAspectFace();

      // enable all clip plane except the rendered one
      for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (aContextPlanes); aPlaneIt.More(); aPlaneIt.Next())
      {
        const Standard_Boolean isOn = (aPlaneIt.Value() != aRenderPlane);
        aContext->ChangeClipping().SetEnabled (aContext, aPlaneIt.Value(), isOn);
      }
      aContext->ShaderManager()->UpdateClippingState();

      // render capping plane using the generated stencil mask
      glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask (GL_TRUE);
      glStencilFunc (GL_EQUAL, 1, 0x01);
      glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
      glEnable (GL_DEPTH_TEST);

      renderPlane (theWorkspace, thePlane, aRenderPlane->ToUseObjectProperties()
                                         ? aGroupIter.Value()->AspectFace()
                                         : NULL);

      aContext->ShaderManager()->RevertClippingState();
      aContext->ShaderManager()->RevertClippingState();
    }

    if (theStructure.InstancedStructure() != NULL)
    {
      renderCappingForStructure (theWorkspace, *theStructure.InstancedStructure(), thePlane);
    }
  }
}

// =======================================================================
// function : RenderCapping
// purpose  :
// =======================================================================
void OpenGl_CappingAlgo::RenderCapping (const Handle(OpenGl_Workspace)& theWorkspace,
                                        const OpenGl_Structure&         theStructure)
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // check whether algorithm need to be performed
  Standard_Boolean isCapping = Standard_False;
  const Graphic3d_SequenceOfHClipPlane& aContextPlanes = aContext->Clipping().Planes();
  for (Graphic3d_SequenceOfHClipPlane::Iterator aCappingIt (aContextPlanes); aCappingIt.More(); aCappingIt.Next())
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
  const OpenGl_AspectFace* aFaceAsp = theWorkspace->AspectFace();

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
  for (Graphic3d_SequenceOfHClipPlane::Iterator aCappingIt (aContextPlanes); aCappingIt.More(); aCappingIt.Next())
  {
    // get plane being rendered
    const Handle(Graphic3d_ClipPlane)& aRenderPlane = aCappingIt.Value();
    if (!aRenderPlane->IsCapping())
    {
      continue;
    }

    // get resource for the plane
    const TCollection_AsciiString& aResId = aRenderPlane->GetId();
    Handle(OpenGl_CappingPlaneResource) aPlaneRes;
    if (!aContext->GetResource (aResId, aPlaneRes))
    {
      // share and register for release once the resource is no longer used
      aPlaneRes = new OpenGl_CappingPlaneResource (aRenderPlane);
      aContext->ShareResource (aResId, aPlaneRes);
    }

    renderCappingForStructure (theWorkspace, theStructure, aPlaneRes);

    // set delayed resource release
    aPlaneRes.Nullify();
    aContext->ReleaseResource (aResId, Standard_True);
  }

  // restore previous application state
  glClear (GL_STENCIL_BUFFER_BIT);
  glDepthFunc (aDepthFuncPrev);
  glStencilFunc (GL_ALWAYS, 0, 0xFF);
  glDisable (GL_STENCIL_TEST);

  // enable clipping
  for (Graphic3d_SequenceOfHClipPlane::Iterator aCappingIt (aContextPlanes); aCappingIt.More(); aCappingIt.Next())
  {
    aContext->ChangeClipping().SetEnabled (aContext, aCappingIt.Value(), Standard_True);
  }

  // restore rendering aspects
  theWorkspace->SetAspectFace (aFaceAsp);
  theWorkspace->SetRenderFilter (aRenderFilter);
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
