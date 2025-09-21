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

DEFINE_STANDARD_HANDLE(Font_TextFormatter, Standard_Transient)

//! This class is intended to prepare formatted text by using:<br>
//! - font to string combination,<br>
//! - alignment,<br>
//! - wrapping.<br>
//!
//! After text formatting, each symbol of formatted text is placed in some position.
//! Further work with the formatter is using an iterator.
//! The iterator gives an access to each symbol inside the initial row.
//! Also it's possible to get only significant/writable symbols of the text.<br>
//! Formatter gives an access to geometrical position of a symbol by the symbol index in the
//! text.<br> Example of correspondence of some text symbol to an index in "row_1\n\nrow_2\n":<br>
//! "row_1\n"  - 0-5 indices;<br>
//! "\n"       - 6 index;<br>
//! "\n"       - 7 index;<br>
//! "row_2\n"  - 8-13 indices.<br>
//! Pay attention that fonts should have the same LineSpacing value for correct formatting.<br>
//! Example of the formatter using:
//! @code
//!   Handle(Font_TextFormatter) aFormatter = new Font_TextFormatter();
//!   aFormatter->Append(text_1, aFont1);
//!   aFormatter->Append(text_2, aFont2);
//!   // setting of additional properties such as wrapping or alignment
//!   aFormatter->Format();
//! @endcode
class Font_TextFormatter : public Standard_Transient
{
public:
  //! Iteration filter flags. Command symbols are skipped with any filter.
  enum IterationFilter
  {
    IterationFilter_None             = 0x0000, //!< no filter
    IterationFilter_ExcludeInvisible = 0x0002, //!< exclude ' ', '\t', '\n'
  };

  //! Iterator through formatted symbols.
  //! It's possible to filter returned symbols to have only significant ones.
  class Iterator
  {
  public:
    //! Constructor with initialization.
    Iterator(const Font_TextFormatter& theFormatter,
             IterationFilter           theFilter = IterationFilter_None)
        : myFilter(theFilter),
          myIter(theFormatter.myString.Iterator()),
          mySymbolChar(0),
          mySymbolCharNext(0)
    {
      mySymbolPosition = readNextSymbol(-1, mySymbolChar);
      mySymbolNext     = readNextSymbol(mySymbolPosition, mySymbolCharNext);
    }

    //! Returns TRUE if iterator points to a valid item.
    Standard_Boolean More() const { return mySymbolPosition >= 0; }

    //! Returns TRUE if next item exists
    Standard_Boolean HasNext() const { return mySymbolNext >= 0; }

    //! Returns current symbol.
    Standard_Utf32Char Symbol() const { return mySymbolChar; }

    //! Returns the next symbol if exists.
    Standard_Utf32Char SymbolNext() const { return mySymbolCharNext; }

    //! Returns current symbol position.
    Standard_Integer SymbolPosition() const { return mySymbolPosition; }

    //! Returns the next symbol position.
    Standard_Integer SymbolPositionNext() const { return mySymbolNext; }

    //! Moves to the next item.
    void Next()
    {
      mySymbolPosition = mySymbolNext;
      mySymbolChar     = mySymbolCharNext;
      mySymbolNext     = readNextSymbol(mySymbolPosition, mySymbolCharNext);
    }

  protected:
    //! Finds index of the next symbol
    Standard_Integer readNextSymbol(const Standard_Integer theSymbolStartingFrom,
                                    Standard_Utf32Char&    theSymbolChar)
    {
      Standard_Integer aNextSymbol = theSymbolStartingFrom;
      for (; *myIter != 0; ++myIter)
      {
        const Standard_Utf32Char aCharCurr = *myIter;
        if (Font_TextFormatter::IsCommandSymbol(aCharCurr))
        {
          continue; // skip unsupported carriage control codes
        }
        aNextSymbol++;
        if ((myFilter & IterationFilter_ExcludeInvisible) != 0)
        {
          if (aCharCurr == '\x0A' || // LF (line feed, new line)
              aCharCurr == ' ' || aCharCurr == '\t')
          {
            continue;
          }
        }
        ++myIter;
        theSymbolChar = aCharCurr;
        return aNextSymbol; // found the first next, not command and not filtered symbol
      }
      return -1; // the next symbol is not found
    }

