// Created on: 1993-09-14
// Created by: GG	
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//GG_040298	eliminer les variables statiques globales.

#define PRO5676 //GG_020197
//              Donner la possibillite d'exprimer la taille
//		par rapport a la hauteur totale du caractere ou par
//		rapport a la hauteur "ascent" situee au dessus de la 
//		ligne de base.
//		(CapsHeight flag)

//GG_160498 etude G1343 gerer les polices transformables pour 
//	    l'utilisation de MFT_FontManager.

// =====================================================================
// DCB_120598 study S3553. Platform specific code has been deleted
// ("#ifdef WNT") to be able to use on WNT the same font descriptor
// string like on UNIX.
// =====================================================================



//-Version	

//-Design	Declaration des variables specifiques aux Fontures de textes

//-Warning	Un style est definie, soit par son type predefini TOF_...
//		soit par sa description (Font string de type ADOBE) 

//-References	

//-Language	C++ 2.0

//-Declarations
#define MAXFIELDS 14
#define PITCHSIZE (0.00028 METER);

// for the class
#include <Aspect_FontStyle.ixx>
#include <Aspect_Units.hxx>
#include <stdio.h>

//-Aliases

//-Global data definitions

//	MyFontType	:	TypeOfFont from Aspect;
//	MyStyle		:	AsciiString from  TCollection
//	MyFontName	:	AsciiCString from TCollection 
//	MyFontSize	:	Real from Standard
//	MyFontSlant	:	Real from Standard
//	MyCapsHeight	:	Boolean from Standard

//-Constructors

//-Destructors

//-Methods, in order

Aspect_FontStyle::Aspect_FontStyle () {

	SetPredefinedStyle(Aspect_TOF_DEFAULT,0.003 METER,0.,Standard_False) ;
}

Aspect_FontStyle::Aspect_FontStyle (const Aspect_TypeOfFont Type, const Quantity_Length Size, const Quantity_PlaneAngle Slant, const Standard_Boolean CapsHeight) {

	SetPredefinedStyle(Type,Size,Slant,CapsHeight) ;
}

Aspect_FontStyle::Aspect_FontStyle (const Standard_CString style, const Quantity_Length Size, const Quantity_PlaneAngle Slant, const Standard_Boolean CapsHeight) {

	MyFontType = Aspect_TOF_USERDEFINED ;
	MyFontSize = Size;
	MyFontSlant = Slant;
	MyCapsHeight = CapsHeight;
	MyStyle = style;
	MyFontName = Normalize(style,MyFontSize) ;
}

Aspect_FontStyle::Aspect_FontStyle (const Standard_CString style) {

	MyFontType = Aspect_TOF_USERDEFINED ;
	MyFontSize = 0. ;
	MyFontSlant = 0. ;
	MyCapsHeight = Standard_False;
	MyStyle = style;
	MyFontName = Normalize(style,MyFontSize) ;
}

Aspect_FontStyle& Aspect_FontStyle::Assign (const Aspect_FontStyle& Other) {

	MyFontType = Other.Style() ;
	MyFontSize = Other.Size() ;
	MyFontSlant = Other.Slant() ;
	MyStyle = Other.Value();
	MyFontName = Other.FullName() ;
	MyCapsHeight = Other.CapsHeight() ;

	return (*this);
}

void Aspect_FontStyle::SetValues ( const Aspect_TypeOfFont Type, const Quantity_Length Size, const Quantity_PlaneAngle Slant, const Standard_Boolean CapsHeight) {

	SetPredefinedStyle (Type,Size,Slant,CapsHeight);
}

void Aspect_FontStyle::SetValues ( const Standard_CString style, const Quantity_Length Size, const Quantity_PlaneAngle Slant, const Standard_Boolean CapsHeight) {
	MyFontType = Aspect_TOF_USERDEFINED ;
	MyFontSize = Size;
	MyFontSlant = Slant;
	MyCapsHeight = CapsHeight;
	MyStyle = style;
	MyFontName = Normalize(style,MyFontSize) ;
}

