// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _OpenGl_GLESExtensions_Header
#define _OpenGl_GLESExtensions_Header

// define items to unify code paths with desktop OpenGL
typedef double GLdouble;
typedef double GLclampd;

// GL_EXT_sRGB_write_control extension for OpenGL ES
// adds GL_FRAMEBUFFER_SRGB_EXT flag as on desktop OpenGL
#define GL_FRAMEBUFFER_SRGB 0x8DB9

// OpenGL ES 3.1+
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100

// OpenGL ES 3.2+ or GL_EXT_texture_buffer for OpenGL ES 3.1+
#define GL_TEXTURE_BUFFER 0x8C2A

// in core since OpenGL ES 3.0, extension GL_OES_rgb8_rgba8
#define GL_LUMINANCE8 0x8040
// GL_EXT_texture_format_BGRA8888
#define GL_BGRA_EXT 0x80E1 // same as GL_BGRA on desktop

// GL_EXT_sRGB
#define GL_SRGB_EXT 0x8C40       // GL_SRGB_EXT
#define GL_SRGB_ALPHA_EXT 0x8C42 // GL_SRGB_ALPHA_EXT

#define GL_R16 0x822A
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB10 0x8052
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGB10_A2 0x8059
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_ALPHA8 0x803C
#define GL_ALPHA16 0x803E
#define GL_RG16 0x822C

#define GL_R16_SNORM 0x8F98
#define GL_RG16_SNORM 0x8F99
#define GL_RGB16_SNORM 0x8F9A
#define GL_RGBA16_SNORM 0x8F9B

#define GL_RED_SNORM 0x8F90
#define GL_RG_SNORM 0x8F91
#define GL_RGB_SNORM 0x8F92
#define GL_RGBA_SNORM 0x8F93

// GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

// debug ARB extension
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
#define GL_DEBUG_LOGGED_MESSAGES 0x9145
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148

// OpenGL ES 3.0+ or OES_texture_half_float
#define GL_HALF_FLOAT_OES 0x8D61

// OpenGL ES 3.1+
#define GL_COMPUTE_SHADER 0x91B9

// OpenGL ES 3.2+
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_TESS_CONTROL_SHADER 0x8E88
#define GL_TESS_EVALUATION_SHADER 0x8E87
#define GL_LINES_ADJACENCY 0x000A
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
#define GL_PATCHES 0x000E

// GL_EXT_texture_compression_s3tc extension
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
//
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT 0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F

#ifndef GL_APIENTRYP
  #define GL_APIENTRYP GL_APIENTRY*
#endif

