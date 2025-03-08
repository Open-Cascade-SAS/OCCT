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

#ifndef _Media_FormatContext_HeaderFile
#define _Media_FormatContext_HeaderFile

#include <Media_Packet.hxx>

#include <TCollection_AsciiString.hxx>

struct AVCodecContext;
struct AVFormatContext;
struct AVStream;
struct AVRational;

//! AVFormatContext wrapper - the media input/output stream holder.
class Media_FormatContext : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Media_FormatContext, Standard_Transient)
public:
  //! Returns string description for AVError code.
  Standard_EXPORT static TCollection_AsciiString FormatAVErrorDescription(int theErrCodeAV);

  //! Convert time units into seconds for context.
  //! @param theTimeUnits value to convert
  //! @return converted time units in seconds
  Standard_EXPORT static double FormatUnitsToSeconds(int64_t theTimeUnits);

  //! Convert time units into seconds. Returns zero for invalid value.
  //! @param theTimeBase  the timebase
  //! @param theTimeUnits value to convert
  //! @return converted time units in seconds
  Standard_EXPORT static double UnitsToSeconds(const AVRational& theTimeBase, int64_t theTimeUnits);

  //! Convert time units into seconds using stream base.
  //! @param theStream    the stream;
  //! @param theTimeUnits value to convert;
  //! @return converted time units in seconds.
  Standard_EXPORT static double StreamUnitsToSeconds(const AVStream& theStream,
                                                     int64_t         theTimeUnits);

  //! Convert seconds into time units for context.
  //! @param theTimeSeconds value to convert
  //! @return time units
  Standard_EXPORT static int64_t SecondsToUnits(double theTimeSeconds);

  //! Convert seconds into time units.
  //! @param theTimeBase    the timebase
  //! @param theTimeSeconds value to convert
  //! @return time units
  Standard_EXPORT static int64_t SecondsToUnits(const AVRational& theTimeBase,
                                                double            theTimeSeconds);

  //! Convert seconds into time units for stream.
  //! @param theStream      the stream
  //! @param theTimeSeconds value to convert
  //! @return time units
  Standard_EXPORT static int64_t StreamSecondsToUnits(const AVStream& theStream,
                                                      double          theTimeSeconds);

  //! Time formatter.
  Standard_EXPORT static TCollection_AsciiString FormatTime(double theSeconds);

  //! Time progress / duration formatter.
  Standard_EXPORT static TCollection_AsciiString FormatTimeProgress(double theProgress,
                                                                    double theDuration);

public:
  //! Constructor.
  Standard_EXPORT Media_FormatContext();

  //! Destructor.
  Standard_EXPORT virtual ~Media_FormatContext();

  //! Return context.
  AVFormatContext* Context() const { return myFormatCtx; }

  //! Open input.
  Standard_EXPORT bool OpenInput(const TCollection_AsciiString& theInput);

  //! Close input.
  Standard_EXPORT void Close();

  //! Return amount of streams.
  Standard_EXPORT unsigned int NbSteams() const;

  //! Return stream.
  Standard_EXPORT const AVStream& Stream(unsigned int theIndex) const;

  //! Format stream info.
  Standard_EXPORT TCollection_AsciiString StreamInfo(unsigned int    theIndex,
                                                     AVCodecContext* theCodecCtx = NULL) const;

  //! Return PTS start base in seconds.
  double PtsStartBase() const { return myPtsStartBase; }

  //! Return duration in seconds.
  double Duration() const { return myDuration; }

  //! av_read_frame() wrapper.
  Standard_EXPORT bool ReadPacket(const Handle(Media_Packet)& thePacket);

  //! Seek stream to specified position.
  Standard_EXPORT bool SeekStream(unsigned int theStreamId, double theSeekPts, bool toSeekBack);

  //! Seek context to specified position.
  Standard_EXPORT bool Seek(double theSeekPts, bool toSeekBack);

protected:
  AVFormatContext* myFormatCtx;    //!< format context
  double           myPtsStartBase; //!< start time
  double           myDuration;     //!< duration
};

#endif // _Media_FormatContext_HeaderFile
