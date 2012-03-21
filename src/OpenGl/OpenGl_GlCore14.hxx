// Created on: 2012-03-06
// Created by: Kirill GAVRILOV
// Copyright (c) -1999 Matra Datavision
// Copyright (c) 2012-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
