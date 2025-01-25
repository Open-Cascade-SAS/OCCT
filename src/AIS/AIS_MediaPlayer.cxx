// Created by: Kirill GAVRILOV
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <AIS_MediaPlayer.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Media_PlayerContext.hxx>
#include <Message_Messenger.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_MediaPlayer, AIS_InteractiveObject)

//! Create an array of triangles defining a rectangle.
static Handle(Graphic3d_ArrayOfTriangles) createRectangleArray(const Graphic3d_Vec2i& theLower,
                                                               const Graphic3d_Vec2i& theUpper,
                                                               Graphic3d_ArrayFlags   theFlags)
{
  Handle(Graphic3d_ArrayOfTriangles) aRectTris = new Graphic3d_ArrayOfTriangles(4, 6, theFlags);
  aRectTris->AddVertex(gp_Pnt(theLower.x(), theLower.y(), 0.0), gp_Pnt2d(0.0, 1.0));
  aRectTris->AddVertex(gp_Pnt(theLower.x(), theUpper.y(), 0.0), gp_Pnt2d(0.0, 0.0));
  aRectTris->AddVertex(gp_Pnt(theUpper.x(), theUpper.y(), 0.0), gp_Pnt2d(1.0, 0.0));
  aRectTris->AddVertex(gp_Pnt(theUpper.x(), theLower.y(), 0.0), gp_Pnt2d(1.0, 1.0));
  aRectTris->AddEdges(1, 2, 3);
  aRectTris->AddEdges(1, 3, 4);
  return aRectTris;
}

//=================================================================================================

AIS_MediaPlayer::AIS_MediaPlayer()
    : myFramePair(new Graphic3d_MediaTextureSet()),
      myFrameSize(1, 1),
      myToClosePlayer(false)
{
  SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  SetZLayer(Graphic3d_ZLayerId_TopOSD);
  SetInfiniteState(true);

  Graphic3d_MaterialAspect aMat;
  myFrameAspect = new Graphic3d_AspectFillArea3d(Aspect_IS_SOLID,
                                                 Quantity_NOC_WHITE,
                                                 Quantity_NOC_BLACK,
                                                 Aspect_TOL_SOLID,
                                                 1.0f,
                                                 aMat,
                                                 aMat);
  myFrameAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myFrameAspect->SetTextureMapOn(true);
  myFrameAspect->SetTextureSet(myFramePair);
}

//=================================================================================================

AIS_MediaPlayer::~AIS_MediaPlayer()
{
  // stop threads
  myFramePair.Nullify();
}

// =======================================================================
// function : OpenInput
// purpose  :
// =======================================================================
void AIS_MediaPlayer::OpenInput(const TCollection_AsciiString& thePath, Standard_Boolean theToWait)
{
  if (myFramePair->PlayerContext().IsNull() && thePath.IsEmpty())
  {
    return;
  }

  myFramePair->OpenInput(thePath, theToWait);
  SynchronizeAspects();
}

// =======================================================================
// function : PresentFrame
// purpose  :
// =======================================================================
bool AIS_MediaPlayer::PresentFrame(const Graphic3d_Vec2i& theLeftCorner,
                                   const Graphic3d_Vec2i& theMaxSize)
{
  if (myToClosePlayer)
  {
    myToClosePlayer = false;
    if (!HasInteractiveContext())
    {
      return false;
    }

    if (!myFramePair->PlayerContext().IsNull())
    {
      myFramePair->PlayerContext()->Pause();
    }

    Handle(AIS_InteractiveContext) aCtx  = GetContext();
    Handle(AIS_InteractiveObject)  aThis = this;
    aCtx->Remove(aThis, false);
    aCtx->CurrentViewer()->Invalidate();
    return true;
  }

  if (myFramePair->PlayerContext().IsNull())
  {
    return false;
  }

  bool toRedraw = myFramePair->SwapFrames();
  toRedraw      = updateSize(theLeftCorner, theMaxSize) || toRedraw;
  if (toRedraw)
  {
    myFrameAspect->SetShaderProgram(myFramePair->ShaderProgram());
    SynchronizeAspects();
  }
  return toRedraw;
}