// put into namespace to avoid collisions with system headers
namespace opencascade
{

// #ifndef GL_ES_VERSION_3_0
typedef void(GL_APIENTRYP PFNGLREADBUFFERPROC)(GLenum src);
typedef void(GL_APIENTRYP PFNGLDRAWRANGEELEMENTSPROC)(GLenum      mode,
                                                      GLuint      start,
                                                      GLuint      end,
                                                      GLsizei     count,
                                                      GLenum      type,
                                                      const void* indices);
typedef void(GL_APIENTRYP PFNGLTEXIMAGE3DPROC)(GLenum      target,
                                               GLint       level,
                                               GLint       internalformat,
                                               GLsizei     width,
                                               GLsizei     height,
                                               GLsizei     depth,
                                               GLint       border,
                                               GLenum      format,
                                               GLenum      type,
                                               const void* pixels);
typedef void(GL_APIENTRYP PFNGLTEXSUBIMAGE3DPROC)(GLenum      target,
                                                  GLint       level,
                                                  GLint       xoffset,
                                                  GLint       yoffset,
                                                  GLint       zoffset,
                                                  GLsizei     width,
                                                  GLsizei     height,
                                                  GLsizei     depth,
                                                  GLenum      format,
                                                  GLenum      type,
                                                  const void* pixels);
typedef void(GL_APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC)(GLenum  target,
                                                      GLint   level,
                                                      GLint   xoffset,
                                                      GLint   yoffset,
                                                      GLint   zoffset,
                                                      GLint   x,
                                                      GLint   y,
                                                      GLsizei width,
                                                      GLsizei height);
typedef void(GL_APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC)(GLenum      target,
                                                         GLint       level,
                                                         GLenum      internalformat,
                                                         GLsizei     width,
                                                         GLsizei     height,
                                                         GLsizei     depth,
                                                         GLint       border,
                                                         GLsizei     imageSize,
                                                         const void* data);
typedef void(GL_APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)(GLenum      target,
                                                            GLint       level,
                                                            GLint       xoffset,
                                                            GLint       yoffset,
                                                            GLint       zoffset,
                                                            GLsizei     width,
                                                            GLsizei     height,
                                                            GLsizei     depth,
                                                            GLenum      format,
                                                            GLsizei     imageSize,
                                                            const void* data);
typedef void(GL_APIENTRYP PFNGLGENQUERIESPROC)(GLsizei n, GLuint* ids);
typedef void(GL_APIENTRYP PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint* ids);
typedef GLboolean(GL_APIENTRYP PFNGLISQUERYPROC)(GLuint id);
typedef void(GL_APIENTRYP PFNGLBEGINQUERYPROC)(GLenum target, GLuint id);
typedef void(GL_APIENTRYP PFNGLENDQUERYPROC)(GLenum target);
typedef void(GL_APIENTRYP PFNGLGETQUERYIVPROC)(GLenum target, GLenum pname, GLint* params);
typedef void(GL_APIENTRYP PFNGLGETQUERYOBJECTUIVPROC)(GLuint id, GLenum pname, GLuint* params);
typedef GLboolean(GL_APIENTRYP PFNGLUNMAPBUFFERPROC)(GLenum target);
typedef void(GL_APIENTRYP PFNGLGETBUFFERPOINTERVPROC)(GLenum target, GLenum pname, void** params);
typedef void(GL_APIENTRYP PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum* bufs);
typedef void(GL_APIENTRYP PFNGLUNIFORMMATRIX2X3FVPROC)(GLint          location,
                                                       GLsizei        count,
                                                       GLboolean      transpose,
                                                       const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC)(GLint          location,
                                                       GLsizei        count,
                                                       GLboolean      transpose,
                                                       const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLUNIFORMMATRIX2X4FVPROC)(GLint          location,
                                                       GLsizei        count,
                                                       GLboolean      transpose,
                                                       const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC)(GLint          location,
                                                       GLsizei        count,
                                                       GLboolean      transpose,
                                                       const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC)(GLint          location,
                                                       GLsizei        count,
                                                       GLboolean      transpose,
                                                       const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)(GLint          location,
                                                       GLsizei        count,
                                                       GLboolean      transpose,
                                                       const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLBLITFRAMEBUFFERPROC)(GLint      srcX0,
                                                    GLint      srcY0,
                                                    GLint      srcX1,
                                                    GLint      srcY1,
                                                    GLint      dstX0,
                                                    GLint      dstY0,
                                                    GLint      dstX1,
                                                    GLint      dstY1,
                                                    GLbitfield mask,
                                                    GLenum     filter);
typedef void(GL_APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)(GLenum  target,
                                                                   GLsizei samples,
                                                                   GLenum  internalformat,
                                                                   GLsizei width,
                                                                   GLsizei height);
typedef void(GL_APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC)(GLenum target,
                                                            GLenum attachment,
                                                            GLuint texture,
                                                            GLint  level,
                                                            GLint  layer);
typedef void*(GL_APIENTRYP PFNGLMAPBUFFERRANGEPROC)(GLenum     target,
                                                    GLintptr   offset,
                                                    GLsizeiptr length,
                                                    GLbitfield access);
typedef void(GL_APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC)(GLenum     target,
                                                           GLintptr   offset,
                                                           GLsizeiptr length);
typedef void(GL_APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void(GL_APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);
typedef void(GL_APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef GLboolean(GL_APIENTRYP PFNGLISVERTEXARRAYPROC)(GLuint array);
typedef void(GL_APIENTRYP PFNGLGETINTEGERI_VPROC)(GLenum target, GLuint index, GLint* data);
typedef void(GL_APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC)(GLenum primitiveMode);
typedef void(GL_APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC)(void);
typedef void(GL_APIENTRYP PFNGLBINDBUFFERRANGEPROC)(GLenum     target,
                                                    GLuint     index,
                                                    GLuint     buffer,
                                                    GLintptr   offset,
                                                    GLsizeiptr size);
typedef void(GL_APIENTRYP PFNGLBINDBUFFERBASEPROC)(GLenum target, GLuint index, GLuint buffer);
typedef void(GL_APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC)(GLuint               program,
                                                              GLsizei              count,
                                                              const GLchar* const* varyings,
                                                              GLenum               bufferMode);
typedef void(GL_APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)(GLuint   program,
                                                                GLuint   index,
                                                                GLsizei  bufSize,
                                                                GLsizei* length,
                                                                GLsizei* size,
                                                                GLenum*  type,
                                                                GLchar*  name);
typedef void(GL_APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint      index,
                                                         GLint       size,
                                                         GLenum      type,
                                                         GLsizei     stride,
                                                         const void* pointer);
typedef void(GL_APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC)(GLuint index, GLenum pname, GLint* params);
typedef void(GL_APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC)(GLuint index, GLenum pname, GLuint* params);
typedef void(
  GL_APIENTRYP PFNGLVERTEXATTRIBI4IPROC)(GLuint index, GLint x, GLint y, GLint z, GLint w);
typedef void(
  GL_APIENTRYP PFNGLVERTEXATTRIBI4UIPROC)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
typedef void(GL_APIENTRYP PFNGLVERTEXATTRIBI4IVPROC)(GLuint index, const GLint* v);
typedef void(GL_APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC)(GLuint index, const GLuint* v);
typedef void(GL_APIENTRYP PFNGLGETUNIFORMUIVPROC)(GLuint program, GLint location, GLuint* params);
typedef GLint(GL_APIENTRYP PFNGLGETFRAGDATALOCATIONPROC)(GLuint program, const GLchar* name);
typedef void(GL_APIENTRYP PFNGLUNIFORM1UIPROC)(GLint location, GLuint v0);
typedef void(GL_APIENTRYP PFNGLUNIFORM2UIPROC)(GLint location, GLuint v0, GLuint v1);
typedef void(GL_APIENTRYP PFNGLUNIFORM3UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2);
typedef void(
  GL_APIENTRYP PFNGLUNIFORM4UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef void(GL_APIENTRYP PFNGLUNIFORM1UIVPROC)(GLint location, GLsizei count, const GLuint* value);
typedef void(GL_APIENTRYP PFNGLUNIFORM2UIVPROC)(GLint location, GLsizei count, const GLuint* value);
typedef void(GL_APIENTRYP PFNGLUNIFORM3UIVPROC)(GLint location, GLsizei count, const GLuint* value);
typedef void(GL_APIENTRYP PFNGLUNIFORM4UIVPROC)(GLint location, GLsizei count, const GLuint* value);
typedef void(GL_APIENTRYP PFNGLCLEARBUFFERIVPROC)(GLenum       buffer,
                                                  GLint        drawbuffer,
                                                  const GLint* value);
typedef void(GL_APIENTRYP PFNGLCLEARBUFFERUIVPROC)(GLenum        buffer,
                                                   GLint         drawbuffer,
                                                   const GLuint* value);
typedef void(GL_APIENTRYP PFNGLCLEARBUFFERFVPROC)(GLenum         buffer,
                                                  GLint          drawbuffer,
                                                  const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLCLEARBUFFERFIPROC)(GLenum  buffer,
                                                  GLint   drawbuffer,
                                                  GLfloat depth,
                                                  GLint   stencil);
typedef const GLubyte*(GL_APIENTRYP PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);
typedef void(GL_APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC)(GLenum     readTarget,
                                                      GLenum     writeTarget,
                                                      GLintptr   readOffset,
                                                      GLintptr   writeOffset,
                                                      GLsizeiptr size);
typedef void(GL_APIENTRYP PFNGLGETUNIFORMINDICESPROC)(GLuint               program,
                                                      GLsizei              uniformCount,
                                                      const GLchar* const* uniformNames,
                                                      GLuint*              uniformIndices);
typedef void(GL_APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC)(GLuint        program,
                                                        GLsizei       uniformCount,
                                                        const GLuint* uniformIndices,
                                                        GLenum        pname,
                                                        GLint*        params);
typedef GLuint(GL_APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint        program,
                                                           const GLchar* uniformBlockName);
typedef void(GL_APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint program,
                                                            GLuint uniformBlockIndex,
                                                            GLenum pname,
                                                            GLint* params);
typedef void(GL_APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)(GLuint   program,
                                                              GLuint   uniformBlockIndex,
                                                              GLsizei  bufSize,
                                                              GLsizei* length,
                                                              GLchar*  uniformBlockName);
typedef void(GL_APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint program,
                                                        GLuint uniformBlockIndex,
                                                        GLuint uniformBlockBinding);
typedef void(GL_APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum  mode,
                                                        GLint   first,
                                                        GLsizei count,
                                                        GLsizei instancecount);
typedef void(GL_APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum      mode,
                                                          GLsizei     count,
                                                          GLenum      type,
                                                          const void* indices,
                                                          GLsizei     instancecount);
typedef GLsync(GL_APIENTRYP PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
typedef GLboolean(GL_APIENTRYP PFNGLISSYNCPROC)(GLsync sync);
typedef void(GL_APIENTRYP PFNGLDELETESYNCPROC)(GLsync sync);
typedef GLenum(GL_APIENTRYP PFNGLCLIENTWAITSYNCPROC)(GLsync     sync,
                                                     GLbitfield flags,
                                                     GLuint64   timeout);
typedef void(GL_APIENTRYP PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void(GL_APIENTRYP PFNGLGETINTEGER64VPROC)(GLenum pname, GLint64* data);
typedef void(GL_APIENTRYP PFNGLGETSYNCIVPROC)(GLsync   sync,
                                              GLenum   pname,
                                              GLsizei  bufSize,
                                              GLsizei* length,
                                              GLint*   values);
typedef void(GL_APIENTRYP PFNGLGETINTEGER64I_VPROC)(GLenum target, GLuint index, GLint64* data);
typedef void(GL_APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC)(GLenum   target,
                                                           GLenum   pname,
                                                           GLint64* params);
typedef void(GL_APIENTRYP PFNGLGENSAMPLERSPROC)(GLsizei count, GLuint* samplers);
typedef void(GL_APIENTRYP PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint* samplers);
typedef GLboolean(GL_APIENTRYP PFNGLISSAMPLERPROC)(GLuint sampler);
typedef void(GL_APIENTRYP PFNGLBINDSAMPLERPROC)(GLuint unit, GLuint sampler);
typedef void(GL_APIENTRYP PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
typedef void(GL_APIENTRYP PFNGLSAMPLERPARAMETERIVPROC)(GLuint       sampler,
                                                       GLenum       pname,
                                                       const GLint* param);
typedef void(GL_APIENTRYP PFNGLSAMPLERPARAMETERFPROC)(GLuint sampler, GLenum pname, GLfloat param);
typedef void(GL_APIENTRYP PFNGLSAMPLERPARAMETERFVPROC)(GLuint         sampler,
                                                       GLenum         pname,
                                                       const GLfloat* param);
typedef void(GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC)(GLuint sampler,
                                                          GLenum pname,
                                                          GLint* params);
typedef void(GL_APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC)(GLuint   sampler,
                                                          GLenum   pname,
                                                          GLfloat* params);
typedef void(GL_APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);
typedef void(GL_APIENTRYP PFNGLBINDTRANSFORMFEEDBACKPROC)(GLenum target, GLuint id);
typedef void(GL_APIENTRYP PFNGLDELETETRANSFORMFEEDBACKSPROC)(GLsizei n, const GLuint* ids);
typedef void(GL_APIENTRYP PFNGLGENTRANSFORMFEEDBACKSPROC)(GLsizei n, GLuint* ids);
typedef GLboolean(GL_APIENTRYP PFNGLISTRANSFORMFEEDBACKPROC)(GLuint id);
typedef void(GL_APIENTRYP PFNGLPAUSETRANSFORMFEEDBACKPROC)(void);
typedef void(GL_APIENTRYP PFNGLRESUMETRANSFORMFEEDBACKPROC)(void);
typedef void(GL_APIENTRYP PFNGLGETPROGRAMBINARYPROC)(GLuint   program,
                                                     GLsizei  bufSize,
                                                     GLsizei* length,
                                                     GLenum*  binaryFormat,
                                                     void*    binary);
typedef void(GL_APIENTRYP PFNGLPROGRAMBINARYPROC)(GLuint      program,
                                                  GLenum      binaryFormat,
                                                  const void* binary,
                                                  GLsizei     length);
typedef void(GL_APIENTRYP PFNGLPROGRAMPARAMETERIPROC)(GLuint program, GLenum pname, GLint value);
typedef void(GL_APIENTRYP PFNGLINVALIDATEFRAMEBUFFERPROC)(GLenum        target,
                                                          GLsizei       numAttachments,
                                                          const GLenum* attachments);
typedef void(GL_APIENTRYP PFNGLINVALIDATESUBFRAMEBUFFERPROC)(GLenum        target,
                                                             GLsizei       numAttachments,
                                                             const GLenum* attachments,
                                                             GLint         x,
                                                             GLint         y,
                                                             GLsizei       width,
                                                             GLsizei       height);
typedef void(GL_APIENTRYP PFNGLTEXSTORAGE2DPROC)(GLenum  target,
                                                 GLsizei levels,
                                                 GLenum  internalformat,
                                                 GLsizei width,
                                                 GLsizei height);
typedef void(GL_APIENTRYP PFNGLTEXSTORAGE3DPROC)(GLenum  target,
                                                 GLsizei levels,
                                                 GLenum  internalformat,
                                                 GLsizei width,
                                                 GLsizei height,
                                                 GLsizei depth);
typedef void(GL_APIENTRYP PFNGLGETINTERNALFORMATIVPROC)(GLenum  target,
                                                        GLenum  internalformat,
                                                        GLenum  pname,
                                                        GLsizei bufSize,
                                                        GLint*  params);
// #endif GL_ES_VERSION_3_0

// #ifndef GL_ES_VERSION_3_1
typedef void(GL_APIENTRYP PFNGLDISPATCHCOMPUTEPROC)(GLuint num_groups_x,
                                                    GLuint num_groups_y,
                                                    GLuint num_groups_z);
typedef void(GL_APIENTRYP PFNGLDISPATCHCOMPUTEINDIRECTPROC)(GLintptr indirect);
typedef void(GL_APIENTRYP PFNGLDRAWARRAYSINDIRECTPROC)(GLenum mode, const void* indirect);
typedef void(GL_APIENTRYP PFNGLDRAWELEMENTSINDIRECTPROC)(GLenum      mode,
                                                         GLenum      type,
                                                         const void* indirect);
typedef void(GL_APIENTRYP PFNGLFRAMEBUFFERPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void(GL_APIENTRYP PFNGLGETFRAMEBUFFERPARAMETERIVPROC)(GLenum target,
                                                              GLenum pname,
                                                              GLint* params);
typedef void(GL_APIENTRYP PFNGLGETPROGRAMINTERFACEIVPROC)(GLuint program,
                                                          GLenum programInterface,
                                                          GLenum pname,
                                                          GLint* params);
typedef GLuint(GL_APIENTRYP PFNGLGETPROGRAMRESOURCEINDEXPROC)(GLuint        program,
                                                              GLenum        programInterface,
                                                              const GLchar* name);
typedef void(GL_APIENTRYP PFNGLGETPROGRAMRESOURCENAMEPROC)(GLuint   program,
                                                           GLenum   programInterface,
                                                           GLuint   index,
                                                           GLsizei  bufSize,
                                                           GLsizei* length,
                                                           GLchar*  name);
typedef void(GL_APIENTRYP PFNGLGETPROGRAMRESOURCEIVPROC)(GLuint        program,
                                                         GLenum        programInterface,
                                                         GLuint        index,
                                                         GLsizei       propCount,
                                                         const GLenum* props,
                                                         GLsizei       bufSize,
                                                         GLsizei*      length,
                                                         GLint*        params);
typedef GLint(GL_APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONPROC)(GLuint        program,
                                                                GLenum        programInterface,
                                                                const GLchar* name);
typedef void(GL_APIENTRYP PFNGLUSEPROGRAMSTAGESPROC)(GLuint     pipeline,
                                                     GLbitfield stages,
                                                     GLuint     program);
typedef void(GL_APIENTRYP PFNGLACTIVESHADERPROGRAMPROC)(GLuint pipeline, GLuint program);
typedef GLuint(GL_APIENTRYP PFNGLCREATESHADERPROGRAMVPROC)(GLenum               type,
                                                           GLsizei              count,
                                                           const GLchar* const* strings);
typedef void(GL_APIENTRYP PFNGLBINDPROGRAMPIPELINEPROC)(GLuint pipeline);
typedef void(GL_APIENTRYP PFNGLDELETEPROGRAMPIPELINESPROC)(GLsizei n, const GLuint* pipelines);
typedef void(GL_APIENTRYP PFNGLGENPROGRAMPIPELINESPROC)(GLsizei n, GLuint* pipelines);
typedef GLboolean(GL_APIENTRYP PFNGLISPROGRAMPIPELINEPROC)(GLuint pipeline);
typedef void(GL_APIENTRYP PFNGLGETPROGRAMPIPELINEIVPROC)(GLuint pipeline,
                                                         GLenum pname,
                                                         GLint* params);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM1IPROC)(GLuint program, GLint location, GLint v0);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM2IPROC)(GLuint program,
                                                     GLint  location,
                                                     GLint  v0,
                                                     GLint  v1);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM3IPROC)(GLuint program,
                                                     GLint  location,
                                                     GLint  v0,
                                                     GLint  v1,
                                                     GLint  v2);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM4IPROC)(GLuint program,
                                                     GLint  location,
                                                     GLint  v0,
                                                     GLint  v1,
                                                     GLint  v2,
                                                     GLint  v3);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM1UIPROC)(GLuint program, GLint location, GLuint v0);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM2UIPROC)(GLuint program,
                                                      GLint  location,
                                                      GLuint v0,
                                                      GLuint v1);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM3UIPROC)(GLuint program,
                                                      GLint  location,
                                                      GLuint v0,
                                                      GLuint v1,
                                                      GLuint v2);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM4UIPROC)(GLuint program,
                                                      GLint  location,
                                                      GLuint v0,
                                                      GLuint v1,
                                                      GLuint v2,
                                                      GLuint v3);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM1FPROC)(GLuint program, GLint location, GLfloat v0);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM2FPROC)(GLuint  program,
                                                     GLint   location,
                                                     GLfloat v0,
                                                     GLfloat v1);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM3FPROC)(GLuint  program,
                                                     GLint   location,
                                                     GLfloat v0,
                                                     GLfloat v1,
                                                     GLfloat v2);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM4FPROC)(GLuint  program,
                                                     GLint   location,
                                                     GLfloat v0,
                                                     GLfloat v1,
                                                     GLfloat v2,
                                                     GLfloat v3);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM1IVPROC)(GLuint       program,
                                                      GLint        location,
                                                      GLsizei      count,
                                                      const GLint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM2IVPROC)(GLuint       program,
                                                      GLint        location,
                                                      GLsizei      count,
                                                      const GLint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM3IVPROC)(GLuint       program,
                                                      GLint        location,
                                                      GLsizei      count,
                                                      const GLint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM4IVPROC)(GLuint       program,
                                                      GLint        location,
                                                      GLsizei      count,
                                                      const GLint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM1UIVPROC)(GLuint        program,
                                                       GLint         location,
                                                       GLsizei       count,
                                                       const GLuint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM2UIVPROC)(GLuint        program,
                                                       GLint         location,
                                                       GLsizei       count,
                                                       const GLuint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM3UIVPROC)(GLuint        program,
                                                       GLint         location,
                                                       GLsizei       count,
                                                       const GLuint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM4UIVPROC)(GLuint        program,
                                                       GLint         location,
                                                       GLsizei       count,
                                                       const GLuint* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM1FVPROC)(GLuint         program,
                                                      GLint          location,
                                                      GLsizei        count,
                                                      const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM2FVPROC)(GLuint         program,
                                                      GLint          location,
                                                      GLsizei        count,
                                                      const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM3FVPROC)(GLuint         program,
                                                      GLint          location,
                                                      GLsizei        count,
                                                      const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORM4FVPROC)(GLuint         program,
                                                      GLint          location,
                                                      GLsizei        count,
                                                      const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVPROC)(GLuint         program,
                                                            GLint          location,
                                                            GLsizei        count,
                                                            GLboolean      transpose,
                                                            const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVPROC)(GLuint         program,
                                                            GLint          location,
                                                            GLsizei        count,
                                                            GLboolean      transpose,
                                                            const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVPROC)(GLuint         program,
                                                            GLint          location,
                                                            GLsizei        count,
                                                            GLboolean      transpose,
                                                            const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)(GLuint         program,
                                                              GLint          location,
                                                              GLsizei        count,
                                                              GLboolean      transpose,
                                                              const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)(GLuint         program,
                                                              GLint          location,
                                                              GLsizei        count,
                                                              GLboolean      transpose,
                                                              const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)(GLuint         program,
                                                              GLint          location,
                                                              GLsizei        count,
                                                              GLboolean      transpose,
                                                              const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)(GLuint         program,
                                                              GLint          location,
                                                              GLsizei        count,
                                                              GLboolean      transpose,
                                                              const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)(GLuint         program,
                                                              GLint          location,
                                                              GLsizei        count,
                                                              GLboolean      transpose,
                                                              const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)(GLuint         program,
                                                              GLint          location,
                                                              GLsizei        count,
                                                              GLboolean      transpose,
                                                              const GLfloat* value);
