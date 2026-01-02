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
inline NCollection_Vec2<float>& floor(NCollection_Vec2<float>& theVec)
{
  theVec.x() = std::floor(theVec.x());
  theVec.y() = std::floor(theVec.y());
  return theVec;
}
} // namespace

//=================================================================================================

OpenGl_TextBuilder::OpenGl_TextBuilder() = default;

//=================================================================================================

void OpenGl_TextBuilder::createGlyphs(
  const occ::handle<Font_TextFormatter>& theFormatter,
  const occ::handle<OpenGl_Context>&     theCtx,
  OpenGl_Font&                           theFont,
  NCollection_Vector<GLuint>&            theTextures,
  NCollection_Vector<NCollection_Handle<NCollection_Vector<NCollection_Vec2<float>>>>&
    theVertsPerTexture,
  NCollection_Vector<NCollection_Handle<NCollection_Vector<NCollection_Vec2<float>>>>&
    theTCrdsPerTexture)
{
  NCollection_Vec2<float> aVec(0.0f, 0.0f);

  theTextures.Clear();
  theVertsPerTexture.Clear();
  theTCrdsPerTexture.Clear();

  OpenGl_Font::Tile aTile = {Font_Rect(), Font_Rect(), 0u};
  for (Font_TextFormatter::Iterator aFormatterIt(
         *theFormatter,
         Font_TextFormatter::IterationFilter_ExcludeInvisible);
       aFormatterIt.More();
       aFormatterIt.Next())
  {
    theFont.RenderGlyph(theCtx, aFormatterIt.Symbol(), aTile);

    const NCollection_Vec2<float>& aBottomLeft =
      theFormatter->BottomLeft(aFormatterIt.SymbolPosition());
    aTile.px.Right += aBottomLeft.x();
    aTile.px.Left += aBottomLeft.x();
    aTile.px.Bottom += aBottomLeft.y();
    aTile.px.Top += aBottomLeft.y();
    const Font_Rect& aRectUV  = aTile.uv;
    const GLuint     aTexture = aTile.texture;

    int aListId = 0;
    for (aListId = 0; aListId < theTextures.Length(); ++aListId)
    {
      if (theTextures.Value(aListId) == aTexture)
      {
        break;
      }
    }
    if (aListId >= theTextures.Length())
    {
      theTextures.Append(aTexture);
      theVertsPerTexture.Append(new NCollection_Vector<NCollection_Vec2<float>>());
      theTCrdsPerTexture.Append(new NCollection_Vector<NCollection_Vec2<float>>());
    }

    NCollection_Vector<NCollection_Vec2<float>>& aVerts = *theVertsPerTexture.ChangeValue(aListId);
    NCollection_Vector<NCollection_Vec2<float>>& aTCrds = *theTCrdsPerTexture.ChangeValue(aListId);

    // apply floor on position to avoid blurring issues
    // due to cross-pixel coordinates
    aVerts.Append(floor(aTile.px.TopRight(aVec)));
    aVerts.Append(floor(aTile.px.TopLeft(aVec)));
    aVerts.Append(floor(aTile.px.BottomLeft(aVec)));
    aTCrds.Append(aRectUV.TopRight(aVec));
    aTCrds.Append(aRectUV.TopLeft(aVec));
    aTCrds.Append(aRectUV.BottomLeft(aVec));

    aVerts.Append(floor(aTile.px.BottomRight(aVec)));
    aVerts.Append(floor(aTile.px.TopRight(aVec)));
    aVerts.Append(floor(aTile.px.BottomLeft(aVec)));
    aTCrds.Append(aRectUV.BottomRight(aVec));
    aTCrds.Append(aRectUV.TopRight(aVec));
    aTCrds.Append(aRectUV.BottomLeft(aVec));
  }
}

//=================================================================================================

void OpenGl_TextBuilder::Perform(
  const occ::handle<Font_TextFormatter>&                theFormatter,
  const occ::handle<OpenGl_Context>&                    theCtx,
  OpenGl_Font&                                          theFont,
  NCollection_Vector<GLuint>&                           theTextures,
  NCollection_Vector<occ::handle<OpenGl_VertexBuffer>>& theVertsPerTexture,
  NCollection_Vector<occ::handle<OpenGl_VertexBuffer>>& theTCrdsPerTexture)
{
  NCollection_Vector<NCollection_Handle<NCollection_Vector<NCollection_Vec2<float>>>>
    aVertsPerTexture;
  NCollection_Vector<NCollection_Handle<NCollection_Vector<NCollection_Vec2<float>>>>
    aTCrdsPerTexture;

  createGlyphs(theFormatter, theCtx, theFont, theTextures, aVertsPerTexture, aTCrdsPerTexture);

  if (theVertsPerTexture.Length() != theTextures.Length())
  {
    for (int aTextureIter = 0; aTextureIter < theVertsPerTexture.Length(); ++aTextureIter)
    {
      theVertsPerTexture.Value(aTextureIter)->Release(theCtx.operator->());
      theTCrdsPerTexture.Value(aTextureIter)->Release(theCtx.operator->());
    }
    theVertsPerTexture.Clear();
    theTCrdsPerTexture.Clear();

    const bool                       isNormalMode = theCtx->ToUseVbo();
    occ::handle<OpenGl_VertexBuffer> aVertsVbo, aTcrdsVbo;
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
      theVertsPerTexture.Append(aVertsVbo);
      theTCrdsPerTexture.Append(aTcrdsVbo);
      aVertsVbo->Create(theCtx);
      aTcrdsVbo->Create(theCtx);
    }
  }

  for (int aTextureIter = 0; aTextureIter < theTextures.Length(); ++aTextureIter)
  {
    const NCollection_Vector<NCollection_Vec2<float>>& aVerts =
      *aVertsPerTexture.Value(aTextureIter);
    occ::handle<OpenGl_VertexBuffer>& aVertsVbo = theVertsPerTexture.ChangeValue(aTextureIter);
    if (!aVertsVbo->Init(theCtx, 2, aVerts.Length(), (GLfloat*)nullptr)
        || !myVboEditor.Init(theCtx, aVertsVbo))
    {
      continue;
    }
    for (int aVertIter = 0; aVertIter < aVerts.Length(); ++aVertIter, myVboEditor.Next())
    {
      myVboEditor.Value() = aVerts.Value(aVertIter);
    }
    myVboEditor.Flush();

    const NCollection_Vector<NCollection_Vec2<float>>& aTCrds =
      *aTCrdsPerTexture.Value(aTextureIter);
    occ::handle<OpenGl_VertexBuffer>& aTCrdsVbo = theTCrdsPerTexture.ChangeValue(aTextureIter);
    if (!aTCrdsVbo->Init(theCtx, 2, aVerts.Length(), (GLfloat*)nullptr)
        || !myVboEditor.Init(theCtx, aTCrdsVbo))
    {
      continue;
    }
    for (int aVertIter = 0; aVertIter < aVerts.Length(); ++aVertIter, myVboEditor.Next())
    {
      myVboEditor.Value() = aTCrds.Value(aVertIter);
    }
    myVboEditor.Flush();
  }
  myVboEditor.Init(nullptr, nullptr);
}
