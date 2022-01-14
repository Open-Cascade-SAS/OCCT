// Created on: 2021-01-15
// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <OpenGl_DepthPeeling.hxx>

#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Texture.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_DepthPeeling, OpenGl_NamedResource)

//=======================================================================
// function : OpenGl_DepthPeeling
// purpose  :
//=======================================================================
OpenGl_DepthPeeling::OpenGl_DepthPeeling()
: OpenGl_NamedResource ("depth_peeling")
{
  myDepthPeelFbosOit[0]      = new OpenGl_FrameBuffer (myResourceId + ":fbo0");
  myDepthPeelFbosOit[1]      = new OpenGl_FrameBuffer (myResourceId + ":fbo1");
  myFrontBackColorFbosOit[0] = new OpenGl_FrameBuffer (myResourceId + ":fbo0_color");
  myFrontBackColorFbosOit[1] = new OpenGl_FrameBuffer (myResourceId + ":fbo1_color");
  myBlendBackFboOit          = new OpenGl_FrameBuffer (myResourceId + ":fbo_blend");
}

// =======================================================================
// function : ~OpenGl_DepthPeeling
// purpose  :
// =======================================================================
OpenGl_DepthPeeling::~OpenGl_DepthPeeling()
{
  Release (NULL);
}

//=======================================================================
// function : Release
// purpose  :
//=======================================================================
void OpenGl_DepthPeeling::Release (OpenGl_Context* theCtx)
{
  myDepthPeelFbosOit[0]     ->Release (theCtx);
  myDepthPeelFbosOit[1]     ->Release (theCtx);
  myFrontBackColorFbosOit[0]->Release (theCtx);
  myFrontBackColorFbosOit[1]->Release (theCtx);
  myBlendBackFboOit         ->Release (theCtx);
}

//=======================================================================
// function : EstimatedDataSize
// purpose  :
//=======================================================================
Standard_Size OpenGl_DepthPeeling::EstimatedDataSize() const
{
  return myDepthPeelFbosOit[0]->EstimatedDataSize()
       + myDepthPeelFbosOit[1]->EstimatedDataSize()
       + myFrontBackColorFbosOit[0]->EstimatedDataSize()
       + myFrontBackColorFbosOit[1]->EstimatedDataSize()
       + myBlendBackFboOit->EstimatedDataSize();
}

//=======================================================================
// function : AttachDepthTexture
// purpose  :
//=======================================================================
void OpenGl_DepthPeeling::AttachDepthTexture (const Handle(OpenGl_Context)& theCtx,
                                              const Handle(OpenGl_Texture)& theDepthStencilTexture)
{
  if (!theDepthStencilTexture.IsNull()
    && theDepthStencilTexture->IsValid())
  {
    for (int aPairIter = 0; aPairIter < 2; ++aPairIter)
    {
      myDepthPeelFbosOit[aPairIter]->BindBuffer (theCtx);
      theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                              theDepthStencilTexture->GetTarget(),
                                              theDepthStencilTexture->TextureId(), 0);
      myDepthPeelFbosOit[aPairIter]->UnbindBuffer (theCtx);
    }
  }
}

//=======================================================================
// function : DetachDepthTexture
// purpose  :
//=======================================================================
void OpenGl_DepthPeeling::DetachDepthTexture (const Handle(OpenGl_Context)& theCtx)
{
  if (!myDepthPeelFbosOit[0]->DepthStencilTexture().IsNull())
  {
    for (int aPairIter = 0; aPairIter < 2; ++aPairIter)
    {
      myDepthPeelFbosOit[aPairIter]->BindBuffer (theCtx);
      theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                              myDepthPeelFbosOit[aPairIter]->DepthStencilTexture()->GetTarget(),
                                              0, 0);
      myDepthPeelFbosOit[aPairIter]->UnbindBuffer (theCtx);
    }
  }
}
