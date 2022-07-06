// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#include <OpenGl_PrimitiveArray.hxx>

#include <OpenGl_PointSprite.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_VertexBufferCompat.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

namespace
{
  //! Convert data type to GL info
  inline GLenum toGlDataType (const Graphic3d_TypeOfData theType,
                              GLint&                     theNbComp)
  {
    switch (theType)
    {
      case Graphic3d_TOD_USHORT:
        theNbComp = 1;
        return GL_UNSIGNED_SHORT;
      case Graphic3d_TOD_UINT:
        theNbComp = 1;
        return GL_UNSIGNED_INT;
      case Graphic3d_TOD_VEC2:
        theNbComp = 2;
        return GL_FLOAT;
      case Graphic3d_TOD_VEC3:
        theNbComp = 3;
        return GL_FLOAT;
      case Graphic3d_TOD_VEC4:
        theNbComp = 4;
        return GL_FLOAT;
      case Graphic3d_TOD_VEC4UB:
        theNbComp = 4;
        return GL_UNSIGNED_BYTE;
      case Graphic3d_TOD_FLOAT:
        theNbComp = 1;
        return GL_FLOAT;
    }
    theNbComp = 0;
    return GL_NONE;
  }

}

//! Auxiliary template for VBO with interleaved attributes.
template<class TheBaseClass, int NbAttributes>
class OpenGl_VertexBufferT : public TheBaseClass
{
public:

  //! Create uninitialized VBO.
  OpenGl_VertexBufferT (const Graphic3d_Buffer& theAttribs)
  : Stride (theAttribs.IsInterleaved() ? theAttribs.Stride : 0)
  {
    memcpy (Attribs, theAttribs.AttributesArray(), sizeof(Graphic3d_Attribute) * NbAttributes);
  }

  virtual bool HasColorAttribute() const
  {
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      if (anAttrib.Id == Graphic3d_TOA_COLOR)
      {
        return true;
      }
    }
    return false;
  }

  virtual bool HasNormalAttribute() const
  {
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      if (anAttrib.Id == Graphic3d_TOA_NORM)
      {
        return true;
      }
    }
    return false;
  }

  virtual void BindPositionAttribute (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!TheBaseClass::IsValid())
    {
      return;
    }

    TheBaseClass::Bind (theGlCtx);
    GLint aNbComp;
    const GLubyte* anOffset = TheBaseClass::myOffset;
    const Standard_Size aMuliplier = Stride != 0 ? 1 : TheBaseClass::myElemsNb;
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      const GLenum   aDataType = toGlDataType (anAttrib.DataType, aNbComp);
      if (anAttrib.Id == Graphic3d_TOA_POS
       && aDataType != GL_NONE)
      {
        TheBaseClass::bindAttribute (theGlCtx, Graphic3d_TOA_POS, aNbComp, aDataType, Stride, anOffset);
        break;
      }

      anOffset += aMuliplier * Graphic3d_Attribute::Stride (anAttrib.DataType);
    }
  }

  virtual void BindAllAttributes (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!TheBaseClass::IsValid())
    {
      return;
    }

    TheBaseClass::Bind (theGlCtx);
    GLint aNbComp;
    const GLubyte* anOffset = TheBaseClass::myOffset;
    const Standard_Size aMuliplier = Stride != 0 ? 1 : TheBaseClass::myElemsNb;
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      const GLenum   aDataType = toGlDataType (anAttrib.DataType, aNbComp);
      if (aDataType != GL_NONE)
      {
        TheBaseClass::bindAttribute (theGlCtx, anAttrib.Id, aNbComp, aDataType, Stride, anOffset);
      }
      anOffset += aMuliplier * Graphic3d_Attribute::Stride (anAttrib.DataType);
    }
  }

  virtual void UnbindAllAttributes (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!TheBaseClass::IsValid())
    {
      return;
    }
    TheBaseClass::Unbind (theGlCtx);

    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      TheBaseClass::unbindAttribute (theGlCtx, anAttrib.Id);
    }
  }

private:

  Graphic3d_Attribute Attribs[NbAttributes];
  Standard_Integer    Stride;

};

// =======================================================================
// function : clearMemoryGL
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::clearMemoryGL (const Handle(OpenGl_Context)& theGlCtx) const
{
  if (!myVboIndices.IsNull())
  {
    myVboIndices->Release (theGlCtx.operator->());
    myVboIndices.Nullify();
  }
  if (!myVboAttribs.IsNull())
  {
    myVboAttribs->Release (theGlCtx.operator->());
    myVboAttribs.Nullify();
  }
}

