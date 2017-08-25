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

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_IndexBuffer.hxx>
#include <OpenGl_PointSprite.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_VertexBufferCompat.hxx>
#include <OpenGl_Workspace.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <NCollection_AlignedAllocator.hxx>

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
  OpenGl_VertexBufferT (const Graphic3d_Attribute* theAttribs,
                        const Standard_Integer     theStride)
  : Stride (theStride)
  {
    memcpy (Attribs, theAttribs, sizeof(Graphic3d_Attribute) * NbAttributes);
  }

  //! Create uninitialized VBO.
  OpenGl_VertexBufferT (const Graphic3d_Buffer& theAttribs)
  : Stride (theAttribs.Stride)
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
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      const GLenum   aDataType = toGlDataType (anAttrib.DataType, aNbComp);
      if (aDataType == GL_NONE)
      {
        continue;
      }
      else if (anAttrib.Id == Graphic3d_TOA_POS)
      {
        TheBaseClass::bindAttribute (theGlCtx, Graphic3d_TOA_POS, aNbComp, aDataType, Stride, anOffset);
        break;
      }

      anOffset += Graphic3d_Attribute::Stride (anAttrib.DataType);
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
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      const GLenum   aDataType = toGlDataType (anAttrib.DataType, aNbComp);
      if (aDataType == GL_NONE)
      {
        continue;
      }

      TheBaseClass::bindAttribute (theGlCtx, anAttrib.Id, aNbComp, aDataType, Stride, anOffset);
      anOffset += Graphic3d_Attribute::Stride (anAttrib.DataType);
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

public:

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

  if (!myVboAttribs->init (theCtx, 0, myAttribs->NbElements, myAttribs->Data(), GL_NONE, myAttribs->Stride))
  {
    TCollection_ExtendedString aMsg;
    aMsg += "VBO creation for Primitive Array has failed for ";
    aMsg += myAttribs->NbElements;
    aMsg += " vertices. Out of memory?";
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_LOW, aMsg);

    clearMemoryGL (theCtx);
    return Standard_False;
  }
  else if (myIndices.IsNull())
  {
    return Standard_True;
  }

  myVboIndices = new OpenGl_IndexBuffer();
  bool isOk = false;
  switch (myIndices->Stride)
  {
    case 2:
    {
      isOk = myVboIndices->Init (theCtx, 1, myIndices->NbElements, reinterpret_cast<const GLushort*> (myIndices->Data()));
      break;
    }
    case 4:
    {
      isOk = myVboIndices->Init (theCtx, 1, myIndices->NbElements, reinterpret_cast<const GLuint*> (myIndices->Data()));
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
    TCollection_ExtendedString aMsg;
    aMsg += "VBO creation for Primitive Array has failed for ";
    aMsg += myIndices->NbElements;
    aMsg += " indices. Out of memory?";
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
     && !theToKeepData)
    {
      myIndices.Nullify();
      myAttribs.Nullify();
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
    Handle(OpenGl_VertexBufferCompat) aVboIndices = new OpenGl_VertexBufferCompat();
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
// function : drawArray
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::drawArray (const Handle(OpenGl_Workspace)& theWorkspace,
                                       const Graphic3d_Vec4*           theFaceColors,
                                       const Standard_Boolean          theHasVertColor) const
{
  if (myVboAttribs.IsNull())
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (myDrawMode == GL_POINTS)
    {
      // extreme compatibility mode - without sprites but with markers
      drawMarkers (theWorkspace);
    }
  #endif
    return;
  }

  const Handle(OpenGl_Context)& aGlContext  = theWorkspace->GetGlContext();
  const bool                    toHilight   = theWorkspace->ToHighlight();
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
        glDrawElements (myDrawMode, aNbElemsInGroup, myVboIndices->GetDataType(), anOffset);
        anOffset += aStride * aNbElemsInGroup;
      }
    }
    else
    {
      // draw one (or sequential) primitive by the indices
      glDrawElements (myDrawMode, myVboIndices->GetElemsNb(), myVboIndices->GetDataType(), anOffset);
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
      glDrawArrays (myDrawMode, aFirstElem, aNbElemsInGroup);
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
      glDrawArrays (myDrawMode, 0, myVboAttribs->GetElemsNb());
    }
  }

  // bind with 0
  myVboAttribs->UnbindAllAttributes (aGlContext);
}

