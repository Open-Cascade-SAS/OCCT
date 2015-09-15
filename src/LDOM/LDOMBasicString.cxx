// Created on: 2001-06-26
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <LDOMBasicString.hxx>
#include <LDOM_MemManager.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <errno.h>

//=======================================================================
//function : LDOMString
//purpose  : Create a free string (not connected to any type of container)
//=======================================================================

LDOMBasicString::LDOMBasicString (const char * aValue)
{
  if (aValue == NULL /*|| aValue[0] == '\0'*/) {
    myType = LDOM_NULL;
    myVal.ptr = NULL;
  } else {
    myType = LDOM_AsciiFree;
    Standard_Size aLen = strlen (aValue) + 1;
    myVal.ptr = new char [aLen];
    memcpy (myVal.ptr, aValue, aLen);
  }
}

//=======================================================================
//function : LDOMString
//purpose  : Create an Ascii string managed by LDOM_Document
//=======================================================================

LDOMBasicString::LDOMBasicString (const char                     * aValue,
                                  const Handle(LDOM_MemManager)& aDoc)
{
  if (aValue == NULL /*|| aValue[0] == '\0'*/) {
    myType = LDOM_NULL;
    myVal.ptr = NULL;
  } else {
    myType = LDOM_AsciiDoc;
    Standard_Integer aLen = (Standard_Integer) strlen (aValue) + 1;
    myVal.ptr = aDoc -> Allocate (aLen);
    memcpy (myVal.ptr, aValue, aLen);
  }
}
  
//=======================================================================
//function : LDOMString
//purpose  : Create an Ascii string managed by LDOM_Document
//=======================================================================

LDOMBasicString::LDOMBasicString (const char                     * aValue,
                                  const Standard_Integer         aLen,
                                  const Handle(LDOM_MemManager)& aDoc)
{
  if (aValue == NULL || aLen == 0) {
    myType = LDOM_NULL;
    myVal.ptr = NULL;
  } else {
    myType = LDOM_AsciiDoc;
    myVal.ptr = aDoc -> Allocate (aLen + 1);
    memcpy (myVal.ptr, aValue, aLen);
    ((char *)myVal.ptr)[aLen] = '\0';
  }
}
  
//=======================================================================
//function : LDOMBasicString
//purpose  : Copy constructor
//=======================================================================

LDOMBasicString::LDOMBasicString (const LDOMBasicString& anOther)
    : myType (anOther.Type())
{
  switch (myType) {
  case LDOM_AsciiFree:
    if (anOther.myVal.ptr) {
      Standard_Size aLen = strlen ((const char *)anOther.myVal.ptr) + 1;
      myVal.ptr = new char [aLen];
      memcpy (myVal.ptr, anOther.myVal.ptr, aLen);
      break;
    }
  case LDOM_AsciiDoc:
  case LDOM_AsciiDocClear:
  case LDOM_AsciiHashed:
    myVal.ptr = anOther.myVal.ptr;
    break;
  case LDOM_Integer:
    myVal.i = anOther.myVal.i;
  default: ;
  }
}

//=======================================================================
//function : ~LDOMString()
//purpose  : Destructor
//=======================================================================

LDOMBasicString::~LDOMBasicString ()
{
  if (myType == LDOM_AsciiFree) {
    if (myVal.ptr)
      delete [] (char *) myVal.ptr;
  }
}

//=======================================================================
//function : operator =
//purpose  : Assignment to NULL
//=======================================================================

LDOMBasicString& LDOMBasicString::operator = (const LDOM_NullPtr *)
{
  if (myType == LDOM_AsciiFree && myVal.ptr)
    delete [] (char *) myVal.ptr;
  myType    = LDOM_NULL;
  myVal.ptr = NULL;
  return * this;
}

//=======================================================================
//function : operator =
//purpose  : Assignment
//=======================================================================

LDOMBasicString& LDOMBasicString::operator = (const LDOMBasicString& anOther)
{
  if (myType == LDOM_AsciiFree && myVal.ptr)
    delete [] (char *) myVal.ptr;
  myType    = anOther.Type();
  switch (myType) {
  case LDOM_AsciiFree:
    if (anOther.myVal.ptr) {
      Standard_Size aLen = strlen ((const char *)anOther.myVal.ptr) + 1;
      myVal.ptr = new char [aLen];
      memcpy (myVal.ptr, anOther.myVal.ptr, aLen);
      break;
    }
  case LDOM_AsciiDoc:
  case LDOM_AsciiDocClear:
  case LDOM_AsciiHashed:
    myVal.ptr = anOther.myVal.ptr;
    break;
  case LDOM_Integer:
    myVal.i = anOther.myVal.i;
  default: ;
  }
  return * this;
}

//=======================================================================
//function : equals
//purpose  : Compare two strings by content
//=======================================================================

Standard_Boolean LDOMBasicString::equals (const LDOMBasicString& anOther) const
{
  Standard_Boolean aResult = Standard_False;
  switch (myType)
  {
  case LDOM_NULL:
    return (anOther == NULL);
  case LDOM_Integer:
    switch (anOther.Type())
    {
    case LDOM_Integer:
      return (myVal.i == anOther.myVal.i);
    case LDOM_AsciiFree:
    case LDOM_AsciiDoc:
    case LDOM_AsciiDocClear:
    case LDOM_AsciiHashed:
      {
        long aLongOther = strtol ((const char *) anOther.myVal.ptr, NULL, 10);
        return (errno == 0 && aLongOther == long(myVal.i));
      }
    case LDOM_NULL:
    default:
      ;
    }
    break;
  default:
    switch (anOther.Type())
    {
    case LDOM_Integer:
      {
        long aLong = strtol ((const char *) myVal.ptr, NULL, 10);
        return (errno == 0 && aLong == long(anOther.myVal.i));
      }
    case LDOM_AsciiFree:
    case LDOM_AsciiDoc:
    case LDOM_AsciiDocClear:
    case LDOM_AsciiHashed:
      return (strcmp ((const char *) myVal.ptr,
                      (const char *) anOther.myVal.ptr) == 0);
    case LDOM_NULL:
    default:
      ;
    }
  }
  return aResult;
}

