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

#define RGBHLS_H_UNDEFINED -1.0

static Standard_Real TheEpsilon = 0.0001;

// Throw exception if RGB values are out of range.
#define Quantity_ColorValidateRgbRange(theR, theG, theB)                                           \
  if (theR < 0.0 || theR > 1.0 || theG < 0.0 || theG > 1.0 || theB < 0.0 || theB > 1.0)            \
  {                                                                                                \
    throw Standard_OutOfRange("Color out");                                                        \
  }

// Throw exception if HLS values are out of range.
#define Quantity_ColorValidateHlsRange(theH, theL, theS)                                           \
  if ((theH < 0.0 && theH != RGBHLS_H_UNDEFINED && theS != 0.0) || (theH > 360.0) || theL < 0.0    \
      || theL > 1.0 || theS < 0.0 || theS > 1.0)                                                   \
  {                                                                                                \
    throw Standard_OutOfRange("Color out");                                                        \
  }

// Throw exception if CIELab color values are out of range.
#define Quantity_ColorValidateLabRange(theL, thea, theb)                                           \
  if (theL < 0. || theL > 100. || thea < -100. || thea > 100. || theb < -110. || theb > 100.)      \
  {                                                                                                \
    throw Standard_OutOfRange("Color out");                                                        \
  }

// Throw exception if CIELch color values are out of range.
#define Quantity_ColorValidateLchRange(theL, thec, theh)                                           \
  if (theL < 0. || theL > 100. || thec < 0. || thec > 135. || theh < 0.0 || theh > 360.)           \
  {                                                                                                \
    throw Standard_OutOfRange("Color out");                                                        \
  }

namespace
{
//! Raw color for defining list of standard color
struct Quantity_StandardColor
{
  const char*             StringName;
  NCollection_Vec3<float> sRgbValues;
  NCollection_Vec3<float> RgbValues;
  Quantity_NameOfColor    EnumName;

  Quantity_StandardColor(Quantity_NameOfColor           theName,
                         const char*                    theStringName,
                         const NCollection_Vec3<float>& thesRGB,
                         const NCollection_Vec3<float>& theRGB)
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
static const Quantity_StandardColor THE_COLORS[] = {
#include "Quantity_ColorTable.pxx"
};

// =======================================================================
// function : Epsilon
// purpose  :
// =======================================================================
Standard_Real Quantity_Color::Epsilon()
{
  return TheEpsilon;
}

// =======================================================================
// function : SetEpsilon
// purpose  :
// =======================================================================
void Quantity_Color::SetEpsilon(const Standard_Real theEpsilon)
{
  TheEpsilon = theEpsilon;
}

// =======================================================================
// function : valuesOf
// purpose  :
// =======================================================================
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

// =======================================================================
// function : StringName
// purpose  :
// =======================================================================
Standard_CString Quantity_Color::StringName(const Quantity_NameOfColor theName)
{
  if ((Standard_Integer)theName < 0 || (Standard_Integer)theName > Quantity_NOC_WHITE)
  {
    throw Standard_OutOfRange("Bad name");
  }
  return THE_COLORS[theName].StringName;
}

// =======================================================================
// function : ColorFromName
// purpose  :
// =======================================================================
Standard_Boolean Quantity_Color::ColorFromName(const Standard_CString theName,
                                               Quantity_NameOfColor&  theColor)
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

// =======================================================================
// function : Quantity_Color
// purpose  :
// =======================================================================
Quantity_Color::Quantity_Color(const Standard_Real        theC1,
                               const Standard_Real        theC2,
                               const Standard_Real        theC3,
                               const Quantity_TypeOfColor theType)
{
  SetValues(theC1, theC2, theC3, theType);
}

// =======================================================================
// function : Quantity_Color
// purpose  :
// =======================================================================
Quantity_Color::Quantity_Color(const NCollection_Vec3<float>& theRgb)
    : myRgb(theRgb)
{
  Quantity_ColorValidateRgbRange(theRgb.r(), theRgb.g(), theRgb.b());
}

// =======================================================================
// function : ChangeContrast
// purpose  :
// =======================================================================
void Quantity_Color::ChangeContrast(const Standard_Real theDelta)
{
  NCollection_Vec3<float> aHls = Convert_LinearRGB_To_HLS(myRgb);
  aHls[2] += aHls[2] * Standard_ShortReal(theDelta) / 100.0f; // saturation
  if (!((aHls[2] > 1.0f) || (aHls[2] < 0.0f)))
  {
    myRgb = Convert_HLS_To_LinearRGB(aHls);
  }
}

// =======================================================================
// function : ChangeIntensity
// purpose  :
// =======================================================================
void Quantity_Color::ChangeIntensity(const Standard_Real theDelta)
{
  NCollection_Vec3<float> aHls = Convert_LinearRGB_To_HLS(myRgb);
  aHls[1] += aHls[1] * Standard_ShortReal(theDelta) / 100.0f; // light
  if (!((aHls[1] > 1.0f) || (aHls[1] < 0.0f)))
  {
    myRgb = Convert_HLS_To_LinearRGB(aHls);
  }
}

// =======================================================================
// function : SetValues
// purpose  :
// =======================================================================
void Quantity_Color::SetValues(const Standard_Real        theC1,
                               const Standard_Real        theC2,
                               const Standard_Real        theC3,
                               const Quantity_TypeOfColor theType)
{
  switch (theType)
  {
    case Quantity_TOC_RGB: {
      Quantity_ColorValidateRgbRange(theC1, theC2, theC3);
      myRgb.SetValues(float(theC1), float(theC2), float(theC3));
      break;
    }
    case Quantity_TOC_sRGB: {
      Quantity_ColorValidateRgbRange(theC1, theC2, theC3);
      myRgb.SetValues((float)Convert_sRGB_To_LinearRGB(theC1),
                      (float)Convert_sRGB_To_LinearRGB(theC2),
                      (float)Convert_sRGB_To_LinearRGB(theC3));
      break;
    }
    case Quantity_TOC_HLS: {
      Quantity_ColorValidateHlsRange(theC1, theC2, theC3);
      myRgb =
        Convert_HLS_To_LinearRGB(NCollection_Vec3<float>(float(theC1), float(theC2), float(theC3)));
      break;
    }
    case Quantity_TOC_CIELab: {
      Quantity_ColorValidateLabRange(theC1, theC2, theC3);
      myRgb =
        Convert_Lab_To_LinearRGB(NCollection_Vec3<float>(float(theC1), float(theC2), float(theC3)));
      break;
    }
    case Quantity_TOC_CIELch: {
      Quantity_ColorValidateLchRange(theC1, theC2, theC3);
      myRgb = Convert_Lab_To_LinearRGB(
        Convert_Lch_To_Lab(NCollection_Vec3<float>(float(theC1), float(theC2), float(theC3))));
      break;
    }
  }
}

// =======================================================================
// function : Delta
// purpose  :
// =======================================================================
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
  Standard_Real       aC1          = Sqrt(aa1 * aa1 + ab1 * ab1);
  Standard_Real       aC2          = Sqrt(aa2 * aa2 + ab2 * ab2);
  Standard_Real       aC_mean      = 0.5 * (aC1 + aC2);
  Standard_Real       aC_mean_pow7 = Pow(aC_mean, 7);
  static const double a25_pow7     = Pow(25., 7);
  Standard_Real       aG           = 0.5 * (1. - Sqrt(aC_mean_pow7 / (aC_mean_pow7 + a25_pow7)));
  Standard_Real       aa1x         = aa1 * (1. + aG);
  Standard_Real       aa2x         = aa2 * (1. + aG);
  Standard_Real       aC1x         = Sqrt(aa1x * aa1x + ab1 * ab1);
  Standard_Real       aC2x         = Sqrt(aa2x * aa2x + ab2 * ab2);
  Standard_Real       aCx_mean     = 0.5 * (aC1x + aC2x);

