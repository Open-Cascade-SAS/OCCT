// Copyright (c) 2021 OPEN CASCADE SAS
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

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <OpenGl_Context.hxx>

#include <OpenGl_ArbTBO.hxx>
#include <OpenGl_ArbIns.hxx>
#include <OpenGl_ArbDbg.hxx>
#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_ExtGS.hxx>
#include <OpenGl_ArbSamplerObject.hxx>
#include <OpenGl_ArbTexBindless.hxx>
#include <OpenGl_GlCore46.hxx>

#if !defined(HAVE_EGL) && defined(HAVE_XLIB)
  #include <GL/glx.h>
#endif

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void OpenGl_GlFunctions::load (OpenGl_Context& theCtx,
                               Standard_Boolean theIsCoreProfile)
{
#if defined(GL_ES_VERSION_2_0)
  (void )theIsCoreProfile;
  theCtx.core11ffp = NULL;
#else
  theCtx.core11ffp = !theIsCoreProfile ? (OpenGl_GlCore11* )this : NULL;
#endif
  theCtx.core11fwd  = (OpenGl_GlCore11Fwd* )this;
  theCtx.core15     = NULL;
  theCtx.core15fwd  = NULL;
  theCtx.core20     = NULL;
  theCtx.core20fwd  = NULL;
  theCtx.core30     = NULL;
  theCtx.core32     = NULL;
  theCtx.core33     = NULL;
  theCtx.core41     = NULL;
  theCtx.core42     = NULL;
  theCtx.core43     = NULL;
  theCtx.core44     = NULL;
  theCtx.core45     = NULL;
  theCtx.core46     = NULL;
  theCtx.arbTBO     = NULL;
  theCtx.arbTboRGB32 = false;
  theCtx.arbClipControl = false;
  theCtx.arbIns     = NULL;
  theCtx.arbDbg     = NULL;
  theCtx.arbFBO     = NULL;
  theCtx.arbFBOBlit = NULL;
  theCtx.extGS      = NULL;

  //! Make record shorter to retrieve function pointer using variable with same name
  const char* aLastFailedProc = NULL;
  #define FindProcShort(theFunc) theCtx.FindProcVerbose(aLastFailedProc, #theFunc, this->theFunc)
  #define checkExtensionShort theCtx.CheckExtension
  #define isGlGreaterEqualShort(theMaj,theMin) theCtx.IsGlGreaterEqual(theMaj,theMin)

#if defined(GL_ES_VERSION_2_0)

  theCtx.hasTexRGBA8 = isGlGreaterEqualShort (3, 0)
                    || checkExtensionShort ("GL_OES_rgb8_rgba8");
  theCtx.hasTexSRGB  = isGlGreaterEqualShort (3, 0);
  theCtx.hasFboSRGB  = isGlGreaterEqualShort (3, 0);
  if (!isGlGreaterEqualShort (3, 0)
    && checkExtensionShort ("GL_EXT_sRGB"))
  {
    // limited support
    theCtx.hasTexSRGB = true;
    theCtx.hasFboSRGB = true;
  }
  theCtx.hasFboRenderMipmap = isGlGreaterEqualShort (3, 0)
                           || checkExtensionShort ("GL_OES_fbo_render_mipmap");
  theCtx.hasSRGBControl = checkExtensionShort ("GL_EXT_sRGB_write_control");
  theCtx.hasPackRowLength   = isGlGreaterEqualShort (3, 0);
  theCtx.hasUnpackRowLength = isGlGreaterEqualShort (3, 0); // || checkExtensionShort ("GL_EXT_unpack_subimage");
  // NPOT textures has limited support within OpenGL ES 2.0
  // which are relaxed by OpenGL ES 3.0 or some extensions
  //theCtx.arbNPTW = isGlGreaterEqualShort (3, 0)
  //           || checkExtensionShort ("GL_OES_texture_npot")
  //           || checkExtensionShort ("GL_NV_texture_npot_2D_mipmap");
  theCtx.arbNPTW     = true;
  theCtx.arbTexRG    = isGlGreaterEqualShort (3, 0)
                    || checkExtensionShort ("GL_EXT_texture_rg");
  theCtx.extBgra     = checkExtensionShort ("GL_EXT_texture_format_BGRA8888");
  theCtx.extAnis = checkExtensionShort ("GL_EXT_texture_filter_anisotropic");
  theCtx.extPDS  = isGlGreaterEqualShort (3, 0)
                || checkExtensionShort ("GL_OES_packed_depth_stencil");

  theCtx.core11fwd = (OpenGl_GlCore11Fwd* )this;
  if (isGlGreaterEqualShort (2, 0))
  {
    // enable compatible functions
    theCtx.core20    = (OpenGl_GlCore20* )this;
    theCtx.core20fwd = (OpenGl_GlCore20* )this;
    theCtx.core15    = (OpenGl_GlCore15* )this;
    theCtx.core15fwd = (OpenGl_GlCore15* )this;
    theCtx.arbFBO    = (OpenGl_ArbFBO*   )this;
  }
  if (isGlGreaterEqualShort (3, 0)
   && FindProcShort (glBlitFramebuffer))
  {
    theCtx.arbFBOBlit = (OpenGl_ArbFBOBlit* )this;
  }
  if (isGlGreaterEqualShort (3, 0)
   && FindProcShort (glGenSamplers)
   && FindProcShort (glDeleteSamplers)
   && FindProcShort (glIsSampler)
   && FindProcShort (glBindSampler)
   && FindProcShort (glSamplerParameteri)
   && FindProcShort (glSamplerParameteriv)
   && FindProcShort (glSamplerParameterf)
   && FindProcShort (glSamplerParameterfv)
   && FindProcShort (glGetSamplerParameteriv)
   && FindProcShort (glGetSamplerParameterfv))
   //&& FindProcShort (glSamplerParameterIiv) // only on Desktop or with extensions GL_OES_texture_border_clamp/GL_EXT_texture_border_clamp
   //&& FindProcShort (glSamplerParameterIuiv)
   //&& FindProcShort (glGetSamplerParameterIiv)
   //&& FindProcShort (glGetSamplerParameterIuiv))
  {
    theCtx.arbSamplerObject = (OpenGl_ArbSamplerObject* )this;
  }
  theCtx.extFragDepth = !isGlGreaterEqualShort(3, 0)
                      && checkExtensionShort ("GL_EXT_frag_depth");
  if (isGlGreaterEqualShort (3, 1)
   && FindProcShort (glTexStorage2DMultisample))
  {
    //
  }

  theCtx.hasUintIndex = isGlGreaterEqualShort (3, 0)
                     || checkExtensionShort ("GL_OES_element_index_uint");
  theCtx.hasHighp     = checkExtensionShort ("GL_OES_fragment_precision_high");
  GLint aRange[2] = {0, 0};
  GLint aPrec     = 0;
  ::glGetShaderPrecisionFormat (GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, aRange, &aPrec);
  if (aPrec != 0)
  {
    theCtx.hasHighp = Standard_True;
  }

  theCtx.arbTexFloat = (isGlGreaterEqualShort (3, 0)
                     && FindProcShort (glTexImage3D))
                     || checkExtensionShort ("GL_OES_texture_float");
  theCtx.hasTexFloatLinear = theCtx.arbTexFloat
                          && checkExtensionShort ("GL_OES_texture_float_linear");

  const bool hasTexBuffer32  = isGlGreaterEqualShort (3, 2) && FindProcShort (glTexBuffer);
  const bool hasExtTexBuffer = checkExtensionShort ("GL_EXT_texture_buffer") && theCtx.FindProc ("glTexBufferEXT", this->glTexBuffer);
  if (hasTexBuffer32 || hasExtTexBuffer)
  {
    theCtx.arbTBO = reinterpret_cast<OpenGl_ArbTBO*> (this);
  }

  bool hasInstanced = isGlGreaterEqualShort (3, 0)
       && FindProcShort (glVertexAttribDivisor)
       && FindProcShort (glDrawArraysInstanced)
       && FindProcShort (glDrawElementsInstanced);
  if (!hasInstanced)
  {
    hasInstanced = checkExtensionShort ("GL_ANGLE_instanced_arrays")
       && theCtx.FindProc ("glVertexAttribDivisorANGLE",   this->glVertexAttribDivisor)
       && theCtx.FindProc ("glDrawArraysInstancedANGLE",   this->glDrawArraysInstanced)
       && theCtx.FindProc ("glDrawElementsInstancedANGLE", this->glDrawElementsInstanced);
  }
  if (hasInstanced)
  {
    theCtx.arbIns = (OpenGl_ArbIns* )this;
  }

  const bool hasVAO = isGlGreaterEqualShort (3, 0)
       && FindProcShort (glBindVertexArray)
       && FindProcShort (glDeleteVertexArrays)
       && FindProcShort (glGenVertexArrays)
       && FindProcShort (glIsVertexArray);
#ifndef __EMSCRIPTEN__ // latest Emscripten does not pretend having / simulating mapping buffer functions
  const bool hasMapBufferRange = isGlGreaterEqualShort (3, 0)
       && FindProcShort (glMapBufferRange)
       && FindProcShort (glUnmapBuffer)
       && FindProcShort (glGetBufferPointerv)
       && FindProcShort (glFlushMappedBufferRange);
#endif

  // load OpenGL ES 3.0 new functions
  const bool has30es = isGlGreaterEqualShort (3, 0)
       && hasVAO
    #ifndef __EMSCRIPTEN__
       && hasMapBufferRange
    #endif
       && hasInstanced
       && theCtx.arbSamplerObject != NULL
       && theCtx.arbFBOBlit != NULL
       && FindProcShort (glReadBuffer)
       && FindProcShort (glDrawRangeElements)
       && FindProcShort (glTexImage3D)
       && FindProcShort (glTexSubImage3D)
       && FindProcShort (glCopyTexSubImage3D)
       && FindProcShort (glCompressedTexImage3D)
       && FindProcShort (glCompressedTexSubImage3D)
       && FindProcShort (glGenQueries)
       && FindProcShort (glDeleteQueries)
       && FindProcShort (glIsQuery)
       && FindProcShort (glBeginQuery)
       && FindProcShort (glEndQuery)
       && FindProcShort (glGetQueryiv)
       && FindProcShort (glGetQueryObjectuiv)
       && FindProcShort (glDrawBuffers)
       && FindProcShort (glUniformMatrix2x3fv)
       && FindProcShort (glUniformMatrix3x2fv)
       && FindProcShort (glUniformMatrix2x4fv)
       && FindProcShort (glUniformMatrix4x2fv)
       && FindProcShort (glUniformMatrix3x4fv)
       && FindProcShort (glUniformMatrix4x3fv)
       && FindProcShort (glRenderbufferStorageMultisample)
       && FindProcShort (glFramebufferTextureLayer)
       && FindProcShort (glGetIntegeri_v)
       && FindProcShort (glBeginTransformFeedback)
       && FindProcShort (glEndTransformFeedback)
       && FindProcShort (glBindBufferRange)
       && FindProcShort (glBindBufferBase)
       && FindProcShort (glTransformFeedbackVaryings)
       && FindProcShort (glGetTransformFeedbackVarying)
       && FindProcShort (glVertexAttribIPointer)
       && FindProcShort (glGetVertexAttribIiv)
       && FindProcShort (glGetVertexAttribIuiv)
       && FindProcShort (glVertexAttribI4i)
       && FindProcShort (glVertexAttribI4ui)
       && FindProcShort (glVertexAttribI4iv)
       && FindProcShort (glVertexAttribI4uiv)
       && FindProcShort (glGetUniformuiv)
       && FindProcShort (glGetFragDataLocation)
       && FindProcShort (glUniform1ui)
       && FindProcShort (glUniform2ui)
       && FindProcShort (glUniform3ui)
       && FindProcShort (glUniform4ui)
       && FindProcShort (glUniform1uiv)
       && FindProcShort (glUniform2uiv)
       && FindProcShort (glUniform3uiv)
       && FindProcShort (glUniform4uiv)
       && FindProcShort (glClearBufferiv)
       && FindProcShort (glClearBufferuiv)
       && FindProcShort (glClearBufferfv)
       && FindProcShort (glClearBufferfi)
       && FindProcShort (glGetStringi)
       && FindProcShort (glCopyBufferSubData)
       && FindProcShort (glGetUniformIndices)
       && FindProcShort (glGetActiveUniformsiv)
       && FindProcShort (glGetUniformBlockIndex)
       && FindProcShort (glGetActiveUniformBlockiv)
       && FindProcShort (glGetActiveUniformBlockName)
       && FindProcShort (glUniformBlockBinding)
       && FindProcShort (glFenceSync)
       && FindProcShort (glIsSync)
       && FindProcShort (glDeleteSync)
       && FindProcShort (glClientWaitSync)
       && FindProcShort (glWaitSync)
       && FindProcShort (glGetInteger64v)
       && FindProcShort (glGetSynciv)
       && FindProcShort (glGetInteger64i_v)
       && FindProcShort (glGetBufferParameteri64v)
       && FindProcShort (glBindTransformFeedback)
       && FindProcShort (glDeleteTransformFeedbacks)
       && FindProcShort (glGenTransformFeedbacks)
       && FindProcShort (glIsTransformFeedback)
       && FindProcShort (glPauseTransformFeedback)
       && FindProcShort (glResumeTransformFeedback)
       && FindProcShort (glGetProgramBinary)
       && FindProcShort (glProgramBinary)
       && FindProcShort (glProgramParameteri)
       && FindProcShort (glInvalidateFramebuffer)
       && FindProcShort (glInvalidateSubFramebuffer)
       && FindProcShort (glTexStorage2D)
       && FindProcShort (glTexStorage3D)
       && FindProcShort (glGetInternalformativ);
  if (!has30es)
  {
    theCtx.checkWrongVersion (3, 0, aLastFailedProc);
  }
  else
  {
    theCtx.core30 = (OpenGl_GlCore30* )this;
    theCtx.hasGetBufferData = true;
  }

  // load OpenGL ES 3.1 new functions
  const bool has31es = isGlGreaterEqualShort (3, 1)
       && has30es
       && FindProcShort (glDispatchCompute)
       && FindProcShort (glDispatchComputeIndirect)
       && FindProcShort (glDrawArraysIndirect)
       && FindProcShort (glDrawElementsIndirect)
       && FindProcShort (glFramebufferParameteri)
       && FindProcShort (glGetFramebufferParameteriv)
       && FindProcShort (glGetProgramInterfaceiv)
       && FindProcShort (glGetProgramResourceIndex)
       && FindProcShort (glGetProgramResourceName)
       && FindProcShort (glGetProgramResourceiv)
       && FindProcShort (glGetProgramResourceLocation)
       && FindProcShort (glUseProgramStages)
       && FindProcShort (glActiveShaderProgram)
       && FindProcShort (glCreateShaderProgramv)
       && FindProcShort (glBindProgramPipeline)
       && FindProcShort (glDeleteProgramPipelines)
       && FindProcShort (glGenProgramPipelines)
       && FindProcShort (glIsProgramPipeline)
       && FindProcShort (glGetProgramPipelineiv)
       && FindProcShort (glProgramUniform1i)
       && FindProcShort (glProgramUniform2i)
       && FindProcShort (glProgramUniform3i)
       && FindProcShort (glProgramUniform4i)
       && FindProcShort (glProgramUniform1ui)
       && FindProcShort (glProgramUniform2ui)
       && FindProcShort (glProgramUniform3ui)
       && FindProcShort (glProgramUniform4ui)
       && FindProcShort (glProgramUniform1f)
       && FindProcShort (glProgramUniform2f)
       && FindProcShort (glProgramUniform3f)
       && FindProcShort (glProgramUniform4f)
       && FindProcShort (glProgramUniform1iv)
       && FindProcShort (glProgramUniform2iv)
       && FindProcShort (glProgramUniform3iv)
       && FindProcShort (glProgramUniform4iv)
       && FindProcShort (glProgramUniform1uiv)
       && FindProcShort (glProgramUniform2uiv)
       && FindProcShort (glProgramUniform3uiv)
       && FindProcShort (glProgramUniform4uiv)
       && FindProcShort (glProgramUniform1fv)
       && FindProcShort (glProgramUniform2fv)
       && FindProcShort (glProgramUniform3fv)
       && FindProcShort (glProgramUniform4fv)
       && FindProcShort (glProgramUniformMatrix2fv)
       && FindProcShort (glProgramUniformMatrix3fv)
       && FindProcShort (glProgramUniformMatrix4fv)
       && FindProcShort (glProgramUniformMatrix2x3fv)
       && FindProcShort (glProgramUniformMatrix3x2fv)
       && FindProcShort (glProgramUniformMatrix2x4fv)
       && FindProcShort (glProgramUniformMatrix4x2fv)
       && FindProcShort (glProgramUniformMatrix3x4fv)
       && FindProcShort (glProgramUniformMatrix4x3fv)
       && FindProcShort (glValidateProgramPipeline)
       && FindProcShort (glGetProgramPipelineInfoLog)
       && FindProcShort (glBindImageTexture)
       && FindProcShort (glGetBooleani_v)
       && FindProcShort (glMemoryBarrier)
       && FindProcShort (glMemoryBarrierByRegion)
       && FindProcShort (glTexStorage2DMultisample)
       && FindProcShort (glGetMultisamplefv)
       && FindProcShort (glSampleMaski)
       && FindProcShort (glGetTexLevelParameteriv)
       && FindProcShort (glGetTexLevelParameterfv)
       && FindProcShort (glBindVertexBuffer)
       && FindProcShort (glVertexAttribFormat)
       && FindProcShort (glVertexAttribIFormat)
       && FindProcShort (glVertexAttribBinding)
       && FindProcShort (glVertexBindingDivisor);
  if (!has31es)
  {
    theCtx.checkWrongVersion (3, 1, aLastFailedProc);
  }

  // initialize debug context extension
  if (isGlGreaterEqualShort (3, 2)
   || checkExtensionShort ("GL_KHR_debug"))
  {
    // this functionality become a part of OpenGL ES 3.2
    theCtx.arbDbg = NULL;
    if (isGlGreaterEqualShort (3, 2)
     && FindProcShort (glDebugMessageControl)
     && FindProcShort (glDebugMessageInsert)
     && FindProcShort (glDebugMessageCallback)
     && FindProcShort (glGetDebugMessageLog))
    {
      theCtx.arbDbg = (OpenGl_ArbDbg* )this;
    }
    // According to GL_KHR_debug spec, all functions should have KHR suffix.
    // However, some implementations can export these functions without suffix.
    else if (!isGlGreaterEqualShort (3, 2)
     && theCtx.FindProc ("glDebugMessageControlKHR",  this->glDebugMessageControl)
     && theCtx.FindProc ("glDebugMessageInsertKHR",   this->glDebugMessageInsert)
     && theCtx.FindProc ("glDebugMessageCallbackKHR", this->glDebugMessageCallback)
     && theCtx.FindProc ("glGetDebugMessageLogKHR",   this->glGetDebugMessageLog))
    {
      theCtx.arbDbg = (OpenGl_ArbDbg* )this;
    }
  }

  // load OpenGL ES 3.2 new functions
  const bool has32es = isGlGreaterEqualShort (3, 2)
       && has31es
       && hasTexBuffer32
       && theCtx.arbDbg != NULL
       && FindProcShort (glBlendBarrier)
       && FindProcShort (glCopyImageSubData)
       && FindProcShort (glPushDebugGroup)
       && FindProcShort (glPopDebugGroup)
       && FindProcShort (glObjectLabel)
       && FindProcShort (glGetObjectLabel)
       && FindProcShort (glObjectPtrLabel)
       && FindProcShort (glGetObjectPtrLabel)
       && FindProcShort (glGetPointerv)
       && FindProcShort (glEnablei)
       && FindProcShort (glDisablei)
       && FindProcShort (glBlendEquationi)
       && FindProcShort (glBlendEquationSeparatei)
       && FindProcShort (glBlendFunci)
       && FindProcShort (glBlendFuncSeparatei)
       && FindProcShort (glColorMaski)
       && FindProcShort (glIsEnabledi)
       && FindProcShort (glDrawElementsBaseVertex)
       && FindProcShort (glDrawRangeElementsBaseVertex)
       && FindProcShort (glDrawElementsInstancedBaseVertex)
       && FindProcShort (glFramebufferTexture)
       && FindProcShort (glPrimitiveBoundingBox)
       && FindProcShort (glGetGraphicsResetStatus)
       && FindProcShort (glReadnPixels)
       && FindProcShort (glGetnUniformfv)
       && FindProcShort (glGetnUniformiv)
       && FindProcShort (glGetnUniformuiv)
       && FindProcShort (glMinSampleShading)
       && FindProcShort (glPatchParameteri)
       && FindProcShort (glTexParameterIiv)
       && FindProcShort (glTexParameterIuiv)
       && FindProcShort (glGetTexParameterIiv)
       && FindProcShort (glGetTexParameterIuiv)
       && FindProcShort (glSamplerParameterIiv)
       && FindProcShort (glSamplerParameterIuiv)
       && FindProcShort (glGetSamplerParameterIiv)
       && FindProcShort (glGetSamplerParameterIuiv)
       && FindProcShort (glTexBufferRange)
       && FindProcShort (glTexStorage3DMultisample);
  if (!has32es)
  {
    theCtx.checkWrongVersion (3, 2, aLastFailedProc);
  }

  theCtx.arbTboRGB32 = isGlGreaterEqualShort (3, 2); // OpenGL ES 3.2 introduces TBO already supporting RGB32 format
  theCtx.extDrawBuffers = checkExtensionShort ("GL_EXT_draw_buffers") && theCtx.FindProc ("glDrawBuffersEXT", this->glDrawBuffers);
  theCtx.arbDrawBuffers = checkExtensionShort ("GL_ARB_draw_buffers") && theCtx.FindProc ("glDrawBuffersARB", this->glDrawBuffers);

  if (isGlGreaterEqualShort (3, 0) && FindProcShort (glDrawBuffers))
  {
    theCtx.hasDrawBuffers = OpenGl_FeatureInCore;
  }
  else if (theCtx.extDrawBuffers || theCtx.arbDrawBuffers)
  {
    theCtx.hasDrawBuffers = OpenGl_FeatureInExtensions;
  }

  // float textures available since OpenGL ES 3.0+,
  // but renderable only since 3.2+ or with extension
  theCtx.hasFloatBuffer = theCtx.hasHalfFloatBuffer = OpenGl_FeatureNotAvailable;
  if (isGlGreaterEqualShort (3, 2))
  {
    theCtx.hasFloatBuffer = theCtx.hasHalfFloatBuffer = OpenGl_FeatureInCore;
  }
  else
  {
    if (checkExtensionShort ("GL_EXT_color_buffer_float"))
    {
      theCtx.hasFloatBuffer = isGlGreaterEqualShort (3, 0) ? OpenGl_FeatureInCore : OpenGl_FeatureInExtensions;
    }
    if (checkExtensionShort ("GL_EXT_color_buffer_half_float"))
    {
      // GL_HALF_FLOAT_OES for OpenGL ES 2.0 and GL_HALF_FLOAT for OpenGL ES 3.0+
      theCtx.hasHalfFloatBuffer = isGlGreaterEqualShort (3, 0) ? OpenGl_FeatureInCore : OpenGl_FeatureInExtensions;
    }
  }

  theCtx.oesSampleVariables = checkExtensionShort ("GL_OES_sample_variables");
  theCtx.oesStdDerivatives  = checkExtensionShort ("GL_OES_standard_derivatives");
  theCtx.hasSampleVariables = isGlGreaterEqualShort (3, 2) ? OpenGl_FeatureInCore :
                              theCtx.oesSampleVariables ? OpenGl_FeatureInExtensions
                                                        : OpenGl_FeatureNotAvailable;
  theCtx.hasGlslBitwiseOps = isGlGreaterEqualShort (3, 0)
                           ? OpenGl_FeatureInCore
                           : OpenGl_FeatureNotAvailable;
  // without hasHighp, dFdx/dFdy precision is considered too low for flat shading (visual artifacts)
  theCtx.hasFlatShading = isGlGreaterEqualShort (3, 0)
                        ? OpenGl_FeatureInCore
                         : (theCtx.oesStdDerivatives && theCtx.hasHighp
                          ? OpenGl_FeatureInExtensions
                          : OpenGl_FeatureNotAvailable);
  if (!isGlGreaterEqualShort (3, 1)
    && theCtx.Vendor().Search("qualcomm") != -1)
  {
    // dFdx/dFdy are completely broken on tested Adreno devices with versions below OpenGl ES 3.1
    theCtx.hasFlatShading = OpenGl_FeatureNotAvailable;
  }

  theCtx.hasGeometryStage = isGlGreaterEqualShort (3, 2)
                   ? OpenGl_FeatureInCore
                   : (checkExtensionShort ("GL_EXT_geometry_shader") && checkExtensionShort ("GL_EXT_shader_io_blocks")
                     ? OpenGl_FeatureInExtensions
                     : OpenGl_FeatureNotAvailable);
#else

  theCtx.hasTexRGBA8 = true;
  theCtx.hasTexSRGB       = isGlGreaterEqualShort (2, 1);
  theCtx.hasFboSRGB       = isGlGreaterEqualShort (2, 1);
  theCtx.hasSRGBControl   = theCtx.hasFboSRGB;
  theCtx.hasFboRenderMipmap = true;
  theCtx.arbDrawBuffers   = checkExtensionShort ("GL_ARB_draw_buffers");
  theCtx.arbNPTW          = checkExtensionShort ("GL_ARB_texture_non_power_of_two");
  theCtx.arbTexFloat      = isGlGreaterEqualShort (3, 0)
                  || checkExtensionShort ("GL_ARB_texture_float");
  theCtx.hasTexFloatLinear = theCtx.arbTexFloat;
  theCtx.arbSampleShading = checkExtensionShort ("GL_ARB_sample_shading");
  theCtx.arbDepthClamp    = isGlGreaterEqualShort (3, 2)
                  || checkExtensionShort ("GL_ARB_depth_clamp")
                  || checkExtensionShort ("NV_depth_clamp");
  theCtx.extBgra          = isGlGreaterEqualShort (1, 2)
                  || checkExtensionShort ("GL_EXT_bgra");
  theCtx.extAnis = checkExtensionShort ("GL_EXT_texture_filter_anisotropic");
  theCtx.extPDS  = checkExtensionShort ("GL_EXT_packed_depth_stencil");
  theCtx.atiMem  = checkExtensionShort ("GL_ATI_meminfo");
  theCtx.nvxMem  = checkExtensionShort ("GL_NVX_gpu_memory_info");

  theCtx.hasDrawBuffers = isGlGreaterEqualShort (2, 0) ? OpenGl_FeatureInCore :
                          theCtx.arbDrawBuffers ? OpenGl_FeatureInExtensions 
                                                : OpenGl_FeatureNotAvailable;

  theCtx.hasGlslBitwiseOps = isGlGreaterEqualShort (3, 0)
                           ? OpenGl_FeatureInCore
                           : checkExtensionShort ("GL_EXT_gpu_shader4")
                            ? OpenGl_FeatureInExtensions
                            : OpenGl_FeatureNotAvailable;

  theCtx.hasFloatBuffer = theCtx.hasHalfFloatBuffer =
    isGlGreaterEqualShort (3, 0) ? OpenGl_FeatureInCore :
    checkExtensionShort ("GL_ARB_color_buffer_float") ? OpenGl_FeatureInExtensions
                                                      : OpenGl_FeatureNotAvailable;

  theCtx.hasGeometryStage = isGlGreaterEqualShort (3, 2)
                          ? OpenGl_FeatureInCore
                          : OpenGl_FeatureNotAvailable;

  theCtx.hasSampleVariables = isGlGreaterEqualShort (4, 0) ? OpenGl_FeatureInCore :
                              theCtx.arbSampleShading ? OpenGl_FeatureInExtensions
                                                      : OpenGl_FeatureNotAvailable;

  bool has12 = false, has13 = false, has14 = false, has15 = false;
  bool has20 = false, has21 = false;
  bool has30 = false, has31 = false, has32 = false, has33 = false;
  bool has40 = false, has41 = false, has42 = false, has43 = false, has44 = false, has45 = false, has46 = false;

  // retrieve platform-dependent extensions
#if defined(HAVE_EGL)
  //
#elif defined(_WIN32)
  if (FindProcShort (wglGetExtensionsStringARB))
  {
    const char* aWglExts = this->wglGetExtensionsStringARB (wglGetCurrentDC());
    if (checkExtensionShort (aWglExts, "WGL_EXT_swap_control"))
    {
      FindProcShort (wglSwapIntervalEXT);
    }
    if (checkExtensionShort (aWglExts, "WGL_ARB_pixel_format"))
    {
      FindProcShort (wglChoosePixelFormatARB);
    }
    if (checkExtensionShort (aWglExts, "WGL_ARB_create_context_profile"))
    {
      FindProcShort (wglCreateContextAttribsARB);
    }
    if (checkExtensionShort (aWglExts, "WGL_NV_DX_interop"))
    {
      FindProcShort (wglDXSetResourceShareHandleNV);
      FindProcShort (wglDXOpenDeviceNV);
      FindProcShort (wglDXCloseDeviceNV);
      FindProcShort (wglDXRegisterObjectNV);
      FindProcShort (wglDXUnregisterObjectNV);
      FindProcShort (wglDXObjectAccessNV);
      FindProcShort (wglDXLockObjectsNV);
      FindProcShort (wglDXUnlockObjectsNV);
    }
    if (checkExtensionShort (aWglExts, "WGL_AMD_gpu_association"))
    {
      FindProcShort (wglGetGPUIDsAMD);
      FindProcShort (wglGetGPUInfoAMD);
      FindProcShort (wglGetContextGPUIDAMD);
    }
  }
#elif defined(HAVE_XLIB)
    const char* aGlxExts = ::glXQueryExtensionsString ((Display* )theCtx.myDisplay, DefaultScreen ((Display* )theCtx.myDisplay));
    if (checkExtensionShort (aGlxExts, "GLX_EXT_swap_control"))
    {
      FindProcShort (glXSwapIntervalEXT);
    }
    if (checkExtensionShort (aGlxExts, "GLX_SGI_swap_control"))
    {
      FindProcShort (glXSwapIntervalSGI);
    }
    if (checkExtensionShort (aGlxExts, "GLX_MESA_query_renderer"))
    {
      FindProcShort (glXQueryRendererIntegerMESA);
      FindProcShort (glXQueryCurrentRendererIntegerMESA);
      FindProcShort (glXQueryRendererStringMESA);
      FindProcShort (glXQueryCurrentRendererStringMESA);
    }
    //extSwapTear = checkExtensionShort (aGlxExts, "GLX_EXT_swap_control_tear");
#endif

  // load OpenGL 1.2 new functions
  has12 = isGlGreaterEqualShort (1, 2)
       && FindProcShort (glBlendColor)
       && FindProcShort (glBlendEquation)
       && FindProcShort (glDrawRangeElements)
       && FindProcShort (glTexImage3D)
       && FindProcShort (glTexSubImage3D)
       && FindProcShort (glCopyTexSubImage3D);
  if (!has12)
  {
    theCtx.checkWrongVersion (1, 2, aLastFailedProc);
  }

  // load OpenGL 1.3 new functions
  has13 = isGlGreaterEqualShort (1, 3)
       && FindProcShort (glActiveTexture)
       && FindProcShort (glSampleCoverage)
       && FindProcShort (glCompressedTexImage3D)
       && FindProcShort (glCompressedTexImage2D)
       && FindProcShort (glCompressedTexImage1D)
       && FindProcShort (glCompressedTexSubImage3D)
       && FindProcShort (glCompressedTexSubImage2D)
       && FindProcShort (glCompressedTexSubImage1D)
       && FindProcShort (glGetCompressedTexImage);
  if (!has13)
  {
    theCtx.checkWrongVersion (1, 3, aLastFailedProc);
  }

  // load OpenGL 1.4 new functions
  has14 = isGlGreaterEqualShort (1, 4)
       && FindProcShort (glBlendFuncSeparate)
       && FindProcShort (glMultiDrawArrays)
       && FindProcShort (glMultiDrawElements)
       && FindProcShort (glPointParameterf)
       && FindProcShort (glPointParameterfv)
       && FindProcShort (glPointParameteri)
       && FindProcShort (glPointParameteriv);
  if (!has14)
  {
    theCtx.checkWrongVersion (1, 4, aLastFailedProc);
  }

  // load OpenGL 1.5 new functions
  has15 = isGlGreaterEqualShort (1, 5)
       && FindProcShort (glGenQueries)
       && FindProcShort (glDeleteQueries)
       && FindProcShort (glIsQuery)
       && FindProcShort (glBeginQuery)
       && FindProcShort (glEndQuery)
       && FindProcShort (glGetQueryiv)
       && FindProcShort (glGetQueryObjectiv)
       && FindProcShort (glGetQueryObjectuiv)
       && FindProcShort (glBindBuffer)
       && FindProcShort (glDeleteBuffers)
       && FindProcShort (glGenBuffers)
       && FindProcShort (glIsBuffer)
       && FindProcShort (glBufferData)
       && FindProcShort (glBufferSubData)
       && FindProcShort (glGetBufferSubData)
       && FindProcShort (glMapBuffer)
       && FindProcShort (glUnmapBuffer)
       && FindProcShort (glGetBufferParameteriv)
       && FindProcShort (glGetBufferPointerv);
  if (has15)
  {
    theCtx.core15    = (OpenGl_GlCore15* )this;
    theCtx.core15fwd = (OpenGl_GlCore15* )this;
    theCtx.hasGetBufferData = true;
  }
  else
  {
    theCtx.checkWrongVersion (1, 5, aLastFailedProc);
  }

  // load OpenGL 2.0 new functions
  has20 = isGlGreaterEqualShort (2, 0)
       && FindProcShort (glBlendEquationSeparate)
       && FindProcShort (glDrawBuffers)
       && FindProcShort (glStencilOpSeparate)
       && FindProcShort (glStencilFuncSeparate)
       && FindProcShort (glStencilMaskSeparate)
       && FindProcShort (glAttachShader)
       && FindProcShort (glBindAttribLocation)
       && FindProcShort (glCompileShader)
       && FindProcShort (glCreateProgram)
       && FindProcShort (glCreateShader)
       && FindProcShort (glDeleteProgram)
       && FindProcShort (glDeleteShader)
       && FindProcShort (glDetachShader)
       && FindProcShort (glDisableVertexAttribArray)
       && FindProcShort (glEnableVertexAttribArray)
       && FindProcShort (glGetActiveAttrib)
       && FindProcShort (glGetActiveUniform)
       && FindProcShort (glGetAttachedShaders)
       && FindProcShort (glGetAttribLocation)
       && FindProcShort (glGetProgramiv)
       && FindProcShort (glGetProgramInfoLog)
       && FindProcShort (glGetShaderiv)
       && FindProcShort (glGetShaderInfoLog)
       && FindProcShort (glGetShaderSource)
       && FindProcShort (glGetUniformLocation)
       && FindProcShort (glGetUniformfv)
       && FindProcShort (glGetUniformiv)
       && FindProcShort (glGetVertexAttribdv)
       && FindProcShort (glGetVertexAttribfv)
       && FindProcShort (glGetVertexAttribiv)
       && FindProcShort (glGetVertexAttribPointerv)
       && FindProcShort (glIsProgram)
       && FindProcShort (glIsShader)
       && FindProcShort (glLinkProgram)
       && FindProcShort (glShaderSource)
       && FindProcShort (glUseProgram)
       && FindProcShort (glUniform1f)
       && FindProcShort (glUniform2f)
       && FindProcShort (glUniform3f)
       && FindProcShort (glUniform4f)
       && FindProcShort (glUniform1i)
       && FindProcShort (glUniform2i)
       && FindProcShort (glUniform3i)
       && FindProcShort (glUniform4i)
       && FindProcShort (glUniform1fv)
       && FindProcShort (glUniform2fv)
       && FindProcShort (glUniform3fv)
       && FindProcShort (glUniform4fv)
       && FindProcShort (glUniform1iv)
       && FindProcShort (glUniform2iv)
       && FindProcShort (glUniform3iv)
       && FindProcShort (glUniform4iv)
       && FindProcShort (glUniformMatrix2fv)
       && FindProcShort (glUniformMatrix3fv)
       && FindProcShort (glUniformMatrix4fv)
       && FindProcShort (glValidateProgram)
       && FindProcShort (glVertexAttrib1d)
       && FindProcShort (glVertexAttrib1dv)
       && FindProcShort (glVertexAttrib1f)
       && FindProcShort (glVertexAttrib1fv)
       && FindProcShort (glVertexAttrib1s)
       && FindProcShort (glVertexAttrib1sv)
       && FindProcShort (glVertexAttrib2d)
       && FindProcShort (glVertexAttrib2dv)
       && FindProcShort (glVertexAttrib2f)
       && FindProcShort (glVertexAttrib2fv)
       && FindProcShort (glVertexAttrib2s)
       && FindProcShort (glVertexAttrib2sv)
       && FindProcShort (glVertexAttrib3d)
       && FindProcShort (glVertexAttrib3dv)
       && FindProcShort (glVertexAttrib3f)
       && FindProcShort (glVertexAttrib3fv)
       && FindProcShort (glVertexAttrib3s)
       && FindProcShort (glVertexAttrib3sv)
       && FindProcShort (glVertexAttrib4Nbv)
       && FindProcShort (glVertexAttrib4Niv)
       && FindProcShort (glVertexAttrib4Nsv)
       && FindProcShort (glVertexAttrib4Nub)
       && FindProcShort (glVertexAttrib4Nubv)
       && FindProcShort (glVertexAttrib4Nuiv)
       && FindProcShort (glVertexAttrib4Nusv)
       && FindProcShort (glVertexAttrib4bv)
       && FindProcShort (glVertexAttrib4d)
       && FindProcShort (glVertexAttrib4dv)
       && FindProcShort (glVertexAttrib4f)
       && FindProcShort (glVertexAttrib4fv)
       && FindProcShort (glVertexAttrib4iv)
       && FindProcShort (glVertexAttrib4s)
       && FindProcShort (glVertexAttrib4sv)
       && FindProcShort (glVertexAttrib4ubv)
       && FindProcShort (glVertexAttrib4uiv)
       && FindProcShort (glVertexAttrib4usv)
       && FindProcShort (glVertexAttribPointer);
  if (has20)
  {
    const char* aGlslVer = (const char* )::glGetString (GL_SHADING_LANGUAGE_VERSION);
    if (aGlslVer == NULL
    || *aGlslVer == '\0')
    {
      // broken context has been detected
      theCtx.checkWrongVersion (2, 0, "GLSL 1.1");
    }
    else
    {
      theCtx.core20    = (OpenGl_GlCore20* )this;
      theCtx.core20fwd = (OpenGl_GlCore20* )this;
    }
  }
  else
  {
    theCtx.checkWrongVersion (2, 0, aLastFailedProc);
  }

  // load OpenGL 2.1 new functions
  has21 = isGlGreaterEqualShort (2, 1)
       && FindProcShort (glUniformMatrix2x3fv)
       && FindProcShort (glUniformMatrix3x2fv)
       && FindProcShort (glUniformMatrix2x4fv)
       && FindProcShort (glUniformMatrix4x2fv)
       && FindProcShort (glUniformMatrix3x4fv)
       && FindProcShort (glUniformMatrix4x3fv);
  if (!has21)
  {
    theCtx.checkWrongVersion (2, 1, aLastFailedProc);
  }

  // load GL_ARB_framebuffer_object (added to OpenGL 3.0 core)
  const bool hasFBO = (isGlGreaterEqualShort (3, 0) || checkExtensionShort ("GL_ARB_framebuffer_object"))
       && FindProcShort (glIsRenderbuffer)
       && FindProcShort (glBindRenderbuffer)
       && FindProcShort (glDeleteRenderbuffers)
       && FindProcShort (glGenRenderbuffers)
       && FindProcShort (glRenderbufferStorage)
       && FindProcShort (glGetRenderbufferParameteriv)
       && FindProcShort (glIsFramebuffer)
       && FindProcShort (glBindFramebuffer)
       && FindProcShort (glDeleteFramebuffers)
       && FindProcShort (glGenFramebuffers)
       && FindProcShort (glCheckFramebufferStatus)
       && FindProcShort (glFramebufferTexture1D)
       && FindProcShort (glFramebufferTexture2D)
       && FindProcShort (glFramebufferTexture3D)
       && FindProcShort (glFramebufferRenderbuffer)
       && FindProcShort (glGetFramebufferAttachmentParameteriv)
       && FindProcShort (glGenerateMipmap)
       && FindProcShort (glBlitFramebuffer)
       && FindProcShort (glRenderbufferStorageMultisample)
       && FindProcShort (glFramebufferTextureLayer);

  // load GL_ARB_vertex_array_object (added to OpenGL 3.0 core)
  const bool hasVAO = (isGlGreaterEqualShort (3, 0) || checkExtensionShort ("GL_ARB_vertex_array_object"))
       && FindProcShort (glBindVertexArray)
       && FindProcShort (glDeleteVertexArrays)
       && FindProcShort (glGenVertexArrays)
       && FindProcShort (glIsVertexArray);

  // load GL_ARB_map_buffer_range (added to OpenGL 3.0 core)
  const bool hasMapBufferRange = (isGlGreaterEqualShort (3, 0) || checkExtensionShort ("GL_ARB_map_buffer_range"))
       && FindProcShort (glMapBufferRange)
       && FindProcShort (glFlushMappedBufferRange);

  // load OpenGL 3.0 new functions
  has30 = isGlGreaterEqualShort (3, 0)
       && hasFBO
       && hasVAO
       && hasMapBufferRange
       && FindProcShort (glColorMaski)
       && FindProcShort (glGetBooleani_v)
       && FindProcShort (glGetIntegeri_v)
       && FindProcShort (glEnablei)
       && FindProcShort (glDisablei)
       && FindProcShort (glIsEnabledi)
       && FindProcShort (glBeginTransformFeedback)
       && FindProcShort (glEndTransformFeedback)
       && FindProcShort (glBindBufferRange)
       && FindProcShort (glBindBufferBase)
       && FindProcShort (glTransformFeedbackVaryings)
       && FindProcShort (glGetTransformFeedbackVarying)
       && FindProcShort (glClampColor)
       && FindProcShort (glBeginConditionalRender)
       && FindProcShort (glEndConditionalRender)
       && FindProcShort (glVertexAttribIPointer)
       && FindProcShort (glGetVertexAttribIiv)
       && FindProcShort (glGetVertexAttribIuiv)
       && FindProcShort (glVertexAttribI1i)
       && FindProcShort (glVertexAttribI2i)
       && FindProcShort (glVertexAttribI3i)
       && FindProcShort (glVertexAttribI4i)
       && FindProcShort (glVertexAttribI1ui)
       && FindProcShort (glVertexAttribI2ui)
       && FindProcShort (glVertexAttribI3ui)
       && FindProcShort (glVertexAttribI4ui)
       && FindProcShort (glVertexAttribI1iv)
       && FindProcShort (glVertexAttribI2iv)
       && FindProcShort (glVertexAttribI3iv)
       && FindProcShort (glVertexAttribI4iv)
       && FindProcShort (glVertexAttribI1uiv)
       && FindProcShort (glVertexAttribI2uiv)
       && FindProcShort (glVertexAttribI3uiv)
       && FindProcShort (glVertexAttribI4uiv)
       && FindProcShort (glVertexAttribI4bv)
       && FindProcShort (glVertexAttribI4sv)
       && FindProcShort (glVertexAttribI4ubv)
       && FindProcShort (glVertexAttribI4usv)
       && FindProcShort (glGetUniformuiv)
       && FindProcShort (glBindFragDataLocation)
       && FindProcShort (glGetFragDataLocation)
       && FindProcShort (glUniform1ui)
       && FindProcShort (glUniform2ui)
       && FindProcShort (glUniform3ui)
       && FindProcShort (glUniform4ui)
       && FindProcShort (glUniform1uiv)
       && FindProcShort (glUniform2uiv)
       && FindProcShort (glUniform3uiv)
       && FindProcShort (glUniform4uiv)
       && FindProcShort (glTexParameterIiv)
       && FindProcShort (glTexParameterIuiv)
       && FindProcShort (glGetTexParameterIiv)
       && FindProcShort (glGetTexParameterIuiv)
       && FindProcShort (glClearBufferiv)
       && FindProcShort (glClearBufferuiv)
       && FindProcShort (glClearBufferfv)
       && FindProcShort (glClearBufferfi)
       && FindProcShort (glGetStringi);
  if (!has30)
  {
    theCtx.checkWrongVersion (3, 0, aLastFailedProc);
  }

  // load GL_ARB_uniform_buffer_object (added to OpenGL 3.1 core)
  const bool hasUBO = (isGlGreaterEqualShort (3, 1) || checkExtensionShort ("GL_ARB_uniform_buffer_object"))
       && FindProcShort (glGetUniformIndices)
       && FindProcShort (glGetActiveUniformsiv)
       && FindProcShort (glGetActiveUniformName)
       && FindProcShort (glGetUniformBlockIndex)
       && FindProcShort (glGetActiveUniformBlockiv)
       && FindProcShort (glGetActiveUniformBlockName)
       && FindProcShort (glUniformBlockBinding);

  // load GL_ARB_copy_buffer (added to OpenGL 3.1 core)
  const bool hasCopyBufSubData = (isGlGreaterEqualShort (3, 1) || checkExtensionShort ("GL_ARB_copy_buffer"))
       && FindProcShort (glCopyBufferSubData);

  if (has30)
  {
    // NPOT textures are required by OpenGL 2.0 specifications
    // but doesn't hardware accelerated by some ancient OpenGL 2.1 hardware (GeForce FX, RadeOn 9700 etc.)
    theCtx.arbNPTW  = true;
    theCtx.arbTexRG = true;

    theCtx.core30 = (OpenGl_GlCore30* )this;
  }

  // load OpenGL 3.1 new functions
  has31 = isGlGreaterEqualShort (3, 1)
       && hasUBO
       && hasCopyBufSubData
       && FindProcShort (glDrawArraysInstanced)
       && FindProcShort (glDrawElementsInstanced)
       && FindProcShort (glTexBuffer)
       && FindProcShort (glPrimitiveRestartIndex);
  if (has31)
  {
    theCtx.arbTBO = (OpenGl_ArbTBO* )this;
    theCtx.arbIns = (OpenGl_ArbIns* )this;
  }
  else
  {
    theCtx.checkWrongVersion (3, 1, aLastFailedProc);

    // initialize TBO extension (ARB)
    if (checkExtensionShort ("GL_ARB_texture_buffer_object")
     && theCtx.FindProc ("glTexBufferARB", this->glTexBuffer))
    {
      theCtx.arbTBO = (OpenGl_ArbTBO* )this;
    }

    // initialize hardware instancing extension (ARB)
    if (checkExtensionShort ("GL_ARB_draw_instanced")
     && theCtx.FindProc ("glDrawArraysInstancedARB",   this->glDrawArraysInstanced)
     && theCtx.FindProc ("glDrawElementsInstancedARB", this->glDrawElementsInstanced))
    {
      theCtx.arbIns = (OpenGl_ArbIns* )this;
    }
  }

  theCtx.arbTboRGB32 = checkExtensionShort ("GL_ARB_texture_buffer_object_rgb32");

  // load GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)
  const bool hasDrawElemsBaseVert = (isGlGreaterEqualShort (3, 2) || checkExtensionShort ("GL_ARB_draw_elements_base_vertex"))
       && FindProcShort (glDrawElementsBaseVertex)
       && FindProcShort (glDrawRangeElementsBaseVertex)
       && FindProcShort (glDrawElementsInstancedBaseVertex)
       && FindProcShort (glMultiDrawElementsBaseVertex);

  // load GL_ARB_provoking_vertex (added to OpenGL 3.2 core)
  const bool hasProvokingVert = (isGlGreaterEqualShort (3, 2) || checkExtensionShort ("GL_ARB_provoking_vertex"))
       && FindProcShort (glProvokingVertex);

  // load GL_ARB_sync (added to OpenGL 3.2 core)
  const bool hasSync = (isGlGreaterEqualShort (3, 2) || checkExtensionShort ("GL_ARB_sync"))
       && FindProcShort (glFenceSync)
       && FindProcShort (glIsSync)
       && FindProcShort (glDeleteSync)
       && FindProcShort (glClientWaitSync)
       && FindProcShort (glWaitSync)
       && FindProcShort (glGetInteger64v)
       && FindProcShort (glGetSynciv);

  // load GL_ARB_texture_multisample (added to OpenGL 3.2 core)
  const bool hasTextureMultisample = (isGlGreaterEqualShort (3, 2) || checkExtensionShort ("GL_ARB_texture_multisample"))
       && FindProcShort (glTexImage2DMultisample)
       && FindProcShort (glTexImage3DMultisample)
       && FindProcShort (glGetMultisamplefv)
       && FindProcShort (glSampleMaski);

  // load OpenGL 3.2 new functions
  has32 = isGlGreaterEqualShort (3, 2)
       && hasDrawElemsBaseVert
       && hasProvokingVert
       && hasSync
       && hasTextureMultisample
       && FindProcShort (glGetInteger64i_v)
       && FindProcShort (glGetBufferParameteri64v)
       && FindProcShort (glFramebufferTexture);
  if (has32)
  {
    theCtx.core32 = (OpenGl_GlCore32* )this;
  }
  else
  {
    theCtx.checkWrongVersion (3, 2, aLastFailedProc);
  }

  // load GL_ARB_blend_func_extended (added to OpenGL 3.3 core)
  const bool hasBlendFuncExtended = (isGlGreaterEqualShort (3, 3) || checkExtensionShort ("GL_ARB_blend_func_extended"))
       && FindProcShort (glBindFragDataLocationIndexed)
       && FindProcShort (glGetFragDataIndex);

  // load GL_ARB_sampler_objects (added to OpenGL 3.3 core)
  const bool hasSamplerObjects = (isGlGreaterEqualShort (3, 3) || checkExtensionShort ("GL_ARB_sampler_objects"))
       && FindProcShort (glGenSamplers)
       && FindProcShort (glDeleteSamplers)
       && FindProcShort (glIsSampler)
       && FindProcShort (glBindSampler)
       && FindProcShort (glSamplerParameteri)
       && FindProcShort (glSamplerParameteriv)
       && FindProcShort (glSamplerParameterf)
       && FindProcShort (glSamplerParameterfv)
       && FindProcShort (glSamplerParameterIiv)
       && FindProcShort (glSamplerParameterIuiv)
       && FindProcShort (glGetSamplerParameteriv)
       && FindProcShort (glGetSamplerParameterIiv)
       && FindProcShort (glGetSamplerParameterfv)
       && FindProcShort (glGetSamplerParameterIuiv);
  if (hasSamplerObjects)
  {
    theCtx.arbSamplerObject = (OpenGl_ArbSamplerObject* )this;
  }

  // load GL_ARB_timer_query (added to OpenGL 3.3 core)
  const bool hasTimerQuery = (isGlGreaterEqualShort (3, 3) || checkExtensionShort ("GL_ARB_timer_query"))
       && FindProcShort (glQueryCounter)
       && FindProcShort (glGetQueryObjecti64v)
       && FindProcShort (glGetQueryObjectui64v);

  // load GL_ARB_vertex_type_2_10_10_10_rev (added to OpenGL 3.3 core)
  const bool hasVertType21010101rev = (isGlGreaterEqualShort (3, 3) || checkExtensionShort ("GL_ARB_vertex_type_2_10_10_10_rev"))
       && FindProcShort (glVertexAttribP1ui)
       && FindProcShort (glVertexAttribP1uiv)
       && FindProcShort (glVertexAttribP2ui)
       && FindProcShort (glVertexAttribP2uiv)
       && FindProcShort (glVertexAttribP3ui)
       && FindProcShort (glVertexAttribP3uiv)
       && FindProcShort (glVertexAttribP4ui)
       && FindProcShort (glVertexAttribP4uiv);

  // load OpenGL 3.3 extra functions
  has33 = isGlGreaterEqualShort (3, 3)
       && hasBlendFuncExtended
       && hasSamplerObjects
       && hasTimerQuery
       && hasVertType21010101rev
       && FindProcShort (glVertexAttribDivisor);
  if (has33)
  {
    theCtx.core33 = (OpenGl_GlCore33* )this;
  }
  else
  {
    theCtx.checkWrongVersion (3, 3, aLastFailedProc);
  }

  // load GL_ARB_draw_indirect (added to OpenGL 4.0 core)
  const bool hasDrawIndirect = (isGlGreaterEqualShort (4, 0) || checkExtensionShort ("GL_ARB_draw_indirect"))
       && FindProcShort (glDrawArraysIndirect)
       && FindProcShort (glDrawElementsIndirect);

  // load GL_ARB_gpu_shader_fp64 (added to OpenGL 4.0 core)
  const bool hasShaderFP64 = (isGlGreaterEqualShort (4, 0) || checkExtensionShort ("GL_ARB_gpu_shader_fp64"))
       && FindProcShort (glUniform1d)
       && FindProcShort (glUniform2d)
       && FindProcShort (glUniform3d)
       && FindProcShort (glUniform4d)
       && FindProcShort (glUniform1dv)
       && FindProcShort (glUniform2dv)
       && FindProcShort (glUniform3dv)
       && FindProcShort (glUniform4dv)
       && FindProcShort (glUniformMatrix2dv)
       && FindProcShort (glUniformMatrix3dv)
       && FindProcShort (glUniformMatrix4dv)
       && FindProcShort (glUniformMatrix2x3dv)
       && FindProcShort (glUniformMatrix2x4dv)
       && FindProcShort (glUniformMatrix3x2dv)
       && FindProcShort (glUniformMatrix3x4dv)
       && FindProcShort (glUniformMatrix4x2dv)
       && FindProcShort (glUniformMatrix4x3dv)
       && FindProcShort (glGetUniformdv);

  // load GL_ARB_shader_subroutine (added to OpenGL 4.0 core)
  const bool hasShaderSubroutine = (isGlGreaterEqualShort (4, 0) || checkExtensionShort ("GL_ARB_shader_subroutine"))
       && FindProcShort (glGetSubroutineUniformLocation)
       && FindProcShort (glGetSubroutineIndex)
       && FindProcShort (glGetActiveSubroutineUniformiv)
       && FindProcShort (glGetActiveSubroutineUniformName)
       && FindProcShort (glGetActiveSubroutineName)
       && FindProcShort (glUniformSubroutinesuiv)
       && FindProcShort (glGetUniformSubroutineuiv)
       && FindProcShort (glGetProgramStageiv);

  // load GL_ARB_tessellation_shader (added to OpenGL 4.0 core)
  const bool hasTessellationShader = (isGlGreaterEqualShort (4, 0) || checkExtensionShort ("GL_ARB_tessellation_shader"))
       && FindProcShort (glPatchParameteri)
       && FindProcShort (glPatchParameterfv);

  // load GL_ARB_transform_feedback2 (added to OpenGL 4.0 core)
  const bool hasTrsfFeedback2 = (isGlGreaterEqualShort (4, 0) || checkExtensionShort ("GL_ARB_transform_feedback2"))
       && FindProcShort (glBindTransformFeedback)
       && FindProcShort (glDeleteTransformFeedbacks)
       && FindProcShort (glGenTransformFeedbacks)
       && FindProcShort (glIsTransformFeedback)
       && FindProcShort (glPauseTransformFeedback)
       && FindProcShort (glResumeTransformFeedback)
       && FindProcShort (glDrawTransformFeedback);

  // load GL_ARB_transform_feedback3 (added to OpenGL 4.0 core)
  const bool hasTrsfFeedback3 = (isGlGreaterEqualShort (4, 0) || checkExtensionShort ("GL_ARB_transform_feedback3"))
       && FindProcShort (glDrawTransformFeedbackStream)
       && FindProcShort (glBeginQueryIndexed)
       && FindProcShort (glEndQueryIndexed)
       && FindProcShort (glGetQueryIndexediv);

  // load OpenGL 4.0 new functions
  has40 = isGlGreaterEqualShort (4, 0)
      && hasDrawIndirect
      && hasShaderFP64
      && hasShaderSubroutine
      && hasTessellationShader
      && hasTrsfFeedback2
      && hasTrsfFeedback3
      && FindProcShort (glMinSampleShading)
      && FindProcShort (glBlendEquationi)
      && FindProcShort (glBlendEquationSeparatei)
      && FindProcShort (glBlendFunci)
      && FindProcShort (glBlendFuncSeparatei);
  if (has40)
  {
    theCtx.arbTboRGB32 = true; // in core since OpenGL 4.0
  }
  else
  {
    theCtx.checkWrongVersion (4, 0, aLastFailedProc);
  }

  // load GL_ARB_ES2_compatibility (added to OpenGL 4.1 core)
  const bool hasES2Compatibility = (isGlGreaterEqualShort (4, 1) || checkExtensionShort ("GL_ARB_ES2_compatibility"))
       && FindProcShort (glReleaseShaderCompiler)
       && FindProcShort (glShaderBinary)
       && FindProcShort (glGetShaderPrecisionFormat)
       && FindProcShort (glDepthRangef)
       && FindProcShort (glClearDepthf);

  // load GL_ARB_get_program_binary (added to OpenGL 4.1 core)
  const bool hasGetProgramBinary = (isGlGreaterEqualShort (4, 1) || checkExtensionShort ("GL_ARB_get_program_binary"))
       && FindProcShort (glGetProgramBinary)
       && FindProcShort (glProgramBinary)
       && FindProcShort (glProgramParameteri);


  // load GL_ARB_separate_shader_objects (added to OpenGL 4.1 core)
  const bool hasSeparateShaderObjects = (isGlGreaterEqualShort (4, 1) || checkExtensionShort ("GL_ARB_separate_shader_objects"))
       && FindProcShort (glUseProgramStages)
       && FindProcShort (glActiveShaderProgram)
       && FindProcShort (glCreateShaderProgramv)
       && FindProcShort (glBindProgramPipeline)
       && FindProcShort (glDeleteProgramPipelines)
       && FindProcShort (glGenProgramPipelines)
       && FindProcShort (glIsProgramPipeline)
       && FindProcShort (glGetProgramPipelineiv)
       && FindProcShort (glProgramUniform1i)
       && FindProcShort (glProgramUniform1iv)
       && FindProcShort (glProgramUniform1f)
       && FindProcShort (glProgramUniform1fv)
       && FindProcShort (glProgramUniform1d)
       && FindProcShort (glProgramUniform1dv)
       && FindProcShort (glProgramUniform1ui)
       && FindProcShort (glProgramUniform1uiv)
       && FindProcShort (glProgramUniform2i)
       && FindProcShort (glProgramUniform2iv)
       && FindProcShort (glProgramUniform2f)
       && FindProcShort (glProgramUniform2fv)
       && FindProcShort (glProgramUniform2d)
       && FindProcShort (glProgramUniform2dv)
       && FindProcShort (glProgramUniform2ui)
       && FindProcShort (glProgramUniform2uiv)
       && FindProcShort (glProgramUniform3i)
       && FindProcShort (glProgramUniform3iv)
       && FindProcShort (glProgramUniform3f)
       && FindProcShort (glProgramUniform3fv)
       && FindProcShort (glProgramUniform3d)
       && FindProcShort (glProgramUniform3dv)
       && FindProcShort (glProgramUniform3ui)
       && FindProcShort (glProgramUniform3uiv)
       && FindProcShort (glProgramUniform4i)
       && FindProcShort (glProgramUniform4iv)
       && FindProcShort (glProgramUniform4f)
       && FindProcShort (glProgramUniform4fv)
       && FindProcShort (glProgramUniform4d)
       && FindProcShort (glProgramUniform4dv)
       && FindProcShort (glProgramUniform4ui)
       && FindProcShort (glProgramUniform4uiv)
       && FindProcShort (glProgramUniformMatrix2fv)
       && FindProcShort (glProgramUniformMatrix3fv)
       && FindProcShort (glProgramUniformMatrix4fv)
       && FindProcShort (glProgramUniformMatrix2dv)
       && FindProcShort (glProgramUniformMatrix3dv)
       && FindProcShort (glProgramUniformMatrix4dv)
       && FindProcShort (glProgramUniformMatrix2x3fv)
       && FindProcShort (glProgramUniformMatrix3x2fv)
       && FindProcShort (glProgramUniformMatrix2x4fv)
       && FindProcShort (glProgramUniformMatrix4x2fv)
       && FindProcShort (glProgramUniformMatrix3x4fv)
       && FindProcShort (glProgramUniformMatrix4x3fv)
       && FindProcShort (glProgramUniformMatrix2x3dv)
       && FindProcShort (glProgramUniformMatrix3x2dv)
       && FindProcShort (glProgramUniformMatrix2x4dv)
       && FindProcShort (glProgramUniformMatrix4x2dv)
       && FindProcShort (glProgramUniformMatrix3x4dv)
       && FindProcShort (glProgramUniformMatrix4x3dv)
       && FindProcShort (glValidateProgramPipeline)
       && FindProcShort (glGetProgramPipelineInfoLog);

  // load GL_ARB_vertex_attrib_64bit (added to OpenGL 4.1 core)
  const bool hasVertAttrib64bit = (isGlGreaterEqualShort (4, 1) || checkExtensionShort ("GL_ARB_vertex_attrib_64bit"))
       && FindProcShort (glVertexAttribL1d)
       && FindProcShort (glVertexAttribL2d)
       && FindProcShort (glVertexAttribL3d)
       && FindProcShort (glVertexAttribL4d)
       && FindProcShort (glVertexAttribL1dv)
       && FindProcShort (glVertexAttribL2dv)
       && FindProcShort (glVertexAttribL3dv)
       && FindProcShort (glVertexAttribL4dv)
       && FindProcShort (glVertexAttribLPointer)
       && FindProcShort (glGetVertexAttribLdv);

  // load GL_ARB_viewport_array (added to OpenGL 4.1 core)
  const bool hasViewportArray = (isGlGreaterEqualShort (4, 1) || checkExtensionShort ("GL_ARB_viewport_array"))
       && FindProcShort (glViewportArrayv)
       && FindProcShort (glViewportIndexedf)
       && FindProcShort (glViewportIndexedfv)
       && FindProcShort (glScissorArrayv)
       && FindProcShort (glScissorIndexed)
       && FindProcShort (glScissorIndexedv)
       && FindProcShort (glDepthRangeArrayv)
       && FindProcShort (glDepthRangeIndexed)
       && FindProcShort (glGetFloati_v)
       && FindProcShort (glGetDoublei_v);

  has41 = isGlGreaterEqualShort (4, 1)
       && hasES2Compatibility
       && hasGetProgramBinary
       && hasSeparateShaderObjects
       && hasVertAttrib64bit
       && hasViewportArray;
  if (has41)
  {
    theCtx.core41 = (OpenGl_GlCore41* )this;
  }
  else
  {
    theCtx.checkWrongVersion (4, 1, aLastFailedProc);
  }

  // load GL_ARB_base_instance (added to OpenGL 4.2 core)
  const bool hasBaseInstance = (isGlGreaterEqualShort (4, 2) || checkExtensionShort ("GL_ARB_base_instance"))
       && FindProcShort (glDrawArraysInstancedBaseInstance)
       && FindProcShort (glDrawElementsInstancedBaseInstance)
       && FindProcShort (glDrawElementsInstancedBaseVertexBaseInstance);

  // load GL_ARB_transform_feedback_instanced (added to OpenGL 4.2 core)
  const bool hasTrsfFeedbackInstanced = (isGlGreaterEqualShort (4, 2) || checkExtensionShort ("GL_ARB_transform_feedback_instanced"))
       && FindProcShort (glDrawTransformFeedbackInstanced)
       && FindProcShort (glDrawTransformFeedbackStreamInstanced);

  // load GL_ARB_internalformat_query (added to OpenGL 4.2 core)
  const bool hasInternalFormatQuery = (isGlGreaterEqualShort (4, 2) || checkExtensionShort ("GL_ARB_internalformat_query"))
       && FindProcShort (glGetInternalformativ);

  // load GL_ARB_shader_atomic_counters (added to OpenGL 4.2 core)
  const bool hasShaderAtomicCounters = (isGlGreaterEqualShort (4, 2) || checkExtensionShort ("GL_ARB_shader_atomic_counters"))
       && FindProcShort (glGetActiveAtomicCounterBufferiv);

  // load GL_ARB_shader_image_load_store (added to OpenGL 4.2 core)
  const bool hasShaderImgLoadStore = (isGlGreaterEqualShort (4, 2) || checkExtensionShort ("GL_ARB_shader_image_load_store"))
       && FindProcShort (glBindImageTexture)
       && FindProcShort (glMemoryBarrier);

  // load GL_ARB_texture_storage (added to OpenGL 4.2 core)
  const bool hasTextureStorage = (isGlGreaterEqualShort (4, 2) || checkExtensionShort ("GL_ARB_texture_storage"))
       && FindProcShort (glTexStorage1D)
       && FindProcShort (glTexStorage2D)
       && FindProcShort (glTexStorage3D);

  has42 = isGlGreaterEqualShort (4, 2)
       && hasBaseInstance
       && hasTrsfFeedbackInstanced
       && hasInternalFormatQuery
       && hasShaderAtomicCounters
       && hasShaderImgLoadStore
       && hasTextureStorage;
  if (has42)
  {
    theCtx.core42 = (OpenGl_GlCore42* )this;
  }
  else
  {
    theCtx.checkWrongVersion (4, 2, aLastFailedProc);
  }

  has43 = isGlGreaterEqualShort (4, 3)
       && FindProcShort (glClearBufferData)
       && FindProcShort (glClearBufferSubData)
       && FindProcShort (glDispatchCompute)
       && FindProcShort (glDispatchComputeIndirect)
       && FindProcShort (glCopyImageSubData)
       && FindProcShort (glFramebufferParameteri)
       && FindProcShort (glGetFramebufferParameteriv)
       && FindProcShort (glGetInternalformati64v)
       && FindProcShort (glInvalidateTexSubImage)
       && FindProcShort (glInvalidateTexImage)
       && FindProcShort (glInvalidateBufferSubData)
       && FindProcShort (glInvalidateBufferData)
       && FindProcShort (glInvalidateFramebuffer)
       && FindProcShort (glInvalidateSubFramebuffer)
       && FindProcShort (glMultiDrawArraysIndirect)
       && FindProcShort (glMultiDrawElementsIndirect)
       && FindProcShort (glGetProgramInterfaceiv)
       && FindProcShort (glGetProgramResourceIndex)
       && FindProcShort (glGetProgramResourceName)
       && FindProcShort (glGetProgramResourceiv)
       && FindProcShort (glGetProgramResourceLocation)
       && FindProcShort (glGetProgramResourceLocationIndex)
       && FindProcShort (glShaderStorageBlockBinding)
       && FindProcShort (glTexBufferRange)
       && FindProcShort (glTexStorage2DMultisample)
       && FindProcShort (glTexStorage3DMultisample)
       && FindProcShort (glTextureView)
       && FindProcShort (glBindVertexBuffer)
       && FindProcShort (glVertexAttribFormat)
       && FindProcShort (glVertexAttribIFormat)
       && FindProcShort (glVertexAttribLFormat)
       && FindProcShort (glVertexAttribBinding)
       && FindProcShort (glVertexBindingDivisor)
       && FindProcShort (glDebugMessageControl)
       && FindProcShort (glDebugMessageInsert)
       && FindProcShort (glDebugMessageCallback)
       && FindProcShort (glGetDebugMessageLog)
       && FindProcShort (glPushDebugGroup)
       && FindProcShort (glPopDebugGroup)
       && FindProcShort (glObjectLabel)
       && FindProcShort (glGetObjectLabel)
       && FindProcShort (glObjectPtrLabel)
       && FindProcShort (glGetObjectPtrLabel);
  if (has43)
  {
    theCtx.core43 = (OpenGl_GlCore43* )this;
  }
  else
  {
    theCtx.checkWrongVersion (4, 3, aLastFailedProc);
  }

  // load GL_ARB_clear_texture (added to OpenGL 4.4 core)
  bool arbTexClear = (isGlGreaterEqualShort (4, 4) || checkExtensionShort ("GL_ARB_clear_texture"))
       && FindProcShort (glClearTexImage)
       && FindProcShort (glClearTexSubImage);

  has44 = isGlGreaterEqualShort (4, 4)
       && arbTexClear
       && FindProcShort (glBufferStorage)
       && FindProcShort (glBindBuffersBase)
       && FindProcShort (glBindBuffersRange)
       && FindProcShort (glBindTextures)
       && FindProcShort (glBindSamplers)
       && FindProcShort (glBindImageTextures)
       && FindProcShort (glBindVertexBuffers);
  if (has44)
  {
    theCtx.core44 = (OpenGl_GlCore44* )this;
  }
  else
  {
    theCtx.checkWrongVersion (4, 4, aLastFailedProc);
  }

  has45 = isGlGreaterEqualShort (4, 5)
       && FindProcShort (glBindVertexBuffers)
       && FindProcShort (glClipControl)
       && FindProcShort (glCreateTransformFeedbacks)
       && FindProcShort (glTransformFeedbackBufferBase)
       && FindProcShort (glTransformFeedbackBufferRange)
       && FindProcShort (glGetTransformFeedbackiv)
       && FindProcShort (glGetTransformFeedbacki_v)
       && FindProcShort (glGetTransformFeedbacki64_v)
       && FindProcShort (glCreateBuffers)
       && FindProcShort (glNamedBufferStorage)
       && FindProcShort (glNamedBufferData)
       && FindProcShort (glNamedBufferSubData)
       && FindProcShort (glCopyNamedBufferSubData)
       && FindProcShort (glClearNamedBufferData)
       && FindProcShort (glClearNamedBufferSubData)
       && FindProcShort (glMapNamedBuffer)
       && FindProcShort (glMapNamedBufferRange)
       && FindProcShort (glUnmapNamedBuffer)
       && FindProcShort (glFlushMappedNamedBufferRange)
       && FindProcShort (glGetNamedBufferParameteriv)
       && FindProcShort (glGetNamedBufferParameteri64v)
       && FindProcShort (glGetNamedBufferPointerv)
       && FindProcShort (glGetNamedBufferSubData)
       && FindProcShort (glCreateFramebuffers)
       && FindProcShort (glNamedFramebufferRenderbuffer)
       && FindProcShort (glNamedFramebufferParameteri)
       && FindProcShort (glNamedFramebufferTexture)
       && FindProcShort (glNamedFramebufferTextureLayer)
       && FindProcShort (glNamedFramebufferDrawBuffer)
       && FindProcShort (glNamedFramebufferDrawBuffers)
       && FindProcShort (glNamedFramebufferReadBuffer)
       && FindProcShort (glInvalidateNamedFramebufferData)
       && FindProcShort (glInvalidateNamedFramebufferSubData)
       && FindProcShort (glClearNamedFramebufferiv)
       && FindProcShort (glClearNamedFramebufferuiv)
       && FindProcShort (glClearNamedFramebufferfv)
       && FindProcShort (glClearNamedFramebufferfi)
       && FindProcShort (glBlitNamedFramebuffer)
       && FindProcShort (glCheckNamedFramebufferStatus)
       && FindProcShort (glGetNamedFramebufferParameteriv)
       && FindProcShort (glGetNamedFramebufferAttachmentParameteriv)
       && FindProcShort (glCreateRenderbuffers)
       && FindProcShort (glNamedRenderbufferStorage)
       && FindProcShort (glNamedRenderbufferStorageMultisample)
       && FindProcShort (glGetNamedRenderbufferParameteriv)
       && FindProcShort (glCreateTextures)
       && FindProcShort (glTextureBuffer)
       && FindProcShort (glTextureBufferRange)
       && FindProcShort (glTextureStorage1D)
       && FindProcShort (glTextureStorage2D)
       && FindProcShort (glTextureStorage3D)
       && FindProcShort (glTextureStorage2DMultisample)
       && FindProcShort (glTextureStorage3DMultisample)
       && FindProcShort (glTextureSubImage1D)
       && FindProcShort (glTextureSubImage2D)
       && FindProcShort (glTextureSubImage3D)
       && FindProcShort (glCompressedTextureSubImage1D)
       && FindProcShort (glCompressedTextureSubImage2D)
       && FindProcShort (glCompressedTextureSubImage3D)
       && FindProcShort (glCopyTextureSubImage1D)
       && FindProcShort (glCopyTextureSubImage2D)
       && FindProcShort (glCopyTextureSubImage3D)
       && FindProcShort (glTextureParameterf)
       && FindProcShort (glTextureParameterfv)
       && FindProcShort (glTextureParameteri)
       && FindProcShort (glTextureParameterIiv)
       && FindProcShort (glTextureParameterIuiv)
       && FindProcShort (glTextureParameteriv)
       && FindProcShort (glGenerateTextureMipmap)
       && FindProcShort (glBindTextureUnit)
       && FindProcShort (glGetTextureImage)
       && FindProcShort (glGetCompressedTextureImage)
       && FindProcShort (glGetTextureLevelParameterfv)
       && FindProcShort (glGetTextureLevelParameteriv)
       && FindProcShort (glGetTextureParameterfv)
       && FindProcShort (glGetTextureParameterIiv)
       && FindProcShort (glGetTextureParameterIuiv)
       && FindProcShort (glGetTextureParameteriv)
       && FindProcShort (glCreateVertexArrays)
       && FindProcShort (glDisableVertexArrayAttrib)
       && FindProcShort (glEnableVertexArrayAttrib)
       && FindProcShort (glVertexArrayElementBuffer)
       && FindProcShort (glVertexArrayVertexBuffer)
       && FindProcShort (glVertexArrayVertexBuffers)
       && FindProcShort (glVertexArrayAttribBinding)
       && FindProcShort (glVertexArrayAttribFormat)
       && FindProcShort (glVertexArrayAttribIFormat)
       && FindProcShort (glVertexArrayAttribLFormat)
       && FindProcShort (glVertexArrayBindingDivisor)
       && FindProcShort (glGetVertexArrayiv)
       && FindProcShort (glGetVertexArrayIndexediv)
       && FindProcShort (glGetVertexArrayIndexed64iv)
       && FindProcShort (glCreateSamplers)
       && FindProcShort (glCreateProgramPipelines)
       && FindProcShort (glCreateQueries)
       && FindProcShort (glGetQueryBufferObjecti64v)
       && FindProcShort (glGetQueryBufferObjectiv)
       && FindProcShort (glGetQueryBufferObjectui64v)
       && FindProcShort (glGetQueryBufferObjectuiv)
       && FindProcShort (glMemoryBarrierByRegion)
       && FindProcShort (glGetTextureSubImage)
       && FindProcShort (glGetCompressedTextureSubImage)
       && FindProcShort (glGetGraphicsResetStatus)
       && FindProcShort (glGetnUniformfv)
       && FindProcShort (glGetnUniformiv)
       && FindProcShort (glGetnUniformuiv)
       && FindProcShort (glReadnPixels)
       && FindProcShort (glTextureBarrier);
  bool hasGetnTexImage = has45
                      && FindProcShort (glGetnCompressedTexImage)
                      && FindProcShort (glGetnTexImage)
                      && FindProcShort (glGetnUniformdv);
  if (has45 && !hasGetnTexImage)
  {
    // Intel driver exports only ARB-suffixed functions in a violation to OpenGL 4.5 specs
    hasGetnTexImage = checkExtensionShort ("GL_ARB_robustness")
                   && theCtx.FindProc ("glGetnCompressedTexImageARB", this->glGetnCompressedTexImage)
                   && theCtx.FindProc ("glGetnTexImageARB",           this->glGetnTexImage)
                   && theCtx.FindProc ("glGetnUniformdvARB",          this->glGetnUniformdv);
    has45 = hasGetnTexImage;
    if (hasGetnTexImage)
    {
      Message::SendTrace() << "Warning! glGetnCompressedTexImage function required by OpenGL 4.5 specs is not found.\n"
                              "A non-standard glGetnCompressedTexImageARB fallback will be used instead.\n"
                              "Please report this issue to OpenGL driver vendor '" << theCtx.myVendor << "'.";
    }
  }
  if (has45)
  {
    theCtx.core45 = (OpenGl_GlCore45* )this;
    theCtx.arbClipControl = true;
  }
  else
  {
    theCtx.checkWrongVersion (4, 5, aLastFailedProc);
  }

  has46 = isGlGreaterEqualShort (4, 6)
       && FindProcShort (glSpecializeShader)
       && FindProcShort (glPolygonOffsetClamp);

  bool hasIndParams = has46
                   && FindProcShort (glMultiDrawArraysIndirectCount)
                   && FindProcShort (glMultiDrawElementsIndirectCount);
  if (has46 && !hasIndParams)
  {
    // Intel driver exports only ARB-suffixed functions in a violation to OpenGL 4.6 specs
    hasIndParams = checkExtensionShort ("GL_ARB_indirect_parameters")
                && theCtx.FindProc ("glMultiDrawArraysIndirectCountARB",   this->glMultiDrawArraysIndirectCount)
                && theCtx.FindProc ("glMultiDrawElementsIndirectCountARB", this->glMultiDrawElementsIndirectCount);
    has46 = hasIndParams;
    if (hasIndParams)
    {
      Message::SendTrace() << "Warning! glMultiDrawArraysIndirectCount function required by OpenGL 4.6 specs is not found.\n"
                              "A non-standard glMultiDrawArraysIndirectCountARB fallback will be used instead.\n"
                              "Please report this issue to OpenGL driver vendor '" << theCtx.myVendor << "'.";
    }
  }

  if (has46)
  {
    theCtx.core46 = (OpenGl_GlCore46* )this;
  }
  else
  {
    theCtx.checkWrongVersion (4, 6, aLastFailedProc);
  }

  // initialize debug context extension
  if (checkExtensionShort ("GL_ARB_debug_output"))
  {
    theCtx.arbDbg = NULL;
    if (has43)
    {
      theCtx.arbDbg = (OpenGl_ArbDbg* )this;
    }
    else if (theCtx.FindProc ("glDebugMessageControlARB",  this->glDebugMessageControl)
          && theCtx.FindProc ("glDebugMessageInsertARB",   this->glDebugMessageInsert)
          && theCtx.FindProc ("glDebugMessageCallbackARB", this->glDebugMessageCallback)
          && theCtx.FindProc ("glGetDebugMessageLogARB",   this->glGetDebugMessageLog))
    {
      theCtx.arbDbg = (OpenGl_ArbDbg* )this;
    }
  }

  // initialize FBO extension (ARB)
  if (hasFBO)
  {
    theCtx.arbFBO     = (OpenGl_ArbFBO*     )this;
    theCtx.arbFBOBlit = (OpenGl_ArbFBOBlit* )this;
    theCtx.extPDS = Standard_True; // extension for EXT, but part of ARB
  }

  // initialize GS extension (EXT)
  if (checkExtensionShort ("GL_EXT_geometry_shader4")
   && FindProcShort (glProgramParameteriEXT))
  {
    theCtx.extGS = (OpenGl_ExtGS* )this;
  }

  // initialize bindless texture extension (ARB)
  if (checkExtensionShort ("GL_ARB_bindless_texture")
   && FindProcShort (glGetTextureHandleARB)
   && FindProcShort (glGetTextureSamplerHandleARB)
   && FindProcShort (glMakeTextureHandleResidentARB)
   && FindProcShort (glMakeTextureHandleNonResidentARB)
   && FindProcShort (glGetImageHandleARB)
   && FindProcShort (glMakeImageHandleResidentARB)
   && FindProcShort (glMakeImageHandleNonResidentARB)
   && FindProcShort (glUniformHandleui64ARB)
   && FindProcShort (glUniformHandleui64vARB)
   && FindProcShort (glProgramUniformHandleui64ARB)
   && FindProcShort (glProgramUniformHandleui64vARB)
   && FindProcShort (glIsTextureHandleResidentARB)
   && FindProcShort (glIsImageHandleResidentARB)
   && FindProcShort (glVertexAttribL1ui64ARB)
   && FindProcShort (glVertexAttribL1ui64vARB)
   && FindProcShort (glGetVertexAttribLui64vARB))
  {
    theCtx.arbTexBindless = (OpenGl_ArbTexBindless* )this;
  }

  if (!has45
    && checkExtensionShort ("GL_ARB_clip_control")
    && FindProcShort (glClipControl))
  {
    theCtx.arbClipControl = true;
  }

  if (has30)
  {
    if (!has32
     && checkExtensionShort ("GL_ARB_texture_multisample")
     && FindProcShort (glTexImage2DMultisample))
    {
      //
    }
    if (!has43
     && checkExtensionShort ("GL_ARB_texture_storage_multisample")
     && FindProcShort (glTexStorage2DMultisample))
    {
      //
    }
  }
#endif
}
