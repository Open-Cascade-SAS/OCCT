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


#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Units.hxx>
#include <Units_Token.hxx>
#include <Units_UnitsDictionary.hxx>
#include <Units_UnitsLexicon.hxx>

#include <sys/stat.h>
#include <sys/types.h>
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

void Units_UnitsLexicon::Creates(const Standard_CString afilename1,
				 const Standard_CString afilename2,
				 const Standard_Boolean amode)
{
  Handle(Units_UnitsDictionary) unitsdictionary;
  struct stat buf;

  thefilename = new TCollection_HAsciiString(afilename2);

  if(!stat(afilename2,&buf)) thetime = buf.st_ctime;

  Units_Lexicon::Creates(afilename1);

  if(amode)unitsdictionary = Units::DictionaryOfUnits(amode);

}

//=======================================================================
//function : UpToDate
//purpose  : 
//=======================================================================

Standard_Boolean Units_UnitsLexicon::UpToDate() const
{
  struct stat buf;
  TCollection_AsciiString string = FileName2();

  if(Units_Lexicon::UpToDate())
    {
      if(!stat(string.ToCString(),&buf))
	{
	  if(thetime >= (Standard_Time)buf.st_ctime) return Standard_True;
	}
    }

  return Standard_False;
}

//=======================================================================
//function : FileName2
//purpose  : 
//=======================================================================

TCollection_AsciiString Units_UnitsLexicon::FileName2() const
{
  return thefilename->String();
}