void Aspect_FontStyle::SetValues ( const Standard_CString style) {
	MyFontType = Aspect_TOF_USERDEFINED ;
	MyFontSize = 0. ;
	MyFontSlant = 0. ;
	MyCapsHeight = Standard_False;
	MyStyle = style;
	MyFontName = Normalize(style,MyFontSize) ;
}

// ============================================================================
//        ---Purpose: Sets the family of the font.
// ============================================================================

void Aspect_FontStyle::SetFamily(const Standard_CString aName) {
    MyFontName = SetField(MyFontName,aName,2);
}

// ============================================================================
//        ---Purpose: Sets the weight of the font.
// ============================================================================

void Aspect_FontStyle::SetWeight(const Standard_CString aName) {
    MyFontName = SetField(MyFontName,aName,3);
}

// ============================================================================
//        ---Purpose: Sets the Registry of the font.
// ============================================================================

void Aspect_FontStyle::SetRegistry(const Standard_CString aName) {
    MyFontName = SetField(MyFontName,aName,13);
}

// ============================================================================
//        ---Purpose: Sets the Encoding of the font.
// ============================================================================

void Aspect_FontStyle::SetEncoding(const Standard_CString aName) {
    MyFontName = SetField(MyFontName,aName,14);
}

Aspect_TypeOfFont Aspect_FontStyle::Style () const {

	return (MyFontType);
}

Standard_Integer Aspect_FontStyle::Length () const {

	return MyStyle.Length();
}

Standard_CString Aspect_FontStyle::Value () const
{
  return MyStyle.ToCString();
}

Quantity_Length Aspect_FontStyle::Size () const {
	return (MyFontSize);
}

Quantity_PlaneAngle Aspect_FontStyle::Slant () const {

	return (MyFontSlant);
}

Standard_Boolean Aspect_FontStyle::CapsHeight () const {

	return (MyCapsHeight);
}

// ============================================================================
//        ---Purpose: Returns the alias font name.
//        --          this is a shorter font name which identify the
//        --          main characteristics of the fonts.
//        ---Example: "helvetica-bold"
// ============================================================================

Standard_CString Aspect_FontStyle::AliasName() const
{
static TCollection_AsciiString aliasname;
    aliasname.Clear();
    TCollection_AsciiString family(Family());
    if( family.Length() > 0 && family != "*" ) {
      TCollection_AsciiString FAMILY = family; FAMILY.UpperCase();
      family.SetValue(1,FAMILY.Value(1));
      aliasname = family; 
    }
    TCollection_AsciiString weight(Weight()); 
    if( weight.Length() > 0 && weight != "*" ) {
      TCollection_AsciiString WEIGHT = weight; WEIGHT.UpperCase();
      if( WEIGHT == "NORMAL" ) {
	weight.Clear();
      } else {
        weight.SetValue(1,WEIGHT.Value(1));
      }
    } else weight.Clear();
    TCollection_AsciiString sslant(SSlant());
    if( sslant.Length() > 0 && sslant != "*" ) {
      if( sslant == "r" ) {
	sslant.Clear();
      } else if( sslant == "i" ) {
        weight += "Italic";
	sslant.Clear();
      } else if( sslant == "o" ) {
        weight += "Oblique";
	sslant.Clear();
      }
    } else sslant.Clear();
    if( weight.Length() > 0 ) {
      aliasname += '-'; aliasname += weight;
    }
    if( sslant.Length() > 0 ) {
      aliasname += '-'; aliasname += sslant;
    }
    TCollection_AsciiString swidth(SWidth());
    if( swidth.Length() > 0 && swidth != "*" ) {
      TCollection_AsciiString SWIDTH = swidth; SWIDTH.UpperCase();
      if( SWIDTH == "NORMAL" ) {
      } else {
	if( !weight.Length() ) aliasname += '-';
        aliasname += '-'; aliasname += swidth;
      }
    }
    return aliasname.ToCString();
}
// ============================================================================
//        ---Purpose: Returns the full normalized font name
// ============================================================================

