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

#ifndef _Media_IFrameQueue_HeaderFile
#define _Media_IFrameQueue_HeaderFile

#include <Media_Frame.hxx>

//! Interface defining frame queuing.
class Media_IFrameQueue
{
public:
  //! Lock the frame, e.g. take ownership on a single (not currently displayed) frame from the queue
  //! to perform decoding into.
  virtual Handle(Media_Frame) LockFrame() = 0;

  //! Release previously locked frame, e.g. it can be displayed on the screen.
  virtual void ReleaseFrame(const Handle(Media_Frame)& theFrame) = 0;
};

#endif // _Media_IFrameQueue_HeaderFile
