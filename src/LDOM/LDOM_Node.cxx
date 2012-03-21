// Created on: 2001-06-27
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


#include <LDOM_MemManager.hxx>
#include <LDOM_BasicAttribute.hxx>
#include <LDOM_BasicElement.hxx>
#include <LDOM_BasicText.hxx>

//=======================================================================
//function : Origin
//purpose  : 
//=======================================================================

const LDOM_BasicNode& LDOM_Node::Origin () const
{
  if (myOrigin == NULL) {
    static LDOM_BasicNode aNullNode;
    return aNullNode;
  }
  return * myOrigin;
}

//=======================================================================
//function : getOwnerDocument
//purpose  : 
//=======================================================================

const LDOM_MemManager& LDOM_Node::getOwnerDocument () const
{
  return myDocument -> Self();
}

//=======================================================================
//function : operator =
//purpose  : Assignment
//=======================================================================

LDOM_Node& LDOM_Node::operator = (const LDOM_Node& theOther)
{ 
  myDocument    = theOther.myDocument;
  myOrigin      = theOther.myOrigin;
  myLastChild   = theOther.myLastChild;
  return * this;
}

//=======================================================================
//function : operator =
//purpose  : Nullify
//=======================================================================

LDOM_Node& LDOM_Node::operator = (const LDOM_NullPtr * /*aNull*/)
{
  myDocument.Nullify();
  myOrigin    = NULL;
  myLastChild = NULL;
  return * this;
}

//=======================================================================
//function : isNull
//purpose  : 
//=======================================================================

Standard_Boolean LDOM_Node::isNull () const
{
  return myOrigin == NULL || myOrigin -> isNull();
}

//=======================================================================
//function : operator ==
//purpose  : Compare two Nodes
//=======================================================================

Standard_Boolean LDOM_Node::operator == (const LDOM_Node& anOther) const
{
  if (isNull())
    return anOther.isNull();
  return myOrigin == anOther.myOrigin;
}

//=======================================================================
//function : operator !=
//purpose  : Compare two Nodes
//=======================================================================

Standard_Boolean LDOM_Node::operator != (const LDOM_Node& anOther) const
{
  if (isNull())
    return !anOther.isNull();
  return myOrigin != anOther.myOrigin;
}

//=======================================================================
//function : getNodeType
//purpose  : 
//=======================================================================

LDOM_Node::NodeType LDOM_Node::getNodeType () const
{
  return myOrigin == NULL ? UNKNOWN : myOrigin -> getNodeType();
}

//=======================================================================
//function : getNodeName
//purpose  : 
//=======================================================================

LDOMString LDOM_Node::getNodeName () const
{
  switch (getNodeType()) {
  case ELEMENT_NODE:
    {
      const LDOM_BasicElement& anElement= *(const LDOM_BasicElement *) myOrigin;
      return LDOMString::CreateDirectString (anElement.GetTagName(),
                                             myDocument -> Self());
    }
  case ATTRIBUTE_NODE:
    {
      const LDOM_BasicAttribute& anAttr= *(const LDOM_BasicAttribute*) myOrigin;
      return LDOMString::CreateDirectString (anAttr.GetName(),
                                             myDocument -> Self());
    }
  default: ;
  }
  return LDOMString ();
}

//=======================================================================
//function : getNodeValue
//purpose  : 
//=======================================================================

LDOMString LDOM_Node::getNodeValue () const
{
  switch (getNodeType()) {
  case ATTRIBUTE_NODE:
    {
      const LDOM_BasicAttribute& anAttr= *(const LDOM_BasicAttribute*) myOrigin;
      return LDOMString (anAttr.GetValue(), myDocument -> Self());
    }
  case TEXT_NODE:
  case CDATA_SECTION_NODE:
  case COMMENT_NODE:
    {
      const LDOM_BasicText& aText = * (const LDOM_BasicText *) myOrigin;
      return LDOMString (aText.GetData(), myDocument -> Self());
    }
  default: ;
  }
  return LDOMString ();
}

//=======================================================================
//function : getFirstChild
//purpose  : 
//=======================================================================

