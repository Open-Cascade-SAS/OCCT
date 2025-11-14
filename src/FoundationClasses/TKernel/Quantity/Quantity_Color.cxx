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

#include <Quantity_Color.hxx>

#include <Quantity_ColorRGBA.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Dump.hxx>
#include <TCollection_AsciiString.hxx>

namespace
{
static constexpr float  RGBHLS_H_UNDEFINED = -1.0f;
static constexpr double DEG_TO_RAD         = M_PI / 180.0;
static constexpr double RAD_TO_DEG         = 180.0 / M_PI;
static constexpr double POW_25_7           = 6103515625.0;         // 25^7 used in CIEDE2000
static constexpr double CIELAB_EPSILON     = 0.008856451679035631; // (6/29)^3
static constexpr double CIELAB_KAPPA       = 7.787037037037037;    // (1/3) * (29/6)^2
static constexpr double CIELAB_OFFSET      = 16.0 / 116.0;
static constexpr double CIELAB_L_COEFF     = 116.0;
static constexpr double CIELAB_A_COEFF     = 500.0;
static constexpr double CIELAB_B_COEFF     = 200.0;
static constexpr double CIELAB_L_OFFSET    = 16.0;
// D65 / 2 deg (CIE 1931) standard illuminant reference white point
static constexpr double D65_REF_X = 95.047;
static constexpr double D65_REF_Y = 100.000;
static constexpr double D65_REF_Z = 108.883;
// sRGB to XYZ conversion matrix (D65 illuminant, 2 deg observer)
static constexpr double RGB_TO_XYZ_R_X = 0.4124564;
static constexpr double RGB_TO_XYZ_R_Y = 0.2126729;
static constexpr double RGB_TO_XYZ_R_Z = 0.0193339;
static constexpr double RGB_TO_XYZ_G_X = 0.3575761;
static constexpr double RGB_TO_XYZ_G_Y = 0.7151522;
static constexpr double RGB_TO_XYZ_G_Z = 0.1191920;
static constexpr double RGB_TO_XYZ_B_X = 0.1804375;
static constexpr double RGB_TO_XYZ_B_Y = 0.0721750;
static constexpr double RGB_TO_XYZ_B_Z = 0.9503041;
// XYZ to sRGB conversion matrix (D65 illuminant, 2 deg observer)
static constexpr double XYZ_TO_RGB_X_R = 3.2404542;
static constexpr double XYZ_TO_RGB_X_G = -0.9692660;
static constexpr double XYZ_TO_RGB_X_B = 0.0556434;
static constexpr double XYZ_TO_RGB_Y_R = -1.5371385;
static constexpr double XYZ_TO_RGB_Y_G = 1.8760108;
static constexpr double XYZ_TO_RGB_Y_B = -0.2040259;
static constexpr double XYZ_TO_RGB_Z_R = -0.4985314;
static constexpr double XYZ_TO_RGB_Z_G = 0.0415560;
static constexpr double XYZ_TO_RGB_Z_B = 1.0572252;
} // namespace

namespace
{
// Returns a reference to the epsilon value.
inline Standard_Real& getEpsilonRef() noexcept
{
  static Standard_Real theEpsilon = 0.0001;
  return theEpsilon;
}

// Validate RGB values are in range [0, 1].
inline void validateRgbRange(Standard_Real theR, Standard_Real theG, Standard_Real theB)
{
  if (theR < 0.0 || theR > 1.0 || theG < 0.0 || theG > 1.0 || theB < 0.0 || theB > 1.0)
  {
    throw Standard_OutOfRange("Color out");
  }
}

// Validate HLS values are in valid ranges.
inline void validateHlsRange(Standard_Real theH, Standard_Real theL, Standard_Real theS)
{
  if ((theH < 0.0 && theH != RGBHLS_H_UNDEFINED && theS != 0.0) || (theH > 360.0) || theL < 0.0
      || theL > 1.0 || theS < 0.0 || theS > 1.0)
  {
    throw Standard_OutOfRange("Color out");
  }
}

// Validate CIELab color values are in valid ranges.
inline void validateLabRange(Standard_Real theL, Standard_Real thea, Standard_Real theb)
{
  if (theL < 0. || theL > 100. || thea < -100. || thea > 100. || theb < -110. || theb > 100.)
  {
    throw Standard_OutOfRange("Color out");
  }
}

// Validate CIELch color values are in valid ranges.
inline void validateLchRange(Standard_Real theL, Standard_Real thec, Standard_Real theh)
{
  if (theL < 0. || theL > 100. || thec < 0. || thec > 135. || theh < 0.0 || theh > 360.)
  {
    throw Standard_OutOfRange("Color out");
  }
}
} // anonymous namespace

