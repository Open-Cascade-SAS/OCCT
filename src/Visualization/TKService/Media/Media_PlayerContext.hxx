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

#ifndef _Media_PlayerContext_HeaderFile
#define _Media_PlayerContext_HeaderFile

#include <Media_IFrameQueue.hxx>
#include <Media_Timer.hxx>
#include <OSD_Thread.hxx>
#include <Standard_Condition.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

class Media_BufferPool;
class Media_CodecContext;
class Media_FormatContext;
class Media_Scaler;

//! Player context.
class Media_PlayerContext : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Media_PlayerContext, Standard_Transient)
public:
  //! Dump first video frame.
  //! @param[in] theSrcVideo  path to the video
  //! @param[out] theMediaInfo  video description
  Standard_EXPORT static Handle(Media_Frame) DumpFirstFrame(
    const TCollection_AsciiString& theSrcVideo,
    TCollection_AsciiString&       theMediaInfo);

  //! Dump first video frame.
  //! @param[in] theSrcVideo  path to the video
  //! @param[in] theOutImage  path to make a screenshot
  //! @param[out] theMediaInfo  video description
  //! @param[in] theMaxSize  when positive - downscales image to specified size
  Standard_EXPORT static bool DumpFirstFrame(const TCollection_AsciiString& theSrcVideo,
                                             const TCollection_AsciiString& theOutImage,
                                             TCollection_AsciiString&       theMediaInfo,
                                             int                            theMaxSize = 0);

public:
  //! Main constructor.
  //! Note that Frame Queue is stored as pointer,
  //! and it is expected that this context is stored as a class field of Frame Queue.
  Standard_EXPORT Media_PlayerContext(Media_IFrameQueue* theFrameQueue);

  //! Destructor.
  Standard_EXPORT virtual ~Media_PlayerContext();

public:
  //! Set new input for playback.
  Standard_EXPORT void SetInput(const TCollection_AsciiString& theInputPath,
                                Standard_Boolean               theToWait);

  //! Return playback state.
  Standard_EXPORT void PlaybackState(Standard_Boolean& theIsPaused,
                                     Standard_Real&    theProgress,
                                     Standard_Real&    theDuration);

  //! Pause/Pause playback depending on the current state.
  Standard_EXPORT void PlayPause(Standard_Boolean& theIsPaused,
                                 Standard_Real&    theProgress,
                                 Standard_Real&    theDuration);

  //! Seek to specified position.
  Standard_EXPORT void Seek(Standard_Real thePosSec);

  //! Pause playback.
  void Pause() { pushPlayEvent(Media_PlayerEvent_PAUSE); }

  //! Resume playback.
  void Resume() { pushPlayEvent(Media_PlayerEvent_RESUME); }

  //! Return TRUE if queue requires RGB pixel format or can handle also YUV pixel format; TRUE by
  //! default.
  bool ToForceRgb() const { return myToForceRgb; }

  //! Set if queue requires RGB pixel format or can handle also YUV pixel format.
  void SetForceRgb(bool theToForce) { myToForceRgb = theToForce; }

private:
  //! Internal enumeration for events.
  enum Media_PlayerEvent
  {
    Media_PlayerEvent_NONE = 0,
    Media_PlayerEvent_PAUSE,
    Media_PlayerEvent_RESUME,
    Media_PlayerEvent_SEEK,
    Media_PlayerEvent_NEXT,
  };

private:
  //! Thread loop.
  Standard_EXPORT void doThreadLoop();

  //! Push new playback event.
  Standard_EXPORT void pushPlayEvent(Media_PlayerEvent thePlayEvent);

  //! Fetch new playback event.
  Standard_EXPORT bool popPlayEvent(Media_PlayerEvent&                 thePlayEvent,
                                    const Handle(Media_FormatContext)& theFormatCtx,
                                    const Handle(Media_CodecContext)&  theVideoCtx,
                                    const Handle(Media_Frame)&         theFrame);

  //! Decode new frame.
  bool receiveFrame(const Handle(Media_Frame)&        theFrame,
                    const Handle(Media_CodecContext)& theVideoCtx);

  //! Thread creation callback.
  static Standard_Address doThreadWrapper(Standard_Address theData)
  {
    Media_PlayerContext* aThis = (Media_PlayerContext*)theData;
    aThis->doThreadLoop();
    return 0;
  }

private:
  Media_IFrameQueue* myFrameQueue; //!< frame queue
  OSD_Thread         myThread;     //!< working thread
  Standard_Mutex     myMutex;      //!< mutex for events
                                   // clang-format off
  Standard_Condition          myWakeEvent;      //!< event to wake up working thread and proceed new playback event
  Standard_Condition          myNextEvent;      //!< event to check if working thread processed next file event (e.g. released file handles of previous input)
  Media_Timer                 myTimer;          //!< playback timer       
  Standard_Real               myDuration;       //!< playback duration

  Handle(Media_BufferPool)    myBufferPools[4]; //!< per-plane pools
  Handle(Media_Frame)         myFrameTmp;       //!< temporary object holding decoded frame
  Handle(Media_Scaler)        myScaler;         //!< pixel format conversion tool
  bool                        myToForceRgb;     //!< flag indicating if queue requires RGB pixel format or can handle also YUV pixel format
                                   // clang-format on

  volatile bool              myToShutDown; //!< flag to terminate working thread
  TCollection_AsciiString    myInputPath;  //!< new input to open
  volatile Standard_Real     mySeekTo;     //!< new seeking position
  volatile Media_PlayerEvent myPlayEvent;  //!< playback event
};

#endif // _Media_PlayerContext_HeaderFile
