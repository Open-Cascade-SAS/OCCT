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

#include <string.h>

#define RGBHLS_H_UNDEFINED -1.0

static Standard_Real TheEpsilon = 0.0001;

// Throw exception if RGB values are out of range.
#define Quantity_ColorValidateRgbRange(theR, theG, theB) \
  if (theR < 0.0 || theR > 1.0 \
   || theG < 0.0 || theG > 1.0 \
   || theB < 0.0 || theB > 1.0) { throw Standard_OutOfRange("Color out"); }

// Throw exception if HLS values are out of range.
#define Quantity_ColorValidateHlsRange(theH, theL, theS) \
  if ((theH < 0.0 && theH != RGBHLS_H_UNDEFINED && theS != 0.0) \
   || (theH > 360.0) \
    || theL < 0.0 || theL > 1.0 \
    || theS < 0.0 || theS > 1.0) { throw Standard_OutOfRange("Color out"); }

namespace
{
  //! Raw color for defining list of standard color
  struct Quantity_StandardColor
  {
    const char*             StringName;
    NCollection_Vec3<float> RgbValues;
    Quantity_NameOfColor    EnumName;

    Quantity_StandardColor (Quantity_NameOfColor theName, const char* theStringName, const NCollection_Vec3<float>& theVec3)
    : StringName (theStringName), RgbValues (theVec3), EnumName (theName) {}
  };
}

// Note that HTML/hex sRGB representation is ignored
#define RawColor(theName, theHex, theR, theG, theB) Quantity_StandardColor(Quantity_NOC_##theName, #theName, NCollection_Vec3<float>(theR##f, theG##f, theB##f))

