// Created on: 1996-09-26
// Created by: Arnaud BOUZY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Resource_ConvertUnicode.hxx>
#include <Resource_Manager.hxx>
#include <Resource_Unicode.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#define isjis(c) (((c)>=0x21 && (c)<=0x7e))
#define iseuc(c) (((c)>=0xa1 && (c)<=0xfe))
#define issjis1(c) (((c)>=0x81 && (c)<=0x9f) || ((c)>=0xe0 && (c)<=0xef))

#define issjis2(c) ((c)>=0x40 && (c)<=0xfc && (c)!=0x7f)

#define ishankana(c) ((c)>=0xa0 && (c)<=0xdf)

static inline Standard_Boolean isshift (unsigned char c) { return c >= 0x80; }
static inline Standard_Boolean isshift (unsigned int c) { return c >= 0x80 && c <= 0xff; }

void Resource_Unicode::ConvertSJISToUnicode(const Standard_CString fromstr,TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentstr = ((unsigned char*) fromstr);
  unsigned int pl,ph;
  // BIG INDIAN USED HERE
  while(*currentstr != '\0') {
    if (issjis1(*currentstr)) {
      
      ph = ((unsigned int) *currentstr);
      // Be Carefull with first and second !!

      currentstr++;

      pl =  ((unsigned int) *currentstr);
      currentstr++;
      
      Resource_sjis_to_unicode(&ph,&pl);
      Standard_ExtCharacter curcar = ((Standard_ExtCharacter) ((ph << 8) | pl));
      TCollection_ExtendedString curext(curcar);
      tostr.AssignCat(curext);
    }
    else {
      TCollection_ExtendedString curext(((char) *currentstr));
      currentstr++;
      tostr.AssignCat(curext);
    }
  }
}


void Resource_Unicode::ConvertEUCToUnicode(const Standard_CString fromstr,TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentstr = ((unsigned char*) fromstr);
  unsigned int pl,ph;
  // BIG INDIAN USED HERE
  while(*currentstr != '\0') {
    if (iseuc(*currentstr)) {
      
      ph = ((unsigned int) *currentstr);
      // Be Carefull with first and second !!

      currentstr++;

      pl =  ((unsigned int) *currentstr);
      currentstr++;
      
      Resource_euc_to_unicode(&ph,&pl);
      Standard_ExtCharacter curcar = ((Standard_ExtCharacter) ((ph << 8) | pl));
      TCollection_ExtendedString curext(curcar);
      tostr.AssignCat(curext);
    }
    else {
      TCollection_ExtendedString curext(((char) *currentstr));
      currentstr++;
      tostr.AssignCat(curext);
    }
  }
}

void Resource_Unicode::ConvertGBToUnicode(const Standard_CString fromstr,TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentstr = ((unsigned char*) fromstr);
  unsigned int pl,ph;
  // BIG INDIAN USED HERE
  while(*currentstr != '\0') {
    if (isshift(*currentstr)) {
      
      ph = ((unsigned int) *currentstr);
      // Be Carefull with first and second !!

      currentstr++;

      pl =  ((unsigned int) *currentstr);
      currentstr++;
      
      Resource_gb_to_unicode(&ph,&pl);
      Standard_ExtCharacter curcar = ((Standard_ExtCharacter) ((ph << 8) | pl));
      TCollection_ExtendedString curext(curcar);
      tostr.AssignCat(curext);
    }
    else {
      TCollection_ExtendedString curext(((char) *currentstr));
      currentstr++;
      tostr.AssignCat(curext);
    }
  }
}

void Resource_Unicode::ConvertANSIToUnicode(const Standard_CString fromstr,TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  TCollection_ExtendedString curext(fromstr);
  tostr.AssignCat(curext);
}