// =======================================================================
// function : drawEdges
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::drawEdges (const OpenGl_Vec4&              theEdgeColour,
                                       const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aGlContext = theWorkspace->GetGlContext();
  if (myVboAttribs.IsNull())
  {
    return;
  }

#if !defined(GL_ES_VERSION_2_0)
  if (aGlContext->core11 != NULL)
  {
    glDisable (GL_LIGHTING);
  }
#endif

  const OpenGl_AspectLine* anAspectLineOld = theWorkspace->SetAspectLine (theWorkspace->AspectFace()->AspectEdge());
  const OpenGl_AspectLine* anAspect = theWorkspace->ApplyAspectLine();

#if !defined(GL_ES_VERSION_2_0)
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
#endif

  if (aGlContext->core20fwd != NULL)
  {
    aGlContext->ShaderManager()->BindLineProgram (NULL,
                                                  anAspect->Aspect()->Type() != Aspect_TOL_SOLID,
                                                  Standard_False,
                                                  Standard_False,
                                                  anAspect->ShaderProgramRes (aGlContext));
  }

  /// OCC22236 NOTE: draw edges for all situations:
  /// 1) draw elements with GL_LINE style as edges from myPArray->bufferVBO[VBOEdges] indices array
  /// 2) draw elements from vertex array, when bounds defines count of primitive's vertices.
  /// 3) draw primitive's edges by vertexes if no edges and bounds array is specified
  myVboAttribs->BindPositionAttribute (aGlContext);

  aGlContext->SetColor4fv   (theEdgeColour);
  aGlContext->SetTypeOfLine (anAspect->Aspect()->Type());
  aGlContext->SetLineWidth  (anAspect->Aspect()->Width());

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
        glDrawElements (myDrawMode, aNbElemsInGroup, myVboIndices->GetDataType(), anOffset);
        anOffset += aStride * aNbElemsInGroup;
      }
    }
    // draw one (or sequential) primitive by the indices
    else
    {
      glDrawElements (myDrawMode, myVboIndices->GetElemsNb(), myVboIndices->GetDataType(), anOffset);
    }
    myVboIndices->Unbind (aGlContext);
  }
  else if (!myBounds.IsNull())
  {
    GLint aFirstElem = 0;
    for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
    {
      const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
      glDrawArrays (myDrawMode, aFirstElem, aNbElemsInGroup);
      aFirstElem += aNbElemsInGroup;
    }
  }
  else
  {
    glDrawArrays (myDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);
  }

  // unbind buffers
  myVboAttribs->UnbindAttribute (aGlContext, Graphic3d_TOA_POS);

  // restore line context
  theWorkspace->SetAspectLine (anAspectLineOld);
}

