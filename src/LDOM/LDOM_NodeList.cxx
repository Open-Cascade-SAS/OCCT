// Created on: 2001-06-28
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

