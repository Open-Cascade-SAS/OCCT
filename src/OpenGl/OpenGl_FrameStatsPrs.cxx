// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <OpenGl_FrameStatsPrs.hxx>

#include <OpenGl_View.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Workspace.hxx>

// =======================================================================
// function : OpenGl_FrameStatsPrs
// purpose  :
// =======================================================================
OpenGl_FrameStatsPrs::OpenGl_FrameStatsPrs()
: myStatsPrev (new OpenGl_FrameStats())
{
  myParams.HAlign = Graphic3d_HTA_CENTER;
  myParams.VAlign = Graphic3d_VTA_CENTER;
  myHasPlane      = false;
}

// =======================================================================
// function : ~OpenGl_FrameStatsPrs
// purpose  :
// =======================================================================
OpenGl_FrameStatsPrs::~OpenGl_FrameStatsPrs()
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_FrameStatsPrs::Release (OpenGl_Context* theCtx)
{
  OpenGl_Text::Release (theCtx);
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void OpenGl_FrameStatsPrs::Update (const Handle(OpenGl_Workspace)& theWorkspace)
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  const Handle(OpenGl_FrameStats)& aStats = aCtx->FrameStats();
  const Graphic3d_RenderingParams& aRendParams = theWorkspace->View()->RenderingParams();
  myTrsfPers = theWorkspace->View()->RenderingParams().StatsPosition;
  myTextAspect.SetAspect (aRendParams.StatsTextAspect);

  // adjust text alignment depending on corner
  const OpenGl_TextParam aParamsPrev = myParams;
  myParams.Height = aRendParams.StatsTextHeight;
  myParams.HAlign = Graphic3d_HTA_CENTER;
  myParams.VAlign = Graphic3d_VTA_CENTER;
  if (!myTrsfPers.IsNull() && (myTrsfPers->Corner2d() & Aspect_TOTP_LEFT) != 0)
  {
    myParams.HAlign = Graphic3d_HTA_LEFT;
  }
  else if (!myTrsfPers.IsNull() && (myTrsfPers->Corner2d() & Aspect_TOTP_RIGHT) != 0)
  {
    myParams.HAlign = Graphic3d_HTA_RIGHT;
  }
  if (!myTrsfPers.IsNull() && (myTrsfPers->Corner2d() & Aspect_TOTP_TOP) != 0)
  {
    myParams.VAlign = Graphic3d_VTA_TOP;
  }
  else if (!myTrsfPers.IsNull() && (myTrsfPers->Corner2d() & Aspect_TOTP_BOTTOM) != 0)
  {
    myParams.VAlign = Graphic3d_VTA_BOTTOM;
  }
  if (myParams.Height != aParamsPrev.Height
   || myParams.HAlign != aParamsPrev.HAlign
   || myParams.VAlign != aParamsPrev.VAlign)
  {
    Release (aCtx.operator->());
  }

  if (myStatsPrev->IsEqual (aStats)
  && !myString.IsEmpty())
  {
    return;
  }

  myStatsPrev->CopyFrom (aStats);
  const TCollection_AsciiString aText = aStats->FormatStats (aRendParams.CollectedStats);

  releaseVbos (aCtx.operator->());
  myString.FromUnicode (aText.ToCString());
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_FrameStatsPrs::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  const Standard_Boolean wasEnabledDepth = theWorkspace->UseDepthWrite();
  if (theWorkspace->UseDepthWrite())
  {
    theWorkspace->UseDepthWrite() = Standard_False;
    glDepthMask (GL_FALSE);
  }

  aCtx->ModelWorldState.Push();
  aCtx->ModelWorldState.ChangeCurrent().InitIdentity();

  aCtx->WorldViewState.Push();
  OpenGl_Mat4& aWorldView = aCtx->WorldViewState.ChangeCurrent();
  if (!myTrsfPers.IsNull())
  {
    myTrsfPers->Apply (theWorkspace->View()->Camera(),
                      aCtx->ProjectionState.Current(), aWorldView,
                      aCtx->VirtualViewport()[2], aCtx->VirtualViewport()[3]);
  }

  aCtx->ApplyModelViewMatrix();

  const OpenGl_AspectText* aTextAspectBack = theWorkspace->SetAspectText (&myTextAspect);
  OpenGl_Text::Render (theWorkspace);
  theWorkspace->SetAspectText (aTextAspectBack);

  aCtx->WorldViewState.Pop();
  aCtx->ModelWorldState.Pop();
  aCtx->ApplyWorldViewMatrix();

  if (theWorkspace->UseDepthWrite() != wasEnabledDepth)
  {
    theWorkspace->UseDepthWrite() = wasEnabledDepth;
    glDepthMask (wasEnabledDepth ? GL_TRUE : GL_FALSE);
  }
}
