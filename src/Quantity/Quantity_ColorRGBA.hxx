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

private:

  static void myTestSize3() { Standard_STATIC_ASSERT (sizeof(float) * 3 == sizeof(Quantity_Color)); }
  static void myTestSize4() { Standard_STATIC_ASSERT (sizeof(float) * 4 == sizeof(Quantity_ColorRGBA)); }

private:

  Quantity_Color     myRgb;
  Standard_ShortReal myAlpha;

};

#endif // _Quantity_ColorRGBA_HeaderFile
