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

#include <Standard_ShortReal.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Vec4.hxx>
class Quantity_ColorDefinitionError;


//! This class allows the definition of a colour.
//! The names of the colours are from the X11 specification.
//! color object may be used for numerous applicative purposes.
//! A color is defined by:
//! -   its respective quantities of red, green and blue (R-G-B values), or
//! -   its hue angle and its values of lightness and  saturation (H-L-S values).
//! These two color definition systems are equivalent.
//! Use this class in conjunction with:
//! -   the Quantity_TypeOfColor enumeration
//! which identifies the color definition system you are using,
//! -   the Quantity_NameOfColor enumeration
//! which lists numerous predefined colors and
//! identifies them by their name.
class Quantity_Color 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a colour with the default value of
  //! Colour name : YELLOW
  Standard_EXPORT Quantity_Color();
  
  //! Creates the colour <AName>.
  Standard_EXPORT Quantity_Color(const Quantity_NameOfColor AName);
  
  //! Creates a color according to the definition system theType.
  //! Quantity_TOC_RGB:
  //!  - theR1 the value of Red   within range [0.0; 1.0]
  //!  - theR2 the value of Green within range [0.0; 1.0]
  //!  - theR3 the value of Blue  within range [0.0; 1.0]
  //!
  //! Quantity_TOC_HLS:
  //!  - theR1 is the Hue (H) angle in degrees within range [0.0; 360.0], 0.0 being Red.
  //!    Value -1.0 is a special value reserved for grayscale color (S should be 0.0).
  //!  - theR2 is the Lightness  (L) within range [0.0; 1.0]
  //!  - theR3 is the Saturation (S) within range [0.0; 1.0]
  Standard_EXPORT Quantity_Color (const Standard_Real theR1,
                                  const Standard_Real theR2,
                                  const Standard_Real theR3,
                                  const Quantity_TypeOfColor theType);

  //! Define color from RGB values.
  Standard_EXPORT explicit Quantity_Color (const NCollection_Vec3<float>& theRgb);

  //! Increases or decreases the contrast by <ADelta>.
  //! <ADelta> is a percentage. Any value greater than zero
  //! will increase the contrast.
  //! The variation is expressed as a percentage of the
  //! current value.
  //! It is a variation of the saturation.
  Standard_EXPORT void ChangeContrast (const Standard_Real ADelta);
  
  //! Increases or decreases the intensity by <ADelta>.
  //! <ADelta> is a percentage. Any value greater than zero
  //! will increase the intensity.
  //! The variation is expressed as a percentage of the
  //! current value.
  //! It is a variation of the lightness.
  Standard_EXPORT void ChangeIntensity (const Standard_Real ADelta);
  
  //! Updates the colour <me> from the definition of the
  //! colour <AName>.
  Standard_EXPORT void SetValues (const Quantity_NameOfColor AName);
  
  //! Updates a color according to the mode specified by theType.
  //! TOC_RGB:
  //!  - theR1 the value of Red   within range [0.0; 1.0]
  //!  - theR2 the value of Green within range [0.0; 1.0]
  //!  - theR3 the value of Blue  within range [0.0; 1.0]
  //!
  //! TOC_HLS:
  //!  - theR1 is the Hue (H) angle in degrees within range [0.0; 360.0], 0.0 being Red.
  //!    -1.0 is a special value reserved for grayscale color (S should be 0.0).
  //!  - theR2 is the Lightness  (L) within range [0.0; 1.0]
  //!  - theR3 is the Saturation (S) within range [0.0; 1.0]
  Standard_EXPORT void SetValues (const Standard_Real theR1,
                                  const Standard_Real theR2,
                                  const Standard_Real theR3,
                                  const Quantity_TypeOfColor theType);
  
  //! Returns the percentage change of contrast and intensity
  //! between <me> and <AColor>.
  //! <DC> and <DI> are percentages, either positive or negative.
  //! The calculation is with respect to the current value of <me>
  //! If <DC> is positive then <me> is more contrasty.
  //! If <DI> is positive then <me> is more intense.
  Standard_EXPORT void Delta (const Quantity_Color& AColor, Standard_Real& DC, Standard_Real& DI) const;
  
  //! Returns the distance between two colours. It's a
  //! value between 0 and the square root of 3
  //! (the black/white distance)
  Standard_EXPORT Standard_Real Distance (const Quantity_Color& AColor) const;
  
  //! Returns the square of distance between two colours.
  Standard_EXPORT Standard_Real SquareDistance (const Quantity_Color& AColor) const;
  
  //! Returns the Blue component (quantity of blue) of the color within range [0.0; 1.0].
  Standard_EXPORT Standard_Real Blue() const;
  
  //! Returns the Green component (quantity of green) of the color within range [0.0; 1.0].
  Standard_EXPORT Standard_Real Green() const;
  
  //! Returns the Hue component (hue angle) of the color
  //! in degrees within range [0.0; 360.0], 0.0 being Red.
  //! -1.0 is a special value reserved for grayscale color (S should be 0.0)
  Standard_EXPORT Standard_Real Hue() const;
  
  //! Returns Standard_True if the distance between <me> and
  //! <Other> is greater than Epsilon ().
  Standard_EXPORT Standard_Boolean IsDifferent (const Quantity_Color& Other) const;
Standard_Boolean operator != (const Quantity_Color& Other) const
{
  return IsDifferent(Other);
}
  
  //! Returns true if the Other color is
  //! -   different from, or
  //! -   equal to this color.
  //! Two colors are considered to be equal if their
  //! distance is no greater than Epsilon().
  //! These methods are aliases of operator != and operator ==.
  Standard_EXPORT Standard_Boolean IsEqual (const Quantity_Color& Other) const;
