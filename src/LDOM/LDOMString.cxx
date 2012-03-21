// Created on: 2001-06-25
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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
      Standard_Integer aLen = strlen (aString) + 1;
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