LDOM_Node LDOM_Node::getFirstChild () const
{
  const NodeType aType = getNodeType ();
  if (aType == ELEMENT_NODE) {
    const LDOM_BasicElement& anElement = * (const LDOM_BasicElement *) myOrigin;
    const LDOM_BasicNode * aChild = anElement.GetFirstChild();
    if (aChild)
      if (aChild -> getNodeType() != LDOM_Node::ATTRIBUTE_NODE)
        return LDOM_Node (* aChild, myDocument);
  }
  return LDOM_Node ();
}

//=======================================================================
//function : getLastChild
//purpose  : 
//=======================================================================

LDOM_Node LDOM_Node::getLastChild () const
{
  const NodeType aType = getNodeType ();
  if (aType == ELEMENT_NODE) {
    if (myLastChild == NULL) {
      const LDOM_BasicElement& anElement = *(const LDOM_BasicElement*) myOrigin;
      (const LDOM_BasicNode *&) myLastChild = anElement.GetLastChild();
    }
    return LDOM_Node (* myLastChild, myDocument);
  }
  return LDOM_Node ();
}

//=======================================================================
//function : getNextSibling
//purpose  : 
//=======================================================================

LDOM_Node LDOM_Node::getNextSibling () const
{
  const LDOM_BasicNode * aSibling = myOrigin -> mySibling;
  if (aSibling)
    if (aSibling -> getNodeType () != ATTRIBUTE_NODE)
      return LDOM_Node (* aSibling, myDocument);
  return LDOM_Node ();
}

//=======================================================================
//function : removeChild
//purpose  : 
//=======================================================================

void LDOM_Node::removeChild (const LDOM_Node& aChild)
{
  const NodeType aType = getNodeType ();
  if (aType == ELEMENT_NODE) {
    const LDOM_BasicElement& anElement = * (LDOM_BasicElement *) myOrigin;
    if (aChild != NULL)
      anElement.RemoveChild (aChild.myOrigin);
    if (aChild.myOrigin == myLastChild)
//      myLastChild = anElement.GetLastChild();
      myLastChild = NULL;
  }
}

//=======================================================================
//function : appendChild
//purpose  : 
//=======================================================================

void LDOM_Node::appendChild (const LDOM_Node& aChild)
{
  const NodeType aType = getNodeType ();
  if (aType == ELEMENT_NODE && aChild != NULL) {
    if (myLastChild) {
      aChild.myOrigin -> SetSibling (myLastChild -> mySibling);
      (const LDOM_BasicNode *&) myLastChild -> mySibling = aChild.myOrigin;
    }else{
      const LDOM_BasicElement& anElement = * (LDOM_BasicElement *) myOrigin;
      anElement.AppendChild (aChild.myOrigin, myLastChild);
    }
    myLastChild = aChild.myOrigin;
  }
}

//=======================================================================
//function : hasChildNodes
//purpose  : 
//=======================================================================

Standard_Boolean LDOM_Node::hasChildNodes () const
{
  const NodeType aType = getNodeType ();
  if (aType == ELEMENT_NODE) {
    const LDOM_BasicElement& anElement = * (const LDOM_BasicElement *) myOrigin;
    const LDOM_BasicNode * aChild = anElement.GetFirstChild();
    if (aChild) return ! aChild -> isNull(); 
  }
  return Standard_False;
}

//=======================================================================
//function : SetValueClear
//purpose  : 
//=======================================================================

void LDOM_Node::SetValueClear () const
{
  LDOMBasicString * aValue = NULL;
  switch (getNodeType()) {
  case ATTRIBUTE_NODE:
    {
      const LDOM_BasicAttribute& anAttr= *(const LDOM_BasicAttribute*) myOrigin;
      aValue = (LDOMBasicString *) & anAttr.GetValue();
      break;
    }
  case TEXT_NODE:
  case CDATA_SECTION_NODE:
  case COMMENT_NODE:
    {
      const LDOM_BasicText& aText = * (const LDOM_BasicText *) myOrigin;
      aValue = (LDOMBasicString *) & aText.GetData();
      break;
    }
  default: return;
  }
  if (aValue -> Type() == LDOMBasicString::LDOM_AsciiDoc)
    aValue -> myType = LDOMBasicString::LDOM_AsciiDocClear;
}
