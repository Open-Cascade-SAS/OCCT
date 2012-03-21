// Created on: 2012-01-26
// Created by: Kirill GAVRILOV
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


#ifndef _OpenGl_ArbVBO_H__
#define _OpenGl_ArbVBO_H__

#include <OpenGl_GlCore12.hxx>

//! VBO is part of OpenGL since 1.5
struct OpenGl_ArbVBO
{

  PFNGLGENBUFFERSARBPROC    glGenBuffersARB;
  PFNGLBINDBUFFERARBPROC    glBindBufferARB;
  PFNGLBUFFERDATAARBPROC    glBufferDataARB;
  PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;

};

#endif // _OpenGl_ArbVBO_H__
