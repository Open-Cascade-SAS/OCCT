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

#ifndef _OpenGl_GlCore41_Header
#define _OpenGl_GlCore41_Header

#include <OpenGl_GlCore40.hxx>

//! OpenGL 4.1 definition.
template<typename theBaseClass_t>
struct OpenGl_TmplCore41 : public theBaseClass_t
{

#if !defined(GL_ES_VERSION_2_0)

public: //! @name GL_ARB_ES2_compatibility (added to OpenGL 4.1 core)

  using theBaseClass_t::glReleaseShaderCompiler;
  using theBaseClass_t::glShaderBinary;
  using theBaseClass_t::glGetShaderPrecisionFormat;
  using theBaseClass_t::glDepthRangef;
  using theBaseClass_t::glClearDepthf;

public: //! @name GL_ARB_get_program_binary (added to OpenGL 4.1 core)

  using theBaseClass_t::glGetProgramBinary;
  using theBaseClass_t::glProgramBinary;
  using theBaseClass_t::glProgramParameteri;

public: //! @name GL_ARB_separate_shader_objects (added to OpenGL 4.1 core)

  using theBaseClass_t::glUseProgramStages;
  using theBaseClass_t::glActiveShaderProgram;
  using theBaseClass_t::glCreateShaderProgramv;
  using theBaseClass_t::glBindProgramPipeline;
  using theBaseClass_t::glDeleteProgramPipelines;
  using theBaseClass_t::glGenProgramPipelines;
  using theBaseClass_t::glIsProgramPipeline;
  using theBaseClass_t::glGetProgramPipelineiv;
  using theBaseClass_t::glProgramUniform1i;
  using theBaseClass_t::glProgramUniform1iv;
  using theBaseClass_t::glProgramUniform1f;
  using theBaseClass_t::glProgramUniform1fv;
  using theBaseClass_t::glProgramUniform1d;
  using theBaseClass_t::glProgramUniform1dv;
  using theBaseClass_t::glProgramUniform1ui;
  using theBaseClass_t::glProgramUniform1uiv;
  using theBaseClass_t::glProgramUniform2i;
  using theBaseClass_t::glProgramUniform2iv;
  using theBaseClass_t::glProgramUniform2f;
  using theBaseClass_t::glProgramUniform2fv;
  using theBaseClass_t::glProgramUniform2d;
  using theBaseClass_t::glProgramUniform2dv;
  using theBaseClass_t::glProgramUniform2ui;
  using theBaseClass_t::glProgramUniform2uiv;
  using theBaseClass_t::glProgramUniform3i;
  using theBaseClass_t::glProgramUniform3iv;
  using theBaseClass_t::glProgramUniform3f;
  using theBaseClass_t::glProgramUniform3fv;
  using theBaseClass_t::glProgramUniform3d;
  using theBaseClass_t::glProgramUniform3dv;
  using theBaseClass_t::glProgramUniform3ui;
  using theBaseClass_t::glProgramUniform3uiv;
  using theBaseClass_t::glProgramUniform4i;
  using theBaseClass_t::glProgramUniform4iv;
  using theBaseClass_t::glProgramUniform4f;
  using theBaseClass_t::glProgramUniform4fv;
  using theBaseClass_t::glProgramUniform4d;
  using theBaseClass_t::glProgramUniform4dv;
  using theBaseClass_t::glProgramUniform4ui;
  using theBaseClass_t::glProgramUniform4uiv;
  using theBaseClass_t::glProgramUniformMatrix2fv;
  using theBaseClass_t::glProgramUniformMatrix3fv;
  using theBaseClass_t::glProgramUniformMatrix4fv;
  using theBaseClass_t::glProgramUniformMatrix2dv;
  using theBaseClass_t::glProgramUniformMatrix3dv;
  using theBaseClass_t::glProgramUniformMatrix4dv;
  using theBaseClass_t::glProgramUniformMatrix2x3fv;
  using theBaseClass_t::glProgramUniformMatrix3x2fv;
  using theBaseClass_t::glProgramUniformMatrix2x4fv;
  using theBaseClass_t::glProgramUniformMatrix4x2fv;
  using theBaseClass_t::glProgramUniformMatrix3x4fv;
  using theBaseClass_t::glProgramUniformMatrix4x3fv;
  using theBaseClass_t::glProgramUniformMatrix2x3dv;
  using theBaseClass_t::glProgramUniformMatrix3x2dv;
  using theBaseClass_t::glProgramUniformMatrix2x4dv;
  using theBaseClass_t::glProgramUniformMatrix4x2dv;
  using theBaseClass_t::glProgramUniformMatrix3x4dv;
  using theBaseClass_t::glProgramUniformMatrix4x3dv;
  using theBaseClass_t::glValidateProgramPipeline;
  using theBaseClass_t::glGetProgramPipelineInfoLog;

public: //! @name GL_ARB_vertex_attrib_64bit (added to OpenGL 4.1 core)

  using theBaseClass_t::glVertexAttribL1d;
  using theBaseClass_t::glVertexAttribL2d;
  using theBaseClass_t::glVertexAttribL3d;
  using theBaseClass_t::glVertexAttribL4d;
  using theBaseClass_t::glVertexAttribL1dv;
  using theBaseClass_t::glVertexAttribL2dv;
  using theBaseClass_t::glVertexAttribL3dv;
  using theBaseClass_t::glVertexAttribL4dv;
  using theBaseClass_t::glVertexAttribLPointer;
  using theBaseClass_t::glGetVertexAttribLdv;

public: //! @name GL_ARB_viewport_array (added to OpenGL 4.1 core)

  using theBaseClass_t::glViewportArrayv;
  using theBaseClass_t::glViewportIndexedf;
  using theBaseClass_t::glViewportIndexedfv;
  using theBaseClass_t::glScissorArrayv;
  using theBaseClass_t::glScissorIndexed;
  using theBaseClass_t::glScissorIndexedv;
  using theBaseClass_t::glDepthRangeArrayv;
  using theBaseClass_t::glDepthRangeIndexed;
  using theBaseClass_t::glGetFloati_v;
  using theBaseClass_t::glGetDoublei_v;

public: //! @name OpenGL 4.1 additives to 4.0

#endif

};

//! OpenGL 4.1 compatibility profile.
typedef OpenGl_TmplCore41<OpenGl_GlCore40Back> OpenGl_GlCore41Back;

//! OpenGL 4.1 core profile.
typedef OpenGl_TmplCore41<OpenGl_GlCore40>     OpenGl_GlCore41;

#endif // _OpenGl_GlCore41_Header
