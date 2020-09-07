// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Quantity_ColorRGBA_HeaderFile
#define _Quantity_ColorRGBA_HeaderFile

#include <Quantity_Color.hxx>
#include <Standard_Assert.hxx>

//! The pair of Quantity_Color and Alpha component (1.0 opaque, 0.0 transparent).
class Quantity_ColorRGBA
{
public:

  //! Creates a color with the default value.
  Quantity_ColorRGBA() : myAlpha (1.0f) {}

  //! Creates the color with specified RGB value.
  explicit Quantity_ColorRGBA (const Quantity_Color& theRgb)
  : myRgb (theRgb), myAlpha (1.0f)
  {}

  //! Creates the color with specified RGBA values.
  Quantity_ColorRGBA (const Quantity_Color& theRgb, float theAlpha)
  : myRgb (theRgb), myAlpha (theAlpha)
  {}

  //! Creates the color from RGBA vector.
  explicit Quantity_ColorRGBA (const NCollection_Vec4<float>& theRgba) 
  : myRgb (theRgba.rgb()), myAlpha (theRgba.a())
  {}

  //! Creates the color from RGBA values.
  Quantity_ColorRGBA (float theRed, float theGreen, float theBlue, float theAlpha)
  : myRgb (theRed, theGreen, theBlue, Quantity_TOC_RGB),
    myAlpha (theAlpha)
  {}

  //! Assign new values to the color.
  void SetValues (float theRed, float theGreen, float theBlue, float theAlpha)
  {
    myRgb.SetValues (theRed, theGreen, theBlue, Quantity_TOC_RGB);
    myAlpha = theAlpha;
  }

  //! Return RGB color value.
  const Quantity_Color& GetRGB() const { return myRgb; }

  //! Modify RGB color components without affecting alpha value.
  Quantity_Color& ChangeRGB() { return myRgb; }

  //! Assign RGB color components without affecting alpha value.
  void SetRGB (const Quantity_Color& theRgb) { myRgb = theRgb; }

  //! Return alpha value (1.0 means opaque, 0.0 means fully transparent).
  Standard_ShortReal Alpha() const { return myAlpha; }

  //! Assign the alpha value.
  void SetAlpha (const Standard_ShortReal theAlpha) { myAlpha = theAlpha; }

  //! Return the color as vector of 4 float elements.
  operator const NCollection_Vec4<float>&() const { return *(const NCollection_Vec4<float>* )this; }

  //! Returns true if the distance between colors is greater than Epsilon().
  bool IsDifferent (const Quantity_ColorRGBA& theOther) const
  {
    return myRgb.IsDifferent (theOther.GetRGB())
        || Abs(myAlpha - theOther.myAlpha) > (float )Quantity_Color::Epsilon();
  }

  //! Returns true if the distance between colors is greater than Epsilon().
  bool operator!= (const Quantity_ColorRGBA& theOther) const { return IsDifferent (theOther); }

  //! Two colors are considered to be equal if their distance is no greater than Epsilon().
  bool IsEqual (const Quantity_ColorRGBA& theOther) const
  {
    return myRgb.IsEqual (theOther.GetRGB())
        && Abs(myAlpha - theOther.myAlpha) <= (float )Quantity_Color::Epsilon();
  }

  //! Two colors are considered to be equal if their distance is no greater than Epsilon().
  bool operator== (const Quantity_ColorRGBA& theOther) const { return IsEqual (theOther); }

public:

  //! Finds color from predefined names.
  //! For example, the name of the color which corresponds to "BLACK" is Quantity_NOC_BLACK.
  //! An alpha component is set to 1.0.
  //! @param theColorNameString the color name
  //! @param theColor a found color
  //! @return false if the color name is unknown, or true if the search by color name was successful
  static Standard_Boolean ColorFromName (const Standard_CString theColorNameString, Quantity_ColorRGBA& theColor)
  {
    Quantity_ColorRGBA aColor;
    if (!Quantity_Color::ColorFromName (theColorNameString, aColor.ChangeRGB()))
    {
      return false;
    }
    theColor = aColor;
    return true;
  }

  //! Parses the string as a hex color (like "#FF0" for short sRGB color, "#FF0F" for short sRGBA color,
  //! "#FFFF00" for RGB color, or "#FFFF00FF" for RGBA color)
  //! @param theHexColorString the string to be parsed
  //! @param theColor a color that is a result of parsing
  //! @param theAlphaComponentIsOff the flag that indicates if a color alpha component is presented
  //! in the input string (false) or not (true)
  //! @return true if parsing was successful, or false otherwise
  Standard_EXPORT static bool ColorFromHex (const char* const   theHexColorString,
                                            Quantity_ColorRGBA& theColor,
                                            const bool          theAlphaComponentIsOff = false);

  //! Returns hex sRGBA string in format "#RRGGBBAA".
  static TCollection_AsciiString ColorToHex (const Quantity_ColorRGBA& theColor,
                                             const bool theToPrefixHash = true)
  {
    NCollection_Vec4<Standard_ShortReal> anSRgb = Convert_LinearRGB_To_sRGB ((NCollection_Vec4<Standard_ShortReal> )theColor);
    NCollection_Vec4<Standard_Integer> anSRgbInt (anSRgb * 255.0f + NCollection_Vec4<Standard_ShortReal> (0.5f));
    char aBuff[12];
    Sprintf (aBuff, theToPrefixHash ? "#%02X%02X%02X%02X" : "%02X%02X%02X%02X",
             anSRgbInt.r(), anSRgbInt.g(), anSRgbInt.b(), anSRgbInt.a());
    return aBuff;
  }

public:

  //! Convert linear RGB components into sRGB using OpenGL specs formula.
  static NCollection_Vec4<float> Convert_LinearRGB_To_sRGB (const NCollection_Vec4<float>& theRGB)
  {
    return NCollection_Vec4<float> (Quantity_Color::Convert_LinearRGB_To_sRGB (theRGB.r()),
                                    Quantity_Color::Convert_LinearRGB_To_sRGB (theRGB.g()),
                                    Quantity_Color::Convert_LinearRGB_To_sRGB (theRGB.b()),
                                    theRGB.a());
  }

  //! Convert sRGB components into linear RGB using OpenGL specs formula.
  static NCollection_Vec4<float> Convert_sRGB_To_LinearRGB (const NCollection_Vec4<float>& theRGB)
  {
    return NCollection_Vec4<float> (Quantity_Color::Convert_sRGB_To_LinearRGB (theRGB.r()),
                                    Quantity_Color::Convert_sRGB_To_LinearRGB (theRGB.g()),
                                    Quantity_Color::Convert_sRGB_To_LinearRGB (theRGB.b()),
                                    theRGB.a());
  }

public:

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson (const Standard_SStream& theSStream, Standard_Integer& theStreamPos);

private:

  static void myTestSize3() { Standard_STATIC_ASSERT (sizeof(float) * 3 == sizeof(Quantity_Color)); }
  static void myTestSize4() { Standard_STATIC_ASSERT (sizeof(float) * 4 == sizeof(Quantity_ColorRGBA)); }

private:

  Quantity_Color     myRgb;
  Standard_ShortReal myAlpha;

};

#endif // _Quantity_ColorRGBA_HeaderFile
