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

#include <Image_VideoRecorder.hxx>
#include "../Media/Media_FFmpegCompatibility.pxx"

#include <Message.hxx>
#include <Message_Messenger.hxx>

#ifdef HAVE_FFMPEG
  // Suppress deprecation warnings - it is difficult to provide compatibility with old and new API
  // at once since new APIs are introduced too often. Should be disabled from time to time to clean
  // up usage of old APIs.
  #if (defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
_Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
  #else
Standard_DISABLE_DEPRECATION_WARNINGS
  #endif

  // Undefine macro that clashes with name used by field of Image_VideoParams;
  // this macro is defined in headers of older versions of libavutil
  // (see definition of macro FF_API_PIX_FMT in version.h)
  #ifdef PixelFormat
    #undef PixelFormat
  #endif

#endif

IMPLEMENT_STANDARD_RTTIEXT(Image_VideoRecorder, Standard_Transient)

//=================================================================================================

Image_VideoRecorder::Image_VideoRecorder()
    : myAVContext(NULL),
      myVideoStream(NULL),
      myVideoCodec(NULL),
      myCodecCtx(NULL),
      myFrame(NULL),
      myScaleCtx(NULL),
      myFrameCount(0)
{
  myFrameRate.num = 1;
  myFrameRate.den = 30;

#ifdef HAVE_FFMPEG
  // initialize libavcodec, and register all codecs and formats, should be done once
  ffmpeg_register_all();
#endif
}

//=================================================================================================

Image_VideoRecorder::~Image_VideoRecorder()
{
  Close();
}

//=================================================================================================

TCollection_AsciiString Image_VideoRecorder::formatAvError(const int theError) const
{
#ifdef HAVE_FFMPEG
  char anErrBuf[AV_ERROR_MAX_STRING_SIZE] = {};
  av_strerror(theError, anErrBuf, AV_ERROR_MAX_STRING_SIZE);
  return anErrBuf;
#else
  return TCollection_AsciiString(theError);
#endif
}

//=================================================================================================

AVCodecContext* Image_VideoRecorder::getCodecContext() const
{
#ifdef HAVE_FFMPEG
  #if FFMPEG_HAVE_AVCODEC_PARAMETERS
    return myCodecCtx;
  #else
    return myVideoStream != NULL ? myVideoStream->codec : NULL;
  #endif
#else
  return NULL;
#endif
}

//=================================================================================================

void Image_VideoRecorder::Close()
{
#ifdef HAVE_FFMPEG
  if (myScaleCtx != NULL)
  {
    sws_freeContext(myScaleCtx);
    myScaleCtx = NULL;
  }

  if (myAVContext == NULL)
  {
    myFrameCount = 0;
    return;
  }

  // Write the trailer, if any. The trailer must be written before you close the CodecContexts open
  // when you wrote the header; otherwise av_write_trailer() may try to use memory that was freed on
  // av_codec_close().
  if (myFrameCount != 0)
  {
    av_write_trailer(myAVContext);
    myFrameCount = 0;
  }

  // close each codec
  if (myVideoStream != NULL)
  {
#if FFMPEG_HAVE_AVCODEC_PARAMETERS
    if (myCodecCtx != NULL)
    {
      avcodec_free_context(&myCodecCtx);
      myCodecCtx = NULL;
    }
#else
    avcodec_close(myVideoStream->codec);
#endif
    myVideoStream = NULL;
  }
  if (myFrame != NULL)
  {
    av_free(myFrame->data[0]);
    av_frame_free(&myFrame);
    myFrame = NULL;
  }

  if (!(myAVContext->oformat->flags & AVFMT_NOFILE))
  {
    // close the output file
    avio_close(myAVContext->pb);
  }

  // free the stream
  avformat_free_context(myAVContext);
  myAVContext = NULL;
#endif
}

//=================================================================================================

