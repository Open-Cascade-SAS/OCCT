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

#include <Quantity_ColorDefinitionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_AsciiString.hxx>

#include <string.h>
// for Test method (suite et fin)
void call_hlsrgb(float h, float l, float s, float& r, float& g, float& b);
void call_rgbhls(float r, float g, float b, float& h, float& l, float& s);

//-Aliases

//-Global data definitions

#define RGBHLS_H_UNDEFINED -1.0

static Standard_Real TheEpsilon = 0.0001;

//	-- les composantes RGB
//	MyRed		:	Standard_ShortReal;
//	MyGreen		:	Standard_ShortReal;
//	MyBlue		:	Standard_ShortReal;

//-Constructors

//-Destructors

//-Methods, in order

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
    Standard_CString aColorName = Quantity_Color::StringName (Quantity_NameOfColor (anIter));
    if (aName == aColorName)
    {
      theColor = (Quantity_NameOfColor )anIter;
      return Standard_True;
    }
  }
  return Standard_False;
}

Quantity_Color::Quantity_Color () {

	Quantity_Color::ValuesOf
		(Quantity_NOC_YELLOW, Quantity_TOC_RGB, MyRed, MyGreen, MyBlue);
}

Quantity_Color::Quantity_Color (const Quantity_NameOfColor AName) {

	Quantity_Color::ValuesOf
		(AName, Quantity_TOC_RGB, MyRed, MyGreen, MyBlue);
}

Quantity_Color::Quantity_Color (const Standard_Real R1, const Standard_Real R2, const Standard_Real R3, const Quantity_TypeOfColor AType) {

	switch (AType) {

		case Quantity_TOC_RGB :
			if ( R1 < 0. || R1 > 1. ||
			     R2 < 0. || R2 > 1. ||
			     R3 < 0. || R3 > 1. )
				throw Standard_OutOfRange("Color out");
			MyRed	= Standard_ShortReal (R1);
			MyGreen	= Standard_ShortReal (R2);
			MyBlue	= Standard_ShortReal (R3);
		break;

		case Quantity_TOC_HLS :
			if ( (R1 < 0. && R1 != RGBHLS_H_UNDEFINED && R3 != 0.0)
			  || (R1 > 360.) ||
			     R2 < 0. || R2 > 1. ||
			     R3 < 0. || R3 > 1. )
				throw Standard_OutOfRange("Color out");
			Quantity_Color::hlsrgb
				(Standard_ShortReal (R1),
				 Standard_ShortReal (R2),
				 Standard_ShortReal (R3),
				 MyRed, MyGreen, MyBlue);
		break;
	}

}

Quantity_Color::Quantity_Color (const NCollection_Vec3<float>& theRgb)
: MyRed  (theRgb.r()),
  MyGreen(theRgb.g()),
  MyBlue (theRgb.b())
{
  if (theRgb.r() < 0.0f || theRgb.r() > 1.0f
   || theRgb.g() < 0.0f || theRgb.g() > 1.0f
   || theRgb.b() < 0.0f || theRgb.b() > 1.0f)
  {
    throw Standard_OutOfRange("Color out");
  }
}

void Quantity_Color::ChangeContrast (const Standard_Real ADelta) {

Standard_ShortReal MyHue, MyLight, MySaturation;

	Quantity_Color::rgbhls
			(MyRed, MyGreen, MyBlue, MyHue, MyLight, MySaturation);

	MySaturation	+= MySaturation * Standard_ShortReal (ADelta) / (float ) 100.0;

	if ( !((MySaturation > 1.0) || (MySaturation < 0.0) )) {
		Quantity_Color::hlsrgb
			(MyHue, MyLight, MySaturation, MyRed, MyGreen, MyBlue);
	}
}

void Quantity_Color::ChangeIntensity (const Standard_Real ADelta) {

Standard_ShortReal MyHue, MyLight, MySaturation;

	Quantity_Color::rgbhls
			(MyRed, MyGreen, MyBlue, MyHue, MyLight, MySaturation);


	MyLight		+= MyLight * Standard_ShortReal (ADelta) / (float ) 100.0;
	if ( !((MyLight > 1.0) || (MyLight < 0.0) )) {
		Quantity_Color::hlsrgb
			(MyHue, MyLight, MySaturation, MyRed, MyGreen, MyBlue);
	}

}

void Quantity_Color::SetValues (const Quantity_NameOfColor AName) {

	Quantity_Color::ValuesOf
		(AName, Quantity_TOC_RGB, MyRed, MyGreen, MyBlue);

}

void Quantity_Color::SetValues (const Standard_Real R1, const Standard_Real R2, const Standard_Real R3, const Quantity_TypeOfColor AType) {

	switch (AType) {

		case Quantity_TOC_RGB :
			if ( R1 < 0. || R1 > 1. ||
			     R2 < 0. || R2 > 1. ||
			     R3 < 0. || R3 > 1. )
				throw Standard_OutOfRange("Color out");
			else {
				MyRed	= Standard_ShortReal (R1);
				MyGreen	= Standard_ShortReal (R2);
				MyBlue	= Standard_ShortReal (R3);
			}
		break;

		case Quantity_TOC_HLS :
			if ( R1 < 0. || R1 > 360. ||
			     R2 < 0. || R2 > 1. ||
			     R3 < 0. || R3 > 1. )
				throw Standard_OutOfRange("Color out");
			else {
				Quantity_Color::hlsrgb
					(Standard_ShortReal (R1),
					 Standard_ShortReal (R2),
					 Standard_ShortReal (R3),
					 MyRed, MyGreen, MyBlue);
			}
		break;
	}

}

Standard_Real Quantity_Color::Distance (const Quantity_Color& AColor) const {

Standard_Real R1, G1, B1;
Standard_Real R2, G2, B2;

	R1	= Standard_Real (MyRed);
	G1	= Standard_Real (MyGreen);
	B1	= Standard_Real (MyBlue);

	R2	= Standard_Real (AColor.MyRed);
	G2	= Standard_Real (AColor.MyGreen);
	B2	= Standard_Real (AColor.MyBlue);

	return	Sqrt (
			((R1 - R2) * (R1 - R2)) +
			((G1 - G2) * (G1 - G2)) +
			((B1 - B2) * (B1 - B2))
		     );

}

Standard_Real Quantity_Color::SquareDistance (const Quantity_Color& AColor) const {

Standard_Real R1, G1, B1;
Standard_Real R2, G2, B2;

	R1	= Standard_Real (MyRed);
	G1	= Standard_Real (MyGreen);
	B1	= Standard_Real (MyBlue);

	R2	= Standard_Real (AColor.MyRed);
	G2	= Standard_Real (AColor.MyGreen);
	B2	= Standard_Real (AColor.MyBlue);

	return	(
			((R1 - R2) * (R1 - R2)) +
			((G1 - G2) * (G1 - G2)) +
			((B1 - B2) * (B1 - B2))
		);

}

void Quantity_Color::Delta (const Quantity_Color& AColor, Standard_Real& DC, Standard_Real& DI) const {

Standard_ShortReal MyHue, MyLight, MySaturation;

	Quantity_Color::rgbhls
			(MyRed, MyGreen, MyBlue, MyHue, MyLight, MySaturation);

	DC	= Standard_Real (MySaturation	- AColor.Saturation ());
	DI	= Standard_Real (MyLight		- AColor.Light ());

}

Standard_Boolean Quantity_Color::IsDifferent (const Quantity_Color& Other) const {

	return (Distance (Other) > Epsilon ());

}

Standard_Boolean Quantity_Color::IsEqual (const Quantity_Color& Other) const {

	return (Distance (Other) <= Epsilon ());

}

Quantity_NameOfColor Quantity_Color::Name () const {

Quantity_NameOfColor MyNewName = Quantity_NOC_BLACK;	// Nom courant.
Quantity_Color Col;				// Couleur courante.
Standard_Real NewDist = 4.0;			// Distance courante.
Standard_Real Dist = 4.0;			// Distance la plus courte.

Standard_Integer Begin, End, Current;

		//
		// Parcours de l'enumeration
		//
		Begin	= Standard_Integer (Quantity_NOC_BLACK);
		End	= Standard_Integer (Quantity_NOC_WHITE);

		for (Current = Begin;
			((Current <= End) && (NewDist != 0.0)); Current++) {

			Col.SetValues (Quantity_NameOfColor (Current));
			NewDist = 
			((MyRed   - Col.MyRed)   * (MyRed - Col.MyRed))     +
			((MyGreen - Col.MyGreen) * (MyGreen - Col.MyGreen)) +
			((MyBlue  - Col.MyBlue)  * (MyBlue - Col.MyBlue));

			if (NewDist < Dist) {
				MyNewName = Quantity_NameOfColor (Current);
				Dist = NewDist;
			}
		} // Fin de boucle for

		return (MyNewName);

}

Standard_Real Quantity_Color::Epsilon()
{
  return TheEpsilon;
}

void Quantity_Color::SetEpsilon (const Standard_Real AnEpsilon) {

	TheEpsilon	= AnEpsilon;

}

Quantity_NameOfColor Quantity_Color::Name (const Standard_Real R, const Standard_Real G, const Standard_Real B) {

	if ( R < 0. || R > 1. ||
	     G < 0. || G > 1. ||
	     B < 0. || B > 1. )
		throw Standard_OutOfRange("Color out");

Quantity_Color AColor (R, G, B, Quantity_TOC_RGB);	// Couleur definie en RGB.

	return (AColor.Name ());

}

Standard_Real Quantity_Color::Red () const {

	return (Standard_Real (MyRed));

}

Standard_Real Quantity_Color::Green () const {

	return (Standard_Real (MyGreen));

}

Standard_Real Quantity_Color::Blue () const {

	return (Standard_Real (MyBlue));

}

Standard_Real Quantity_Color::Hue () const {

Standard_ShortReal MyHue, MyLight, MySaturation;

	Quantity_Color::rgbhls
			(MyRed, MyGreen, MyBlue, MyHue, MyLight, MySaturation);

	return (Standard_Real (MyHue));

}

Standard_Real Quantity_Color::Light () const {

Standard_ShortReal MyHue, MyLight, MySaturation;

	Quantity_Color::rgbhls
			(MyRed, MyGreen, MyBlue, MyHue, MyLight, MySaturation);

	return (Standard_Real (MyLight));

}

Standard_Real Quantity_Color::Saturation () const {

Standard_ShortReal MyHue, MyLight, MySaturation;

	Quantity_Color::rgbhls
			(MyRed, MyGreen, MyBlue, MyHue, MyLight, MySaturation);

	return (Standard_Real (MySaturation));

}

void Quantity_Color::Values (Standard_Real& R1, Standard_Real& R2, Standard_Real& R3, const Quantity_TypeOfColor AType) const {


	switch (AType) {

		case Quantity_TOC_RGB :
			R1	= Standard_Real (MyRed);
			R2	= Standard_Real (MyGreen);
			R3	= Standard_Real (MyBlue);
		break;

		case Quantity_TOC_HLS :
		      { Standard_ShortReal MyHue, MyLight, MySaturation;

			Quantity_Color::rgbhls
			 (MyRed, MyGreen, MyBlue, MyHue, MyLight, MySaturation);

			R1	= Standard_Real (MyHue);
			R2	= Standard_Real (MyLight);
			R3	= Standard_Real (MySaturation);
		      }
		break;
	}

}

void Quantity_Color::RgbHls (const Standard_Real R, const Standard_Real G, const Standard_Real B, Standard_Real& H, Standard_Real& L, Standard_Real& S) {

Standard_ShortReal HH, LL, SS;

	call_rgbhls (Standard_ShortReal (R),
			Standard_ShortReal (G),
			Standard_ShortReal (B),
			HH, LL, SS);

	H	= Standard_Real (HH);
	L	= Standard_Real (LL);
	S	= Standard_Real (SS);

}

void Quantity_Color::HlsRgb (const Standard_Real H, const Standard_Real L, const Standard_Real S, Standard_Real& R, Standard_Real& G, Standard_Real& B) {

Standard_ShortReal RR, GG, BB;

	call_hlsrgb (Standard_ShortReal (H),
			Standard_ShortReal (L),
			Standard_ShortReal (S),
			RR, GG, BB);

	R	= Standard_Real (RR);
	G	= Standard_Real (GG);
	B	= Standard_Real (BB);

}

void Quantity_Color::rgbhls (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B, Standard_ShortReal& H, Standard_ShortReal& L, Standard_ShortReal& S) {

	call_rgbhls (R, G, B, H, L, S);

}

void Quantity_Color::hlsrgb (const Standard_ShortReal H, const Standard_ShortReal L, const Standard_ShortReal S, Standard_ShortReal& R, Standard_ShortReal& G, Standard_ShortReal& B) {

	call_hlsrgb (H, L, S, R, G, B);

}

//=======================================================================
//function : Color2argb
//purpose  : 
//=======================================================================

void Quantity_Color::Color2argb (const Quantity_Color& theColor,
                                 Standard_Integer& theARGB)
{
  const Standard_Integer aColor[3] = {
    static_cast<Standard_Integer> (255 * theColor.MyRed),
    static_cast<Standard_Integer> (255 * theColor.MyGreen),
    static_cast<Standard_Integer> (255 * theColor.MyBlue)
  };
  theARGB = (((aColor[0] & 0xff) << 16) |
             ((aColor[1] & 0xff) << 8)  |
             (aColor[2] & 0xff));
}

//=======================================================================
//function : Argb2color
//purpose  : 
//=======================================================================

void Quantity_Color::Argb2color (const Standard_Integer theARGB,
                                 Quantity_Color& theColor)
{
  const Standard_Real aColor[3] = {
    static_cast <Standard_Real> ((theARGB & 0xff0000) >> 16),
    static_cast <Standard_Real> ((theARGB & 0x00ff00) >> 8),
    static_cast <Standard_Real> ((theARGB & 0x0000ff))
  };
  theColor.SetValues(aColor[0] / 255.0, aColor[1] / 255.0, aColor[2] / 255.0,
                     Quantity_TOC_RGB);
}