  // mean H
  Standard_Real ah1x = (aC1x > TheEpsilon ? ATan2(ab1, aa1x) * 180. / M_PI : 270.);
  Standard_Real ah2x = (aC2x > TheEpsilon ? ATan2(ab2, aa2x) * 180. / M_PI : 270.);
  if (ah1x < 0.)
    ah1x += 360.;
  if (ah2x < 0.)
    ah2x += 360.;
  Standard_Real aHx_mean = 0.5 * (ah1x + ah2x);
  Standard_Real aDeltahx = ah2x - ah1x;
  if (Abs(aDeltahx) > 180.)
  {
    aHx_mean += (aHx_mean < 180. ? 180. : -180.);
    aDeltahx += (ah1x >= ah2x ? 360. : -360.);
  }

  // deltas
  Standard_Real aDeltaLx = aL2 - aL1;
  Standard_Real aDeltaCx = aC2x - aC1x;
  Standard_Real aDeltaHx = 2. * Sqrt(aC1x * aC2x) * Sin(0.5 * aDeltahx * M_PI / 180.);

  // factors
  Standard_Real aT = 1. - 0.17 * Cos((aHx_mean - 30.) * M_PI / 180.)
                     + 0.24 * Cos((2. * aHx_mean) * M_PI / 180.)
                     + 0.32 * Cos((3. * aHx_mean + 6.) * M_PI / 180.)
                     - 0.20 * Cos((4. * aHx_mean - 63.) * M_PI / 180.);

  Standard_Real aLx_mean50_2 = (aLx_mean - 50.) * (aLx_mean - 50.);
  Standard_Real aS_L         = 1. + 0.015 * aLx_mean50_2 / Sqrt(20. + aLx_mean50_2);
  Standard_Real aS_C         = 1. + 0.045 * aCx_mean;
  Standard_Real aS_H         = 1. + 0.015 * aCx_mean * aT;

