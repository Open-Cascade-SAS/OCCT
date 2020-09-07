// Created on: 2019-03-22
// Created by: Timur Izmaylov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <Quantity_ColorRGBA.hxx>

#include <NCollection_Vec4.hxx>
#include <Standard_Dump.hxx>

#include <algorithm>

namespace
{

  //! The integer type used to represent some color or color component
  typedef unsigned int ColorInteger;

  //! Defines all possible lengths of strings representing color in hex format
  enum HexColorLength
  {
    HexColorLength_ShortRGB  = 3, //!< short RGB hex color format
    HexColorLength_ShortRGBA = 4, //!< short RGBA hex color format
    HexColorLength_RGB       = 6, //!< RGB hex color format
    HexColorLength_RGBA      = 8  //!< RGBA hex color format
  };

  //! Takes next color component from the integer representing a color (it is a step in a process of a conversion
  //! implemented by the function ConvertIntegerToColorRGBA)
  //! @param theColorInteger the integer representing a color
  //! @param theColorComponentBase the base of the numeral system used to represent a color
  //! @return a color component taken from the integer
  static Standard_ShortReal takeColorComponentFromInteger (ColorInteger&      theColorInteger,
                                                           const ColorInteger theColorComponentBase)
  {
    Standard_ASSERT_RETURN (theColorComponentBase >= 2,
                            "'theColorComponentBase' must be greater than 1.",
                            0.0f);
    const ColorInteger       aColorComponentMaxValue  = theColorComponentBase - 1;
    const ColorInteger       aColorComponentAsInteger = theColorInteger % theColorComponentBase;
    const Standard_ShortReal aColorComponent          = aColorComponentAsInteger * 1.0f / aColorComponentMaxValue;
    theColorInteger /= theColorComponentBase;
    return aColorComponent;
  }

  //! Converts the integer representing a color to a RGBA color object
  //! @param theColorInteger the integer representing a color (using the numerical system based
  //! on theColorComponentBase value, where color components represent digits:
  //! an alpha component is a low number and a red component is a high number)
  //! @param theColorComponentBase the base of the numeral system used to represent a color
  //! @param hasAlphaComponent true if the integer to be converted contains an alpha component value
  //! @param theColor a color that is a result of a conversion
  //! @return true if a conversion was successful, or false otherwise
  static bool convertIntegerToColorRGBA (ColorInteger        theColorInteger,
                                         const ColorInteger  theColorComponentBase,
                                         const bool          hasAlphaComponent,
                                         Quantity_ColorRGBA& theColor)
  {
    Standard_ASSERT_RETURN (theColorComponentBase >= 2,
                            "'theColorComponentBase' must be greater than 1.",
                            0.0f);
    NCollection_Vec4<float> aColor (1.0f);
    if (hasAlphaComponent)
    {
      const Standard_ShortReal anAlphaComponent = takeColorComponentFromInteger (theColorInteger,
                                                                                 theColorComponentBase);
      aColor.a()                                = anAlphaComponent;
    }
    for (Standard_Integer aColorComponentIndex = 2; aColorComponentIndex >= 0; --aColorComponentIndex)
    {
      const Standard_ShortReal aColorComponent = takeColorComponentFromInteger (theColorInteger, theColorComponentBase);
      aColor[aColorComponentIndex]             = Quantity_Color::Convert_sRGB_To_LinearRGB (aColorComponent);
    }
    if (theColorInteger != 0)
    {
      return false;
    }
    theColor = Quantity_ColorRGBA (aColor);
    return true;
  }

  //! Converts the string to an integer number using the number base
  //! @tparam TheNumber the type of a resulting number
  //! @param theString the string to be converted
  //! @param theNumber a number that is the result of the conversion
  //! @param theBase the base of a numeral system used to represent a number in a string form
  //! @return true if a conversion was successful, or false otherwise
  template <typename TheNumber>
  static bool convertStringToInteger (const char* const theString, TheNumber& theNumber, const TheNumber theBase = 10)
  {
    std::stringstream aConversionStringStream;
    aConversionStringStream << std::setbase (theBase) << theString;
    if (aConversionStringStream.fail())
    {
      return false;
    }
    aConversionStringStream >> theNumber;
    if (aConversionStringStream.fail())
    {
      return false;
    }
    return true;
  }

