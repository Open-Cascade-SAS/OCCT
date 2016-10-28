// Created on: 1993-10-08
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Units_UnitsLexicon.hxx>

#include <OSD_OpenFile.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Units.hxx>
#include <Units_Token.hxx>
#include <Units_UnitsDictionary.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_UnitsLexicon,Units_Lexicon)

//=======================================================================
//function : Units_UnitsLexicon
//purpose  : 
//=======================================================================
Units_UnitsLexicon::Units_UnitsLexicon() : Units_Lexicon()
{}

//=======================================================================
//function : Creates
//purpose  : 
//=======================================================================

void Units_UnitsLexicon::Creates(const Standard_Boolean amode)
{
  Handle(Units_UnitsDictionary) unitsdictionary;

  Units_Lexicon::Creates();

  if(amode)unitsdictionary = Units::DictionaryOfUnits(amode);

}