  protected:
    IterationFilter myFilter;            //!< possibility to filter not-necessary symbols
                                         // clang-format off
    NCollection_Utf8Iter myIter; //!< the next symbol iterator value over the text formatter string
    Standard_Integer     mySymbolPosition; //!< the current position
    Standard_Utf32Char   mySymbolChar; //!< the current symbol
    Standard_Integer     mySymbolNext; //!< position of the next symbol in iterator, if zero, the iterator is finished
                                         // clang-format on
    Standard_Utf32Char mySymbolCharNext; //!< the current symbol
  };

  //! Default constructor.
  Standard_EXPORT Font_TextFormatter();

  //! Setup alignment style.
  Standard_EXPORT void SetupAlignment(const Graphic3d_HorizontalTextAlignment theAlignX,
                                      const Graphic3d_VerticalTextAlignment   theAlignY);

  //! Reset current progress.
  Standard_EXPORT void Reset();

  //! Render specified text to inner buffer.
  Standard_EXPORT void Append(const NCollection_String& theString, Font_FTFont& theFont);

  //! Perform formatting on the buffered text.
  //! Should not be called more than once after initialization!
  Standard_EXPORT void Format();

  Standard_DEPRECATED("BottomLeft should be used instead")

  const NCollection_Vec2<Standard_ShortReal>& TopLeft(const Standard_Integer theIndex) const
  {
    return BottomLeft(theIndex);
  }

  //! Returns specific glyph rectangle.
  const NCollection_Vec2<Standard_ShortReal>& BottomLeft(const Standard_Integer theIndex) const
  {
    return myCorners.Value(theIndex);
  }

  //! Returns current rendering string.
  inline const NCollection_String& String() const { return myString; }

  //! Returns symbol bounding box
  //! @param bounding box.
  Standard_EXPORT Standard_Boolean GlyphBoundingBox(const Standard_Integer theIndex,
                                                    Font_Rect&             theBndBox) const;

  //! Returns the line height
  //! @param theIndex a line index, obtained by LineIndex()
  Standard_ShortReal LineHeight(const Standard_Integer theIndex) const
  {
    return theIndex == 0 ? myAscender : myLineSpacing;
  }

  //! Returns width of a line
  Standard_EXPORT Standard_ShortReal LineWidth(const Standard_Integer theIndex) const;

  //! Returns true if the symbol by the index is '\n'. The width of the symbol is zero.
  Standard_EXPORT Standard_Boolean IsLFSymbol(const Standard_Integer theIndex) const;

  //! Returns position of the first symbol in a line using alignment
  Standard_EXPORT Standard_ShortReal FirstPosition() const;

  //! Returns column index of the corner index in the current line
  Standard_EXPORT Standard_Integer LinePositionIndex(const Standard_Integer theIndex) const;

  //! Returns row index of the corner index among text lines
  Standard_EXPORT Standard_Integer LineIndex(const Standard_Integer theIndex) const;

  //! Returns tab size.
  inline Standard_Integer TabSize() const { return myTabSize; }

  //! Returns horizontal alignment style
  Graphic3d_HorizontalTextAlignment HorizontalTextAlignment() const { return myAlignX; }

  //! Returns vertical   alignment style
  Graphic3d_VerticalTextAlignment VerticalTextAlignment() const { return myAlignY; }

  //! Sets text wrapping width, zero means that the text is not bounded by width
  void SetWrapping(const Standard_ShortReal theWidth) { myWrappingWidth = theWidth; }

  //! Returns text maximum width, zero means that the text is not bounded by width
  Standard_Boolean HasWrapping() const { return myWrappingWidth > 0; }

  //! Returns text maximum width, zero means that the text is not bounded by width
  Standard_ShortReal Wrapping() const { return myWrappingWidth; }

  //! returns TRUE when trying not to break words when wrapping text
  Standard_Boolean WordWrapping() const { return myIsWordWrapping; }

  //! returns TRUE when trying not to break words when wrapping text
  void SetWordWrapping(const Standard_Boolean theIsWordWrapping)
  {
    myIsWordWrapping = theIsWordWrapping;
  }

  //! @return width of formatted text.
  inline Standard_ShortReal ResultWidth() const { return myBndWidth; }

  //! @return height of formatted text.
  inline Standard_ShortReal ResultHeight() const
  {
    return myLineSpacing * Standard_ShortReal(myLinesNb);
  }

  //! @return maximum width of the text symbol
  Standard_ShortReal MaximumSymbolWidth() const { return myMaxSymbolWidth; }

