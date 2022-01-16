// Created on: 2015-06-05
// Created by: Kirill Gavrilov
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Graphic3d_StereoMode_HeaderFile
#define _Graphic3d_StereoMode_HeaderFile

//! This enumeration defines the list of stereoscopic output modes.
enum Graphic3d_StereoMode
{
  Graphic3d_StereoMode_QuadBuffer,       //!< OpenGL QuadBuffer
  Graphic3d_StereoMode_Anaglyph,         //!< Anaglyph glasses, the type should be specified in addition
  Graphic3d_StereoMode_RowInterlaced,    //!< Row-interlaced stereo
  Graphic3d_StereoMode_ColumnInterlaced, //!< Column-interlaced stereo
  Graphic3d_StereoMode_ChessBoard,       //!< chess-board stereo for DLP TVs
  Graphic3d_StereoMode_SideBySide,       //!< horizontal pair
  Graphic3d_StereoMode_OverUnder,        //!< vertical   pair
  Graphic3d_StereoMode_SoftPageFlip,     //!< software PageFlip for shutter glasses, should NOT be used!
  Graphic3d_StereoMode_OpenVR,           //!< OpenVR (HMD)
};
enum { Graphic3d_StereoMode_NB = Graphic3d_StereoMode_OpenVR + 1 };

#endif // _Graphic3d_StereoMode_HeaderFile
