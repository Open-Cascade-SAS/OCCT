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

#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Font_TextFormatter, Standard_Transient)

namespace
{
typedef NCollection_Vec2<float> Vec2f;

//! Auxiliary function to translate corners by the vector.
inline void move(NCollection_Vector<Vec2f>& theCorners,
                 const Vec2f&               theMoveVec,
                 int                        theCharLower,
                 const int                  theCharUpper)
{
  for (; theCharLower <= theCharUpper; ++theCharLower)
  {
    theCorners.ChangeValue(theCharLower) += theMoveVec;
  }
}

//! Auxiliary function to translate corners in vertical direction.
inline void moveY(NCollection_Vector<Vec2f>& theCorners,
                  const float                theMoveVec,
                  int                        theCharLower,
                  const int                  theCharUpper)
{
  for (; theCharLower <= theCharUpper; ++theCharLower)
  {
    theCorners.ChangeValue(theCharLower).y() += theMoveVec;
  }
}

} // namespace

//=================================================================================================

Font_TextFormatter::Font_TextFormatter()
    : myAlignX(Graphic3d_HTA_LEFT),
      myAlignY(Graphic3d_VTA_TOP),
      myTabSize(8),
      myWrappingWidth(0.0f),
      myIsWordWrapping(true),
      myLastSymbolWidth(0.0f),
      myMaxSymbolWidth(0.0f),
      //
      myPen(0.0f, 0.0f),
      myLineSpacing(0.0f),
      myAscender(0.0f),
      myIsFormatted(false),
      //
      myLinesNb(0),
      myRectLineStart(0),
      myNewLineNb(0),
      myPenCurrLine(0.0f),
      myBndTop(0.0f),
      myBndWidth(0.0f),
      myMoveVec(0.0f, 0.0f)
{
  //
}

//=================================================================================================

void Font_TextFormatter::SetupAlignment(const Graphic3d_HorizontalTextAlignment theAlignX,
                                        const Graphic3d_VerticalTextAlignment   theAlignY)
{
  myAlignX = theAlignX;
  myAlignY = theAlignY;
}

//=================================================================================================

void Font_TextFormatter::Reset()
{
  myIsFormatted = false;
  myString.Clear();
  myPen.x() = myPen.y() = 0.0f;
  myLineSpacing = myAscender = 0.0f;
  myCorners.Clear();
  myNewLines.Clear();

  myLastSymbolWidth = 0.0f;
  myMaxSymbolWidth  = 0.0f;
}

//=================================================================================================

void Font_TextFormatter::Append(const NCollection_String& theString, Font_FTFont& theFont)
{
  if (theString.IsEmpty())
  {
    return;
  }

  myAscender    = std::max(myAscender, theFont.Ascender());
  myLineSpacing = std::max(myLineSpacing, theFont.LineSpacing());
  myString += theString;

  int aSymbolsCounter = 0; // special counter to process tabulation symbols

  // first pass - render all symbols using associated font on single ZERO baseline
  for (Font_TextFormatter::Iterator aFormatterIt(*this); aFormatterIt.More(); aFormatterIt.Next())
  {
    const char32_t aCharThis = aFormatterIt.Symbol();
    const char32_t aCharNext = aFormatterIt.SymbolNext();

    float anAdvanceX = 0;
    if (IsCommandSymbol(aCharThis))
    {
      continue; // skip unsupported carriage control codes
    }
    else if (aCharThis == '\x0A') // LF (line feed, new line)
    {
      aSymbolsCounter = 0;
      myNewLines.Append(myPen.x());
      anAdvanceX = 0; // the symbol has null width
    }
    else if (aCharThis == ' ')
    {
      anAdvanceX = theFont.AdvanceX(' ', aCharNext);
    }
    else if (aCharThis == '\t')
    {
      const int aSpacesNum = (myTabSize - (aSymbolsCounter - 1) % myTabSize);
      anAdvanceX           = theFont.AdvanceX(' ', aCharNext) * float(aSpacesNum);
      aSymbolsCounter += aSpacesNum;
    }
    else
    {
      anAdvanceX = theFont.AdvanceX(aCharThis, aCharNext);
    }
    ++aSymbolsCounter;
    myCorners.Append(myPen);
    myPen.x() += anAdvanceX;
    myMaxSymbolWidth = std::max(myMaxSymbolWidth, anAdvanceX);
  }
  myLastSymbolWidth = myPen.x() - myCorners.Last().x();
}

