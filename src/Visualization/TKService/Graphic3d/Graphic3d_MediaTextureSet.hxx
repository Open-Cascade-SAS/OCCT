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

#ifndef _Graphic3d_MediaTextureSet_HeaderFile
#define _Graphic3d_MediaTextureSet_HeaderFile

#include <Media_IFrameQueue.hxx>
#include <Graphic3d_MediaTexture.hxx>
#include <Graphic3d_TextureSet.hxx>

class Graphic3d_ShaderProgram;
class Media_PlayerContext;

//! Texture adapter for Media_Frame.
class Graphic3d_MediaTextureSet : public Graphic3d_TextureSet, public Media_IFrameQueue
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_MediaTextureSet, Graphic3d_TextureSet)
public:
  //! Callback definition.
  typedef void (*CallbackOnUpdate_t)(void* theUserPtr);

public:
  //! Empty constructor.
  Standard_EXPORT Graphic3d_MediaTextureSet();

  //! Setup callback to be called on queue progress (e.g. when new frame should be displayed).
  Standard_EXPORT void SetCallback(CallbackOnUpdate_t theCallbackFunction,
                                   void*              theCallbackUserPtr);

  //! Call callback.
  Standard_EXPORT void Notify();

  //! Return input media.
  const TCollection_AsciiString& Input() const { return myInput; }

  //! Open specified file.
  //! Passing an empty path would close current input.
  Standard_EXPORT void OpenInput(const TCollection_AsciiString& thePath,
                                 Standard_Boolean               theToWait);

  //! Return player context; it can be NULL until first OpenInput().
  const Handle(Media_PlayerContext)& PlayerContext() const { return myPlayerCtx; }

  //! Swap front/back frames.
  Standard_EXPORT Standard_Boolean SwapFrames();

  //! Return front frame dimensions.
  Graphic3d_Vec2i FrameSize() const { return myFrameSize; }

  //! Return shader program for displaying texture set.
  Handle(Graphic3d_ShaderProgram) ShaderProgram() const
  {
    if (myIsPlanarYUV)
    {
      return myIsFullRangeYUV ? myShaderYUVJ : myShaderYUV;
    }
    return Handle(Graphic3d_ShaderProgram)();
  }

  //! Return TRUE if texture set defined 3 YUV planes.
  Standard_Boolean IsPlanarYUV() const { return myIsPlanarYUV; }

  //! Return TRUE if YUV range is full.
  Standard_Boolean IsFullRangeYUV() const { return myIsFullRangeYUV; }

  //! Return duration in seconds.
  double Duration() const { return myDuration; }

  //! Return playback progress in seconds.
  double Progress() const { return myProgress; }

  //! @name Media_IFrameQueue interface
private:
  //! Lock the frame for decoding into.
  Standard_EXPORT virtual Handle(Media_Frame) LockFrame() Standard_OVERRIDE;

  //! Release the frame to present decoding results.
  Standard_EXPORT virtual void ReleaseFrame(const Handle(Media_Frame)& theFrame) Standard_OVERRIDE;

protected:
  Handle(Media_PlayerContext)     myPlayerCtx;        //!< player context
  Handle(Media_Frame)             myFramePair[2];     //!< front/back frames pair
  Handle(Graphic3d_ShaderProgram) myShaderYUV;        //!< shader program for YUV  texture set
  Handle(Graphic3d_ShaderProgram) myShaderYUVJ;       //!< shader program for YUVJ texture set
  Handle(Standard_HMutex)         myMutex;            //!< mutex for accessing frames
  TCollection_AsciiString         myInput;            //!< input media
  CallbackOnUpdate_t              myCallbackFunction; //!< callback function
  void*                           myCallbackUserPtr;  //!< callback data
  Graphic3d_Vec2i                 myFrameSize;        //!< front frame size
  Standard_Real                   myProgress;         //!< playback progress in seconds
  Standard_Real                   myDuration;         //!< stream duration
  Standard_Integer                myFront;            //!< index of front texture
  Standard_Boolean                myToPresentFrame;   //!< flag
  // clang-format off
  Standard_Boolean                myIsPlanarYUV;       //!< front frame contains planar YUV data or native texture format
  Standard_Boolean                myIsFullRangeYUV;    //!< front frame defines full-range or reduced-range YUV
  // clang-format on
};

#endif // _Graphic3d_MediaTextureSet_HeaderFile
