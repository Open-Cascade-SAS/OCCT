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

//AGV 140202: Replace(const char *) for (LDOMBasicString)=>myTagName

#include <LDOM_BasicElement.hxx>
#include <LDOM_BasicAttribute.hxx>
#include <LDOM_BasicText.hxx>
#include <LDOM_MemManager.hxx>
#include <LDOM_NodeList.hxx>

//=======================================================================
//function : Create
//purpose  : construction in the Document's data pool
//=======================================================================

LDOM_BasicElement& LDOM_BasicElement::Create
                                        (const char                     * aName,
                                         const Standard_Integer         aLen,
                                         const Handle(LDOM_MemManager)& aDoc)
{
  if (aName == NULL) {
    static LDOM_BasicElement aVoidElement;
    aVoidElement = LDOM_BasicElement();
    return aVoidElement;
  }
  void * aMem = aDoc -> Allocate (sizeof(LDOM_BasicElement));
  LDOM_BasicElement * aNewElem = new (aMem) LDOM_BasicElement;

  Standard_Integer aHash;
//  aDoc -> HashedAllocate (aString, strlen(aString), aNewElem -> myTagName);
  aNewElem -> myTagName =  aDoc -> HashedAllocate (aName, aLen, aHash);

  aNewElem -> myNodeType = LDOM_Node::ELEMENT_NODE;
  return * aNewElem;
}

//=======================================================================
//function : RemoveNodes
//purpose  : 
//=======================================================================

void LDOM_BasicElement::RemoveNodes ()
{
  const LDOM_BasicNode * aNode = (const LDOM_BasicNode *) myFirstChild;
  while (aNode) {
    const LDOM_BasicNode * aNext = aNode -> GetSibling();
    switch (aNode -> getNodeType ()) {
    case LDOM_Node::ELEMENT_NODE:
      {
        LDOM_BasicElement& anElement = * (LDOM_BasicElement *) aNode;
        anElement = NULL;
        break;
      }
    case LDOM_Node::ATTRIBUTE_NODE:
      {
        LDOM_BasicAttribute& anAttr = * (LDOM_BasicAttribute *) aNode;
        anAttr = NULL;
        break;
      }
    case LDOM_Node::TEXT_NODE:
    case LDOM_Node::COMMENT_NODE:
    case LDOM_Node::CDATA_SECTION_NODE:
      {
        LDOM_BasicText& aTxt = * (LDOM_BasicText *) aNode;
        aTxt = NULL;
        break;
      }
    default: ;
    }
    aNode = aNext;
  }
  myFirstChild = NULL;
}

//=======================================================================
//function : operator =
//purpose  : Nullify
//=======================================================================

LDOM_BasicElement& LDOM_BasicElement:: operator = (const LDOM_NullPtr * aNull)
{
  myTagName = NULL;
  RemoveNodes ();
  LDOM_BasicNode::operator= (aNull);
  return * this;
}

//=======================================================================
//function : LDOM_BasicElement
//purpose  : Constructor
//=======================================================================
/*
LDOM_BasicElement::LDOM_BasicElement (const LDOM_Element& anElement)
     : LDOM_BasicNode   (LDOM_Node::ELEMENT_NODE),
       myAttributeMask  (0),
       myFirstChild     (NULL)
{
//  LDOMString aNewTagName (anElement.getTagName(), anElement.myDocument);
//  myTagName = aNewTagName;
  const LDOM_BasicElement& anOther =
    (const LDOM_BasicElement&) anElement.Origin();
  myTagName = anOther.GetTagName();
}
*/
//=======================================================================
//function : ~LDOM_BasicElement
//purpose  : Destructor
//=======================================================================

LDOM_BasicElement::~LDOM_BasicElement ()
{
  myTagName = NULL;
  RemoveNodes ();
}

//=======================================================================
//function : GetLastChild
//purpose  : 
//=======================================================================

const LDOM_BasicNode * LDOM_BasicElement::GetLastChild () const
{
  const LDOM_BasicNode * aNode = myFirstChild;
  if (aNode) {
    if (aNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE)
      aNode = NULL;
    else
      while (aNode -> mySibling) {
        if (aNode -> mySibling -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE)
          break;
        aNode = aNode -> mySibling;
      }
  }
  return aNode;
}