void Quantity_Color::ValuesOf (const Quantity_NameOfColor AName, const Quantity_TypeOfColor AType, Standard_ShortReal& R1, Standard_ShortReal& R2, Standard_ShortReal& R3) {

Standard_ShortReal RR = 0 ;
Standard_ShortReal RG = 0 ;
Standard_ShortReal RB = 0 ;

switch (AName) {

	case Quantity_NOC_BLACK :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_MATRAGRAY :
		RR = R1 = (float ) 0.600000; RG = R2 = (float ) 0.600000; RB = R3 = (float ) 0.600000;
	break;
	case Quantity_NOC_MATRABLUE :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = 0.750000;
	break;
	case Quantity_NOC_ALICEBLUE :
		RR = R1 = (float ) 0.941176; RG = R2 = (float ) 0.972549; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_ANTIQUEWHITE :
		RR = R1 = (float ) 0.980392; RG = R2 = (float ) 0.921569; RB = R3 = (float ) 0.843137;
	break;
	case Quantity_NOC_ANTIQUEWHITE1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.937255; RB = R3 = (float ) 0.858824;
	break;
	case Quantity_NOC_ANTIQUEWHITE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.874510; RB = R3 = (float ) 0.800000;
	break;
	case Quantity_NOC_ANTIQUEWHITE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.752941; RB = R3 = (float ) 0.690196;
	break;
	case Quantity_NOC_ANTIQUEWHITE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.513725; RB = R3 = (float ) 0.470588;
	break;
	case Quantity_NOC_AQUAMARINE1 :
		RR = R1 = (float ) 0.498039; RG = R2 = 1.000000; RB = R3 = (float ) 0.831373;
	break;
	case Quantity_NOC_AQUAMARINE2 :
		RR = R1 = (float ) 0.462745; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.776471;
	break;
	case Quantity_NOC_AQUAMARINE4 :
		RR = R1 = (float ) 0.270588; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.454902;
	break;
	case Quantity_NOC_AZURE :
		RR = R1 = (float ) 0.941176; RG = R2 = 1.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_AZURE2 :
		RR = R1 = (float ) 0.878431; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_AZURE3 :
		RR = R1 = (float ) 0.756863; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_AZURE4 :
		RR = R1 = (float ) 0.513725; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_BEIGE :
		RR = R1 = (float ) 0.960784; RG = R2 = (float ) 0.960784; RB = R3 = (float ) 0.862745;
	break;
	case Quantity_NOC_BISQUE :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.894118; RB = R3 = (float ) 0.768627;
	break;
	case Quantity_NOC_BISQUE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.835294; RB = R3 = (float ) 0.717647;
	break;
	case Quantity_NOC_BISQUE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.717647; RB = R3 = (float ) 0.619608;
	break;
	case Quantity_NOC_BISQUE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.490196; RB = R3 = (float ) 0.419608;
	break;
	case Quantity_NOC_BLANCHEDALMOND :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.921569; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_BLUE1 :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_BLUE2 :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_BLUE3 :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_BLUE4 :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_BLUEVIOLET :
		RR = R1 = (float ) 0.541176; RG = R2 = (float ) 0.168627; RB = R3 = (float ) 0.886275;
	break;
	case Quantity_NOC_BROWN :
		RR = R1 = (float ) 0.647059; RG = R2 = (float ) 0.164706; RB = R3 = (float ) 0.164706;
	break;
	case Quantity_NOC_BROWN1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.250980; RB = R3 = (float ) 0.250980;
	break;
	case Quantity_NOC_BROWN2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.231373; RB = R3 = (float ) 0.231373;
	break;
	case Quantity_NOC_BROWN3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.200000; RB = R3 = (float ) 0.200000;
	break;
	case Quantity_NOC_BROWN4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.137255; RB = R3 = (float ) 0.137255;
	break;
	case Quantity_NOC_BURLYWOOD :
		RR = R1 = (float ) 0.870588; RG = R2 = (float ) 0.721569; RB = R3 = (float ) 0.529412;
	break;
	case Quantity_NOC_BURLYWOOD1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.827451; RB = R3 = (float ) 0.607843;
	break;
	case Quantity_NOC_BURLYWOOD2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.772549; RB = R3 = (float ) 0.568627;
	break;
	case Quantity_NOC_BURLYWOOD3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.666667; RB = R3 = (float ) 0.490196;
	break;
	case Quantity_NOC_BURLYWOOD4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.450980; RB = R3 = (float ) 0.333333;
	break;
	case Quantity_NOC_CADETBLUE :
		RR = R1 = (float ) 0.372549; RG = R2 = (float ) 0.619608; RB = R3 = (float ) 0.627451;
	break;
	case Quantity_NOC_CADETBLUE1 :
		RR = R1 = (float ) 0.596078; RG = R2 = (float ) 0.960784; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_CADETBLUE2 :
		RR = R1 = (float ) 0.556863; RG = R2 = (float ) 0.898039; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_CADETBLUE3 :
		RR = R1 = (float ) 0.478431; RG = R2 = (float ) 0.772549; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_CADETBLUE4 :
		RR = R1 = (float ) 0.325490; RG = R2 = (float ) 0.525490; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_CHARTREUSE :
		RR = R1 = (float ) 0.498039; RG = R2 = 1.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_CHARTREUSE1 :
		RR = R1 = (float ) 0.498039; RG = R2 = 1.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_CHARTREUSE2 :
		RR = R1 = (float ) 0.462745; RG = R2 = (float ) 0.933333; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_CHARTREUSE3 :
		RR = R1 = (float ) 0.400000; RG = R2 = (float ) 0.803922; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_CHARTREUSE4 :
		RR = R1 = (float ) 0.270588; RG = R2 = (float ) 0.545098; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_CHOCOLATE :
		RR = R1 = (float ) 0.823529; RG = R2 = (float ) 0.411765; RB = R3 = (float ) 0.117647;
	break;
	case Quantity_NOC_CHOCOLATE1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.498039; RB = R3 = (float ) 0.141176;
	break;
	case Quantity_NOC_CHOCOLATE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.462745; RB = R3 = (float ) 0.129412;
	break;
	case Quantity_NOC_CHOCOLATE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.400000; RB = R3 = (float ) 0.113725;
	break;
	case Quantity_NOC_CHOCOLATE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.270588; RB = R3 = (float ) 0.074510;
	break;
	case Quantity_NOC_CORAL :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.498039; RB = R3 = (float ) 0.313725;
	break;
	case Quantity_NOC_CORAL1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.447059; RB = R3 = (float ) 0.337255;
	break;
	case Quantity_NOC_CORAL2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.415686; RB = R3 = (float ) 0.313725;
	break;
	case Quantity_NOC_CORAL3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.356863; RB = R3 = (float ) 0.270588;
	break;
	case Quantity_NOC_CORAL4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.243137; RB = R3 = (float ) 0.184314;
	break;
	case Quantity_NOC_CORNFLOWERBLUE :
		RR = R1 = (float ) 0.392157; RG = R2 = (float ) 0.584314; RB = R3 = (float ) 0.929412;
	break;
	case Quantity_NOC_CORNSILK1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.972549; RB = R3 = (float ) 0.862745;
	break;
	case Quantity_NOC_CORNSILK2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.909804; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_CORNSILK3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.784314; RB = R3 = (float ) 0.694118;
	break;
	case Quantity_NOC_CORNSILK4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.533333; RB = R3 = (float ) 0.470588;
	break;
	case Quantity_NOC_CYAN1 :
		RR = R1 = 0.000000; RG = R2 = 1.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_CYAN2 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_CYAN3 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_CYAN4 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_DARKGOLDENROD :
		RR = R1 = (float ) 0.721569; RG = R2 = (float ) 0.525490; RB = R3 = (float ) 0.043137;
	break;
	case Quantity_NOC_DARKGOLDENROD1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.725490; RB = R3 = (float ) 0.058824;
	break;
	case Quantity_NOC_DARKGOLDENROD2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.678431; RB = R3 = (float ) 0.054902;
	break;
	case Quantity_NOC_DARKGOLDENROD3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.584314; RB = R3 = (float ) 0.047059;
	break;
	case Quantity_NOC_DARKGOLDENROD4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.396078; RB = R3 = (float ) 0.031373;
	break;
	case Quantity_NOC_DARKGREEN :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.392157; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_DARKKHAKI :
		RR = R1 = (float ) 0.741176; RG = R2 = (float ) 0.717647; RB = R3 = (float ) 0.419608;
	break;
	case Quantity_NOC_DARKOLIVEGREEN :
		RR = R1 = (float ) 0.333333; RG = R2 = (float ) 0.419608; RB = R3 = (float ) 0.184314;
	break;
	case Quantity_NOC_DARKOLIVEGREEN1 :
		RR = R1 = (float ) 0.792157; RG = R2 = 1.000000; RB = R3 = (float ) 0.439216;
	break;
	case Quantity_NOC_DARKOLIVEGREEN2 :
		RR = R1 = (float ) 0.737255; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.407843;
	break;
	case Quantity_NOC_DARKOLIVEGREEN3 :
		RR = R1 = (float ) 0.635294; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.352941;
	break;
	case Quantity_NOC_DARKOLIVEGREEN4 :
		RR = R1 = (float ) 0.431373; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.239216;
	break;
	case Quantity_NOC_DARKORANGE :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.549020; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_DARKORANGE1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.498039; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_DARKORANGE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.462745; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_DARKORANGE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.400000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_DARKORANGE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.270588; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_DARKORCHID :
		RR = R1 = (float ) 0.600000; RG = R2 = (float ) 0.196078; RB = R3 = (float ) 0.800000;
	break;
	case Quantity_NOC_DARKORCHID1 :
		RR = R1 = (float ) 0.749020; RG = R2 = (float ) 0.243137; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_DARKORCHID2 :
		RR = R1 = (float ) 0.698039; RG = R2 = (float ) 0.227451; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_DARKORCHID3 :
		RR = R1 = (float ) 0.603922; RG = R2 = (float ) 0.196078; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_DARKORCHID4 :
		RR = R1 = (float ) 0.407843; RG = R2 = (float ) 0.133333; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_DARKSALMON :
		RR = R1 = (float ) 0.913725; RG = R2 = (float ) 0.588235; RB = R3 = (float ) 0.478431;
	break;
	case Quantity_NOC_DARKSEAGREEN :
		RR = R1 = (float ) 0.560784; RG = R2 = (float ) 0.737255; RB = R3 = (float ) 0.560784;
	break;
	case Quantity_NOC_DARKSEAGREEN1 :
		RR = R1 = (float ) 0.756863; RG = R2 = 1.000000; RB = R3 = (float ) 0.756863;
	break;
	case Quantity_NOC_DARKSEAGREEN2 :
		RR = R1 = (float ) 0.705882; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.705882;
	break;
	case Quantity_NOC_DARKSEAGREEN3 :
		RR = R1 = (float ) 0.607843; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.607843;
	break;
	case Quantity_NOC_DARKSEAGREEN4 :
		RR = R1 = (float ) 0.411765; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.411765;
	break;
	case Quantity_NOC_DARKSLATEBLUE :
		RR = R1 = (float ) 0.282353; RG = R2 = (float ) 0.239216; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_DARKSLATEGRAY1 :
		RR = R1 = (float ) 0.592157; RG = R2 = 1.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_DARKSLATEGRAY2 :
		RR = R1 = (float ) 0.552941; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_DARKSLATEGRAY3 :
		RR = R1 = (float ) 0.474510; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_DARKSLATEGRAY4 :
		RR = R1 = (float ) 0.321569; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_DARKSLATEGRAY :
		RR = R1 = (float ) 0.184314; RG = R2 = (float ) 0.309804; RB = R3 = (float ) 0.309804;
	break;
	case Quantity_NOC_DARKTURQUOISE :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.807843; RB = R3 = (float ) 0.819608;
	break;
	case Quantity_NOC_DARKVIOLET :
		RR = R1 = (float ) 0.580392; RG = R2 = 0.000000; RB = R3 = (float ) 0.827451;
	break;
	case Quantity_NOC_DEEPPINK :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.078431; RB = R3 = (float ) 0.576471;
	break;
	case Quantity_NOC_DEEPPINK2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.070588; RB = R3 = (float ) 0.537255;
	break;
	case Quantity_NOC_DEEPPINK3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.062745; RB = R3 = (float ) 0.462745;
	break;
	case Quantity_NOC_DEEPPINK4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.039216; RB = R3 = (float ) 0.313725;
	break;
	case Quantity_NOC_DEEPSKYBLUE1 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.749020; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_DEEPSKYBLUE2 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.698039; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_DEEPSKYBLUE3 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.603922; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_DEEPSKYBLUE4 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.407843; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_DODGERBLUE1 :
		RR = R1 = (float ) 0.117647; RG = R2 = (float ) 0.564706; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_DODGERBLUE2 :
		RR = R1 = (float ) 0.109804; RG = R2 = (float ) 0.525490; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_DODGERBLUE3 :
		RR = R1 = (float ) 0.094118; RG = R2 = (float ) 0.454902; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_DODGERBLUE4 :
		RR = R1 = (float ) 0.062745; RG = R2 = (float ) 0.305882; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_FIREBRICK :
		RR = R1 = (float ) 0.698039; RG = R2 = (float ) 0.133333; RB = R3 = (float ) 0.133333;
	break;
	case Quantity_NOC_FIREBRICK1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.188235; RB = R3 = (float ) 0.188235;
	break;
	case Quantity_NOC_FIREBRICK2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.172549; RB = R3 = (float ) 0.172549;
	break;
	case Quantity_NOC_FIREBRICK3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.149020; RB = R3 = (float ) 0.149020;
	break;
	case Quantity_NOC_FIREBRICK4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.101961; RB = R3 = (float ) 0.101961;
	break;
	case Quantity_NOC_FLORALWHITE :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.980392; RB = R3 = (float ) 0.941176;
	break;
	case Quantity_NOC_FORESTGREEN :
		RR = R1 = (float ) 0.133333; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.133333;
	break;
	case Quantity_NOC_GAINSBORO :
		RR = R1 = (float ) 0.862745; RG = R2 = (float ) 0.862745; RB = R3 = (float ) 0.862745;
	break;
	case Quantity_NOC_GHOSTWHITE :
		RR = R1 = (float ) 0.972549; RG = R2 = (float ) 0.972549; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_GOLD :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.843137; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GOLD1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.843137; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GOLD2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.788235; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GOLD3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.678431; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GOLD4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.458824; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GOLDENROD :
		RR = R1 = (float ) 0.854902; RG = R2 = (float ) 0.647059; RB = R3 = (float ) 0.125490;
	break;
	case Quantity_NOC_GOLDENROD1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.756863; RB = R3 = (float ) 0.145098;
	break;
	case Quantity_NOC_GOLDENROD2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.705882; RB = R3 = (float ) 0.133333;
	break;
	case Quantity_NOC_GOLDENROD3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.607843; RB = R3 = (float ) 0.113725;
	break;
	case Quantity_NOC_GOLDENROD4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.411765; RB = R3 = (float ) 0.078431;
	break;
	case Quantity_NOC_GRAY :
		RR = R1 = (float ) 0.752941; RG = R2 = (float ) 0.752941; RB = R3 = (float ) 0.752941;
	break;
	case Quantity_NOC_GRAY0 :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GRAY1 :
		RR = R1 = (float ) 0.011765; RG = R2 = (float ) 0.011765; RB = R3 = (float ) 0.011765;
	break;
	case Quantity_NOC_GRAY10 :
		RR = R1 = (float ) 0.101961; RG = R2 = (float ) 0.101961; RB = R3 = (float ) 0.101961;
	break;
	case Quantity_NOC_GRAY11 :
		RR = R1 = (float ) 0.109804; RG = R2 = (float ) 0.109804; RB = R3 = (float ) 0.109804;
	break;
	case Quantity_NOC_GRAY12 :
		RR = R1 = (float ) 0.121569; RG = R2 = (float ) 0.121569; RB = R3 = (float ) 0.121569;
	break;
	case Quantity_NOC_GRAY13 :
		RR = R1 = (float ) 0.129412; RG = R2 = (float ) 0.129412; RB = R3 = (float ) 0.129412;
	break;
	case Quantity_NOC_GRAY14 :
		RR = R1 = (float ) 0.141176; RG = R2 = (float ) 0.141176; RB = R3 = (float ) 0.141176;
	break;
	case Quantity_NOC_GRAY15 :
		RR = R1 = (float ) 0.149020; RG = R2 = (float ) 0.149020; RB = R3 = (float ) 0.149020;
	break;
	case Quantity_NOC_GRAY16 :
		RR = R1 = (float ) 0.160784; RG = R2 = (float ) 0.160784; RB = R3 = (float ) 0.160784;
	break;
	case Quantity_NOC_GRAY17 :
		RR = R1 = (float ) 0.168627; RG = R2 = (float ) 0.168627; RB = R3 = (float ) 0.168627;
	break;
	case Quantity_NOC_GRAY18 :
		RR = R1 = (float ) 0.180392; RG = R2 = (float ) 0.180392; RB = R3 = (float ) 0.180392;
	break;
	case Quantity_NOC_GRAY19 :
		RR = R1 = (float ) 0.188235; RG = R2 = (float ) 0.188235; RB = R3 = (float ) 0.188235;
	break;
	case Quantity_NOC_GRAY2 :
		RR = R1 = (float ) 0.019608; RG = R2 = (float ) 0.019608; RB = R3 = (float ) 0.019608;
	break;
	case Quantity_NOC_GRAY20 :
		RR = R1 = (float ) 0.200000; RG = R2 = (float ) 0.200000; RB = R3 = (float ) 0.200000;
	break;
	case Quantity_NOC_GRAY21 :
		RR = R1 = (float ) 0.211765; RG = R2 = (float ) 0.211765; RB = R3 = (float ) 0.211765;
	break;
	case Quantity_NOC_GRAY22 :
		RR = R1 = (float ) 0.219608; RG = R2 = (float ) 0.219608; RB = R3 = (float ) 0.219608;
	break;
	case Quantity_NOC_GRAY23 :
		RR = R1 = (float ) 0.231373; RG = R2 = (float ) 0.231373; RB = R3 = (float ) 0.231373;
	break;
	case Quantity_NOC_GRAY24 :
		RR = R1 = (float ) 0.239216; RG = R2 = (float ) 0.239216; RB = R3 = (float ) 0.239216;
	break;
	case Quantity_NOC_GRAY25 :
		RR = R1 = (float ) 0.250980; RG = R2 = (float ) 0.250980; RB = R3 = (float ) 0.250980;
	break;
	case Quantity_NOC_GRAY26 :
		RR = R1 = (float ) 0.258824; RG = R2 = (float ) 0.258824; RB = R3 = (float ) 0.258824;
	break;
	case Quantity_NOC_GRAY27 :
		RR = R1 = (float ) 0.270588; RG = R2 = (float ) 0.270588; RB = R3 = (float ) 0.270588;
	break;
	case Quantity_NOC_GRAY28 :
		RR = R1 = (float ) 0.278431; RG = R2 = (float ) 0.278431; RB = R3 = (float ) 0.278431;
	break;
	case Quantity_NOC_GRAY29 :
		RR = R1 = (float ) 0.290196; RG = R2 = (float ) 0.290196; RB = R3 = (float ) 0.290196;
	break;
	case Quantity_NOC_GRAY3 :
		RR = R1 = (float ) 0.031373; RG = R2 = (float ) 0.031373; RB = R3 = (float ) 0.031373;
	break;
	case Quantity_NOC_GRAY30 :
		RR = R1 = (float ) 0.301961; RG = R2 = (float ) 0.301961; RB = R3 = (float ) 0.301961;
	break;
	case Quantity_NOC_GRAY31 :
		RR = R1 = (float ) 0.309804; RG = R2 = (float ) 0.309804; RB = R3 = (float ) 0.309804;
	break;
	case Quantity_NOC_GRAY32 :
		RR = R1 = (float ) 0.321569; RG = R2 = (float ) 0.321569; RB = R3 = (float ) 0.321569;
	break;
	case Quantity_NOC_GRAY33 :
		RR = R1 = (float ) 0.329412; RG = R2 = (float ) 0.329412; RB = R3 = (float ) 0.329412;
	break;
	case Quantity_NOC_GRAY34 :
		RR = R1 = (float ) 0.341176; RG = R2 = (float ) 0.341176; RB = R3 = (float ) 0.341176;
	break;
	case Quantity_NOC_GRAY35 :
		RR = R1 = (float ) 0.349020; RG = R2 = (float ) 0.349020; RB = R3 = (float ) 0.349020;
	break;
	case Quantity_NOC_GRAY36 :
		RR = R1 = (float ) 0.360784; RG = R2 = (float ) 0.360784; RB = R3 = (float ) 0.360784;
	break;
	case Quantity_NOC_GRAY37 :
		RR = R1 = (float ) 0.368627; RG = R2 = (float ) 0.368627; RB = R3 = (float ) 0.368627;
	break;
	case Quantity_NOC_GRAY38 :
		RR = R1 = (float ) 0.380392; RG = R2 = (float ) 0.380392; RB = R3 = (float ) 0.380392;
	break;
	case Quantity_NOC_GRAY39 :
		RR = R1 = (float ) 0.388235; RG = R2 = (float ) 0.388235; RB = R3 = (float ) 0.388235;
	break;
	case Quantity_NOC_GRAY4 :
		RR = R1 = (float ) 0.039216; RG = R2 = (float ) 0.039216; RB = R3 = (float ) 0.039216;
	break;
	case Quantity_NOC_GRAY40 :
		RR = R1 = (float ) 0.400000; RG = R2 = (float ) 0.400000; RB = R3 = (float ) 0.400000;
	break;
	case Quantity_NOC_GRAY41 :
		RR = R1 = (float ) 0.411765; RG = R2 = (float ) 0.411765; RB = R3 = (float ) 0.411765;
	break;
	case Quantity_NOC_GRAY42 :
		RR = R1 = (float ) 0.419608; RG = R2 = (float ) 0.419608; RB = R3 = (float ) 0.419608;
	break;
	case Quantity_NOC_GRAY43 :
		RR = R1 = (float ) 0.431373; RG = R2 = (float ) 0.431373; RB = R3 = (float ) 0.431373;
	break;
	case Quantity_NOC_GRAY44 :
		RR = R1 = (float ) 0.439216; RG = R2 = (float ) 0.439216; RB = R3 = (float ) 0.439216;
	break;
	case Quantity_NOC_GRAY45 :
		RR = R1 = (float ) 0.450980; RG = R2 = (float ) 0.450980; RB = R3 = (float ) 0.450980;
	break;
	case Quantity_NOC_GRAY46 :
		RR = R1 = (float ) 0.458824; RG = R2 = (float ) 0.458824; RB = R3 = (float ) 0.458824;
	break;
	case Quantity_NOC_GRAY47 :
		RR = R1 = (float ) 0.470588; RG = R2 = (float ) 0.470588; RB = R3 = (float ) 0.470588;
	break;
	case Quantity_NOC_GRAY48 :
		RR = R1 = (float ) 0.478431; RG = R2 = (float ) 0.478431; RB = R3 = (float ) 0.478431;
	break;
	case Quantity_NOC_GRAY49 :
		RR = R1 = (float ) 0.490196; RG = R2 = (float ) 0.490196; RB = R3 = (float ) 0.490196;
	break;
	case Quantity_NOC_GRAY5 :
		RR = R1 = (float ) 0.050980; RG = R2 = (float ) 0.050980; RB = R3 = (float ) 0.050980;
	break;
	case Quantity_NOC_GRAY50 :
		RR = R1 = (float ) 0.498039; RG = R2 = (float ) 0.498039; RB = R3 = (float ) 0.498039;
	break;
	case Quantity_NOC_GRAY51 :
		RR = R1 = (float ) 0.509804; RG = R2 = (float ) 0.509804; RB = R3 = (float ) 0.509804;
	break;
	case Quantity_NOC_GRAY52 :
		RR = R1 = (float ) 0.521569; RG = R2 = (float ) 0.521569; RB = R3 = (float ) 0.521569;
	break;
	case Quantity_NOC_GRAY53 :
		RR = R1 = (float ) 0.529412; RG = R2 = (float ) 0.529412; RB = R3 = (float ) 0.529412;
	break;
	case Quantity_NOC_GRAY54 :
		RR = R1 = (float ) 0.541176; RG = R2 = (float ) 0.541176; RB = R3 = (float ) 0.541176;
	break;
	case Quantity_NOC_GRAY55 :
		RR = R1 = (float ) 0.549020; RG = R2 = (float ) 0.549020; RB = R3 = (float ) 0.549020;
	break;
	case Quantity_NOC_GRAY56 :
		RR = R1 = (float ) 0.560784; RG = R2 = (float ) 0.560784; RB = R3 = (float ) 0.560784;
	break;
	case Quantity_NOC_GRAY57 :
		RR = R1 = (float ) 0.568627; RG = R2 = (float ) 0.568627; RB = R3 = (float ) 0.568627;
	break;
	case Quantity_NOC_GRAY58 :
		RR = R1 = (float ) 0.580392; RG = R2 = (float ) 0.580392; RB = R3 = (float ) 0.580392;
	break;
	case Quantity_NOC_GRAY59 :
		RR = R1 = (float ) 0.588235; RG = R2 = (float ) 0.588235; RB = R3 = (float ) 0.588235;
	break;
	case Quantity_NOC_GRAY6 :
		RR = R1 = (float ) 0.058824; RG = R2 = (float ) 0.058824; RB = R3 = (float ) 0.058824;
	break;
	case Quantity_NOC_GRAY60 :
		RR = R1 = (float ) 0.600000; RG = R2 = (float ) 0.600000; RB = R3 = (float ) 0.600000;
	break;
	case Quantity_NOC_GRAY61 :
		RR = R1 = (float ) 0.611765; RG = R2 = (float ) 0.611765; RB = R3 = (float ) 0.611765;
	break;
	case Quantity_NOC_GRAY62 :
		RR = R1 = (float ) 0.619608; RG = R2 = (float ) 0.619608; RB = R3 = (float ) 0.619608;
	break;
	case Quantity_NOC_GRAY63 :
		RR = R1 = (float ) 0.631373; RG = R2 = (float ) 0.631373; RB = R3 = (float ) 0.631373;
	break;
	case Quantity_NOC_GRAY64 :
		RR = R1 = (float ) 0.639216; RG = R2 = (float ) 0.639216; RB = R3 = (float ) 0.639216;
	break;
	case Quantity_NOC_GRAY65 :
		RR = R1 = (float ) 0.650980; RG = R2 = (float ) 0.650980; RB = R3 = (float ) 0.650980;
	break;
	case Quantity_NOC_GRAY66 :
		RR = R1 = (float ) 0.658824; RG = R2 = (float ) 0.658824; RB = R3 = (float ) 0.658824;
	break;
	case Quantity_NOC_GRAY67 :
		RR = R1 = (float ) 0.670588; RG = R2 = (float ) 0.670588; RB = R3 = (float ) 0.670588;
	break;
	case Quantity_NOC_GRAY68 :
		RR = R1 = (float ) 0.678431; RG = R2 = (float ) 0.678431; RB = R3 = (float ) 0.678431;
	break;
	case Quantity_NOC_GRAY69 :
		RR = R1 = (float ) 0.690196; RG = R2 = (float ) 0.690196; RB = R3 = (float ) 0.690196;
	break;
	case Quantity_NOC_GRAY7 :
		RR = R1 = (float ) 0.070588; RG = R2 = (float ) 0.070588; RB = R3 = (float ) 0.070588;
	break;
	case Quantity_NOC_GRAY70 :
		RR = R1 = (float ) 0.701961; RG = R2 = (float ) 0.701961; RB = R3 = (float ) 0.701961;
	break;
	case Quantity_NOC_GRAY71 :
		RR = R1 = (float ) 0.709804; RG = R2 = (float ) 0.709804; RB = R3 = (float ) 0.709804;
	break;
	case Quantity_NOC_GRAY72 :
		RR = R1 = (float ) 0.721569; RG = R2 = (float ) 0.721569; RB = R3 = (float ) 0.721569;
	break;
	case Quantity_NOC_GRAY73 :
		RR = R1 = (float ) 0.729412; RG = R2 = (float ) 0.729412; RB = R3 = (float ) 0.729412;
	break;
	case Quantity_NOC_GRAY74 :
		RR = R1 = (float ) 0.741176; RG = R2 = (float ) 0.741176; RB = R3 = (float ) 0.741176;
	break;
	case Quantity_NOC_GRAY75 :
		RR = R1 = (float ) 0.749020; RG = R2 = (float ) 0.749020; RB = R3 = (float ) 0.749020;
	break;
	case Quantity_NOC_GRAY76 :
		RR = R1 = (float ) 0.760784; RG = R2 = (float ) 0.760784; RB = R3 = (float ) 0.760784;
	break;
	case Quantity_NOC_GRAY77 :
		RR = R1 = (float ) 0.768627; RG = R2 = (float ) 0.768627; RB = R3 = (float ) 0.768627;
	break;
	case Quantity_NOC_GRAY78 :
		RR = R1 = (float ) 0.780392; RG = R2 = (float ) 0.780392; RB = R3 = (float ) 0.780392;
	break;
	case Quantity_NOC_GRAY79 :
		RR = R1 = (float ) 0.788235; RG = R2 = (float ) 0.788235; RB = R3 = (float ) 0.788235;
	break;
	case Quantity_NOC_GRAY8 :
		RR = R1 = (float ) 0.078431; RG = R2 = (float ) 0.078431; RB = R3 = (float ) 0.078431;
	break;
	case Quantity_NOC_GRAY80 :
		RR = R1 = (float ) 0.800000; RG = R2 = (float ) 0.800000; RB = R3 = (float ) 0.800000;
	break;
	case Quantity_NOC_GRAY81 :
		RR = R1 = (float ) 0.811765; RG = R2 = (float ) 0.811765; RB = R3 = (float ) 0.811765;
	break;
	case Quantity_NOC_GRAY82 :
		RR = R1 = (float ) 0.819608; RG = R2 = (float ) 0.819608; RB = R3 = (float ) 0.819608;
	break;
	case Quantity_NOC_GRAY83 :
		RR = R1 = (float ) 0.831373; RG = R2 = (float ) 0.831373; RB = R3 = (float ) 0.831373;
	break;
	case Quantity_NOC_GRAY85 :
		RR = R1 = (float ) 0.850980; RG = R2 = (float ) 0.850980; RB = R3 = (float ) 0.850980;
	break;
	case Quantity_NOC_GRAY86 :
		RR = R1 = (float ) 0.858824; RG = R2 = (float ) 0.858824; RB = R3 = (float ) 0.858824;
	break;
	case Quantity_NOC_GRAY87 :
		RR = R1 = (float ) 0.870588; RG = R2 = (float ) 0.870588; RB = R3 = (float ) 0.870588;
	break;
	case Quantity_NOC_GRAY88 :
		RR = R1 = (float ) 0.878431; RG = R2 = (float ) 0.878431; RB = R3 = (float ) 0.878431;
	break;
	case Quantity_NOC_GRAY89 :
		RR = R1 = (float ) 0.890196; RG = R2 = (float ) 0.890196; RB = R3 = (float ) 0.890196;
	break;
	case Quantity_NOC_GRAY9 :
		RR = R1 = (float ) 0.090196; RG = R2 = (float ) 0.090196; RB = R3 = (float ) 0.090196;
	break;
	case Quantity_NOC_GRAY90 :
		RR = R1 = (float ) 0.898039; RG = R2 = (float ) 0.898039; RB = R3 = (float ) 0.898039;
	break;
	case Quantity_NOC_GRAY91 :
		RR = R1 = (float ) 0.909804; RG = R2 = (float ) 0.909804; RB = R3 = (float ) 0.909804;
	break;
	case Quantity_NOC_GRAY92 :
		RR = R1 = (float ) 0.921569; RG = R2 = (float ) 0.921569; RB = R3 = (float ) 0.921569;
	break;
	case Quantity_NOC_GRAY93 :
		RR = R1 = (float ) 0.929412; RG = R2 = (float ) 0.929412; RB = R3 = (float ) 0.929412;
	break;
	case Quantity_NOC_GRAY94 :
		RR = R1 = (float ) 0.941176; RG = R2 = (float ) 0.941176; RB = R3 = (float ) 0.941176;
	break;
	case Quantity_NOC_GRAY95 :
		RR = R1 = (float ) 0.949020; RG = R2 = (float ) 0.949020; RB = R3 = (float ) 0.949020;
	break;
	case Quantity_NOC_GREEN :
		RR = R1 = 0.000000; RG = R2 = 1.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GREEN1 :
		RR = R1 = 0.000000; RG = R2 = 1.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GREEN2 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.933333; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GREEN3 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.803922; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GREEN4 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.545098; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_GREENYELLOW :
		RR = R1 = (float ) 0.678431; RG = R2 = 1.000000; RB = R3 = (float ) 0.184314;
	break;
	case Quantity_NOC_GRAY97 :
		RR = R1 = (float ) 0.968627; RG = R2 = (float ) 0.968627; RB = R3 = (float ) 0.968627;
	break;
	case Quantity_NOC_GRAY98 :
		RR = R1 = (float ) 0.980392; RG = R2 = (float ) 0.980392; RB = R3 = (float ) 0.980392;
	break;
	case Quantity_NOC_GRAY99 :
		RR = R1 = (float ) 0.988235; RG = R2 = (float ) 0.988235; RB = R3 = (float ) 0.988235;
	break;
	case Quantity_NOC_HONEYDEW :
		RR = R1 = (float ) 0.941176; RG = R2 = 1.000000; RB = R3 = (float ) 0.941176;
	break;
	case Quantity_NOC_HONEYDEW2 :
		RR = R1 = (float ) 0.878431; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.878431;
	break;
	case Quantity_NOC_HONEYDEW3 :
		RR = R1 = (float ) 0.756863; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.756863;
	break;
	case Quantity_NOC_HONEYDEW4 :
		RR = R1 = (float ) 0.513725; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.513725;
	break;
	case Quantity_NOC_HOTPINK :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.411765; RB = R3 = (float ) 0.705882;
	break;
	case Quantity_NOC_HOTPINK1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.431373; RB = R3 = (float ) 0.705882;
	break;
	case Quantity_NOC_HOTPINK2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.415686; RB = R3 = (float ) 0.654902;
	break;
	case Quantity_NOC_HOTPINK3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.376471; RB = R3 = (float ) 0.564706;
	break;
	case Quantity_NOC_HOTPINK4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.227451; RB = R3 = (float ) 0.384314;
	break;
	case Quantity_NOC_INDIANRED :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.360784; RB = R3 = (float ) 0.360784;
	break;
	case Quantity_NOC_INDIANRED1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.415686; RB = R3 = (float ) 0.415686;
	break;
	case Quantity_NOC_INDIANRED2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.388235; RB = R3 = (float ) 0.388235;
	break;
	case Quantity_NOC_INDIANRED3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.333333; RB = R3 = (float ) 0.333333;
	break;
	case Quantity_NOC_INDIANRED4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.227451; RB = R3 = (float ) 0.227451;
	break;
	case Quantity_NOC_IVORY :
		RR = R1 = 1.000000; RG = R2 = 1.000000; RB = R3 = (float ) 0.941176;
	break;
	case Quantity_NOC_IVORY2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.878431;
	break;
	case Quantity_NOC_IVORY3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.756863;
	break;
	case Quantity_NOC_IVORY4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.513725;
	break;
	case Quantity_NOC_KHAKI :
		RR = R1 = (float ) 0.941176; RG = R2 = (float ) 0.901961; RB = R3 = (float ) 0.549020;
	break;
	case Quantity_NOC_KHAKI1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.964706; RB = R3 = (float ) 0.560784;
	break;
	case Quantity_NOC_KHAKI2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.901961; RB = R3 = (float ) 0.521569;
	break;
	case Quantity_NOC_KHAKI3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.776471; RB = R3 = (float ) 0.450980;
	break;
	case Quantity_NOC_KHAKI4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.525490; RB = R3 = (float ) 0.305882;
	break;
	case Quantity_NOC_LAVENDER :
		RR = R1 = (float ) 0.901961; RG = R2 = (float ) 0.901961; RB = R3 = (float ) 0.980392;
	break;
	case Quantity_NOC_LAVENDERBLUSH1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.941176; RB = R3 = (float ) 0.960784;
	break;
	case Quantity_NOC_LAVENDERBLUSH2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.878431; RB = R3 = (float ) 0.898039;
	break;
	case Quantity_NOC_LAVENDERBLUSH3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.756863; RB = R3 = (float ) 0.772549;
	break;
	case Quantity_NOC_LAVENDERBLUSH4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.513725; RB = R3 = (float ) 0.525490;
	break;
	case Quantity_NOC_LAWNGREEN :
		RR = R1 = (float ) 0.486275; RG = R2 = (float ) 0.988235; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_LEMONCHIFFON1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.980392; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_LEMONCHIFFON2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.913725; RB = R3 = (float ) 0.749020;
	break;
	case Quantity_NOC_LEMONCHIFFON3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.788235; RB = R3 = (float ) 0.647059;
	break;
	case Quantity_NOC_LEMONCHIFFON4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.537255; RB = R3 = (float ) 0.439216;
	break;
	case Quantity_NOC_LIGHTBLUE :
		RR = R1 = (float ) 0.678431; RG = R2 = (float ) 0.847059; RB = R3 = (float ) 0.901961;
	break;
	case Quantity_NOC_LIGHTBLUE1 :
		RR = R1 = (float ) 0.749020; RG = R2 = (float ) 0.937255; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_LIGHTBLUE2 :
		RR = R1 = (float ) 0.698039; RG = R2 = (float ) 0.874510; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_LIGHTBLUE3 :
		RR = R1 = (float ) 0.603922; RG = R2 = (float ) 0.752941; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_LIGHTBLUE4 :
		RR = R1 = (float ) 0.407843; RG = R2 = (float ) 0.513725; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_LIGHTCORAL :
		RR = R1 = (float ) 0.941176; RG = R2 = (float ) 0.501961; RB = R3 = (float ) 0.501961;
	break;
	case Quantity_NOC_LIGHTCYAN1 :
		RR = R1 = (float ) 0.878431; RG = R2 = 1.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_LIGHTCYAN2 :
		RR = R1 = (float ) 0.819608; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_LIGHTCYAN3 :
		RR = R1 = (float ) 0.705882; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_LIGHTCYAN4 :
		RR = R1 = (float ) 0.478431; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_LIGHTGOLDENROD :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.866667; RB = R3 = (float ) 0.509804;
	break;
	case Quantity_NOC_LIGHTGOLDENROD1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.925490; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_LIGHTGOLDENROD2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.862745; RB = R3 = (float ) 0.509804;
	break;
	case Quantity_NOC_LIGHTGOLDENROD3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.745098; RB = R3 = (float ) 0.439216;
	break;
	case Quantity_NOC_LIGHTGOLDENROD4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.505882; RB = R3 = (float ) 0.298039;
	break;
	case Quantity_NOC_LIGHTGOLDENRODYELLOW :
		RR = R1 = (float ) 0.980392; RG = R2 = (float ) 0.980392; RB = R3 = (float ) 0.823529;
	break;
	case Quantity_NOC_LIGHTGRAY :
		RR = R1 = (float ) 0.827451; RG = R2 = (float ) 0.827451; RB = R3 = (float ) 0.827451;
	break;
	case Quantity_NOC_LIGHTPINK :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.713725; RB = R3 = (float ) 0.756863;
	break;
	case Quantity_NOC_LIGHTPINK1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.682353; RB = R3 = (float ) 0.725490;
	break;
	case Quantity_NOC_LIGHTPINK2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.635294; RB = R3 = (float ) 0.678431;
	break;
	case Quantity_NOC_LIGHTPINK3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.549020; RB = R3 = (float ) 0.584314;
	break;
	case Quantity_NOC_LIGHTPINK4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.372549; RB = R3 = (float ) 0.396078;
	break;
	case Quantity_NOC_LIGHTSALMON1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.627451; RB = R3 = (float ) 0.478431;
	break;
	case Quantity_NOC_LIGHTSALMON2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.584314; RB = R3 = (float ) 0.447059;
	break;
	case Quantity_NOC_LIGHTSALMON3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.505882; RB = R3 = (float ) 0.384314;
	break;
	case Quantity_NOC_LIGHTSALMON4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.341176; RB = R3 = (float ) 0.258824;
	break;
	case Quantity_NOC_LIGHTSEAGREEN :
		RR = R1 = (float ) 0.125490; RG = R2 = (float ) 0.698039; RB = R3 = (float ) 0.666667;
	break;
	case Quantity_NOC_LIGHTSKYBLUE :
		RR = R1 = (float ) 0.529412; RG = R2 = (float ) 0.807843; RB = R3 = (float ) 0.980392;
	break;
	case Quantity_NOC_LIGHTSKYBLUE1 :
		RR = R1 = (float ) 0.690196; RG = R2 = (float ) 0.886275; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_LIGHTSKYBLUE2 :
		RR = R1 = (float ) 0.643137; RG = R2 = (float ) 0.827451; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_LIGHTSKYBLUE3 :
		RR = R1 = (float ) 0.552941; RG = R2 = (float ) 0.713725; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_LIGHTSKYBLUE4 :
		RR = R1 = (float ) 0.376471; RG = R2 = (float ) 0.482353; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_LIGHTSLATEBLUE :
		RR = R1 = (float ) 0.517647; RG = R2 = (float ) 0.439216; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_LIGHTSLATEGRAY :
		RR = R1 = (float ) 0.466667; RG = R2 = (float ) 0.533333; RB = R3 = (float ) 0.600000;
	break;
	case Quantity_NOC_LIGHTSTEELBLUE :
		RR = R1 = (float ) 0.690196; RG = R2 = (float ) 0.768627; RB = R3 = (float ) 0.870588;
	break;
	case Quantity_NOC_LIGHTSTEELBLUE1 :
		RR = R1 = (float ) 0.792157; RG = R2 = (float ) 0.882353; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_LIGHTSTEELBLUE2 :
		RR = R1 = (float ) 0.737255; RG = R2 = (float ) 0.823529; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_LIGHTSTEELBLUE3 :
		RR = R1 = (float ) 0.635294; RG = R2 = (float ) 0.709804; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_LIGHTSTEELBLUE4 :
		RR = R1 = (float ) 0.431373; RG = R2 = (float ) 0.482353; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_LIGHTYELLOW :
		RR = R1 = 1.000000; RG = R2 = 1.000000; RB = R3 = (float ) 0.878431;
	break;
	case Quantity_NOC_LIGHTYELLOW2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.819608;
	break;
	case Quantity_NOC_LIGHTYELLOW3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.705882;
	break;
	case Quantity_NOC_LIGHTYELLOW4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.478431;
	break;
	case Quantity_NOC_LIMEGREEN :
		RR = R1 = (float ) 0.196078; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.196078;
	break;
	case Quantity_NOC_LINEN :
		RR = R1 = (float ) 0.980392; RG = R2 = (float ) 0.941176; RB = R3 = (float ) 0.901961;
	break;
	case Quantity_NOC_MAGENTA1 :
		RR = R1 = 1.000000; RG = R2 = 0.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_MAGENTA2 :
		RR = R1 = (float ) 0.933333; RG = R2 = 0.000000; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_MAGENTA3 :
		RR = R1 = (float ) 0.803922; RG = R2 = 0.000000; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_MAGENTA4 :
		RR = R1 = (float ) 0.545098; RG = R2 = 0.000000; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_MAROON :
		RR = R1 = (float ) 0.690196; RG = R2 = (float ) 0.188235; RB = R3 = (float ) 0.376471;
	break;
	case Quantity_NOC_MAROON1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.203922; RB = R3 = (float ) 0.701961;
	break;
	case Quantity_NOC_MAROON2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.188235; RB = R3 = (float ) 0.654902;
	break;
	case Quantity_NOC_MAROON3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.160784; RB = R3 = (float ) 0.564706;
	break;
	case Quantity_NOC_MAROON4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.109804; RB = R3 = (float ) 0.384314;
	break;
	case Quantity_NOC_MEDIUMAQUAMARINE :
		RR = R1 = (float ) 0.400000; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.666667;
	break;
	case Quantity_NOC_MEDIUMORCHID :
		RR = R1 = (float ) 0.729412; RG = R2 = (float ) 0.333333; RB = R3 = (float ) 0.827451;
	break;
	case Quantity_NOC_MEDIUMORCHID1 :
		RR = R1 = (float ) 0.878431; RG = R2 = (float ) 0.400000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_MEDIUMORCHID2 :
		RR = R1 = (float ) 0.819608; RG = R2 = (float ) 0.372549; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_MEDIUMORCHID3 :
		RR = R1 = (float ) 0.705882; RG = R2 = (float ) 0.321569; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_MEDIUMORCHID4 :
		RR = R1 = (float ) 0.478431; RG = R2 = (float ) 0.215686; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_MEDIUMPURPLE :
		RR = R1 = (float ) 0.576471; RG = R2 = (float ) 0.439216; RB = R3 = (float ) 0.858824;
	break;
	case Quantity_NOC_MEDIUMPURPLE1 :
		RR = R1 = (float ) 0.670588; RG = R2 = (float ) 0.509804; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_MEDIUMPURPLE2 :
		RR = R1 = (float ) 0.623529; RG = R2 = (float ) 0.474510; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_MEDIUMPURPLE3 :
		RR = R1 = (float ) 0.537255; RG = R2 = (float ) 0.407843; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_MEDIUMPURPLE4 :
		RR = R1 = (float ) 0.364706; RG = R2 = (float ) 0.278431; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_MEDIUMSEAGREEN :
		RR = R1 = (float ) 0.235294; RG = R2 = (float ) 0.701961; RB = R3 = (float ) 0.443137;
	break;
	case Quantity_NOC_MEDIUMSLATEBLUE :
		RR = R1 = (float ) 0.482353; RG = R2 = (float ) 0.407843; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_MEDIUMSPRINGGREEN :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.980392; RB = R3 = (float ) 0.603922;
	break;
	case Quantity_NOC_MEDIUMTURQUOISE :
		RR = R1 = (float ) 0.282353; RG = R2 = (float ) 0.819608; RB = R3 = (float ) 0.800000;
	break;
	case Quantity_NOC_MEDIUMVIOLETRED :
		RR = R1 = (float ) 0.780392; RG = R2 = (float ) 0.082353; RB = R3 = (float ) 0.521569;
	break;
	case Quantity_NOC_MIDNIGHTBLUE :
		RR = R1 = (float ) 0.098039; RG = R2 = (float ) 0.098039; RB = R3 = (float ) 0.439216;
	break;
	case Quantity_NOC_MINTCREAM :
		RR = R1 = (float ) 0.960784; RG = R2 = 1.000000; RB = R3 = (float ) 0.980392;
	break;
	case Quantity_NOC_MISTYROSE :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.894118; RB = R3 = (float ) 0.882353;
	break;
	case Quantity_NOC_MISTYROSE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.835294; RB = R3 = (float ) 0.823529;
	break;
	case Quantity_NOC_MISTYROSE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.717647; RB = R3 = (float ) 0.709804;
	break;
	case Quantity_NOC_MISTYROSE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.490196; RB = R3 = (float ) 0.482353;
	break;
	case Quantity_NOC_MOCCASIN :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.894118; RB = R3 = (float ) 0.709804;
	break;
	case Quantity_NOC_NAVAJOWHITE1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.870588; RB = R3 = (float ) 0.678431;
	break;
	case Quantity_NOC_NAVAJOWHITE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.811765; RB = R3 = (float ) 0.631373;
	break;
	case Quantity_NOC_NAVAJOWHITE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.701961; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_NAVAJOWHITE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.474510; RB = R3 = (float ) 0.368627;
	break;
	case Quantity_NOC_NAVYBLUE :
		RR = R1 = 0.000000; RG = R2 = 0.000000; RB = R3 = (float ) 0.501961;
	break;
	case Quantity_NOC_OLDLACE :
		RR = R1 = (float ) 0.992157; RG = R2 = (float ) 0.960784; RB = R3 = (float ) 0.901961;
	break;
	case Quantity_NOC_OLIVEDRAB :
		RR = R1 = (float ) 0.419608; RG = R2 = (float ) 0.556863; RB = R3 = (float ) 0.137255;
	break;
	case Quantity_NOC_OLIVEDRAB1 :
		RR = R1 = (float ) 0.752941; RG = R2 = 1.000000; RB = R3 = (float ) 0.243137;
	break;
	case Quantity_NOC_OLIVEDRAB2 :
		RR = R1 = (float ) 0.701961; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.227451;
	break;
	case Quantity_NOC_OLIVEDRAB3 :
		RR = R1 = (float ) 0.603922; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.196078;
	break;
	case Quantity_NOC_OLIVEDRAB4 :
		RR = R1 = (float ) 0.411765; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.133333;
	break;
	case Quantity_NOC_ORANGE :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.647059; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGE1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.647059; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.603922; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.521569; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.352941; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGERED :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.270588; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGERED1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.270588; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGERED2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.250980; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGERED3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.215686; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORANGERED4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.145098; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ORCHID :
		RR = R1 = (float ) 0.854902; RG = R2 = (float ) 0.439216; RB = R3 = (float ) 0.839216;
	break;
	case Quantity_NOC_ORCHID1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.513725; RB = R3 = (float ) 0.980392;
	break;
	case Quantity_NOC_ORCHID2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.478431; RB = R3 = (float ) 0.913725;
	break;
	case Quantity_NOC_ORCHID3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.411765; RB = R3 = (float ) 0.788235;
	break;
	case Quantity_NOC_ORCHID4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.278431; RB = R3 = (float ) 0.537255;
	break;
	case Quantity_NOC_PALEGOLDENROD :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.909804; RB = R3 = (float ) 0.666667;
	break;
	case Quantity_NOC_PALEGREEN :
		RR = R1 = (float ) 0.596078; RG = R2 = (float ) 0.984314; RB = R3 = (float ) 0.596078;
	break;
	case Quantity_NOC_PALEGREEN1 :
		RR = R1 = (float ) 0.603922; RG = R2 = 1.000000; RB = R3 = (float ) 0.603922;
	break;
	case Quantity_NOC_PALEGREEN2 :
		RR = R1 = (float ) 0.564706; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.564706;
	break;
	case Quantity_NOC_PALEGREEN3 :
		RR = R1 = (float ) 0.486275; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.486275;
	break;
	case Quantity_NOC_PALEGREEN4 :
		RR = R1 = (float ) 0.329412; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.329412;
	break;
	case Quantity_NOC_PALETURQUOISE :
		RR = R1 = (float ) 0.686275; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_PALETURQUOISE1 :
		RR = R1 = (float ) 0.733333; RG = R2 = 1.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_PALETURQUOISE2 :
		RR = R1 = (float ) 0.682353; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_PALETURQUOISE3 :
		RR = R1 = (float ) 0.588235; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_PALETURQUOISE4 :
		RR = R1 = (float ) 0.400000; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_PALEVIOLETRED :
		RR = R1 = (float ) 0.858824; RG = R2 = (float ) 0.439216; RB = R3 = (float ) 0.576471;
	break;
	case Quantity_NOC_PALEVIOLETRED1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.509804; RB = R3 = (float ) 0.670588;
	break;
	case Quantity_NOC_PALEVIOLETRED2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.474510; RB = R3 = (float ) 0.623529;
	break;
	case Quantity_NOC_PALEVIOLETRED3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.407843; RB = R3 = (float ) 0.537255;
	break;
	case Quantity_NOC_PALEVIOLETRED4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.278431; RB = R3 = (float ) 0.364706;
	break;
	case Quantity_NOC_PAPAYAWHIP :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.937255; RB = R3 = (float ) 0.835294;
	break;
	case Quantity_NOC_PEACHPUFF :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.854902; RB = R3 = (float ) 0.725490;
	break;
	case Quantity_NOC_PEACHPUFF2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.796078; RB = R3 = (float ) 0.678431;
	break;
	case Quantity_NOC_PEACHPUFF3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.686275; RB = R3 = (float ) 0.584314;
	break;
	case Quantity_NOC_PEACHPUFF4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.466667; RB = R3 = (float ) 0.396078;
	break;
	case Quantity_NOC_PERU :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.521569; RB = R3 = (float ) 0.247059;
	break;
	case Quantity_NOC_PINK :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.752941; RB = R3 = (float ) 0.796078;
	break;
	case Quantity_NOC_PINK1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.709804; RB = R3 = (float ) 0.772549;
	break;
	case Quantity_NOC_PINK2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.662745; RB = R3 = (float ) 0.721569;
	break;
	case Quantity_NOC_PINK3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.568627; RB = R3 = (float ) 0.619608;
	break;
	case Quantity_NOC_PINK4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.388235; RB = R3 = (float ) 0.423529;
	break;
	case Quantity_NOC_PLUM :
		RR = R1 = (float ) 0.866667; RG = R2 = (float ) 0.627451; RB = R3 = (float ) 0.866667;
	break;
	case Quantity_NOC_PLUM1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.733333; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_PLUM2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.682353; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_PLUM3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.588235; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_PLUM4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.400000; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_POWDERBLUE :
		RR = R1 = (float ) 0.690196; RG = R2 = (float ) 0.878431; RB = R3 = (float ) 0.901961;
	break;
	case Quantity_NOC_PURPLE :
		RR = R1 = (float ) 0.627451; RG = R2 = (float ) 0.125490; RB = R3 = (float ) 0.941176;
	break;
	case Quantity_NOC_PURPLE1 :
		RR = R1 = (float ) 0.607843; RG = R2 = (float ) 0.188235; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_PURPLE2 :
		RR = R1 = (float ) 0.568627; RG = R2 = (float ) 0.172549; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_PURPLE3 :
		RR = R1 = (float ) 0.490196; RG = R2 = (float ) 0.149020; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_PURPLE4 :
		RR = R1 = (float ) 0.333333; RG = R2 = (float ) 0.101961; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_RED :
		RR = R1 = 1.000000; RG = R2 = 0.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_RED1 :
		RR = R1 = 1.000000; RG = R2 = 0.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_RED2 :
		RR = R1 = (float ) 0.933333; RG = R2 = 0.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_RED3 :
		RR = R1 = (float ) 0.803922; RG = R2 = 0.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_RED4 :
		RR = R1 = (float ) 0.545098; RG = R2 = 0.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_ROSYBROWN :
		RR = R1 = (float ) 0.737255; RG = R2 = (float ) 0.560784; RB = R3 = (float ) 0.560784;
	break;
	case Quantity_NOC_ROSYBROWN1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.756863; RB = R3 = (float ) 0.756863;
	break;
	case Quantity_NOC_ROSYBROWN2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.705882; RB = R3 = (float ) 0.705882;
	break;
	case Quantity_NOC_ROSYBROWN3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.607843; RB = R3 = (float ) 0.607843;
	break;
	case Quantity_NOC_ROSYBROWN4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.411765; RB = R3 = (float ) 0.411765;
	break;
	case Quantity_NOC_ROYALBLUE :
		RR = R1 = (float ) 0.254902; RG = R2 = (float ) 0.411765; RB = R3 = (float ) 0.882353;
	break;
	case Quantity_NOC_ROYALBLUE1 :
		RR = R1 = (float ) 0.282353; RG = R2 = (float ) 0.462745; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_ROYALBLUE2 :
		RR = R1 = (float ) 0.262745; RG = R2 = (float ) 0.431373; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_ROYALBLUE3 :
		RR = R1 = (float ) 0.227451; RG = R2 = (float ) 0.372549; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_ROYALBLUE4 :
		RR = R1 = (float ) 0.152941; RG = R2 = (float ) 0.250980; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_SADDLEBROWN :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.270588; RB = R3 = (float ) 0.074510;
	break;
	case Quantity_NOC_SALMON :
		RR = R1 = (float ) 0.980392; RG = R2 = (float ) 0.501961; RB = R3 = (float ) 0.447059;
	break;
	case Quantity_NOC_SALMON1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.549020; RB = R3 = (float ) 0.411765;
	break;
	case Quantity_NOC_SALMON2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.509804; RB = R3 = (float ) 0.384314;
	break;
	case Quantity_NOC_SALMON3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.439216; RB = R3 = (float ) 0.329412;
	break;
	case Quantity_NOC_SALMON4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.298039; RB = R3 = (float ) 0.223529;
	break;
	case Quantity_NOC_SANDYBROWN :
		RR = R1 = (float ) 0.956863; RG = R2 = (float ) 0.643137; RB = R3 = (float ) 0.376471;
	break;
	case Quantity_NOC_SEAGREEN :
		RR = R1 = (float ) 0.180392; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.341176;
	break;
	case Quantity_NOC_SEAGREEN1 :
		RR = R1 = (float ) 0.329412; RG = R2 = 1.000000; RB = R3 = (float ) 0.623529;
	break;
	case Quantity_NOC_SEAGREEN2 :
		RR = R1 = (float ) 0.305882; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.580392;
	break;
	case Quantity_NOC_SEAGREEN3 :
		RR = R1 = (float ) 0.262745; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.501961;
	break;
	case Quantity_NOC_SEAGREEN4 :
		RR = R1 = (float ) 0.180392; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.341176;
	break;
	case Quantity_NOC_SEASHELL :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.960784; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_SEASHELL2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.898039; RB = R3 = (float ) 0.870588;
	break;
	case Quantity_NOC_SEASHELL3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.772549; RB = R3 = (float ) 0.749020;
	break;
	case Quantity_NOC_SEASHELL4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.525490; RB = R3 = (float ) 0.509804;
	break;
	case Quantity_NOC_BEET :
		RR = R1 = (float ) 0.556863; RG = R2 = (float ) 0.219608; RB = R3 = (float ) 0.556863;
	break;
	case Quantity_NOC_TEAL :
		RR = R1 = (float ) 0.219608; RG = R2 = (float ) 0.556863; RB = R3 = (float ) 0.556863;
	break;
	case Quantity_NOC_SIENNA :
		RR = R1 = (float ) 0.627451; RG = R2 = (float ) 0.321569; RB = R3 = (float ) 0.176471;
	break;
	case Quantity_NOC_SIENNA1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.509804; RB = R3 = (float ) 0.278431;
	break;
	case Quantity_NOC_SIENNA2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.474510; RB = R3 = (float ) 0.258824;
	break;
	case Quantity_NOC_SIENNA3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.407843; RB = R3 = (float ) 0.223529;
	break;
	case Quantity_NOC_SIENNA4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.278431; RB = R3 = (float ) 0.149020;
	break;
	case Quantity_NOC_SKYBLUE :
		RR = R1 = (float ) 0.529412; RG = R2 = (float ) 0.807843; RB = R3 = (float ) 0.921569;
	break;
	case Quantity_NOC_SKYBLUE1 :
		RR = R1 = (float ) 0.529412; RG = R2 = (float ) 0.807843; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_SKYBLUE2 :
		RR = R1 = (float ) 0.494118; RG = R2 = (float ) 0.752941; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_SKYBLUE3 :
		RR = R1 = (float ) 0.423529; RG = R2 = (float ) 0.650980; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_SKYBLUE4 :
		RR = R1 = (float ) 0.290196; RG = R2 = (float ) 0.439216; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_SLATEBLUE :
		RR = R1 = (float ) 0.415686; RG = R2 = (float ) 0.352941; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_SLATEBLUE1 :
		RR = R1 = (float ) 0.513725; RG = R2 = (float ) 0.435294; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_SLATEBLUE2 :
		RR = R1 = (float ) 0.478431; RG = R2 = (float ) 0.403922; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_SLATEBLUE3 :
		RR = R1 = (float ) 0.411765; RG = R2 = (float ) 0.349020; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_SLATEBLUE4 :
		RR = R1 = (float ) 0.278431; RG = R2 = (float ) 0.235294; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_SLATEGRAY1 :
		RR = R1 = (float ) 0.776471; RG = R2 = (float ) 0.886275; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_SLATEGRAY2 :
		RR = R1 = (float ) 0.725490; RG = R2 = (float ) 0.827451; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_SLATEGRAY3 :
		RR = R1 = (float ) 0.623529; RG = R2 = (float ) 0.713725; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_SLATEGRAY4 :
		RR = R1 = (float ) 0.423529; RG = R2 = (float ) 0.482353; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_SLATEGRAY :
		RR = R1 = (float ) 0.439216; RG = R2 = (float ) 0.501961; RB = R3 = (float ) 0.564706;
	break;
	case Quantity_NOC_SNOW :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.980392; RB = R3 = (float ) 0.980392;
	break;
	case Quantity_NOC_SNOW2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.913725; RB = R3 = (float ) 0.913725;
	break;
	case Quantity_NOC_SNOW3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.788235; RB = R3 = (float ) 0.788235;
	break;
	case Quantity_NOC_SNOW4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.537255; RB = R3 = (float ) 0.537255;
	break;
	case Quantity_NOC_SPRINGGREEN :
		RR = R1 = 0.000000; RG = R2 = 1.000000; RB = R3 = (float ) 0.498039;
	break;
	case Quantity_NOC_SPRINGGREEN2 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.933333; RB = R3 = (float ) 0.462745;
	break;
	case Quantity_NOC_SPRINGGREEN3 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.400000;
	break;
	case Quantity_NOC_SPRINGGREEN4 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.545098; RB = R3 = (float ) 0.270588;
	break;
	case Quantity_NOC_STEELBLUE :
		RR = R1 = (float ) 0.274510; RG = R2 = (float ) 0.509804; RB = R3 = (float ) 0.705882;
	break;
	case Quantity_NOC_STEELBLUE1 :
		RR = R1 = (float ) 0.388235; RG = R2 = (float ) 0.721569; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_STEELBLUE2 :
		RR = R1 = (float ) 0.360784; RG = R2 = (float ) 0.674510; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_STEELBLUE3 :
		RR = R1 = (float ) 0.309804; RG = R2 = (float ) 0.580392; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_STEELBLUE4 :
		RR = R1 = (float ) 0.211765; RG = R2 = (float ) 0.392157; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_TAN :
		RR = R1 = (float ) 0.823529; RG = R2 = (float ) 0.705882; RB = R3 = (float ) 0.549020;
	break;
	case Quantity_NOC_TAN1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.647059; RB = R3 = (float ) 0.309804;
	break;
	case Quantity_NOC_TAN2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.603922; RB = R3 = (float ) 0.286275;
	break;
	case Quantity_NOC_TAN3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.521569; RB = R3 = (float ) 0.247059;
	break;
	case Quantity_NOC_TAN4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.352941; RB = R3 = (float ) 0.168627;
	break;
	case Quantity_NOC_THISTLE :
		RR = R1 = (float ) 0.847059; RG = R2 = (float ) 0.749020; RB = R3 = (float ) 0.847059;
	break;
	case Quantity_NOC_THISTLE1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.882353; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_THISTLE2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.823529; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_THISTLE3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.709804; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_THISTLE4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.482353; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_TOMATO :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.388235; RB = R3 = (float ) 0.278431;
	break;
	case Quantity_NOC_TOMATO1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.388235; RB = R3 = (float ) 0.278431;
	break;
	case Quantity_NOC_TOMATO2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.360784; RB = R3 = (float ) 0.258824;
	break;
	case Quantity_NOC_TOMATO3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.309804; RB = R3 = (float ) 0.223529;
	break;
	case Quantity_NOC_TOMATO4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.211765; RB = R3 = (float ) 0.149020;
	break;
	case Quantity_NOC_TURQUOISE :
		RR = R1 = (float ) 0.250980; RG = R2 = (float ) 0.878431; RB = R3 = (float ) 0.815686;
	break;
	case Quantity_NOC_TURQUOISE1 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.960784; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_TURQUOISE2 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.898039; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_TURQUOISE3 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.772549; RB = R3 = (float ) 0.803922;
	break;
	case Quantity_NOC_TURQUOISE4 :
		RR = R1 = 0.000000; RG = R2 = (float ) 0.525490; RB = R3 = (float ) 0.545098;
	break;
	case Quantity_NOC_VIOLET :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.509804; RB = R3 = (float ) 0.933333;
	break;
	case Quantity_NOC_VIOLETRED :
		RR = R1 = (float ) 0.815686; RG = R2 = (float ) 0.125490; RB = R3 = (float ) 0.564706;
	break;
	case Quantity_NOC_VIOLETRED1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.243137; RB = R3 = (float ) 0.588235;
	break;
	case Quantity_NOC_VIOLETRED2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.227451; RB = R3 = (float ) 0.549020;
	break;
	case Quantity_NOC_VIOLETRED3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.196078; RB = R3 = (float ) 0.470588;
	break;
	case Quantity_NOC_VIOLETRED4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.133333; RB = R3 = (float ) 0.321569;
	break;
	case Quantity_NOC_WHEAT :
		RR = R1 = (float ) 0.960784; RG = R2 = (float ) 0.870588; RB = R3 = (float ) 0.701961;
	break;
	case Quantity_NOC_WHEAT1 :
		RR = R1 = 1.000000; RG = R2 = (float ) 0.905882; RB = R3 = (float ) 0.729412;
	break;
	case Quantity_NOC_WHEAT2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.847059; RB = R3 = (float ) 0.682353;
	break;
	case Quantity_NOC_WHEAT3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.729412; RB = R3 = (float ) 0.588235;
	break;
	case Quantity_NOC_WHEAT4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.494118; RB = R3 = (float ) 0.400000;
	break;
	case Quantity_NOC_WHITE :
		RR = R1 = 1.000000; RG = R2 = 1.000000; RB = R3 = 1.000000;
	break;
	case Quantity_NOC_WHITESMOKE :
		RR = R1 = (float ) 0.960784; RG = R2 = (float ) 0.960784; RB = R3 = (float ) 0.960784;
	break;
	case Quantity_NOC_YELLOW :
		RR = R1 = 1.000000; RG = R2 = 1.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_YELLOW1 :
		RR = R1 = 1.000000; RG = R2 = 1.000000; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_YELLOW2 :
		RR = R1 = (float ) 0.933333; RG = R2 = (float ) 0.933333; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_YELLOW3 :
		RR = R1 = (float ) 0.803922; RG = R2 = (float ) 0.803922; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_YELLOW4 :
		RR = R1 = (float ) 0.545098; RG = R2 = (float ) 0.545098; RB = R3 = 0.000000;
	break;
	case Quantity_NOC_YELLOWGREEN :
		RR = R1 = (float ) 0.603922; RG = R2 = (float ) 0.803922; RB = R3 = (float ) 0.196078;
	break;
	default :
		throw Standard_OutOfRange("Bad name");
	break;
}
	if (AType == Quantity_TOC_HLS)
	    Quantity_Color::rgbhls (RR, RG, RB, R1, R2, R3);
}

