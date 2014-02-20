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
