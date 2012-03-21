// Created on: 2001-06-28
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


#include <LDOM_DeclareSequence.hxx>
#include <LDOM_NodeList.hxx>
#include <LDOM_BasicNode.hxx>

typedef const LDOM_BasicNode * LDOM_BasicNodePtr;

DECLARE_SEQUENCE   (LDOM_BasicNodeSequence, LDOM_BasicNodePtr)
IMPLEMENT_SEQUENCE (LDOM_BasicNodeSequence, LDOM_BasicNodePtr)

//=======================================================================
//function : LDOM_NodeList()
//purpose  : Constructor
//=======================================================================

LDOM_NodeList::LDOM_NodeList (  )
{
  mySeq = new LDOM_BasicNodeSequence;
}

//=======================================================================
//function : LDOM_NodeList
//purpose  : 
//=======================================================================

LDOM_NodeList::LDOM_NodeList (const Handle(LDOM_MemManager)& aDoc)
     : myDoc (aDoc)
{
  mySeq = new LDOM_BasicNodeSequence;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void LDOM_NodeList::Append (const LDOM_BasicNode& aNode) const
{
  mySeq -> Append (&aNode);
}

//=======================================================================
//function : LDOM_NodeList
//purpose  : Copy constructor
//=======================================================================

LDOM_NodeList::LDOM_NodeList (const LDOM_NodeList& theOther)
{
  mySeq = new LDOM_BasicNodeSequence;
  * mySeq = * theOther.mySeq;
  myDoc = theOther.myDoc;
}

//=======================================================================
//function : ~LDOM_NodeList
//purpose  : Destructor
//=======================================================================

LDOM_NodeList::~LDOM_NodeList ()
{
  delete mySeq;
}

//=======================================================================
//function : operator =
//purpose  : Assignment
//=======================================================================

LDOM_NodeList& LDOM_NodeList::operator = (const LDOM_NodeList& theOther)
{
  myDoc = theOther.myDoc;
  * mySeq = * theOther.mySeq;
  return * this;
}
  
//=======================================================================
//function : operator =
//purpose  : Nullify
//=======================================================================

LDOM_NodeList& LDOM_NodeList::operator = (const LDOM_NullPtr *)
{
  myDoc.Nullify();
  mySeq -> Clear ();
  return * this;
}
  
//=======================================================================
//function : operator ==
//purpose  : 
//=======================================================================

Standard_Boolean LDOM_NodeList::operator == (const LDOM_NullPtr *) const
{
  return myDoc.IsNull() || mySeq -> Length () == 0;
}

//=======================================================================
//function : operator !=
//purpose  : 
//=======================================================================

Standard_Boolean LDOM_NodeList::operator != (const LDOM_NullPtr *) const
{
  return ! (myDoc.IsNull() || mySeq -> Length () == 0);
}

//=======================================================================
//function : item
//purpose  : 
//=======================================================================

LDOM_Node LDOM_NodeList::item (const Standard_Integer anIndex) const
{
  if (myDoc.IsNull() || anIndex < 0 || anIndex >= mySeq -> Length ())
    return LDOM_Node();
  return LDOM_Node (* mySeq -> Value(anIndex+1), myDoc);
}

//=======================================================================
//function : getLength
//purpose  : 
//=======================================================================

Standard_Integer LDOM_NodeList::getLength () const
{
  return mySeq -> Length();
}

