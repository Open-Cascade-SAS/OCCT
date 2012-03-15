// File:      OpenGl_GlCore12.hxx
// Created:   06 March 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_GlCore12_H__
#define _OpenGl_GlCore12_H__

#include <OpenGl_GlCore11.hxx>

// GL version can be defined by system gl.h header
#undef GL_VERSION_1_2
#undef GL_VERSION_1_3
#undef GL_VERSION_1_4
#undef GL_VERSION_1_5
#undef GL_VERSION_2_0

// include glext.h provided by Khronos group
#include <glext.h>

//! Function list for GL1.2 core functionality.
struct OpenGl_GlCore12
{

  PFNGLBLENDCOLORPROC        glBlendColor;
  PFNGLBLENDEQUATIONPROC     glBlendEquation;
  PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
  PFNGLTEXIMAGE3DPROC        glTexImage3D;
  PFNGLTEXSUBIMAGE3DPROC     glTexSubImage3D;
  PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;

};

#endif // _OpenGl_GlCore12_H__
