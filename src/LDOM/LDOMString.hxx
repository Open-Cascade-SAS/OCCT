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



#ifndef LDOMString_HeaderFile
#define LDOMString_HeaderFile

#include <LDOMBasicString.hxx>
#include <Handle_LDOM_MemManager.hxx>

//  Class LDOMString
//  Represents various object types which can be mapped to XML strings
//  LDOMString is not an independent type: you must be sure that the owner
//  LDOM_Document is never lost during the lifetime of its LDOMStrings - for
//  that it is necessary to keep at least one LDOM_Document or LDOM_Node alive
//  before all LDOMString's (LDOM_AsciiDoc type) are destroyed.

class LDOMString : public LDOMBasicString
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOMString                    () : myPtrDoc (NULL) {}
  //    Empty constructor

  LDOMString                    (const LDOMString& anOther)
    : LDOMBasicString (anOther), myPtrDoc (anOther.myPtrDoc) {}
  //    Copy constructor

  LDOMString                    (const Standard_Integer aValue)
    : LDOMBasicString (aValue), myPtrDoc (NULL) {}
  //    Integer => LDOMString

//  Standard_EXPORT LDOMString (const Standard_Real aValue);

  LDOMString                    (const char * aValue)
    : LDOMBasicString (aValue), myPtrDoc (NULL) {}
  //    Create LDOM_AsciiFree

  const LDOM_MemManager&       getOwnerDocument  () const
                        { return * myPtrDoc; }

  LDOMString&                   operator =        (const LDOM_NullPtr * aNull)
                        { LDOMBasicString::operator= (aNull); return *this; }

  LDOMString&                   operator =        (const LDOMString& anOther)
  {
    myPtrDoc = anOther.myPtrDoc;
    LDOMBasicString::operator= (anOther);
    return * this;
  }

 private:
  friend class LDOM_Document;
  friend class LDOM_Node;
  friend class LDOM_Element;
  friend class LDOM_BasicElement;
  friend class LDOM_BasicAttribute;
  friend class LDOM_BasicText;

  static LDOMString CreateDirectString
                                (const char             * aValue,
                                 const LDOM_MemManager& aDoc);

  LDOMString                    (const LDOMBasicString& anOther,
                                 const LDOM_MemManager& aDoc)
    : LDOMBasicString (anOther), myPtrDoc (&aDoc) {}
  //    Plain copy from LDOMBasicString

  LDOMString                    (const LDOMBasicString&         anOther,
                                 const Handle(LDOM_MemManager)& aDoc);
  //    Copy from another string with allocation in the document space

 private:
  // ---------- PRIVATE FIELDS -------------
  const LDOM_MemManager * myPtrDoc;
};

#endif