  //! Checks if the character is a hexadecimal digit (0 .. 9, a .. f, A .. F)
  //! @param theCharacter the character to be checked
  //! @return true if the checking character is a hexadecimal digit, or false otherwise
  static bool isHexDigit (const char theCharacter)
  {
    return std::isxdigit (static_cast<unsigned char> (theCharacter)) != 0;
  }

  //! Checks if the string consists only of hexadecimal digits (0 .. 9, a .. f, A .. F)
  //! @param theString the string to be checked
  //! @param theLength the length of the checked string
  //! @return true if the checking string consists only of hexadecimal digits, or false otherwise
  //! an empty string is not interpreted as a hex string
  static bool isHexString (const char* const theString, const std::size_t theLength)
  {
    if (theLength == 0)
    {
      return false;
    }
    // std::all_of is not used due to VS2008 compilability limitation
    return std::count_if (theString, theString + theLength, isHexDigit) == static_cast<std::ptrdiff_t> (theLength);
  }

} // namespace

//=======================================================================
// function : ColorFromHex
// purpose  :
//=======================================================================
bool Quantity_ColorRGBA::ColorFromHex (const char* const   theHexColorString,
                                       Quantity_ColorRGBA& theColor,
                                       const bool          theAlphaComponentIsOff)
{
  std::size_t aHexColorStringLength = std::strlen (theHexColorString);
  if (aHexColorStringLength == 0)
  {
    return false;
  }

  const bool        hasPrefix       = (theHexColorString[0] == '#');
  const std::size_t aPrefixLength   = hasPrefix ? 1 : 0;
  const char* const aHexColorString = theHexColorString + aPrefixLength;
  aHexColorStringLength -= aPrefixLength;
  if (!isHexString (aHexColorString, aHexColorStringLength))
  {
    return false;
  }

  ColorInteger aHexColorInteger;
  if (!convertStringToInteger (aHexColorString, aHexColorInteger, 16u))
  {
    return false;
  }

  bool hasAlphaComponent = false;
  bool isShort           = false;
  switch (static_cast<HexColorLength> (aHexColorStringLength))
  {
    case HexColorLength_ShortRGBA:
      hasAlphaComponent = true;
      Standard_FALLTHROUGH
    case HexColorLength_ShortRGB:
      isShort = true;
      break;
    case HexColorLength_RGBA:
      hasAlphaComponent = true;
      break;
    case HexColorLength_RGB:
      break;
    default:
      return false;
  }
  if (theAlphaComponentIsOff && hasAlphaComponent)
  {
    return false;
  }
  // to distinguish with a usual integer color component value
  if (isShort && !hasAlphaComponent && !hasPrefix)
  {
    return false;
  }

  const ColorInteger THE_HEX_COLOR_COMPONENT_BASE       = 1 << 8;
  const ColorInteger THE_HEX_COLOR_COMPONENT_SHORT_BASE = 1 << 4;
  const ColorInteger aColorComponentBase = isShort ? THE_HEX_COLOR_COMPONENT_SHORT_BASE : THE_HEX_COLOR_COMPONENT_BASE;
  return convertIntegerToColorRGBA (aHexColorInteger, aColorComponentBase, hasAlphaComponent, theColor);
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Quantity_ColorRGBA::DumpJson (Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_FIELD_VALUES_NUMERICAL (theOStream, "RGBA", 4, myRgb.Red(), myRgb.Green(), myRgb.Blue(), myAlpha)
}

//=======================================================================
//function : InitFromJson
//purpose  : 
//=======================================================================
Standard_Boolean Quantity_ColorRGBA::InitFromJson (const Standard_SStream& theSStream, Standard_Integer& theStreamPos)
{
  Standard_Integer aPos = theStreamPos;

  Standard_Real aRed, aGreen, aBlue, anAlpha;
  OCCT_INIT_VECTOR_CLASS (Standard_Dump::Text (theSStream), "RGBA", aPos, 4, &aRed, &aGreen, &aBlue, &anAlpha)

  SetValues ((Standard_ShortReal)aRed, (Standard_ShortReal)aGreen, (Standard_ShortReal)aBlue, (Standard_ShortReal)anAlpha);
  return Standard_True;
}
