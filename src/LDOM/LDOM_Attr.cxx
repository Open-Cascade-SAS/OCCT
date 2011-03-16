// File:      LDOM_Attr.cxx
// Created:   26.06.01 19:01:10
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001
// History:


#include <LDOM_Attr.hxx>
#include <LDOM_BasicAttribute.hxx>

//=======================================================================
//function : LDOM_Attr
//purpose  : Constructor
//=======================================================================

LDOM_Attr::LDOM_Attr (const LDOM_BasicAttribute& anAttr,
                      const Handle(LDOM_MemManager)& aDoc)
     : LDOM_Node (anAttr, aDoc) {}

//=======================================================================
//function : setValue
//purpose  : 
//=======================================================================

void LDOM_Attr::setValue (const LDOMString& aValue)
{
  LDOM_BasicAttribute& anAttr = (LDOM_BasicAttribute&) Origin ();
  anAttr.SetValue (aValue, myDocument);
}

