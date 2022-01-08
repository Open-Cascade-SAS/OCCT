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
#include <OpenGl_IndexBuffer.hxx>
#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>

namespace
{
  //! Auxiliary structure defining vertex with two attributes.
  struct OpenGl_Vec3Vec4ub
  {
    Graphic3d_Vec3   Pos;
    Graphic3d_Vec4ub Color;
  };

  //! Auxiliary function formatting rendering time in " 10 ms (100 FPS)" format.
  static TCollection_AsciiString formatTimeMs (Standard_Real theSeconds)
  {
    const Standard_Real aFpsVal = theSeconds != 0.0 ? 1.0 / theSeconds : 0.0;
    char aFps[50];
    Sprintf (aFps, "%.1f", aFpsVal);
    return TCollection_AsciiString() + Standard_Integer(theSeconds * 1000.0) + " ms (" + aFps + " FPS)";
  }
}

// =======================================================================
// function : OpenGl_FrameStatsPrs
// purpose  :
// =======================================================================
OpenGl_FrameStatsPrs::OpenGl_FrameStatsPrs()
: myStatsPrev (new OpenGl_FrameStats()),
  myCountersTrsfPers (new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_UPPER,  Graphic3d_Vec2i (20, 20))),
  myChartTrsfPers    (new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_RIGHT_UPPER, Graphic3d_Vec2i (20, 20))),
  myChartVertices (new OpenGl_VertexBuffer()),
  myChartIndices (new OpenGl_IndexBuffer()),
  myChartLines (new OpenGl_VertexBuffer())
{
  //
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
  myCountersText.Release (theCtx);
  myChartLabels[0].Release (theCtx);
  myChartLabels[1].Release (theCtx);
  myChartLabels[2].Release (theCtx);
  myChartVertices->Release (theCtx);
  myChartIndices->Release (theCtx);
  myChartLines->Release (theCtx);
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
  myCountersTrsfPers = theWorkspace->View()->RenderingParams().StatsPosition;
  myChartTrsfPers    = theWorkspace->View()->RenderingParams().ChartPosition;
  myTextAspect.SetAspect (aRendParams.StatsTextAspect);

  // adjust text alignment depending on corner
  Graphic3d_Text aParams ((Standard_ShortReal)aRendParams.StatsTextHeight);
  aParams.SetHorizontalAlignment (Graphic3d_HTA_CENTER);
  aParams.SetVerticalAlignment (Graphic3d_VTA_CENTER);
  if (!myCountersTrsfPers.IsNull() && (myCountersTrsfPers->Corner2d() & Aspect_TOTP_LEFT) != 0)
  {
    aParams.SetHorizontalAlignment (Graphic3d_HTA_LEFT);
  }
  else if (!myCountersTrsfPers.IsNull() && (myCountersTrsfPers->Corner2d() & Aspect_TOTP_RIGHT) != 0)
  {
    aParams.SetHorizontalAlignment (Graphic3d_HTA_RIGHT);
  }
  if (!myCountersTrsfPers.IsNull() && (myCountersTrsfPers->Corner2d() & Aspect_TOTP_TOP) != 0)
  {
    aParams.SetVerticalAlignment (Graphic3d_VTA_TOP);
  }
  else if (!myCountersTrsfPers.IsNull() && (myCountersTrsfPers->Corner2d() & Aspect_TOTP_BOTTOM) != 0)
  {
    aParams.SetVerticalAlignment (Graphic3d_VTA_BOTTOM);
  }
  if (aParams.Height() != myCountersText.Text()->Height()
   || aParams.HorizontalAlignment() != myCountersText.Text()->HorizontalAlignment()
   || aParams.VerticalAlignment() != myCountersText.Text()->VerticalAlignment())
  {
    myCountersText.Release (aCtx.operator->());
  }

  if (!aStats->IsFrameUpdated (myStatsPrev)
   && !myCountersText.Text()->Text().IsEmpty())
  {
    return;
  }

  Handle(Graphic3d_Text) aText = myCountersText.Text();
  aText->SetText (aStats->FormatStats (aRendParams.CollectedStats).ToCString());
  aText->SetHeight (aParams.Height());
  aText->SetPosition (gp_Pnt());
  aText->SetHorizontalAlignment (aParams.HorizontalAlignment());
  aText->SetVerticalAlignment (aParams.VerticalAlignment());
  myCountersText.Reset (aCtx);

  updateChart (theWorkspace);
}