Standard_Boolean Image_VideoRecorder::Open(const char*              theFileName,
                                           const Image_VideoParams& theParams)
{
#ifdef HAVE_FFMPEG
  Close();
  if (theParams.Width <= 0 || theParams.Height <= 0)
  {
    return Standard_False;
  }

  // allocate the output media context
  avformat_alloc_output_context2(&myAVContext,
                                 NULL,
                                 theParams.Format.IsEmpty() ? NULL : theParams.Format.ToCString(),
                                 theFileName);
  if (myAVContext == NULL)
  {
    ::Message::SendFail(
      "ViewerTest_VideoRecorder, could not deduce output format from file extension");
    return Standard_False;
  }

  // add the audio stream using the default format codecs and initialize the codecs
  if (!addVideoStream(theParams, myAVContext->oformat->video_codec))
  {
    Close();
    return Standard_False;
  }

  // open video codec and allocate the necessary encode buffers
  if (!openVideoCodec(theParams))
  {
    Close();
    return Standard_False;
  }

  #ifdef OCCT_DEBUG
  av_dump_format(myAVContext, 0, theFileName, 1);
  #endif

  // open the output file, if needed
  if ((myAVContext->oformat->flags & AVFMT_NOFILE) == 0)
  {
    const int aResAv = avio_open(&myAVContext->pb, theFileName, AVIO_FLAG_WRITE);
    if (aResAv < 0)
    {
      ::Message::SendFail(TCollection_AsciiString("Error: could not open '") + theFileName + "', "
                          + formatAvError(aResAv));
      Close();
      return Standard_False;
    }
  }

  // write the stream header, if any
  const int aResAv = avformat_write_header(myAVContext, NULL);
  if (aResAv < 0)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: can not open output file '") + theFileName
                        + "', " + formatAvError(aResAv));
    Close();
    return Standard_False;
  }
#else
  (void)theFileName;
  (void)theParams;
#endif
  return Standard_True;
}

//=================================================================================================

Standard_Boolean Image_VideoRecorder::addVideoStream(const Image_VideoParams& theParams,
                                                     const Standard_Integer   theDefCodecId)
{
  myFrameRate.num = theParams.FpsNum;
  myFrameRate.den = theParams.FpsDen;

#ifdef HAVE_FFMPEG
  // find the encoder
  TCollection_AsciiString aCodecName;
  if (!theParams.VideoCodec.IsEmpty())
  {
    myVideoCodec = ffmpeg_find_encoder_by_name(theParams.VideoCodec.ToCString());
    aCodecName   = theParams.VideoCodec;
  }
  else
  {
    myVideoCodec = ffmpeg_find_encoder((AVCodecID)theDefCodecId);
    aCodecName   = avcodec_get_name((AVCodecID)theDefCodecId);
  }
  if (myVideoCodec == NULL)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: can not find encoder for ") + aCodecName);
    return Standard_False;
  }

  const AVCodecID aCodecId = myVideoCodec->id;
  myVideoStream            = avformat_new_stream(myAVContext, myVideoCodec);
  if (myVideoStream == NULL)
  {
    ::Message::SendFail("Error: can not allocate stream");
    return Standard_False;
  }
  myVideoStream->id = myAVContext->nb_streams - 1;

#if FFMPEG_HAVE_AVCODEC_PARAMETERS
  // For FFmpeg 5.0+, allocate and use separate codec context
  myCodecCtx = avcodec_alloc_context3(myVideoCodec);
  if (myCodecCtx == NULL)
  {
    ::Message::SendFail("Error: can not allocate codec context");
    return Standard_False;
  }
  AVCodecContext* aCodecCtx = myCodecCtx;
#else
  // For FFmpeg 4.x, use stream's codec context
  AVCodecContext* aCodecCtx = myVideoStream->codec;
