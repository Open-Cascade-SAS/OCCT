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

#ifndef LDOM_Element_HeaderFile
#define LDOM_Element_HeaderFile

#include <LDOM_Attr.hxx>
#include <LDOM_NodeList.hxx>

class LDOM_BasicElement;

//  Class LDOM_Element
//

class LDOM_Element : public LDOM_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_Element () {}
  //    Empty constructor

  LDOM_Element (const LDOM_Element& anOther) : LDOM_Node (anOther) {}
  //    Copy constructor

  LDOM_Element& operator =              (const LDOM_Element& anOther)
                    { return (LDOM_Element&) LDOM_Node::operator = (anOther); }
  //    Assignment

  LDOM_Element& operator =              (const LDOM_NullPtr * aNull)
                    { return (LDOM_Element&) LDOM_Node::operator = (aNull); }
  //    Nullify

  LDOMString    getTagName              () const { return getNodeName(); }

  Standard_EXPORT LDOMString
                getAttribute            (const LDOMString& aName) const; 

  Standard_EXPORT LDOM_Attr
                getAttributeNode        (const LDOMString& aName) const;

  Standard_EXPORT LDOM_NodeList
                getElementsByTagName    (const LDOMString& aName) const;

  Standard_EXPORT void  setAttribute    (const LDOMString& aName,
                                         const LDOMString& aValue);

  Standard_EXPORT void  setAttributeNode(const LDOM_Attr& aNewAttr);

  Standard_EXPORT void  removeAttribute (const LDOMString& aName);

//      AGV auxiliary API
  Standard_EXPORT LDOM_Element
                GetChildByTagName       (const LDOMString& aTagName) const;

  Standard_EXPORT LDOM_Element
                GetSiblingByTagName     () const;

  Standard_EXPORT void
                ReplaceElement          (const LDOM_Element& anOther);
  //    The old element is destroyed by the new one

  Standard_EXPORT LDOM_NodeList
                GetAttributesList       () const;

 protected:
  friend class LDOM_Document;
  friend class LDOMParser;
  // ---------- PROTECTED METHODS ----------

  LDOM_Element                          (const LDOM_BasicElement&       anElem,
                                         const Handle(LDOM_MemManager)& aDoc);

 private:
  // ---------- PRIVATE FIELDS ----------

};


#endif
