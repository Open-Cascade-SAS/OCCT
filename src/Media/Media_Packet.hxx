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

#ifndef _Media_Packet_HeaderFile
#define _Media_Packet_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

struct AVPacket;

//! AVPacket wrapper - the packet (data chunk for decoding/encoding) holder.
class Media_Packet : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Media_Packet, Standard_Transient)
public:

  //! Empty constructor
  Standard_EXPORT Media_Packet();

  //! Destructor.
  Standard_EXPORT virtual ~Media_Packet();

  //! av_packet_unref() wrapper.
  Standard_EXPORT void Unref();

  //! Return packet.
  const AVPacket* Packet() const { return myPacket; }

  //! Return packet.
  AVPacket* ChangePacket() { return myPacket; }

  //! Return data.
  Standard_EXPORT const uint8_t* Data() const;

  //! Return data.
  Standard_EXPORT uint8_t* ChangeData();

  //! Return data size.
  Standard_EXPORT int Size() const;

  //! Return presentation timestamp (PTS).
  Standard_EXPORT int64_t Pts() const;

  //! Return decoding timestamp (DTS).
  Standard_EXPORT int64_t Dts() const;

  //! Return Duration.
  Standard_EXPORT int64_t Duration() const;

  //! Return Duration in seconds.
  double DurationSeconds() const { return myDurationSec; }

  //! Set Duration in seconds.
  void SetDurationSeconds (double theDurationSec) { myDurationSec = theDurationSec; }

  //! Return stream index.
  Standard_EXPORT int StreamIndex() const;

  //! Return TRUE for a key frame.
  Standard_EXPORT bool IsKeyFrame() const;

  //! Mark as key frame.
  Standard_EXPORT void SetKeyFrame();

protected:

  AVPacket* myPacket;      //!< packet
  double    myDurationSec; //!< packet duration in seconds

};

#endif // _Media_Packet_HeaderFile