//=======================================================================
//function : GetAttribute
//purpose  : 
//=======================================================================

const LDOM_BasicAttribute& LDOM_BasicElement::GetAttribute
                                     (const LDOMBasicString& aName,
                                      const LDOM_BasicNode   * aLastCh) const
{
  const LDOM_BasicNode * aNode;
  if (aLastCh)
    aNode = aLastCh -> GetSibling (); 
  else
    aNode = myFirstChild;
  const char * aNameStr = aName.GetString();
  while (aNode) {
    if (aNode -> getNodeType () == LDOM_Node::ATTRIBUTE_NODE) {
      const LDOM_BasicAttribute * anAttr = (const LDOM_BasicAttribute *) aNode;
      if (!strcmp (aNameStr, anAttr -> GetName()))
        return * anAttr;
    }
    aNode = aNode -> mySibling;
  }
static const LDOM_BasicAttribute aNullAttribute;
  return aNullAttribute;
}

//=======================================================================
//function : GetFirstAttribute
//purpose  : private method
//=======================================================================

const LDOM_BasicAttribute * LDOM_BasicElement::GetFirstAttribute
                                    (const LDOM_BasicNode *&  theLastCh,
                                     const LDOM_BasicNode **& thePrevNode) const
{
  //  Find the First Attribute as well as the Last Child among siblings
  const LDOM_BasicNode * aFirstAttr;
  const LDOM_BasicNode ** aPrevNode;
  if (theLastCh) {
    aFirstAttr = theLastCh -> mySibling; 
    aPrevNode  = (const LDOM_BasicNode **) &(theLastCh -> mySibling);
    while (aFirstAttr) {
      if (aFirstAttr -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE) break;
      aPrevNode = (const LDOM_BasicNode **) & (aFirstAttr -> mySibling);
      aFirstAttr = aFirstAttr -> mySibling;
    }
  } else {
    aFirstAttr = myFirstChild;
    aPrevNode  = (const LDOM_BasicNode **) &myFirstChild;
    while (aFirstAttr) {
      if (aFirstAttr -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE) break;
      if (aFirstAttr -> isNull() == Standard_False) theLastCh = aFirstAttr;
      aPrevNode = (const LDOM_BasicNode **) & (aFirstAttr -> mySibling);
      aFirstAttr = aFirstAttr -> mySibling;
    }
  }
  thePrevNode = aPrevNode;
  return (LDOM_BasicAttribute *) aFirstAttr;
}

//=======================================================================
//function : AddAttribute
//purpose  : Add or replace an attribute
//=======================================================================

const LDOM_BasicNode * LDOM_BasicElement::AddAttribute
                                  (const LDOMBasicString&         anAttrName,
                                   const LDOMBasicString&         anAttrValue,
                                   const Handle(LDOM_MemManager)& aDocument,
                                   const LDOM_BasicNode           * aLastCh)
{
  //  Create attribute
  Standard_Integer aHash;
  LDOM_BasicAttribute& anAttr =
    LDOM_BasicAttribute::Create (anAttrName, aDocument, aHash);
  anAttr.myValue = anAttrValue;

  //  Initialize the loop of attribute name search
  const LDOM_BasicNode ** aPrNode;
  const LDOM_BasicAttribute * aFirstAttr = GetFirstAttribute (aLastCh, aPrNode);
  const char * aNameStr = anAttrName.GetString();

  //  Check attribute hash value against the current mask
  const unsigned int anAttrMaskValue = aHash & (8*sizeof(myAttributeMask) - 1);
  const unsigned long anAttributeMask = (1 << anAttrMaskValue);
#ifdef OCCT_DEBUG_MASK
  anAttributeMask = 0xffffffff;
#endif
  if ((myAttributeMask & anAttributeMask) == 0) {
    // this is new attribute, OK
    myAttributeMask |= anAttributeMask;
    * aPrNode = &anAttr;
    anAttr.SetSibling (aFirstAttr);
  } else {
    // this attribute may have already been installed  
    LDOM_BasicAttribute * aCurrentAttr = (LDOM_BasicAttribute *) aFirstAttr;
    while (aCurrentAttr) {
      if (aCurrentAttr -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE)
        if (LDOM_MemManager::CompareStrings (aNameStr, aHash,
                                             aCurrentAttr -> GetName())) {
          aCurrentAttr -> SetValue (anAttrValue, aDocument);
          break;
        }
      aCurrentAttr = (LDOM_BasicAttribute *) aCurrentAttr -> mySibling;
    }
    if (aCurrentAttr == NULL) {
      // this is new attribute, OK
      * aPrNode = &anAttr;
      anAttr.SetSibling (aFirstAttr);
    }
  }
  return aLastCh;
}

