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

//! This class is intended to prepare formatted text by using:
//! - font to string combination,
//! - alignment,
//! - wrapping.
//!
//! After text formatting, each symbol of formatted text is placed in some position.
//! Further work with the formatter is using an iterator.
//! The iterator gives an access to each symbol inside the initial row.
//! Also it's possible to get only significant/writable symbols of the text.
//! Formatter gives an access to geometrical position of a symbol by the symbol index in the
//! text. Example of correspondence of some text symbol to an index in "row_1\n\nrow_2\n":
//! "row_1\n"  - 0-5 indices;
//! "\n"       - 6 index;
//! "\n"       - 7 index;
//! "row_2\n"  - 8-13 indices.
//! Pay attention that fonts should have the same LineSpacing value for correct formatting.
//! Example of the formatter using:
//! @code
//!   occ::handle<Font_TextFormatter> aFormatter = new Font_TextFormatter();
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
    bool More() const { return mySymbolPosition >= 0; }

    //! Returns TRUE if next item exists
    bool HasNext() const { return mySymbolNext >= 0; }

    //! Returns current symbol.
    char32_t Symbol() const { return mySymbolChar; }

    //! Returns the next symbol if exists.
    char32_t SymbolNext() const { return mySymbolCharNext; }

    //! Returns current symbol position.
    int SymbolPosition() const { return mySymbolPosition; }

    //! Returns the next symbol position.
    int SymbolPositionNext() const { return mySymbolNext; }

    //! Moves to the next item.
    void Next()
    {
      mySymbolPosition = mySymbolNext;
      mySymbolChar     = mySymbolCharNext;
      mySymbolNext     = readNextSymbol(mySymbolPosition, mySymbolCharNext);
    }

  protected:
    //! Finds index of the next symbol
    int readNextSymbol(const int theSymbolStartingFrom, char32_t& theSymbolChar)
    {
      int aNextSymbol = theSymbolStartingFrom;
      for (; *myIter != 0; ++myIter)
      {
        const char32_t aCharCurr = *myIter;
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
    IterationFilter myFilter;  //!< possibility to filter not-necessary symbols
                               // clang-format off
    NCollection_UtfIterator<char> myIter; //!< the next symbol iterator value over the text formatter string
    int     mySymbolPosition; //!< the current position
    char32_t   mySymbolChar; //!< the current symbol
    int     mySymbolNext; //!< position of the next symbol in iterator, if zero, the iterator is finished
                               // clang-format on
    char32_t mySymbolCharNext; //!< the current symbol
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
  const NCollection_Vec2<float>& TopLeft(const int theIndex) const { return BottomLeft(theIndex); }

  //! Returns specific glyph rectangle.
  const NCollection_Vec2<float>& BottomLeft(const int theIndex) const
  {
    return myCorners.Value(theIndex);
  }

  //! Returns current rendering string.
  inline const NCollection_String& String() const { return myString; }

  //! Returns symbol bounding box
  //! @param bounding box.
  Standard_EXPORT bool GlyphBoundingBox(const int theIndex, Font_Rect& theBndBox) const;

  //! Returns the line height
  //! @param theIndex a line index, obtained by LineIndex()
  float LineHeight(const int theIndex) const { return theIndex == 0 ? myAscender : myLineSpacing; }

  //! Returns width of a line
  Standard_EXPORT float LineWidth(const int theIndex) const;

  //! Returns true if the symbol by the index is '\n'. The width of the symbol is zero.
  Standard_EXPORT bool IsLFSymbol(const int theIndex) const;

  //! Returns position of the first symbol in a line using alignment
  Standard_EXPORT float FirstPosition() const;

  //! Returns column index of the corner index in the current line
  Standard_EXPORT int LinePositionIndex(const int theIndex) const;

  //! Returns row index of the corner index among text lines
  Standard_EXPORT int LineIndex(const int theIndex) const;

  //! Returns tab size.
  inline int TabSize() const { return myTabSize; }

  //! Returns horizontal alignment style
  Graphic3d_HorizontalTextAlignment HorizontalTextAlignment() const { return myAlignX; }

  //! Returns vertical alignment style
  Graphic3d_VerticalTextAlignment VerticalTextAlignment() const { return myAlignY; }

  //! Sets text wrapping width, zero means that the text is not bounded by width
  void SetWrapping(const float theWidth) { myWrappingWidth = theWidth; }

  //! Returns text maximum width, zero means that the text is not bounded by width
  bool HasWrapping() const { return myWrappingWidth > 0; }

  //! Returns text maximum width, zero means that the text is not bounded by width
  float Wrapping() const { return myWrappingWidth; }

  //! returns TRUE when trying not to break words when wrapping text
  bool WordWrapping() const { return myIsWordWrapping; }

  //! returns TRUE when trying not to break words when wrapping text
  void SetWordWrapping(const bool theIsWordWrapping) { myIsWordWrapping = theIsWordWrapping; }

  //! @return width of formatted text.
  inline float ResultWidth() const { return myBndWidth; }

  //! @return height of formatted text.
  inline float ResultHeight() const { return myLineSpacing * float(myLinesNb); }

  //! @return maximum width of the text symbol
  float MaximumSymbolWidth() const { return myMaxSymbolWidth; }

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
    theBndBox.Bottom = theBndBox.Top - myLineSpacing * float(myLinesNb);
  }

  //! Returns internal container of the top left corners of a formatted rectangles.
  const NCollection_Vector<NCollection_Vec2<float>>& Corners() const { return myCorners; }

  //! Returns container of each line position at LF in formatted text
  const NCollection_Vector<float>& NewLines() const { return myNewLines; }

  //! Returns true if the symbol is CR, BEL, FF, NP, BS or VT
  static inline bool IsCommandSymbol(const char32_t& theSymbol)
  {
    if (theSymbol == '\x0D'   // CR  (carriage return)
        || theSymbol == '\a'  // BEL (alarm)
        || theSymbol == '\f'  // FF  (form feed) NP (new page)
        || theSymbol == '\b'  // BS  (backspace)
        || theSymbol == '\v') // VT  (vertical tab)
      return true;

    return false;
  }

  //! Returns true if the symbol separates words when wrapping is enabled
  static bool IsSeparatorSymbol(const char32_t& theSymbol)
  {
    return theSymbol == '\x0A'     // new line
           || theSymbol == ' '     // space
           || theSymbol == '\x09'; // tab
  }

  DEFINE_STANDARD_RTTIEXT(Font_TextFormatter, Standard_Transient)

protected: //! @name class auxiliary methods
  //! Move glyphs on the current line to correct position.
  Standard_EXPORT void newLine(const int theLastRect, const float theMaxLineWidth);

protected:                                    //! @name configuration
  Graphic3d_HorizontalTextAlignment myAlignX; //!< horizontal alignment style
  Graphic3d_VerticalTextAlignment   myAlignY; //!< vertical   alignment style
  // clang-format off
  int                  myTabSize; //!< horizontal tabulation width (number of space symbols)
  float                myWrappingWidth; //!< text is wrapped by the width if defined (more 0)
  bool                  myIsWordWrapping;  //!< if TRUE try not to break words when wrapping text (true by default)
  float                myLastSymbolWidth; //!< width of the last symbol
  float                myMaxSymbolWidth; //!< maximum symbol width of the formatter string
  // clang-format on

protected:                          //! @name input data
  NCollection_String      myString; //!< currently rendered text
  NCollection_Vec2<float> myPen;    //!< current pen position
  NCollection_Vector<NCollection_Vec2<float>>
                            myCorners;  //!< The bottom left corners of a formatted rectangles.
  NCollection_Vector<float> myNewLines; //!< position at LF
                                        // clang-format off
  float myLineSpacing;   //!< line spacing (computed as maximum of all fonts involved in text formatting)
  float myAscender;      //!< line spacing for the first line
  bool               myIsFormatted;   //!< formatting state

protected: //! @name temporary variables for formatting routines

  int   myLinesNb;       //!< overall (new)lines number (including splitting by width limit)
                                        // clang-format on
  int myRectLineStart;                  //!< id of first rectangle on the current line
  int myNewLineNb;

  float                   myPenCurrLine; //!< current baseline position
  float                   myBndTop;
  float                   myBndWidth;
  NCollection_Vec2<float> myMoveVec; //!< local variable
};

#endif // Font_TextFormatter_Header