typedef void(GL_APIENTRYP PFNGLVALIDATEPROGRAMPIPELINEPROC)(GLuint pipeline);
typedef void(GL_APIENTRYP PFNGLGETPROGRAMPIPELINEINFOLOGPROC)(GLuint   pipeline,
                                                              GLsizei  bufSize,
                                                              GLsizei* length,
                                                              GLchar*  infoLog);
typedef void(GL_APIENTRYP PFNGLBINDIMAGETEXTUREPROC)(GLuint    unit,
                                                     GLuint    texture,
                                                     GLint     level,
                                                     GLboolean layered,
                                                     GLint     layer,
                                                     GLenum    access,
                                                     GLenum    format);
typedef void(GL_APIENTRYP PFNGLGETBOOLEANI_VPROC)(GLenum target, GLuint index, GLboolean* data);
typedef void(GL_APIENTRYP PFNGLMEMORYBARRIERPROC)(GLbitfield barriers);
typedef void(GL_APIENTRYP PFNGLMEMORYBARRIERBYREGIONPROC)(GLbitfield barriers);
typedef void(GL_APIENTRYP PFNGLTEXSTORAGE2DMULTISAMPLEPROC)(GLenum    target,
                                                            GLsizei   samples,
                                                            GLenum    internalformat,
                                                            GLsizei   width,
                                                            GLsizei   height,
                                                            GLboolean fixedsamplelocations);