// =======================================================================
// function : updateSize
// purpose  :
// =======================================================================
bool AIS_MediaPlayer::updateSize(const Graphic3d_Vec2i& theLeftCorner,
                                 const Graphic3d_Vec2i& theMaxSize)
{
  const Graphic3d_Vec2i aFrameSize = myFramePair->FrameSize();
  Graphic3d_Vec2i       aNewPos    = theLeftCorner;
  Graphic3d_Vec2i       aNewSize   = myFrameSize;
  if (aFrameSize.x() > 0 && aFrameSize.y() > 0)
  {
    const double anAspect   = double(theMaxSize.x()) / double(theMaxSize.y());
    const double aFitAspect = double(aFrameSize.x()) / double(aFrameSize.y());
    aNewSize                = aFrameSize;
    if (aFitAspect >= anAspect)
    {
      aNewSize.y() = int(double(aFrameSize.x()) / aFitAspect);
    }
    else
    {
      aNewSize.x() = int(double(aFrameSize.y()) * aFitAspect);
    }

    for (int aCoord = 0; aCoord < 2; ++aCoord)
    {
      if (aNewSize[aCoord] > theMaxSize[aCoord])
      {
        const double aScale = double(theMaxSize[aCoord]) / double(aNewSize[aCoord]);
        aNewSize.x()        = int(double(aNewSize.x()) * aScale);
        aNewSize.y()        = int(double(aNewSize.y()) * aScale);
      }
    }

    aNewPos = theLeftCorner + theMaxSize / 2 - aNewSize / 2;
  }
  else if (myFrameSize.x() < 2 || myFrameSize.y() < 2)
  {
    aNewSize = theMaxSize;
  }

  if (myFrameSize == aNewSize && myFrameBottomLeft == aNewPos)
  {
    return false;
  }

  myFrameSize       = aNewSize;
  myFrameBottomLeft = aNewPos;
  if (HasInteractiveContext())
  {
    SetToUpdate();
    GetContext()->Redisplay(this, false);
    GetContext()->CurrentViewer()->Invalidate();
  }
  return true;
}

// =======================================================================
// function : PlayPause
// purpose  :
// =======================================================================
void AIS_MediaPlayer::PlayPause()
{
  if (myFramePair->PlayerContext().IsNull())
  {
    return;
  }

  Standard_Real aProgress = 0.0, aDuration = 0.0;
  bool          isPaused = false;
  myFramePair->PlayerContext()->PlayPause(isPaused, aProgress, aDuration);
}

// =======================================================================
// function : Compute
// purpose  :
// =======================================================================
void AIS_MediaPlayer::Compute(const Handle(PrsMgr_PresentationManager)&,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer            theMode)
{
  thePrs->SetInfiniteState(IsInfinite());
  if (theMode != 0)
  {
    return;
  }

  // main frame
  {
    Handle(Graphic3d_ArrayOfTriangles) aTris =
      createRectangleArray(myFrameBottomLeft,
                           myFrameBottomLeft + myFrameSize,
                           Graphic3d_ArrayFlags_VertexTexel);
    Handle(Graphic3d_Group) aMainGroup = thePrs->NewGroup();
    aMainGroup->SetGroupPrimitivesAspect(myFrameAspect);
    aMainGroup->AddPrimitiveArray(aTris);
  }
}

// =======================================================================
// function : ComputeSelection
// purpose  :
// =======================================================================
void AIS_MediaPlayer::ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                       const Standard_Integer             theMode)
{
  if (theMode != 0)
  {
    return;
  }

  Handle(Graphic3d_ArrayOfTriangles) aTris = createRectangleArray(myFrameBottomLeft,
                                                                  myFrameBottomLeft + myFrameSize,
                                                                  Graphic3d_ArrayFlags_None);

  Handle(SelectMgr_EntityOwner)            anOwner = new SelectMgr_EntityOwner(this, 5);
  Handle(Select3D_SensitivePrimitiveArray) aSens   = new Select3D_SensitivePrimitiveArray(anOwner);
  aSens->InitTriangulation(aTris->Attributes(), aTris->Indices(), TopLoc_Location());
  theSel->Add(aSens);
}
