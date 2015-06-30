// Created on: 2001-06-27
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

#include <LDOM_MemManager.hxx>
#include <LDOM_BasicElement.hxx>
#include <LDOM_BasicAttribute.hxx>
#include <LDOM_Element.hxx>

#include <Standard_ProgramError.hxx>

#include <stdio.h>

//=======================================================================
//function : LDOM_Element
//purpose  : 
//=======================================================================

LDOM_Element::LDOM_Element (const LDOM_BasicElement&       anElem,
                            const Handle(LDOM_MemManager)& aDoc)
     : LDOM_Node (anElem, aDoc) {}

//=======================================================================
//function : getAttribute
//purpose  : 
//=======================================================================

LDOMString LDOM_Element::getAttribute (const LDOMString& aName) const
{
  const LDOM_BasicElement& anElem = (const LDOM_BasicElement&) Origin();
  if (anElem.isNull()) return LDOMString ();
  if (myLastChild == NULL) {
    const LDOM_BasicNode * aNode = anElem.GetFirstChild();
    if (aNode && aNode -> getNodeType () != LDOM_Node::ATTRIBUTE_NODE)
      for(;;) {
        const LDOM_BasicNode * aSibling = aNode -> GetSibling();
        if (aSibling == NULL)
          return LDOMString ();
        if (aSibling -> getNodeType () == LDOM_Node::ATTRIBUTE_NODE) {
          (const LDOM_BasicNode *&) myLastChild = aNode;
          break;
        }
        aNode = aSibling;
      }
  }
  const LDOM_BasicAttribute& anAttr = anElem.GetAttribute (aName, myLastChild);
  if (anAttr.isNull())
    return LDOMString ();
  return LDOMString (anAttr.GetValue(), myDocument -> Self());
}

//=======================================================================
//function : getAttributeNode
//purpose  : 
//=======================================================================

LDOM_Attr LDOM_Element::getAttributeNode (const LDOMString& aName) const
{
  const LDOM_BasicElement& anElem = (const LDOM_BasicElement&) Origin();
  if (anElem.isNull()) return LDOM_Attr ();
  if (myLastChild == NULL) {
    const LDOM_BasicNode * aNode = anElem.GetFirstChild();
    if (aNode && aNode -> getNodeType () != LDOM_Node::ATTRIBUTE_NODE)
      for(;;) {
        const LDOM_BasicNode * aSibling = aNode -> GetSibling();
        if (aSibling == NULL)
          return LDOM_Attr ();
        if (aSibling -> getNodeType () == LDOM_Node::ATTRIBUTE_NODE) {
          (const LDOM_BasicNode *&) myLastChild = aSibling;
          break;
        }
        aNode = aSibling;
      }
  }
  const LDOM_BasicAttribute& anAttr = anElem.GetAttribute (aName, myLastChild);
  return LDOM_Attr (anAttr, myDocument);
}

//=======================================================================
//function : getElementsByTagName
//purpose  : 
//=======================================================================

LDOM_NodeList LDOM_Element::getElementsByTagName
                                        (const LDOMString& theTagName) const
{
  LDOM_NodeList aList (myDocument);
  if (isNull() == Standard_False) {
    const LDOM_BasicElement& anElem = (const LDOM_BasicElement&) Origin();
//    if (anElem.GetTagName().equals(theTagName))
    if (strcmp (anElem.GetTagName(), theTagName.GetString()) == 0)
      aList.Append (anElem);
    anElem.AddElementsByTagName (aList, theTagName);
  }
  return aList;
}

//=======================================================================
//function : setAttribute
//purpose  : 
//=======================================================================

void LDOM_Element::setAttribute (const LDOMString& aName,const LDOMString& aVal)
{
  LDOM_BasicElement& anElem = (LDOM_BasicElement&) Origin();
  if (anElem.isNull()) return;

  myLastChild = anElem.AddAttribute (aName, LDOMString (aVal, myDocument),
                                     myDocument, myLastChild);
}

