// Created on: 2014-03-17
// Created by: Kirill GAVRILOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef OpenGl_GlCore11Fwd_HeaderFile
#define OpenGl_GlCore11Fwd_HeaderFile

#include <OpenGl_GlFunctions.hxx>

//! OpenGL 1.1 core without deprecated Fixed Pipeline entry points.
//! Notice that all functions within this structure are actually exported by system GL library.
//! The main purpose for these hint - to control visibility of functions per GL version
//! (global functions should not be used directly to achieve this effect!).
struct OpenGl_GlCore11Fwd : protected OpenGl_GlFunctions
{

public: //! @name Miscellaneous

  using OpenGl_GlFunctions::glClearColor;
  using OpenGl_GlFunctions::glClear;
  using OpenGl_GlFunctions::glColorMask;
  using OpenGl_GlFunctions::glBlendFunc;
  using OpenGl_GlFunctions::glCullFace;
  using OpenGl_GlFunctions::glFrontFace;
  using OpenGl_GlFunctions::glLineWidth;
  using OpenGl_GlFunctions::glPolygonOffset;
  using OpenGl_GlFunctions::glScissor;
  using OpenGl_GlFunctions::glEnable;
  using OpenGl_GlFunctions::glDisable;
  using OpenGl_GlFunctions::glIsEnabled;
  using OpenGl_GlFunctions::glGetBooleanv;
  using OpenGl_GlFunctions::glGetFloatv;
  using OpenGl_GlFunctions::glGetIntegerv;
  using OpenGl_GlFunctions::glGetError;
  using OpenGl_GlFunctions::glGetString;
  using OpenGl_GlFunctions::glFinish;
  using OpenGl_GlFunctions::glFlush;
  using OpenGl_GlFunctions::glHint;

public: //! @name Depth Buffer

  using OpenGl_GlFunctions::glClearDepth;
  using OpenGl_GlFunctions::glClearDepthf;
  using OpenGl_GlFunctions::glDepthFunc;
  using OpenGl_GlFunctions::glDepthMask;
  using OpenGl_GlFunctions::glDepthRange;
  using OpenGl_GlFunctions::glDepthRangef;

public: //! @name Transformation

  using OpenGl_GlFunctions::glViewport;

public: //! @name Vertex Arrays

  using OpenGl_GlFunctions::glDrawArrays;
  using OpenGl_GlFunctions::glDrawElements;

public: //! @name Raster functions

  using OpenGl_GlFunctions::glPixelStorei;
  using OpenGl_GlFunctions::glReadPixels;

public: //! @name Stenciling

  using OpenGl_GlFunctions::glStencilFunc;
  using OpenGl_GlFunctions::glStencilMask;
  using OpenGl_GlFunctions::glStencilOp;
  using OpenGl_GlFunctions::glClearStencil;

public: //! @name Texture mapping

  using OpenGl_GlFunctions::glTexParameterf;
  using OpenGl_GlFunctions::glTexParameteri;
  using OpenGl_GlFunctions::glTexParameterfv;
  using OpenGl_GlFunctions::glTexParameteriv;
  using OpenGl_GlFunctions::glGetTexParameterfv;
  using OpenGl_GlFunctions::glGetTexParameteriv;
  using OpenGl_GlFunctions::glTexImage2D;
  using OpenGl_GlFunctions::glGenTextures;
  using OpenGl_GlFunctions::glDeleteTextures;
  using OpenGl_GlFunctions::glBindTexture;
  using OpenGl_GlFunctions::glIsTexture;
  using OpenGl_GlFunctions::glTexSubImage2D;
  using OpenGl_GlFunctions::glCopyTexImage2D;
  using OpenGl_GlFunctions::glCopyTexSubImage2D;

public: //! @name desktop extensions - not supported in OpenGL ES 2..0

  using OpenGl_GlFunctions::glTexImage1D;
  using OpenGl_GlFunctions::glTexSubImage1D;
  using OpenGl_GlFunctions::glCopyTexImage1D;
  using OpenGl_GlFunctions::glCopyTexSubImage1D;
  using OpenGl_GlFunctions::glGetTexImage;
  using OpenGl_GlFunctions::glAlphaFunc;
  using OpenGl_GlFunctions::glPointSize;

  // added to OpenGL ES 3.0
  using OpenGl_GlFunctions::glReadBuffer;
  using OpenGl_GlFunctions::glDrawBuffer;

  // added to OpenGL ES 3.1
  using OpenGl_GlFunctions::glGetTexLevelParameteriv;

  // added to OpenGL ES 3.2
  using OpenGl_GlFunctions::glGetPointerv;

  using OpenGl_GlFunctions::glPolygonMode;
  using OpenGl_GlFunctions::glLogicOp;

};

#endif // _OpenGl_GlCore11Fwd_Header