// =======================================================================
// function : initNormalVbo
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_PrimitiveArray::initNormalVbo (const Handle(OpenGl_Context)& theCtx) const
{
  switch (myAttribs->NbAttributes)
  {
    case 1:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 1> (*myAttribs); break;
    case 2:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 2> (*myAttribs); break;
    case 3:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 3> (*myAttribs); break;
    case 4:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 4> (*myAttribs); break;
    case 5:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 5> (*myAttribs); break;
    case 6:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 6> (*myAttribs); break;
    case 7:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 7> (*myAttribs); break;
    case 8:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 8> (*myAttribs); break;
    case 9:  myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 9> (*myAttribs); break;
    case 10: myVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBuffer, 10>(*myAttribs); break;
  }

  const Standard_Boolean isAttribMutable     = myAttribs->IsMutable();
  const Standard_Boolean isAttribInterleaved = myAttribs->IsInterleaved();
  if (myAttribs->NbElements != myAttribs->NbMaxElements()
   && myIndices.IsNull()
   && (!isAttribInterleaved || isAttribMutable))
  {
    throw Standard_ProgramError ("OpenGl_PrimitiveArray::buildVBO() - vertex attribute data with reserved size is not supported");
  }

  // specify data type as Byte and NbComponents as Stride, so that OpenGl_VertexBuffer::EstimatedDataSize() will return correct value
  const Standard_Integer aNbVertexes = (isAttribMutable || !isAttribInterleaved) ? myAttribs->NbMaxElements() : myAttribs->NbElements;
  if (!myVboAttribs->init (theCtx, myAttribs->Stride, aNbVertexes, myAttribs->Data(), GL_UNSIGNED_BYTE, myAttribs->Stride))
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString("VBO creation for Primitive Array has failed for ") + aNbVertexes + " vertices. Out of memory?";
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_LOW, aMsg);

    clearMemoryGL (theCtx);
    return Standard_False;
  }
  else if (myIndices.IsNull())
  {
    if (isAttribMutable && isAttribInterleaved)
    {
      // for mutable interlaced array we can change dynamically number of vertexes (they will be just skipped at the end of buffer);
      // this doesn't matter in case if we have indexed array
      myVboAttribs->SetElemsNb (myAttribs->NbElements);
    }
    return Standard_True;
  }

  const Standard_Integer aNbIndexes = !myIndices->IsMutable() ? myIndices->NbElements : myIndices->NbMaxElements();
  myVboIndices = new OpenGl_IndexBuffer();
  bool isOk = false;
  switch (myIndices->Stride)
  {
    case 2:
    {
      isOk = myVboIndices->Init (theCtx, 1, aNbIndexes, reinterpret_cast<const GLushort*> (myIndices->Data()));
      myVboIndices->SetElemsNb (myIndices->NbElements);
      myIndices->Validate();
      break;
    }
    case 4:
    {
      isOk = myVboIndices->Init (theCtx, 1, aNbIndexes, reinterpret_cast<const GLuint*> (myIndices->Data()));
      myVboIndices->SetElemsNb (myIndices->NbElements);
      myIndices->Validate();
      break;
    }
    default:
    {
      clearMemoryGL (theCtx);
      return Standard_False;
    }
  }
  if (!isOk)
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString("VBO creation for Primitive Array has failed for ") + aNbIndexes + " indices. Out of memory?";
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
    clearMemoryGL (theCtx);
    return Standard_False;
  }
  return Standard_True;
}

// =======================================================================
// function : buildVBO
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_PrimitiveArray::buildVBO (const Handle(OpenGl_Context)& theCtx,
                                                  const Standard_Boolean        theToKeepData) const
{
  bool isNormalMode = theCtx->ToUseVbo();
  clearMemoryGL (theCtx);
  if (myAttribs.IsNull()
   || myAttribs->IsEmpty()
   || myAttribs->NbElements < 1
   || myAttribs->NbAttributes < 1
   || myAttribs->NbAttributes > 10)
  {
    // vertices should be always defined - others are optional
    return Standard_False;
  }

  if (isNormalMode
   && initNormalVbo (theCtx))
  {
    if (!theCtx->caps->keepArrayData
     && !theToKeepData
     && !myAttribs->IsMutable())
    {
      myIndices.Nullify();
      myAttribs.Nullify();
    }
    else
    {
      myAttribs->Validate();
    }
    return Standard_True;
  }

  Handle(OpenGl_VertexBufferCompat) aVboAttribs;
  switch (myAttribs->NbAttributes)
  {
    case 1:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 1> (*myAttribs); break;
    case 2:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 2> (*myAttribs); break;
    case 3:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 3> (*myAttribs); break;
    case 4:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 4> (*myAttribs); break;
    case 5:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 5> (*myAttribs); break;
    case 6:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 6> (*myAttribs); break;
    case 7:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 7> (*myAttribs); break;
    case 8:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 8> (*myAttribs); break;
    case 9:  aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 9> (*myAttribs); break;
    case 10: aVboAttribs = new OpenGl_VertexBufferT<OpenGl_VertexBufferCompat, 10>(*myAttribs); break;
  }
  aVboAttribs->initLink (myAttribs, 0, myAttribs->NbElements, GL_NONE);
  if (!myIndices.IsNull())
  {
    Handle(OpenGl_IndexBufferCompat) aVboIndices = new OpenGl_IndexBufferCompat();
    switch (myIndices->Stride)
    {
      case 2:
      {
        aVboIndices->initLink (myIndices, 1, myIndices->NbElements, GL_UNSIGNED_SHORT);
        break;
      }
      case 4:
      {
        aVboIndices->initLink (myIndices, 1, myIndices->NbElements, GL_UNSIGNED_INT);
        break;
      }
      default:
      {
        return Standard_False;
      }
    }
    myVboIndices = aVboIndices;
  }
  myVboAttribs = aVboAttribs;
  if (!theCtx->caps->keepArrayData
   && !theToKeepData)
  {
    // does not make sense for compatibility mode
    //myIndices.Nullify();
    //myAttribs.Nullify();
  }

  return Standard_True;
}

