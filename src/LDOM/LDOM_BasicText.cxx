// Created on: 2001-07-26
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

#include <LDOM_BasicText.hxx>
#include <LDOM_MemManager.hxx>
#include <LDOM_CharacterData.hxx>

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
