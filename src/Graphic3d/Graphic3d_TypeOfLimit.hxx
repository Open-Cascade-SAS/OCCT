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

#ifndef _Graphic3d_TypeOfLimit_HeaderFile
#define _Graphic3d_TypeOfLimit_HeaderFile

//! Type of graphic resource limit.
enum Graphic3d_TypeOfLimit
{
  Graphic3d_TypeOfLimit_MaxNbLights,             //!< maximum number of active light sources
  Graphic3d_TypeOfLimit_MaxNbClipPlanes,         //!< maximum number of active clipping planes
  Graphic3d_TypeOfLimit_MaxNbViews,              //!< maximum number of views
  Graphic3d_TypeOfLimit_MaxTextureSize,          //!< maximum size of texture
  Graphic3d_TypeOfLimit_MaxViewDumpSizeX,        //!< maximum width  for image dump
  Graphic3d_TypeOfLimit_MaxViewDumpSizeY,        //!< maximum height for image dump
  Graphic3d_TypeOfLimit_MaxCombinedTextureUnits, //!< maximum number of combined texture units for
                                                 //!< multitexturing
  Graphic3d_TypeOfLimit_MaxMsaa,                 //!< maximum number of MSAA samples
  Graphic3d_TypeOfLimit_HasPBR, //!< indicates whether PBR metallic-roughness shading model is
                                //!< supported
  Graphic3d_TypeOfLimit_HasRayTracing,         //!< indicates whether ray tracing is supported
  Graphic3d_TypeOfLimit_HasRayTracingTextures, //!< indicates whether ray tracing textures are
                                               //!< supported
  Graphic3d_TypeOfLimit_HasRayTracingAdaptiveSampling,       //!< indicates whether adaptive screen
                                                             //!< sampling is supported
  Graphic3d_TypeOfLimit_HasRayTracingAdaptiveSamplingAtomic, //!< indicates whether optimized
                                                             //!< adaptive screen sampling is
                                                             //!< supported (hardware supports
                                                             //!< atomic float operations)
  Graphic3d_TypeOfLimit_HasSRGB,       //!< indicates whether sRGB rendering is supported
  Graphic3d_TypeOfLimit_HasBlendedOit, //!< indicates whether necessary GL extensions for Weighted,
                                       //!< Blended OIT available (without MSAA).
  Graphic3d_TypeOfLimit_HasBlendedOitMsaa, //!< indicates whether necessary GL extensions for
                                           //!< Weighted, Blended OIT available (with MSAA).
  Graphic3d_TypeOfLimit_HasFlatShading,    //!< indicates whether Flat shading
                                        //!< (Graphic3d_TypeOfShadingModel_PhongFacet) is supported
  Graphic3d_TypeOfLimit_HasMeshEdges,    //!< indicates whether advanced mesh edges presentation is
                                         //!< supported
  Graphic3d_TypeOfLimit_IsWorkaroundFBO, //!< indicates whether workaround for Intel driver problem
                                         //!< with empty FBO for images with big width is applied.
  Graphic3d_TypeOfLimit_NB               //!< number of elements in this enumeration
};

#endif // _Graphic3d_TypeOfLimit_HeaderFile
