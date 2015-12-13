// Created on: 2001-09-12
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

#include <LDOM_CharacterData.hxx>
#include <LDOM_BasicText.hxx>
#include <Standard_ProgramError.hxx>

//=======================================================================
//function : LDOM_CharacterData
//purpose  : 
//=======================================================================

LDOM_CharacterData::LDOM_CharacterData (const LDOM_BasicText&          aText,
                                        const Handle(LDOM_MemManager)& aDoc)
     : LDOM_Node (aText, aDoc), myLength (-1) {}

//=======================================================================
//function : operator =
//purpose  : Nullify
//=======================================================================

LDOM_CharacterData& LDOM_CharacterData::operator = (const LDOM_NullPtr* theNull)
{
  LDOM_Node::operator = (theNull);
  myLength = -1;
  return * this;
}

//=======================================================================
//function : operator =
//purpose  : Assignment
//=======================================================================

LDOM_CharacterData& LDOM_CharacterData::operator =
                                        (const LDOM_CharacterData& theOther)
{
  LDOM_Node::operator = (theOther);
  myLength = theOther.myLength;
  return * this;
}

//=======================================================================
//function : setData
//purpose  : replace the data
//=======================================================================

void LDOM_CharacterData::setData (const LDOMString& theValue)
{
  LDOM_BasicText& aText = (LDOM_BasicText&) Origin ();
  aText.SetData (theValue, myDocument);
  myLength = -1;
}

//=======================================================================
//function : getLength
//purpose  : query the data length
//=======================================================================

Standard_Integer LDOM_CharacterData::getLength () const
{
  if (myLength < 0)
    (Standard_Integer&)myLength = (Standard_Integer)strlen (getNodeValue().GetString());
  return myLength;
}