Standard_CString Quantity_Color::StringName (
                                 const Quantity_NameOfColor AName) {

Standard_CString TheName;
 
switch (AName) {

	case Quantity_NOC_BLACK :
		TheName = "BLACK";
	break;
	case Quantity_NOC_MATRAGRAY :
		TheName = "MATRAGRAY";
	break;
	case Quantity_NOC_MATRABLUE :
		TheName = "MATRABLUE";
	break;
	case Quantity_NOC_ALICEBLUE :
		TheName = "ALICEBLUE";
	break;
	case Quantity_NOC_ANTIQUEWHITE :
		TheName = "ANTIQUEWHITE";
	break;
	case Quantity_NOC_ANTIQUEWHITE1 :
		TheName = "ANTIQUEWHITE1";
	break;
	case Quantity_NOC_ANTIQUEWHITE2 :
		TheName = "ANTIQUEWHITE2";
	break;
	case Quantity_NOC_ANTIQUEWHITE3 :
		TheName = "ANTIQUEWHITE3";
	break;
	case Quantity_NOC_ANTIQUEWHITE4 :
		TheName = "ANTIQUEWHITE4";
	break;
	case Quantity_NOC_AQUAMARINE1 :
		TheName = "AQUAMARINE1";
	break;
	case Quantity_NOC_AQUAMARINE2 :
		TheName = "AQUAMARINE2";
	break;
	case Quantity_NOC_AQUAMARINE4 :
		TheName = "AQUAMARINE4";
	break;
	case Quantity_NOC_AZURE :
		TheName = "AZURE";
	break;
	case Quantity_NOC_AZURE2 :
		TheName = "AZURE2";
	break;
	case Quantity_NOC_AZURE3 :
		TheName = "AZURE3";
	break;
	case Quantity_NOC_AZURE4 :
		TheName = "AZURE4";
	break;
	case Quantity_NOC_BEIGE :
		TheName = "BEIGE";
	break;
	case Quantity_NOC_BISQUE :
		TheName = "BISQUE";
	break;
	case Quantity_NOC_BISQUE2 :
		TheName = "BISQUE2";
	break;
	case Quantity_NOC_BISQUE3 :
		TheName = "BISQUE3";
	break;
	case Quantity_NOC_BISQUE4 :
		TheName = "BISQUE4";
	break;
	case Quantity_NOC_BLANCHEDALMOND :
		TheName = "BLANCHEDALMOND";
	break;
	case Quantity_NOC_BLUE1 :
		TheName = "BLUE1";
	break;
	case Quantity_NOC_BLUE2 :
		TheName = "BLUE2";
	break;
	case Quantity_NOC_BLUE3 :
		TheName = "BLUE3";
	break;
	case Quantity_NOC_BLUE4 :
		TheName = "BLUE4";
	break;
	case Quantity_NOC_BLUEVIOLET :
		TheName = "BLUEVIOLET";
	break;
	case Quantity_NOC_BROWN :
		TheName = "BROWN";
	break;
	case Quantity_NOC_BROWN1 :
		TheName = "BROWN1";
	break;
	case Quantity_NOC_BROWN2 :
		TheName = "BROWN2";
	break;
	case Quantity_NOC_BROWN3 :
		TheName = "BROWN3";
	break;
	case Quantity_NOC_BROWN4 :
		TheName = "BROWN4";
	break;
	case Quantity_NOC_BURLYWOOD :
		TheName = "BURLYWOOD";
	break;
	case Quantity_NOC_BURLYWOOD1 :
		TheName = "BURLYWOOD1";
	break;
	case Quantity_NOC_BURLYWOOD2 :
		TheName = "BURLYWOOD2";
	break;
	case Quantity_NOC_BURLYWOOD3 :
		TheName = "BURLYWOOD3";
	break;
	case Quantity_NOC_BURLYWOOD4 :
		TheName = "BURLYWOOD4";
	break;
	case Quantity_NOC_CADETBLUE :
		TheName = "CADETBLUE";
	break;
	case Quantity_NOC_CADETBLUE1 :
		TheName = "CADETBLUE1";
	break;
	case Quantity_NOC_CADETBLUE2 :
		TheName = "CADETBLUE2";
	break;
	case Quantity_NOC_CADETBLUE3 :
		TheName = "CADETBLUE3";
	break;
	case Quantity_NOC_CADETBLUE4 :
		TheName = "CADETBLUE4";
	break;
	case Quantity_NOC_CHARTREUSE :
		TheName = "CHARTREUSE";
	break;
	case Quantity_NOC_CHARTREUSE1 :
		TheName = "CHARTREUSE1";
	break;
	case Quantity_NOC_CHARTREUSE2 :
		TheName = "CHARTREUSE2";
	break;
	case Quantity_NOC_CHARTREUSE3 :
		TheName = "CHARTREUSE3";
	break;
	case Quantity_NOC_CHARTREUSE4 :
		TheName = "CHARTREUSE4";
	break;
	case Quantity_NOC_CHOCOLATE :
		TheName = "CHOCOLATE";
	break;
	case Quantity_NOC_CHOCOLATE1 :
		TheName = "CHOCOLATE1";
	break;
	case Quantity_NOC_CHOCOLATE2 :
		TheName = "CHOCOLATE2";
	break;
	case Quantity_NOC_CHOCOLATE3 :
		TheName = "CHOCOLATE3";
	break;
	case Quantity_NOC_CHOCOLATE4 :
		TheName = "CHOCOLATE4";
	break;
	case Quantity_NOC_CORAL :
		TheName = "CORAL";
	break;
	case Quantity_NOC_CORAL1 :
		TheName = "CORAL1";
	break;
	case Quantity_NOC_CORAL2 :
		TheName = "CORAL2";
	break;
	case Quantity_NOC_CORAL3 :
		TheName = "CORAL3";
	break;
	case Quantity_NOC_CORAL4 :
		TheName = "CORAL4";
	break;
	case Quantity_NOC_CORNFLOWERBLUE :
		TheName = "CORNFLOWERBLUE";
	break;
	case Quantity_NOC_CORNSILK1 :
		TheName = "CORNSILK1";
	break;
	case Quantity_NOC_CORNSILK2 :
		TheName = "CORNSILK2";
	break;
	case Quantity_NOC_CORNSILK3 :
		TheName = "CORNSILK3";
	break;
	case Quantity_NOC_CORNSILK4 :
		TheName = "CORNSILK4";
	break;
	case Quantity_NOC_CYAN1 :
		TheName = "CYAN1";
	break;
	case Quantity_NOC_CYAN2 :
		TheName = "CYAN2";
	break;
	case Quantity_NOC_CYAN3 :
		TheName = "CYAN3";
	break;
	case Quantity_NOC_CYAN4 :
		TheName = "CYAN4";
	break;
	case Quantity_NOC_DARKGOLDENROD :
		TheName = "DARKGOLDENROD";
	break;
	case Quantity_NOC_DARKGOLDENROD1 :
		TheName = "DARKGOLDENROD1";
	break;
	case Quantity_NOC_DARKGOLDENROD2 :
		TheName = "DARKGOLDENROD2";
	break;
	case Quantity_NOC_DARKGOLDENROD3 :
		TheName = "DARKGOLDENROD3";
	break;
	case Quantity_NOC_DARKGOLDENROD4 :
		TheName = "DARKGOLDENROD4";
	break;
	case Quantity_NOC_DARKGREEN :
		TheName = "DARKGREEN";
	break;
	case Quantity_NOC_DARKKHAKI :
		TheName = "DARKKHAKI";
	break;
	case Quantity_NOC_DARKOLIVEGREEN :
		TheName = "DARKOLIVEGREEN";
	break;
	case Quantity_NOC_DARKOLIVEGREEN1 :
		TheName = "DARKOLIVEGREEN1";
	break;
	case Quantity_NOC_DARKOLIVEGREEN2 :
		TheName = "DARKOLIVEGREEN2";
	break;
	case Quantity_NOC_DARKOLIVEGREEN3 :
		TheName = "DARKOLIVEGREEN3";
	break;
	case Quantity_NOC_DARKOLIVEGREEN4 :
		TheName = "DARKOLIVEGREEN4";
	break;
	case Quantity_NOC_DARKORANGE :
		TheName = "DARKORANGE";
	break;
	case Quantity_NOC_DARKORANGE1 :
		TheName = "DARKORANGE1";
	break;
	case Quantity_NOC_DARKORANGE2 :
		TheName = "DARKORANGE2";
	break;
	case Quantity_NOC_DARKORANGE3 :
		TheName = "DARKORANGE3";
	break;
	case Quantity_NOC_DARKORANGE4 :
		TheName = "DARKORANGE4";
	break;
	case Quantity_NOC_DARKORCHID :
		TheName = "DARKORCHID";
	break;
	case Quantity_NOC_DARKORCHID1 :
		TheName = "DARKORCHID1";
	break;
	case Quantity_NOC_DARKORCHID2 :
		TheName = "DARKORCHID2";
	break;
	case Quantity_NOC_DARKORCHID3 :
		TheName = "DARKORCHID3";
	break;
	case Quantity_NOC_DARKORCHID4 :
		TheName = "DARKORCHID4";
	break;
	case Quantity_NOC_DARKSALMON :
		TheName = "DARKSALMON";
	break;
	case Quantity_NOC_DARKSEAGREEN :
		TheName = "DARKSEAGREEN";
	break;
	case Quantity_NOC_DARKSEAGREEN1 :
		TheName = "DARKSEAGREEN1";
	break;
	case Quantity_NOC_DARKSEAGREEN2 :
		TheName = "DARKSEAGREEN2";
	break;
	case Quantity_NOC_DARKSEAGREEN3 :
		TheName = "DARKSEAGREEN3";
	break;
	case Quantity_NOC_DARKSEAGREEN4 :
		TheName = "DARKSEAGREEN4";
	break;
	case Quantity_NOC_DARKSLATEBLUE :
		TheName = "DARKSLATEBLUE";
	break;
	case Quantity_NOC_DARKSLATEGRAY1 :
		TheName = "DARKSLATEGRAY1";
	break;
	case Quantity_NOC_DARKSLATEGRAY2 :
		TheName = "DARKSLATEGRAY2";
	break;
	case Quantity_NOC_DARKSLATEGRAY3 :
		TheName = "DARKSLATEGRAY3";
	break;
	case Quantity_NOC_DARKSLATEGRAY4 :
		TheName = "DARKSLATEGRAY4";
	break;
	case Quantity_NOC_DARKSLATEGRAY :
		TheName = "DARKSLATEGRAY";
	break;
	case Quantity_NOC_DARKTURQUOISE :
		TheName = "DARKTURQUOISE";
	break;
	case Quantity_NOC_DARKVIOLET :
		TheName = "DARKVIOLET";
	break;
	case Quantity_NOC_DEEPPINK :
		TheName = "DEEPPINK";
	break;
	case Quantity_NOC_DEEPPINK2 :
		TheName = "DEEPPINK2";
	break;
	case Quantity_NOC_DEEPPINK3 :
		TheName = "DEEPPINK3";
	break;
	case Quantity_NOC_DEEPPINK4 :
		TheName = "DEEPPINK4";
	break;
	case Quantity_NOC_DEEPSKYBLUE1 :
		TheName = "DEEPSKYBLUE1";
	break;
	case Quantity_NOC_DEEPSKYBLUE2 :
		TheName = "DEEPSKYBLUE2";
	break;
	case Quantity_NOC_DEEPSKYBLUE3 :
		TheName = "DEEPSKYBLUE3";
	break;
	case Quantity_NOC_DEEPSKYBLUE4 :
		TheName = "DEEPSKYBLUE4";
	break;
	case Quantity_NOC_DODGERBLUE1 :
		TheName = "DODGERBLUE1";
	break;
	case Quantity_NOC_DODGERBLUE2 :
		TheName = "DODGERBLUE2";
	break;
	case Quantity_NOC_DODGERBLUE3 :
		TheName = "DODGERBLUE3";
	break;
	case Quantity_NOC_DODGERBLUE4 :
		TheName = "DODGERBLUE4";
	break;
	case Quantity_NOC_FIREBRICK :
		TheName = "FIREBRICK";
	break;
	case Quantity_NOC_FIREBRICK1 :
		TheName = "FIREBRICK1";
	break;
	case Quantity_NOC_FIREBRICK2 :
		TheName = "FIREBRICK2";
	break;
	case Quantity_NOC_FIREBRICK3 :
		TheName = "FIREBRICK3";
	break;
	case Quantity_NOC_FIREBRICK4 :
		TheName = "FIREBRICK4";
	break;
	case Quantity_NOC_FLORALWHITE :
		TheName = "FLORALWHITE";
	break;
	case Quantity_NOC_FORESTGREEN :
		TheName = "FORESTGREEN";
	break;
	case Quantity_NOC_GAINSBORO :
		TheName = "GAINSBORO";
	break;
	case Quantity_NOC_GHOSTWHITE :
		TheName = "GHOSTWHITE";
	break;
	case Quantity_NOC_GOLD :
		TheName = "GOLD";
	break;
	case Quantity_NOC_GOLD1 :
		TheName = "GOLD1";
	break;
	case Quantity_NOC_GOLD2 :
		TheName = "GOLD2";
	break;
	case Quantity_NOC_GOLD3 :
		TheName = "GOLD3";
	break;
	case Quantity_NOC_GOLD4 :
		TheName = "GOLD4";
	break;
	case Quantity_NOC_GOLDENROD :
		TheName = "GOLDENROD";
	break;
	case Quantity_NOC_GOLDENROD1 :
		TheName = "GOLDENROD1";
	break;
	case Quantity_NOC_GOLDENROD2 :
		TheName = "GOLDENROD2";
	break;
	case Quantity_NOC_GOLDENROD3 :
		TheName = "GOLDENROD3";
	break;
	case Quantity_NOC_GOLDENROD4 :
		TheName = "GOLDENROD4";
	break;
	case Quantity_NOC_GRAY :
		TheName = "GRAY";
	break;
	case Quantity_NOC_GRAY0 :
		TheName = "GRAY0";
	break;
	case Quantity_NOC_GRAY1 :
		TheName = "GRAY1";
	break;
	case Quantity_NOC_GRAY10 :
		TheName = "GRAY10";
	break;
	case Quantity_NOC_GRAY11 :
		TheName = "GRAY11";
	break;
	case Quantity_NOC_GRAY12 :
		TheName = "GRAY12";
	break;
	case Quantity_NOC_GRAY13 :
		TheName = "GRAY13";
	break;
	case Quantity_NOC_GRAY14 :
		TheName = "GRAY14";
	break;
	case Quantity_NOC_GRAY15 :
		TheName = "GRAY15";
	break;
	case Quantity_NOC_GRAY16 :
		TheName = "GRAY16";
	break;
	case Quantity_NOC_GRAY17 :
		TheName = "GRAY17";
	break;
	case Quantity_NOC_GRAY18 :
		TheName = "GRAY18";
	break;
	case Quantity_NOC_GRAY19 :
		TheName = "GRAY19";
	break;
	case Quantity_NOC_GRAY2 :
		TheName = "GRAY2";
	break;
	case Quantity_NOC_GRAY20 :
		TheName = "GRAY20";
	break;
	case Quantity_NOC_GRAY21 :
		TheName = "GRAY21";
	break;
	case Quantity_NOC_GRAY22 :
		TheName = "GRAY22";
	break;
	case Quantity_NOC_GRAY23 :
		TheName = "GRAY23";
	break;
	case Quantity_NOC_GRAY24 :
		TheName = "GRAY24";
	break;
	case Quantity_NOC_GRAY25 :
		TheName = "GRAY25";
	break;
	case Quantity_NOC_GRAY26 :
		TheName = "GRAY26";
	break;
	case Quantity_NOC_GRAY27 :
		TheName = "GRAY27";
	break;
	case Quantity_NOC_GRAY28 :
		TheName = "GRAY28";
	break;
	case Quantity_NOC_GRAY29 :
		TheName = "GRAY29";
	break;
	case Quantity_NOC_GRAY3 :
		TheName = "GRAY3";
	break;
	case Quantity_NOC_GRAY30 :
		TheName = "GRAY30";
	break;
	case Quantity_NOC_GRAY31 :
		TheName = "GRAY31";
	break;
	case Quantity_NOC_GRAY32 :
		TheName = "GRAY32";
	break;
	case Quantity_NOC_GRAY33 :
		TheName = "GRAY33";
	break;
	case Quantity_NOC_GRAY34 :
		TheName = "GRAY34";
	break;
	case Quantity_NOC_GRAY35 :
		TheName = "GRAY35";
	break;
	case Quantity_NOC_GRAY36 :
		TheName = "GRAY36";
	break;
	case Quantity_NOC_GRAY37 :
		TheName = "GRAY37";
	break;
	case Quantity_NOC_GRAY38 :
		TheName = "GRAY38";
	break;
	case Quantity_NOC_GRAY39 :
		TheName = "GRAY39";
	break;
	case Quantity_NOC_GRAY4 :
		TheName = "GRAY4";
	break;
	case Quantity_NOC_GRAY40 :
		TheName = "GRAY40";
	break;
	case Quantity_NOC_GRAY41 :
		TheName = "GRAY41";
	break;
	case Quantity_NOC_GRAY42 :
		TheName = "GRAY42";
	break;
	case Quantity_NOC_GRAY43 :
		TheName = "GRAY43";
	break;
	case Quantity_NOC_GRAY44 :
		TheName = "GRAY44";
	break;
	case Quantity_NOC_GRAY45 :
		TheName = "GRAY45";
	break;
	case Quantity_NOC_GRAY46 :
		TheName = "GRAY46";
	break;
	case Quantity_NOC_GRAY47 :
		TheName = "GRAY47";
	break;
	case Quantity_NOC_GRAY48 :
		TheName = "GRAY48";
	break;
	case Quantity_NOC_GRAY49 :
		TheName = "GRAY49";
	break;
	case Quantity_NOC_GRAY5 :
		TheName = "GRAY5";
	break;
	case Quantity_NOC_GRAY50 :
		TheName = "GRAY50";
	break;
	case Quantity_NOC_GRAY51 :
		TheName = "GRAY51";
	break;
	case Quantity_NOC_GRAY52 :
		TheName = "GRAY52";
	break;
	case Quantity_NOC_GRAY53 :
		TheName = "GRAY53";
	break;
	case Quantity_NOC_GRAY54 :
		TheName = "GRAY54";
	break;
	case Quantity_NOC_GRAY55 :
		TheName = "GRAY55";
	break;
	case Quantity_NOC_GRAY56 :
		TheName = "GRAY56";
	break;
	case Quantity_NOC_GRAY57 :
		TheName = "GRAY57";
	break;
	case Quantity_NOC_GRAY58 :
		TheName = "GRAY58";
	break;
	case Quantity_NOC_GRAY59 :
		TheName = "GRAY59";
	break;
	case Quantity_NOC_GRAY6 :
		TheName = "GRAY6";
	break;
	case Quantity_NOC_GRAY60 :
		TheName = "GRAY60";
	break;
	case Quantity_NOC_GRAY61 :
		TheName = "GRAY61";
	break;
	case Quantity_NOC_GRAY62 :
		TheName = "GRAY62";
	break;
	case Quantity_NOC_GRAY63 :
		TheName = "GRAY63";
	break;
	case Quantity_NOC_GRAY64 :
		TheName = "GRAY64";
	break;
	case Quantity_NOC_GRAY65 :
		TheName = "GRAY65";
	break;
	case Quantity_NOC_GRAY66 :
		TheName = "GRAY66";
	break;
	case Quantity_NOC_GRAY67 :
		TheName = "GRAY67";
	break;
	case Quantity_NOC_GRAY68 :
		TheName = "GRAY68";
	break;
	case Quantity_NOC_GRAY69 :
		TheName = "GRAY69";
	break;
	case Quantity_NOC_GRAY7 :
		TheName = "GRAY7";
	break;
	case Quantity_NOC_GRAY70 :
		TheName = "GRAY70";
	break;
	case Quantity_NOC_GRAY71 :
		TheName = "GRAY71";
	break;
	case Quantity_NOC_GRAY72 :
		TheName = "GRAY72";
	break;
	case Quantity_NOC_GRAY73 :
		TheName = "GRAY73";
	break;
	case Quantity_NOC_GRAY74 :
		TheName = "GRAY74";
	break;
	case Quantity_NOC_GRAY75 :
		TheName = "GRAY75";
	break;
	case Quantity_NOC_GRAY76 :
		TheName = "GRAY76";
	break;
	case Quantity_NOC_GRAY77 :
		TheName = "GRAY77";
	break;
	case Quantity_NOC_GRAY78 :
		TheName = "GRAY78";
	break;
	case Quantity_NOC_GRAY79 :
		TheName = "GRAY79";
	break;
	case Quantity_NOC_GRAY8 :
		TheName = "GRAY8";
	break;
	case Quantity_NOC_GRAY80 :
		TheName = "GRAY80";
	break;
	case Quantity_NOC_GRAY81 :
		TheName = "GRAY81";
	break;
	case Quantity_NOC_GRAY82 :
		TheName = "GRAY82";
	break;
	case Quantity_NOC_GRAY83 :
		TheName = "GRAY83";
	break;
	case Quantity_NOC_GRAY85 :
		TheName = "GRAY85";
	break;
	case Quantity_NOC_GRAY86 :
		TheName = "GRAY86";
	break;
	case Quantity_NOC_GRAY87 :
		TheName = "GRAY87";
	break;
	case Quantity_NOC_GRAY88 :
		TheName = "GRAY88";
	break;
	case Quantity_NOC_GRAY89 :
		TheName = "GRAY89";
	break;
	case Quantity_NOC_GRAY9 :
		TheName = "GRAY9";
	break;
	case Quantity_NOC_GRAY90 :
		TheName = "GRAY90";
	break;
	case Quantity_NOC_GRAY91 :
		TheName = "GRAY91";
	break;
	case Quantity_NOC_GRAY92 :
		TheName = "GRAY92";
	break;
	case Quantity_NOC_GRAY93 :
		TheName = "GRAY93";
	break;
	case Quantity_NOC_GRAY94 :
		TheName = "GRAY94";
	break;
	case Quantity_NOC_GRAY95 :
		TheName = "GRAY95";
	break;
	case Quantity_NOC_GREEN :
		TheName = "GREEN";
	break;
	case Quantity_NOC_GREEN1 :
		TheName = "GREEN1";
	break;
	case Quantity_NOC_GREEN2 :
		TheName = "GREEN2";
	break;
	case Quantity_NOC_GREEN3 :
		TheName = "GREEN3";
	break;
	case Quantity_NOC_GREEN4 :
		TheName = "GREEN4";
	break;
	case Quantity_NOC_GREENYELLOW :
		TheName = "GREENYELLOW";
	break;
	case Quantity_NOC_GRAY97 :
		TheName = "GRAY97";
	break;
	case Quantity_NOC_GRAY98 :
		TheName = "GRAY98";
	break;
	case Quantity_NOC_GRAY99 :
		TheName = "GRAY99";
	break;
	case Quantity_NOC_HONEYDEW :
		TheName = "HONEYDEW";
	break;
	case Quantity_NOC_HONEYDEW2 :
		TheName = "HONEYDEW2";
	break;
	case Quantity_NOC_HONEYDEW3 :
		TheName = "HONEYDEW3";
	break;
	case Quantity_NOC_HONEYDEW4 :
		TheName = "HONEYDEW4";
	break;
	case Quantity_NOC_HOTPINK :
		TheName = "HOTPINK";
	break;
	case Quantity_NOC_HOTPINK1 :
		TheName = "HOTPINK1";
	break;
	case Quantity_NOC_HOTPINK2 :
		TheName = "HOTPINK2";
	break;
	case Quantity_NOC_HOTPINK3 :
		TheName = "HOTPINK3";
	break;
	case Quantity_NOC_HOTPINK4 :
		TheName = "HOTPINK4";
	break;
	case Quantity_NOC_INDIANRED :
		TheName = "INDIANRED";
	break;
	case Quantity_NOC_INDIANRED1 :
		TheName = "INDIANRED1";
	break;
	case Quantity_NOC_INDIANRED2 :
		TheName = "INDIANRED2";
	break;
	case Quantity_NOC_INDIANRED3 :
		TheName = "INDIANRED3";
	break;
	case Quantity_NOC_INDIANRED4 :
		TheName = "INDIANRED4";
	break;
	case Quantity_NOC_IVORY :
		TheName = "IVORY";
	break;
	case Quantity_NOC_IVORY2 :
		TheName = "IVORY2";
	break;
	case Quantity_NOC_IVORY3 :
		TheName = "IVORY3";
	break;
	case Quantity_NOC_IVORY4 :
		TheName = "IVORY4";
	break;
	case Quantity_NOC_KHAKI :
		TheName = "KHAKI";
	break;
	case Quantity_NOC_KHAKI1 :
		TheName = "KHAKI1";
	break;
	case Quantity_NOC_KHAKI2 :
		TheName = "KHAKI2";
	break;
	case Quantity_NOC_KHAKI3 :
		TheName = "KHAKI3";
	break;
	case Quantity_NOC_KHAKI4 :
		TheName = "KHAKI4";
	break;
	case Quantity_NOC_LAVENDER :
		TheName = "LAVENDER";
	break;
	case Quantity_NOC_LAVENDERBLUSH1 :
		TheName = "LAVENDERBLUSH1";
	break;
	case Quantity_NOC_LAVENDERBLUSH2 :
		TheName = "LAVENDERBLUSH2";
	break;
	case Quantity_NOC_LAVENDERBLUSH3 :
		TheName = "LAVENDERBLUSH3";
	break;
	case Quantity_NOC_LAVENDERBLUSH4 :
		TheName = "LAVENDERBLUSH4";
	break;
	case Quantity_NOC_LAWNGREEN :
		TheName = "LAWNGREEN";
	break;
	case Quantity_NOC_LEMONCHIFFON1 :
		TheName = "LEMONCHIFFON1";
	break;
	case Quantity_NOC_LEMONCHIFFON2 :
		TheName = "LEMONCHIFFON2";
	break;
	case Quantity_NOC_LEMONCHIFFON3 :
		TheName = "LEMONCHIFFON3";
	break;
	case Quantity_NOC_LEMONCHIFFON4 :
		TheName = "LEMONCHIFFON4";
	break;
	case Quantity_NOC_LIGHTBLUE :
		TheName = "LIGHTBLUE";
	break;
	case Quantity_NOC_LIGHTBLUE1 :
		TheName = "LIGHTBLUE1";
	break;
	case Quantity_NOC_LIGHTBLUE2 :
		TheName = "LIGHTBLUE2";
	break;
	case Quantity_NOC_LIGHTBLUE3 :
		TheName = "LIGHTBLUE3";
	break;
	case Quantity_NOC_LIGHTBLUE4 :
		TheName = "LIGHTBLUE4";
	break;
	case Quantity_NOC_LIGHTCORAL :
		TheName = "LIGHTCORAL";
	break;
	case Quantity_NOC_LIGHTCYAN1 :
		TheName = "LIGHTCYAN1";
	break;
	case Quantity_NOC_LIGHTCYAN2 :
		TheName = "LIGHTCYAN2";
	break;
	case Quantity_NOC_LIGHTCYAN3 :
		TheName = "LIGHTCYAN3";
	break;
	case Quantity_NOC_LIGHTCYAN4 :
		TheName = "LIGHTCYAN4";
	break;
	case Quantity_NOC_LIGHTGOLDENROD :
		TheName = "LIGHTGOLDENROD";
	break;
	case Quantity_NOC_LIGHTGOLDENROD1 :
		TheName = "LIGHTGOLDENROD1";
	break;
	case Quantity_NOC_LIGHTGOLDENROD2 :
		TheName = "LIGHTGOLDENROD2";
	break;
	case Quantity_NOC_LIGHTGOLDENROD3 :
		TheName = "LIGHTGOLDENROD3";
	break;
	case Quantity_NOC_LIGHTGOLDENROD4 :
		TheName = "LIGHTGOLDENROD4";
	break;
	case Quantity_NOC_LIGHTGOLDENRODYELLOW :
		TheName = "LIGHTGOLDENRODYELLOW";
	break;
	case Quantity_NOC_LIGHTGRAY :
		TheName = "LIGHTGRAY";
	break;
	case Quantity_NOC_LIGHTPINK :
		TheName = "LIGHTPINK";
	break;
	case Quantity_NOC_LIGHTPINK1 :
		TheName = "LIGHTPINK1";
	break;
	case Quantity_NOC_LIGHTPINK2 :
		TheName = "LIGHTPINK2";
	break;
	case Quantity_NOC_LIGHTPINK3 :
		TheName = "LIGHTPINK3";
	break;
	case Quantity_NOC_LIGHTPINK4 :
		TheName = "LIGHTPINK4";
	break;
	case Quantity_NOC_LIGHTSALMON1 :
		TheName = "LIGHTSALMON1";
	break;
	case Quantity_NOC_LIGHTSALMON2 :
		TheName = "LIGHTSALMON2";
	break;
	case Quantity_NOC_LIGHTSALMON3 :
		TheName = "LIGHTSALMON3";
	break;
	case Quantity_NOC_LIGHTSALMON4 :
		TheName = "LIGHTSALMON4";
	break;
	case Quantity_NOC_LIGHTSEAGREEN :
		TheName = "LIGHTSEAGREEN";
	break;
	case Quantity_NOC_LIGHTSKYBLUE :
		TheName = "LIGHTSKYBLUE";
	break;
	case Quantity_NOC_LIGHTSKYBLUE1 :
		TheName = "LIGHTSKYBLUE1";
	break;
	case Quantity_NOC_LIGHTSKYBLUE2 :
		TheName = "LIGHTSKYBLUE2";
	break;
	case Quantity_NOC_LIGHTSKYBLUE3 :
		TheName = "LIGHTSKYBLUE3";
	break;
	case Quantity_NOC_LIGHTSKYBLUE4 :
		TheName = "LIGHTSKYBLUE4";
	break;
	case Quantity_NOC_LIGHTSLATEBLUE :
		TheName = "LIGHTSLATEBLUE";
	break;
	case Quantity_NOC_LIGHTSLATEGRAY :
		TheName = "LIGHTSLATEGRAY";
	break;
	case Quantity_NOC_LIGHTSTEELBLUE :
		TheName = "LIGHTSTEELBLUE";
	break;
	case Quantity_NOC_LIGHTSTEELBLUE1 :
		TheName = "LIGHTSTEELBLUE1";
	break;
	case Quantity_NOC_LIGHTSTEELBLUE2 :
		TheName = "LIGHTSTEELBLUE2";
	break;
	case Quantity_NOC_LIGHTSTEELBLUE3 :
		TheName = "LIGHTSTEELBLUE3";
	break;
	case Quantity_NOC_LIGHTSTEELBLUE4 :
		TheName = "LIGHTSTEELBLUE4";
	break;
	case Quantity_NOC_LIGHTYELLOW :
		TheName = "LIGHTYELLOW";
	break;
	case Quantity_NOC_LIGHTYELLOW2 :
		TheName = "LIGHTYELLOW2";
	break;
	case Quantity_NOC_LIGHTYELLOW3 :
		TheName = "LIGHTYELLOW3";
	break;
	case Quantity_NOC_LIGHTYELLOW4 :
		TheName = "LIGHTYELLOW4";
	break;
	case Quantity_NOC_LIMEGREEN :
		TheName = "LIMEGREEN";
	break;
	case Quantity_NOC_LINEN :
		TheName = "LINEN";
	break;
	case Quantity_NOC_MAGENTA1 :
		TheName = "MAGENTA1";
	break;
	case Quantity_NOC_MAGENTA2 :
		TheName = "MAGENTA2";
	break;
	case Quantity_NOC_MAGENTA3 :
		TheName = "MAGENTA3";
	break;
	case Quantity_NOC_MAGENTA4 :
		TheName = "MAGENTA4";
	break;
	case Quantity_NOC_MAROON :
		TheName = "MAROON";
	break;
	case Quantity_NOC_MAROON1 :
		TheName = "MAROON1";
	break;
	case Quantity_NOC_MAROON2 :
		TheName = "MAROON2";
	break;
	case Quantity_NOC_MAROON3 :
		TheName = "MAROON3";
	break;
	case Quantity_NOC_MAROON4 :
		TheName = "MAROON4";
	break;
	case Quantity_NOC_MEDIUMAQUAMARINE :
		TheName = "MEDIUMAQUAMARINE";
	break;
	case Quantity_NOC_MEDIUMORCHID :
		TheName = "MEDIUMORCHID";
	break;
	case Quantity_NOC_MEDIUMORCHID1 :
		TheName = "MEDIUMORCHID1";
	break;
	case Quantity_NOC_MEDIUMORCHID2 :
		TheName = "MEDIUMORCHID2";
	break;
	case Quantity_NOC_MEDIUMORCHID3 :
		TheName = "MEDIUMORCHID3";
	break;
	case Quantity_NOC_MEDIUMORCHID4 :
		TheName = "MEDIUMORCHID4";
	break;
	case Quantity_NOC_MEDIUMPURPLE :
		TheName = "MEDIUMPURPLE";
	break;
	case Quantity_NOC_MEDIUMPURPLE1 :
		TheName = "MEDIUMPURPLE1";
	break;
	case Quantity_NOC_MEDIUMPURPLE2 :
		TheName = "MEDIUMPURPLE2";
	break;
	case Quantity_NOC_MEDIUMPURPLE3 :
		TheName = "MEDIUMPURPLE3";
	break;
	case Quantity_NOC_MEDIUMPURPLE4 :
		TheName = "MEDIUMPURPLE4";
	break;
	case Quantity_NOC_MEDIUMSEAGREEN :
		TheName = "MEDIUMSEAGREEN";
	break;
	case Quantity_NOC_MEDIUMSLATEBLUE :
		TheName = "MEDIUMSLATEBLUE";
	break;
	case Quantity_NOC_MEDIUMSPRINGGREEN :
		TheName = "MEDIUMSPRINGGREEN";
	break;
	case Quantity_NOC_MEDIUMTURQUOISE :
		TheName = "MEDIUMTURQUOISE";
	break;
	case Quantity_NOC_MEDIUMVIOLETRED :
		TheName = "MEDIUMVIOLETRED";
	break;
	case Quantity_NOC_MIDNIGHTBLUE :
		TheName = "MIDNIGHTBLUE";
	break;
	case Quantity_NOC_MINTCREAM :
		TheName = "MINTCREAM";
	break;
	case Quantity_NOC_MISTYROSE :
		TheName = "MISTYROSE";
	break;
	case Quantity_NOC_MISTYROSE2 :
		TheName = "MISTYROSE2";
	break;
	case Quantity_NOC_MISTYROSE3 :
		TheName = "MISTYROSE3";
	break;
	case Quantity_NOC_MISTYROSE4 :
		TheName = "MISTYROSE4";
	break;
	case Quantity_NOC_MOCCASIN :
		TheName = "MOCCASIN";
	break;
	case Quantity_NOC_NAVAJOWHITE1 :
		TheName = "NAVAJOWHITE1";
	break;
	case Quantity_NOC_NAVAJOWHITE2 :
		TheName = "NAVAJOWHITE2";
	break;
	case Quantity_NOC_NAVAJOWHITE3 :
		TheName = "NAVAJOWHITE3";
	break;
	case Quantity_NOC_NAVAJOWHITE4 :
		TheName = "NAVAJOWHITE4";
	break;
	case Quantity_NOC_NAVYBLUE :
		TheName = "NAVYBLUE";
	break;
	case Quantity_NOC_OLDLACE :
		TheName = "OLDLACE";
	break;
	case Quantity_NOC_OLIVEDRAB :
		TheName = "OLIVEDRAB";
	break;
	case Quantity_NOC_OLIVEDRAB1 :
		TheName = "OLIVEDRAB1";
	break;
	case Quantity_NOC_OLIVEDRAB2 :
		TheName = "OLIVEDRAB2";
	break;
	case Quantity_NOC_OLIVEDRAB3 :
		TheName = "OLIVEDRAB3";
	break;
	case Quantity_NOC_OLIVEDRAB4 :
		TheName = "OLIVEDRAB4";
	break;
	case Quantity_NOC_ORANGE :
		TheName = "ORANGE";
	break;
	case Quantity_NOC_ORANGE1 :
		TheName = "ORANGE1";
	break;
	case Quantity_NOC_ORANGE2 :
		TheName = "ORANGE2";
	break;
	case Quantity_NOC_ORANGE3 :
		TheName = "ORANGE3";
	break;
	case Quantity_NOC_ORANGE4 :
		TheName = "ORANGE4";
	break;
	case Quantity_NOC_ORANGERED :
		TheName = "ORANGERED";
	break;
	case Quantity_NOC_ORANGERED1 :
		TheName = "ORANGERED1";
	break;
	case Quantity_NOC_ORANGERED2 :
		TheName = "ORANGERED2";
	break;
	case Quantity_NOC_ORANGERED3 :
		TheName = "ORANGERED3";
	break;
	case Quantity_NOC_ORANGERED4 :
		TheName = "ORANGERED4";
	break;
	case Quantity_NOC_ORCHID :
		TheName = "ORCHID";
	break;
	case Quantity_NOC_ORCHID1 :
		TheName = "ORCHID1";
	break;
	case Quantity_NOC_ORCHID2 :
		TheName = "ORCHID2";
	break;
	case Quantity_NOC_ORCHID3 :
		TheName = "ORCHID3";
	break;
	case Quantity_NOC_ORCHID4 :
		TheName = "ORCHID4";
	break;
	case Quantity_NOC_PALEGOLDENROD :
		TheName = "PALEGOLDENROD";
	break;
	case Quantity_NOC_PALEGREEN :
		TheName = "PALEGREEN";
	break;
	case Quantity_NOC_PALEGREEN1 :
		TheName = "PALEGREEN1";
	break;
	case Quantity_NOC_PALEGREEN2 :
		TheName = "PALEGREEN2";
	break;
	case Quantity_NOC_PALEGREEN3 :
		TheName = "PALEGREEN3";
	break;
	case Quantity_NOC_PALEGREEN4 :
		TheName = "PALEGREEN4";
	break;
	case Quantity_NOC_PALETURQUOISE :
		TheName = "PALETURQUOISE";
	break;
	case Quantity_NOC_PALETURQUOISE1 :
		TheName = "PALETURQUOISE1";
	break;
	case Quantity_NOC_PALETURQUOISE2 :
		TheName = "PALETURQUOISE2";
	break;
	case Quantity_NOC_PALETURQUOISE3 :
		TheName = "PALETURQUOISE3";
	break;
	case Quantity_NOC_PALETURQUOISE4 :
		TheName = "PALETURQUOISE4";
	break;
	case Quantity_NOC_PALEVIOLETRED :
		TheName = "PALEVIOLETRED";
	break;
	case Quantity_NOC_PALEVIOLETRED1 :
		TheName = "PALEVIOLETRED1";
	break;
	case Quantity_NOC_PALEVIOLETRED2 :
		TheName = "PALEVIOLETRED2";
	break;
	case Quantity_NOC_PALEVIOLETRED3 :
		TheName = "PALEVIOLETRED3";
	break;
	case Quantity_NOC_PALEVIOLETRED4 :
		TheName = "PALEVIOLETRED4";
	break;
	case Quantity_NOC_PAPAYAWHIP :
		TheName = "PAPAYAWHIP";
	break;
	case Quantity_NOC_PEACHPUFF :
		TheName = "PEACHPUFF";
	break;
	case Quantity_NOC_PEACHPUFF2 :
		TheName = "PEACHPUFF2";
	break;
	case Quantity_NOC_PEACHPUFF3 :
		TheName = "PEACHPUFF3";
	break;
	case Quantity_NOC_PEACHPUFF4 :
		TheName = "PEACHPUFF4";
	break;
	case Quantity_NOC_PERU :
		TheName = "PERU";
	break;
	case Quantity_NOC_PINK :
		TheName = "PINK";
	break;
	case Quantity_NOC_PINK1 :
		TheName = "PINK1";
	break;
	case Quantity_NOC_PINK2 :
		TheName = "PINK2";
	break;
	case Quantity_NOC_PINK3 :
		TheName = "PINK3";
	break;
	case Quantity_NOC_PINK4 :
		TheName = "PINK4";
	break;
	case Quantity_NOC_PLUM :
		TheName = "PLUM";
	break;
	case Quantity_NOC_PLUM1 :
		TheName = "PLUM1";
	break;
	case Quantity_NOC_PLUM2 :
		TheName = "PLUM2";
	break;
	case Quantity_NOC_PLUM3 :
		TheName = "PLUM3";
	break;
	case Quantity_NOC_PLUM4 :
		TheName = "PLUM4";
	break;
	case Quantity_NOC_POWDERBLUE :
		TheName = "POWDERBLUE";
	break;
	case Quantity_NOC_PURPLE :
		TheName = "PURPLE";
	break;
	case Quantity_NOC_PURPLE1 :
		TheName = "PURPLE1";
	break;
	case Quantity_NOC_PURPLE2 :
		TheName = "PURPLE2";
	break;
	case Quantity_NOC_PURPLE3 :
		TheName = "PURPLE3";
	break;
	case Quantity_NOC_PURPLE4 :
		TheName = "PURPLE4";
	break;
	case Quantity_NOC_RED :
		TheName = "RED";
	break;
	case Quantity_NOC_RED1 :
		TheName = "RED1";
	break;
	case Quantity_NOC_RED2 :
		TheName = "RED2";
	break;
	case Quantity_NOC_RED3 :
		TheName = "RED3";
	break;
	case Quantity_NOC_RED4 :
		TheName = "RED4";
	break;
	case Quantity_NOC_ROSYBROWN :
		TheName = "ROSYBROWN";
	break;
	case Quantity_NOC_ROSYBROWN1 :
		TheName = "ROSYBROWN1";
	break;
	case Quantity_NOC_ROSYBROWN2 :
		TheName = "ROSYBROWN2";
	break;
	case Quantity_NOC_ROSYBROWN3 :
		TheName = "ROSYBROWN3";
	break;
	case Quantity_NOC_ROSYBROWN4 :
		TheName = "ROSYBROWN4";
	break;
	case Quantity_NOC_ROYALBLUE :
		TheName = "ROYALBLUE";
	break;
	case Quantity_NOC_ROYALBLUE1 :
		TheName = "ROYALBLUE1";
	break;
	case Quantity_NOC_ROYALBLUE2 :
		TheName = "ROYALBLUE2";
	break;
	case Quantity_NOC_ROYALBLUE3 :
		TheName = "ROYALBLUE3";
	break;
	case Quantity_NOC_ROYALBLUE4 :
		TheName = "ROYALBLUE4";
	break;
	case Quantity_NOC_SADDLEBROWN :
		TheName = "SADDLEBROWN";
	break;
	case Quantity_NOC_SALMON :
		TheName = "SALMON";
	break;
	case Quantity_NOC_SALMON1 :
		TheName = "SALMON1";
	break;
	case Quantity_NOC_SALMON2 :
		TheName = "SALMON2";
	break;
	case Quantity_NOC_SALMON3 :
		TheName = "SALMON3";
	break;
	case Quantity_NOC_SALMON4 :
		TheName = "SALMON4";
	break;
	case Quantity_NOC_SANDYBROWN :
		TheName = "SANDYBROWN";
	break;
	case Quantity_NOC_SEAGREEN :
		TheName = "SEAGREEN";
	break;
	case Quantity_NOC_SEAGREEN1 :
		TheName = "SEAGREEN1";
	break;
	case Quantity_NOC_SEAGREEN2 :
		TheName = "SEAGREEN2";
	break;
	case Quantity_NOC_SEAGREEN3 :
		TheName = "SEAGREEN3";
	break;
	case Quantity_NOC_SEAGREEN4 :
		TheName = "SEAGREEN4";
	break;
	case Quantity_NOC_SEASHELL :
		TheName = "SEASHELL";
	break;
	case Quantity_NOC_SEASHELL2 :
		TheName = "SEASHELL2";
	break;
	case Quantity_NOC_SEASHELL3 :
		TheName = "SEASHELL3";
	break;
	case Quantity_NOC_SEASHELL4 :
		TheName = "SEASHELL4";
	break;
	case Quantity_NOC_BEET :
		TheName = "BEET";
	break;
	case Quantity_NOC_TEAL :
		TheName = "TEAL";
	break;
	case Quantity_NOC_SIENNA :
		TheName = "SIENNA";
	break;
	case Quantity_NOC_SIENNA1 :
		TheName = "SIENNA1";
	break;
	case Quantity_NOC_SIENNA2 :
		TheName = "SIENNA2";
	break;
	case Quantity_NOC_SIENNA3 :
		TheName = "SIENNA3";
	break;
	case Quantity_NOC_SIENNA4 :
		TheName = "SIENNA4";
	break;
	case Quantity_NOC_SKYBLUE :
		TheName = "SKYBLUE";
	break;
	case Quantity_NOC_SKYBLUE1 :
		TheName = "SKYBLUE1";
	break;
	case Quantity_NOC_SKYBLUE2 :
		TheName = "SKYBLUE2";
	break;
	case Quantity_NOC_SKYBLUE3 :
		TheName = "SKYBLUE3";
	break;
	case Quantity_NOC_SKYBLUE4 :
		TheName = "SKYBLUE4";
	break;
	case Quantity_NOC_SLATEBLUE :
		TheName = "SLATEBLUE";
	break;
	case Quantity_NOC_SLATEBLUE1 :
		TheName = "SLATEBLUE1";
	break;
	case Quantity_NOC_SLATEBLUE2 :
		TheName = "SLATEBLUE2";
	break;
	case Quantity_NOC_SLATEBLUE3 :
		TheName = "SLATEBLUE3";
	break;
	case Quantity_NOC_SLATEBLUE4 :
		TheName = "SLATEBLUE4";
	break;
	case Quantity_NOC_SLATEGRAY1 :
		TheName = "SLATEGRAY1";
	break;
	case Quantity_NOC_SLATEGRAY2 :
		TheName = "SLATEGRAY2";
	break;
	case Quantity_NOC_SLATEGRAY3 :
		TheName = "SLATEGRAY3";
	break;
	case Quantity_NOC_SLATEGRAY4 :
		TheName = "SLATEGRAY4";
	break;
	case Quantity_NOC_SLATEGRAY :
		TheName = "SLATEGRAY";
	break;
	case Quantity_NOC_SNOW :
		TheName = "SNOW";
	break;
	case Quantity_NOC_SNOW2 :
		TheName = "SNOW2";
	break;
	case Quantity_NOC_SNOW3 :
		TheName = "SNOW3";
	break;
	case Quantity_NOC_SNOW4 :
		TheName = "SNOW4";
	break;
	case Quantity_NOC_SPRINGGREEN :
		TheName = "SPRINGGREEN";
	break;
	case Quantity_NOC_SPRINGGREEN2 :
		TheName = "SPRINGGREEN2";
	break;
	case Quantity_NOC_SPRINGGREEN3 :
		TheName = "SPRINGGREEN3";
	break;
	case Quantity_NOC_SPRINGGREEN4 :
		TheName = "SPRINGGREEN4";
	break;
	case Quantity_NOC_STEELBLUE :
		TheName = "STEELBLUE";
	break;
	case Quantity_NOC_STEELBLUE1 :
		TheName = "STEELBLUE1";
	break;
	case Quantity_NOC_STEELBLUE2 :
		TheName = "STEELBLUE2";
	break;
	case Quantity_NOC_STEELBLUE3 :
		TheName = "STEELBLUE3";
	break;
	case Quantity_NOC_STEELBLUE4 :
		TheName = "STEELBLUE4";
	break;
	case Quantity_NOC_TAN :
		TheName = "TAN";
	break;
	case Quantity_NOC_TAN1 :
		TheName = "TAN1";
	break;
	case Quantity_NOC_TAN2 :
		TheName = "TAN2";
	break;
	case Quantity_NOC_TAN3 :
		TheName = "TAN3";
	break;
	case Quantity_NOC_TAN4 :
		TheName = "TAN4";
	break;
	case Quantity_NOC_THISTLE :
		TheName = "THISTLE";
	break;
	case Quantity_NOC_THISTLE1 :
		TheName = "THISTLE1";
	break;
	case Quantity_NOC_THISTLE2 :
		TheName = "THISTLE2";
	break;
	case Quantity_NOC_THISTLE3 :
		TheName = "THISTLE3";
	break;
	case Quantity_NOC_THISTLE4 :
		TheName = "THISTLE4";
	break;
	case Quantity_NOC_TOMATO :
		TheName = "TOMATO";
	break;
	case Quantity_NOC_TOMATO1 :
		TheName = "TOMATO1";
	break;
	case Quantity_NOC_TOMATO2 :
		TheName = "TOMATO2";
	break;
	case Quantity_NOC_TOMATO3 :
		TheName = "TOMATO3";
	break;
	case Quantity_NOC_TOMATO4 :
		TheName = "TOMATO4";
	break;
	case Quantity_NOC_TURQUOISE :
		TheName = "TURQUOISE";
	break;
	case Quantity_NOC_TURQUOISE1 :
		TheName = "TURQUOISE1";
	break;
	case Quantity_NOC_TURQUOISE2 :
		TheName = "TURQUOISE2";
	break;
	case Quantity_NOC_TURQUOISE3 :
		TheName = "TURQUOISE3";
	break;
	case Quantity_NOC_TURQUOISE4 :
		TheName = "TURQUOISE4";
	break;
	case Quantity_NOC_VIOLET :
		TheName = "VIOLET";
	break;
	case Quantity_NOC_VIOLETRED :
		TheName = "VIOLETRED";
	break;
	case Quantity_NOC_VIOLETRED1 :
		TheName = "VIOLETRED1";
	break;
	case Quantity_NOC_VIOLETRED2 :
		TheName = "VIOLETRED2";
	break;
	case Quantity_NOC_VIOLETRED3 :
		TheName = "VIOLETRED3";
	break;
	case Quantity_NOC_VIOLETRED4 :
		TheName = "VIOLETRED4";
	break;
	case Quantity_NOC_WHEAT :
		TheName = "WHEAT";
	break;
	case Quantity_NOC_WHEAT1 :
		TheName = "WHEAT1";
	break;
	case Quantity_NOC_WHEAT2 :
		TheName = "WHEAT2";
	break;
	case Quantity_NOC_WHEAT3 :
		TheName = "WHEAT3";
	break;
	case Quantity_NOC_WHEAT4 :
		TheName = "WHEAT4";
	break;
	case Quantity_NOC_WHITE :
		TheName = "WHITE";
	break;
	case Quantity_NOC_WHITESMOKE :
		TheName = "WHITESMOKE";
	break;
	case Quantity_NOC_YELLOW :
		TheName = "YELLOW";
	break;
	case Quantity_NOC_YELLOW1 :
		TheName = "YELLOW1";
	break;
	case Quantity_NOC_YELLOW2 :
		TheName = "YELLOW2";
	break;
	case Quantity_NOC_YELLOW3 :
		TheName = "YELLOW3";
	break;
	case Quantity_NOC_YELLOW4 :
		TheName = "YELLOW4";
	break;
	case Quantity_NOC_YELLOWGREEN :
		TheName = "YELLOWGREEN";
	break;
	default :
		TheName = "UNDEFINED";
		throw Standard_OutOfRange("Bad name");
	break;
}
return (TheName);
}

