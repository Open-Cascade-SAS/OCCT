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

#ifndef _V3d_ImageDumpOptions_HeaderFile
#define _V3d_ImageDumpOptions_HeaderFile

#include <Graphic3d_BufferType.hxx>
#include <V3d_StereoDumpOptions.hxx>
#include <Graphic3d_ZLayerId.hxx>

//! The structure defines options for image dump functionality.
struct V3d_ImageDumpOptions
{

  // clang-format off
  Standard_Integer      Width;          //!< Width  of image dump to allocate an image, 0 by default (meaning that image should be already allocated).
  Standard_Integer      Height;         //!< Height of image dump to allocate an image, 0 by default (meaning that image should be already allocated).
  Graphic3d_BufferType  BufferType;     //!< Which buffer to dump (color / depth), Graphic3d_BT_RGB by default.
  V3d_StereoDumpOptions StereoOptions;  //!< Dumping stereoscopic camera, V3d_SDO_MONO by default (middle-point monographic projection).
  Standard_Integer      TileSize;       //!< The view dimension limited for tiled dump, 0 by default (automatic tiling depending on hardware capabilities).
  Standard_Boolean      ToAdjustAspect; //!< Flag to override active view aspect ratio by (Width / Height) defined for image dump (TRUE by default).
  Graphic3d_ZLayerId    TargetZLayerId; //!< Target z layer id which defines the last layer to be drawn before image dump.
  Standard_Boolean      IsSingleLayer;  //<! Flags if dumping is to be done to a single or to multiple layers.
  Standard_CString      LightName;      //<! Name of the target light whose shadowmap is to be dumped.
  // clang-format on

public:
  //! Default constructor.
  V3d_ImageDumpOptions()
      : Width(0),
        Height(0),
        BufferType(Graphic3d_BT_RGB),
        StereoOptions(V3d_SDO_MONO),
        TileSize(0),
        ToAdjustAspect(Standard_True),
        TargetZLayerId(Graphic3d_ZLayerId_BotOSD),
        IsSingleLayer(Standard_False),
        LightName("")
  {
  }
};

#endif // _V3d_ImageDumpOptions_HeaderFile
