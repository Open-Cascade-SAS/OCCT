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

#ifndef Font_TextFormatter_Header
#define Font_TextFormatter_Header

#include <Font_Rect.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_String.hxx>

class Font_FTFont;

//! This class intended to prepare formatted text.
class Font_TextFormatter
{
public:

  //! Default constructor.
  Standard_EXPORT Font_TextFormatter();

  //! Setup alignment style.
  Standard_EXPORT void SetupAlignment (const Graphic3d_HorizontalTextAlignment theAlignX,
                                       const Graphic3d_VerticalTextAlignment   theAlignY);

  //! Reset current progress.
  Standard_EXPORT void Reset();

  //! Render specified text to inner buffer.
  Standard_EXPORT void Append (const NCollection_String& theString,
                               Font_FTFont&              theFont);

  //! Perform formatting on the buffered text.
  //! Should not be called more than once after initialization!
  Standard_EXPORT void Format();

  //! Returns specific glyph rectangle.
  inline const NCollection_Vec2<Standard_ShortReal>& TopLeft (const Standard_Integer theIndex) const
  {
    return myCorners.Value (theIndex);
  }

  //! Returns current rendering string.
  inline const NCollection_String& String() const
  {
    return myString;
  }

  //! Returns tab size.
  inline Standard_Integer TabSize() const
  {
    return myTabSize;
  }

  //! @return width of formatted text.
  inline Standard_ShortReal ResultWidth() const
  {
    return myBndWidth;
  }

  //! @return height of formatted text.
  inline Standard_ShortReal ResultHeight() const
  {
    return myLineSpacing * Standard_ShortReal(myLinesNb);
  }

  //! @param bounding box.
  inline void BndBox (Font_Rect& theBndBox) const
  {
    theBndBox.Left = 0.0f;
    switch (myAlignX)
    {
      default:
      case Graphic3d_HTA_LEFT:  theBndBox.Right  =  myBndWidth; break;
      case Graphic3d_HTA_RIGHT: theBndBox.Right  = -myBndWidth; break;
      case Graphic3d_HTA_CENTER:
      {
        theBndBox.Left  = -0.5f * myBndWidth;
        theBndBox.Right =  0.5f * myBndWidth;
        break;
      }
    }
    theBndBox.Top    = myBndTop;
    theBndBox.Bottom = theBndBox.Top - myLineSpacing * Standard_ShortReal(myLinesNb);
  }

protected: //! @name class auxiliary methods

  //! Move glyphs on the current line to correct position.
  Standard_EXPORT void newLine (const Standard_Integer theLastRect);

protected: //! @name configuration

  Graphic3d_HorizontalTextAlignment myAlignX;  //!< horizontal alignment style
  Graphic3d_VerticalTextAlignment   myAlignY;  //!< vertical   alignment style
  Standard_Integer                  myTabSize; //!< horizontal tabulation width (number of space symbols)

protected: //! @name input data

  NCollection_String myString;        //!< currently rendered text
  NCollection_Vec2<Standard_ShortReal>
                     myPen;           //!< current pen position
  NCollection_Vector < NCollection_Vec2<Standard_ShortReal> >
                     myCorners;       //!< The top left corners of a formatted rectangles.
  Standard_Integer   myRectsNb;       //!< rectangles number
  NCollection_Vector<Standard_ShortReal>
                     myNewLines;      //!< position at LF
  Standard_ShortReal myLineSpacing;   //!< line spacing (computed as maximum of all fonts involved in text formatting)
  Standard_ShortReal myAscender;      //!<
  bool               myIsFormatted;   //!< formatting state

protected: //! @name temporary variables for formatting routines

  Standard_Integer   myLinesNb;       //!< overall (new)lines number (including splitting by width limit)
  Standard_Integer   myRectLineStart; //!< id of first rectangle on the current line
  Standard_Integer   myRectWordStart; //!< id of first rectangle in the current word
  Standard_Integer   myNewLineNb;

  Standard_ShortReal myPenCurrLine;   //!< current baseline position
  Standard_ShortReal myBndTop;
  Standard_ShortReal myBndWidth;
  NCollection_Vec2<Standard_ShortReal>
                     myMoveVec;       //!< local variable
};

#endif // Font_TextFormatter_Header