//=================================================================================================

void Font_TextFormatter::newLine(const int theLastRect, const float theMaxLineWidth)
{
  int aFirstCornerId = myRectLineStart;
  int aLastCornerId  = theLastRect;

  if (aFirstCornerId >= myCorners.Length())
  {
    ++myLinesNb;
    myPenCurrLine -= myLineSpacing;
    return;
  }

  float     aXMin = BottomLeft(aFirstCornerId).x();
  Font_Rect aBndBox;
  GlyphBoundingBox(aLastCornerId, aBndBox);
  float aXMax = aBndBox.Right;

  myMoveVec.y() = myPenCurrLine;
  switch (myAlignX)
  {
    default:
    case Graphic3d_HTA_LEFT:
      myMoveVec.x() = -aXMin;
      break;
    case Graphic3d_HTA_RIGHT:
      myMoveVec.x() = -aXMin + (theMaxLineWidth - (aXMax - aXMin)) - theMaxLineWidth;
      break;
    case Graphic3d_HTA_CENTER:
      myMoveVec.x() = -aXMin + 0.5f * (theMaxLineWidth - (aXMax - aXMin)) - 0.5f * theMaxLineWidth;
      break;
  }

  move(myCorners, myMoveVec, myRectLineStart, theLastRect);

  ++myLinesNb;
  myPenCurrLine -= myLineSpacing;
  myRectLineStart = theLastRect + 1;
}

//=================================================================================================

void Font_TextFormatter::Format()
{
  if (myCorners.Length() == 0 || myIsFormatted)
  {
    return;
  }

  myIsFormatted = true;
  myLinesNb = myRectLineStart = 0;
  myBndTop                    = 0.0f;
  myBndWidth                  = 0.0f;
  myMoveVec.x() = myMoveVec.y() = 0.0f;

  // split text into lines and apply horizontal alignment
  myPenCurrLine = -myAscender;
  int aRectIter = 0;
  myNewLineNb   = 0;

  float aMaxLineWidth = Wrapping();
  if (HasWrapping())
  {
    // it is not possible to wrap less than symbol width
    aMaxLineWidth = std::max(aMaxLineWidth, MaximumSymbolWidth());
  }
  else
  {
    if (myNewLines.IsEmpty()) // If only one line
    {
      aMaxLineWidth = myPen.x();
    }
    else
    {
      for (int aLineIt = 0; aLineIt < myNewLines.Size(); aLineIt++)
      {
        aMaxLineWidth = std::max(aMaxLineWidth, LineWidth(aLineIt));
      }
      // clang-format off
      aMaxLineWidth = std::max(aMaxLineWidth, LineWidth (myNewLines.Size())); // processing the last line also
      // clang-format on
    }
  }

  char32_t aCharPrev = 0;
  for (Font_TextFormatter::Iterator aFormatterIt(*this); aFormatterIt.More(); aFormatterIt.Next())
  {
    const char32_t aCharThis = aFormatterIt.Symbol();
    aRectIter                = aFormatterIt.SymbolPosition();

    if (aCharThis == '\x0A') // LF (line feed, new line)
    {
      const int aLastRect = aRectIter; // last rect on current line
      newLine(aLastRect, aMaxLineWidth);
      ++myNewLineNb;
      continue;
    }
    else if (HasWrapping()) // wrap lines longer than maximum width
    {
      int aFirstCornerId = myRectLineStart;

      Font_Rect aBndBox;
      GlyphBoundingBox(aRectIter, aBndBox);
      const float aNextXPos     = aBndBox.Right - BottomLeft(aFirstCornerId).x();
      bool        isCurWordFits = true;
      if (myIsWordWrapping && IsSeparatorSymbol(aCharPrev))
      {
        for (Font_TextFormatter::Iterator aWordIt = aFormatterIt; aWordIt.More(); aWordIt.Next())
        {
          if (IsSeparatorSymbol(aWordIt.Symbol()))
          {
            break;
          }
          float aWordWidthPx = myCorners[aWordIt.SymbolPosition()].x() - myCorners[aRectIter].x();
          if (aNextXPos + aWordWidthPx > aMaxLineWidth)
          {
            isCurWordFits = false;
            break;
          }
        }
      }
      if (aNextXPos > aMaxLineWidth
          || !isCurWordFits) // wrap the line and do processing of the symbol
      {
        const int aLastRect = aRectIter - 1; // last rect on current line
        newLine(aLastRect, aMaxLineWidth);
      }
    }
    aCharPrev = aCharThis;
  }

  myBndWidth = aMaxLineWidth;

  // move last line
  newLine(myCorners.Length() - 1, aMaxLineWidth);

  // apply vertical alignment style
  if (myAlignY == Graphic3d_VTA_BOTTOM)
  {
    myBndTop = -myLineSpacing - myPenCurrLine;
  }
  else if (myAlignY == Graphic3d_VTA_CENTER)
  {
    myBndTop = 0.5f * (myLineSpacing * float(myLinesNb));
  }
  else if (myAlignY == Graphic3d_VTA_TOPFIRSTLINE)
  {
    myBndTop = myAscender;
  }

  if (myAlignY != Graphic3d_VTA_TOP)
  {
    moveY(myCorners, myBndTop, 0, myCorners.Length() - 1);
  }
}