typedef void(GL_APIENTRYP PFNGLGETMULTISAMPLEFVPROC)(GLenum pname, GLuint index, GLfloat* val);
typedef void(GL_APIENTRYP PFNGLSAMPLEMASKIPROC)(GLuint maskNumber, GLbitfield mask);
typedef void(GL_APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum target,
                                                           GLint  level,
                                                           GLenum pname,
                                                           GLint* params);
typedef void(GL_APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC)(GLenum   target,
                                                           GLint    level,
                                                           GLenum   pname,
                                                           GLfloat* params);
typedef void(GL_APIENTRYP PFNGLBINDVERTEXBUFFERPROC)(GLuint   bindingindex,
                                                     GLuint   buffer,
                                                     GLintptr offset,
                                                     GLsizei  stride);
typedef void(GL_APIENTRYP PFNGLVERTEXATTRIBFORMATPROC)(GLuint    attribindex,
                                                       GLint     size,
                                                       GLenum    type,
                                                       GLboolean normalized,
                                                       GLuint    relativeoffset);
typedef void(GL_APIENTRYP PFNGLVERTEXATTRIBIFORMATPROC)(GLuint attribindex,
                                                        GLint  size,
                                                        GLenum type,
                                                        GLuint relativeoffset);
typedef void(GL_APIENTRYP PFNGLVERTEXATTRIBBINDINGPROC)(GLuint attribindex, GLuint bindingindex);
typedef void(GL_APIENTRYP PFNGLVERTEXBINDINGDIVISORPROC)(GLuint bindingindex, GLuint divisor);
// #endif GL_ES_VERSION_3_1

