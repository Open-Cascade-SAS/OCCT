// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Quantity_Color_HeaderFile
#define _Quantity_Color_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_HashUtils.hxx>
#include <Standard_ShortReal.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <NCollection_Vec4.hxx>

//! This class allows the definition of an RGB color as triplet of 3 normalized floating point
//! values (red, green, blue).
//!
//! Although Quantity_Color can be technically used for pass-through storage of RGB triplet in any
//! color space, other OCCT interfaces taking/returning Quantity_Color would expect them in linear
//! space. Therefore, take a look into methods converting to and from non-linear sRGB color space,
//! if needed; for instance, application usually providing color picking within 0..255 range in sRGB
//! color space.
class Quantity_Color
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates Quantity_NOC_YELLOW color (for historical reasons).
  Quantity_Color()
      : myRgb(valuesOf(Quantity_NOC_YELLOW, Quantity_TOC_RGB))
  {
  }

  //! Creates the color from enumeration value.
  Quantity_Color(const Quantity_NameOfColor theName)
      : myRgb(valuesOf(theName, Quantity_TOC_RGB))
  {
  }

  //! Creates a color according to the definition system theType.
  //! Throws exception if values are out of range.
  Standard_EXPORT Quantity_Color(const Standard_Real        theC1,
                                 const Standard_Real        theC2,
                                 const Standard_Real        theC3,
                                 const Quantity_TypeOfColor theType);

  //! Define color from linear RGB values.
  Standard_EXPORT explicit Quantity_Color(const NCollection_Vec3<float>& theRgb);

  //! Returns the name of the nearest color from the Quantity_NameOfColor enumeration.
  Standard_EXPORT Quantity_NameOfColor Name() const;

  //! Updates the color from specified named color.
  void SetValues(const Quantity_NameOfColor theName)
  {
    myRgb = valuesOf(theName, Quantity_TOC_RGB);
  }

  //! Return the color as vector of 3 float elements.
  const NCollection_Vec3<float>& Rgb() const { return myRgb; }

  //! Return the color as vector of 3 float elements.
  operator const NCollection_Vec3<float>&() const { return myRgb; }

  //! Returns in theC1, theC2 and theC3 the components of this color
  //! according to the color system definition theType.
  Standard_EXPORT void Values(Standard_Real&             theC1,
                              Standard_Real&             theC2,
                              Standard_Real&             theC3,
                              const Quantity_TypeOfColor theType) const;

  //! Updates a color according to the mode specified by theType.
  //! Throws exception if values are out of range.
  Standard_EXPORT void SetValues(const Standard_Real        theC1,
                                 const Standard_Real        theC2,
                                 const Standard_Real        theC3,
                                 const Quantity_TypeOfColor theType);

  //! Returns the Red component (quantity of red) of the color within range [0.0; 1.0].
  Standard_Real Red() const { return myRgb.r(); }

  //! Returns the Green component (quantity of green) of the color within range [0.0; 1.0].
  Standard_Real Green() const { return myRgb.g(); }

  //! Returns the Blue component (quantity of blue) of the color within range [0.0; 1.0].
  Standard_Real Blue() const { return myRgb.b(); }

  //! Returns the Hue component (hue angle) of the color
  //! in degrees within range [0.0; 360.0], 0.0 being Red.
  //! -1.0 is a special value reserved for grayscale color (S should be 0.0)
  Standard_Real Hue() const { return Convert_LinearRGB_To_HLS(myRgb)[0]; }

  //! Returns the Light component (value of the lightness) of the color within range [0.0; 1.0].
  Standard_Real Light() const { return Convert_LinearRGB_To_HLS(myRgb)[1]; }

  //! Increases or decreases the intensity (variation of the lightness).
  //! The delta is a percentage. Any value greater than zero will increase the intensity.
  //! The variation is expressed as a percentage of the current value.
  Standard_EXPORT void ChangeIntensity(const Standard_Real theDelta);

  //! Returns the Saturation component (value of the saturation) of the color within range
  //! [0.0; 1.0].
  Standard_Real Saturation() const { return Convert_LinearRGB_To_HLS(myRgb)[2]; }

  //! Increases or decreases the contrast (variation of the saturation).
  //! The delta is a percentage. Any value greater than zero will increase the contrast.
  //! The variation is expressed as a percentage of the current value.
  Standard_EXPORT void ChangeContrast(const Standard_Real theDelta);

  //! Returns TRUE if the distance between two colors is greater than Epsilon().
  Standard_Boolean IsDifferent(const Quantity_Color& theOther) const
  {
    return (SquareDistance(theOther) > Epsilon() * Epsilon());
  }

  //! Alias to IsDifferent().
  Standard_Boolean operator!=(const Quantity_Color& theOther) const
  {
    return IsDifferent(theOther);
  }

  //! Returns TRUE if the distance between two colors is no greater than Epsilon().
  Standard_Boolean IsEqual(const Quantity_Color& theOther) const
  {
    return (SquareDistance(theOther) <= Epsilon() * Epsilon());
  }

  //! Alias to IsEqual().
  Standard_Boolean operator==(const Quantity_Color& theOther) const { return IsEqual(theOther); }

  //! Returns the distance between two colors. It's a value between 0 and the square root of 3 (the
  //! black/white distance).
  Standard_Real Distance(const Quantity_Color& theColor) const
  {
    return (NCollection_Vec3<Standard_Real>(myRgb)
            - NCollection_Vec3<Standard_Real>(theColor.myRgb))
      .Modulus();
  }

  //! Returns the square of distance between two colors.
  Standard_Real SquareDistance(const Quantity_Color& theColor) const
  {
    return (NCollection_Vec3<Standard_Real>(myRgb)
            - NCollection_Vec3<Standard_Real>(theColor.myRgb))
      .SquareModulus();
  }

  //! Returns the percentage change of contrast and intensity between this and another color.
  //! <DC> and <DI> are percentages, either positive or negative.
  //! The calculation is with respect to this color.
  //! If <DC> is positive then <me> is more contrasty.
  //! If <DI> is positive then <me> is more intense.
  Standard_EXPORT void Delta(const Quantity_Color& theColor,
                             Standard_Real&        DC,
                             Standard_Real&        DI) const;

  //! Returns the value of the perceptual difference between this color
  //! and @p theOther, computed using the CIEDE2000 formula.
  //! The difference is in range [0, 100.], with 1 approximately corresponding
  //! to the minimal perceivable difference (usually difference 5 or greater is
  //! needed for the difference to be recognizable in practice).
  Standard_EXPORT Standard_Real DeltaE2000(const Quantity_Color& theOther) const;