//=================================================================================================

bool Font_TextFormatter::GlyphBoundingBox(const int theIndex, Font_Rect& theBndBox) const
{
  if (theIndex < 0 || theIndex >= Corners().Size())
  {
    return false;
  }

  const NCollection_Vec2<float>& aLeftCorner = BottomLeft(theIndex);
  theBndBox.Left                             = aLeftCorner.x();
  theBndBox.Right                            = aLeftCorner.x() + myLastSymbolWidth;
  theBndBox.Bottom                           = aLeftCorner.y();
  theBndBox.Top                              = theBndBox.Bottom + myLineSpacing;
  if (theIndex + 1 >= myCorners.Length())
  {
    // the last symbol
    return true;
  }

  const NCollection_Vec2<float>& aNextLeftCorner = BottomLeft(theIndex + 1);
  if (std::abs(aLeftCorner.y() - aNextLeftCorner.y()) < Precision::Confusion()) // in the same row
  {
    theBndBox.Right = aNextLeftCorner.x();
  }
  else
  {
    // the next symbol is on the next row either by '\n' or by wrapping
    float aLineWidth = LineWidth(LineIndex(theIndex));
    theBndBox.Left   = aLeftCorner.x();
    switch (myAlignX)
    {
      case Graphic3d_HTA_LEFT:
        theBndBox.Right = aLineWidth;
        break;
      case Graphic3d_HTA_RIGHT:
        theBndBox.Right = myBndWidth;
        break;
      case Graphic3d_HTA_CENTER:
        theBndBox.Right = 0.5f * (myBndWidth + aLineWidth);
        break;
    }
  }
  return true;
}

//=================================================================================================

bool Font_TextFormatter::IsLFSymbol(const int theIndex) const
{
  Font_Rect aBndBox;
  if (!GlyphBoundingBox(theIndex, aBndBox))
  {
    return false;
  }

  return std::abs(aBndBox.Right - aBndBox.Left) < Precision::Confusion();
}

//=================================================================================================

float Font_TextFormatter::FirstPosition() const
{
  switch (myAlignX)
  {
    default:
    case Graphic3d_HTA_LEFT:
      return 0;
    case Graphic3d_HTA_RIGHT:
      return myBndWidth;
    case Graphic3d_HTA_CENTER:
      return 0.5f * myBndWidth;
  }
}

//=================================================================================================

int Font_TextFormatter::LinePositionIndex(const int theIndex) const
{
  int anIndex = 0;

  float anIndexHeight = BottomLeft(theIndex).y();
  for (int aPrevIndex = theIndex - 1; aPrevIndex >= 0; aPrevIndex--)
  {
    if (BottomLeft(aPrevIndex).y() > anIndexHeight)
    {
      break;
    }
    anIndex++;
  }
  return anIndex;
}

//=================================================================================================

int Font_TextFormatter::LineIndex(const int theIndex) const
{
  if (myLineSpacing < 0.0f)
  {
    return 0;
  }

  return (int)std::abs((BottomLeft(theIndex).y() + myAscender) / myLineSpacing);
}

//=================================================================================================

float Font_TextFormatter::LineWidth(const int theIndex) const
{
  if (theIndex < 0)
  {
    return 0;
  }

  if (theIndex < myNewLines.Length())
  {
    return theIndex == 0 ? myNewLines[0] : myNewLines[theIndex] - myNewLines[theIndex - 1];
  }

  if (theIndex == myNewLines.Length()) // the last line
  {
    return theIndex == 0 ? myPen.x() : myPen.x() - myNewLines[theIndex - 1];
  }

  return 0;
}
