// File:	Units_UnitsLexicon.cxx
// Created:	Fri Oct  8 10:37:31 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@meteox>


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
