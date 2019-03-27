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

#ifndef _Media_Frame_HeaderFile
#define _Media_Frame_HeaderFile

#include <Graphic3d_Vec2.hxx>
#include <Image_PixMap.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

struct AVFrame;

//! AVFrame wrapper - the frame (decoded image/audio sample data) holder.
class Media_Frame : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Media_Frame, Standard_Transient)
public:

  //! Convert pixel format from FFmpeg (AVPixelFormat) to OCCT.
  Standard_EXPORT static Image_Format FormatFFmpeg2Occt (int theFormat);

  //! Convert pixel format from OCCT to FFmpeg (AVPixelFormat).
  //! Returns -1 (AV_PIX_FMT_NONE) if undefined.
  Standard_EXPORT static int FormatOcct2FFmpeg (Image_Format theFormat);

  //! Swap AVFrame* within two frames.
  Standard_EXPORT static void Swap (const Handle(Media_Frame)& theFrame1,
                                    const Handle(Media_Frame)& theFrame2);

public:

  //! Empty constructor
  Standard_EXPORT Media_Frame();

  //! Destructor
  Standard_EXPORT virtual ~Media_Frame();

  //! Return true if frame does not contain any data.
  Standard_EXPORT bool IsEmpty() const;

  //! av_frame_unref() wrapper.
  Standard_EXPORT void Unref();

  //! Return image dimensions.
  Graphic3d_Vec2i Size() const { return Graphic3d_Vec2i (SizeX(), SizeY()); }

  //! Return image width.
  Standard_EXPORT int SizeX() const;

  //! Return image height.
  Standard_EXPORT int SizeY() const;

  //! Return pixel format (AVPixelFormat).
  Standard_EXPORT int Format() const;

  //! Return TRUE if YUV range is full.
  Standard_EXPORT bool IsFullRangeYUV() const;

  //! Access data plane for specified Id.
  Standard_EXPORT uint8_t* Plane (int thePlaneId) const;

  //! @return linesize in bytes for specified data plane
  Standard_EXPORT int LineSize (int thePlaneId) const;

  //! @return frame timestamp estimated using various heuristics, in stream time base
  Standard_EXPORT int64_t BestEffortTimestamp() const;

  //! Return frame.
  const AVFrame* Frame() const { return myFrame; }

  //! Return frame.
  AVFrame* ChangeFrame() { return myFrame; }

  //! Return presentation timestamp (PTS).
  double Pts() const { return myFramePts; }

  //! Set presentation timestamp (PTS).
  void SetPts (double thePts) { myFramePts = thePts; }

  //! Return PAR.
  float PixelAspectRatio() const { return myPixelRatio; }

  //! Set PAR.
  void SetPixelAspectRatio (float theRatio) { myPixelRatio = theRatio; }

  //! Return locked state.
  bool IsLocked() const { return myIsLocked; }

  //! Lock/free frame for edition.
  void SetLocked (bool theToLock) { myIsLocked = theToLock; }

public:

  //! Wrap allocated image pixmap.
  Standard_EXPORT bool InitWrapper (const Handle(Image_PixMap)& thePixMap);

protected:

  AVFrame* myFrame;      //!< frame
  double   myFramePts;   //!< presentation timestamp
  float    myPixelRatio; //!< pixel aspect ratio
  bool     myIsLocked;   //!< locked state

};

#endif // _Media_Frame_HeaderFile
