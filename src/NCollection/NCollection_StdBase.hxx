// File:	NCollection_StdBase.hxx
// Created:	Sun May 04 17:30:38 2003
// Author:	Alexander Grigoriev

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