namespace
{
//! Raw color for defining list of standard color
struct Quantity_StandardColor
{
  const char*             StringName;
  NCollection_Vec3<float> sRgbValues;
  NCollection_Vec3<float> RgbValues;
  Quantity_NameOfColor    EnumName;

  constexpr Quantity_StandardColor(Quantity_NameOfColor           theName,
                                   const char*                    theStringName,
                                   const NCollection_Vec3<float>& thesRGB,
                                   const NCollection_Vec3<float>& theRGB) noexcept
      : StringName(theStringName),
        sRgbValues(thesRGB),
        RgbValues(theRGB),
        EnumName(theName)
  {
  }
};
} // namespace

// Note that HTML/hex sRGB representation is ignored
#define RawColor(theName, theHex, SRGB, sR, sG, sB, RGB, theR, theG, theB)                         \
  Quantity_StandardColor(Quantity_NOC_##theName,                                                   \
                         #theName,                                                                 \
                         NCollection_Vec3<float>(sR##f, sG##f, sB##f),                             \
                         NCollection_Vec3<float>(theR##f, theG##f, theB##f))

//! Name list of standard materials (defined within enumeration).
static constexpr Quantity_StandardColor THE_COLORS[] = {
#include "Quantity_ColorTable.pxx"
};

//=================================================================================================

Standard_Real Quantity_Color::Epsilon() noexcept
{
  return getEpsilonRef();
}

//=================================================================================================

void Quantity_Color::SetEpsilon(const Standard_Real theEpsilon) noexcept
{
  getEpsilonRef() = theEpsilon;
}

//=================================================================================================

NCollection_Vec3<float> Quantity_Color::valuesOf(const Quantity_NameOfColor theName,
                                                 const Quantity_TypeOfColor theType)
{
  if ((Standard_Integer)theName < 0 || (Standard_Integer)theName > Quantity_NOC_WHITE)
  {
    throw Standard_OutOfRange("Bad name");
  }

  const NCollection_Vec3<float>& anRgb = THE_COLORS[theName].RgbValues;
  switch (theType)
  {
    case Quantity_TOC_RGB:
      return anRgb;
    case Quantity_TOC_sRGB:
      return Convert_LinearRGB_To_sRGB(anRgb);
    case Quantity_TOC_HLS:
      return Convert_LinearRGB_To_HLS(anRgb);
    case Quantity_TOC_CIELab:
      return Convert_LinearRGB_To_Lab(anRgb);
    case Quantity_TOC_CIELch:
      return Convert_Lab_To_Lch(Convert_LinearRGB_To_Lab(anRgb));
  }
  throw Standard_ProgramError("Internal error");
}

//=================================================================================================

Standard_CString Quantity_Color::StringName(const Quantity_NameOfColor theName) noexcept
{
  if ((Standard_Integer)theName < 0 || (Standard_Integer)theName > Quantity_NOC_WHITE)
  {
    return "UNDEFINED";
  }
  return THE_COLORS[theName].StringName;
}

//=================================================================================================

Standard_Boolean Quantity_Color::ColorFromName(const Standard_CString theName,
                                               Quantity_NameOfColor&  theColor) noexcept
{
  TCollection_AsciiString aName(theName);
  aName.UpperCase();
  if (aName.Search("QUANTITY_NOC_") == 1)
  {
    aName = aName.SubString(14, aName.Length());
  }

  for (Standard_Integer anIter = Quantity_NOC_BLACK; anIter <= Quantity_NOC_WHITE; ++anIter)
  {
    Standard_CString aColorName = THE_COLORS[anIter].StringName;
    if (aName == aColorName)
    {
      theColor = (Quantity_NameOfColor)anIter;
      return Standard_True;
    }
  }

  // aliases
  if (aName == "BLUE1")
  {
    theColor = Quantity_NOC_BLUE1;
  }
  else if (aName == "CHARTREUSE1")
  {
    theColor = Quantity_NOC_CHARTREUSE1;
  }
  else if (aName == "CYAN1")
  {
    theColor = Quantity_NOC_CYAN1;
  }
  else if (aName == "GOLD1")
  {
    theColor = Quantity_NOC_GOLD1;
  }
  else if (aName == "GREEN1")
  {
    theColor = Quantity_NOC_GREEN1;
  }
  else if (aName == "LIGHTCYAN1")
  {
    theColor = Quantity_NOC_LIGHTCYAN1;
  }
  else if (aName == "MAGENTA1")
  {
    theColor = Quantity_NOC_MAGENTA1;
  }
  else if (aName == "ORANGE1")
  {
    theColor = Quantity_NOC_ORANGE1;
  }
  else if (aName == "ORANGERED1")
  {
    theColor = Quantity_NOC_ORANGERED1;
  }
  else if (aName == "RED1")
  {
    theColor = Quantity_NOC_RED1;
  }
  else if (aName == "TOMATO1")
  {
    theColor = Quantity_NOC_TOMATO1;
  }
  else if (aName == "YELLOW1")
  {
    theColor = Quantity_NOC_YELLOW1;
  }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

bool Quantity_Color::ColorFromHex(const Standard_CString theHexColorString,
                                  Quantity_Color&        theColor)
{
  Quantity_ColorRGBA aColorRGBA;
  if (!Quantity_ColorRGBA::ColorFromHex(theHexColorString, aColorRGBA, true))
  {
    return false;
  }
  theColor = aColorRGBA.GetRGB();
  return true;
}

//=================================================================================================

Quantity_Color::Quantity_Color(const Standard_Real        theC1,
                               const Standard_Real        theC2,
                               const Standard_Real        theC3,
                               const Quantity_TypeOfColor theType)
{
  SetValues(theC1, theC2, theC3, theType);
}

//=================================================================================================

Quantity_Color::Quantity_Color(const NCollection_Vec3<float>& theRgb)
    : myRgb(theRgb)
{
  validateRgbRange(theRgb.r(), theRgb.g(), theRgb.b());
}

//=================================================================================================

void Quantity_Color::ChangeContrast(const Standard_Real theDelta)
{
  NCollection_Vec3<float> aHls = Convert_LinearRGB_To_HLS(myRgb);
  aHls[2] += aHls[2] * Standard_ShortReal(theDelta) / 100.0f; // saturation
  if (!((aHls[2] > 1.0f) || (aHls[2] < 0.0f)))
  {
    myRgb = Convert_HLS_To_LinearRGB(aHls);
  }
}

//=================================================================================================

void Quantity_Color::ChangeIntensity(const Standard_Real theDelta)
{
  NCollection_Vec3<float> aHls = Convert_LinearRGB_To_HLS(myRgb);
  aHls[1] += aHls[1] * Standard_ShortReal(theDelta) / 100.0f; // light
  if (!((aHls[1] > 1.0f) || (aHls[1] < 0.0f)))
  {
    myRgb = Convert_HLS_To_LinearRGB(aHls);
  }
}

//=================================================================================================

void Quantity_Color::SetValues(const Standard_Real        theC1,
                               const Standard_Real        theC2,
                               const Standard_Real        theC3,
                               const Quantity_TypeOfColor theType)
{
  switch (theType)
  {
    case Quantity_TOC_RGB: {
      validateRgbRange(theC1, theC2, theC3);
      myRgb.SetValues(float(theC1), float(theC2), float(theC3));
      break;
    }
    case Quantity_TOC_sRGB: {
      validateRgbRange(theC1, theC2, theC3);
      myRgb.SetValues((float)Convert_sRGB_To_LinearRGB(theC1),
                      (float)Convert_sRGB_To_LinearRGB(theC2),
                      (float)Convert_sRGB_To_LinearRGB(theC3));
      break;
    }
    case Quantity_TOC_HLS: {
      validateHlsRange(theC1, theC2, theC3);
      myRgb =
        Convert_HLS_To_LinearRGB(NCollection_Vec3<float>(float(theC1), float(theC2), float(theC3)));
      break;
    }
    case Quantity_TOC_CIELab: {
      validateLabRange(theC1, theC2, theC3);
      myRgb =
        Convert_Lab_To_LinearRGB(NCollection_Vec3<float>(float(theC1), float(theC2), float(theC3)));
      break;
    }
    case Quantity_TOC_CIELch: {
      validateLchRange(theC1, theC2, theC3);
      myRgb = Convert_Lab_To_LinearRGB(
        Convert_Lch_To_Lab(NCollection_Vec3<float>(float(theC1), float(theC2), float(theC3))));
      break;
    }
  }
}

//=================================================================================================

void Quantity_Color::Delta(const Quantity_Color& theColor,
                           Standard_Real&        theDC,
                           Standard_Real&        theDI) const
{
  const NCollection_Vec3<float> aHls1 = Convert_LinearRGB_To_HLS(myRgb);
  const NCollection_Vec3<float> aHls2 = Convert_LinearRGB_To_HLS(theColor.myRgb);
  theDC                               = Standard_Real(aHls1[2] - aHls2[2]); // saturation
  theDI                               = Standard_Real(aHls1[1] - aHls2[1]); // light
}

// =======================================================================
// function : DeltaE2000
// purpose  : color difference according to CIE Delta E 2000 formula
// see http://brucelindbloom.com/index.html?Eqn_DeltaE_CIE2000.html
// =======================================================================
Standard_Real Quantity_Color::DeltaE2000(const Quantity_Color& theOther) const
{
  // get color components in CIE Lch space
  Standard_Real aL1, aL2, aa1, aa2, ab1, ab2;
  this->Values(aL1, aa1, ab1, Quantity_TOC_CIELab);
  theOther.Values(aL2, aa2, ab2, Quantity_TOC_CIELab);

  // mean L
  Standard_Real aLx_mean = 0.5 * (aL1 + aL2);

  // mean C
  Standard_Real aC1          = std::sqrt(aa1 * aa1 + ab1 * ab1);
  Standard_Real aC2          = std::sqrt(aa2 * aa2 + ab2 * ab2);
  Standard_Real aC_mean      = 0.5 * (aC1 + aC2);
  Standard_Real aC_mean_pow7 = std::pow(aC_mean, 7);
  Standard_Real aG           = 0.5 * (1. - std::sqrt(aC_mean_pow7 / (aC_mean_pow7 + POW_25_7)));
  Standard_Real aa1x         = aa1 * (1. + aG);
  Standard_Real aa2x         = aa2 * (1. + aG);
  Standard_Real aC1x         = std::sqrt(aa1x * aa1x + ab1 * ab1);
  Standard_Real aC2x         = std::sqrt(aa2x * aa2x + ab2 * ab2);
  Standard_Real aCx_mean     = 0.5 * (aC1x + aC2x);

  // mean H
  Standard_Real ah1x = (aC1x > Epsilon() ? std::atan2(ab1, aa1x) * RAD_TO_DEG : 270.);
  Standard_Real ah2x = (aC2x > Epsilon() ? std::atan2(ab2, aa2x) * RAD_TO_DEG : 270.);
  if (ah1x < 0.)
    ah1x += 360.;
  if (ah2x < 0.)
    ah2x += 360.;
  Standard_Real aHx_mean = 0.5 * (ah1x + ah2x);
  Standard_Real aDeltahx = ah2x - ah1x;
  if (std::abs(aDeltahx) > 180.)
  {
    aHx_mean += (aHx_mean < 180. ? 180. : -180.);
    aDeltahx += (ah1x >= ah2x ? 360. : -360.);
  }

  // deltas
  Standard_Real aDeltaLx = aL2 - aL1;
  Standard_Real aDeltaCx = aC2x - aC1x;
  Standard_Real aDeltaHx = 2. * std::sqrt(aC1x * aC2x) * std::sin(0.5 * aDeltahx * DEG_TO_RAD);

  // factors
  Standard_Real aT = 1. - 0.17 * std::cos((aHx_mean - 30.) * DEG_TO_RAD)
                     + 0.24 * std::cos((2. * aHx_mean) * DEG_TO_RAD)
                     + 0.32 * std::cos((3. * aHx_mean + 6.) * DEG_TO_RAD)
                     - 0.20 * std::cos((4. * aHx_mean - 63.) * DEG_TO_RAD);

  Standard_Real aLx_mean50_2 = (aLx_mean - 50.) * (aLx_mean - 50.);
  Standard_Real aS_L         = 1. + 0.015 * aLx_mean50_2 / std::sqrt(20. + aLx_mean50_2);
  Standard_Real aS_C         = 1. + 0.045 * aCx_mean;
  Standard_Real aS_H         = 1. + 0.015 * aCx_mean * aT;

  Standard_Real aDelta_theta  = 30. * std::exp(-(aHx_mean - 275.) * (aHx_mean - 275.) / 625.);
  Standard_Real aCx_mean_pow7 = std::pow(aCx_mean, 7);
  Standard_Real aR_C          = 2. * std::sqrt(aCx_mean_pow7 / (aCx_mean_pow7 + POW_25_7));
  Standard_Real aR_T          = -aR_C * std::sin(2. * aDelta_theta * DEG_TO_RAD);

  // finally, the difference
  Standard_Real aDL         = aDeltaLx / aS_L;
  Standard_Real aDC         = aDeltaCx / aS_C;
  Standard_Real aDH         = aDeltaHx / aS_H;
  Standard_Real aDeltaE2000 = std::sqrt(aDL * aDL + aDC * aDC + aDH * aDH + aR_T * aDC * aDH);
  return aDeltaE2000;
}

//=================================================================================================

Quantity_NameOfColor Quantity_Color::Name() const
{
  // it is better finding closest sRGB color (closest to human eye) instead of linear RGB color,
  // as enumeration defines color names for human
  const NCollection_Vec3<float> ansRgbVec(
    Convert_LinearRGB_To_sRGB(NCollection_Vec3<Standard_Real>(myRgb)));
  Standard_ShortReal   aDist2   = ShortRealLast();
  Quantity_NameOfColor aResName = Quantity_NOC_BLACK;
  for (Standard_Integer aColIter = Quantity_NOC_BLACK; aColIter <= Quantity_NOC_WHITE; ++aColIter)
  {
    const Standard_ShortReal aNewDist2 =
      (ansRgbVec - THE_COLORS[aColIter].sRgbValues).SquareModulus();
    if (aNewDist2 < aDist2)
    {
      aResName = Quantity_NameOfColor(aColIter);
      aDist2   = aNewDist2;
      if (aNewDist2 == 0.0f)
      {
        break;
      }
    }
  }
  return aResName;
}

//=================================================================================================

void Quantity_Color::Values(Standard_Real&             theR1,
                            Standard_Real&             theR2,
                            Standard_Real&             theR3,
                            const Quantity_TypeOfColor theType) const
{
  switch (theType)
  {
    case Quantity_TOC_RGB: {
      theR1 = myRgb.r();
      theR2 = myRgb.g();
      theR3 = myRgb.b();
      break;
    }
    case Quantity_TOC_sRGB: {
      theR1 = Convert_LinearRGB_To_sRGB((Standard_Real)myRgb.r());
      theR2 = Convert_LinearRGB_To_sRGB((Standard_Real)myRgb.g());
      theR3 = Convert_LinearRGB_To_sRGB((Standard_Real)myRgb.b());
      break;
    }
    case Quantity_TOC_HLS: {
      const NCollection_Vec3<float> aHls = Convert_LinearRGB_To_HLS(myRgb);
      theR1                              = aHls[0];
      theR2                              = aHls[1];
      theR3                              = aHls[2];
      break;
    }
    case Quantity_TOC_CIELab: {
      const NCollection_Vec3<float> aLab = Convert_LinearRGB_To_Lab(myRgb);
      theR1                              = aLab[0];
      theR2                              = aLab[1];
      theR3                              = aLab[2];
      break;
    }
    case Quantity_TOC_CIELch: {
      const NCollection_Vec3<float> aLch = Convert_Lab_To_Lch(Convert_LinearRGB_To_Lab(myRgb));
      theR1                              = aLch[0];
      theR2                              = aLch[1];
      theR3                              = aLch[2];
      break;
    }
  }
}

// =======================================================================
// function : Convert_HLS_To_sRGB
// purpose  : Reference: La synthese d'images, Collection Hermes
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_HLS_To_sRGB(const NCollection_Vec3<float>& theHls)
{
  float       aHue        = theHls[0];
  const float aLight      = theHls[1];
  const float aSaturation = theHls[2];
  if (aSaturation == 0.0f && aHue == RGBHLS_H_UNDEFINED)
  {
    return NCollection_Vec3<float>(aLight, aLight, aLight);
  }

  int   aHueIndex = 0;
  float lmuls     = aLight * aSaturation;
  if (aHue == 360.0f)
  {
    aHue      = 0.0;
    aHueIndex = 0;
  }
  else
  {
    aHue /= 60.0f;
    aHueIndex = (int)aHue;
  }

  switch (aHueIndex)
  {
    case 0:
      return NCollection_Vec3<float>(aLight, aLight - lmuls + lmuls * aHue, aLight - lmuls);
    case 1:
      return NCollection_Vec3<float>(aLight + lmuls - lmuls * aHue, aLight, aLight - lmuls);
    case 2:
      return NCollection_Vec3<float>(aLight - lmuls, aLight, aLight - 3 * lmuls + lmuls * aHue);
    case 3:
      return NCollection_Vec3<float>(aLight - lmuls, aLight + 3 * lmuls - lmuls * aHue, aLight);
    case 4:
      return NCollection_Vec3<float>(aLight - 5 * lmuls + lmuls * aHue, aLight - lmuls, aLight);
    case 5:
      return NCollection_Vec3<float>(aLight, aLight - lmuls, aLight + 5 * lmuls - lmuls * aHue);
  }
  throw Standard_OutOfRange("Color out");
}

// =======================================================================
// function : Convert_sRGB_To_HLS
// purpose  : Reference: La synthese d'images, Collection Hermes
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_sRGB_To_HLS(
  const NCollection_Vec3<float>& theRgb) noexcept
{
  float aPlus = 0.0f;
  float aDiff = theRgb.g() - theRgb.b();

  // compute maximum from RGB components, which will be a luminance
  float aMax = theRgb.r();
  if (theRgb.g() > aMax)
  {
    aPlus = 2.0;
    aDiff = theRgb.b() - theRgb.r();
    aMax  = theRgb.g();
  }
  if (theRgb.b() > aMax)
  {
    aPlus = 4.0;
    aDiff = theRgb.r() - theRgb.g();
    aMax  = theRgb.b();
  }

  // compute minimum from RGB components
  float min = theRgb.r();
  if (theRgb.g() < min)
    min = theRgb.g();
  if (theRgb.b() < min)
    min = theRgb.b();

  const float aDelta = aMax - min;

  // compute saturation
  float aSaturation = 0.0f;
  if (aMax != 0.0f)
    aSaturation = aDelta / aMax;

  // compute hue
  float aHue = RGBHLS_H_UNDEFINED;
  if (aSaturation != 0.0f)
  {
    aHue = 60.0f * (aPlus + aDiff / aDelta);
    if (aHue < 0.0f)
      aHue += 360.0f;
  }
  return NCollection_Vec3<float>(aHue, aMax, aSaturation);
}

// =======================================================================
// function : CIELab_f
// purpose  : non-linear function transforming XYZ coordinates to CIE Lab
// see http://www.brucelindbloom.com/index.html?Equations.html
// =======================================================================
static inline double CIELab_f(double theValue) noexcept
{
  return theValue > CIELAB_EPSILON ? std::pow(theValue, 1. / 3.)
                                   : (CIELAB_KAPPA * theValue) + CIELAB_OFFSET;
}

// =======================================================================
// function : CIELab_invertf
// purpose  : inverse of non-linear function transforming XYZ coordinates to CIE Lab
// see http://www.brucelindbloom.com/index.html?Equations.html
// =======================================================================
static inline double CIELab_invertf(double theValue) noexcept
{
  double aV3 = theValue * theValue * theValue;
  return aV3 > CIELAB_EPSILON ? aV3 : (theValue - CIELAB_OFFSET) / CIELAB_KAPPA;
}

// =======================================================================
// function : Convert_LinearRGB_To_Lab
// purpose  : convert RGB color to CIE Lab color
// see https://www.easyrgb.com/en/math.php
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_LinearRGB_To_Lab(
  const NCollection_Vec3<float>& theRgb) noexcept
{
  double aR = theRgb[0];
  double aG = theRgb[1];
  double aB = theRgb[2];

  // convert to XYZ normalized to D65 / 2 deg (CIE 1931) standard illuminant intensities
  // see http://www.brucelindbloom.com/index.html?Equations.html
  double aX = (aR * RGB_TO_XYZ_R_X + aG * RGB_TO_XYZ_G_X + aB * RGB_TO_XYZ_B_X) * 100. / D65_REF_X;
  double aY = (aR * RGB_TO_XYZ_R_Y + aG * RGB_TO_XYZ_G_Y + aB * RGB_TO_XYZ_B_Y);
  double aZ = (aR * RGB_TO_XYZ_R_Z + aG * RGB_TO_XYZ_G_Z + aB * RGB_TO_XYZ_B_Z) * 100. / D65_REF_Z;

  // convert to Lab
  double afX = CIELab_f(aX);
  double afY = CIELab_f(aY);
  double afZ = CIELab_f(aZ);

  double aL = CIELAB_L_COEFF * afY - CIELAB_L_OFFSET;
  double aa = CIELAB_A_COEFF * (afX - afY);
  double ab = CIELAB_B_COEFF * (afY - afZ);

  return NCollection_Vec3<float>((float)aL, (float)aa, (float)ab);
}

// =======================================================================
// function : Convert_Lab_To_LinearRGB
// purpose  : convert CIE Lab color to RGB
// see https://www.easyrgb.com/en/math.php
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_Lab_To_LinearRGB(
  const NCollection_Vec3<float>& theLab) noexcept
{
  double aL = theLab[0];
  double aa = theLab[1];
  double ab = theLab[2];

  // conversion from Lab to RGB can yield point outside of RGB cube,
  // in such case we will reduce a and b components gradually
  // (by 0.1% at each step) until we fit into the range;
  // NB: the procedure could be improved to get more precise
  // result but this does not seem really crucial
  const int NBSTEPS = 1000;
  for (Standard_Integer aRate = NBSTEPS;; aRate--)
  {
    double aC = aRate / (double)NBSTEPS;

    // convert to XYZ for D65 / 2 deg (CIE 1931) standard illuminant
    double afY = (aL + CIELAB_L_OFFSET) / CIELAB_L_COEFF;
    double afX = aC * aa / CIELAB_A_COEFF + afY;
    double afZ = afY - aC * ab / CIELAB_B_COEFF;

    double aX = CIELab_invertf(afX) * D65_REF_X;
    double aY = CIELab_invertf(afY) * D65_REF_Y;
    double aZ = CIELab_invertf(afZ) * D65_REF_Z;

    // convert to RGB
    // see http://www.brucelindbloom.com/index.html?Equations.html
    double aR = (aX * XYZ_TO_RGB_X_R + aY * XYZ_TO_RGB_Y_R + aZ * XYZ_TO_RGB_Z_R) / 100.;
    double aG = (aX * XYZ_TO_RGB_X_G + aY * XYZ_TO_RGB_Y_G + aZ * XYZ_TO_RGB_Z_G) / 100.;
    double aB = (aX * XYZ_TO_RGB_X_B + aY * XYZ_TO_RGB_Y_B + aZ * XYZ_TO_RGB_Z_B) / 100.;

    // exit if we are in range or at zero C
    if (aRate == 0 || (aR >= 0. && aR <= 1. && aG >= 0. && aG <= 1. && aB >= 0. && aB <= 1.))
    {
      return NCollection_Vec3<float>((float)aR, (float)aG, (float)aB);
    }
  }
}

// =======================================================================
// function : Convert_Lab_To_Lch
// purpose  : convert CIE Lab color to CIE Lch color
// see https://www.easyrgb.com/en/math.php
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_Lab_To_Lch(
  const NCollection_Vec3<float>& theLab) noexcept
{
  double aa = theLab[1];
  double ab = theLab[2];

  double aC = std::sqrt(aa * aa + ab * ab);
  double aH = (aC > Epsilon() ? std::atan2(ab, aa) * RAD_TO_DEG : 0.);

  if (aH < 0.)
    aH += 360.;

  return NCollection_Vec3<float>(theLab[0], (float)aC, (float)aH);
}

// =======================================================================
// function : Convert_Lch_To_Lab
// purpose  : convert CIE Lch color to CIE Lab color
// see https://www.easyrgb.com/en/math.php
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_Lch_To_Lab(
  const NCollection_Vec3<float>& theLch) noexcept
{
  double aC = theLch[1];
  double aH = theLch[2];

  aH *= DEG_TO_RAD;

  double aa = aC * std::cos(aH);
  double ab = aC * std::sin(aH);

  return NCollection_Vec3<float>(theLch[0], (float)aa, (float)ab);
}

//=================================================================================================

void Quantity_Color::DumpJson(Standard_OStream& theOStream, Standard_Integer) const {
  OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "RGB", 3, myRgb.r(), myRgb.g(), myRgb.b())}

//=================================================================================================

Standard_Boolean Quantity_Color::InitFromJson(const Standard_SStream& theSStream,
                                              Standard_Integer&       theStreamPos)
{
  Standard_Integer aPos = theStreamPos;
  Standard_Real    aRed, aGreen, aBlue;
  OCCT_INIT_VECTOR_CLASS(Standard_Dump::Text(theSStream), "RGB", aPos, 3, &aRed, &aGreen, &aBlue)

  SetValues((Standard_ShortReal)aRed,
            (Standard_ShortReal)aGreen,
            (Standard_ShortReal)aBlue,
            Quantity_TOC_RGB);
  return Standard_True;
}
