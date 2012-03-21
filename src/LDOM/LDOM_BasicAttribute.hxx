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



#ifndef LDOM_BasicAttribute_HeaderFile
#define LDOM_BasicAttribute_HeaderFile

#include <LDOM_BasicNode.hxx>
#include <LDOMBasicString.hxx>

class LDOM_XmlReader;
class LDOM_Element;
class LDOM_Attr;
class LDOM_Node;

//  Class LDOM_BasicAttribute
//

class LDOM_BasicAttribute : public LDOM_BasicNode
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_BasicAttribute () : LDOM_BasicNode (LDOM_Node::UNKNOWN) {}
  //    Empty constructor

  LDOM_BasicAttribute&  operator =      (const LDOM_NullPtr * aNull);
  //    Nullify

  const char *           GetName        () const { return myName; }

  const LDOMBasicString& GetValue       () const { return myValue; }

  void                   SetValue       (const LDOMBasicString&         aValue,
                                         const Handle(LDOM_MemManager)& aDoc)
                                { myValue = LDOMString (aValue, aDoc); }

 private:
  friend class LDOM_Node;
  friend class LDOM_Attr;
  friend class LDOM_Element;
  friend class LDOM_BasicElement;
  friend class LDOM_XmlReader;
  
  // ---------- PRIVATE METHODS ----------

  LDOM_BasicAttribute (const LDOMBasicString& aName)
    : LDOM_BasicNode (LDOM_Node::ATTRIBUTE_NODE), myName (aName.GetString()) {}
  //    Constructor

  static LDOM_BasicAttribute& Create (const LDOMBasicString&           theName,
                                      const Handle(LDOM_MemManager)&   theDoc,
                                      Standard_Integer&           theHashIndex);

  LDOM_BasicAttribute (const LDOM_Attr& anAttr);

 private:
  // ---------- PRIVATE FIELDS ----------

//  LDOMBasicString       myName;
  const char            * myName;
  LDOMBasicString       myValue;
};

#endif
