// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_VertexBuffer.hxx>

#include <OpenGl_ShaderManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_VertexBuffer, OpenGl_Buffer)

//=================================================================================================

OpenGl_VertexBuffer::OpenGl_VertexBuffer()
{
  //
}

//=================================================================================================

OpenGl_VertexBuffer::~OpenGl_VertexBuffer()
{
  //
}

//=================================================================================================

unsigned int OpenGl_VertexBuffer::GetTarget() const
{
  return GL_ARRAY_BUFFER;
}

//=================================================================================================

void OpenGl_VertexBuffer::BindVertexAttrib(const occ::handle<OpenGl_Context>& theGlCtx,
                                           const unsigned int                 theAttribLoc) const
{
  if (!IsValid() || theAttribLoc == GLuint(-1))
  {
    return;
  }
  Bind(theGlCtx);
  theGlCtx->core20fwd->glEnableVertexAttribArray(theAttribLoc);
  theGlCtx->core20fwd
    ->glVertexAttribPointer(theAttribLoc, GLint(myComponentsNb), myDataType, GL_FALSE, 0, myOffset);
}

//=================================================================================================

void OpenGl_VertexBuffer::UnbindVertexAttrib(const occ::handle<OpenGl_Context>& theGlCtx,
                                             const unsigned int                 theAttribLoc) const
{
  if (!IsValid() || theAttribLoc == GLuint(-1))
  {
    return;
  }
  theGlCtx->core20fwd->glDisableVertexAttribArray(theAttribLoc);
  Unbind(theGlCtx);
}

//=================================================================================================

void OpenGl_VertexBuffer::BindAllAttributes(const occ::handle<OpenGl_Context>&) const
{
  //
}

//=================================================================================================

void OpenGl_VertexBuffer::BindPositionAttribute(const occ::handle<OpenGl_Context>&) const
{
  //
}

//=================================================================================================

void OpenGl_VertexBuffer::UnbindAllAttributes(const occ::handle<OpenGl_Context>&) const
{
  //
}

//=================================================================================================

bool OpenGl_VertexBuffer::HasColorAttribute() const
{
  return false;
}

//=================================================================================================

bool OpenGl_VertexBuffer::HasNormalAttribute() const
{
  return false;
}

//=================================================================================================

void OpenGl_VertexBuffer::bindAttribute(const occ::handle<OpenGl_Context>& theCtx,
                                        const Graphic3d_TypeOfAttribute    theAttribute,
                                        const int                          theNbComp,
                                        const unsigned int                 theDataType,
                                        const int                          theStride,
                                        const void*                        theOffset)
{
  if (theCtx->ActiveProgram().IsNull())
  {
    if (theCtx->core11ffp != nullptr)
    {
      bindFixed(theCtx, theAttribute, theNbComp, theDataType, theStride, theOffset);
    }
    else
    {
      // OpenGL handles vertex attribute setup independently from active GLSL program,
      // but OCCT historically requires program to be bound beforehand (this check could be removed
      // in future).
      Message::SendFail(
        "Error: OpenGl_VertexBuffer::bindAttribute() does nothing without active GLSL program");
    }
    return;
  }

  theCtx->core20fwd->glEnableVertexAttribArray(theAttribute);
  theCtx->core20fwd->glVertexAttribPointer(theAttribute,
                                           theNbComp,
                                           theDataType,
                                           theDataType != GL_FLOAT,
                                           theStride,
                                           theOffset);
}

//=================================================================================================

void OpenGl_VertexBuffer::unbindAttribute(const occ::handle<OpenGl_Context>& theCtx,
                                          const Graphic3d_TypeOfAttribute    theAttribute)
{
  if (theCtx->ActiveProgram().IsNull())
  {
    if (theCtx->core11ffp != nullptr)
    {
      unbindFixed(theCtx, theAttribute);
    }
    return;
  }

  theCtx->core20fwd->glDisableVertexAttribArray(theAttribute);
}

//=================================================================================================

void OpenGl_VertexBuffer::bindFixed(const occ::handle<OpenGl_Context>& theCtx,
                                    const Graphic3d_TypeOfAttribute    theMode,
                                    const int                          theNbComp,
                                    const unsigned int                 theDataType,
                                    const int                          theStride,
                                    const void*                        theOffset)
{
  switch (theMode)
  {
    case Graphic3d_TOA_POS: {
      theCtx->core11ffp->glEnableClientState(GL_VERTEX_ARRAY);
      theCtx->core11ffp->glVertexPointer(theNbComp, theDataType, theStride, theOffset);
      return;
    }
    case Graphic3d_TOA_NORM: {
      theCtx->core11ffp->glEnableClientState(GL_NORMAL_ARRAY);
      theCtx->core11ffp->glNormalPointer(theDataType, theStride, theOffset);
      return;
    }
    case Graphic3d_TOA_UV: {
      theCtx->core11ffp->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      theCtx->core11ffp->glTexCoordPointer(theNbComp, theDataType, theStride, theOffset);
      return;
    }
    case Graphic3d_TOA_COLOR: {
      theCtx->core11ffp->glEnableClientState(GL_COLOR_ARRAY);
      theCtx->core11ffp->glColorPointer(theNbComp, theDataType, theStride, theOffset);
      theCtx->core11ffp->glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      theCtx->core11fwd->glEnable(GL_COLOR_MATERIAL);
      return;
    }
    case Graphic3d_TOA_CUSTOM: {
      return;
    }
  }
}

//=================================================================================================

void OpenGl_VertexBuffer::unbindFixed(const occ::handle<OpenGl_Context>& theCtx,
                                      const Graphic3d_TypeOfAttribute    theMode)
{
  switch (theMode)
  {
    case Graphic3d_TOA_POS:
      theCtx->core11ffp->glDisableClientState(GL_VERTEX_ARRAY);
      return;
    case Graphic3d_TOA_NORM:
      theCtx->core11ffp->glDisableClientState(GL_NORMAL_ARRAY);
      return;
    case Graphic3d_TOA_UV:
      theCtx->core11ffp->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      return;
    case Graphic3d_TOA_COLOR:
      unbindFixedColor(theCtx);
      return;
    case Graphic3d_TOA_CUSTOM: {
      return;
    }
  }
}

//=================================================================================================

void OpenGl_VertexBuffer::unbindFixedColor(const occ::handle<OpenGl_Context>& theCtx)
{
  theCtx->core11ffp->glDisableClientState(GL_COLOR_ARRAY);
  theCtx->core11fwd->glDisable(GL_COLOR_MATERIAL);

  // invalidate FFP material state after GL_COLOR_MATERIAL has modified it (took values from the
  // vertex color)
  theCtx->ShaderManager()->UpdateMaterialState();
}
