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


#include <LDOM_BasicAttribute.hxx>
#include <LDOM_MemManager.hxx>

//=======================================================================
//function : LDOM_BasicAttribute
//purpose  : 
//=======================================================================

LDOM_BasicAttribute::LDOM_BasicAttribute (const LDOM_Attr& anAttr)
     : LDOM_BasicNode   (anAttr.Origin()),
       myName           (anAttr.getName().GetString()),
       myValue          (anAttr.getValue()) {}

//=======================================================================
//function : Create
//purpose  : construction in the Document's data pool
//=======================================================================

LDOM_BasicAttribute& LDOM_BasicAttribute::Create
                                        (const LDOMBasicString&         theName,
                                         const Handle(LDOM_MemManager)& theDoc,
                                         Standard_Integer&              theHash)
{
  void * aMem = theDoc -> Allocate (sizeof(LDOM_BasicAttribute));
  LDOM_BasicAttribute * aNewAtt = new (aMem) LDOM_BasicAttribute;

  const char * aString = theName.GetString();
  aNewAtt -> myName =
    theDoc -> HashedAllocate (aString, strlen(aString), theHash);

  aNewAtt -> myNodeType = LDOM_Node::ATTRIBUTE_NODE;
  return * aNewAtt;
}

//=======================================================================
//function : operator =
//purpose  : Assignment to NULL
//=======================================================================

LDOM_BasicAttribute& LDOM_BasicAttribute::operator= (const LDOM_NullPtr * aNull)
{
  myName = NULL;
  myValue = aNull;
  LDOM_BasicNode::operator= (aNull);
  return * this;
}