//
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TESTS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Quantity_Color::Test () {

void TestOfColor ();

	try {
	  OCC_CATCH_SIGNALS
		TestOfColor ();
	}

	catch (Standard_Failure const& anException) {
		cout << anException << endl;
	}

}

void TestOfColor () {

Standard_Real H, L, S;
Standard_Real R, G, B;
Standard_Real DC, DI;
Standard_Integer i;

cout << "definition color tests\n----------------------\n";

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

cout << "Get values and names of color tests\n-----------------------------------\n";

	C1.Values (R, G, B, Quantity_TOC_RGB);
	if ( (R!=1.0) || (G!=1.0) || (B!=0.0) ) {
		cout << "TEST_ERROR : Values () bad default color\n";
		cout << "R, G, B values: " << R << " " << G << " " << B << "\n";
	}
	if ( (C1.Red ()!=1.0) || (C1.Green ()!=1.0) || (C1.Blue ()!=0.0) ) {
		cout << "TEST_ERROR : Values () bad default color\n";
		cout << "R, G, B values: " << C1.Red () << " " << C1.Green ()
			<< " " << C1.Blue () << "\n";
	}
	if (strcmp (Quantity_Color::StringName (C1.Name()), cyan) != 0)
		cout << "TEST_ERROR : StringName () " <<
			Quantity_Color::StringName (C1.Name()) << 
                                        " != YELLOW\n";

	RR=0.262745; GG=0.431373; BB=0.933333;
	C1.SetValues (RR, GG, BB, Quantity_TOC_RGB);
	C2.Values (R, G, B, Quantity_TOC_RGB);
	if ( (Abs (RR-R) > DELTA) ||
	     (Abs (GG-G) > DELTA) ||
	     (Abs (BB-B) > DELTA) ) {
		cout << "TEST_ERROR : Values () bad default color\n";
		cout << "R, G, B values: " << R << " " << G << " " << B << "\n";
	}

	if (C2 != C1) {
		cout << "TEST_ERROR : IsDifferent ()\n";
	}
	if (C3 == C1) {
		cout << "TEST_ERROR : IsEqual ()\n";
	}

	cout << "Distance C1,C2 " << C1.Distance (C2) << "\n";
	cout << "Distance C1,C3 " << C1.Distance (C3) << "\n";
	cout << "Distance C2,C3 " << C2.Distance (C3) << "\n";
	cout << "SquareDistance C1,C2 " << C1.SquareDistance (C2) << "\n";
	cout << "SquareDistance C1,C3 " << C1.SquareDistance (C3) << "\n";
	cout << "SquareDistance C2,C3 " << C2.SquareDistance (C3) << "\n";

	if (strcmp (Quantity_Color::StringName (C2.Name()), blue) != 0)
		cout << "TEST_ERROR : StringName () " <<
			Quantity_Color::StringName (C2.Name()) << 
                        " != ROYALBLUE2\n";

cout << "conversion rgbhls tests\n-----------------------\n";
	Quantity_Color::RgbHls (R, G, B, H, L, S);
	Quantity_Color::HlsRgb (H, L, S, R, G, B);
	RR=0.262745; GG=0.431373; BB=0.933333;
	if ( (Abs (RR-R) > DELTA) ||
	     (Abs (GG-G) > DELTA) ||
	     (Abs (BB-B) > DELTA) ) {
		cout << "TEST_ERROR : RgbHls or HlsRgb bad conversion\n";
		cout << "RGB init : " << RR << " " << GG << " " << BB << "\n";
		cout << "RGB values : " << R << " " << G << " " << B << "\n";
		cout << "Difference RGB : "
			<< RR-R << " " << GG-G << " " << BB-B << "\n";
	}

cout << "distance tests\n--------------\n";
	R = (float ) 0.9568631; G = (float ) 0.6431371; B = (float ) 0.3764711;
	C2.SetValues (R, G, B, Quantity_TOC_RGB);
	if (C2.Distance (C3) > DELTA) {
		cout << "TEST_ERROR : Distance () bad result\n";
		cout << "Distance C2 and C3 : " << C2.Distance (C3) << "\n";
	}

	C2.Delta (C3, DC, DI);
	if (Abs (DC) > DELTA)
		cout << "TEST_ERROR : Delta () bad result for DC\n";
	if (Abs (DI) > DELTA)
		cout << "TEST_ERROR : Delta () bad result for DI\n";

cout << "name tests\n----------\n";
	R = (float ) 0.9568631; G = (float ) 0.6431371; B = (float ) 0.3764711;
	C2.SetValues (R, G, B, Quantity_TOC_RGB);
	if (strcmp (Quantity_Color::StringName (C2.Name()), brown) != 0)
		cout << "TEST_ERROR : StringName () " <<
			Quantity_Color::StringName (C2.Name()) << 
                        " != SANDYBROWN\n";

cout << "contrast change tests\n---------------------\n";
	for (i=1; i<=10; i++) {
		C2.ChangeContrast (10.);
		C2.ChangeContrast (-9.09090909);
	}
	C2.Values (R, G, B, Quantity_TOC_RGB);
	RR=0.956863; GG=0.6431371; BB=0.3764711;
	if ( (Abs (RR-R) > DELTA) ||
	     (Abs (GG-G) > DELTA) ||
	     (Abs (BB-B) > DELTA) ) {
		cout << "TEST_ERROR : ChangeContrast () bad values\n";
		cout << "RGB init : " << RR << " " << GG << " " << BB << "\n";
		cout << "RGB values : " << R << " " << G << " " << B << "\n";
	}

}

