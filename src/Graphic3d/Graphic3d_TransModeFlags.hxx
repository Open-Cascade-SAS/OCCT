// Created on: 2004-10-29
// Created by: Alexander BORODIN
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef Graphic3d_TRANS_MODE_FLAGS_HXX
#define Graphic3d_TRANS_MODE_FLAGS_HXX

typedef Standard_Integer Graphic3d_TransModeFlags;

enum {
  Graphic3d_TMF_None               = 0x0000,
  Graphic3d_TMF_PanPers            = 0x0001,
  Graphic3d_TMF_ZoomPers           = 0x0002,
  Graphic3d_TMF_RotatePers         = 0x0008,
  Graphic3d_TMF_TriedronPers       = 0x0020,
  Graphic3d_TMF_2d                 = 0x0040,
  Graphic3d_TMF_2d_IsTopDown       = 0x0041,
  Graphic3d_TMF_FullPers           = Graphic3d_TMF_PanPers | Graphic3d_TMF_ZoomPers | Graphic3d_TMF_RotatePers
};


#endif
