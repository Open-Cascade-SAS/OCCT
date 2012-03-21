// Created on: 1993-10-08
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <Units_UnitsLexicon.ixx>
#include <Units.hxx>
#include <Units_UnitsDictionary.hxx>
#include <Units_Token.hxx>
#include <TCollection_AsciiString.hxx>

#include <sys/types.h>
#include <sys/stat.h>

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
	  if(thetime >= buf.st_ctime) return Standard_True;
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