/*
 *
 * Objet	:	Algorithme de conversion HLS vers RGB
 *
 * Rappels	:	RGB et HLS sont 2 modeles de representation
 *			des couleurs
 *
 * Parametres	:	h, l, s connus avec h dans [0,360] et l et s dans [0,1]
 *			r, g, b dans [0,1]
 *
 * Reference	:	La synthese d'images, Collection Hermes
 *
 * Adaptation	:	Cal	07 fevrier 1992
 *
 * Modification	:	?
 */

void call_hlsrgb (float h, float l, float s, float& r, float& g, float& b)
/* parametres d'entree : h, l, s connus avec h dans [0,360]
 * et l et s dans [0,1] */
/* parametres de sortie : r, g, b dans [0,1] */
{

	float hcopy;
	float lmuls;
	int hi;

	hcopy = h;
	lmuls = l * s;

	if (s == 0.0 && h == RGBHLS_H_UNDEFINED) {
	      /* cas achromatique */
	      r = g = b = l;
	}
	else {
	   /* cas chromatique */
	   if (hcopy == 360.0) {
	      hcopy = 0.0;
	      hi = 0;
	   }
	   else {
	      hcopy /= 60.0;
	      hi = (int)hcopy;
	   }

	   switch (hi) {
	      case 0 :
			/*
			   r = l;
			   g = l * (1 - (s * (1 - (hcopy - hi))));
			   b = l * (1 - s);
			 */
			r = l;
			b = l - lmuls;
			g = b + lmuls * hcopy;
	      break;
	      case 1 :
			/*
			   r = l * (1 - (s * (hcopy - hi)));
			   g = l;
			   b = l * (1 - s);
			 */
			r = l + lmuls - lmuls * hcopy;
			g = l;
			b = l - lmuls;
	      break;
	      case 2 :
			/*
			   r = l * (1 - s);
			   g = l;
			   b = l * (1 - (s * (1 - (hcopy - hi))));
			 */
			r = l - lmuls;
			g = l;
			b = l - 3 * lmuls + lmuls * hcopy;
	      break;
	      case 3 :
			/*
			   r = l * (1 - s);
			   g = l * (1 - (s * (hcopy - hi)));
			   b = l;
			 */
			r = l - lmuls;
			g = l + 3 * lmuls - lmuls * hcopy;
			b = l;
	      break;
	      case 4 :
			/*
			   r = l * (1 - (s * (1 - (hcopy - hi))));
			   g = l * (1 - s);
			   b = l;
			 */
			r = l - 5 * lmuls + lmuls * hcopy;
			g = l - lmuls;
			b = l;
	      break;
	      case 5 :
			/*
			   r = l;
			   g = l * (1 - s);
			   b = l * (1 - (s * (hcopy - hi)));
			 */
			r = l;
			g = l - lmuls;
			b = l + 5 * lmuls - lmuls * hcopy;
	      break;
	   }
	}
}