// =======================================================================
// function : updateChart
// purpose  :
// =======================================================================
void OpenGl_FrameStatsPrs::updateChart (const Handle(OpenGl_Workspace)& theWorkspace)
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  const Handle(OpenGl_FrameStats)& aStats = aCtx->FrameStats();
  const Graphic3d_RenderingParams& aRendParams = theWorkspace->View()->RenderingParams();

  const Standard_Integer aNbBins = aStats->DataFrames().Size();
  if (aNbBins <= 1)
  {
    myChartIndices ->Release (aCtx.get());
    myChartVertices->Release (aCtx.get());
    myChartLines   ->Release (aCtx.get());
    return;
  }

  Standard_Real aMaxDuration = aRendParams.StatsMaxChartTime;
  if (aMaxDuration <= 0.0f)
  {
    for (Standard_Integer aFrameIter = aStats->DataFrames().Lower(); aFrameIter <= aStats->DataFrames().Upper(); ++aFrameIter)
    {
      const Graphic3d_FrameStatsData& aFrame = aStats->DataFrames().Value (aFrameIter);
      aMaxDuration = Max (aMaxDuration, aFrame.TimerValue (Graphic3d_FrameStatsTimer_ElapsedFrame));
    }
    aMaxDuration = Ceiling (aMaxDuration * 1000.0 * 0.1) * 0.001 * 10.0; // round number
    aMaxDuration = Max (Min (aMaxDuration, 0.1), 0.005); // limit by 100 ms (10 FPS) and 5 ms (200 FPS)
  }

  const Standard_Integer aNbTimers = 4;
  const Graphic3d_FrameStatsTimer aTimers[4] =
  {
    Graphic3d_FrameStatsTimer_CpuDynamics,
    Graphic3d_FrameStatsTimer_CpuPicking,
    Graphic3d_FrameStatsTimer_CpuCulling,
    Graphic3d_FrameStatsTimer_ElapsedFrame,
  };
  const Graphic3d_Vec4ub aColors[4] =
  {
    Graphic3d_Vec4ub (255, 0, 0, 127),
    Graphic3d_Vec4ub (255, 127, 39, 127),
    Graphic3d_Vec4ub (255, 0, 0, 127),
    Graphic3d_Vec4ub (0, 255, 0, 127),
  };

  const Standard_Integer aNbVerts   = aNbBins * 4 * aNbTimers;
  const Standard_Integer aNbIndexes = aNbBins * 2 * 3 * aNbTimers;
  bool toFillEdges = false;
  if (myChartArray.IsNull()
   || myChartArray->VertexNumber() != aNbVerts
   || myChartArray->EdgeNumber()   != aNbIndexes)
  {
    myChartArray = new Graphic3d_ArrayOfTriangles (aNbVerts, aNbIndexes, false, true);
    toFillEdges = true;
  }

  const Graphic3d_Vec2i aViewSize (aCtx->VirtualViewport()[2], aCtx->VirtualViewport()[3]);
  Graphic3d_Vec2i aCharSize (aRendParams.ChartSize);
  if (aCharSize.x() <= 0)
  {
    aCharSize.x() = aViewSize.x() / 2;
  }
  if (aCharSize.y() <= 0)
  {
    aCharSize.y() = Standard_Integer(0.15 * aViewSize.y());
  }

  const Graphic3d_Vec2d aBinSize  (Standard_Real(aCharSize.x()) / Standard_Real(aNbBins), 0.15 * aViewSize.y());
  Graphic3d_Vec2i anOffset;
  if (!myChartTrsfPers.IsNull()
    && myChartTrsfPers->IsTrihedronOr2d())
  {
    if ((myChartTrsfPers->Corner2d() & Aspect_TOTP_LEFT) != 0)
    {
      anOffset.x() = 0;
    }
    else if ((myChartTrsfPers->Corner2d() & Aspect_TOTP_RIGHT) != 0)
    {
      anOffset.x() = -aCharSize.x();
    }
    else
    {
      anOffset.x() = -aCharSize.x() / 2;
    }

    if ((myChartTrsfPers->Corner2d() & Aspect_TOTP_BOTTOM) != 0)
    {
      anOffset.y() = aCharSize.y();
    }
    else if ((myChartTrsfPers->Corner2d() & Aspect_TOTP_TOP) != 0)
    {
      anOffset.y() = 0;
    }
    else
    {
      anOffset.y() = aCharSize.y() / 2;
    }
  }

  Standard_Integer aVertLast = 1;
  const bool isTopDown = false;
  for (Standard_Integer aFrameIter = 0; aFrameIter < aNbBins; ++aFrameIter)
  {
    Standard_Integer aFrameIndex = aStats->DataFrames().Lower() + aStats->LastDataFrameIndex() + 1 + aFrameIter;
    if (aFrameIndex > aStats->DataFrames().Upper())
    {
      aFrameIndex -= aNbBins;
    }

    const Graphic3d_FrameStatsData& aFrame = aStats->DataFrames().Value (aFrameIndex);
    Standard_Real aTimeElapsed = 0.0;
    Standard_Real aCurrY = 0.0;
    for (Standard_Integer aTimerIter = 0; aTimerIter < aNbTimers; ++aTimerIter)
    {
      if (aTimers[aTimerIter] == Graphic3d_FrameStatsTimer_ElapsedFrame)
      {
        aTimeElapsed = aFrame.TimerValue (aTimers[aTimerIter]);
      }
      else
      {
        aTimeElapsed += aFrame.TimerValue (aTimers[aTimerIter]);
      }

      const Standard_Real aBinX1 = anOffset.x() + Standard_Real(aFrameIter) * aBinSize.x();
      const Standard_Real aBinX2 = aBinX1 + aBinSize.x();
      const Standard_Real aCurrSizeY = Min (aTimeElapsed / aMaxDuration, 1.2) * aBinSize.y();
      const Standard_Real aBinY1 = isTopDown ? (anOffset.y() - aCurrY)     : (anOffset.y() - aBinSize.y() + aCurrY);
      const Standard_Real aBinY2 = isTopDown ? (anOffset.y() - aCurrSizeY) : (anOffset.y() - aBinSize.y() + aCurrSizeY);
      myChartArray->SetVertice (aVertLast + 0, gp_Pnt (aBinX1, aBinY2, 0.0));
      myChartArray->SetVertice (aVertLast + 1, gp_Pnt (aBinX1, aBinY1, 0.0));
      myChartArray->SetVertice (aVertLast + 2, gp_Pnt (aBinX2, aBinY1, 0.0));
      myChartArray->SetVertice (aVertLast + 3, gp_Pnt (aBinX2, aBinY2, 0.0));

      if (toFillEdges)
      {
        const Graphic3d_Vec4ub& aTimerColor = aColors[aTimerIter];
        myChartArray->SetVertexColor (aVertLast + 0, aTimerColor);
        myChartArray->SetVertexColor (aVertLast + 1, aTimerColor);
        myChartArray->SetVertexColor (aVertLast + 2, aTimerColor);
        myChartArray->SetVertexColor (aVertLast + 3, aTimerColor);
        myChartArray->AddEdges (aVertLast + 0, aVertLast + 1, aVertLast + 3);
        myChartArray->AddEdges (aVertLast + 1, aVertLast + 2, aVertLast + 3);
      }
      aVertLast += 4;

      if (aTimers[aTimerIter] == Graphic3d_FrameStatsTimer_ElapsedFrame)
      {
        aTimeElapsed = 0.0;
        aCurrY = 0.0;
      }
      else
      {
        aCurrY = aCurrSizeY;
      }
    }
  }

  myChartVertices->init (aCtx,
                         myChartArray->Attributes()->Stride,
                         myChartArray->Attributes()->NbElements,
                         myChartArray->Attributes()->Data(),
                         GL_UNSIGNED_BYTE,
                         myChartArray->Attributes()->Stride);
  if (myChartArray->Indices()->Stride == 2)
  {
    myChartIndices ->Init (aCtx,
                           1,
                           myChartArray->Indices()->NbElements,
                           (const GLushort* )myChartArray->Indices()->Data());
  }
  else if (myChartArray->Indices()->Stride == 4)
  {
    myChartIndices ->Init (aCtx,
                           1,
                           myChartArray->Indices()->NbElements,
                           (const GLuint* )myChartArray->Indices()->Data());
  }

  {
    const Graphic3d_Vec4ub aWhite (255, 255, 255, 255);
    const OpenGl_Vec3Vec4ub aLines[4] =
    {
      { Graphic3d_Vec3((float )anOffset.x(), (float )anOffset.y(),                0.0f), aWhite },
      { Graphic3d_Vec3(float(anOffset.x() + aCharSize.x()), (float )anOffset.y(), 0.0f), aWhite },
      { Graphic3d_Vec3((float )anOffset.x(), float(anOffset.y() - aBinSize.y()),  0.0f), aWhite },
      { Graphic3d_Vec3(float(anOffset.x() + aCharSize.x()), float(anOffset.y() - aBinSize.y()),+ 0.0f), aWhite },
    };
    myChartLines->init (aCtx, sizeof(OpenGl_Vec3Vec4ub), 4, aLines, GL_UNSIGNED_BYTE, sizeof(OpenGl_Vec3Vec4ub));
  }

  {
    Graphic3d_Text aParams ((Standard_ShortReal)aRendParams.StatsTextHeight);
    aParams.SetHorizontalAlignment ((!myChartTrsfPers.IsNull()
                    && myChartTrsfPers->IsTrihedronOr2d()
                    && (myChartTrsfPers->Corner2d() & Aspect_TOTP_RIGHT) != 0)
                    ? Graphic3d_HTA_RIGHT
                    : Graphic3d_HTA_LEFT);
    aParams.SetVerticalAlignment (Graphic3d_VTA_CENTER);
    TCollection_AsciiString aLabels[3] =
    {
      TCollection_AsciiString() + 0 + " ms",
      formatTimeMs(aMaxDuration * 0.5),
      formatTimeMs(aMaxDuration)
    };

    const float aLabX = aParams.HorizontalAlignment() == Graphic3d_HTA_RIGHT
                      ? float(anOffset.x())
                      : float(anOffset.x() + aCharSize.x());

    myChartLabels[0].Text()->SetText (aLabels[isTopDown ? 0 : 2].ToCString());
    myChartLabels[0].Text()->SetPosition (gp_Pnt (aLabX, float(anOffset.y()), 0.0f));

    myChartLabels[1].Text()->SetText (aLabels[isTopDown ? 1 : 1].ToCString());
    myChartLabels[1].Text()->SetPosition (gp_Pnt (aLabX, float(anOffset.y() - aBinSize.y() / 2), 0.0f));

    myChartLabels[2].Text()->SetText (aLabels[isTopDown ? 2 : 0].ToCString());
    myChartLabels[2].Text()->SetPosition (gp_Pnt (aLabX, float(anOffset.y() - aBinSize.y()), 0.0f));

    for (int i = 0; i < 3; i++)
    {
      myChartLabels[i].Text()->SetHeight (aParams.Height());
      myChartLabels[i].Text()->SetHorizontalAlignment (aParams.HorizontalAlignment());
      myChartLabels[i].Text()->SetVerticalAlignment (aParams.VerticalAlignment());

      myChartLabels[i].Reset(aCtx);
    }
  }
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
    aCtx->core11fwd->glDepthMask (GL_FALSE);
  }
  const bool wasDepthClamped = aCtx->arbDepthClamp && aCtx->core11fwd->glIsEnabled (GL_DEPTH_CLAMP);
  if (aCtx->arbDepthClamp && !wasDepthClamped)
  {
    aCtx->core11fwd->glEnable (GL_DEPTH_CLAMP);
  }

  const OpenGl_Aspects* aTextAspectBack = theWorkspace->SetAspects (&myTextAspect);

  aCtx->ModelWorldState.Push();
  aCtx->ModelWorldState.ChangeCurrent().InitIdentity();

  // draw counters
  {
    aCtx->WorldViewState.Push();
    if (!myCountersTrsfPers.IsNull())
    {
      myCountersTrsfPers->Apply (aCtx->Camera(),
                                 aCtx->ProjectionState.Current(), aCtx->WorldViewState.ChangeCurrent(),
                                 aCtx->VirtualViewport()[2], aCtx->VirtualViewport()[3]);
    }
    aCtx->ApplyModelViewMatrix();
    myCountersText.Render (theWorkspace);
    aCtx->WorldViewState.Pop();
  }

  // draw chart
  if (myChartIndices->IsValid()
   && myChartIndices->GetElemsNb() > 0)
  {
    aCtx->WorldViewState.Push();
    if (!myChartTrsfPers.IsNull())
    {
      myChartTrsfPers->Apply (aCtx->Camera(),
                              aCtx->ProjectionState.Current(), aCtx->WorldViewState.ChangeCurrent(),
                              aCtx->VirtualViewport()[2], aCtx->VirtualViewport()[3]);
    }
    aCtx->ApplyModelViewMatrix();

    aCtx->ShaderManager()->BindFaceProgram (Handle(OpenGl_TextureSet)(), Graphic3d_TypeOfShadingModel_Unlit,
                                            Graphic3d_AlphaMode_Blend, true, false,
                                            Handle(OpenGl_ShaderProgram)());
    aCtx->SetColor4fv (OpenGl_Vec4 (1.0f, 1.0f, 1.0f, 1.0f));
    aCtx->core15fwd->glEnable (GL_BLEND);
    aCtx->core15fwd->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    myChartVertices->Bind (aCtx);
    myChartVertices->bindAttribute (aCtx, Graphic3d_TOA_POS,   3, GL_FLOAT,         myChartVertices->GetComponentsNb(), NULL);
    myChartVertices->bindAttribute (aCtx, Graphic3d_TOA_COLOR, 4, GL_UNSIGNED_BYTE, myChartVertices->GetComponentsNb(), (void* )sizeof(Graphic3d_Vec3));

    myChartIndices->Bind (aCtx);
    aCtx->core15fwd->glDrawElements (GL_TRIANGLES, myChartIndices->GetElemsNb(), myChartIndices->GetDataType(), NULL);
    myChartIndices->Unbind (aCtx);
    myChartVertices->Unbind (aCtx);
    myChartVertices->unbindAttribute (aCtx, Graphic3d_TOA_COLOR);
    myChartVertices->unbindAttribute (aCtx, Graphic3d_TOA_POS);
    aCtx->core15fwd->glDisable (GL_BLEND);

    myChartLines->Bind (aCtx);
    myChartLines->bindAttribute (aCtx, Graphic3d_TOA_POS,   3, GL_FLOAT,         myChartLines->GetComponentsNb(), NULL);
    myChartLines->bindAttribute (aCtx, Graphic3d_TOA_COLOR, 4, GL_UNSIGNED_BYTE, myChartLines->GetComponentsNb(), (void* )sizeof(Graphic3d_Vec3));
    aCtx->core15fwd->glDrawArrays (GL_LINES, 0, myChartLines->GetElemsNb());
    myChartLines->Unbind (aCtx);
    myChartLines->unbindAttribute (aCtx, Graphic3d_TOA_COLOR);
    myChartLines->unbindAttribute (aCtx, Graphic3d_TOA_POS);

    myChartLabels[0].Render (theWorkspace);
    myChartLabels[1].Render (theWorkspace);
    myChartLabels[2].Render (theWorkspace);

    aCtx->WorldViewState.Pop();
  }

  aCtx->ModelWorldState.Pop();
  aCtx->ApplyWorldViewMatrix();

  theWorkspace->SetAspects (aTextAspectBack);
  if (theWorkspace->UseDepthWrite() != wasEnabledDepth)
  {
    theWorkspace->UseDepthWrite() = wasEnabledDepth;
    aCtx->core11fwd->glDepthMask (wasEnabledDepth ? GL_TRUE : GL_FALSE);
  }
  if (aCtx->arbDepthClamp && !wasDepthClamped)
  {
    aCtx->core11fwd->glDisable (GL_DEPTH_CLAMP);
  }
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_FrameStatsPrs::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, OpenGl_FrameStatsPrs)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, OpenGl_Element)
}