Standard_Boolean operator == (const Quantity_Color& Other) const
{
  return IsEqual(Other);
}
  
  //! Returns the Light component (value of the lightness) of the color within range [0.0; 1.0].
  Standard_EXPORT Standard_Real Light() const;
  
  //! Returns the name of the color defined by its
  //! quantities of red R, green G and blue B; more
  //! precisely this is the nearest color from the
  //! Quantity_NameOfColor enumeration.
  //! Exceptions
  //! Standard_OutOfRange if R, G or B is less than 0. or greater than 1.
  Standard_EXPORT Quantity_NameOfColor Name() const;
  
  //! Returns the Red component (quantity of red) of the color within range [0.0; 1.0].
  Standard_EXPORT Standard_Real Red() const;
  
  //! Returns the Saturation component (value of the saturation) of the color within range [0.0; 1.0].
  Standard_EXPORT Standard_Real Saturation() const;

  //! Return the color as vector of 3 float elements.
  operator const NCollection_Vec3<float>&() const { return *(const NCollection_Vec3<float>* )this; }

  //! Returns in theR1, theR2 and theR3 the components of this color according to the color system definition theType.
  //! If theType is Quantity_TOC_RGB:
  //!  - theR1 the value of Red   between 0.0 and 1.0
  //!  - theR2 the value of Green between 0.0 and 1.0
  //!  - theR3 the value of Blue  between 0.0 and 1.0
  //! If theType is Quantity_TOC_HLS:
  //!  - theR1 is the Hue (H) angle in degrees within range [0.0; 360.0], 0.0 being Red.
  //!    -1.0 is a special value reserved for grayscale color (S should be 0.0).
  //!  - theR2 is the Lightness  (L) within range [0.0; 1.0]
  //!  - theR3 is the Saturation (S) within range [0.0; 1.0]
  Standard_EXPORT void Values (Standard_Real& theR1,
                               Standard_Real& theR2,
                               Standard_Real& theR3,
                               const Quantity_TypeOfColor theType) const;
  
  //! Sets the specified value used to compare <me> and
  //! an other color in IsDifferent and in IsEqual methods.
  //! Warning: The default value is 0.0001
  Standard_EXPORT static void SetEpsilon (const Standard_Real AnEpsilon);
  
  //! Returns the specified value used to compare <me> and
  //! an other color in IsDifferent and in IsEqual methods.
  Standard_EXPORT static Standard_Real Epsilon();
  
  //! Returns the name of the colour for which the RGB components
  //! are nearest to <R>, <G> and <B>.
  Standard_EXPORT static Quantity_NameOfColor Name (const Standard_Real R, const Standard_Real G, const Standard_Real B);
  
  //! Returns the name of the color identified by
  //! AName in the Quantity_NameOfColor enumeration.
  //! For example, the name of the color which
  //! corresponds to Quantity_NOC_BLACK is "BLACK".
  //! Exceptions
  //! Standard_OutOfRange if AName in not known
  //! in the Quantity_NameOfColor enumeration.
  Standard_EXPORT static Standard_CString StringName (const Quantity_NameOfColor AColor);
  
  //! Finds color from predefined names.
  //! For example, the name of the color which
  //! corresponds to "BLACK" is Quantity_NOC_BLACK.
  //! Returns false if name is unknown.
  Standard_EXPORT static Standard_Boolean ColorFromName (const Standard_CString theName, Quantity_NameOfColor& theColor);
  
  //! Converts HLS components into RGB ones.
  Standard_EXPORT static void HlsRgb (const Standard_Real H, const Standard_Real L, const Standard_Real S, Standard_Real& R, Standard_Real& G, Standard_Real& B);
  
  //! Converts RGB components into HLS ones.
  Standard_EXPORT static void RgbHls (const Standard_Real R, const Standard_Real G, const Standard_Real B, Standard_Real& H, Standard_Real& L, Standard_Real& S);
  
  //! Convert the Color value to ARGB integer value.
  //! theARGB has Alpha equal to zero, so the output is
  //! formatted as 0x00RRGGBB
  Standard_EXPORT static void Color2argb (const Quantity_Color& theColor, Standard_Integer& theARGB);
  
  //! Convert integer ARGB value to Color. Alpha bits are ignored
  Standard_EXPORT static void Argb2color (const Standard_Integer theARGB, Quantity_Color& theColor);
  
  //! Internal test
  Standard_EXPORT static void Test();

private:

  //! Converts HLS components into RGB ones.
  Standard_EXPORT static void hlsrgb (const Standard_ShortReal H, const Standard_ShortReal L, const Standard_ShortReal S, Standard_ShortReal& R, Standard_ShortReal& G, Standard_ShortReal& B);
  
  //! Converts RGB components into HLS ones.
  Standard_EXPORT static void rgbhls (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B, Standard_ShortReal& H, Standard_ShortReal& L, Standard_ShortReal& S);
  
  //! Returns the values of a predefined colour according to
  //! the mode specified by TypeOfColor
  //! TOC_RGB : <R1> the value of red between 0. and 1.
  //! <R2> the value of green between 0. and 1.
  //! <R3> the value of blue between 0. and 1.
  //!
  //! TOC_HLS : <R1> is the hue angle in degrees, 0. being red
  //! <R2> is the lightness between 0. and 1.
  //! <R3> is the saturation between 0. and 1.
  Standard_EXPORT static void ValuesOf (const Quantity_NameOfColor AName, const Quantity_TypeOfColor AType, Standard_ShortReal& R1, Standard_ShortReal& R2, Standard_ShortReal& R3);


  Standard_ShortReal MyRed;
  Standard_ShortReal MyGreen;
  Standard_ShortReal MyBlue;

};

#endif // _Quantity_Color_HeaderFile