//=======================================================================
//function : RemoveAttribute
//purpose  : Find and delete an attribute from list
//=======================================================================

const LDOM_BasicNode * LDOM_BasicElement::RemoveAttribute
                                        (const LDOMBasicString& aName,
                                         const LDOM_BasicNode   * aLastCh) const
{
  //  Check attribute hash value against the current mask
  const char * const aNameStr = aName.GetString();
  const Standard_Integer aHash =
    LDOM_MemManager::Hash (aNameStr, (Standard_Integer)strlen(aNameStr));
  const unsigned int anAttrMaskValue = aHash & (8*sizeof(myAttributeMask) - 1);
  const unsigned long anAttributeMask = (1 << anAttrMaskValue);
#ifdef OCCT_DEBUG_MASK
  anAttributeMask = 0xffffffff;
#endif
  if ((myAttributeMask & anAttributeMask) == 0) {
    ; // maybe cause for exception
  } else {
    const LDOM_BasicNode ** aPrevNode;  // dummy
    const LDOM_BasicAttribute * anAttr = GetFirstAttribute (aLastCh, aPrevNode);
    while (anAttr) {
      if (anAttr -> getNodeType () == LDOM_Node::ATTRIBUTE_NODE)
        if (LDOM_MemManager::CompareStrings(aNameStr,aHash,anAttr->GetName())) {
          anAttr = NULL;
          break;
        }
      anAttr = (const LDOM_BasicAttribute *) anAttr -> mySibling;
    }
  }
  return aLastCh;
}

//=======================================================================
//function : RemoveChild
//purpose  : 
//=======================================================================

void LDOM_BasicElement::RemoveChild (const LDOM_BasicNode * aChild) const
{
  const LDOM_BasicNode *  aNode = myFirstChild;
  const LDOM_BasicNode ** aPrevNode = (const LDOM_BasicNode **) &myFirstChild;
  while (aNode) {
    if (aNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE)
      break;
    if (aNode == aChild) {
      * aPrevNode = aNode -> GetSibling();
      * (LDOM_BasicNode *) aChild = NULL;
      break;
    }
    aPrevNode = (const LDOM_BasicNode **) & (aNode -> mySibling);
    aNode = aNode -> GetSibling();
  }
  // here may be the cause to throw an exception
}

//=======================================================================
//function : AppendChild
//purpose  : 
//=======================================================================

void LDOM_BasicElement::AppendChild (const LDOM_BasicNode *  aChild,
                                     const LDOM_BasicNode *& aLastChild) const
{
  if (aLastChild) {
    (const LDOM_BasicNode *&) aChild -> mySibling = aLastChild -> mySibling;
    (const LDOM_BasicNode *&) aLastChild -> mySibling = aChild;
  } else {
    const LDOM_BasicNode * aNode = myFirstChild;
    const LDOM_BasicNode ** aPrevNode = (const LDOM_BasicNode **) &myFirstChild;
    while (aNode) {
      if (aNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE) {
        (const LDOM_BasicNode *&) aChild -> mySibling = aNode;
        break;
      }
      aPrevNode = (const LDOM_BasicNode **) & (aNode -> mySibling);
      aNode = aNode -> mySibling;
    }
    * aPrevNode = aChild;
  }
  aLastChild = aChild;
}

//=======================================================================
//function : AddElementsByTagName
//purpose  : Add to the List all sub-elements with the given name (recursive) 
//=======================================================================

void LDOM_BasicElement::AddElementsByTagName
                (LDOM_NodeList& aList, const LDOMBasicString& aTagName) const
{
  const LDOM_BasicNode  * aNode         = myFirstChild;
  const char            * aTagString    = aTagName.GetString();
  while (aNode) {
    if (aNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE)
      break;
    if (aNode -> getNodeType() == LDOM_Node::ELEMENT_NODE) {
      LDOM_BasicElement& anElement = * (LDOM_BasicElement *) aNode;
//      if (anElement.GetTagName().equals(aTagName))
      if (strcmp (anElement.GetTagName(), aTagString) == 0)
        aList.Append (anElement);
      anElement.AddElementsByTagName (aList, aTagName);
    }
    aNode = aNode -> GetSibling();
  }
}