Standard_Boolean Resource_Unicode::ConvertUnicodeToSJIS(const TCollection_ExtendedString& fromstr,
							Standard_PCharacter& tostr,
							const Standard_Integer maxsize)
{
  Standard_Integer nbtrans = 0;
  Standard_Integer nbext = 1;
  Standard_Boolean finished = Standard_False;
  Standard_ExtCharacter curcar;
  unsigned int pl,ph;
  // BIG INDIAN USED HERE
  
  while (!finished) {
    if (nbext > fromstr.Length()) {
      finished = Standard_True;
      tostr[nbtrans] = '\0';
    }
    else {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = (((unsigned int) curcar) >> 8) & 0xFF;
      pl = ((unsigned int) curcar) & 0xFF;
      Resource_unicode_to_sjis(&ph,&pl);
      if (issjis1(ph)) {
	if (nbtrans < (maxsize-3)) {
	  tostr[nbtrans] = ((char) ph);
	  nbtrans++;
	  tostr[nbtrans] =  ((char) pl);
	  nbtrans++;
	}
	else {
	  tostr[nbtrans] = '\0';
	  nbtrans = maxsize-1;
	  return Standard_False;
	}
      }
      else {
	tostr[nbtrans] =  ((char) pl);
	nbtrans++;
      }
      if (nbtrans >= (maxsize - 1)) {
	tostr[maxsize-1] = '\0';
	finished = Standard_True;
	return Standard_False;
      }
    }
  }
  return Standard_True;
}
	  
Standard_Boolean Resource_Unicode::ConvertUnicodeToEUC(const TCollection_ExtendedString& fromstr,
						       Standard_PCharacter& tostr,
						       const Standard_Integer maxsize)
{
  Standard_Integer nbtrans = 0;
  Standard_Integer nbext = 1;
  Standard_Boolean finished = Standard_False;
  Standard_ExtCharacter curcar;
  unsigned int pl,ph;
  // BIG INDIAN USED HERE
  
  while (!finished) {
    if (nbext > fromstr.Length()) {
      finished = Standard_True;
      tostr[nbtrans] = '\0';
    }
    else {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = (((unsigned int) curcar) >> 8) & 0xFF;
      pl = ((unsigned int) curcar) & 0xFF;
      Resource_unicode_to_euc(&ph,&pl);
      if (iseuc(ph)) {
	if (nbtrans < (maxsize-3)) {
	  tostr[nbtrans] = ((char) ph);
	  nbtrans++;
	  tostr[nbtrans] =  ((char) pl);
	  nbtrans++;
	}
	else {
	  tostr[nbtrans-1] = '\0';
	  nbtrans = maxsize-1;
	  return Standard_False;
	}
      }
      else {
	tostr[nbtrans] =  ((char) pl);
	nbtrans++;
      }
      if (nbtrans >= (maxsize - 1)) {
	tostr[maxsize-1] = '\0';
	finished = Standard_True;
	return Standard_False;
      }
    }
  }
  return Standard_True;
}
	  
Standard_Boolean Resource_Unicode::ConvertUnicodeToGB(const TCollection_ExtendedString& fromstr,
						      Standard_PCharacter& tostr,
						      const Standard_Integer maxsize)
{
  Standard_Integer nbtrans = 0;
  Standard_Integer nbext = 1;
  Standard_Boolean finished = Standard_False;
  Standard_ExtCharacter curcar;
  unsigned int pl,ph;
  // BIG INDIAN USED HERE
  
  while (!finished) {
    if (nbext > fromstr.Length()) {
      finished = Standard_True;
      tostr[nbtrans] = '\0';
    }
    else {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = (((unsigned int) curcar) >> 8) & 0xFF;
      pl = ((unsigned int) curcar) & 0xFF;
      Resource_unicode_to_gb(&ph,&pl);
      if (isshift(ph)) {
	if (nbtrans < (maxsize-3)) {
	  tostr[nbtrans] = ((char) ph);
	  nbtrans++;
	  tostr[nbtrans] =  ((char) pl);
	  nbtrans++;
	}
	else {
	  tostr[nbtrans-1] = '\0';
	  nbtrans = maxsize-1;
	  return Standard_False;
	}
      }
      else {
	tostr[nbtrans] =  ((char) curcar) & 0xFF;
	nbtrans++;
      }
      if (nbtrans >= (maxsize - 1)) {
	tostr[maxsize-1] = '\0';
	finished = Standard_True;
	return Standard_False;
      }
    }
  }
  return Standard_True;
}
	  