// =======================================================================
// function : updateVBO
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::updateVBO (const Handle(OpenGl_Context)& theCtx) const
{
  if (!myAttribs.IsNull())
  {
    Graphic3d_BufferRange aRange = myAttribs->InvalidatedRange();
    if (!aRange.IsEmpty()
     &&  myVboAttribs->IsValid()
     && !myVboAttribs->IsVirtual())
    {
      myVboAttribs->Bind (theCtx);
      theCtx->core15fwd->glBufferSubData (myVboAttribs->GetTarget(),
                                          aRange.Start,
                                          aRange.Length,
                                          myAttribs->Data() + aRange.Start);
      myVboAttribs->Unbind (theCtx);
      if (myAttribs->IsInterleaved())
      {
        myVboAttribs->SetElemsNb (myAttribs->NbElements);
      }
    }
    myAttribs->Validate();
  }
  if (!myIndices.IsNull())
  {
    Graphic3d_BufferRange aRange = myIndices->InvalidatedRange();
    if (!aRange.IsEmpty()
     &&  myVboIndices->IsValid()
     && !myVboIndices->IsVirtual())
    {
      myVboIndices->Bind (theCtx);
      theCtx->core15fwd->glBufferSubData (myVboIndices->GetTarget(),
                                          aRange.Start,
                                          aRange.Length,
                                          myIndices->Data() + aRange.Start);
      myVboIndices->Unbind (theCtx);
      myVboIndices->SetElemsNb (myIndices->NbElements);
    }
    myIndices->Validate();
  }
}

// =======================================================================
// function : drawArray
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::drawArray (const Handle(OpenGl_Workspace)& theWorkspace,
                                       const Graphic3d_Vec4*           theFaceColors,
                                       const Standard_Boolean          theHasVertColor) const
{
  const Handle(OpenGl_Context)& aGlContext = theWorkspace->GetGlContext();
  if (myVboAttribs.IsNull())
  {
    if (myDrawMode == GL_POINTS
     && aGlContext->core11ffp != NULL)
    {
      // extreme compatibility mode - without sprites but with markers
      drawMarkers (theWorkspace);
    }
    return;
  }

  const bool   toHilight = theWorkspace->ToHighlight();
  const GLenum aDrawMode = !aGlContext->ActiveProgram().IsNull()
                         && aGlContext->ActiveProgram()->HasTessellationStage()
                         ? GL_PATCHES
                         : myDrawMode;
  myVboAttribs->BindAllAttributes (aGlContext);
  if (theHasVertColor && toHilight)
  {
    // disable per-vertex color
    OpenGl_VertexBuffer::unbindAttribute (aGlContext, Graphic3d_TOA_COLOR);
  }
  if (!myVboIndices.IsNull())
  {
    myVboIndices->Bind (aGlContext);
    GLubyte* anOffset = myVboIndices->GetDataOffset();
    if (!myBounds.IsNull())
    {
      // draw primitives by vertex count with the indices
      const size_t aStride = myVboIndices->GetDataType() == GL_UNSIGNED_SHORT ? sizeof(unsigned short) : sizeof(unsigned int);
      for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
      {
        const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
        if (theFaceColors != NULL) aGlContext->SetColor4fv (theFaceColors[aGroupIter]);
        aGlContext->core11fwd->glDrawElements (aDrawMode, aNbElemsInGroup, myVboIndices->GetDataType(), anOffset);
        anOffset += aStride * aNbElemsInGroup;
      }
    }
    else
    {
      // draw one (or sequential) primitive by the indices
      aGlContext->core11fwd->glDrawElements (aDrawMode, myVboIndices->GetElemsNb(), myVboIndices->GetDataType(), anOffset);
    }
    myVboIndices->Unbind (aGlContext);
  }
  else if (!myBounds.IsNull())
  {
    GLint aFirstElem = 0;
    for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
    {
      const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
      if (theFaceColors != NULL) aGlContext->SetColor4fv (theFaceColors[aGroupIter]);
      aGlContext->core11fwd->glDrawArrays (aDrawMode, aFirstElem, aNbElemsInGroup);
      aFirstElem += aNbElemsInGroup;
    }
  }
  else
  {
    if (myDrawMode == GL_POINTS)
    {
      drawMarkers (theWorkspace);
    }
    else
    {
      aGlContext->core11fwd->glDrawArrays (aDrawMode, 0, myVboAttribs->GetElemsNb());
    }
  }

  // bind with 0
  myVboAttribs->UnbindAllAttributes (aGlContext);
}

