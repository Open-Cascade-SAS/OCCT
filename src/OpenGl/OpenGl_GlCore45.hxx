// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _OpenGl_GlCore45_Header
#define _OpenGl_GlCore45_Header

#include <OpenGl_GlCore44.hxx>

//! OpenGL 4.5 definition.
struct OpenGl_GlCore45 : public OpenGl_GlCore44
{
private:
  typedef OpenGl_GlCore44 theBaseClass_t;

public: //! @name OpenGL 4.5 additives to 4.4

#if !defined(GL_ES_VERSION_2_0)
  using theBaseClass_t::glClipControl;
  using theBaseClass_t::glCreateTransformFeedbacks;
  using theBaseClass_t::glTransformFeedbackBufferBase;
  using theBaseClass_t::glTransformFeedbackBufferRange;
  using theBaseClass_t::glGetTransformFeedbackiv;
  using theBaseClass_t::glGetTransformFeedbacki_v;
  using theBaseClass_t::glGetTransformFeedbacki64_v;
  using theBaseClass_t::glCreateBuffers;
  using theBaseClass_t::glNamedBufferStorage;
  using theBaseClass_t::glNamedBufferData;
  using theBaseClass_t::glNamedBufferSubData;
  using theBaseClass_t::glCopyNamedBufferSubData;
  using theBaseClass_t::glClearNamedBufferData;
  using theBaseClass_t::glClearNamedBufferSubData;
  using theBaseClass_t::glMapNamedBuffer;
  using theBaseClass_t::glMapNamedBufferRange;
  using theBaseClass_t::glUnmapNamedBuffer;
  using theBaseClass_t::glFlushMappedNamedBufferRange;
  using theBaseClass_t::glGetNamedBufferParameteriv;
  using theBaseClass_t::glGetNamedBufferParameteri64v;
  using theBaseClass_t::glGetNamedBufferPointerv;
  using theBaseClass_t::glGetNamedBufferSubData;
  using theBaseClass_t::glCreateFramebuffers;
  using theBaseClass_t::glNamedFramebufferRenderbuffer;
  using theBaseClass_t::glNamedFramebufferParameteri;
  using theBaseClass_t::glNamedFramebufferTexture;
  using theBaseClass_t::glNamedFramebufferTextureLayer;
  using theBaseClass_t::glNamedFramebufferDrawBuffer;
  using theBaseClass_t::glNamedFramebufferDrawBuffers;
  using theBaseClass_t::glNamedFramebufferReadBuffer;
  using theBaseClass_t::glInvalidateNamedFramebufferData;
  using theBaseClass_t::glInvalidateNamedFramebufferSubData;
  using theBaseClass_t::glClearNamedFramebufferiv;
  using theBaseClass_t::glClearNamedFramebufferuiv;
  using theBaseClass_t::glClearNamedFramebufferfv;
  using theBaseClass_t::glClearNamedFramebufferfi;
  using theBaseClass_t::glBlitNamedFramebuffer;
  using theBaseClass_t::glCheckNamedFramebufferStatus;
  using theBaseClass_t::glGetNamedFramebufferParameteriv;
  using theBaseClass_t::glGetNamedFramebufferAttachmentParameteriv;
  using theBaseClass_t::glCreateRenderbuffers;
  using theBaseClass_t::glNamedRenderbufferStorage;
  using theBaseClass_t::glNamedRenderbufferStorageMultisample;
  using theBaseClass_t::glGetNamedRenderbufferParameteriv;
  using theBaseClass_t::glCreateTextures;
  using theBaseClass_t::glTextureBuffer;
  using theBaseClass_t::glTextureBufferRange;
  using theBaseClass_t::glTextureStorage1D;
  using theBaseClass_t::glTextureStorage2D;
  using theBaseClass_t::glTextureStorage3D;
  using theBaseClass_t::glTextureStorage2DMultisample;
  using theBaseClass_t::glTextureStorage3DMultisample;
  using theBaseClass_t::glTextureSubImage1D;
  using theBaseClass_t::glTextureSubImage2D;
  using theBaseClass_t::glTextureSubImage3D;
  using theBaseClass_t::glCompressedTextureSubImage1D;
  using theBaseClass_t::glCompressedTextureSubImage2D;
  using theBaseClass_t::glCompressedTextureSubImage3D;
  using theBaseClass_t::glCopyTextureSubImage1D;
  using theBaseClass_t::glCopyTextureSubImage2D;
  using theBaseClass_t::glCopyTextureSubImage3D;
  using theBaseClass_t::glTextureParameterf;
  using theBaseClass_t::glTextureParameterfv;
  using theBaseClass_t::glTextureParameteri;
  using theBaseClass_t::glTextureParameterIiv;
  using theBaseClass_t::glTextureParameterIuiv;
  using theBaseClass_t::glTextureParameteriv;
  using theBaseClass_t::glGenerateTextureMipmap;
  using theBaseClass_t::glBindTextureUnit;
  using theBaseClass_t::glGetTextureImage;
  using theBaseClass_t::glGetCompressedTextureImage;
  using theBaseClass_t::glGetTextureLevelParameterfv;
  using theBaseClass_t::glGetTextureLevelParameteriv;
  using theBaseClass_t::glGetTextureParameterfv;
  using theBaseClass_t::glGetTextureParameterIiv;
  using theBaseClass_t::glGetTextureParameterIuiv;
  using theBaseClass_t::glGetTextureParameteriv;
  using theBaseClass_t::glCreateVertexArrays;
  using theBaseClass_t::glDisableVertexArrayAttrib;
  using theBaseClass_t::glEnableVertexArrayAttrib;
  using theBaseClass_t::glVertexArrayElementBuffer;
  using theBaseClass_t::glVertexArrayVertexBuffer;
  using theBaseClass_t::glVertexArrayVertexBuffers;
  using theBaseClass_t::glVertexArrayAttribBinding;
  using theBaseClass_t::glVertexArrayAttribFormat;
  using theBaseClass_t::glVertexArrayAttribIFormat;
  using theBaseClass_t::glVertexArrayAttribLFormat;
  using theBaseClass_t::glVertexArrayBindingDivisor;
  using theBaseClass_t::glGetVertexArrayiv;
  using theBaseClass_t::glGetVertexArrayIndexediv;
  using theBaseClass_t::glGetVertexArrayIndexed64iv;
  using theBaseClass_t::glCreateSamplers;
  using theBaseClass_t::glCreateProgramPipelines;
  using theBaseClass_t::glCreateQueries;
  using theBaseClass_t::glGetQueryBufferObjecti64v;
  using theBaseClass_t::glGetQueryBufferObjectiv;
  using theBaseClass_t::glGetQueryBufferObjectui64v;
  using theBaseClass_t::glGetQueryBufferObjectuiv;
  using theBaseClass_t::glMemoryBarrierByRegion;
  using theBaseClass_t::glGetTextureSubImage;
  using theBaseClass_t::glGetCompressedTextureSubImage;
  using theBaseClass_t::glGetGraphicsResetStatus;
  using theBaseClass_t::glGetnCompressedTexImage;
  using theBaseClass_t::glGetnTexImage;
  using theBaseClass_t::glGetnUniformdv;
  using theBaseClass_t::glGetnUniformfv;
  using theBaseClass_t::glGetnUniformiv;
  using theBaseClass_t::glGetnUniformuiv;
  using theBaseClass_t::glReadnPixels;
  using theBaseClass_t::glTextureBarrier;
#endif

};

#endif // _OpenGl_GlCore45_Header