  //! @param bounding box.
  inline void BndBox(Font_Rect& theBndBox) const
  {
    theBndBox.Left = 0.0f;
    switch (myAlignX)
    {
      default:
      case Graphic3d_HTA_LEFT:
        theBndBox.Right = myBndWidth;
        break;
      case Graphic3d_HTA_RIGHT:
        theBndBox.Right = -myBndWidth;
        break;
      case Graphic3d_HTA_CENTER: {
        theBndBox.Left  = -0.5f * myBndWidth;
        theBndBox.Right = 0.5f * myBndWidth;
        break;
      }
    }
    theBndBox.Top    = myBndTop;
    theBndBox.Bottom = theBndBox.Top - myLineSpacing * Standard_ShortReal(myLinesNb);
  }

  //! Returns internal container of the top left corners of a formatted rectangles.
  const NCollection_Vector<NCollection_Vec2<Standard_ShortReal>>& Corners() const
  {
    return myCorners;
  }

  //! Returns container of each line position at LF in formatted text
  const NCollection_Vector<Standard_ShortReal>& NewLines() const { return myNewLines; }

  //! Returns true if the symbol is CR, BEL, FF, NP, BS or VT
  static inline Standard_Boolean IsCommandSymbol(const Standard_Utf32Char& theSymbol)
  {
    if (theSymbol == '\x0D'   // CR  (carriage return)
        || theSymbol == '\a'  // BEL (alarm)
        || theSymbol == '\f'  // FF  (form feed) NP (new page)
        || theSymbol == '\b'  // BS  (backspace)
        || theSymbol == '\v') // VT  (vertical tab)
      return Standard_True;

    return Standard_False;
  }

  //! Returns true if the symbol separates words when wrapping is enabled
  static Standard_Boolean IsSeparatorSymbol(const Standard_Utf32Char& theSymbol)
  {
    return theSymbol == '\x0A'     // new line
           || theSymbol == ' '     // space
           || theSymbol == '\x09'; // tab
  }

  DEFINE_STANDARD_RTTIEXT(Font_TextFormatter, Standard_Transient)

protected: //! @name class auxiliary methods
  //! Move glyphs on the current line to correct position.
  Standard_EXPORT void newLine(const Standard_Integer   theLastRect,
                               const Standard_ShortReal theMaxLineWidth);

protected:                                    //! @name configuration
  Graphic3d_HorizontalTextAlignment myAlignX; //!< horizontal alignment style
  Graphic3d_VerticalTextAlignment   myAlignY; //!< vertical   alignment style
  // clang-format off
  Standard_Integer                  myTabSize; //!< horizontal tabulation width (number of space symbols)
  Standard_ShortReal                myWrappingWidth; //!< text is wrapped by the width if defined (more 0)
  Standard_Boolean                  myIsWordWrapping;  //!< if TRUE try not to break words when wrapping text (true by default)
  Standard_ShortReal                myLastSymbolWidth; //!< width of the last symbol
  Standard_ShortReal                myMaxSymbolWidth; //!< maximum symbol width of the formatter string
  // clang-format on

protected:                                       //! @name input data
  NCollection_String                   myString; //!< currently rendered text
  NCollection_Vec2<Standard_ShortReal> myPen;    //!< current pen position
  NCollection_Vector<NCollection_Vec2<Standard_ShortReal>>
    myCorners; //!< The bottom left corners of a formatted rectangles.
  NCollection_Vector<Standard_ShortReal> myNewLines; //!< position at LF
                                                     // clang-format off
  Standard_ShortReal myLineSpacing;   //!< line spacing (computed as maximum of all fonts involved in text formatting)
  Standard_ShortReal myAscender;      //!< line spacing for the first line
  bool               myIsFormatted;   //!< formatting state

protected: //! @name temporary variables for formatting routines

  Standard_Integer   myLinesNb;       //!< overall (new)lines number (including splitting by width limit)
                                                     // clang-format on
  Standard_Integer myRectLineStart;                  //!< id of first rectangle on the current line
  Standard_Integer myNewLineNb;

  Standard_ShortReal                   myPenCurrLine; //!< current baseline position
  Standard_ShortReal                   myBndTop;
  Standard_ShortReal                   myBndWidth;
  NCollection_Vec2<Standard_ShortReal> myMoveVec; //!< local variable
};

#endif // Font_TextFormatter_Header