//=======================================================================
//function : AddAttributes
//purpose  : 
//=======================================================================

void LDOM_BasicElement::AddAttributes (LDOM_NodeList&       aList,
                                       const LDOM_BasicNode * aLastChild) const
{
  const LDOM_BasicNode * aBNode;
  if (aLastChild)
    aBNode = aLastChild -> GetSibling();
  else
    aBNode = GetFirstChild();
  while (aBNode) {
    if (aBNode -> getNodeType() == LDOM_Node::ATTRIBUTE_NODE)
      aList.Append (* aBNode);
    aBNode = aBNode -> GetSibling();
  }
}

//=======================================================================
//function : ReplaceElement
//purpose  : Copy data and children into this node from another one
//           The only preserved data is mySibling
//=======================================================================

void LDOM_BasicElement::ReplaceElement
                                (const LDOM_BasicElement&       anOtherElem,
                                 const Handle(LDOM_MemManager)& aDocument)
{
  myTagName          = anOtherElem.GetTagName();
  myAttributeMask    = anOtherElem.myAttributeMask;
  myFirstChild       = NULL;
  const LDOM_BasicNode * aBNode = anOtherElem.GetFirstChild ();
  const LDOM_BasicNode * aLastChild = NULL;

  // Loop on children (non-attributes)
  for (; aBNode != NULL; aBNode = aBNode -> GetSibling()) {
    if (aBNode -> isNull())
      continue;
    LDOM_BasicNode * aNewBNode;
    const LDOM_Node::NodeType aNewNodeType = aBNode -> getNodeType();
    switch (aNewNodeType) {
    case LDOM_Node::ELEMENT_NODE:
      {
        const LDOM_BasicElement& aBNodeElem = *(const LDOM_BasicElement*)aBNode;
        const char * aTagString = aBNodeElem.GetTagName();
        LDOM_BasicElement& aNewBNodeElem =
          LDOM_BasicElement::Create (aTagString, (Standard_Integer)strlen(aTagString), aDocument);
        aNewBNodeElem.ReplaceElement (aBNodeElem, aDocument); //reccur
        aNewBNode = &aNewBNodeElem;
        break;
      }
    case LDOM_Node::ATTRIBUTE_NODE:
      goto loop_attr;
    case LDOM_Node::TEXT_NODE:
    case LDOM_Node::COMMENT_NODE:
    case LDOM_Node::CDATA_SECTION_NODE:
      {
        const LDOM_BasicText& aBNodeText = * (const LDOM_BasicText *) aBNode;
        aNewBNode = &LDOM_BasicText::Create (aNewNodeType,
                                             LDOMString(aBNodeText.GetData(),
                                                        aDocument),
                                             aDocument);
        break;
      }
    default: continue;
    }
    if (GetFirstChild())
      (const LDOM_BasicNode *&) aLastChild -> mySibling = aNewBNode;
    else
      (const LDOM_BasicNode *&) myFirstChild = aNewBNode;
    (const LDOM_BasicNode *&) aLastChild = aNewBNode;
  }

  // Loop on attributes (in the end of the list of children)
loop_attr:
  LDOM_BasicNode * aLastAttr = (LDOM_BasicNode *) aLastChild;
  for (; aBNode != NULL; aBNode = aBNode -> GetSibling()) {
    Standard_Integer aHash;
    if (aBNode -> isNull()) continue;
    const LDOM_BasicAttribute * aBNodeAtt= (const LDOM_BasicAttribute *) aBNode;
    LDOM_BasicAttribute * aNewAtt =
      &LDOM_BasicAttribute::Create (aBNodeAtt -> GetName(), aDocument, aHash);
    aNewAtt -> SetValue (aBNodeAtt->myValue, aDocument);
    if (aLastAttr)
      aLastAttr -> SetSibling (aNewAtt);
    else
      myFirstChild = aNewAtt;
    aLastAttr = aNewAtt;
  }
}