//=======================================================================
//function : operator TCollection_AsciiString
//purpose  : 
//=======================================================================

LDOMBasicString::operator TCollection_AsciiString () const
{
  switch (myType) {
  case LDOM_Integer:
    return TCollection_AsciiString (myVal.i);
  case LDOM_AsciiFree:
  case LDOM_AsciiDoc:
  case LDOM_AsciiDocClear:
  case LDOM_AsciiHashed:
    return TCollection_AsciiString (Standard_CString(myVal.ptr));
  default: ;
  }
  return TCollection_AsciiString ();
}

//=======================================================================
//function : operator TCollection_ExtendedString
//purpose  : 
//=======================================================================

LDOMBasicString::operator TCollection_ExtendedString () const
{
  switch (myType) {
  case LDOM_Integer:
    return TCollection_ExtendedString (myVal.i);
  case LDOM_AsciiFree:
  case LDOM_AsciiDoc:
  case LDOM_AsciiDocClear:
  case LDOM_AsciiHashed:
  {
    char buf[6] = {'\0','\0','\0','\0','\0','\0'};
    const long aUnicodeHeader = 0xfeff;
    Standard_CString ptr = Standard_CString (myVal.ptr);
    errno = 0;
    // Check if ptr is ascii string
    if (ptr[0] != '#' || ptr[1] != '#')
      return TCollection_ExtendedString (ptr);
    buf[0] = ptr[2];
    buf[1] = ptr[3];
    buf[2] = ptr[4];
    buf[3] = ptr[5];
    if (strtol (&buf[0], NULL, 16) != aUnicodeHeader)
      return TCollection_ExtendedString (ptr);

    // convert Unicode to Extended String
    ptr += 2;
    Standard_Size aLength = (strlen(ptr) / 4), j = 0;
    Standard_ExtCharacter * aResult = new Standard_ExtCharacter[aLength--];
    while (aLength--) {
      ptr += 4;
      buf[0] = ptr[0];
      buf[1] = ptr[1];
      buf[2] = ptr[2];
      buf[3] = ptr[3];
      aResult[j++] = Standard_ExtCharacter (strtol (&buf[0], NULL, 16));
      if (errno) {
        delete [] aResult;
        return TCollection_ExtendedString();
      }
    }
    aResult[j] = 0;
    TCollection_ExtendedString aResultStr (aResult);
    delete [] aResult;
    return aResultStr;
  }
  default: ;
  }
  return TCollection_ExtendedString();
}

//=======================================================================
//function : GetInteger
//purpose  : Conversion to Integer
//=======================================================================

Standard_Boolean LDOMBasicString::GetInteger (Standard_Integer& aResult) const
{
  switch (myType) {
  case LDOM_Integer:
    aResult = myVal.i;
    break;
  case LDOM_AsciiFree:
  case LDOM_AsciiDoc:
  case LDOM_AsciiDocClear:
  case LDOM_AsciiHashed:
    {
      char * ptr;
      long aValue = strtol ((const char *)myVal.ptr, &ptr, 10);
      if (ptr == myVal.ptr || errno == ERANGE || errno == EINVAL)
        return Standard_False;
      aResult = Standard_Integer (aValue);
      break;
    }
  default:
    return Standard_False;
  }
  return Standard_True;
}

#ifdef OCCT_DEBUG
#ifndef _MSC_VER
//=======================================================================
// Debug Function for DBX: use "print -p <Variable> or pp <Variable>"
//=======================================================================
#include <LDOM_OSStream.hxx>
#define FLITERAL 0x10
char * db_pretty_print (const LDOMBasicString * aString, int fl, char *)
{
  LDOM_OSStream out (128);
  const char * str;
  switch (aString -> myType) {
  case LDOMBasicString::LDOM_Integer:
    if ((fl & FLITERAL) == 0)  out << "LDOM_Integer: ";
    out << '\"' << aString -> myVal.i << '\"'; goto finis;
  case LDOMBasicString::LDOM_AsciiFree:
    if ((fl & FLITERAL) == 0)  out << "LDOM_AsciiFree: ";
    break;
  case LDOMBasicString::LDOM_AsciiDoc:
    if ((fl & FLITERAL) == 0)  out << "LDOM_AsciiDoc: ";
    break;
  case LDOMBasicString::LDOM_AsciiDocClear:
    if ((fl & FLITERAL) == 0)  out << "LDOM_AsciiDocClear: ";
    break;
  case LDOMBasicString::LDOM_AsciiHashed:
    if ((fl & FLITERAL) == 0)  out << "LDOM_AsciiHashed: ";
    break;
  default:
    out << "Unknown type";
  }
  str = (const char *) aString -> myVal.ptr;
  out << '\"';
  if (strlen (str) > 512)
    out.rdbuf() -> sputn (str, 512);
  else
    out << str;
  out << '\"';
 finis:
  out << ends;
  return (char *)out.str();
}
#endif
#endif