Standard_CString Aspect_FontStyle::FullName () const {

	return (MyFontName.ToCString());
}

// ============================================================================
//        ---Purpose: Returns the foundry of the font.
//        ---Example: "adobe"
// ============================================================================

Standard_CString Aspect_FontStyle::Foundry() const
{
    return Field(MyFontName,1);
}

// ============================================================================
//        ---Purpose: Returns the family of the font.
//        ---Example: "helvetica"
// ============================================================================

Standard_CString Aspect_FontStyle::Family() const
{
    return Field(MyFontName,2);
}

// ============================================================================
//        ---Purpose: Returns the weight of the font.
//        ---Example: "bold"
// ============================================================================

Standard_CString Aspect_FontStyle::Weight() const
{
    return Field(MyFontName,3);
}

// ============================================================================
//        ---Purpose: Returns the char set registry of the font.
//        ---Example: "iso8859"
// ============================================================================

Standard_CString Aspect_FontStyle::Registry() const
{
    return Field(MyFontName,13);
}

// ============================================================================
//        ---Purpose: Returns the char set encoding of the font.
//        ---Example: "1"
// ============================================================================

Standard_CString Aspect_FontStyle::Encoding() const
{
    return Field(MyFontName,14);
}

// ============================================================================
//        ---Purpose: Returns the slant of the font.
//        ---Example: "i"
// ============================================================================

Standard_CString Aspect_FontStyle::SSlant() const
{
    return Field(MyFontName,4);
}

// ============================================================================
//        ---Purpose: Returns the width name of the font.
//        ---Example: "normal"
// ============================================================================

Standard_CString Aspect_FontStyle::SWidth() const
{
    return Field(MyFontName,5);
}

// ============================================================================
//        ---Purpose: Returns the style name of the font.
//        ---Example: "serif"
// ============================================================================

Standard_CString Aspect_FontStyle::SStyle() const
{
    return Field(MyFontName,6);
}

// ============================================================================
//        ---Purpose: Returns the pixel size of the font.
//        ---Example: "14"
// ============================================================================

Standard_CString Aspect_FontStyle::SPixelSize() const
{
    return Field(MyFontName,7);
}

// ============================================================================
//        ---Purpose: Returns the point size of the font.
//        ---Example: "140"
// ============================================================================

Standard_CString Aspect_FontStyle::SPointSize() const
{
    return Field(MyFontName,8);
}
// ============================================================================
//        ---Purpose: Returns the resolution X of the font.
//        ---Example: "75"
// ============================================================================

Standard_CString Aspect_FontStyle::SResolutionX() const
{
    return Field(MyFontName,9);
}

// ============================================================================
//        ---Purpose: Returns the resolution Y of the font.
//        ---Example: "75"
// ============================================================================

Standard_CString Aspect_FontStyle::SResolutionY() const
{
    return Field(MyFontName,10);
}

// ============================================================================
//        ---Purpose: Returns the spacing of the font.
//        ---Example: "p"
// ============================================================================

Standard_CString Aspect_FontStyle::SSpacing() const
{
    return Field(MyFontName,11);
}

// ============================================================================
//        ---Purpose: Returns the average width of the font.
//        ---Example: "90"
// ============================================================================

Standard_CString Aspect_FontStyle::SAverageWidth() const
{
    return Field(MyFontName,12);
}

// ============================================================================
//        ---Purpose: Dumps the font attributes.
// ============================================================================