Standard_Boolean Resource_Unicode::ConvertUnicodeToANSI(const TCollection_ExtendedString& fromstr,
							Standard_PCharacter& tostr,
							const Standard_Integer maxsize)
{
  Standard_Integer nbtrans = 0;
  Standard_Integer nbext = 1;
  Standard_Boolean finished = Standard_False;
  Standard_ExtCharacter curcar;
  unsigned int pl,ph;
  // BIG INDIAN USED HERE
  
  while (!finished) {
    if (nbext > fromstr.Length()) {
      finished = Standard_True;
      tostr[nbtrans] = '\0';
    }
    else {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = ((unsigned int) curcar) >> 8;
      pl = ((unsigned int) curcar) & 0xFF;
      if (ph == 0) {
	tostr[nbtrans] =  ((char) pl);
      }
      else {
	tostr[nbtrans] =  ' ';
      }
      nbtrans++;
    }
    if (nbtrans >= (maxsize - 1)) {
      tostr[maxsize-1] = '\0';
      finished = Standard_True;
      return Standard_False;
    }
  }
  return Standard_True;
}

static Standard_Boolean AlreadyRead = Standard_False;
	  
static Resource_FormatType& Resource_Current_Format()
{
  static Resource_FormatType theformat = Resource_ANSI;
  if (!AlreadyRead) {
    AlreadyRead = Standard_True ;
    Handle(Resource_Manager) mgr = new Resource_Manager("CharSet");
    if (mgr->Find("FormatType")) {
      TCollection_AsciiString form = mgr->Value("FormatType");
      if (form.IsEqual("SJIS")) {
	theformat = Resource_SJIS;
      }
      else if (form.IsEqual("EUC")) {
	theformat = Resource_EUC;
      }
      else if (form.IsEqual("GB")) {
	theformat = Resource_GB;
      }
      else {
	theformat = Resource_ANSI;
      }
    }
    else {
      theformat = Resource_ANSI;
    }
  }
  return theformat;
}

void Resource_Unicode::SetFormat(const Resource_FormatType typecode)
{
  AlreadyRead = Standard_True;
  Resource_Current_Format() = typecode;
}

Resource_FormatType Resource_Unicode::GetFormat()
{
  return Resource_Current_Format();
}


void  Resource_Unicode::ReadFormat()
{
  AlreadyRead = Standard_False;
  Resource_Unicode::GetFormat();
}

void Resource_Unicode::ConvertFormatToUnicode(const Standard_CString fromstr,
					      TCollection_ExtendedString& tostr)
{
  Resource_FormatType theform = Resource_Unicode::GetFormat();
  switch (theform) {
  case Resource_SJIS :
    {
      ConvertSJISToUnicode(fromstr,tostr);
      break;
    }
  case Resource_EUC :
    {
      ConvertEUCToUnicode(fromstr,tostr);
      break;
    }
  case Resource_GB :
    {
      ConvertGBToUnicode(fromstr,tostr);
      break;
    }
  case Resource_ANSI :
    {
      ConvertANSIToUnicode(fromstr,tostr);
      break;
    }
  }
}

Standard_Boolean Resource_Unicode::ConvertUnicodeToFormat(const TCollection_ExtendedString& fromstr,
							  Standard_PCharacter& tostr,
							  const Standard_Integer maxsize)
{
  Resource_FormatType theform = Resource_Unicode::GetFormat();
  switch (theform) {
  case Resource_SJIS :
    {
      return ConvertUnicodeToSJIS(fromstr,tostr,maxsize);
    }
  case Resource_EUC :
    {
      return ConvertUnicodeToEUC(fromstr,tostr,maxsize);
    }
  case Resource_GB :
    {
      return ConvertUnicodeToGB(fromstr,tostr,maxsize);
    }
  case Resource_ANSI :
    {
      return ConvertUnicodeToANSI(fromstr,tostr,maxsize);
    }
  }
  return Standard_False;
}