#endif

  aCodecCtx->codec_id       = aCodecId;
  // resolution must be a multiple of two
  aCodecCtx->width  = theParams.Width;
  aCodecCtx->height = theParams.Height;
  // Timebase is the fundamental unit of time (in seconds) in terms of which frame timestamps are
  // represented. For fixed-fps content, timebase should be 1/framerate and timestamp increments
  // should be identical to 1.
  aCodecCtx->time_base.den = myFrameRate.num;
  aCodecCtx->time_base.num = myFrameRate.den;
  aCodecCtx->gop_size      = 12; // emit one intra frame every twelve frames at most

  // some formats want stream headers to be separate
  if (myAVContext->oformat->flags & AVFMT_GLOBALHEADER)
  {
    aCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }

#if FFMPEG_HAVE_AVCODEC_PARAMETERS
  // Copy codec context parameters to stream
  if (avcodec_parameters_from_context(myVideoStream->codecpar, aCodecCtx) < 0)
  {
    ::Message::SendFail("Error: can not copy codec parameters to stream");
    return Standard_False;
  }
#endif
  return Standard_True;
#else
  (void)theParams;
  (void)theDefCodecId;
  return Standard_False;
#endif
}

//=================================================================================================

Standard_Boolean Image_VideoRecorder::openVideoCodec(const Image_VideoParams& theParams)
{
#ifdef HAVE_FFMPEG
  AVDictionary*   anOptions = NULL;
  AVCodecContext* aCodecCtx = getCodecContext();

  if (aCodecCtx == NULL)
  {
    ::Message::SendFail("Error: codec context is not available");
    return Standard_False;
  }

  // setup default values
  aCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
  // av_dict_set (&anOptions, "threads", "auto", 0);
  if (myVideoCodec == ffmpeg_find_encoder_by_name("mpeg2video"))
  {
    // just for testing, we also add B frames
    aCodecCtx->max_b_frames = 2;
    aCodecCtx->bit_rate     = 6000000;
  }
  else if (myVideoCodec == ffmpeg_find_encoder_by_name("mpeg4"))
  {
    //
  }
  else if (myVideoCodec == ffmpeg_find_encoder_by_name("mjpeg"))
  {
    aCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    aCodecCtx->qmin = aCodecCtx->qmax = 5;
  }
  else if (myVideoCodec == ffmpeg_find_encoder_by_name("huffyuv"))
  {
    aCodecCtx->pix_fmt = AV_PIX_FMT_RGB24;
  }
  else if (myVideoCodec == ffmpeg_find_encoder_by_name("png"))
  {
    aCodecCtx->pix_fmt           = AV_PIX_FMT_RGB24;
    aCodecCtx->compression_level = 9; // 0..9
  }
  else if (myVideoCodec == ffmpeg_find_encoder_by_name("h264")
           || myVideoCodec == ffmpeg_find_encoder_by_name("libx264"))
  {
    // use CRF (Constant Rate Factor) as best single-pass compressing method
    // clang-format off
    av_dict_set (&anOptions, "crf",     "20",        0); // quality 18-28, 23 is default (normal), 18 is almost lossless
    av_dict_set (&anOptions, "preset",  "slow",      0); // good compression (see also "veryslow", "ultrafast")
    // clang-format on

    // live-capturing
    // av_dict_set (&anOptions, "qp",     "0",         0); // instead of crf
    // av_dict_set (&anOptions, "preset", "ultrafast", 0);

    // compatibility with devices
    // av_dict_set (&anOptions, "profile", "baseline",  0);
    // av_dict_set (&anOptions, "level",   "3.0",       0);
  }
  else if (myVideoCodec == ffmpeg_find_encoder_by_name("vp8")
           || myVideoCodec == ffmpeg_find_encoder_by_name("vp9"))
  {
    av_dict_set(&anOptions, "crf", "20", 0); // quality 4-63, 10 is normal
  }

  // override defaults with specified options
  if (!theParams.PixelFormat.IsEmpty())
  {
    const AVPixelFormat aPixFormat = av_get_pix_fmt(theParams.PixelFormat.ToCString());
    if (aPixFormat == AV_PIX_FMT_NONE)
    {
      ::Message::SendFail(
        TCollection_AsciiString("Error: unknown pixel format has been specified '")
        + theParams.PixelFormat + "'");
      return Standard_False;
    }

    aCodecCtx->pix_fmt = aPixFormat;
    for (Resource_DataMapOfAsciiStringAsciiString::Iterator aParamIter(theParams.VideoCodecParams);
         aParamIter.More();
         aParamIter.Next())
    {
      av_dict_set(&anOptions, aParamIter.Key().ToCString(), aParamIter.Value().ToCString(), 0);
    }
  }

  // open codec
  int aResAv = avcodec_open2(aCodecCtx, myVideoCodec, &anOptions);
  if (anOptions != NULL)
  {
    av_dict_free(&anOptions);
  }
  if (aResAv < 0)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: can not open video codec, ")
                        + formatAvError(aResAv));
    return Standard_False;
  }

  // allocate and init a re-usable frame
  myFrame = av_frame_alloc();
  if (myFrame == NULL)
  {
    ::Message::SendFail("Error: can not allocate video frame");
    return Standard_False;
  }

  // allocate the encoded raw picture
  aResAv = av_image_alloc(myFrame->data,
                          myFrame->linesize,
                          aCodecCtx->width,
                          aCodecCtx->height,
                          aCodecCtx->pix_fmt,
                          1);
  if (aResAv < 0)
  {
    memset(myFrame->data, 0, sizeof(myFrame->data));
    memset(myFrame->linesize, 0, sizeof(myFrame->linesize));
    ::Message::SendFail(TCollection_AsciiString("Error: can not allocate picture ")
                        + aCodecCtx->width + "x" + aCodecCtx->height + ", "
                        + formatAvError(aResAv));
    return Standard_False;
  }
  // copy data and linesize picture pointers to frame
  myFrame->format = aCodecCtx->pix_fmt;
  myFrame->width  = aCodecCtx->width;
  myFrame->height = aCodecCtx->height;

  const Standard_Size aStride = aCodecCtx->width + 16 - (aCodecCtx->width % 16);
  if (!myImgSrcRgba.InitZero(Image_Format_RGBA, aCodecCtx->width, aCodecCtx->height, aStride))
  {
    ::Message::SendFail(TCollection_AsciiString("Error: can not allocate RGBA32 picture ")
                        + aCodecCtx->width + "x" + aCodecCtx->height);
    return Standard_False;
  }

  myScaleCtx = sws_getContext(aCodecCtx->width,
                              aCodecCtx->height,
                              AV_PIX_FMT_RGBA,
                              aCodecCtx->width,
                              aCodecCtx->height,
                              aCodecCtx->pix_fmt,
                              SWS_BICUBIC,
                              NULL,
                              NULL,
                              NULL);
  if (myScaleCtx == NULL)
  {
    ::Message::SendFail("Error: can not initialize the conversion context");
    return Standard_False;
  }
  return Standard_True;