void Aspect_FontStyle::Dump() const
{
    cout << ".Aspect_FontStyle::Dump() :" << endl;
    cout << "-----------------" << endl << endl;
    cout << "  style name is : '" << this->Value() << "'" << endl;
    cout << "  Normalized font name is : '" << this->FullName() << "'" << endl;
    cout << "  Alias font name is : '" << this->AliasName() << "'" << endl;
    cout << "    Foundry is : '" << this->Foundry() << "'" << endl;
    cout << "    Family is : '" << this->Family() << "'" << endl;
    cout << "    Weight is : '" << this->Weight() << "'" << endl;
    cout << "    Slant is : '" << this->SSlant() << "'" << endl;
    cout << "    Width is : '" << this->SWidth() << "'" << endl;
    cout << "    Style is : '" << this->SStyle() << "'" << endl;
    cout << "    PixelSize is : '" << this->SPixelSize() << "'" << endl;
    cout << "    PointSize is : '" << this->SPointSize() << "'" << endl;
    cout << "    ResolutionX is : '" << this->SResolutionX() << "'" << endl;
    cout << "    ResolutionY is : '" << this->SResolutionY() << "'" << endl;
    cout << "    Spacing is : '" << this->SSpacing() << "'" << endl;
    cout << "    AverageWidth is : '" << this->SAverageWidth() << "'" << endl;
    cout << "    Registry is : '" << this->Registry() << "'" << endl;
    cout << "    Encoding is : '" << this->Encoding() << "'" << endl << endl;
}

void Aspect_FontStyle::SetPredefinedStyle( const Aspect_TypeOfFont Type, const Quantity_Length Size, const Quantity_PlaneAngle Slant, const Standard_Boolean CapsHeight) {

	if( Size <= 0. )
		Aspect_FontStyleDefinitionError::Raise ("Bad font Size");

	MyFontType = Type ;
	MyFontSize = Size;
	MyFontSlant = Slant ;
	MyCapsHeight = CapsHeight;
	Standard_CString pstyle = "" ;
	switch ( Type ) {
	    case Aspect_TOF_USERDEFINED :
		Aspect_FontStyleDefinitionError::Raise ("Bad Font Type Style");
	    case Aspect_TOF_DEFAULT :
		pstyle = "Defaultfont" ;
		break ;
	    case Aspect_TOF_COURIER :
		pstyle = "Courier" ;
		break ;
	    case Aspect_TOF_HELVETICA :
		pstyle = "Helvetica" ;
		break ;
	    case Aspect_TOF_TIMES :
		pstyle = "Times" ;
		break ;
	}
	MyStyle = pstyle;
	MyFontName = Normalize(pstyle,MyFontSize);
}

Standard_Boolean Aspect_FontStyle::IsEqual(const Aspect_FontStyle& Other) const
{
  return (
	  (MyFontType == Other.MyFontType) &&
	  (MyFontName.Length() == Other.MyFontName.Length()) &&
	  (MyFontName == Other.MyFontName) &&
	  (MyFontSize == Other.MyFontSize) &&
	  (MyFontSlant == Other.MyFontSlant) &&
	  (MyCapsHeight == Other.MyCapsHeight)
	 );

}

Standard_Boolean Aspect_FontStyle::IsNotEqual(const Aspect_FontStyle& Other) const
{
  return !IsEqual(Other);
}

// ============================================================================
//        ---Purpose: Returns a normalized XLFD descriptor from the font name
//        --         <aFontName>
// ============================================================================