// #ifndef GL_ES_VERSION_3_2
typedef void(GL_APIENTRY* GLDEBUGPROC)(GLenum        source,
                                       GLenum        type,
                                       GLuint        id,
                                       GLenum        severity,
                                       GLsizei       length,
                                       const GLchar* message,
                                       const void*   userParam);

typedef void(GL_APIENTRYP PFNGLBLENDBARRIERPROC)(void);
typedef void(GL_APIENTRYP PFNGLCOPYIMAGESUBDATAPROC)(GLuint  srcName,
                                                     GLenum  srcTarget,
                                                     GLint   srcLevel,
                                                     GLint   srcX,
                                                     GLint   srcY,
                                                     GLint   srcZ,
                                                     GLuint  dstName,
                                                     GLenum  dstTarget,
                                                     GLint   dstLevel,
                                                     GLint   dstX,
                                                     GLint   dstY,
                                                     GLint   dstZ,
                                                     GLsizei srcWidth,
                                                     GLsizei srcHeight,
                                                     GLsizei srcDepth);
typedef void(GL_APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC)(GLenum        source,
                                                        GLenum        type,
                                                        GLenum        severity,
                                                        GLsizei       count,
                                                        const GLuint* ids,
                                                        GLboolean     enabled);
typedef void(GL_APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC)(GLenum        source,
                                                       GLenum        type,
                                                       GLuint        id,
                                                       GLenum        severity,
                                                       GLsizei       length,
                                                       const GLchar* buf);
