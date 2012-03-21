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



#include <LDOM_Document.hxx>
#include <LDOM_MemManager.hxx>
#include <LDOM_BasicElement.hxx>
#include <LDOM_BasicText.hxx>

#define MEMORY_GRANULE 10000

//=======================================================================
//function : LDOM_Document()
//purpose  : Constructor
//=======================================================================

LDOM_Document::LDOM_Document ()
{
  myMemManager = new LDOM_MemManager (MEMORY_GRANULE);
}

//=======================================================================
//function : LDOM_Document
//purpose  : Constructor to be used in LDOM_MemManager::Doc()
//=======================================================================

LDOM_Document::LDOM_Document (const LDOM_MemManager& aMemManager)
{
  myMemManager = &aMemManager;
}

//=======================================================================
//function : ~LDOM_Document
//purpose  : Destructor
//=======================================================================

LDOM_Document::~LDOM_Document ()
{}

//=======================================================================
//function : isNull
//purpose  : 
//=======================================================================

Standard_Boolean LDOM_Document::isNull () const
{
  const LDOM_BasicElement * const aRootElement = myMemManager -> RootElement();
  if (aRootElement == NULL) return Standard_True;
  return aRootElement -> isNull();
}

//=======================================================================
//function : getDocumentElement
//purpose  : 
//=======================================================================

LDOM_Element LDOM_Document::getDocumentElement () const
{
  return LDOM_Element (* myMemManager -> RootElement(), myMemManager);
}

//=======================================================================
//function : getElementsByTagName
//purpose  : 
//=======================================================================

LDOM_NodeList LDOM_Document::getElementsByTagName
                                        (const LDOMString& theTagName) const
{
  LDOM_NodeList aList (myMemManager);
  LDOM_BasicElement * anElem = (LDOM_BasicElement *)myMemManager->RootElement();
  const char        * aTagString = theTagName.GetString();
  if (anElem) {
//  if (anElem -> GetTagName().equals(theTagName))
    if (strcmp (anElem -> GetTagName(), aTagString) == 0)
      aList.Append (* anElem);
    anElem -> AddElementsByTagName (aList, theTagName);
  }
  return aList;
}

//=======================================================================
//function : createDocument (static)
//purpose  : 
//=======================================================================

LDOM_Document LDOM_Document::createDocument (const LDOMString& theQualifiedName)
{
  LDOM_Document aDoc;
  const char * aString = theQualifiedName.GetString();
  if (strlen(aString) == 0)
    aString = "document";
  aDoc.myMemManager -> myRootElement =
    & LDOM_BasicElement::Create (aString, strlen(aString), aDoc.myMemManager);
  return aDoc;
}

//=======================================================================
//function : createElement
//purpose  : 
//=======================================================================

LDOM_Element LDOM_Document::createElement (const LDOMString& theTagName)
{
  const char * aTagString = theTagName.GetString();
  LDOM_BasicElement& aBasicElem = LDOM_BasicElement::Create (aTagString,
                                                             strlen(aTagString),
                                                             myMemManager);
  return LDOM_Element (aBasicElem, myMemManager);
}

//=======================================================================
//function : createTextNode
//purpose  : 
//=======================================================================

LDOM_Text LDOM_Document::createTextNode (const LDOMString& theData)
{
  LDOM_BasicText& aBasicText
    = LDOM_BasicText::Create (LDOM_Node::TEXT_NODE,
                              LDOMString (theData, myMemManager), myMemManager);
  return LDOM_Text (aBasicText, myMemManager);
}

//=======================================================================
//function : createCDATASection
//purpose  : 
//=======================================================================

LDOM_CDATASection LDOM_Document::createCDATASection (const LDOMString& theData)
{
  LDOM_BasicText& aBasicText =
    LDOM_BasicText::Create (LDOM_Node::CDATA_SECTION_NODE,
                            LDOMString (theData, myMemManager), myMemManager);
  const LDOM_CDATASection aNewNode (aBasicText, myMemManager);
  aNewNode.SetValueClear(); // no use to beware markup characters
  return aNewNode;
}

//=======================================================================
//function : createComment
//purpose  : 
//=======================================================================

LDOM_Comment LDOM_Document::createComment (const LDOMString& theData)
{
  LDOM_BasicText& aBasicText =
    LDOM_BasicText::Create (LDOM_Node::COMMENT_NODE,
                            LDOMString (theData, myMemManager), myMemManager);
  return LDOM_Comment (aBasicText, myMemManager);
}

//=======================================================================
//function : operator =
//purpose  : Nullify
//=======================================================================

LDOM_Document& LDOM_Document::operator = (const LDOM_NullPtr *)
{
  myMemManager = new LDOM_MemManager (MEMORY_GRANULE);
  return * this;
}

//=======================================================================
//function : operator ==
//purpose  : Compare to NULL
//=======================================================================

Standard_Boolean LDOM_Document::operator == (const LDOM_NullPtr *) const
{
  return myMemManager -> RootElement() == NULL;
}

//=======================================================================
//function : operator !=
//purpose  : Compare to NULL
//=======================================================================

Standard_Boolean LDOM_Document::operator != (const LDOM_NullPtr *) const
{
  return myMemManager -> RootElement() != NULL;
}
