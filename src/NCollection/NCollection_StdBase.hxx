// Created on: 2003-05-04
// Created by: Alexander Grigoriev
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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


// DEFINITION OF BASE COLLECTIONS FOR Open Cascade STANDARD TYPES

#ifndef _NCollection_StdBase_HeaderFile
#define _NCollection_StdBase_HeaderFile

#include <Standard_PrimitiveTypes.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_DefineBaseCollection.hxx>

DEFINE_BASECOLLECTION(NCollection_BaseCollInteger       , Standard_Integer)
DEFINE_BASECOLLECTION(NCollection_BaseCollReal          , Standard_Real)
DEFINE_BASECOLLECTION(NCollection_BaseCollCharacter     , Standard_Character)
DEFINE_BASECOLLECTION(NCollection_BaseCollBoolean       , Standard_Boolean)
DEFINE_BASECOLLECTION(NCollection_BaseCollAsciiString   ,
                      TCollection_AsciiString)
DEFINE_BASECOLLECTION(NCollection_BaseCollExtendedString,
                      TCollection_ExtendedString)
DEFINE_BASECOLLECTION(NCollection_BaseCollTransient     ,
                      Handle_Standard_Transient)

#endif
