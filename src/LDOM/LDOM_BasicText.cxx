// Created on: 2001-07-26
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



#include <LDOM_BasicText.hxx>
#include <LDOM_MemManager.hxx>

//=======================================================================
//function : LDOM_BasicText()
//purpose  : Constructor
//=======================================================================

LDOM_BasicText::LDOM_BasicText (const LDOM_CharacterData& aText)
     : LDOM_BasicNode   (aText.Origin()),
       myValue          (aText.getData()) {}

//=======================================================================
//function : Create
//purpose  : construction in the Document's data pool
//=======================================================================

LDOM_BasicText& LDOM_BasicText::Create (const LDOM_Node::NodeType       aType,
                                        const LDOMBasicString&          aData,
                                        const Handle(LDOM_MemManager)&  aDoc)
{
  void * aMem = aDoc -> Allocate (sizeof(LDOM_BasicText));
  LDOM_BasicText * aNewText = new (aMem) LDOM_BasicText (aType, aData);
  return * aNewText;
}

//=======================================================================
//function : operator =
//purpose  : Assignment to NULL
//=======================================================================

LDOM_BasicText& LDOM_BasicText::operator= (const LDOM_NullPtr * aNull)
{
  myValue = aNull;
  LDOM_BasicNode::operator= (aNull);
  return * this;
}