public:
  //! Returns the color from Quantity_NameOfColor enumeration nearest to specified RGB values.
  static Quantity_NameOfColor Name(const Standard_Real theR,
                                   const Standard_Real theG,
                                   const Standard_Real theB)
  {
    const Quantity_Color aColor(theR, theG, theB, Quantity_TOC_RGB);
    return aColor.Name();
  }

  //! Returns the name of the color identified by the given Quantity_NameOfColor enumeration value.
  Standard_EXPORT static Standard_CString StringName(const Quantity_NameOfColor theColor);

  //! Finds color from predefined names.
  //! For example, the name of the color which corresponds to "BLACK" is Quantity_NOC_BLACK.
  //! Returns FALSE if name is unknown.
  Standard_EXPORT static Standard_Boolean ColorFromName(const Standard_CString theName,
                                                        Quantity_NameOfColor&  theColor);

  //! Finds color from predefined names.
  //! @param theColorNameString the color name
  //! @param theColor a found color
  //! @return false if the color name is unknown, or true if the search by color name was successful
  static Standard_Boolean ColorFromName(const Standard_CString theColorNameString,
                                        Quantity_Color&        theColor)
  {
    Quantity_NameOfColor aColorName = Quantity_NOC_BLACK;
    if (!ColorFromName(theColorNameString, aColorName))
    {
      return false;
    }
    theColor = aColorName;
    return true;
  }

