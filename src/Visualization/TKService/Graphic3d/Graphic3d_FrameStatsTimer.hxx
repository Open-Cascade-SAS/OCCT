// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _Graphic3d_FrameStatsTimer_HeaderFile
#define _Graphic3d_FrameStatsTimer_HeaderFile

//! Timers for collecting frame performance statistics.
enum Graphic3d_FrameStatsTimer
{
  Graphic3d_FrameStatsTimer_ElapsedFrame,
  Graphic3d_FrameStatsTimer_CpuFrame,
  Graphic3d_FrameStatsTimer_CpuCulling,
  Graphic3d_FrameStatsTimer_CpuPicking,
  Graphic3d_FrameStatsTimer_CpuDynamics,
};

enum
{
  Graphic3d_FrameStatsTimer_NB = Graphic3d_FrameStatsTimer_CpuDynamics + 1
};

#endif // _Graphic3d_FrameStatsTimer_HeaderFile