typedef void(GL_APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback,
                                                         const void* userParam);
typedef GLuint(GL_APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC)(GLuint   count,
                                                         GLsizei  bufSize,
                                                         GLenum*  sources,
                                                         GLenum*  types,
                                                         GLuint*  ids,
                                                         GLenum*  severities,
                                                         GLsizei* lengths,
                                                         GLchar*  messageLog);
typedef void(GL_APIENTRYP PFNGLPUSHDEBUGGROUPPROC)(GLenum        source,
                                                   GLuint        id,
                                                   GLsizei       length,
                                                   const GLchar* message);
typedef void(GL_APIENTRYP PFNGLPOPDEBUGGROUPPROC)(void);
typedef void(GL_APIENTRYP PFNGLOBJECTLABELPROC)(GLenum        identifier,
                                                GLuint        name,
                                                GLsizei       length,
                                                const GLchar* label);
typedef void(GL_APIENTRYP PFNGLGETOBJECTLABELPROC)(GLenum   identifier,
                                                   GLuint   name,
                                                   GLsizei  bufSize,
                                                   GLsizei* length,
                                                   GLchar*  label);
typedef void(GL_APIENTRYP PFNGLOBJECTPTRLABELPROC)(const void*   ptr,
                                                   GLsizei       length,
                                                   const GLchar* label);
