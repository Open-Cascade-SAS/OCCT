// File:      OpenGl_GlCore14.hxx
// Created:   06 March 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_GlCore14_H__
#define _OpenGl_GlCore14_H__

#include <OpenGl_GlCore13.hxx>

//! Function list for GL1.4 core functionality.
struct OpenGl_GlCore14 : public OpenGl_GlCore13
{

  PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
  PFNGLMULTIDRAWARRAYSPROC   glMultiDrawArrays;
  PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
  PFNGLPOINTPARAMETERFPROC   glPointParameterf;
  PFNGLPOINTPARAMETERFVPROC  glPointParameterfv;
  PFNGLPOINTPARAMETERIPROC   glPointParameteri;
  PFNGLPOINTPARAMETERIVPROC  glPointParameteriv;

};

#endif // _OpenGl_GlCore14_H__
