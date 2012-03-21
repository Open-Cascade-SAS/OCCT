// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//============================================================================
//==== Titre: Standard_Character.hxx
//==== Role : The header file of primitve type "Character" from package 
//====        "Standard"
//==== 
//==== Implementation:  This is a primitive type implemented as typedef
//====        typedef char Standard_Character
//============================================================================

#ifndef _Standard_Character_HeaderFile
#define _Standard_Character_HeaderFile

#ifndef _Standard_ctype_HeaderFile
#include <Standard_ctype.hxx>
#endif

#include <string.h>

#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

class Handle_Standard_Type;

__Standard_API const Handle_Standard_Type& Standard_Character_Type_();

//class Standard_OStream;
//void ShallowDump (const Standard_Character, Standard_OStream& );
// =====================================
// Method implemented in Standard_Character.cxx
// =====================================
__Standard_API Standard_Integer   HashCode(const Standard_Character, const Standard_Integer);

// ===============================================
// Methods from Standard_Entity class which are redefined:  
//    - Hascode
//    - IsEqual
//    - IsSimilar
//    - Shallowcopy
//    - ShallowDump
// ===============================================

// ===============
// inline methods 
// ===============

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two characters have the same value
// ------------------------------------------------------------------
inline Standard_Boolean IsEqual(const Standard_Character One,
				const Standard_Character Two)
{ return One == Two; }

// ------------------------------------------------------------------
// IsSimilar : Returns Standard_True if two characters have the same value
// ------------------------------------------------------------------
inline Standard_Boolean IsSimilar(const Standard_Character One, 
				  const Standard_Character Two)
{ return One == Two; }

// ===============================================
// Character classification functions
//
// NOTE: Character classification routines in C standard library 
// (isdigit(), isalpha() etc.) have integer argument instead of char. 
// Therefore if character from extended Ascii part of char table
// (i.e. above 128) is passed into such functions it is converted 
// to int giving negative value (if characters are signed that
// is default for most compilers). 
// It can be dangerous since implementation of these C functions
// may use argument as index in the array without any checks 
// (for instance, in Microsoft VC++ -- see MSDN)
// To avoid this, we cast char to unsigned char when passing to these functions.
// ===============================================
  
// ==================================================================
// IsAlphabetic : Returns Standard_True if a character is alphabetic
// ==================================================================
inline Standard_Boolean IsAlphabetic(const Standard_Character me) 
{ return isalpha((unsigned char)me); }

// ==================================================================
// IsDigit : Returns Standard_True if a character is a digit
// ==================================================================
inline Standard_Boolean IsDigit(const Standard_Character me) 
{ return isdigit((unsigned char)me); }

// ==================================================================
// IsXDigit : Returns Standard_True if a character is a digit
// ==================================================================
inline Standard_Boolean IsXDigit(const Standard_Character me) 
{ return isxdigit((unsigned char)me); }

// ==================================================================
// IsAlphanumeric : Returns Standard_True if a character is alphanumeric
// ==================================================================
inline Standard_Boolean IsAlphanumeric(const Standard_Character me) 
{ return (IsAlphabetic(me) || IsDigit(me)) ; }

// ==================================================================
// IsControl : Returns Standard_True if a character  is a control character
// ==================================================================
inline Standard_Boolean IsControl(const Standard_Character me) 
{ return iscntrl((unsigned char)me); }


// ==================================================================
// IsGraphic : Returns Standard_True if a character is graphic
// ==================================================================
inline Standard_Boolean IsGraphic(const Standard_Character me) 
{ return isgraph((unsigned char)me); }

// ==================================================================
// IsLowerCase : Returns Standard_True if a character is lowercase
// ==================================================================
inline Standard_Boolean IsLowerCase(const Standard_Character me) 
{ return islower((unsigned char)me); }

// ==================================================================
// IsPrintable : Returns Standard_True if a character is printable
// ==================================================================
inline Standard_Boolean IsPrintable(const Standard_Character me) 
{ return isprint((unsigned char)me); }

// ==================================================================
// IsPunctuation : Returns Standard_True if a character is a graphic and 
//                 not a alphanumeric character
// ==================================================================
inline Standard_Boolean IsPunctuation(const Standard_Character me) 
{ return ( IsGraphic(me) && !IsAlphanumeric(me)); }

// ==================================================================
// IsSpace : Returns Standard_True if a character is a space
// ==================================================================
inline Standard_Boolean IsSpace(const Standard_Character me) 
{ return isspace((unsigned char)me); }

// ==================================================================
// IsUppercase : Returns Standard_True if a character is uppercase
// ==================================================================
inline Standard_Boolean IsUpperCase(const Standard_Character me) 
{ return isupper((unsigned char)me); }

// ==================================================================
// LowerCase : Returns a lowercase character
// ==================================================================
inline Standard_Character LowerCase(const Standard_Character me) 
{ return (Standard_Character)(unsigned char)tolower(me); }

// ==================================================================
// UpperCase : Returns a uppercase character
// ==================================================================
inline Standard_Character UpperCase(const Standard_Character me) 
{ return (Standard_Character)(unsigned char)toupper(me); }

// ------------------------------------------------------------------
// ShallowCopy : Make a copy of one Character
// ------------------------------------------------------------------
inline Standard_Character ShallowCopy (const Standard_Character me) 
{ return me; }

#endif














