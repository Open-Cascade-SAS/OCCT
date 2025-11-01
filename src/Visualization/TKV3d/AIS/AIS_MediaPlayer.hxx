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

#ifndef _AIS_MediaPlayer_HeaderFile
#define _AIS_MediaPlayer_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_MediaTextureSet.hxx>

class Media_PlayerContext;

//! Presentation for video playback.
class AIS_MediaPlayer : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_MediaPlayer, AIS_InteractiveObject)
public:
  //! Empty constructor.
  Standard_EXPORT AIS_MediaPlayer();

  //! Destructor.
  Standard_EXPORT virtual ~AIS_MediaPlayer();

  //! Setup callback to be called on queue progress (e.g. when new frame should be displayed).
  void SetCallback(Graphic3d_MediaTextureSet::CallbackOnUpdate_t theCallbackFunction,
                   void*                                         theCallbackUserPtr)
  {
    myFramePair->SetCallback(theCallbackFunction, theCallbackUserPtr);
  }

  //! Open specified file.
  Standard_EXPORT void OpenInput(const TCollection_AsciiString& thePath,
                                 Standard_Boolean               theToWait);

  //! Display new frame.
  Standard_EXPORT bool PresentFrame(const Graphic3d_Vec2i& theLeftCorner,
                                    const Graphic3d_Vec2i& theMaxSize);

  //! Return player context.
  const Handle(Media_PlayerContext)& PlayerContext() const { return myFramePair->PlayerContext(); }

  //! Switch playback state.
  Standard_EXPORT void PlayPause();

  //! Schedule player to be closed.
  void SetClosePlayer()
  {
    myToClosePlayer = true;
    myFramePair->Notify();
  }

  //! Return duration.
  double Duration() const { return myFramePair->Duration(); }

  //! @name AIS_InteractiveObject interface
protected:
  //! Accept only display mode 0.
  virtual Standard_Boolean AcceptDisplayMode(const Standard_Integer theMode) const Standard_OVERRIDE
  {
    return theMode == 0;
  }

  //! Compute presentation.
  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute selection
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                                const Standard_Integer theMode) Standard_OVERRIDE;

protected:
  //! Update frame size.
  Standard_EXPORT bool updateSize(const Graphic3d_Vec2i& theLeftCorner,
                                  const Graphic3d_Vec2i& theMaxSize);

protected:
  Handle(Graphic3d_MediaTextureSet)  myFramePair;
  Handle(Graphic3d_AspectFillArea3d) myFrameAspect;
  Graphic3d_Vec2i                    myFrameBottomLeft;
  Graphic3d_Vec2i                    myFrameSize;
  bool                               myToClosePlayer;
};

#endif // _AIS_MediaPlayer_HeaderFile
