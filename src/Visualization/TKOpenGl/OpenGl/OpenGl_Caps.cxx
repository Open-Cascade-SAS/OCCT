// Created on: 2013-08-25
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_Caps.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Caps, Standard_Transient)

//=================================================================================================

OpenGl_Caps::OpenGl_Caps()
    : sRGBDisable(false),
      compressedTexturesDisable(false),
      vboDisable(false),
      pntSpritesDisable(false),
      keepArrayData(false),
      ffpEnable(false),
      usePolygonMode(false),
      useSystemBuffer(false),
      swapInterval(1),
      useZeroToOneDepth(false),
      buffersNoSwap(false),
      buffersOpaqueAlpha(true),
      buffersDeepColor(false),
      contextStereo(false),
      contextDebug(false),
      contextSyncDebug(false),
      contextNoAccel(false),
      contextCompatible(true),
      contextNoExtensions(false),
      contextMajorVersionUpper(-1),
      contextMinorVersionUpper(-1),
      isTopDownTextureUV(false),
      glslWarnings(false),
      suppressExtraMsg(true),
      glslDumpLevel(OpenGl_ShaderProgramDumpLevel_Off)
{
#if defined(__EMSCRIPTEN__)
  buffersNoSwap = true; // swap has no effect in WebGL
#endif
#ifdef OCCT_DEBUG
  contextDebug     = true;
  contextSyncDebug = true;
#endif
}

// =======================================================================
// function : operator=
// purpose  :
// =======================================================================
OpenGl_Caps& OpenGl_Caps::operator=(const OpenGl_Caps& theCopy)
{
  sRGBDisable               = theCopy.sRGBDisable;
  compressedTexturesDisable = theCopy.compressedTexturesDisable;
  vboDisable                = theCopy.vboDisable;
  pntSpritesDisable         = theCopy.pntSpritesDisable;
  keepArrayData             = theCopy.keepArrayData;
  ffpEnable                 = theCopy.ffpEnable;
  useSystemBuffer           = theCopy.useSystemBuffer;
  swapInterval              = theCopy.swapInterval;
  useZeroToOneDepth         = theCopy.useZeroToOneDepth;
  buffersNoSwap             = theCopy.buffersNoSwap;
  buffersOpaqueAlpha        = theCopy.buffersOpaqueAlpha;
  buffersDeepColor          = theCopy.buffersDeepColor;
  contextStereo             = theCopy.contextStereo;
  contextDebug              = theCopy.contextDebug;
  contextSyncDebug          = theCopy.contextSyncDebug;
  contextNoAccel            = theCopy.contextNoAccel;
  contextCompatible         = theCopy.contextCompatible;
  contextNoExtensions       = theCopy.contextNoExtensions;
  contextMajorVersionUpper  = theCopy.contextMajorVersionUpper;
  contextMinorVersionUpper  = theCopy.contextMinorVersionUpper;
  isTopDownTextureUV        = theCopy.isTopDownTextureUV;
  glslWarnings              = theCopy.glslWarnings;
  suppressExtraMsg          = theCopy.suppressExtraMsg;
  glslDumpLevel             = theCopy.glslDumpLevel;
  return *this;
}

//=================================================================================================

OpenGl_Caps::~OpenGl_Caps()
{
  //
}
