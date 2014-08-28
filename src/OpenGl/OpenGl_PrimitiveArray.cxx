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
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Workspace.hxx>

namespace
{
  template<class T>
  void BindProgramWithMaterial (const Handle(OpenGl_Workspace)& theWS,
                                const T*                      theAspect)
  {
    const Handle(OpenGl_Context)& aCtx = theWS->GetGlContext();
    const Handle(OpenGl_ShaderProgram)& aProgram = theAspect->ShaderProgramRes (theWS);
    if (aProgram.IsNull())
    {
      OpenGl_ShaderProgram::Unbind (aCtx);
      return;
    }
    aProgram->BindWithVariables (aCtx);

    const OpenGl_MaterialState* aMaterialState = aCtx->ShaderManager()->MaterialState (aProgram);
    if (aMaterialState == NULL || aMaterialState->Aspect() != theAspect)
    {
      aCtx->ShaderManager()->UpdateMaterialStateTo (aProgram, theAspect);
    }

    aCtx->ShaderManager()->PushState (aProgram);
  }

  //! Convert index data type from size
  inline GLenum toGlIndexType (const Standard_Integer theStride)
  {
    switch (theStride)
    {
      case 2:  return GL_UNSIGNED_SHORT;
      case 4:  return GL_UNSIGNED_INT;
      default: return GL_NONE;
    }
  }

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
    }
    theNbComp = 0;
    return GL_NONE;
  }

}