  Standard_Real aDelta_theta  = 30. * Exp(-(aHx_mean - 275.) * (aHx_mean - 275.) / 625.);
  Standard_Real aCx_mean_pow7 = Pow(aCx_mean, 7);
  Standard_Real aR_C          = 2. * Sqrt(aCx_mean_pow7 / (aCx_mean_pow7 + a25_pow7));
  Standard_Real aR_T          = -aR_C * Sin(2. * aDelta_theta * M_PI / 180.);

  // finally, the difference
  Standard_Real aDL         = aDeltaLx / aS_L;
  Standard_Real aDC         = aDeltaCx / aS_C;
  Standard_Real aDH         = aDeltaHx / aS_H;
  Standard_Real aDeltaE2000 = Sqrt(aDL * aDL + aDC * aDC + aDH * aDH + aR_T * aDC * aDH);
  return aDeltaE2000;
}

// =======================================================================
// function : Name
// purpose  :
// =======================================================================
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

// =======================================================================
// function : Values
// purpose  :
// =======================================================================
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
NCollection_Vec3<float> Quantity_Color::Convert_sRGB_To_HLS(const NCollection_Vec3<float>& theRgb)
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
static inline double CIELab_f(double theValue)
{
  return theValue > 0.008856451679035631 ? Pow(theValue, 1. / 3.)
                                         : (7.787037037037037 * theValue) + 16. / 116.;
}

// =======================================================================
// function : CIELab_invertf
// purpose  : inverse of non-linear function transforming XYZ coordinates to CIE Lab
// see http://www.brucelindbloom.com/index.html?Equations.html
// =======================================================================
static inline double CIELab_invertf(double theValue)
{
  double aV3 = theValue * theValue * theValue;
  return aV3 > 0.008856451679035631 ? aV3 : (theValue - 16. / 116.) / 7.787037037037037;
}

// =======================================================================
// function : Convert_LinearRGB_To_Lab
// purpose  : convert RGB color to CIE Lab color
// see https://www.easyrgb.com/en/math.php
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_LinearRGB_To_Lab(
  const NCollection_Vec3<float>& theRgb)
{
  double aR = theRgb[0];
  double aG = theRgb[1];
  double aB = theRgb[2];

  // convert to XYZ normalized to D65 / 2 deg (CIE 1931) standard illuminant intensities
  // see http://www.brucelindbloom.com/index.html?Equations.html
  double aX = (aR * 0.4124564 + aG * 0.3575761 + aB * 0.1804375) * 100. / 95.047;
  double aY = (aR * 0.2126729 + aG * 0.7151522 + aB * 0.0721750) * 100. / 100.000;
  double aZ = (aR * 0.0193339 + aG * 0.1191920 + aB * 0.9503041) * 100. / 108.883;

  // convert to Lab
  double afX = CIELab_f(aX);
  double afY = CIELab_f(aY);
  double afZ = CIELab_f(aZ);

  double aL = 116. * afY - 16.;
  double aa = 500. * (afX - afY);
  double ab = 200. * (afY - afZ);

  return NCollection_Vec3<float>((float)aL, (float)aa, (float)ab);
}

// =======================================================================
// function : Convert_Lab_To_LinearRGB
// purpose  : convert CIE Lab color to RGB
// see https://www.easyrgb.com/en/math.php
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_Lab_To_LinearRGB(
  const NCollection_Vec3<float>& theLab)
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
    double afY = (aL + 16.) / 116.;
    double afX = aC * aa / 500. + afY;
    double afZ = afY - aC * ab / 200.;

    double aX = CIELab_invertf(afX) * 95.047;
    double aY = CIELab_invertf(afY) * 100.000;
    double aZ = CIELab_invertf(afZ) * 108.883;

    // convert to RGB
    // see http://www.brucelindbloom.com/index.html?Equations.html
    double aR = (aX * 3.2404542 + aY * -1.5371385 + aZ * -0.4985314) / 100.;
    double aG = (aX * -0.9692660 + aY * 1.8760108 + aZ * 0.0415560) / 100.;
    double aB = (aX * 0.0556434 + aY * -0.2040259 + aZ * 1.0572252) / 100.;

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
NCollection_Vec3<float> Quantity_Color::Convert_Lab_To_Lch(const NCollection_Vec3<float>& theLab)
{
  double aa = theLab[1];
  double ab = theLab[2];

  double aC = Sqrt(aa * aa + ab * ab);
  double aH = (aC > TheEpsilon ? ATan2(ab, aa) * 180. / M_PI : 0.);

  if (aH < 0.)
    aH += 360.;

  return NCollection_Vec3<float>(theLab[0], (float)aC, (float)aH);
}

// =======================================================================
// function : Convert_Lch_To_Lab
// purpose  : convert CIE Lch color to CIE Lab color
// see https://www.easyrgb.com/en/math.php
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_Lch_To_Lab(const NCollection_Vec3<float>& theLch)
{
  double aC = theLch[1];
  double aH = theLch[2];

  aH *= M_PI / 180.;

  double aa = aC * Cos(aH);
  double ab = aC * Sin(aH);

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
