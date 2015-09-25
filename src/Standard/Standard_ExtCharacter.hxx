// Copyright (c) 1998-1999 Matra Datavision
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

#include <Standard_TypeDef.hxx>
#include <Standard_OStream.hxx>

#include <string.h>

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

#endif