//=======================================================================
//function : setAttributeNode
//purpose  : 
//=======================================================================

void LDOM_Element::setAttributeNode (const LDOM_Attr& aNewAttr)
{
  setAttribute (aNewAttr.getName(), aNewAttr.getValue());
}

//=======================================================================
//function : removeAttribute
//purpose  : 
//=======================================================================

void LDOM_Element::removeAttribute (const LDOMString& aName)
{
  const LDOM_BasicElement& anElem = (const LDOM_BasicElement&) Origin();
  if (anElem.isNull()) return;
  anElem.RemoveAttribute (aName, myLastChild);
}

//=======================================================================
//function : GetChildByTagName
//purpose  : 
//=======================================================================

LDOM_Element LDOM_Element::GetChildByTagName (const LDOMString& aTagName) const
{
// Verify preconditions
  LDOM_Element aVoidElement;
  if (isNull() || aTagName == NULL)
    return aVoidElement;
    
// Take the first child. If it doesn't match look for other ones in a loop
  LDOM_Node aChildNode = getFirstChild();
  while (aChildNode != NULL)
  {
    const LDOM_Node::NodeType aNodeType = aChildNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
      break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE)
    {
      LDOMString
#ifdef DOM2_MODEL
        aNodeName = aChildNode.getLocalName(); // try DOM2/namespaces
      if (aNodeName == NULL)
#endif
        aNodeName = aChildNode.getNodeName();          // use DOM1
      if (aNodeName.equals(aTagName))
        return (LDOM_Element&) aChildNode;           // a match has been found
    }
    aChildNode = aChildNode.getNextSibling();
  }
  return aVoidElement;
}

//=======================================================================
//function : GetSiblingByTagName
//purpose  : 
//=======================================================================

LDOM_Element LDOM_Element::GetSiblingByTagName () const
{
// Verify preconditions
  LDOM_Element aVoidElement;
  if (isNull()) return aVoidElement;

  LDOMString aTagName = getTagName();
    
// Take the first child. If it doesn't match look for other ones in a loop
  LDOM_Node aNextNode = getNextSibling();
  while (aNextNode != NULL)
  {
    const LDOM_Node::NodeType aNodeType = aNextNode.getNodeType();
    if (aNodeType == LDOM_Node::ATTRIBUTE_NODE)
      break;
    if (aNodeType == LDOM_Node::ELEMENT_NODE)
    {
      LDOM_Element aNextElement = (LDOM_Element&) aNextNode;
      if (aNextElement.getTagName().equals(aTagName))
        return aNextElement;           // a match has been found
    }
    aNextNode = aNextNode.getNextSibling();
  }
  return aVoidElement;
}

//=======================================================================
//function : ReplaceElement
//purpose  : Permanently replace the element erasing the old data though the
//           children are not erased.
//           If anOther belongs to different Document, full copy of all its
//           children is performed.
//=======================================================================

void LDOM_Element::ReplaceElement (const LDOM_Element& anOther)
{
  LDOM_BasicElement& anElem = (LDOM_BasicElement&) Origin();
  const LDOM_BasicElement& anOtherElem =
      (const LDOM_BasicElement&) anOther.Origin();
  if (myDocument == anOther.myDocument) {
    anElem.myTagName       = anOtherElem.myTagName;
    anElem.myAttributeMask = anOtherElem.myAttributeMask;
    anElem.myFirstChild    = anOtherElem.myFirstChild;
    (const LDOM_BasicNode *&) myLastChild = anOther.myLastChild;
  } else {
    anElem.ReplaceElement (anOtherElem, myDocument);
    (const LDOM_BasicNode *&) myLastChild = NULL;
  }
}

//=======================================================================
//function : GetAttributesList
//purpose  : 
//=======================================================================

LDOM_NodeList LDOM_Element::GetAttributesList () const
{
  LDOM_NodeList aList (myDocument);
  const LDOM_BasicElement& anElem = (const LDOM_BasicElement&) Origin();
  anElem.AddAttributes (aList, myLastChild);
  return aList;
}