/*
 * Objet	:	Algorithme de conversion RGB vers HLS
 *
 * Rappels	:	RGB et HLS sont 2 modeles de representation
 *			des couleurs
 *
 * Parametres	:	r, g, b connus dans [0,1]
 *			h, l, s avec h dans [0,360] et l et s dans [0,1]
 *
 * Reference	:	La synthese d'images, Collection Hermes
 *
 * Creation	:	Cal	04 fevrier 1992
 *
 * Modification	:	?
 */

void call_rgbhls (float r, float g, float b, float& h, float& l, float& s)
/* parametres d'entree : r, g, b connus dans [0,1] */
/* parametres de sortie : h, l, s avec h dans [0,360] et l et s dans [0,1] */
{

	float max, min;
	float plus, diff;
	float delta;

	/* calcul du max entre r, g et b */
		     { plus = 0.0; diff = g - b; max = r; }
	if (g > max) { plus = 2.0; diff = b - r; max = g; }
	if (b > max) { plus = 4.0; diff = r - g; max = b; }

	/* calcul du min entre r, g et b */
	min = r;
	if (g < min) min = g;
	if (b < min) min = b;

	delta = max - min;

	/* calcul de la luminance */
	l = max;

	/* calcul de la saturation */
	s = 0.0;
	if (max != 0.0) s = delta / max;

	/* calcul de la teinte */
	if (s == 0.0)
	   /* cas des gris */
	   h = RGBHLS_H_UNDEFINED;
	else {
	   h = (float ) 60.0 * ( plus + diff / delta );
	   if (h < 0.0) h += 360.0;
	}
}
