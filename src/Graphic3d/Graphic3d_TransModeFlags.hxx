// File:	Graphic3d_TransModeFlags.hxx
// Created:	Fri Oct 29 2004
// Author:	Alexander BORODIN
// Copyright:	Open CASCADE 2004

#ifndef Graphic3d_TRANS_MODE_FLAGS_HXX
#define Graphic3d_TRANS_MODE_FLAGS_HXX

Standard_EXPORT typedef Standard_Integer Graphic3d_TransModeFlags;

enum {
  Graphic3d_TMF_None               = 0x0000,
  Graphic3d_TMF_PanPers            = 0x0001,
  Graphic3d_TMF_ZoomPers           = 0x0002,
  Graphic3d_TMF_RotatePers         = 0x0008,
  Graphic3d_TMF_TriedronPers       = 0x0020,
  Graphic3d_TMF_FullPers           = Graphic3d_TMF_PanPers | Graphic3d_TMF_ZoomPers | Graphic3d_TMF_RotatePers
};


#endif
