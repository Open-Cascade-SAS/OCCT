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

#include <OpenGl_GlCore20.hxx>


IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Caps,Standard_Transient)

// =======================================================================
// function : OpenGl_Caps
// purpose  :
// =======================================================================
OpenGl_Caps::OpenGl_Caps()
: vboDisable        (Standard_False),
  pntSpritesDisable (Standard_False),
  keepArrayData     (Standard_False),
#if !defined(GL_ES_VERSION_2_0)
  ffpEnable         (Standard_False),
  useSystemBuffer   (Standard_False),
#else
  ffpEnable         (Standard_False),
  useSystemBuffer   (Standard_True),
#endif
  swapInterval      (1),
  buffersNoSwap     (Standard_False),
  contextStereo     (Standard_False),
#ifdef OCCT_DEBUG
  contextDebug      (Standard_True),
  contextSyncDebug  (Standard_True),
#else
  contextDebug      (Standard_False),
  contextSyncDebug  (Standard_False),
#endif
  contextNoAccel    (Standard_False),
#if !defined(GL_ES_VERSION_2_0)
  contextCompatible (Standard_True),
#else
  contextCompatible (Standard_False),
#endif
  glslWarnings      (Standard_False),
  suppressExtraMsg  (Standard_True)
{
  //
}

// =======================================================================
// function : operator=
// purpose  :
// =======================================================================
OpenGl_Caps& OpenGl_Caps::operator= (const OpenGl_Caps& theCopy)
{
  vboDisable        = theCopy.vboDisable;
  pntSpritesDisable = theCopy.pntSpritesDisable;
  keepArrayData     = theCopy.keepArrayData;
  ffpEnable         = theCopy.ffpEnable;
  useSystemBuffer   = theCopy.useSystemBuffer;
  swapInterval      = theCopy.swapInterval;
  buffersNoSwap     = theCopy.buffersNoSwap;
  contextStereo     = theCopy.contextStereo;
  contextDebug      = theCopy.contextDebug;
  contextSyncDebug  = theCopy.contextSyncDebug;
  contextNoAccel    = theCopy.contextNoAccel;
  contextCompatible = theCopy.contextCompatible;
  glslWarnings      = theCopy.glslWarnings;
  suppressExtraMsg  = theCopy.suppressExtraMsg;
  return *this;
}

// =======================================================================
// function : ~OpenGl_Caps
// purpose  :
// =======================================================================
OpenGl_Caps::~OpenGl_Caps()
{
  //
}
