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

#ifndef _Media_CodecContext_HeaderFile
#define _Media_CodecContext_HeaderFile

#include <Media_Packet.hxx>

struct AVCodec;
struct AVCodecContext;
struct AVStream;
class Media_Frame;

//! AVCodecContext wrapper - the coder/decoder holder.
class Media_CodecContext : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Media_CodecContext, Standard_Transient)
public:
  //! Constructor.
  Standard_EXPORT Media_CodecContext();

  //! Destructor.
  Standard_EXPORT virtual ~Media_CodecContext();

  //! Return context.
  AVCodecContext* Context() const { return myCodecCtx; }

  //! Open codec specified within the stream.
  //! @param theStream stream to open
  //! @param thePtsStartBase PTS start in seconds
  //! @param theNbThreads amount of threads to use for AVMEDIA_TYPE_VIDEO stream;
  //!                     -1 means OSD_Parallel::NbLogicalProcessors(),
  //!                      0 means auto by FFmpeg itself
  //!                     >0 means specified number of threads (decoder should support
  //!                     multi-threading to take effect)
  Standard_EXPORT bool Init(const AVStream& theStream,
                            double          thePtsStartBase,
                            int             theNbThreads = -1);

  //! Open codec.
  //! @param theStream stream to open
  //! @param thePtsStartBase PTS start in seconds
  //! @param theNbThreads amount of threads to use for AVMEDIA_TYPE_VIDEO stream;
  //!                     -1 means OSD_Parallel::NbLogicalProcessors(),
  //!                      0 means auto by FFmpeg itself
  //!                     >0 means specified number of threads (decoder should support
  //!                     multi-threading to take effect)
  //! @param theCodecId codec (AVCodecID) to open
  Standard_EXPORT bool Init(const AVStream& theStream,
                            double          thePtsStartBase,
                            int             theNbThreads,
                            int             theCodecId);

  //! Close input.
  Standard_EXPORT void Close();

  //! @return source frame width
  Standard_EXPORT int SizeX() const;

  //! @return source frame height
  Standard_EXPORT int SizeY() const;

  //! Return stream index.
  int StreamIndex() const { return myStreamIndex; }

  //! avcodec_flush_buffers() wrapper.
  Standard_EXPORT void Flush();

  //! Return true if packet belongs to this stream.
  Standard_EXPORT bool CanProcessPacket(const Handle(Media_Packet)& thePacket) const;

  //! avcodec_send_packet() wrapper.
  Standard_EXPORT bool SendPacket(const Handle(Media_Packet)& thePacket);

  //! avcodec_receive_frame() wrapper.
  Standard_EXPORT bool ReceiveFrame(const Handle(Media_Frame)& theFrame);

protected:
  AVCodecContext* myCodecCtx;         //!< codec context
  AVCodec*        myCodec;            //!< opened codec
  double          myPtsStartBase;     //!< starting PTS in context
  double          myPtsStartStream;   //!< starting PTS in the stream
  double          myTimeBase;         //!< stream timebase
  int             myStreamIndex;      //!< stream index
  float           myPixelAspectRatio; //!< pixel aspect ratio
};

#endif // _Media_CodecContext_HeaderFile
