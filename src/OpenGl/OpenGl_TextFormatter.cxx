// Created on: 2013-01-29
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

#include <OpenGl_TextFormatter.hxx>

#include <OpenGl_VertexBuffer.hxx>

#include <cmath>

namespace
{

  //! Auxiliary function to translate rectangle by the vector.
  inline void move (Font_FTFont::Rect& theRect,
                    const OpenGl_Vec2& theVec)
  {
    theRect.Left   += theVec.x();
    theRect.Right  += theVec.x();
    theRect.Top    += theVec.y();
    theRect.Bottom += theVec.y();
  }

  //! Auxiliary function to translate rectangles by the vector.
  inline void move (NCollection_Vector<OpenGl_Font::Tile>& theRects,
                    const OpenGl_Vec2&                     theMoveVec,
                    Standard_Integer                       theCharLower,
                    const Standard_Integer                 theCharUpper)
  {
    for(; theCharLower <= theCharUpper; ++theCharLower)
    {
      Font_FTFont::Rect& aRect = theRects.ChangeValue (theCharLower).px;
      move (aRect, theMoveVec);
    }
  }

  //! Auxiliary function to translate rectangles in horizontal direction.
  /*inline void moveX (NCollection_Vector<OpenGl_Font::Tile>& theRects,
                     const Standard_ShortReal               theMoveVec,
                     Standard_Integer                       theCharLower,
                     const Standard_Integer                 theCharUpper)
  {
    for (; theCharLower <= theCharUpper; ++theCharLower)
    {
      Font_FTFont::Rect& aRect = theRects.ChangeValue (theCharLower).px;
      aRect.Left  += theMoveVec;
      aRect.Right += theMoveVec;
    }
  }*/

  //! Auxiliary function to translate rectangles in vertical direction.
  inline void moveY (NCollection_Vector<OpenGl_Font::Tile>& theRects,
                     const Standard_ShortReal               theMoveVec,
                     Standard_Integer                       theCharLower,
                     const Standard_Integer                 theCharUpper)
  {
    for(; theCharLower <= theCharUpper; ++theCharLower)
    {
      Font_FTFont::Rect& aRect = theRects.ChangeValue (theCharLower).px;
      aRect.Top    += theMoveVec;
      aRect.Bottom += theMoveVec;
    }
  }

  //! Apply floor to vector components.
  //! @param  theVec - vector to change (by reference!)
  //! @return modified vector
  inline OpenGl_Vec2& floor (OpenGl_Vec2& theVec)
  {
    theVec.x() = std::floor (theVec.x());
    theVec.y() = std::floor (theVec.y());
    return theVec;
  }

};

IMPLEMENT_STANDARD_HANDLE (OpenGl_TextFormatter, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_TextFormatter, Standard_Transient)

// =======================================================================
// function : OpenGl_TextFormatter
// purpose  :
// =======================================================================
OpenGl_TextFormatter::OpenGl_TextFormatter()
: myAlignX (Graphic3d_HTA_LEFT),
  myAlignY (Graphic3d_VTA_TOP),
  myTabSize (8),
  //
  myPen (0.0f, 0.0f),
  myRectsNb (0),
  myLineSpacing (0.0f),
  myAscender (0.0f),
  myIsFormatted (false),
  //
  myLinesNb (0),
  myRectLineStart (0),
  myRectWordStart (0),
  myNewLineNb(0),
  myPenCurrLine (0.0f),
  myLineLeft (0.0f),
  myLineTail (0.0f),
  myBndTop   (0.0f),
  myBndWidth (0.0f),
  myMoveVec (0.0f, 0.0f)
{
  //
}