//! Auxiliary template for VBO with interleaved attributes.
template<int NbAttributes>
class OpenGl_VertexBufferT : public OpenGl_VertexBuffer
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

  virtual void BindFixedPosition (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!IsValid())
    {
      return;
    }

    Bind (theGlCtx);
    GLint aNbComp;
    const GLubyte* anOffset = NULL;
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
        bindFixed (theGlCtx, Graphic3d_TOA_POS, aNbComp, aDataType, Stride, anOffset);
        break;
      }

      anOffset += Graphic3d_Attribute::Stride (anAttrib.DataType);
    }
  }

  virtual void BindFixed (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!IsValid())
    {
      return;
    }

    Bind (theGlCtx);
    GLint aNbComp;
    const GLubyte* anOffset = NULL;
    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      const GLenum   aDataType = toGlDataType (anAttrib.DataType, aNbComp);
      if (aDataType == GL_NONE)
      {
        continue;
      }

      bindFixed (theGlCtx, anAttrib.Id, aNbComp, aDataType, Stride, anOffset);
      anOffset += Graphic3d_Attribute::Stride (anAttrib.DataType);
    }
  }

  virtual void UnbindFixed (const Handle(OpenGl_Context)& theGlCtx) const
  {
    if (!IsValid())
    {
      return;
    }
    Unbind (theGlCtx);

    for (Standard_Integer anAttribIter = 0; anAttribIter < NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = Attribs[anAttribIter];
      unbindFixed (theGlCtx, anAttrib.Id);
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
// function : BuildVBO
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_PrimitiveArray::BuildVBO (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aGlCtx = theWorkspace->GetGlContext();
  if (myAttribs.IsNull()
   || myAttribs->IsEmpty()
   || myAttribs->NbElements < 1)
  {
    // vertices should be always defined - others are optional
    return Standard_False;
  }

  switch (myAttribs->NbAttributes)
  {
    case 1:  myVboAttribs = new OpenGl_VertexBufferT<1> (*myAttribs); break;
    case 2:  myVboAttribs = new OpenGl_VertexBufferT<2> (*myAttribs); break;
    case 3:  myVboAttribs = new OpenGl_VertexBufferT<3> (*myAttribs); break;
    case 4:  myVboAttribs = new OpenGl_VertexBufferT<4> (*myAttribs); break;
    case 5:  myVboAttribs = new OpenGl_VertexBufferT<5> (*myAttribs); break;
    case 6:  myVboAttribs = new OpenGl_VertexBufferT<6> (*myAttribs); break;
    case 7:  myVboAttribs = new OpenGl_VertexBufferT<7> (*myAttribs); break;
    case 8:  myVboAttribs = new OpenGl_VertexBufferT<8> (*myAttribs); break;
    case 9:  myVboAttribs = new OpenGl_VertexBufferT<9> (*myAttribs); break;
    case 10: myVboAttribs = new OpenGl_VertexBufferT<10>(*myAttribs); break;
    default: return Standard_False;
  }

  if (!myVboAttribs->init (aGlCtx, 0, myAttribs->NbElements, myAttribs->Data(), GL_NONE, myAttribs->Stride))
  {
    clearMemoryGL (aGlCtx);
    return Standard_False;
  }

  if (!myIndices.IsNull())
  {
    myVboIndices = new OpenGl_IndexBuffer();
    bool isOk = Standard_False;
    switch (myIndices->Stride)
    {
      case 2:
      {
        isOk = myVboIndices->Init (aGlCtx, 1, myIndices->NbElements, reinterpret_cast<const GLushort*> (myIndices->Data()));
        break;
      }
      case 4:
      {
        isOk = myVboIndices->Init (aGlCtx, 1, myIndices->NbElements, reinterpret_cast<const GLuint*> (myIndices->Data()));
        break;
      }
      default: break;
    }
    if (!isOk)
    {
      clearMemoryGL (aGlCtx);
      return Standard_False;
    }
  }

  if (!aGlCtx->caps->keepArrayData)
  {
    myIndices.Nullify();
    myAttribs.Nullify();
  }
  
  return Standard_True;
}

// =======================================================================
// function : DrawArray
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawArray (Tint theLightingModel,
                                       const Aspect_InteriorStyle theInteriorStyle,
                                       Tint theEdgeFlag,
                                       const TEL_COLOUR* theInteriorColour,
                                       const TEL_COLOUR* theLineColour,
                                       const TEL_COLOUR* theEdgeColour,
                                       const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aGlContext  = theWorkspace->GetGlContext();
  const Graphic3d_Vec4*         aFaceColors = myBounds.IsNull() ? NULL : myBounds->Colors;
  const bool                    toHilight   = (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT) != 0;
  bool                          hasVColors  = false;

  glColor3fv (myDrawMode <= GL_LINE_STRIP ? theLineColour->rgb : theInteriorColour->rgb);

  // Temporarily disable environment mapping
  if (myDrawMode <= GL_LINE_STRIP)
  {
    glPushAttrib (GL_ENABLE_BIT);
    glDisable (GL_TEXTURE_1D);
    glDisable (GL_TEXTURE_2D);
  }

  if ((myDrawMode >  GL_LINE_STRIP && theInteriorStyle != Aspect_IS_EMPTY) ||
      (myDrawMode <= GL_LINE_STRIP))
  {
    if (toHilight)
    {
      aFaceColors = NULL;
    }

    if (theInteriorStyle == Aspect_IS_HIDDENLINE)
    {
      theEdgeFlag = 1;
      aFaceColors = NULL;
    }

    // Sometimes the GL_LIGHTING mode is activated here
    // without glEnable(GL_LIGHTING) call for an unknown reason, so it is necessary
    // to call glEnable(GL_LIGHTING) to synchronize Light On/Off mechanism*
    if (theLightingModel == 0 || myDrawMode <= GL_LINE_STRIP)
      glDisable (GL_LIGHTING);
    else
      glEnable (GL_LIGHTING);

    if (!myVboAttribs.IsNull())
    {
      myVboAttribs->BindFixed (aGlContext);
      if (myVboAttribs->HasColorAttribute())
      {
        if (toHilight)
        {
          // disable per-vertex colors
          OpenGl_VertexBuffer::unbindFixed (aGlContext, Graphic3d_TOA_COLOR);
        }
        else
        {
          hasVColors = true;
        }
      }
      if (!myVboIndices.IsNull())
      {
        myVboIndices->Bind (aGlContext);
        if (!myBounds.IsNull())
        {
          // draw primitives by vertex count with the indices
          const size_t aStride  = myVboIndices->GetDataType() == GL_UNSIGNED_SHORT ? sizeof(unsigned short) : sizeof(unsigned int);
          GLubyte*     anOffset = NULL;
          for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
          {
            const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
            if (aFaceColors != NULL) glColor3fv (aFaceColors[aGroupIter].GetData());
            glDrawElements (myDrawMode, aNbElemsInGroup, myVboIndices->GetDataType(), anOffset);
            anOffset += aStride * aNbElemsInGroup;
          }
        }
        else
        {
          // draw one (or sequential) primitive by the indices
          glDrawElements (myDrawMode, myVboIndices->GetElemsNb(), myVboIndices->GetDataType(), NULL);
        }
        myVboIndices->Unbind (aGlContext);
      }
      else if (!myBounds.IsNull())
      {
        GLint aFirstElem = 0;
        for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
        {
          const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
          if (aFaceColors != NULL) glColor3fv (aFaceColors[aGroupIter].GetData());
          glDrawArrays (myDrawMode, aFirstElem, aNbElemsInGroup);
          aFirstElem += aNbElemsInGroup;
        }
      }
      else
      {
        if (myDrawMode == GL_POINTS)
        {
          DrawMarkers (theWorkspace);
        }
        else
        {
          glDrawArrays (myDrawMode, 0, myVboAttribs->GetElemsNb());
        }
      }

      // bind with 0
      myVboAttribs->UnbindFixed (aGlContext);
    }
    else
    {
      GLint aNbComp;
      for (Standard_Integer anAttribIter = 0; anAttribIter < myAttribs->NbAttributes; ++anAttribIter)
      {
        const Graphic3d_Attribute& anAttrib = myAttribs->Attribute (anAttribIter);
        if (anAttrib.Id == Graphic3d_TOA_COLOR)
        {
          if (toHilight)
          {
            continue;
          }
          hasVColors = true;
        }
        const GLenum  aDataType = toGlDataType (anAttrib.DataType, aNbComp);
        const GLvoid* aData     = myAttribs->Data (anAttribIter);
        if (aDataType == GL_NONE)
        {
          continue;
        }

        OpenGl_VertexBuffer::bindFixed (aGlContext, anAttrib.Id, aNbComp, aDataType, myAttribs->Stride, aData);
      }

      if (!myBounds.IsNull())
      {
        GLint aFirstElem = 0;
        if (!myIndices.IsNull())
        {
          const GLenum anIndexType = toGlIndexType (myIndices->Stride);
          for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
          {
            const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
            if (aFaceColors != NULL) glColor3fv (aFaceColors[aGroupIter].GetData());
            glDrawElements (myDrawMode, aNbElemsInGroup, anIndexType, myIndices->value (aFirstElem));
            aFirstElem += aNbElemsInGroup;
          }
        }
        else
        {
          for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
          {
            const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
            if (aFaceColors != NULL) glColor3fv (aFaceColors[aGroupIter].GetData());
            glDrawArrays (myDrawMode, aFirstElem, aNbElemsInGroup);
            aFirstElem += aNbElemsInGroup;
          }
        }
      }
      else if (!myIndices.IsNull())
      {
        glDrawElements (myDrawMode, myIndices->NbElements, toGlIndexType (myIndices->Stride), myIndices->Data());
      }
      else
      {
        if (myDrawMode == GL_POINTS)
        {
          DrawMarkers (theWorkspace);
        }
        else
        {
          glDrawArrays (myDrawMode, 0, myAttribs->NbElements);
        }
      }

      for (Standard_Integer anAttribIter = 0; anAttribIter < myAttribs->NbAttributes; ++anAttribIter)
      {
        const Graphic3d_Attribute& anAttrib = myAttribs->Attribute (anAttribIter);
        OpenGl_VertexBuffer::unbindFixed (aGlContext, anAttrib.Id);
      }
    }
  }

  if (hasVColors)
  {
    theWorkspace->NamedStatus |= OPENGL_NS_RESMAT;
  }

  if (theEdgeFlag && myDrawMode > GL_LINE_STRIP)
  {
    DrawEdges (theEdgeColour, theWorkspace);
  }

  if (myDrawMode <= GL_LINE_STRIP)
    glPopAttrib();
}

// =======================================================================
// function : DrawEdges
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawEdges (const TEL_COLOUR*               theEdgeColour,
                                       const Handle(OpenGl_Workspace)& theWorkspace) const
{
  glDisable (GL_LIGHTING);

  const Handle(OpenGl_Context)& aGlContext = theWorkspace->GetGlContext();
  const OpenGl_AspectLine* anAspectLineOld = NULL;
  if (myDrawMode > GL_LINE_STRIP)
  {
    anAspectLineOld = theWorkspace->SetAspectLine (theWorkspace->AspectFace (Standard_True)->AspectEdge());
    const OpenGl_AspectLine* anAspect = theWorkspace->AspectLine (Standard_True);

    glPushAttrib (GL_POLYGON_BIT);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    if (aGlContext->IsGlGreaterEqual (2, 0))
    {
      BindProgramWithMaterial (theWorkspace, anAspect);
    }
  }

  /// OCC22236 NOTE: draw edges for all situations:
  /// 1) draw elements with GL_LINE style as edges from myPArray->bufferVBO[VBOEdges] indices array
  /// 2) draw elements from vertex array, when bounds defines count of primitive's vertices.
  /// 3) draw primitive's edges by vertexes if no edges and bounds array is specified
  if (!myVboAttribs.IsNull())
  {
    myVboAttribs->BindFixedPosition (aGlContext);
    glColor3fv (theEdgeColour->rgb);
    if (!myVboIndices.IsNull())
    {
      myVboIndices->Bind (aGlContext);

      // draw primitives by vertex count with the indices
      if (!myBounds.IsNull())
      {
        const size_t aStride  = myVboIndices->GetDataType() == GL_UNSIGNED_SHORT ? sizeof(unsigned short) : sizeof(unsigned int);
        GLubyte*     anOffset = NULL;
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
        glDrawElements (myDrawMode, myVboIndices->GetElemsNb(), myVboIndices->GetDataType(), NULL);
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
      glDrawArrays (myDrawMode, 0, myAttribs->NbElements);
    }

    // unbind buffers
    myVboAttribs->UnbindFixed (aGlContext, GL_VERTEX_ARRAY);
  }
  else
  {
    GLint aNbComp;
    for (Standard_Integer anAttribIter = 0; anAttribIter < myAttribs->NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = myAttribs->Attribute (anAttribIter);
      if (anAttrib.Id == Graphic3d_TOA_POS)
      {
        const GLenum  aDataType = toGlDataType (anAttrib.DataType, aNbComp);
        const GLvoid* aData     = myAttribs->Data (anAttribIter);
        OpenGl_VertexBuffer::bindFixed (aGlContext, anAttrib.Id, aNbComp, aDataType, myAttribs->Stride, aData);
        break;
      }
    }

    glColor3fv (theEdgeColour->rgb);
    if (!myBounds.IsNull())
    {
      if (!myIndices.IsNull())
      {
        const GLenum anIndexType = toGlIndexType (myIndices->Stride);
        GLint aFirstElem = 0;
        for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
        {
          const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
          glDrawElements (myDrawMode, aNbElemsInGroup, anIndexType, myIndices->value (aFirstElem));
          aFirstElem += aNbElemsInGroup;
        }
      }
      else
      {
        GLint aFirstElem = 0;
        for (Standard_Integer aGroupIter = 0; aGroupIter < myBounds->NbBounds; ++aGroupIter)
        {
          const GLint aNbElemsInGroup = myBounds->Bounds[aGroupIter];
          glDrawArrays (myDrawMode, aFirstElem, aNbElemsInGroup);
          aFirstElem += aNbElemsInGroup;
        }
      }
    }
    else if (!myIndices.IsNull())
    {
      glDrawElements (myDrawMode, myIndices->NbElements, toGlIndexType (myIndices->Stride), myIndices->Data());
    }
    else
    {
      glDrawArrays (myDrawMode, 0, myAttribs->NbElements);
    }
  }

  if (myDrawMode > GL_LINE_STRIP)
  {
    // Restore line context
    theWorkspace->SetAspectLine (anAspectLineOld);
    glPopAttrib();
  }
}

// =======================================================================
// function : DrawMarkers
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawMarkers (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const OpenGl_AspectMarker* anAspectMarker     = theWorkspace->AspectMarker (Standard_True);
  const Handle(OpenGl_Context)&     aCtx        = theWorkspace->GetGlContext();
  const Handle(OpenGl_PointSprite)& aSpriteNorm = anAspectMarker->SpriteRes (theWorkspace);
  const Standard_Boolean            isHilight   = (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT);
  if (aCtx->IsGlGreaterEqual (2, 0)
   && !aSpriteNorm.IsNull() && !aSpriteNorm->IsDisplayList())
  {
    // Textured markers will be drawn with the point sprites
    glPointSize (anAspectMarker->MarkerSize());

    Handle(OpenGl_Texture) aTextureBack;
    if (anAspectMarker->Type() != Aspect_TOM_POINT)
    {
      const Handle(OpenGl_PointSprite)& aSprite = (isHilight && anAspectMarker->SpriteHighlightRes (theWorkspace)->IsValid())
                                                ? anAspectMarker->SpriteHighlightRes (theWorkspace)
                                                : aSpriteNorm;
      aTextureBack = theWorkspace->EnableTexture (aSprite);

      glEnable (GL_ALPHA_TEST);
      glAlphaFunc (GL_GEQUAL, 0.1f);

      glEnable (GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glDrawArrays (myDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);

    glDisable (GL_BLEND);
    glDisable (GL_ALPHA_TEST);
    if (anAspectMarker->Type() != Aspect_TOM_POINT)
    {
      theWorkspace->EnableTexture (aTextureBack);
    }
    glPointSize (1.0f);
    return;
  }

  // Textured markers will be drawn with the glBitmap
  if (anAspectMarker->Type() == Aspect_TOM_POINT
   || anAspectMarker->Type() == Aspect_TOM_O_POINT)
  {
    const GLfloat aPntSize = anAspectMarker->Type() == Aspect_TOM_POINT
                           ? anAspectMarker->MarkerSize()
                           : 0.0f;
    if (aPntSize > 0.0f)
    {
      glPointSize (aPntSize);
    }
    glDrawArrays (myDrawMode, 0, !myVboAttribs.IsNull() ? myVboAttribs->GetElemsNb() : myAttribs->NbElements);
    if (aPntSize > 0.0f)
    {
      glPointSize (1.0f);
    }
  }

  if (anAspectMarker->Type() != Aspect_TOM_POINT
   && !aSpriteNorm.IsNull())
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
        glRasterPos3fv (myAttribs->Value<Graphic3d_Vec3> (anIter).GetData());
        aSpriteNorm->DrawBitmap (theWorkspace->GetGlContext());
      }
    }
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
  myIsVboInit (Standard_False),
  myUID       (theDriver->GetNextPrimitiveArrayUID())
{
  if (!myIndices.IsNull()
    && myIndices->NbElements < 1)
  {
    // dummy index buffer?
    myIndices.Nullify();
  }
  if (myAttribs.IsNull())
  {
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
    case Graphic3d_TOPA_POLYGONS:
      myDrawMode = GL_POLYGON;
      break;
    case Graphic3d_TOPA_TRIANGLES:
      myDrawMode = GL_TRIANGLES;
      break;
    case Graphic3d_TOPA_QUADRANGLES:
      myDrawMode = GL_QUADS;
      break;
    case Graphic3d_TOPA_TRIANGLESTRIPS:
      myDrawMode = GL_TRIANGLE_STRIP;
      break;
    case Graphic3d_TOPA_QUADRANGLESTRIPS:
      myDrawMode = GL_QUAD_STRIP;
      break;
    case Graphic3d_TOPA_TRIANGLEFANS:
      myDrawMode = GL_TRIANGLE_FAN;
      break;
    case Graphic3d_TOPA_UNDEFINED:
      break;
  }
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

  const OpenGl_AspectFace*   anAspectFace   = theWorkspace->AspectFace   (Standard_True);
  const OpenGl_AspectLine*   anAspectLine   = theWorkspace->AspectLine   (Standard_True);
  const OpenGl_AspectMarker* anAspectMarker = theWorkspace->AspectMarker (myDrawMode == GL_POINTS);

  // create VBOs on first render call
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  if (!myIsVboInit
   && !aCtx->caps->vboDisable
   && aCtx->core15 != NULL
   && (myDrawMode != GL_POINTS || anAspectMarker->SpriteRes (theWorkspace).IsNull() || !anAspectMarker->SpriteRes (theWorkspace)->IsDisplayList()))
  {
    if (!BuildVBO (theWorkspace))
    {
      TCollection_ExtendedString aMsg;
      aMsg += "VBO creation for Primitive Array has failed for ";
      aMsg += myAttribs->NbElements;
      aMsg += " vertices. Out of memory?";
      aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_PERFORMANCE_ARB, 0, GL_DEBUG_SEVERITY_LOW_ARB, aMsg);
    }
    myIsVboInit = Standard_True;
  }

  if (myDrawMode <= GL_LINE_STRIP)
  {
    glDisable (GL_LIGHTING);
  }

  Tint aFrontLightingModel = anAspectFace->IntFront().color_mask;
  const TEL_COLOUR* anInteriorColor = &anAspectFace->IntFront().matcol;
  const TEL_COLOUR* anEdgeColor = &anAspectFace->AspectEdge()->Color();
  const TEL_COLOUR* aLineColor  = myDrawMode == GL_POINTS ? &anAspectMarker->Color() : &anAspectLine->Color();

  // Use highlight colors
  if (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)
  {
    anEdgeColor = anInteriorColor = aLineColor = theWorkspace->HighlightColor;
    aFrontLightingModel = 0;
  }

  if (aCtx->IsGlGreaterEqual (2, 0))
  {
    switch (myDrawMode)
    {
      case GL_POINTS:
      {
        BindProgramWithMaterial (theWorkspace, anAspectMarker);
        break;
      }
      case GL_LINES:
      case GL_LINE_STRIP:
      {
        BindProgramWithMaterial (theWorkspace, anAspectLine);
        break;
      }
      default: // polygonal array
      {
        BindProgramWithMaterial (theWorkspace, anAspectFace);
        break;
      }
    }
  }

  DrawArray (aFrontLightingModel,
             anAspectFace->InteriorStyle(),
             anAspectFace->Edge(),
             anInteriorColor,
             aLineColor,
             anEdgeColor,
             theWorkspace);
}
