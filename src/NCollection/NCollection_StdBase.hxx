// Created on: 2003-05-04
// Created by: Alexander Grigoriev
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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
