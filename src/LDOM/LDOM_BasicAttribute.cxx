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

#include <LDOM_BasicAttribute.hxx>
#include <LDOM_MemManager.hxx>
#include <LDOM_Attr.hxx>

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
    theDoc -> HashedAllocate (aString, (Standard_Integer)strlen(aString), theHash);

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
