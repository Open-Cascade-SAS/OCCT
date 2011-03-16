// File:  Graphic3d_NListOfHAsciiString.hxx
// Created: 20.01.2009
// Author:      Alexander A. BORODIN

#ifndef _Graphic3d_NListOfHAsciiString_HeaderFile
#define _Graphic3d_NListOfHAsciiString_HeaderFile

#include <NCollection_DefineList.hxx>
#include <TCollection_HAsciiString.hxx>

DEFINE_LIST   (Graphic3d_NListOfHAsciiString,
               NCollection_List,
               Handle(TCollection_HAsciiString))

#endif
