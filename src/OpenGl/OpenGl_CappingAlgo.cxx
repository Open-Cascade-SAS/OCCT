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

#include <OpenGl_ClippingIterator.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_CappingPlaneResource.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_ShaderManager.hxx>

namespace
{
//! Auxiliary sentry object managing stencil test.
struct StencilTestSentry
{
  StencilTestSentry(const Handle(OpenGl_Context)& theCtx)
      : myCtx(theCtx.get()),
        myDepthFuncPrev(0)
  {
  }

  //! Restore previous application state.
  ~StencilTestSentry()
  {
    if (myDepthFuncPrev != 0)
    {
      myCtx->core11fwd->glClear(GL_STENCIL_BUFFER_BIT);
      myCtx->core11fwd->glDepthFunc(myDepthFuncPrev);
      myCtx->core11fwd->glStencilFunc(GL_ALWAYS, 0, 0xFF);
      myCtx->core11fwd->glDisable(GL_STENCIL_TEST);
    }
  }

  //! Prepare for rendering the clip planes.
  void Init()
  {
    if (myDepthFuncPrev == 0)
    {
      myCtx->core11fwd->glEnable(GL_STENCIL_TEST);
      myCtx->core11fwd->glGetIntegerv(GL_DEPTH_FUNC, &myDepthFuncPrev);
      myCtx->core11fwd->glDepthFunc(GL_LESS);
    }
  }

private:
  OpenGl_Context* myCtx;
  GLint           myDepthFuncPrev;
};

//! Render infinite capping plane.
//! @param theWorkspace [in] the GL workspace, context state.
//! @param thePlane [in] the graphical plane, for which the capping surface is rendered.
static void renderPlane(const Handle(OpenGl_Workspace)&            theWorkspace,
                        const Handle(OpenGl_CappingPlaneResource)& thePlane)
{
  const Handle(OpenGl_Context)& aContext       = theWorkspace->GetGlContext();
  const bool                    wasCullAllowed = theWorkspace->SetAllowFaceCulling(true);

  // set identity model matrix
  aContext->ModelWorldState.Push();
  aContext->ModelWorldState.SetCurrent(thePlane->Orientation());
  aContext->ApplyModelViewMatrix();

  thePlane->Primitives().Render(theWorkspace);

  aContext->ModelWorldState.Pop();
  aContext->ApplyModelViewMatrix();

  theWorkspace->SetAllowFaceCulling(wasCullAllowed);
}

//! Render capping for specific structure.
static void renderCappingForStructure(StencilTestSentry&                         theStencilSentry,
                                      const Handle(OpenGl_Workspace)&            theWorkspace,
                                      const OpenGl_Structure&                    theStructure,
                                      const Handle(Graphic3d_ClipPlane)&         theClipChain,
                                      const Standard_Integer                     theSubPlaneIndex,
                                      const Handle(OpenGl_CappingPlaneResource)& thePlane)
{
  const Standard_Integer aPrevFilter = theWorkspace->RenderFilter();
  const Standard_Integer anAnyFilter =
    aPrevFilter
    & ~(Standard_Integer)(OpenGl_RenderFilter_OpaqueOnly | OpenGl_RenderFilter_TransparentOnly);

  const Handle(OpenGl_Context)&      aContext     = theWorkspace->GetGlContext();
  const Handle(Graphic3d_ClipPlane)& aRenderPlane = thePlane->Plane();
  for (OpenGl_Structure::GroupIterator aGroupIter(theStructure.Groups()); aGroupIter.More();
       aGroupIter.Next())
  {
    if (!aGroupIter.Value()->IsClosed())
    {
      continue;
    }

    // clear stencil only if something has been actually drawn
    theStencilSentry.Init();

    // check if capping plane should be rendered within current pass (only opaque / only
    // transparent)
    const OpenGl_Aspects* anObjAspectFace =
      aRenderPlane->ToUseObjectProperties() ? aGroupIter.Value()->GlAspects() : NULL;
    thePlane->Update(aContext,
                     anObjAspectFace != NULL ? anObjAspectFace->Aspect()
                                             : Handle(Graphic3d_Aspects)());
    theWorkspace->SetAspects(thePlane->AspectFace());
    theWorkspace->SetRenderFilter(aPrevFilter);
    if (!theWorkspace->ShouldRender(&thePlane->Primitives(), aGroupIter.Value()))
    {
      continue;
    }

    // suppress only opaque/transparent filter since for filling stencil the whole geometry should
    // be drawn
    theWorkspace->SetRenderFilter(anAnyFilter);

    // enable only the rendering plane to generate stencil mask
    aContext->ChangeClipping().DisableAllExcept(theClipChain, theSubPlaneIndex);
    aContext->ShaderManager()->UpdateClippingState();

    aContext->core11fwd->glClear(GL_STENCIL_BUFFER_BIT);
    const NCollection_Vec4<bool> aColorMaskBack = aContext->ColorMaskRGBA();
    aContext->SetColorMask(false);
    // override aspects, disable culling
    theWorkspace->SetAspects(&theWorkspace->NoneCulling());
    theWorkspace->ApplyAspects();

    // evaluate number of pair faces
    if (theWorkspace->UseZBuffer())
    {
      aContext->core11fwd->glDisable(GL_DEPTH_TEST);
    }
    if (theWorkspace->UseDepthWrite())
    {
      aContext->core11fwd->glDepthMask(GL_FALSE);
    }
    aContext->core11fwd->glStencilFunc(GL_ALWAYS, 1, 0x01);
    aContext->core11fwd->glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);

    // render closed primitives
    if (aRenderPlane->ToUseObjectProperties())
    {
      aGroupIter.Value()->Render(theWorkspace);
    }
    else
    {
      for (; aGroupIter.More(); aGroupIter.Next())
      {
        if (aGroupIter.Value()->IsClosed())
        {
          aGroupIter.Value()->Render(theWorkspace);
        }
      }
    }

    // override material, cull back faces
    theWorkspace->SetAspects(&theWorkspace->FrontCulling());
    theWorkspace->ApplyAspects();

    // enable all clip plane except the rendered one
    aContext->ChangeClipping().EnableAllExcept(theClipChain, theSubPlaneIndex);
    aContext->ShaderManager()->UpdateClippingState();

    // render capping plane using the generated stencil mask
    aContext->SetColorMaskRGBA(aColorMaskBack);
    if (theWorkspace->UseDepthWrite())
    {
      aContext->core11fwd->glDepthMask(GL_TRUE);
    }
    aContext->core11fwd->glStencilFunc(GL_EQUAL, 1, 0x01);
    aContext->core11fwd->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    if (theWorkspace->UseZBuffer())
    {
      aContext->core11fwd->glEnable(GL_DEPTH_TEST);
    }

    theWorkspace->SetAspects(thePlane->AspectFace());
    renderPlane(theWorkspace, thePlane);

    // turn on the current plane to restore initial state
    aContext->ChangeClipping().ResetCappingFilter();
    aContext->ShaderManager()->RevertClippingState();
    aContext->ShaderManager()->RevertClippingState();
  }

