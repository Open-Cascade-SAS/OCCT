// Created on: 2001-06-26
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



#ifndef LDOM_Element_HeaderFile
#define LDOM_Element_HeaderFile

#include <LDOM_Attr.hxx>
#include <LDOM_NodeList.hxx>

class LDOMParser;
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

  Standard_EXPORT LDOM_Element&
                operator =              (const LDOM_NullPtr * aNull)
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