// =======================================================================
// function : SetupAlignment
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::SetupAlignment (const Graphic3d_HorizontalTextAlignment theAlignX,
                                           const Graphic3d_VerticalTextAlignment   theAlignY)
{
  myAlignX = theAlignX;
  myAlignY = theAlignY;
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::Reset()
{
  myIsFormatted = false;
  myString.Clear();
  myPen.x() = myPen.y() = 0.0f;
  myRectsNb = 0;
  myLineSpacing = myAscender = 0.0f;
  myRects.Clear();
  myNewLines.Clear();
}

// =======================================================================
// function : Result
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::Result (NCollection_Vector<GLuint>& theTextures,
                                   NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > >& theVertsPerTexture,
                                   NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > >& theTCrdsPerTexture) const
{
  OpenGl_Vec2 aVec (0.0f, 0.0f);
  theTextures.Clear();
  theVertsPerTexture.Clear();
  theTCrdsPerTexture.Clear();
  for (Standard_Integer aRectIter = 0; aRectIter < myRectsNb; ++aRectIter)
  {
    const Font_FTFont::Rect& aRect    = myRects.Value (aRectIter).px;
    const Font_FTFont::Rect& aRectUV  = myRects.Value (aRectIter).uv;
    const GLuint             aTexture = myRects.Value (aRectIter).texture;

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
    aVerts.Append (floor(aRect.BottomLeft (aVec)));
    aVerts.Append (floor(aRect.TopLeft    (aVec)));
    aVerts.Append (floor(aRect.TopRight   (aVec)));
    aTCrds.Append (aRectUV.BottomLeft (aVec));
    aTCrds.Append (aRectUV.TopLeft    (aVec));
    aTCrds.Append (aRectUV.TopRight   (aVec));

    aVerts.Append (floor(aRect.BottomLeft  (aVec)));
    aVerts.Append (floor(aRect.TopRight    (aVec)));
    aVerts.Append (floor(aRect.BottomRight (aVec)));
    aTCrds.Append (aRectUV.BottomLeft  (aVec));
    aTCrds.Append (aRectUV.TopRight    (aVec));
    aTCrds.Append (aRectUV.BottomRight (aVec));
  }
}

// =======================================================================
// function : Result
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::Result (const Handle(OpenGl_Context)&                    theCtx,
                                   NCollection_Vector<GLuint>&                      theTextures,
                                   NCollection_Vector<Handle(OpenGl_VertexBuffer)>& theVertsPerTexture,
                                   NCollection_Vector<Handle(OpenGl_VertexBuffer)>& theTCrdsPerTexture) const
{
  NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > > aVertsPerTexture;
  NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > > aTCrdsPerTexture;
  Result (theTextures, aVertsPerTexture, aTCrdsPerTexture);

  if (theVertsPerTexture.Length() != theTextures.Length())
  {
    for (Standard_Integer aTextureIter = 0; aTextureIter < theVertsPerTexture.Length(); ++aTextureIter)
    {
      theVertsPerTexture.Value (aTextureIter)->Release (theCtx.operator->());
      theTCrdsPerTexture.Value (aTextureIter)->Release (theCtx.operator->());
    }
    theVertsPerTexture.Clear();
    theTCrdsPerTexture.Clear();

    while (theVertsPerTexture.Length() < theTextures.Length())
    {
      Handle(OpenGl_VertexBuffer) aVertsVbo = new OpenGl_VertexBuffer();
      Handle(OpenGl_VertexBuffer) aTcrdsVbo = new OpenGl_VertexBuffer();
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

// =======================================================================
// function : Result
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::Result (const Handle(OpenGl_Context)&                 /*theCtx*/,
                                   NCollection_Vector<GLuint>&                   theTextures,
                                   NCollection_Vector<Handle(OpenGl_Vec2Array)>& theVertsPerTexture,
                                   NCollection_Vector<Handle(OpenGl_Vec2Array)>& theTCrdsPerTexture) const
{
  NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > > aVertsPerTexture;
  NCollection_Vector< NCollection_Handle <NCollection_Vector <OpenGl_Vec2> > > aTCrdsPerTexture;
  Result (theTextures, aVertsPerTexture, aTCrdsPerTexture);

  theVertsPerTexture.Clear();
  theTCrdsPerTexture.Clear();

  for (Standard_Integer aTextureIter = 0; aTextureIter < theTextures.Length(); ++aTextureIter)
  {
    const NCollection_Vector<OpenGl_Vec2>& aVerts = *aVertsPerTexture.Value (aTextureIter);
    const NCollection_Vector<OpenGl_Vec2>& aTCrds = *aTCrdsPerTexture.Value (aTextureIter);
    Handle(OpenGl_Vec2Array) aVertsArray = new OpenGl_Vec2Array (1, aVerts.Length());
    Handle(OpenGl_Vec2Array) aTCrdsArray = new OpenGl_Vec2Array (1, aVerts.Length());
    theVertsPerTexture.Append (aVertsArray);
    theTCrdsPerTexture.Append (aTCrdsArray);

    for (Standard_Integer aVertIter = 0; aVertIter < aVerts.Length(); ++aVertIter)
    {
      aVertsArray->ChangeValue (aVertIter + 1) = aVerts.Value (aVertIter);
    }
    for (Standard_Integer aVertIter = 0; aVertIter < aVerts.Length(); ++aVertIter)
    {
      aTCrdsArray->ChangeValue (aVertIter + 1) = aTCrds.Value (aVertIter);
    }
  }
}

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::Append (const Handle(OpenGl_Context)& theCtx,
                                   const NCollection_String&     theString,
                                   OpenGl_Font&                  theFont)
{
  if (theString.IsEmpty())
  {
    return;
  }

  myAscender    = Max (myAscender,    theFont.Ascender());
  myLineSpacing = Max (myLineSpacing, theFont.LineSpacing());
  myString     += theString;

  int aSymbolsCounter = 0; // special counter to process tabulation symbols

  // first pass - render all symbols using associated font on single ZERO baseline
  OpenGl_Font::Tile aTile;
  memset (&aTile, 0, sizeof(aTile));
  for (NCollection_Utf8Iter anIter = theString.Iterator(); *anIter != 0;)
  {
    const Standard_Utf32Char aCharThis =   *anIter;
    const Standard_Utf32Char aCharNext = *++anIter;

    if (aCharThis == '\x0D' // CR  (carriage return)
     || aCharThis == '\a'   // BEL (alarm)
     || aCharThis == '\f'   // FF  (form feed) NP (new page)
     || aCharThis == '\b'   // BS  (backspace)
     || aCharThis == '\v')  // VT  (vertical tab)
    {
      continue; // skip unsupported carriage control codes
    }
    else if (aCharThis == '\x0A') // LF (line feed, new line)
    {
      aSymbolsCounter = 0;
      myNewLines.Append (myPen.x());
      continue; // will be processed on second pass
    }
    else if (aCharThis == ' ')
    {
      ++aSymbolsCounter;
      myPen.x() += theFont.AdvanceX (' ', aCharNext);
      continue;
    }
    else if (aCharThis == '\t')
    {
      const Standard_Integer aSpacesNum = (myTabSize - (aSymbolsCounter - 1) % myTabSize);
      myPen.x() += theFont.AdvanceX (' ', aCharNext) * Standard_ShortReal(aSpacesNum);
      aSymbolsCounter += aSpacesNum;
      continue;
    }

    ++aSymbolsCounter;
    theFont.RenderGlyph (theCtx,
                         aCharThis, aCharNext,
                         aTile, myPen);
    myRects.Append (aTile);

    ++myRectsNb;
  }
}

// =======================================================================
// function : newLine
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::newLine (const Standard_Integer theLastRect)
{
  if (myRectLineStart >= myRectsNb)
  {
    ++myLinesNb;
    myPenCurrLine -= myLineSpacing;
    return;
  }

  myMoveVec.y() = myPenCurrLine;
  switch (myAlignX)
  {
    default:
    case Graphic3d_HTA_LEFT:
    {
      myMoveVec.x() = (myNewLineNb > 0) ? -myNewLines.Value (myNewLineNb - 1) : 0.0f;
      break;
    }
    case Graphic3d_HTA_RIGHT:
    {
      myMoveVec.x() = (myNewLineNb < myNewLines.Length())
                    ? -myNewLines.Value (myNewLineNb)
                    : -myPen.x();
      break;
    }
    case Graphic3d_HTA_CENTER:
    {
      const Standard_ShortReal aFrom = (myNewLineNb > 0)
                                     ? myNewLines.Value (myNewLineNb - 1)
                                     : 0.0f;
      const Standard_ShortReal aTo   = (myNewLineNb < myNewLines.Length())
                                     ? myNewLines.Value (myNewLineNb)
                                     : myPen.x();
      myMoveVec.x() = -0.5f * (aFrom + aTo);
      break;
    }
  }

  move (myRects, myMoveVec, myRectLineStart, theLastRect);

  ++myLinesNb;
  myPenCurrLine -= myLineSpacing;
  myRectLineStart = myRectWordStart = theLastRect + 1;
  if (myRectLineStart < myRectsNb)
  {
    myLineLeft = myRects.Value (myRectLineStart).px.Left;
  }
}

// =======================================================================
// function : Format
// purpose  :
// =======================================================================
void OpenGl_TextFormatter::Format()
{
  if (myRectsNb == 0 || myIsFormatted)
  {
    return;
  }

  myIsFormatted = true;
  myLinesNb = myRectLineStart = myRectWordStart = 0;
  myLineLeft   = 0.0f;
  myLineTail   = 0.0f;
  myBndTop     = 0.0f;
  myBndWidth   = 0.0f;
  myMoveVec.x() = myMoveVec.y() = 0.0f;

  // split text into lines and apply horizontal alignment
  myPenCurrLine = -myAscender;
  Standard_Integer aRectIter = 0;
  myNewLineNb = 0;
  for (NCollection_Utf8Iter anIter = myString.Iterator(); *anIter != 0; ++anIter)
  {
    const Standard_Utf32Char aCharThis = *anIter;
    if (aCharThis == '\x0D' // CR  (carriage return)
     || aCharThis == '\a'   // BEL (alarm)
     || aCharThis == '\f'   // FF  (form feed) NP (new page)
     || aCharThis == '\b'   // BS  (backspace)
     || aCharThis == '\v')  // VT  (vertical tab)
    {
      continue; // skip unsupported carriage control codes
    }
    else if (aCharThis == '\x0A') // LF (line feed, new line)
    {
      const Standard_Integer aLastRect = aRectIter - 1; // last rect on current line
      newLine (aLastRect);
      ++myNewLineNb;
      continue;
    }
    else if (aCharThis == ' '
          || aCharThis == '\t')
    {
      myRectWordStart = aRectIter;
      continue;
    }

    Standard_ShortReal aWidth = myRects.Value (aRectIter).px.Right - myLineLeft;
    myBndWidth = Max (myBndWidth, aWidth);

    ++aRectIter;
  }

  // move last line
  newLine (myRectsNb - 1);

  // apply vertical alignment style
  if (myAlignY == Graphic3d_VTA_BOTTOM)
  {
    myBndTop = -myLineSpacing - myPenCurrLine;
    moveY (myRects, myBndTop, 0, myRectsNb - 1);
  }
  else if (myAlignY == Graphic3d_VTA_CENTER)
  {
    myBndTop = 0.5f * (myLineSpacing * Standard_ShortReal(myLinesNb));
    moveY (myRects, myBndTop, 0, myRectsNb - 1);
  }
}
