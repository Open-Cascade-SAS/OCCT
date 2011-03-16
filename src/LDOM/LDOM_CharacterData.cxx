// File:      LDOM_CharacterData.cxx
// Created:   12.09.01 15:05:37
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2001
// History:

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
  if (&aText == NULL)
    Standard_ProgramError::Raise("LDOM_CharacterData::setData: called on void");
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
    (Standard_Integer&)myLength = strlen (getNodeValue().GetString());
  return myLength;
}
