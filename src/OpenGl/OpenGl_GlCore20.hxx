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


#ifndef _OpenGl_GlCore20_H__
#define _OpenGl_GlCore20_H__

#include <OpenGl_GlCore15.hxx>

//! Function list for GL2.0 core functionality.
struct OpenGl_GlCore20 : public OpenGl_GlCore15
{

  PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate;
  PFNGLDRAWBUFFERSPROC              glDrawBuffers;
  PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
  PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
  PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate;
  PFNGLATTACHSHADERPROC             glAttachShader;
  PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
  PFNGLCOMPILESHADERPROC            glCompileShader;
  PFNGLCREATEPROGRAMPROC            glCreateProgram;
  PFNGLCREATESHADERPROC             glCreateShader;
  PFNGLDELETEPROGRAMPROC            glDeleteProgram;
  PFNGLDELETESHADERPROC             glDeleteShader;
  PFNGLDETACHSHADERPROC             glDetachShader;
  PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
  PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
  PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib;
  PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;
  PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders;
  PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
  PFNGLGETPROGRAMIVPROC             glGetProgramiv;
  PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
  PFNGLGETSHADERIVPROC              glGetShaderiv;
  PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
  PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
  PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
  PFNGLGETUNIFORMFVPROC             glGetUniformfv;
  PFNGLGETUNIFORMIVPROC             glGetUniformiv;
  PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv;
  PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv;
  PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv;
  PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv;
  PFNGLISPROGRAMPROC                glIsProgram;
  PFNGLISSHADERPROC                 glIsShader;
  PFNGLLINKPROGRAMPROC              glLinkProgram;
  PFNGLSHADERSOURCEPROC             glShaderSource;
  PFNGLUSEPROGRAMPROC               glUseProgram;
  PFNGLUNIFORM1FPROC                glUniform1f;
  PFNGLUNIFORM2FPROC                glUniform2f;
  PFNGLUNIFORM3FPROC                glUniform3f;
  PFNGLUNIFORM4FPROC                glUniform4f;
  PFNGLUNIFORM1IPROC                glUniform1i;
  PFNGLUNIFORM2IPROC                glUniform2i;
  PFNGLUNIFORM3IPROC                glUniform3i;
  PFNGLUNIFORM4IPROC                glUniform4i;
  PFNGLUNIFORM1FVPROC               glUniform1fv;
  PFNGLUNIFORM2FVPROC               glUniform2fv;
  PFNGLUNIFORM3FVPROC               glUniform3fv;
  PFNGLUNIFORM4FVPROC               glUniform4fv;
  PFNGLUNIFORM1IVPROC               glUniform1iv;
  PFNGLUNIFORM2IVPROC               glUniform2iv;
  PFNGLUNIFORM3IVPROC               glUniform3iv;
  PFNGLUNIFORM4IVPROC               glUniform4iv;
  PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv;
  PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
  PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
  PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
  PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d;
  PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv;
  PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f;
  PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv;
  PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s;
  PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv;
  PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d;
  PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv;
  PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f;
  PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv;
  PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s;
  PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv;
  PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d;
  PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv;
  PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f;
  PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv;
  PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s;
  PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv;
  PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv;
  PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv;
  PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv;
  PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub;
  PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv;
  PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv;
  PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv;
  PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv;
  PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d;
  PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv;
  PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
  PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv;
  PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv;
  PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s;
  PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv;
  PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv;
  PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv;
  PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv;
  PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;

};

#endif // _OpenGl_GlCore20_H__