  if (theStructure.InstancedStructure() != NULL)
  {
    renderCappingForStructure(theStencilSentry,
                              theWorkspace,
                              *theStructure.InstancedStructure(),
                              theClipChain,
                              theSubPlaneIndex,
                              thePlane);
  }
}
} // namespace

// =======================================================================
// function : RenderCapping
// purpose  :
// =======================================================================
void OpenGl_CappingAlgo::RenderCapping(const Handle(OpenGl_Workspace)& theWorkspace,
                                       const OpenGl_Structure&         theStructure)
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
  if (!aContext->Clipping().IsCappingOn())
  {
    // do not perform algorithm if there is nothing to render
    return;
  }

  // remember current aspect face defined in workspace
  const OpenGl_Aspects* aFaceAsp = theWorkspace->Aspects();

  // only filled primitives should be rendered
  const Standard_Integer aPrevFilter = theWorkspace->RenderFilter();
  theWorkspace->SetRenderFilter(aPrevFilter | OpenGl_RenderFilter_FillModeOnly);
  StencilTestSentry aStencilSentry(aContext);

  // generate capping for every clip plane
  for (OpenGl_ClippingIterator aCappingIt(aContext->Clipping()); aCappingIt.More();
       aCappingIt.Next())
  {
    // get plane being rendered
    const Handle(Graphic3d_ClipPlane)& aClipChain = aCappingIt.Value();
    if (!aClipChain->IsCapping() || aCappingIt.IsDisabled())
    {
      continue;
    }

    Standard_Integer aSubPlaneIndex = 1;
    for (const Graphic3d_ClipPlane* aSubPlaneIter = aClipChain.get(); aSubPlaneIter != NULL;
         aSubPlaneIter = aSubPlaneIter->ChainNextPlane().get(), ++aSubPlaneIndex)
    {
      // get resource for the plane
      const TCollection_AsciiString&      aResId = aSubPlaneIter->GetId();
      Handle(OpenGl_CappingPlaneResource) aPlaneRes;
      if (!aContext->GetResource(aResId, aPlaneRes))
      {
        // share and register for release once the resource is no longer used
        aPlaneRes = new OpenGl_CappingPlaneResource(aSubPlaneIter);
        aContext->ShareResource(aResId, aPlaneRes);
      }

      renderCappingForStructure(aStencilSentry,
                                theWorkspace,
                                theStructure,
                                aClipChain,
                                aSubPlaneIndex,
                                aPlaneRes);

      // set delayed resource release
      aPlaneRes.Nullify();
      aContext->ReleaseResource(aResId, Standard_True);
    }
  }

  // restore rendering aspects
  theWorkspace->SetAspects(aFaceAsp);
  theWorkspace->SetRenderFilter(aPrevFilter);
}
