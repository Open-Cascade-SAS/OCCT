// Created on: 2015-03-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_LayerFilter_H__
#define _OpenGl_LayerFilter_H__

//! Tool object to specify processed OpenGL layers
//! for intermixed rendering of raytracable and non-raytracable layers.
enum OpenGl_LayerFilter
{
  OpenGl_LF_All,        //!< process all layers
  OpenGl_LF_Upper,      //!< process only top non-raytracable layers
  OpenGl_LF_Bottom,     //!< process only Graphic3d_ZLayerId_BotOSD
  OpenGl_LF_Single,     //!< process single layer
  OpenGl_LF_RayTracable //!< process only normal raytracable layers (save the bottom layer)
};

#endif //_OpenGl_LayerFilter_H__
