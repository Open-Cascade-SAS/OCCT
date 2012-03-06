// File:      LDOM_BasicText.cxx
// Created:   26.07.01 19:18:50
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001
// History:


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
