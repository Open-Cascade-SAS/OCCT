// Created on: 2001-06-25
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

#include <LDOMString.hxx>
#include <LDOM_MemManager.hxx>

//=======================================================================
//function : CreateDirectString
//purpose  : Only for hashed strings!!
//=======================================================================

LDOMString LDOMString::CreateDirectString (const char             * aValue,
                                           const LDOM_MemManager& aDoc)
{
  LDOMString aResult;
  aResult.myPtrDoc = &aDoc;
  aResult.SetDirect (LDOMBasicString::LDOM_AsciiHashed, aValue);
  return aResult;
}

//=======================================================================
//function : LDOMString
//purpose  : Copy from another string with allocation in the document
//=======================================================================

LDOMString::LDOMString (const LDOMBasicString&          anOther,
                        const Handle(LDOM_MemManager)&  aDoc)
     : myPtrDoc (&aDoc -> Self())
{
  myType = anOther.Type();
  switch (myType)
  {
  case LDOM_Integer:
    anOther.GetInteger (myVal.i);
    break;
  case LDOM_AsciiFree:
    myType = LDOM_AsciiDoc;
  case LDOM_AsciiDocClear:
  case LDOM_AsciiDoc:
    {
      const char * aString = anOther.GetString ();
      Standard_Integer aLen = (Standard_Integer)(strlen (aString) + 1);
      myVal.ptr = ((LDOM_MemManager *) myPtrDoc) -> Allocate (aLen);
      memcpy (myVal.ptr, aString, aLen);
    }
    break;
  case LDOM_AsciiHashed:
    myVal.ptr = (void *)anOther.GetString ();
    break;
  default:
    myType = LDOM_NULL;
  }
}

//=======================================================================
//function : LDOMString
//purpose  : Copy from another with allocation in the document if necessary
//=======================================================================
/*
LDOMString::LDOMString (const LDOMString& anOther, const LDOM_Document& aDoc)
     : myPtrDoc (&aDoc.myMemManager -> Self())
{
  switch (anOther.Type())
  {
  case LDOM_Integer:
    myType = LDOM_Integer;
    anOther.GetInteger (myVal.i);
    break;
  case LDOM_AsciiDoc:
    if (aDoc == anOther.getOwnerDocument())
  case LDOM_AsciiHashed:
      myVal.ptr = (void *)anOther.GetString ();
    else {
  case LDOM_AsciiFree:
      const char * aString = anOther.GetString ();
      Standard_Integer aLen = strlen (aString) + 1;
      myVal.ptr = aDoc.AllocMem (aLen);
      memcpy (myVal.ptr, aString, aLen);
      myType = LDOM_AsciiDoc;
    }
    break;
  default: ;
  }
}
*/