typedef void(GL_APIENTRYP PFNGLGETOBJECTPTRLABELPROC)(const void* ptr,
                                                      GLsizei     bufSize,
                                                      GLsizei*    length,
                                                      GLchar*     label);
typedef void(GL_APIENTRYP PFNGLGETPOINTERVPROC)(GLenum pname, void** params);
typedef void(GL_APIENTRYP PFNGLENABLEIPROC)(GLenum target, GLuint index);
typedef void(GL_APIENTRYP PFNGLDISABLEIPROC)(GLenum target, GLuint index);
typedef void(GL_APIENTRYP PFNGLBLENDEQUATIONIPROC)(GLuint buf, GLenum mode);
typedef void(GL_APIENTRYP PFNGLBLENDEQUATIONSEPARATEIPROC)(GLuint buf,
                                                           GLenum modeRGB,
                                                           GLenum modeAlpha);
typedef void(GL_APIENTRYP PFNGLBLENDFUNCIPROC)(GLuint buf, GLenum src, GLenum dst);
typedef void(GL_APIENTRYP PFNGLBLENDFUNCSEPARATEIPROC)(GLuint buf,
                                                       GLenum srcRGB,
                                                       GLenum dstRGB,
                                                       GLenum srcAlpha,
                                                       GLenum dstAlpha);
