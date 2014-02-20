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