// =======================================================================
// function : drawEdges
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::drawEdges (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aGlContext = theWorkspace->GetGlContext();
  if (myVboAttribs.IsNull())
  {
    return;
  }

  const OpenGl_Aspects* anAspect = theWorkspace->Aspects();
  const Standard_Integer aPolyModeOld = aGlContext->SetPolygonMode (GL_LINE);

  if (aGlContext->core20fwd != NULL)
  {
    aGlContext->ShaderManager()->BindLineProgram (Handle(OpenGl_TextureSet)(), anAspect->Aspect()->EdgeLineType(),
                                                  Graphic3d_TypeOfShadingModel_Unlit, Graphic3d_AlphaMode_Opaque, Standard_False,
                                                  anAspect->ShaderProgramRes (aGlContext));
  }
  aGlContext->SetSampleAlphaToCoverage (aGlContext->ShaderManager()->MaterialState().HasAlphaCutoff());
  const GLenum aDrawMode = !aGlContext->ActiveProgram().IsNull()
                         && aGlContext->ActiveProgram()->HasTessellationStage()
                         ? GL_PATCHES
                         : myDrawMode;
  if (aGlContext->ActiveProgram().IsNull()
   && aGlContext->core11ffp != NULL)
  {
    aGlContext->core11fwd->glDisable (GL_LIGHTING);
  }

  /// OCC22236 NOTE: draw edges for all situations:
  /// 1) draw elements with GL_LINE style as edges from myPArray->bufferVBO[VBOEdges] indices array
  /// 2) draw elements from vertex array, when bounds defines count of primitive's vertices.
  /// 3) draw primitive's edges by vertexes if no edges and bounds array is specified
  myVboAttribs->BindPositionAttribute (aGlContext);

  aGlContext->SetColor4fv (theWorkspace->EdgeColor().a() >= 0.1f
                         ? theWorkspace->EdgeColor()
                         : theWorkspace->View()->BackgroundColor());
  aGlContext->SetLineStipple((float )anAspect->Aspect()->LineStippleFactor(), anAspect->Aspect()->LinePattern());
  aGlContext->SetLineWidth  (anAspect->Aspect()->EdgeWidth());

  if (!myVboIndices.IsNull())
  {
    myVboIndices->Bind (aGlContext);
    GLubyte* anOffset = myVboIndices->GetDataOffset();

    // draw primitives by vertex count with the indices
    if (!myBounds.IsNull())
    {
      const size_t aStride = myVboIndices->GetDataType() == GL_UNSIGNED_SHORT ? sizeof(unsigned short) : sizeof(unsigned int);
      for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
      {
        const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
        aGlContext->core11fwd->glDrawElements (aDrawMode, aNbElemsInGroup, myVboIndices->GetDataType(), anOffset);
        anOffset += aStride * aNbElemsInGroup;
      }
    }
    // draw one (or sequential) primitive by the indices
    else
    {
      aGlContext->core11fwd->glDrawElements (aDrawMode, myVboIndices->GetElemsNb(), myVboIndices->GetDataType(), anOffset);
    }
    myVboIndices->Unbind (aGlContext);
  }
  else if (!myBounds.IsNull())
  {
    GLint aFirstElem = 0;
    for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
    {
      const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
      aGlContext->core11fwd->glDrawArrays (aDrawMode, aFirstElem, aNbElemsInGroup);
      aFirstElem += aNbElemsInGroup;
    }
  }
  else
  {
    aGlContext->core11fwd->glDrawArrays (aDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);
  }

  // unbind buffers
  myVboAttribs->UnbindAttribute (aGlContext, Graphic3d_TOA_POS);

  // restore line context
  aGlContext->SetPolygonMode (aPolyModeOld);
}

// =======================================================================
// function : drawMarkers
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::drawMarkers (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const OpenGl_Aspects* anAspectMarker = theWorkspace->Aspects();
  const Handle(OpenGl_Context)&   aCtx = theWorkspace->GetGlContext();
  const GLenum aDrawMode = !aCtx->ActiveProgram().IsNull()
                         && aCtx->ActiveProgram()->HasTessellationStage()
                         ? GL_PATCHES
                         : myDrawMode;
  if (anAspectMarker->Aspect()->MarkerType() == Aspect_TOM_POINT)
  {
    aCtx->SetPointSize (anAspectMarker->MarkerSize());
    aCtx->core11fwd->glDrawArrays (aDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);
    aCtx->SetPointSize (1.0f);
    return;
  }

  if (aCtx->core11ffp != NULL)
  {
    aCtx->core11fwd->glEnable (GL_ALPHA_TEST);
    aCtx->core11fwd->glAlphaFunc (GL_GEQUAL, 0.1f);
  }

  aCtx->core11fwd->glEnable (GL_BLEND);
  aCtx->core11fwd->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (anAspectMarker->HasPointSprite (aCtx))
  {
    // Textured markers will be drawn with the point sprites
    aCtx->SetPointSize (anAspectMarker->MarkerSize());
    aCtx->SetPointSpriteOrigin();

    aCtx->core11fwd->glDrawArrays (aDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);

    aCtx->SetPointSize (1.0f);
  }
  // Textured markers will be drawn with the glBitmap
  else if (const Handle(OpenGl_PointSprite)& aSprite = anAspectMarker->SpriteRes (aCtx, theWorkspace->ToHighlight()))
  {
    for (Standard_Integer anIter = 0; anIter < myAttribs->NbElements; anIter++)
    {
      aCtx->core11ffp->glRasterPos3fv (myAttribs->Value<Graphic3d_Vec3> (anIter).GetData());
      aSprite->DrawBitmap (theWorkspace->GetGlContext());
    }
  }

  aCtx->core11fwd->glDisable (GL_BLEND);
  if (aCtx->core11ffp != NULL)
  {
    if (aCtx->ShaderManager()->MaterialState().AlphaCutoff() >= ShortRealLast())
    {
      aCtx->core11fwd->glDisable (GL_ALPHA_TEST);
    }
    else
    {
      aCtx->core11fwd->glAlphaFunc (GL_GEQUAL, aCtx->ShaderManager()->MaterialState().AlphaCutoff());
    }
  }
}

