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

//AGV 140202: Repl.(const char *) for (LDOMBasicString) => myTagName

#ifndef LDOM_BasicElement_HeaderFile
#define LDOM_BasicElement_HeaderFile

#include <LDOM_BasicNode.hxx>
#include <LDOMBasicString.hxx>
#include <LDOM_Node.hxx>

class LDOM_XmlReader;
class LDOMParser;
class LDOM_NodeList;
class LDOM_Element;
class LDOM_BasicAttribute;

//  Class LDOM_BasicElement
//

class LDOM_BasicElement : public LDOM_BasicNode
{
 public:
 
  // ---------- PUBLIC METHODS ----------

  LDOM_BasicElement ()
    : LDOM_BasicNode    (LDOM_Node::UNKNOWN),
      myTagName         (NULL),
      myAttributeMask   (0),
      myFirstChild      (NULL) {}
  //    Empty constructor

  static LDOM_BasicElement& Create (const char                     * aName,
                                    const Standard_Integer         aLength,
                                    const Handle(LDOM_MemManager)& aDoc);

//  Standard_EXPORT LDOM_BasicElement (const LDOM_BasicElement& theOther);
  //    Copy constructor

  Standard_EXPORT LDOM_BasicElement&
                operator =              (const LDOM_NullPtr * aNull);
  //    Nullify

  Standard_EXPORT ~LDOM_BasicElement ();
  //    Destructor

  const char *  GetTagName              () const { return myTagName; }

  const LDOM_BasicNode *
                GetFirstChild           () const { return myFirstChild; }

  Standard_EXPORT const LDOM_BasicNode *
                GetLastChild            () const;

  Standard_EXPORT const LDOM_BasicAttribute&
                GetAttribute            (const LDOMBasicString& aName,
                                         const LDOM_BasicNode * aLastCh) const;
  //    Search for attribute name, using or setting myFirstAttribute

 protected:
  // ---------- PROTECTED METHODS ----------

//  LDOM_BasicElement (const LDOM_Element& anElement);
  //    Constructor

  Standard_EXPORT const LDOM_BasicNode *
                AddAttribute            (const LDOMBasicString&   anAttrName,
                                         const LDOMBasicString&   anAttrValue,
                                         const Handle(LDOM_MemManager)& aDoc,
                                         const LDOM_BasicNode     * aLastCh);
  //    add or replace an attribute to the element

  Standard_EXPORT const LDOM_BasicNode *
                RemoveAttribute         (const LDOMBasicString& aName,
                                         const LDOM_BasicNode * aLastCh) const;

  Standard_EXPORT void
                RemoveChild             (const LDOM_BasicNode * aChild) const;
  //    remove a child element

  Standard_EXPORT void
                AppendChild             (const LDOM_BasicNode *  aChild,
                                         const LDOM_BasicNode *& aLastCh) const;
  //    append a child node to the end of the list

 private:
  friend class LDOMParser;
  friend class LDOM_XmlReader;
  friend class LDOM_Document;
  friend class LDOM_Element;
  friend class LDOM_Node;
  // ---------- PRIVATE METHODS ----------

  const LDOM_BasicAttribute *
                GetFirstAttribute       (const LDOM_BasicNode *& aLastCh,
                                         const LDOM_BasicNode **& thePrN) const;

  void          RemoveNodes             ();

  void          ReplaceElement          (const LDOM_BasicElement&       anOther,
                                         const Handle(LDOM_MemManager)& aDoc);
  //    remark: recursive

  void          AddElementsByTagName    (LDOM_NodeList&         aList,
                                         const LDOMBasicString& aTagName) const;
  //    remark: recursive

  void          AddAttributes           (LDOM_NodeList&         aList,
                                         const LDOM_BasicNode * aLastCh) const;
  //    add attributes to list

 private:
  // ---------- PRIVATE FIELDS ----------

//  LDOMBasicString       myTagName;
  const char            * myTagName;
  unsigned long         myAttributeMask;
  LDOM_BasicNode        * myFirstChild;
};

#endif