#else
  (void)theParams;
  return Standard_False;
#endif
}

//=================================================================================================

Standard_Boolean Image_VideoRecorder::writeVideoFrame(const Standard_Boolean theToFlush)
{
#ifdef HAVE_FFMPEG
  if (myImgSrcRgba.Format() != Image_Format_RGBA)
  {
    throw Standard_ProgramError("Image_VideoRecorder, unsupported image format");
  }

  int             aResAv    = 0;
  AVCodecContext* aCodecCtx = getCodecContext();
  if (!theToFlush)
  {
    uint8_t* aSrcData[4]     = {(uint8_t*)myImgSrcRgba.ChangeData(), NULL, NULL, NULL};
    int      aSrcLinesize[4] = {(int)myImgSrcRgba.SizeRowBytes(), 0, 0, 0};
    sws_scale(myScaleCtx,
              aSrcData,
              aSrcLinesize,
              0,
              aCodecCtx->height,
              myFrame->data,
              myFrame->linesize);
  }

#if FFMPEG_HAVE_NEW_DECODE_API
  // New API: use avcodec_send_frame/avcodec_receive_packet
  if (!theToFlush)
  {
    myFrame->pts = myFrameCount;
    aResAv = avcodec_send_frame(aCodecCtx, myFrame);
  }
  else
  {
    aResAv = avcodec_send_frame(aCodecCtx, NULL); // flush
  }
  
  if (aResAv < 0)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: can not send frame to encoder, ")
                        + formatAvError(aResAv));
    return Standard_False;
  }

  while (aResAv >= 0)
  {
    AVPacket* aPacket = av_packet_alloc();
    aResAv = avcodec_receive_packet(aCodecCtx, aPacket);
    if (aResAv == AVERROR(EAGAIN) || aResAv == AVERROR_EOF)
    {
      av_packet_free(&aPacket);
      break; // need more input or end of stream
    }
    else if (aResAv < 0)
    {
      av_packet_free(&aPacket);
      ::Message::SendFail(TCollection_AsciiString("Error: can not encode video frame, ")
                          + formatAvError(aResAv));
      return Standard_False;
    }

    // rescale output packet timestamp values from codec to stream timebase
    aPacket->pts          = av_rescale_q_rnd(aPacket->pts,
                                   aCodecCtx->time_base,
                                   myVideoStream->time_base,
                                   AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    aPacket->dts          = av_rescale_q_rnd(aPacket->dts,
                                   aCodecCtx->time_base,
                                   myVideoStream->time_base,
                                   AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    aPacket->duration     = av_rescale_q(aPacket->duration, aCodecCtx->time_base, myVideoStream->time_base);
    aPacket->stream_index = myVideoStream->index;

    // write the compressed frame to the media file
    aResAv = av_interleaved_write_frame(myAVContext, aPacket);
    av_packet_free(&aPacket);
    
    if (aResAv < 0)
    {
      ::Message::SendFail(TCollection_AsciiString("Error: can not write video frame, ")
                          + formatAvError(aResAv));
      return Standard_False;
    }
  }
#else
  // Old API: use avcodec_encode_video2
  AVPacket aPacket;
  memset(&aPacket, 0, sizeof(aPacket));
  av_init_packet(&aPacket);
  {
    // encode the image
    if (!theToFlush)
    {
      myFrame->pts = myFrameCount;
    }
    int isGotPacket = 0;
    aResAv = avcodec_encode_video2(aCodecCtx, &aPacket, theToFlush ? NULL : myFrame, &isGotPacket);
    if (aResAv < 0)
    {
      ::Message::SendFail(TCollection_AsciiString("Error: can not encode video frame, ")
                          + formatAvError(aResAv));
      return Standard_False;
    }

    // if size is zero, it means the image was buffered
    if (isGotPacket)
    {
      const AVRational& aTimeBase = aCodecCtx->time_base;

      // rescale output packet timestamp values from codec to stream timebase
      aPacket.pts          = av_rescale_q_rnd(aPacket.pts,
                                     aTimeBase,
                                     myVideoStream->time_base,
                                     AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
      aPacket.dts          = av_rescale_q_rnd(aPacket.dts,
                                     aTimeBase,
                                     myVideoStream->time_base,
                                     AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
      aPacket.duration     = av_rescale_q(aPacket.duration, aTimeBase, myVideoStream->time_base);
      aPacket.stream_index = myVideoStream->index;

      // write the compressed frame to the media file
      aResAv = av_interleaved_write_frame(myAVContext, &aPacket);
    }
    else
    {
      aResAv = 0;
    }
  }

  if (aResAv < 0)
  {
    ::Message::SendFail(TCollection_AsciiString("Error: can not write video frame, ")
                        + formatAvError(aResAv));
    return Standard_False;
  }
#endif

  ++myFrameCount;
  return Standard_True;
#else
  (void)theToFlush;
  return Standard_False;
#endif
}
