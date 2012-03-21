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
//==== Titre: Standard_ExtCharacter.hxx
//==== Role : The headr file of primitve type "ExtCharacter" from package 
//====        "Standard"
//==== 
//==== Implementation:  This is a primitive type implemented with typedef
//====                  Excepting "Unicod encoding"
//============================================================================

#ifndef _Standard_ExtCharacter_HeaderFile
#define _Standard_ExtCharacter_HeaderFile

#ifndef _Standard_ctype_HeaderFile
#include <Standard_ctype.hxx>
#endif

#include <string.h>

#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

class Handle_Standard_Type;

__Standard_API const Handle_Standard_Type& Standard_ExtCharacter_Type_();
//class Standard_OStream;
//void ShallowDump (const Standard_ExtCharacter, Standard_OStream& );
// =====================================
// Method implemented in Standard_ExtCharacter.cxx
// =====================================
__Standard_API Standard_Integer HashCode(const Standard_ExtCharacter, const Standard_Integer);

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
// ToExtCharacter : Returns an ExtCharacter from a Character
// ------------------------------------------------------------------
inline Standard_ExtCharacter ToExtCharacter(const Standard_Character achar)
{
//  extension a zero (partie haute-->octet de gauche) de achar
    return (Standard_ExtCharacter)( (unsigned char)achar & 0x00ff );
}

// ------------------------------------------------------------------
// ToCharacter : Returns an Character from an ExtCharacter
// ------------------------------------------------------------------
inline Standard_Character ToCharacter(const Standard_ExtCharacter achar)
{
//  recuperer partie basse(octet de droite) de achar
    return (Standard_Character)(unsigned char)(achar & 0x00ff);
}

// ------------------------------------------------------------------
// IsAnAscii : Returns True if an ExtCharacter is in the "Ascii Range"
// ------------------------------------------------------------------
inline Standard_Boolean IsAnAscii(const Standard_ExtCharacter achar)
{
    return ! ( achar & 0xff00 );
}

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two characters have the same value
// ------------------------------------------------------------------
inline Standard_Boolean IsEqual(const Standard_ExtCharacter One,
				const Standard_ExtCharacter Two)
{ return One == Two; }

// ------------------------------------------------------------------
// IsSimilar : Returns Standard_True if two characters have the same value
// ------------------------------------------------------------------
inline Standard_Boolean IsSimilar(const Standard_ExtCharacter One, 
				  const Standard_ExtCharacter Two)
{ return One == Two; }


// ------------------------------------------------------------------
// ShallowCopy : Make a copy of one Character
// ------------------------------------------------------------------
inline Standard_ExtCharacter ShallowCopy (const Standard_ExtCharacter me) 
{ return me; }

#endif