//! Name list of standard materials (defined within enumeration).
static const Quantity_StandardColor THE_COLORS[] =
{
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
void Quantity_Color::SetEpsilon (const Standard_Real theEpsilon)
{
  TheEpsilon = theEpsilon;
}

// =======================================================================
// function : valuesOf
// purpose  :
// =======================================================================
NCollection_Vec3<float> Quantity_Color::valuesOf (const Quantity_NameOfColor theName,
                                                  const Quantity_TypeOfColor theType)
{
  if ((Standard_Integer )theName < 0 || (Standard_Integer )theName > Quantity_NOC_WHITE)
  {
    throw Standard_OutOfRange("Bad name");
  }

  const NCollection_Vec3<float>& anRgb = THE_COLORS[theName].RgbValues;
  switch (theType)
  {
    case Quantity_TOC_RGB: return anRgb;
    case Quantity_TOC_HLS: return Convert_sRGB_To_HLS (anRgb);
  }
  throw Standard_ProgramError("Internal error");
}

// =======================================================================
// function : StringName
// purpose  :
// =======================================================================
Standard_CString Quantity_Color::StringName (const Quantity_NameOfColor theName)
{
  if ((Standard_Integer )theName < 0 || (Standard_Integer )theName > Quantity_NOC_WHITE)
  {
    throw Standard_OutOfRange("Bad name");
  }
  return THE_COLORS[theName].StringName;
}

// =======================================================================
// function : ColorFromName
// purpose  :
// =======================================================================
Standard_Boolean Quantity_Color::ColorFromName (const Standard_CString theName,
                                                Quantity_NameOfColor&  theColor)
{
  TCollection_AsciiString aName (theName);
  aName.UpperCase();
  if (aName.Search("QUANTITY_NOC_") == 1)
  {
    aName = aName.SubString (14, aName.Length());
  }

  for (Standard_Integer anIter = Quantity_NOC_BLACK; anIter <= Quantity_NOC_WHITE; ++anIter)
  {
    Standard_CString aColorName = THE_COLORS[anIter].StringName;
    if (aName == aColorName)
    {
      theColor = (Quantity_NameOfColor )anIter;
      return Standard_True;
    }
  }

  // aliases
  if      (aName == "BLUE1")       { theColor = Quantity_NOC_BLUE1; }
  else if (aName == "CHARTREUSE1") { theColor = Quantity_NOC_CHARTREUSE1; }
  else if (aName == "CYAN1")       { theColor = Quantity_NOC_CYAN1; }
  else if (aName == "GOLD1")       { theColor = Quantity_NOC_GOLD1; }
  else if (aName == "GREEN1")      { theColor = Quantity_NOC_GREEN1; }
  else if (aName == "LIGHTCYAN1")  { theColor = Quantity_NOC_LIGHTCYAN1; }
  else if (aName == "MAGENTA1")    { theColor = Quantity_NOC_MAGENTA1; }
  else if (aName == "ORANGE1")     { theColor = Quantity_NOC_ORANGE1; }
  else if (aName == "ORANGERED1")  { theColor = Quantity_NOC_ORANGERED1; }
  else if (aName == "RED1")        { theColor = Quantity_NOC_RED1; }
  else if (aName == "TOMATO1")     { theColor = Quantity_NOC_TOMATO1; }
  else if (aName == "YELLOW1")     { theColor = Quantity_NOC_YELLOW1; }
  else
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
// function : ColorFromHex
// purpose  :
//=======================================================================
bool Quantity_Color::ColorFromHex (const Standard_CString theHexColorString,
                                   Quantity_Color& theColor)
{
  Quantity_ColorRGBA aColorRGBA;
  if (!Quantity_ColorRGBA::ColorFromHex (theHexColorString, aColorRGBA, true))
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
Quantity_Color::Quantity_Color (const Standard_Real theR1, const Standard_Real theR2, const Standard_Real theR3,
                                const Quantity_TypeOfColor theType)
{
  switch (theType)
  {
    case Quantity_TOC_RGB:
    {
      Quantity_ColorValidateRgbRange(theR1, theR2, theR3);
      myRgb.SetValues (float(theR1), float(theR2), float(theR3));
      break;
    }
    case Quantity_TOC_HLS:
    {
      Quantity_ColorValidateHlsRange(theR1, theR2, theR3);
      myRgb = Convert_HLS_To_sRGB (NCollection_Vec3<float> (float(theR1), float(theR2), float(theR3)));
      break;
    }
  }
}

// =======================================================================
// function : Quantity_Color
// purpose  :
// =======================================================================
Quantity_Color::Quantity_Color (const NCollection_Vec3<float>& theRgb)
: myRgb (theRgb)
{
  Quantity_ColorValidateRgbRange(theRgb.r(), theRgb.g(), theRgb.b());
}

// =======================================================================
// function : ChangeContrast
// purpose  :
// =======================================================================
void Quantity_Color::ChangeContrast (const Standard_Real theDelta)
{
  NCollection_Vec3<float> aHls = Convert_sRGB_To_HLS (myRgb);
  aHls[2] += aHls[2] * Standard_ShortReal (theDelta) / 100.0f; // saturation
  if (!((aHls[2] > 1.0f) || (aHls[2] < 0.0f)))
  {
    myRgb = Convert_HLS_To_sRGB (aHls);
  }
}

// =======================================================================
// function : ChangeIntensity
// purpose  :
// =======================================================================
void Quantity_Color::ChangeIntensity (const Standard_Real theDelta)
{
  NCollection_Vec3<float> aHls = Convert_sRGB_To_HLS (myRgb);
  aHls[1] += aHls[1] * Standard_ShortReal (theDelta) / 100.0f; // light
  if (!((aHls[1] > 1.0f) || (aHls[1] < 0.0f)))
  {
    myRgb = Convert_HLS_To_sRGB (aHls);
  }
}

// =======================================================================
// function : SetValues
// purpose  :
// =======================================================================
void Quantity_Color::SetValues (const Standard_Real theR1, const Standard_Real theR2, const Standard_Real theR3,
                                const Quantity_TypeOfColor theType)
{
  switch (theType)
  {
    case Quantity_TOC_RGB:
    {
      Quantity_ColorValidateRgbRange(theR1, theR2, theR3);
      myRgb.SetValues (float(theR1), float(theR2), float(theR3));
      break;
    }
    case Quantity_TOC_HLS:
    {
      Quantity_ColorValidateHlsRange(theR1, theR2, theR3);
      myRgb = Convert_HLS_To_sRGB (NCollection_Vec3<float> (float(theR1), float(theR2), float(theR3)));
      break;
    }
  }
}

// =======================================================================
// function : Delta
// purpose  :
// =======================================================================
void Quantity_Color::Delta (const Quantity_Color& theColor,
                            Standard_Real& theDC,
                            Standard_Real& theDI) const
{
  const NCollection_Vec3<float> aHls1 = Convert_sRGB_To_HLS (myRgb);
  const NCollection_Vec3<float> aHls2 = Convert_sRGB_To_HLS (theColor.myRgb);
  theDC = Standard_Real (aHls1[2] - aHls2[2]); // saturation
  theDI = Standard_Real (aHls1[1] - aHls2[1]); // light
}

// =======================================================================
// function : Name
// purpose  :
// =======================================================================
Quantity_NameOfColor Quantity_Color::Name() const
{
  Standard_ShortReal aDist2 = 4.0f;
  Quantity_NameOfColor aResName = Quantity_NOC_BLACK;
  for (Standard_Integer aColIter = Quantity_NOC_BLACK; aColIter <= Quantity_NOC_WHITE; ++aColIter)
  {
    const Standard_ShortReal aNewDist2 = (myRgb - THE_COLORS[aColIter].RgbValues).SquareModulus();
    if (aNewDist2 < aDist2)
    {
      aResName = Quantity_NameOfColor (aColIter);
      aDist2 = aNewDist2;
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
void Quantity_Color::Values (Standard_Real& theR1, Standard_Real& theR2, Standard_Real& theR3,
                             const Quantity_TypeOfColor theType) const
{
  switch (theType)
  {
    case Quantity_TOC_RGB:
    {
      theR1 = myRgb.r();
      theR2 = myRgb.g();
      theR3 = myRgb.b();
      break;
    }
    case Quantity_TOC_HLS:
    {
      const NCollection_Vec3<float> aHls = Convert_sRGB_To_HLS (myRgb);
      theR1 = aHls[0];
      theR2 = aHls[1];
      theR3 = aHls[2];
      break;
    }
  }
}

// =======================================================================
// function : Convert_HLS_To_sRGB
// purpose  : Reference: La synthese d'images, Collection Hermes
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_HLS_To_sRGB (const NCollection_Vec3<float>& theHls)
{
  float aHue = theHls[0];
  const float aLight = theHls[1];
  const float aSaturation = theHls[2];
  if (aSaturation == 0.0f
   && aHue == RGBHLS_H_UNDEFINED)
  {
    return NCollection_Vec3<float> (aLight, aLight, aLight);
  }

  int aHueIndex = 0;
  float lmuls = aLight * aSaturation;
  if (aHue == 360.0f)
  {
    aHue = 0.0;
    aHueIndex = 0;
  }
  else
  {
    aHue /= 60.0f;
    aHueIndex = (int )aHue;
  }

  switch (aHueIndex)
  {
    case 0: return NCollection_Vec3<float> (aLight,
                                            aLight - lmuls + lmuls * aHue,
                                            aLight - lmuls);
    case 1: return NCollection_Vec3<float> (aLight + lmuls - lmuls * aHue,
                                            aLight,
                                            aLight - lmuls);
    case 2: return NCollection_Vec3<float> (aLight - lmuls,
                                            aLight,
                                            aLight - 3 * lmuls + lmuls * aHue);
    case 3: return NCollection_Vec3<float> (aLight - lmuls,
                                            aLight + 3 * lmuls - lmuls * aHue,
                                            aLight);
    case 4: return NCollection_Vec3<float> (aLight - 5 * lmuls + lmuls * aHue,
                                            aLight - lmuls,
                                            aLight);
    case 5 : return NCollection_Vec3<float> (aLight,
                                             aLight - lmuls,
                                             aLight + 5 * lmuls - lmuls * aHue);
  }
  throw Standard_OutOfRange("Color out");
}

// =======================================================================
// function : Convert_sRGB_To_HLS
// purpose  : Reference: La synthese d'images, Collection Hermes
// =======================================================================
NCollection_Vec3<float> Quantity_Color::Convert_sRGB_To_HLS (const NCollection_Vec3<float>& theRgb)
{
  float aPlus = 0.0f;
  float aDiff = theRgb.g() - theRgb.b();

  // compute maximum from RGB components, which will be a luminance
  float aMax = theRgb.r();
  if (theRgb.g() > aMax) { aPlus = 2.0; aDiff = theRgb.b() - theRgb.r(); aMax = theRgb.g(); }
  if (theRgb.b() > aMax) { aPlus = 4.0; aDiff = theRgb.r() - theRgb.g(); aMax = theRgb.b(); }

  // compute minimum from RGB components
  float min = theRgb.r();
  if (theRgb.g() < min) min = theRgb.g();
  if (theRgb.b() < min) min = theRgb.b();

  const float aDelta = aMax - min;

  // compute saturation
  float aSaturation = 0.0f;
  if (aMax != 0.0f) aSaturation = aDelta / aMax;

  // compute hue
  float aHue = RGBHLS_H_UNDEFINED;
  if (aSaturation != 0.0f)
  {
    aHue = 60.0f * (aPlus + aDiff / aDelta);
    if (aHue < 0.0f) aHue += 360.0f;
  }
  return NCollection_Vec3<float> (aHue, aMax, aSaturation);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TESTS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void TestOfColor()
{
  Standard_Real H, L, S;
  Standard_Real R, G, B;
  Standard_Real DC, DI;
  Standard_Integer i;

  std::cout << "definition color tests\n----------------------\n";

  Quantity_Color C1;
  Quantity_Color C2 (Quantity_NOC_ROYALBLUE2);
  Quantity_Color C3 (Quantity_NOC_SANDYBROWN);

  // An Introduction to Standard_Object-Oriented Programming and C++ p43
  // a comment for the "const char *const" declaration
  const char *const cyan = "YELLOW";
  const char *const blue = "ROYALBLUE2";
  const char *const brown = "SANDYBROWN";

  Standard_Real RR, GG, BB;

  const Standard_Real DELTA = 1.0e-4;

  std::cout << "Get values and names of color tests\n-----------------------------------\n";

  C1.Values (R, G, B, Quantity_TOC_RGB);
  if ((R!=1.0) || (G!=1.0) || (B!=0.0))
  {
    std::cout << "TEST_ERROR : Values () bad default color\n";
    std::cout << "R, G, B values: " << R << " " << G << " " << B << "\n";
  }
  if ( (C1.Red ()!=1.0) || (C1.Green ()!=1.0) || (C1.Blue ()!=0.0) )
  {
    std::cout << "TEST_ERROR : Values () bad default color\n";
    std::cout << "R, G, B values: " << C1.Red () << " " << C1.Green () << " " << C1.Blue () << "\n";
  }
  if (strcmp (Quantity_Color::StringName (C1.Name()), cyan) != 0)
  {
    std::cout << "TEST_ERROR : StringName () " << Quantity_Color::StringName (C1.Name()) <<  " != YELLOW\n";
  }

  RR=0.262745; GG=0.431373; BB=0.933333;
  C1.SetValues (RR, GG, BB, Quantity_TOC_RGB);
  C2.Values (R, G, B, Quantity_TOC_RGB);
  if ((Abs (RR-R) > DELTA)
   || (Abs (GG-G) > DELTA)
   || (Abs (BB-B) > DELTA))
  {
    std::cout << "TEST_ERROR : Values () bad default color\n";
    std::cout << "R, G, B values: " << R << " " << G << " " << B << "\n";
  }

  if (C2 != C1)
  {
    std::cout << "TEST_ERROR : IsDifferent ()\n";
  }
  if (C3 == C1)
  {
    std::cout << "TEST_ERROR : IsEqual ()\n";
  }

  std::cout << "Distance C1,C2 " << C1.Distance (C2) << "\n";
  std::cout << "Distance C1,C3 " << C1.Distance (C3) << "\n";
  std::cout << "Distance C2,C3 " << C2.Distance (C3) << "\n";
  std::cout << "SquareDistance C1,C2 " << C1.SquareDistance (C2) << "\n";
  std::cout << "SquareDistance C1,C3 " << C1.SquareDistance (C3) << "\n";
  std::cout << "SquareDistance C2,C3 " << C2.SquareDistance (C3) << "\n";

  if (strcmp (Quantity_Color::StringName (C2.Name()), blue) != 0)
  {
    std::cout << "TEST_ERROR : StringName () " << Quantity_Color::StringName (C2.Name()) <<  " != ROYALBLUE2\n";
  }

  std::cout << "conversion rgbhls tests\n-----------------------\n";
  Quantity_Color::RgbHls (R, G, B, H, L, S);
  Quantity_Color::HlsRgb (H, L, S, R, G, B);
  RR=0.262745; GG=0.431373; BB=0.933333;
  if ((Abs (RR-R) > DELTA)
   || (Abs (GG-G) > DELTA)
   || (Abs (BB-B) > DELTA))
  {
    std::cout << "TEST_ERROR : RgbHls or HlsRgb bad conversion\n";
    std::cout << "RGB init : " << RR << " " << GG << " " << BB << "\n";
    std::cout << "RGB values : " << R << " " << G << " " << B << "\n";
    std::cout << "Difference RGB : " << RR-R << " " << GG-G << " " << BB-B << "\n";
  }

  std::cout << "distance tests\n--------------\n";
  R = (float ) 0.9568631; G = (float ) 0.6431371; B = (float ) 0.3764711;
  C2.SetValues (R, G, B, Quantity_TOC_RGB);
  if (C2.Distance (C3) > DELTA)
  {
    std::cout << "TEST_ERROR : Distance () bad result\n";
    std::cout << "Distance C2 and C3 : " << C2.Distance (C3) << "\n";
  }

  C2.Delta (C3, DC, DI);
  if (Abs (DC) > DELTA)
  {
    std::cout << "TEST_ERROR : Delta () bad result for DC\n";
  }
  if (Abs (DI) > DELTA)
  {
    std::cout << "TEST_ERROR : Delta () bad result for DI\n";
  }

  std::cout << "name tests\n----------\n";
  R = (float ) 0.9568631; G = (float ) 0.6431371; B = (float ) 0.3764711;
  C2.SetValues (R, G, B, Quantity_TOC_RGB);
  if (strcmp (Quantity_Color::StringName (C2.Name()), brown) != 0)
  {
    std::cout << "TEST_ERROR : StringName () " << Quantity_Color::StringName (C2.Name()) <<  " != SANDYBROWN\n";
  }

  std::cout << "contrast change tests\n---------------------\n";
  for (i=1; i<=10; i++)
  {
    C2.ChangeContrast (10.);
    C2.ChangeContrast (-9.09090909);
  }
  C2.Values (R, G, B, Quantity_TOC_RGB);
  RR=0.956863; GG=0.6431371; BB=0.3764711;
  if ((Abs (RR-R) > DELTA)
   || (Abs (GG-G) > DELTA)
   || (Abs (BB-B) > DELTA))
  {
    std::cout << "TEST_ERROR : ChangeContrast () bad values\n";
    std::cout << "RGB init : " << RR << " " << GG << " " << BB << "\n";
    std::cout << "RGB values : " << R << " " << G << " " << B << "\n";
  }
}

// =======================================================================
// function : Test
// purpose  :
// =======================================================================
void Quantity_Color::Test()
{
  try
  {
    OCC_CATCH_SIGNALS
    TestOfColor();
  }
  catch (Standard_Failure const& anException)
  {
    std::cout << anException << std::endl;
  }
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Quantity_Color::DumpJson (Standard_OStream& theOStream, const Standard_Integer) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, Quantity_Color);
  OCCT_DUMP_FIELD_VALUES_NUMERICAL (theOStream, "RGB", 3, myRgb.r(), myRgb.g(), myRgb.b())
}