typedef void(GL_APIENTRYP PFNGLCOLORMASKIPROC)(GLuint    index,
                                               GLboolean r,
                                               GLboolean g,
                                               GLboolean b,
                                               GLboolean a);
typedef GLboolean(GL_APIENTRYP PFNGLISENABLEDIPROC)(GLenum target, GLuint index);
typedef void(GL_APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXPROC)(GLenum      mode,
                                                           GLsizei     count,
                                                           GLenum      type,
                                                           const void* indices,
                                                           GLint       basevertex);
typedef void(GL_APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)(GLenum      mode,
                                                                GLuint      start,
                                                                GLuint      end,
                                                                GLsizei     count,
                                                                GLenum      type,
                                                                const void* indices,
                                                                GLint       basevertex);
typedef void(GL_APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)(GLenum      mode,
                                                                    GLsizei     count,
                                                                    GLenum      type,
                                                                    const void* indices,
                                                                    GLsizei     instancecount,
                                                                    GLint       basevertex);
typedef void(GL_APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC)(GLenum target,
                                                       GLenum attachment,
                                                       GLuint texture,
                                                       GLint  level);
typedef void(GL_APIENTRYP PFNGLPRIMITIVEBOUNDINGBOXPROC)(GLfloat minX,
                                                         GLfloat minY,
                                                         GLfloat minZ,
                                                         GLfloat minW,
                                                         GLfloat maxX,
                                                         GLfloat maxY,
                                                         GLfloat maxZ,
                                                         GLfloat maxW);
typedef GLenum(GL_APIENTRYP PFNGLGETGRAPHICSRESETSTATUSPROC)(void);
typedef void(GL_APIENTRYP PFNGLREADNPIXELSPROC)(GLint   x,
                                                GLint   y,
                                                GLsizei width,
                                                GLsizei height,
                                                GLenum  format,
                                                GLenum  type,
                                                GLsizei bufSize,
                                                void*   data);
typedef void(GL_APIENTRYP PFNGLGETNUNIFORMFVPROC)(GLuint   program,
                                                  GLint    location,
                                                  GLsizei  bufSize,
                                                  GLfloat* params);
typedef void(GL_APIENTRYP PFNGLGETNUNIFORMIVPROC)(GLuint  program,
                                                  GLint   location,
                                                  GLsizei bufSize,
                                                  GLint*  params);
typedef void(GL_APIENTRYP PFNGLGETNUNIFORMUIVPROC)(GLuint  program,
                                                   GLint   location,
                                                   GLsizei bufSize,
                                                   GLuint* params);
typedef void(GL_APIENTRYP PFNGLMINSAMPLESHADINGPROC)(GLfloat value);
typedef void(GL_APIENTRYP PFNGLPATCHPARAMETERIPROC)(GLenum pname, GLint value);
typedef void(GL_APIENTRYP PFNGLTEXPARAMETERIIVPROC)(GLenum       target,
                                                    GLenum       pname,
                                                    const GLint* params);
typedef void(GL_APIENTRYP PFNGLTEXPARAMETERIUIVPROC)(GLenum        target,
                                                     GLenum        pname,
                                                     const GLuint* params);
typedef void(GL_APIENTRYP PFNGLGETTEXPARAMETERIIVPROC)(GLenum target, GLenum pname, GLint* params);
typedef void(GL_APIENTRYP PFNGLGETTEXPARAMETERIUIVPROC)(GLenum  target,
                                                        GLenum  pname,
                                                        GLuint* params);
typedef void(GL_APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC)(GLuint       sampler,
                                                        GLenum       pname,
                                                        const GLint* param);
typedef void(GL_APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC)(GLuint        sampler,
                                                         GLenum        pname,
                                                         const GLuint* param);
typedef void(GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC)(GLuint sampler,
                                                           GLenum pname,
                                                           GLint* params);
typedef void(GL_APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC)(GLuint  sampler,
                                                            GLenum  pname,
                                                            GLuint* params);
typedef void(GL_APIENTRYP PFNGLTEXBUFFERPROC)(GLenum target, GLenum internalformat, GLuint buffer);
typedef void(GL_APIENTRYP PFNGLTEXBUFFERRANGEPROC)(GLenum     target,
                                                   GLenum     internalformat,
                                                   GLuint     buffer,
                                                   GLintptr   offset,
                                                   GLsizeiptr size);
typedef void(GL_APIENTRYP PFNGLTEXSTORAGE3DMULTISAMPLEPROC)(GLenum    target,
                                                            GLsizei   samples,
                                                            GLenum    internalformat,
                                                            GLsizei   width,
                                                            GLsizei   height,
                                                            GLsizei   depth,
                                                            GLboolean fixedsamplelocations);
// #endif GL_ES_VERSION_3_2

} // namespace opencascade

#endif // _OpenGl_GLESExtensions_Header
