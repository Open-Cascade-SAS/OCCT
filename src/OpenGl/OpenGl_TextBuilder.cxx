// Created on: 2015-06-18
// Created by: Ilya SEVRIKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <OpenGl_TextBuilder.hxx>
#include <OpenGl_VertexBufferCompat.hxx>

#include <Font_FTFont.hxx>
#include <Font_TextFormatter.hxx>

namespace
{
  //! Apply floor to vector components.
  //! @param  theVec - vector to change (by reference!)
  //! @return modified vector
  inline OpenGl_Vec2& floor (OpenGl_Vec2& theVec)
  {
    theVec.x() = std::floor (theVec.x());
    theVec.y() = std::floor (theVec.y());
    return theVec;
  }
}

// =======================================================================
// function : OpenGl_TextBuilder
// purpose  :
// =======================================================================
OpenGl_TextBuilder::OpenGl_TextBuilder()
{
  //
}

// =======================================================================
// function : createGlyphs
// purpose  :
// =======================================================================
void OpenGl_TextBuilder::createGlyphs (const Handle(Font_TextFormatter)&                                          theFormatter,
                                       const Handle(OpenGl_Context)&                                              theCtx,
                                       OpenGl_Font&                                                               theFont,
                                       NCollection_Vector<GLuint>&                                                theTextures,
                                       NCollection_Vector<NCollection_Handle<NCollection_Vector<OpenGl_Vec2> > >& theVertsPerTexture,
                                       NCollection_Vector<NCollection_Handle<NCollection_Vector<OpenGl_Vec2> > >& theTCrdsPerTexture)
{
  OpenGl_Vec2 aVec (0.0f, 0.0f);

  theTextures.Clear();
  theVertsPerTexture.Clear();
  theTCrdsPerTexture.Clear();

  OpenGl_Font::Tile aTile = {Font_Rect(), Font_Rect(), 0u};
  for (Font_TextFormatter::Iterator aFormatterIt (*theFormatter, Font_TextFormatter::IterationFilter_ExcludeInvisible);
       aFormatterIt.More(); aFormatterIt.Next())
  {
    theFont.RenderGlyph (theCtx, aFormatterIt.Symbol(), aTile);

    const OpenGl_Vec2& aBottomLeft = theFormatter->BottomLeft (aFormatterIt.SymbolPosition());
    aTile.px.Right  += aBottomLeft.x();
    aTile.px.Left   += aBottomLeft.x();
    aTile.px.Bottom += aBottomLeft.y();
    aTile.px.Top    += aBottomLeft.y();
    const Font_Rect& aRectUV  = aTile.uv;
    const GLuint     aTexture = aTile.texture;

    Standard_Integer aListId = 0;
    for (aListId = 0; aListId < theTextures.Length(); ++aListId)
    {
      if (theTextures.Value (aListId) == aTexture)
      {
        break;
      }
    }
    if (aListId >= theTextures.Length())
    {
      theTextures.Append (aTexture);
      theVertsPerTexture.Append (new NCollection_Vector<OpenGl_Vec2>());
      theTCrdsPerTexture.Append (new NCollection_Vector<OpenGl_Vec2>());
    }

    NCollection_Vector<OpenGl_Vec2>& aVerts = *theVertsPerTexture.ChangeValue (aListId);
    NCollection_Vector<OpenGl_Vec2>& aTCrds = *theTCrdsPerTexture.ChangeValue (aListId);

    // apply floor on position to avoid blurring issues
    // due to cross-pixel coordinates
    aVerts.Append (floor(aTile.px.TopRight   (aVec)));
    aVerts.Append (floor(aTile.px.TopLeft    (aVec)));
    aVerts.Append (floor(aTile.px.BottomLeft (aVec)));
    aTCrds.Append (aRectUV.TopRight   (aVec));
    aTCrds.Append (aRectUV.TopLeft    (aVec));
    aTCrds.Append (aRectUV.BottomLeft (aVec));

    aVerts.Append (floor(aTile.px.BottomRight (aVec)));
    aVerts.Append (floor(aTile.px.TopRight    (aVec)));
    aVerts.Append (floor(aTile.px.BottomLeft  (aVec)));
    aTCrds.Append (aRectUV.BottomRight (aVec));
    aTCrds.Append (aRectUV.TopRight    (aVec));
    aTCrds.Append (aRectUV.BottomLeft  (aVec));
  }
}

