// Created on: 1992-06-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Units_Lexicon.hxx>

#include <OSD.hxx>
#include <OSD_OpenFile.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Units_Token.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_Lexicon,MMgt_TShared)

#ifdef _MSC_VER
# include <stdio.h>
#else
#include <Standard_Stream.hxx>
#endif  // _MSC_VER

//=======================================================================
//function : Units_Lexicon
//purpose  : 
//=======================================================================

Units_Lexicon::Units_Lexicon()
{
}


//=======================================================================
//function : Creates
//purpose  : 
//=======================================================================

static inline bool strrightadjust (char *str)
{
  for (size_t len = strlen(str); len > 0 && IsSpace (str[len-1]); len--)
    str[len-1] = '\0';
  return str[0] != '\0';
}

void Units_Lexicon::Creates(const Standard_CString afilename)
{
  std::ifstream file;
  OSD_OpenStream (file, afilename, std::ios::in);
  if(!file) {
#ifdef OCCT_DEBUG
    cout<<"unable to open "<<afilename<<" for input"<<endl;
#endif
    return;
  }

  thefilename = new TCollection_HAsciiString(afilename);
  thesequenceoftokens = new Units_TokensSequence();
  thetime = OSD_FileStatCTime (afilename);

  // read file line-by-line; each line has fixed format:
  // first 30 symbols for prefix or symbol (e.g. "k" for kilo)
  // then 10 symbols for operation
  // then 30 symbols for numeric parameter (e.g. multiplier)
  // line can be shorter if last fields are empty
  Handle(Units_Token) token;
  for (int nline = 0; ; nline++) {
    char line[256];
    memset (line, 0, sizeof(line));
    if (! file.getline (line, 255))
      break;

    // trim trailing white space
    if (! strrightadjust (line)) // empty line
      continue;

    // split line to parts
    char chain[31], oper[11], coeff[31];
    memset(chain,0x00,sizeof(chain));
    memset(oper,0x00,sizeof(oper));
    memset(coeff,0x00,sizeof(coeff));

    sscanf (line, "%30c%10c%30c", chain, oper, coeff);

    // remove trailing spaces and check values
    if (! strrightadjust (chain))
      continue;
    strrightadjust (oper);
    double value = 0;
    if (strrightadjust (coeff))
      OSD::CStringToReal (coeff, value);

    // add token
    if(thesequenceoftokens->IsEmpty()) {
      token = new Units_Token(chain,oper,value);
      thesequenceoftokens->Prepend(token);
    }
    else {
      AddToken(chain,oper,value);
    }
  }
  file.close();
}


//=======================================================================
//function : UpToDate
//purpose  : 
//=======================================================================

Standard_Boolean Units_Lexicon::UpToDate() const
{
  TCollection_AsciiString aPath = FileName();
  Standard_Time aTime = OSD_FileStatCTime (aPath.ToCString());
  return aTime != 0
      && aTime <= thetime;
}


//=======================================================================
//function : FileName
//purpose  : 
//=======================================================================

TCollection_AsciiString Units_Lexicon::FileName() const
{
  return thefilename->String();
}


//=======================================================================
//function : AddToken
//purpose  : 
//=======================================================================

void Units_Lexicon::AddToken(const Standard_CString aword,
			     const Standard_CString amean,
			     const Standard_Real avalue)
{
  Handle(Units_Token) token;
  Handle(Units_Token) referencetoken;
  Standard_Boolean found = Standard_False;
  Standard_Integer index;

  for(index=1;index<=thesequenceoftokens->Length();index++) {
    referencetoken = thesequenceoftokens->Value(index);
    if( referencetoken->Word() == aword ) {
      referencetoken->Update(amean);
      found = Standard_True;
      break;
    }
    else if( !( referencetoken->Word()>aword ) ) {
      token = new Units_Token(aword,amean,avalue);
      thesequenceoftokens->InsertBefore(index,token);
      found = Standard_True;
      break;
    }
  }
  if(!found) {
    token = new Units_Token(aword,amean,avalue);
    thesequenceoftokens->Append(token);
  }
}