public:
  //!@name Routines converting colors between different encodings and color spaces

  //! Parses the string as a hex color (like "#FF0" for short sRGB color, or "#FFFF00" for sRGB
  //! color)
  //! @param theHexColorString the string to be parsed
  //! @param theColor a color that is a result of parsing
  //! @return true if parsing was successful, or false otherwise
  Standard_EXPORT static bool ColorFromHex(const Standard_CString theHexColorString,
                                           Quantity_Color&        theColor);

  //! Returns hex sRGB string in format "#FFAAFF".
  static TCollection_AsciiString ColorToHex(const Quantity_Color& theColor,
                                            const bool            theToPrefixHash = true)
  {
    NCollection_Vec3<Standard_ShortReal> anSRgb =
      Convert_LinearRGB_To_sRGB((NCollection_Vec3<Standard_ShortReal>)theColor);
    NCollection_Vec3<Standard_Integer> anSRgbInt(anSRgb * 255.0f
                                                 + NCollection_Vec3<Standard_ShortReal>(0.5f));
    char                               aBuff[10];
    Sprintf(aBuff,
            theToPrefixHash ? "#%02X%02X%02X" : "%02X%02X%02X",
            anSRgbInt.r(),
            anSRgbInt.g(),
            anSRgbInt.b());
    return aBuff;
  }

  //! Converts sRGB components into HLS ones.
  Standard_EXPORT static NCollection_Vec3<float> Convert_sRGB_To_HLS(
    const NCollection_Vec3<float>& theRgb);

  //! Converts HLS components into RGB ones.
  Standard_EXPORT static NCollection_Vec3<float> Convert_HLS_To_sRGB(
    const NCollection_Vec3<float>& theHls);

  //! Converts Linear RGB components into HLS ones.
  static NCollection_Vec3<float> Convert_LinearRGB_To_HLS(const NCollection_Vec3<float>& theRgb)
  {
    return Convert_sRGB_To_HLS(Convert_LinearRGB_To_sRGB(theRgb));
  }

  //! Converts HLS components into linear RGB ones.
  static NCollection_Vec3<float> Convert_HLS_To_LinearRGB(const NCollection_Vec3<float>& theHls)
  {
    return Convert_sRGB_To_LinearRGB(Convert_HLS_To_sRGB(theHls));
  }

  //! Converts linear RGB components into CIE Lab ones.
  Standard_EXPORT static NCollection_Vec3<float> Convert_LinearRGB_To_Lab(
    const NCollection_Vec3<float>& theRgb);

  //! Converts CIE Lab components into CIE Lch ones.
  Standard_EXPORT static NCollection_Vec3<float> Convert_Lab_To_Lch(
    const NCollection_Vec3<float>& theLab);

  //! Converts CIE Lab components into linear RGB ones.
  //! Note that the resulting values may be out of the valid range for RGB.
  Standard_EXPORT static NCollection_Vec3<float> Convert_Lab_To_LinearRGB(
    const NCollection_Vec3<float>& theLab);

  //! Converts CIE Lch components into CIE Lab ones.
  Standard_EXPORT static NCollection_Vec3<float> Convert_Lch_To_Lab(
    const NCollection_Vec3<float>& theLch);

  //! Convert the color value to ARGB integer value, with alpha equals to 0.
  //! So the output is formatted as 0x00RRGGBB.
  //! Note that this unpacking does NOT involve non-linear sRGB -> linear RGB conversion,
  //! as would be usually expected for RGB color packed into 4 bytes.
  //! @param[in] theColor  color to convert
  //! @param[out] theARGB  result color encoded as integer
  static void Color2argb(const Quantity_Color& theColor, Standard_Integer& theARGB)
  {
    const NCollection_Vec3<Standard_Integer> aColor(
      static_cast<Standard_Integer>(255.0f * theColor.myRgb.r() + 0.5f),
      static_cast<Standard_Integer>(255.0f * theColor.myRgb.g() + 0.5f),
      static_cast<Standard_Integer>(255.0f * theColor.myRgb.b() + 0.5f));
    theARGB = (((aColor.r() & 0xff) << 16) | ((aColor.g() & 0xff) << 8) | (aColor.b() & 0xff));
  }

  //! Convert integer ARGB value to Color. Alpha bits are ignored.
  //! Note that this packing does NOT involve linear -> non-linear sRGB conversion,
  //! as would be usually expected to preserve higher (for human eye) color precision in 4 bytes.
  static void Argb2color(const Standard_Integer theARGB, Quantity_Color& theColor)
  {
    const NCollection_Vec3<Standard_Real> aColor(
      static_cast<Standard_Real>((theARGB & 0xff0000) >> 16),
      static_cast<Standard_Real>((theARGB & 0x00ff00) >> 8),
      static_cast<Standard_Real>((theARGB & 0x0000ff)));
    theColor.SetValues(aColor.r() / 255.0,
                       aColor.g() / 255.0,
                       aColor.b() / 255.0,
                       Quantity_TOC_sRGB);
  }

  //! Convert linear RGB component into sRGB using OpenGL specs formula (double precision), also
  //! known as gamma correction.
  static Standard_Real Convert_LinearRGB_To_sRGB(Standard_Real theLinearValue)
  {
    return theLinearValue <= 0.0031308 ? theLinearValue * 12.92
                                       : Pow(theLinearValue, 1.0 / 2.4) * 1.055 - 0.055;
  }

  //! Convert linear RGB component into sRGB using OpenGL specs formula (single precision), also
  //! known as gamma correction.
  static float Convert_LinearRGB_To_sRGB(float theLinearValue)
  {
    return theLinearValue <= 0.0031308f ? theLinearValue * 12.92f
                                        : powf(theLinearValue, 1.0f / 2.4f) * 1.055f - 0.055f;
  }

  //! Convert sRGB component into linear RGB using OpenGL specs formula (double precision), also
  //! known as gamma correction.
  static Standard_Real Convert_sRGB_To_LinearRGB(Standard_Real thesRGBValue)
  {
    return thesRGBValue <= 0.04045 ? thesRGBValue / 12.92
                                   : Pow((thesRGBValue + 0.055) / 1.055, 2.4);
  }

  //! Convert sRGB component into linear RGB using OpenGL specs formula (single precision), also
  //! known as gamma correction.
  static float Convert_sRGB_To_LinearRGB(float thesRGBValue)
  {
    return thesRGBValue <= 0.04045f ? thesRGBValue / 12.92f
                                    : powf((thesRGBValue + 0.055f) / 1.055f, 2.4f);
  }

  //! Convert linear RGB components into sRGB using OpenGL specs formula.
  template <typename T>
  static NCollection_Vec3<T> Convert_LinearRGB_To_sRGB(const NCollection_Vec3<T>& theRGB)
  {
    return NCollection_Vec3<T>(Convert_LinearRGB_To_sRGB(theRGB.r()),
                               Convert_LinearRGB_To_sRGB(theRGB.g()),
                               Convert_LinearRGB_To_sRGB(theRGB.b()));
  }

  //! Convert sRGB components into linear RGB using OpenGL specs formula.
  template <typename T>
  static NCollection_Vec3<T> Convert_sRGB_To_LinearRGB(const NCollection_Vec3<T>& theRGB)
  {
    return NCollection_Vec3<T>(Convert_sRGB_To_LinearRGB(theRGB.r()),
                               Convert_sRGB_To_LinearRGB(theRGB.g()),
                               Convert_sRGB_To_LinearRGB(theRGB.b()));
  }

  //! Convert linear RGB component into sRGB using approximated uniform gamma coefficient 2.2.
  static float Convert_LinearRGB_To_sRGB_approx22(float theLinearValue)
  {
    return powf(theLinearValue, 2.2f);
  }

  //! Convert sRGB component into linear RGB using approximated uniform gamma coefficient 2.2
  static float Convert_sRGB_To_LinearRGB_approx22(float thesRGBValue)
  {
    return powf(thesRGBValue, 1.0f / 2.2f);
  }

  //! Convert linear RGB components into sRGB using approximated uniform gamma coefficient 2.2
  static NCollection_Vec3<float> Convert_LinearRGB_To_sRGB_approx22(
    const NCollection_Vec3<float>& theRGB)
  {
    return NCollection_Vec3<float>(Convert_LinearRGB_To_sRGB_approx22(theRGB.r()),
                                   Convert_LinearRGB_To_sRGB_approx22(theRGB.g()),
                                   Convert_LinearRGB_To_sRGB_approx22(theRGB.b()));
  }

  //! Convert sRGB components into linear RGB using approximated uniform gamma coefficient 2.2
  static NCollection_Vec3<float> Convert_sRGB_To_LinearRGB_approx22(
    const NCollection_Vec3<float>& theRGB)
  {
    return NCollection_Vec3<float>(Convert_sRGB_To_LinearRGB_approx22(theRGB.r()),
                                   Convert_sRGB_To_LinearRGB_approx22(theRGB.g()),
                                   Convert_sRGB_To_LinearRGB_approx22(theRGB.b()));
  }

  //! Converts HLS components into sRGB ones.
  static void HlsRgb(const Standard_Real theH,
                     const Standard_Real theL,
                     const Standard_Real theS,
                     Standard_Real&      theR,
                     Standard_Real&      theG,
                     Standard_Real&      theB)
  {
    const NCollection_Vec3<float> anRgb =
      Convert_HLS_To_sRGB(NCollection_Vec3<float>((float)theH, (float)theL, (float)theS));
    theR = anRgb[0];
    theG = anRgb[1];
    theB = anRgb[2];
  }

  //! Converts sRGB components into HLS ones.
  static void RgbHls(const Standard_Real theR,
                     const Standard_Real theG,
                     const Standard_Real theB,
                     Standard_Real&      theH,
                     Standard_Real&      theL,
                     Standard_Real&      theS)
  {
    const NCollection_Vec3<float> aHls =
      Convert_sRGB_To_HLS(NCollection_Vec3<float>((float)theR, (float)theG, (float)theB));
    theH = aHls[0];
    theL = aHls[1];
    theS = aHls[2];
  }

public:
  //! Returns the value used to compare two colors for equality; 0.0001 by default.
  Standard_EXPORT static Standard_Real Epsilon();

  //! Set the value used to compare two colors for equality.
  Standard_EXPORT static void SetEpsilon(const Standard_Real theEpsilon);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson(const Standard_SStream& theSStream,
                                                Standard_Integer&       theStreamPos);

private:
  //! Returns the values of a predefined color according to the mode.
  Standard_EXPORT static NCollection_Vec3<float> valuesOf(const Quantity_NameOfColor theName,
                                                          const Quantity_TypeOfColor theType);

private:
  NCollection_Vec3<float> myRgb;
};

namespace std
{
template <>
struct hash<Quantity_Color>
{
  std::size_t operator()(const Quantity_Color& theColor) const noexcept
  {
    unsigned char aByteArr[3] = {static_cast<unsigned char>(255 * theColor.Red()),
                                 static_cast<unsigned char>(255 * theColor.Green()),
                                 static_cast<unsigned char>(255 * theColor.Blue())};
    return opencascade::hashBytes(aByteArr, sizeof(aByteArr));
  }
};
} // namespace std

#endif // _Quantity_Color_HeaderFile