// =======================================================================
// function : drawMarkers
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::drawMarkers (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const OpenGl_AspectMarker* anAspectMarker     = theWorkspace->ApplyAspectMarker();
  const Handle(OpenGl_Context)&     aCtx        = theWorkspace->GetGlContext();
  const Handle(OpenGl_TextureSet)& aSpriteNormRes = anAspectMarker->SpriteRes (aCtx);
  const OpenGl_PointSprite* aSpriteNorm = !aSpriteNormRes.IsNull() ? dynamic_cast<const OpenGl_PointSprite*> (aSpriteNormRes->First().get()) : NULL;
  if (aSpriteNorm != NULL
  && !aSpriteNorm->IsDisplayList())
  {
    // Textured markers will be drawn with the point sprites
    aCtx->SetPointSize (anAspectMarker->MarkerSize());
    aCtx->SetPointSpriteOrigin();
  #if !defined(GL_ES_VERSION_2_0)
    if (aCtx->core11 != NULL)
    {
      aCtx->core11fwd->glEnable (GL_ALPHA_TEST);
      aCtx->core11fwd->glAlphaFunc (GL_GEQUAL, 0.1f);
    }
  #endif

    aCtx->core11fwd->glEnable (GL_BLEND);
    aCtx->core11fwd->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    aCtx->core11fwd->glDrawArrays (myDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);

    aCtx->core11fwd->glDisable (GL_BLEND);
  #if !defined(GL_ES_VERSION_2_0)
    if (aCtx->core11 != NULL)
    {
      aCtx->core11fwd->glDisable (GL_ALPHA_TEST);
    }
  #endif
    aCtx->SetPointSize (1.0f);
    return;
  }
  else if (anAspectMarker->Aspect()->Type() == Aspect_TOM_POINT)
  {
    aCtx->SetPointSize (anAspectMarker->MarkerSize());
    aCtx->core11fwd->glDrawArrays (myDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);
    aCtx->SetPointSize (1.0f);
  }
#if !defined(GL_ES_VERSION_2_0)
  // Textured markers will be drawn with the glBitmap
  else if (anAspectMarker->Aspect()->Type() != Aspect_TOM_POINT
        && aSpriteNorm != NULL)
  {
    /**if (!isHilight && (myPArray->vcolours != NULL))
    {
      for (Standard_Integer anIter = 0; anIter < myAttribs->NbElements; anIter++)
      {
        glColor4ubv    (myPArray->vcolours[anIter].GetData());
        glRasterPos3fv (myAttribs->Value<Graphic3d_Vec3> (anIter).GetData());
        aSpriteNorm->DrawBitmap (theWorkspace->GetGlContext());
      }
    }
    else*/
    {
      for (Standard_Integer anIter = 0; anIter < myAttribs->NbElements; anIter++)
      {
        aCtx->core11->glRasterPos3fv (myAttribs->Value<Graphic3d_Vec3> (anIter).GetData());
        aSpriteNorm->DrawBitmap (theWorkspace->GetGlContext());
      }
    }
  }
#endif
}

// =======================================================================
// function : OpenGl_PrimitiveArray
// purpose  :
// =======================================================================
OpenGl_PrimitiveArray::OpenGl_PrimitiveArray (const OpenGl_GraphicDriver* theDriver)

