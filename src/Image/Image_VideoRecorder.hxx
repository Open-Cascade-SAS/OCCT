// Created on: 2016-04-01
// Created by: Anastasia BORISOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef Image_VideoRecorder_HeaderFile_
#define Image_VideoRecorder_HeaderFile_

#include <Image_PixMap.hxx>
#include <Resource_DataMapOfAsciiStringAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

// forward declarations
struct AVFormatContext;
struct AVStream;
struct AVCodec;
struct AVFrame;
struct SwsContext;

//! Auxiliary structure defining video parameters.
//! Please refer to FFmpeg documentation for defining text values.
struct Image_VideoParams
{
  TCollection_AsciiString Format;           //!< [optional]  video format (container), if empty - will be determined from the file name
  TCollection_AsciiString VideoCodec;       //!< [optional]  codec identifier, if empty - default codec from file format will be used
  TCollection_AsciiString PixelFormat;      //!< [optional]  pixel format, if empty - default codec pixel format will be used
  Standard_Integer        Width;            //!< [mandatory] video frame width
  Standard_Integer        Height;           //!< [mandatory] video frame height
  Standard_Integer        FpsNum;           //!< [mandatory] framerate numerator
  Standard_Integer        FpsDen;           //!< [mandatory] framerate denumerator
  Resource_DataMapOfAsciiStringAsciiString
                          VideoCodecParams; //!< map of advanced video codec parameters

  //! Empty constructor.
  Image_VideoParams() : Width (0), Height (0), FpsNum (0), FpsDen (1) {}

  //! Setup playback FPS.
  void SetFramerate (const Standard_Integer theNumerator,
                     const Standard_Integer theDenominator)
  {
    FpsNum = theNumerator;
    FpsDen = theDenominator;
  }

  //! Setup playback FPS.
  //! For fixed-fps content, timebase should be 1/framerate and timestamp increments should be identical to 1.
  void SetFramerate (const Standard_Integer theValue)
  {
    FpsNum = theValue;
    FpsDen = 1;
  }
};

//! Video recording tool based on FFmpeg framework.
class Image_VideoRecorder : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Image_VideoRecorder, Standard_Transient)
public:

  //! Empty constructor.
  Standard_EXPORT Image_VideoRecorder();

  //! Destructor.
  Standard_EXPORT virtual ~Image_VideoRecorder();

  //! Close the stream - stop recorder.
  Standard_EXPORT void Close();

  //! Open output stream - initialize recorder.
  //! @param theFileName [in] video filename
  //! @param theParams   [in] video parameters
  Standard_EXPORT Standard_Boolean Open (const char* theFileName,
                                         const Image_VideoParams& theParams);

  //! Access RGBA frame, should NOT be re-initialized outside.
  //! Note that image is expected to have upper-left origin.
  Image_PixMap& ChangeFrame() { return myImgSrcRgba; }

  //! Return current frame index.
  int64_t FrameCount() const { return myFrameCount; }

  //! Push new frame, should be called after Open().
  Standard_Boolean PushFrame()
  {
    return writeVideoFrame (Standard_False);
  }

protected:

  //! Wrapper for av_strerror().
  Standard_EXPORT TCollection_AsciiString formatAvError (const int theError) const;

  //! Append video stream.
  //! theParams     [in] video parameters
  //! theDefCodecId [in] identifier of codec managed by FFmpeg library (AVCodecID enum)
  Standard_EXPORT Standard_Boolean addVideoStream (const Image_VideoParams& theParams,
                                                   const Standard_Integer   theDefCodecId);

  //! Open video codec.
  Standard_EXPORT Standard_Boolean openVideoCodec (const Image_VideoParams& theParams);

  //! Write new video frame.
  Standard_EXPORT Standard_Boolean writeVideoFrame (const Standard_Boolean theToFlush);

protected:

  //! AVRational alias.
  struct VideoRational
  {
    int num; //!< numerator
    int den; //!< denominator
  };

protected:

  AVFormatContext* myAVContext;   //!< video context
  AVStream*        myVideoStream; //!< video stream
  AVCodec*         myVideoCodec;  //!< video codec
  AVFrame*         myFrame;       //!< frame to record
  SwsContext*      myScaleCtx;    //!< scale context for conversion from RGBA to YUV

  Image_PixMap     myImgSrcRgba;  //!< input RGBA image
  VideoRational    myFrameRate;   //!< video framerate
  int64_t          myFrameCount;  //!< current frame index

};

DEFINE_STANDARD_HANDLE(Image_VideoRecorder, Standard_Transient)

#endif // Image_VideoRecorder_HeaderFile_
