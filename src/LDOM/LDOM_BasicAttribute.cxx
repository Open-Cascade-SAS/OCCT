// File:      LDOM_BasicAttribute.cxx
// Created:   26.06.01 18:48:27
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001
// History:

#include <LDOM_BasicAttribute.hxx>
#include <LDOM_MemManager.hxx>

#ifdef WNT
// Disable the warning: "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

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