Standard_CString Aspect_FontStyle::Normalize(const Standard_CString aFontName,Standard_Real& aSize)
{
    Aspect_FontStyleDefinitionError_Raise_if( 
				!aFontName ,"NULL font descriptor");

static TCollection_AsciiString afontstring;
static TCollection_AsciiString astar("*");
static TCollection_AsciiString azero("0");
TCollection_AsciiString afield,afontname(aFontName);
Standard_CString pfontname = NULL;
Standard_Boolean xlfd = Standard_False;
Standard_Integer i,len = afontname.Length();

    afontstring = "";

    if( len <= 0 ) return pfontname;

    if( afontname.Value(1) != '-' ) afontname.Prepend("-adobe-");
    for( i=1 ; i<=MAXFIELDS ; i++ ) {
      Standard_CString pfield = Field(afontname,i);
      switch (i) {
        case 1:
	  if( pfield ) {
            afield = pfield; afield.LowerCase();
	    {afontstring += '-'; afontstring += afield;}
	  } else {
            afontstring += "-adobe";
	  }
	  break;
        case 7:
	  if( pfield ) {
            afield = pfield; afield.LowerCase();
	    if( afield == astar ) {
	      if( aSize > 0. ) afontstring += "-*";
	      else {afontstring += "-0"; xlfd = Standard_True;}
	    } else if( afield == azero ) { afontstring += "-0"; xlfd = Standard_True;}
	    else {afontstring += '-'; afontstring += afield;}
	  } else {
            if( aSize > 0. ) afontstring += "-*";
	    else { afontstring += "-0"; xlfd = Standard_True;}
	  }
	  break;
        case 8:
        case 9:
        case 10:
	  if( xlfd ) {
	    afontstring += "-0";
	  } else if( pfield ) {
            afield = pfield; afield.LowerCase();
	    afontstring += '-'; afontstring += afield;
	  } else {
            afontstring += "-*";
	  }
	  break;
        case 13:
	  if( pfield ) {
            afield = pfield; afield.LowerCase();
	    afontstring += '-'; afontstring += afield;
	  } else {
            afontstring += "-*";
	  }
	  break;
	default:
	  if( pfield ) {
            afield = pfield;
	    afontstring += '-'; afontstring += afield;
          } else {
            afontstring += "-*";
          }
      }
    }
    pfontname = afontstring.ToCString();
    if( aSize == 0. ) {
      Standard_CString psize = Field(pfontname,7);
      Standard_Integer isize;
      if( sscanf(psize,"%d",&isize) == 1 ) {
        aSize = isize*PITCHSIZE;
      } else {
        aSize = 2. MILLIMETER;
      }
    }

    return pfontname;
}

// ============================================================================
//        ---Purpose: Returns the field at position <aRank>
//        --          from the font name <aFontName>.
// ============================================================================

Standard_CString Aspect_FontStyle::Field(const TCollection_AsciiString& aFontName,const Standard_Integer aRank)
{
static TCollection_AsciiString afieldstring;
Standard_Integer length = aFontName.Length();
Standard_CString pfield = NULL;

    if( length > 0 && aRank >= 1 && aRank <= MAXFIELDS ) {
      Standard_Integer start = 1,end = 0;
      start = aFontName.Location(aRank,'-',1,length);
      end = aFontName.Location(aRank+1,'-',1,length);
      afieldstring = "";
      if( start > 0 && end > 0 ) {
        start++ ; 
	end--;
        if( end >= start ) afieldstring = aFontName.SubString(start,end);
        else afieldstring = "";
      } else if( start > 0 ) {
        start++ ; 
	end = length;
        if( end >= start ) afieldstring = aFontName.SubString(start,end);
        else afieldstring = "";
      } else {
        return NULL;
      }
    }
    pfield = afieldstring.ToCString();

    return pfield;
}

// ============================================================================
//        ---Purpose: Sets the field at position <aRank>
//	  --	      of the font <aFontName>
//        --          from the field <aField>.
// ============================================================================

Standard_CString Aspect_FontStyle::SetField(const TCollection_AsciiString& aFontName,const Standard_CString aField,const Standard_Integer aRank)
{
static TCollection_AsciiString afontstring;
Standard_Integer length = aFontName.Length();
Standard_CString pfontname = NULL;

    if( length > 0 && aRank >= 1 && aRank <= MAXFIELDS ) {
      Standard_Integer start = 1,end = 0;
      start = aFontName.Location(aRank,'-',1,length);
      end = aFontName.Location(aRank+1,'-',1,length);
      TCollection_AsciiString lowername(""),uppername(""); 
      if( start > 0 && end > 0 ) {
        if( end > start ) {
	  if( start > 1 ) lowername = aFontName.SubString(1,start);
	  uppername = aFontName.SubString(end,length);
	}
      } else if( start > 1 ) {
	lowername = aFontName.SubString(1,start);
      } else {
        return NULL;
      }
      afontstring = lowername;
      afontstring += aField; afontstring += uppername;
      pfontname = afontstring.ToCString();
    }

    return pfontname;
}