// =======================================================================
// function : OpenGl_PrimitiveArray
// purpose  :
// =======================================================================
OpenGl_PrimitiveArray::OpenGl_PrimitiveArray (const OpenGl_GraphicDriver* theDriver)

: myDrawMode  (DRAW_MODE_NONE),
  myIsFillType(Standard_False),
  myIsVboInit (Standard_False)
{
  if (theDriver != NULL)
  {
    myUID = theDriver->GetNextPrimitiveArrayUID();
  }
}

// =======================================================================
// function : OpenGl_PrimitiveArray
// purpose  :
// =======================================================================
OpenGl_PrimitiveArray::OpenGl_PrimitiveArray (const OpenGl_GraphicDriver*          theDriver,
                                              const Graphic3d_TypeOfPrimitiveArray theType,
                                              const Handle(Graphic3d_IndexBuffer)& theIndices,
                                              const Handle(Graphic3d_Buffer)&      theAttribs,
                                              const Handle(Graphic3d_BoundBuffer)& theBounds)

: myIndices   (theIndices),
  myAttribs   (theAttribs),
  myBounds    (theBounds),
  myDrawMode  (DRAW_MODE_NONE),
  myIsFillType(Standard_False),
  myIsVboInit (Standard_False)
{
  if (!myIndices.IsNull()
    && myIndices->NbElements < 1)
  {
    // dummy index buffer?
    myIndices.Nullify();
  }

  if (theDriver != NULL)
  {
    myUID = theDriver->GetNextPrimitiveArrayUID();
    const Handle(OpenGl_Context)& aCtx = theDriver->GetSharedContext();
    if (!aCtx.IsNull()
      && aCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
    {
      processIndices (aCtx);
    }
  }

  setDrawMode (theType);
}

// =======================================================================
// function : ~OpenGl_PrimitiveArray
// purpose  :
// =======================================================================
OpenGl_PrimitiveArray::~OpenGl_PrimitiveArray()
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::Release (OpenGl_Context* theContext)
{
  myIsVboInit = Standard_False;
  if (!myVboIndices.IsNull())
  {
    if (theContext)
    {
      theContext->DelayedRelease (myVboIndices);
    }
    myVboIndices.Nullify();
  }
  if (!myVboAttribs.IsNull())
  {
    if (theContext)
    {
      theContext->DelayedRelease (myVboAttribs);
    }
    myVboAttribs.Nullify();
  }
}

// =======================================================================
// function : EstimatedDataSize
// purpose  :
// =======================================================================
Standard_Size OpenGl_PrimitiveArray::EstimatedDataSize() const
{
  Standard_Size aSize = 0;
  if (!myVboAttribs.IsNull())
  {
    aSize += myVboAttribs->EstimatedDataSize();
  }
  if (!myVboIndices.IsNull())
  {
    aSize += myVboIndices->EstimatedDataSize();
  }
  return aSize;
}

// =======================================================================
// function : UpdateDrawStats
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::UpdateDrawStats (Graphic3d_FrameStatsDataTmp& theStats,
                                             bool theIsDetailed) const
{
  ++theStats[Graphic3d_FrameStatsCounter_NbElemsNotCulled];
  if (myIsFillType)
  {
    ++theStats[Graphic3d_FrameStatsCounter_NbElemsFillNotCulled];
  }
  else if (myDrawMode == GL_POINTS)
  {
    ++theStats[Graphic3d_FrameStatsCounter_NbElemsPointNotCulled];
  }
  else
  {
    ++theStats[Graphic3d_FrameStatsCounter_NbElemsLineNotCulled];
  }

  if (!theIsDetailed
  ||  myVboAttribs.IsNull()
  || !myVboAttribs->IsValid())
  {
    return;
  }

  const Standard_Integer aNbIndices = !myVboIndices.IsNull() ? myVboIndices->GetElemsNb() : myVboAttribs->GetElemsNb();
  const Standard_Integer aNbBounds  = !myBounds.IsNull() ? myBounds->NbBounds : 1;
  switch (myDrawMode)
  {
    case GL_POINTS:
    {
      theStats[Graphic3d_FrameStatsCounter_NbPointsNotCulled] += aNbIndices;
      break;
    }
    case GL_LINES:
    {
      theStats[Graphic3d_FrameStatsCounter_NbLinesNotCulled] += aNbIndices / 2;
      break;
    }
    case GL_LINE_STRIP:
    {
      theStats[Graphic3d_FrameStatsCounter_NbLinesNotCulled] += aNbIndices - aNbBounds;
      break;
    }
    case GL_LINES_ADJACENCY:
    {
      theStats[Graphic3d_FrameStatsCounter_NbLinesNotCulled] += aNbIndices / 4;
      break;
    }
    case GL_LINE_STRIP_ADJACENCY:
    {
      theStats[Graphic3d_FrameStatsCounter_NbLinesNotCulled] += aNbIndices - 4 * aNbBounds;
      break;
    }
    case GL_TRIANGLES:
    {
      theStats[Graphic3d_FrameStatsCounter_NbTrianglesNotCulled] += aNbIndices / 3;
      break;
    }
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    {
      theStats[Graphic3d_FrameStatsCounter_NbTrianglesNotCulled] += aNbIndices - 2 * aNbBounds;
      break;
    }
    case GL_TRIANGLES_ADJACENCY:
    {
      theStats[Graphic3d_FrameStatsCounter_NbTrianglesNotCulled] += aNbIndices / 6;
      break;
    }
    case GL_TRIANGLE_STRIP_ADJACENCY:
    {
      theStats[Graphic3d_FrameStatsCounter_NbTrianglesNotCulled] += aNbIndices - 4 * aNbBounds;
      break;
    }
    case GL_QUADS:
    {
      theStats[Graphic3d_FrameStatsCounter_NbTrianglesNotCulled] += aNbIndices / 2;
      break;
    }
    case GL_QUAD_STRIP:
    {
      theStats[Graphic3d_FrameStatsCounter_NbTrianglesNotCulled] += (aNbIndices / 2 - aNbBounds) * 2;
      break;
    }
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  if (myDrawMode == DRAW_MODE_NONE)
  {
    return;
  }

  const OpenGl_Aspects* anAspectFace = theWorkspace->Aspects();
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  bool toDrawArray = true, toSetLinePolygMode = false;
  int toDrawInteriorEdges = 0; // 0 - no edges, 1 - glsl edges, 2 - polygonMode
  if (myIsFillType)
  {
    toDrawArray = anAspectFace->Aspect()->InteriorStyle() != Aspect_IS_EMPTY;
    if (anAspectFace->Aspect()->ToDrawEdges())
    {
      toDrawInteriorEdges = 1;
      toDrawArray = true;
      if (aCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES
       && (anAspectFace->Aspect()->EdgeLineType() != Aspect_TOL_SOLID
        || aCtx->hasGeometryStage == OpenGl_FeatureNotAvailable
        || aCtx->caps->usePolygonMode))
      {
        toDrawInteriorEdges = 2;
        if (anAspectFace->Aspect()->InteriorStyle() == Aspect_IS_EMPTY)
        {
          if (anAspectFace->Aspect()->EdgeLineType() != Aspect_TOL_SOLID)
          {
            toDrawArray = false;
          }
          else
          {
            toSetLinePolygMode = true;
          }
        }
      }
    }
  }
  else
  {
    if (myDrawMode == GL_POINTS)
    {
      if (anAspectFace->Aspect()->MarkerType() == Aspect_TOM_EMPTY)
      {
        return;
      }
    }
    else
    {
      if (anAspectFace->Aspect()->LineType() == Aspect_TOL_EMPTY)
      {
        return;
      }
    }
  }

  // create VBOs on first render call
  if (!myIsVboInit)
  {
    // compatibility - keep data to draw markers using display lists
    Standard_Boolean toKeepData = myDrawMode == GL_POINTS
                               && anAspectFace->IsDisplayListSprite (aCtx);
    if (aCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
    {
      processIndices (aCtx);
    }
    buildVBO (aCtx, toKeepData);
    myIsVboInit = Standard_True;
  }
  else if ((!myAttribs.IsNull()
         &&  myAttribs->IsMutable())
        || (!myIndices.IsNull()
         &&  myIndices->IsMutable()))
  {
    updateVBO (aCtx);
  }

  Graphic3d_TypeOfShadingModel aShadingModel = Graphic3d_TypeOfShadingModel_Unlit;
  anAspectFace = theWorkspace->ApplyAspects (false); // do not bind textures before binding the program
  const Handle(OpenGl_TextureSet)& aTextureSet = theWorkspace->TextureSet();
  const bool toEnableEnvMap = !aTextureSet.IsNull()
                            && aTextureSet == theWorkspace->EnvironmentTexture();
  if (toDrawArray)
  {
    const bool hasColorAttrib = !myVboAttribs.IsNull()
                              && myVboAttribs->HasColorAttribute();
    const bool toHilight    = theWorkspace->ToHighlight();
    const bool hasVertColor = hasColorAttrib && !toHilight;
    const bool hasVertNorm  = !myVboAttribs.IsNull() && myVboAttribs->HasNormalAttribute();
    switch (myDrawMode)
    {
      case GL_POINTS:
      {
        aShadingModel = aCtx->ShaderManager()->ChooseMarkerShadingModel (anAspectFace->ShadingModel(), hasVertNorm);
        aCtx->ShaderManager()->BindMarkerProgram (aTextureSet,
                                                  aShadingModel, Graphic3d_AlphaMode_Opaque,
                                                  hasVertColor, anAspectFace->ShaderProgramRes (aCtx));
        break;
      }
      case GL_LINES:
      case GL_LINE_STRIP:
      {
        aShadingModel = aCtx->ShaderManager()->ChooseLineShadingModel (anAspectFace->ShadingModel(), hasVertNorm);
        aCtx->ShaderManager()->BindLineProgram (Handle(OpenGl_TextureSet)(),
                                                anAspectFace->Aspect()->LineType(),
                                                aShadingModel,
                                                Graphic3d_AlphaMode_Opaque,
                                                hasVertColor,
                                                anAspectFace->ShaderProgramRes (aCtx));
        break;
      }
      default:
      {
        aShadingModel = aCtx->ShaderManager()->ChooseFaceShadingModel (anAspectFace->ShadingModel(), hasVertNorm);
        aCtx->ShaderManager()->BindFaceProgram (aTextureSet,
                                                aShadingModel,
                                                aCtx->ShaderManager()->MaterialState().HasAlphaCutoff() ? Graphic3d_AlphaMode_Mask : Graphic3d_AlphaMode_Opaque,
                                                toDrawInteriorEdges == 1 ? anAspectFace->Aspect()->InteriorStyle() : Aspect_IS_SOLID,
                                                hasVertColor,
                                                toEnableEnvMap,
                                                toDrawInteriorEdges == 1,
                                                anAspectFace->ShaderProgramRes (aCtx));
        if (toDrawInteriorEdges == 1)
        {
          aCtx->ShaderManager()->PushInteriorState (aCtx->ActiveProgram(), anAspectFace->Aspect());
        }
        else if (toSetLinePolygMode)
        {
          aCtx->SetPolygonMode (GL_LINE);
        }
        break;
      }
    }

    // bind textures after GLSL program to set mock textures to slots used by program
    aCtx->BindTextures (aTextureSet, aCtx->ActiveProgram());
    if (!aTextureSet.IsNull()
     && !aTextureSet->IsEmpty()
     && myDrawMode != GL_POINTS) // transformation is not supported within point sprites
    {
      if (const Handle(OpenGl_Texture)& aFirstTexture = aTextureSet->First())
      {
        aCtx->SetTextureMatrix (aFirstTexture->Sampler()->Parameters(), aFirstTexture->IsTopDown());
      }
    }
    aCtx->SetSampleAlphaToCoverage (aCtx->ShaderManager()->MaterialState().HasAlphaCutoff());

    const bool isForcedBlend = anAspectFace->Aspect()->AlphaMode() == Graphic3d_AlphaMode_MaskBlend;
    if (isForcedBlend)
    {
      aCtx->core11fwd->glEnable (GL_BLEND);
      aCtx->core11fwd->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    const Graphic3d_Vec4* aFaceColors = !myBounds.IsNull() && !toHilight && anAspectFace->Aspect()->InteriorStyle() != Aspect_IS_HIDDENLINE
                                      ?  myBounds->Colors
                                      :  NULL;
    const OpenGl_Vec4& anInteriorColor = theWorkspace->InteriorColor();
    aCtx->SetColor4fv (anInteriorColor);
    if (!myIsFillType)
    {
      if (myDrawMode == GL_LINES
       || myDrawMode == GL_LINE_STRIP)
      {
        aCtx->SetLineStipple((float )anAspectFace->Aspect()->LineStippleFactor(), anAspectFace->Aspect()->LinePattern());
        aCtx->SetLineWidth  (anAspectFace->Aspect()->LineWidth());
      }

      drawArray (theWorkspace, aFaceColors, hasColorAttrib);
      if (isForcedBlend)
      {
        aCtx->core11fwd->glDisable (GL_BLEND);
      }
      return;
    }

    drawArray (theWorkspace, aFaceColors, hasColorAttrib);

    // draw outline - only closed triangulation with defined vertex normals can be drawn in this way
    if (anAspectFace->Aspect()->ToDrawSilhouette()
     && aCtx->ToCullBackFaces()
     && aCtx->ShaderManager()->BindOutlineProgram())
    {
      const Graphic3d_Vec2i aViewSize (aCtx->Viewport()[2], aCtx->Viewport()[3]);
      const Standard_Integer aMin = aViewSize.minComp();
      const GLfloat anEdgeWidth  = (GLfloat )anAspectFace->Aspect()->EdgeWidth() * aCtx->LineWidthScale() / (GLfloat )aMin;
      const GLfloat anOrthoScale = aCtx->Camera()->IsOrthographic() ? (GLfloat )aCtx->Camera()->Scale() : -1.0f;

      const Handle(OpenGl_ShaderProgram)& anOutlineProgram = aCtx->ActiveProgram();
      anOutlineProgram->SetUniform (aCtx, anOutlineProgram->GetStateLocation (OpenGl_OCCT_SILHOUETTE_THICKNESS), anEdgeWidth);
      anOutlineProgram->SetUniform (aCtx, anOutlineProgram->GetStateLocation (OpenGl_OCCT_ORTHO_SCALE),          anOrthoScale);
      aCtx->SetColor4fv (anAspectFace->Aspect()->EdgeColorRGBA());

      aCtx->SetFaceCulling (Graphic3d_TypeOfBackfacingModel_FrontCulled);
      drawArray (theWorkspace, NULL, false);
      aCtx->SetFaceCulling (Graphic3d_TypeOfBackfacingModel_BackCulled);
    }

    if (isForcedBlend)
    {
      aCtx->core11fwd->glDisable (GL_BLEND);
    }
  }

  // draw triangulation edges using Polygon Mode
  if (toDrawInteriorEdges == 2)
  {
    if (anAspectFace->Aspect()->InteriorStyle() == Aspect_IS_HOLLOW
     && anAspectFace->Aspect()->EdgeLineType()  == Aspect_TOL_SOLID)
    {
      aCtx->SetPolygonMode (GL_FILL);
    }
    else
    {
      drawEdges (theWorkspace);
    }
  }
}

// =======================================================================
// function : setDrawMode
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::setDrawMode (const Graphic3d_TypeOfPrimitiveArray theType)
{
  if (myAttribs.IsNull())
  {
    myDrawMode = DRAW_MODE_NONE;
    myIsFillType = false;
    return;
  }

  switch (theType)
  {
    case Graphic3d_TOPA_POINTS:
      myDrawMode   = GL_POINTS;
      myIsFillType = false;
      break;
    case Graphic3d_TOPA_SEGMENTS:
      myDrawMode   = GL_LINES;
      myIsFillType = false;
      break;
    case Graphic3d_TOPA_POLYLINES:
      myDrawMode   = GL_LINE_STRIP;
      myIsFillType = false;
      break;
    case Graphic3d_TOPA_TRIANGLES:
      myDrawMode   = GL_TRIANGLES;
      myIsFillType = true;
      break;
    case Graphic3d_TOPA_TRIANGLESTRIPS:
      myDrawMode   = GL_TRIANGLE_STRIP;
      myIsFillType = true;
      break;
    case Graphic3d_TOPA_TRIANGLEFANS:
      myDrawMode   = GL_TRIANGLE_FAN;
      myIsFillType = true;
      break;
    //
    case Graphic3d_TOPA_LINES_ADJACENCY:
      myDrawMode = GL_LINES_ADJACENCY;
      myIsFillType = false;
      break;
    case Graphic3d_TOPA_LINE_STRIP_ADJACENCY:
      myDrawMode   = GL_LINE_STRIP_ADJACENCY;
      myIsFillType = false;
      break;
    case Graphic3d_TOPA_TRIANGLES_ADJACENCY:
      myDrawMode   = GL_TRIANGLES_ADJACENCY;
      myIsFillType = true;
      break;
    case Graphic3d_TOPA_TRIANGLE_STRIP_ADJACENCY:
      myDrawMode   = GL_TRIANGLE_STRIP_ADJACENCY;
      myIsFillType = true;
      break;
    //
    case Graphic3d_TOPA_QUADRANGLES:
      myDrawMode   = GL_QUADS;
      myIsFillType = true;
      break;
    case Graphic3d_TOPA_QUADRANGLESTRIPS:
      myDrawMode   = GL_QUAD_STRIP;
      myIsFillType = true;
      break;
    case Graphic3d_TOPA_POLYGONS:
      myDrawMode   = GL_POLYGON;
      myIsFillType = true;
      break;
    case Graphic3d_TOPA_UNDEFINED:
      myDrawMode   = DRAW_MODE_NONE;
      myIsFillType = false;
      break;
  }
}

// =======================================================================
// function : processIndices
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_PrimitiveArray::processIndices (const Handle(OpenGl_Context)& theContext) const
{
  if (myIndices.IsNull()
   || myAttribs.IsNull()
   || theContext->hasUintIndex)
  {
    return Standard_True;
  }

  if (myAttribs->NbElements > std::numeric_limits<GLushort>::max())
  {
    Handle(Graphic3d_Buffer) anAttribs = new Graphic3d_Buffer (Graphic3d_Buffer::DefaultAllocator());
    if (!anAttribs->Init (myIndices->NbElements, myAttribs->AttributesArray(), myAttribs->NbAttributes))
    {
      return Standard_False; // failed to initialize attribute array
    }

    for (Standard_Integer anIdxIdx = 0; anIdxIdx < myIndices->NbElements; ++anIdxIdx)
    {
      const Standard_Integer anIndex = myIndices->Index (anIdxIdx);
      memcpy (anAttribs->ChangeData() + myAttribs->Stride * anIdxIdx,
              myAttribs->Data()       + myAttribs->Stride * anIndex,
              myAttribs->Stride);
    }

    myIndices.Nullify();
    myAttribs = anAttribs;
  }

  return Standard_True;
}

// =======================================================================
// function : InitBuffers
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::InitBuffers (const Handle(OpenGl_Context)&        theContext,
                                         const Graphic3d_TypeOfPrimitiveArray theType,
                                         const Handle(Graphic3d_IndexBuffer)& theIndices,
                                         const Handle(Graphic3d_Buffer)&      theAttribs,
                                         const Handle(Graphic3d_BoundBuffer)& theBounds)
{
  // Release old graphic resources
  Release (theContext.get());

  myIndices = theIndices;
  myAttribs = theAttribs;
  myBounds = theBounds;
  if (!theContext.IsNull()
    && theContext->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    processIndices (theContext);
  }

  setDrawMode (theType);
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, OpenGl_PrimitiveArray)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, OpenGl_Element)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myVboIndices.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myVboAttribs.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myIndices.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myAttribs.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myBounds.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDrawMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsFillType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsVboInit)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUID)
}
