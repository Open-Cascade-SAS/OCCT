// Created on: 2001-09-12
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