: myDrawMode  (DRAW_MODE_NONE),
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
  #if defined (GL_ES_VERSION_2_0)
    const Handle(OpenGl_Context)& aCtx = theDriver->GetSharedContext();
    if (!aCtx.IsNull())
    {
      processIndices (aCtx);
    }
  #endif
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
// function : Render
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  if (myDrawMode == DRAW_MODE_NONE)
  {
    return;
  }

  const OpenGl_AspectFace*   anAspectFace   = theWorkspace->ApplyAspectFace();
  const OpenGl_AspectLine*   anAspectLine   = theWorkspace->ApplyAspectLine();
  const OpenGl_AspectMarker* anAspectMarker = myDrawMode == GL_POINTS
                                            ? theWorkspace->ApplyAspectMarker()
                                            : theWorkspace->AspectMarker();

  // create VBOs on first render call
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  if (!myIsVboInit)
  {
    // compatibility - keep data to draw markers using display lists
    Standard_Boolean toKeepData = Standard_False;
    if (myDrawMode == GL_POINTS)
    {
      const Handle(OpenGl_TextureSet)& aSpriteNormRes = anAspectMarker->SpriteRes (aCtx);
      const OpenGl_PointSprite* aSpriteNorm = !aSpriteNormRes.IsNull() ? dynamic_cast<const OpenGl_PointSprite*> (aSpriteNormRes->First().get()) : NULL;
      toKeepData = aSpriteNorm != NULL
               &&  aSpriteNorm->IsDisplayList();
    }
  #if defined (GL_ES_VERSION_2_0)
    processIndices (aCtx);
  #endif
    buildVBO (aCtx, toKeepData);
    myIsVboInit = Standard_True;
  }

  const Standard_Boolean hasColorAttrib = !myVboAttribs.IsNull()
                                        && myVboAttribs->HasColorAttribute();
  const Standard_Boolean isLightOn = !anAspectFace->IsNoLighting()
                                  && !myVboAttribs.IsNull()
                                  &&  myVboAttribs->HasNormalAttribute();

  // Temporarily disable environment mapping
  Handle(OpenGl_TextureSet) aTextureBack;
  bool toDrawArray = true;
  if (myDrawMode > GL_LINE_STRIP)
  {
    toDrawArray = anAspectFace->Aspect()->InteriorStyle() != Aspect_IS_EMPTY;
  }
  else if (myDrawMode <= GL_LINE_STRIP)
  {
    aTextureBack = aCtx->BindTextures (Handle(OpenGl_TextureSet)());
    if (myDrawMode == GL_POINTS)
    {
      toDrawArray = anAspectMarker->Aspect()->Type() != Aspect_TOM_EMPTY;
    }
    else
    {
      toDrawArray = anAspectLine->Aspect()->Type() != Aspect_TOL_EMPTY;
    }
  }

  if (toDrawArray)
  {
    const bool             toHilight    = theWorkspace->ToHighlight();
    const Standard_Boolean hasVertColor = hasColorAttrib && !toHilight;
    switch (myDrawMode)
    {
      case GL_POINTS:
      {
        const Handle(OpenGl_TextureSet)& aSpriteNormRes = anAspectMarker->SpriteRes (aCtx);
        const OpenGl_PointSprite* aSpriteNorm = !aSpriteNormRes.IsNull() ? dynamic_cast<const OpenGl_PointSprite*> (aSpriteNormRes->First().get()) : NULL;
        if (aSpriteNorm != NULL
        && !aSpriteNorm->IsDisplayList())
        {
          const Handle(OpenGl_TextureSet)& aSprite = toHilight && anAspectMarker->SpriteHighlightRes (aCtx)->First()->IsValid()
                                                   ? anAspectMarker->SpriteHighlightRes (aCtx)
                                                   : aSpriteNormRes;
          aCtx->BindTextures (aSprite);
          aCtx->ShaderManager()->BindMarkerProgram (aSprite, isLightOn, hasVertColor, anAspectMarker->ShaderProgramRes (aCtx));
        }
        else
        {
          aCtx->ShaderManager()->BindMarkerProgram (Handle(OpenGl_TextureSet)(), isLightOn, hasVertColor, anAspectMarker->ShaderProgramRes (aCtx));
        }
        break;
      }
      case GL_LINES:
      case GL_LINE_STRIP:
      {
        aCtx->ShaderManager()->BindLineProgram (NULL,
                                                anAspectLine->Aspect()->Type() != Aspect_TOL_SOLID,
                                                isLightOn,
                                                hasVertColor,
                                                anAspectLine->ShaderProgramRes (aCtx));
        break;
      }
      default:
      {
        const Handle(OpenGl_TextureSet)& aTextures = aCtx->ActiveTextures();
        const Standard_Boolean isLightOnFace = isLightOn
                                            && (aTextures.IsNull()
                                             || aTextures->IsEmpty()
                                             || aTextures->First().IsNull()
                                             || aTextures->First()->Sampler()->Parameters()->IsModulate());
        const Standard_Boolean toEnableEnvMap = (!aTextures.IsNull() && (aTextures == theWorkspace->EnvironmentTexture()));
        aCtx->ShaderManager()->BindFaceProgram (aTextures,
                                                isLightOnFace,
                                                hasVertColor,
                                                toEnableEnvMap,
                                                anAspectFace->ShaderProgramRes (aCtx));
        break;
      }
    }

  #if !defined(GL_ES_VERSION_2_0)
    // manage FFP lighting
    if (aCtx->ActiveProgram().IsNull()
     && aCtx->core11 != NULL)
    {
      if (!isLightOn)
      {
        glDisable (GL_LIGHTING);
      }
      else
      {
        glEnable (GL_LIGHTING);
      }
    }
  #endif

    if (!aCtx->ActiveTextures().IsNull()
     && !aCtx->ActiveTextures()->IsEmpty()
     && !aCtx->ActiveTextures()->First().IsNull()
     && myDrawMode != GL_POINTS) // transformation is not supported within point sprites
    {
      aCtx->SetTextureMatrix (aCtx->ActiveTextures()->First()->Sampler()->Parameters());
    }

    if (myDrawMode <= GL_LINE_STRIP)
    {
      const OpenGl_Vec4& aLineColor = myDrawMode == GL_POINTS ? theWorkspace->MarkerColor() : theWorkspace->LineColor();
      aCtx->SetColor4fv (aLineColor);
    }
    else
    {
      const OpenGl_Vec4& anInteriorColor = theWorkspace->InteriorColor();
      aCtx->SetColor4fv (anInteriorColor);
    }
    if (myDrawMode == GL_LINES
     || myDrawMode == GL_LINE_STRIP)
    {
      aCtx->SetTypeOfLine (anAspectLine->Aspect()->Type());
      aCtx->SetLineWidth  (anAspectLine->Aspect()->Width());
    }

    const Graphic3d_Vec4* aFaceColors = !myBounds.IsNull() && !toHilight && anAspectFace->Aspect()->InteriorStyle() != Aspect_IS_HIDDENLINE
                                      ?  myBounds->Colors
                                      :  NULL;
    drawArray (theWorkspace, aFaceColors, hasColorAttrib);
  }

  if (myDrawMode <= GL_LINE_STRIP)
  {
    aCtx->BindTextures (aTextureBack);
  }
  else
  {
    if (anAspectFace->Aspect()->ToDrawEdges()
     || anAspectFace->Aspect()->InteriorStyle() == Aspect_IS_HIDDENLINE)
    {
      const OpenGl_Vec4& anEdgeColor = theWorkspace->EdgeColor();
      drawEdges (anEdgeColor, theWorkspace);

      // restore OpenGL polygon mode if needed
    #if !defined(GL_ES_VERSION_2_0)
      if (anAspectFace->Aspect()->InteriorStyle() >= Aspect_IS_HATCH)
      {
        glPolygonMode (GL_FRONT_AND_BACK,
          anAspectFace->Aspect()->InteriorStyle() == Aspect_IS_POINT ? GL_POINT : GL_FILL);
      }
    #endif
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
    return;
  }

  switch (theType)
  {
    case Graphic3d_TOPA_POINTS:
      myDrawMode = GL_POINTS;
      break;
    case Graphic3d_TOPA_POLYLINES:
      myDrawMode = GL_LINE_STRIP;
      break;
    case Graphic3d_TOPA_SEGMENTS:
      myDrawMode = GL_LINES;
      break;
    case Graphic3d_TOPA_TRIANGLES:
      myDrawMode = GL_TRIANGLES;
      break;
    case Graphic3d_TOPA_TRIANGLESTRIPS:
      myDrawMode = GL_TRIANGLE_STRIP;
      break;
    case Graphic3d_TOPA_TRIANGLEFANS:
      myDrawMode = GL_TRIANGLE_FAN;
      break;
  #if !defined(GL_ES_VERSION_2_0)
    case Graphic3d_TOPA_POLYGONS:
      myDrawMode = GL_POLYGON;
      break;
    case Graphic3d_TOPA_QUADRANGLES:
      myDrawMode = GL_QUADS;
      break;
    case Graphic3d_TOPA_QUADRANGLESTRIPS:
      myDrawMode = GL_QUAD_STRIP;
      break;
  #else
    case Graphic3d_TOPA_POLYGONS:
    case Graphic3d_TOPA_QUADRANGLES:
    case Graphic3d_TOPA_QUADRANGLESTRIPS:
  #endif
    case Graphic3d_TOPA_UNDEFINED:
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
    Handle(Graphic3d_Buffer) anAttribs = new Graphic3d_Buffer (new NCollection_AlignedAllocator (16));
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
  Release (theContext.operator->());

  myIndices = theIndices;
  myAttribs = theAttribs;
  myBounds = theBounds;
#if defined(GL_ES_VERSION_2_0)
  processIndices (theContext);
#endif

  setDrawMode (theType);
}
