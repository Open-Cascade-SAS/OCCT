// Created on: 1992-06-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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



#include <Units_Lexicon.ixx>
#include <Units_Token.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <OSD.hxx>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WNT
# include <stdio.h>
#else
#include <Standard_Stream.hxx>
#endif  // WNT

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
  ifstream file(afilename, ios::in);
  if(!file) {
    cout<<"unable to open "<<afilename<<" for input"<<endl;
    return;
  }

  thefilename = new TCollection_HAsciiString(afilename);
  thesequenceoftokens = new Units_TokensSequence();

  struct stat buf;
  if(!stat(afilename,&buf)) thetime = buf.st_ctime;

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
    for (int i=0; i < 31; i++) chain[i] = '\0';
    for (int i=0; i < 11; i++) oper[i]  = '\0';
    for (int i=0; i < 31; i++) coeff[i] = '\0';

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
  struct stat buf;
  TCollection_AsciiString string = FileName();

  if(!stat(string.ToCString(),&buf)) {
    if(thetime >= buf.st_ctime)
      return Standard_True;
  }

  return Standard_False;
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