// =======================================================================
// function : CreateTextures
// purpose  :
// =======================================================================
void OpenGl_TextBuilder::Perform (const Handle(Font_TextFormatter)&                theFormatter,
                                  const Handle(OpenGl_Context)&                    theCtx,
                                  OpenGl_Font&                                     theFont,
                                  NCollection_Vector<GLuint>&                      theTextures,
                                  NCollection_Vector<Handle(OpenGl_VertexBuffer)>& theVertsPerTexture,
                                  NCollection_Vector<Handle(OpenGl_VertexBuffer)>& theTCrdsPerTexture)
{
  NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > > aVertsPerTexture;
  NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > > aTCrdsPerTexture;

  createGlyphs (theFormatter, theCtx, theFont, theTextures, aVertsPerTexture, aTCrdsPerTexture);

  if (theVertsPerTexture.Length() != theTextures.Length())
  {
    for (Standard_Integer aTextureIter = 0; aTextureIter < theVertsPerTexture.Length(); ++aTextureIter)
    {
      theVertsPerTexture.Value (aTextureIter)->Release (theCtx.operator->());
      theTCrdsPerTexture.Value (aTextureIter)->Release (theCtx.operator->());
    }
    theVertsPerTexture.Clear();
    theTCrdsPerTexture.Clear();

    const bool isNormalMode = theCtx->ToUseVbo();
    Handle(OpenGl_VertexBuffer) aVertsVbo, aTcrdsVbo;
    while (theVertsPerTexture.Length() < theTextures.Length())
    {
      if (isNormalMode)
      {
        aVertsVbo = new OpenGl_VertexBuffer();
        aTcrdsVbo = new OpenGl_VertexBuffer();
      }
      else
      {
        aVertsVbo = new OpenGl_VertexBufferCompat();
        aTcrdsVbo = new OpenGl_VertexBufferCompat();
      }
      theVertsPerTexture.Append (aVertsVbo);
      theTCrdsPerTexture.Append (aTcrdsVbo);
      aVertsVbo->Create (theCtx);
      aTcrdsVbo->Create (theCtx);
    }
  }

  for (Standard_Integer aTextureIter = 0; aTextureIter < theTextures.Length(); ++aTextureIter)
  {
    const NCollection_Vector<OpenGl_Vec2>& aVerts = *aVertsPerTexture.Value (aTextureIter);
    Handle(OpenGl_VertexBuffer)& aVertsVbo = theVertsPerTexture.ChangeValue (aTextureIter);
    if (!aVertsVbo->Init (theCtx, 2, aVerts.Length(), (GLfloat* )NULL)
     || !myVboEditor.Init (theCtx, aVertsVbo))
    {
      continue;
    }
    for (Standard_Integer aVertIter = 0; aVertIter < aVerts.Length(); ++aVertIter, myVboEditor.Next())
    {
      myVboEditor.Value() = aVerts.Value (aVertIter);
    }
    myVboEditor.Flush();

    const NCollection_Vector<OpenGl_Vec2>& aTCrds = *aTCrdsPerTexture.Value (aTextureIter);
    Handle(OpenGl_VertexBuffer)& aTCrdsVbo = theTCrdsPerTexture.ChangeValue (aTextureIter);
    if (!aTCrdsVbo->Init (theCtx, 2, aVerts.Length(), (GLfloat* )NULL)
     || !myVboEditor.Init (theCtx, aTCrdsVbo))
    {
      continue;
    }
    for (Standard_Integer aVertIter = 0; aVertIter < aVerts.Length(); ++aVertIter, myVboEditor.Next())
    {
      myVboEditor.Value() = aTCrds.Value (aVertIter);
    }
    myVboEditor.Flush();
  }
  myVboEditor.Init (NULL, NULL);
}
