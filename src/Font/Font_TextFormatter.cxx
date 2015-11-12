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

#include <Font_TextFormatter.hxx>

#include <Font_FTFont.hxx>

namespace
{
  typedef NCollection_Vec2<Standard_ShortReal> Vec2f;

  //! Auxiliary function to translate corners by the vector.
  inline void move (NCollection_Vector< Vec2f >& theCorners,
                    const Vec2f&                 theMoveVec,
                    Standard_Integer             theCharLower,
                    const Standard_Integer       theCharUpper)
  {
    for(; theCharLower <= theCharUpper; ++theCharLower)
    {
      theCorners.ChangeValue (theCharLower) += theMoveVec;
    }
  }

  //! Auxiliary function to translate corners in vertical direction.
  inline void moveY (NCollection_Vector<Vec2f>& theCorners,
                     const Standard_ShortReal   theMoveVec,
                     Standard_Integer           theCharLower,
                     const Standard_Integer     theCharUpper)
  {
    for(; theCharLower <= theCharUpper; ++theCharLower)
    {
      theCorners.ChangeValue (theCharLower).y() += theMoveVec;
    }
  }

}

// =======================================================================
// function : Font_TextFormatter
// purpose  :
// =======================================================================
Font_TextFormatter::Font_TextFormatter()
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
void Font_TextFormatter::SetupAlignment (const Graphic3d_HorizontalTextAlignment theAlignX,
                                         const Graphic3d_VerticalTextAlignment   theAlignY)
{
  myAlignX = theAlignX;
  myAlignY = theAlignY;
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void Font_TextFormatter::Reset()
{
  myIsFormatted = false;
  myString.Clear();
  myPen.x() = myPen.y() = 0.0f;
  myRectsNb = 0;
  myLineSpacing = myAscender = 0.0f;
  myCorners.Clear();
  myNewLines.Clear();
}

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
void Font_TextFormatter::Append (const NCollection_String& theString,
                                 Font_FTFont&              theFont)
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

    myCorners.Append (myPen);

    myPen.x() += theFont.AdvanceX (aCharThis, aCharNext);

    ++myRectsNb;
  }
}

// =======================================================================
// function : newLine
// purpose  :
// =======================================================================
void Font_TextFormatter::newLine (const Standard_Integer theLastRect)
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

  move (myCorners, myMoveVec, myRectLineStart, theLastRect);

  ++myLinesNb;
  myPenCurrLine -= myLineSpacing;
  myRectLineStart = myRectWordStart = theLastRect + 1;
}

// =======================================================================
// function : Format
// purpose  :
// =======================================================================
void Font_TextFormatter::Format()
{
  if (myRectsNb == 0 || myIsFormatted)
  {
    return;
  }

  myIsFormatted = true;
  myLinesNb = myRectLineStart = myRectWordStart = 0;
  myBndTop     = 0.0f;
  myBndWidth   = 0.0f;
  myMoveVec.x() = myMoveVec.y() = 0.0f;

  // split text into lines and apply horizontal alignment
  myPenCurrLine = -myAscender;
  Standard_Integer aRectIter = 0;
  myNewLineNb = 0;
  Standard_ShortReal aMaxLineWidth = -1.0f;
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
      // calculate max line width
      if (myNewLineNb == 0)
      {
        aMaxLineWidth = myNewLines.Value(0);
      }
      else
      {
        aMaxLineWidth = Max (aMaxLineWidth, myNewLines.Value (myNewLineNb) - myNewLines.Value (myNewLineNb - 1));
      }

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

    ++aRectIter;
  }

  // If only one line
  if (aMaxLineWidth < 0.0f)
  {
    aMaxLineWidth = myPen.x();
  }
  else // Consider last line
  {
    aMaxLineWidth = Max (aMaxLineWidth, myPen.x() - myNewLines.Value (myNewLineNb - 1));
  }

  myBndWidth = aMaxLineWidth;

  // move last line
  newLine (myRectsNb - 1);

  // apply vertical alignment style
  if (myAlignY == Graphic3d_VTA_BOTTOM)
  {
    myBndTop = -myLineSpacing - myPenCurrLine;
  }
  else if (myAlignY == Graphic3d_VTA_CENTER)
  {
    myBndTop = 0.5f * (myLineSpacing * Standard_ShortReal(myLinesNb));
  }
  else if (myAlignY == Graphic3d_VTA_TOPFIRSTLINE)
  {
    myBndTop = myAscender;
  }

  if (myAlignY != Graphic3d_VTA_TOP)
  {
    moveY (myCorners, myBndTop, 0, myRectsNb - 1);
  }
}
