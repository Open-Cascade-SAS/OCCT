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

#ifndef LDOM_Attr_HeaderFile
#define LDOM_Attr_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicAttribute;

//  Class LDOM_Attr
//

class LDOM_Attr : public LDOM_Node
{
public:
  // ---------- PUBLIC METHODS ----------

  LDOM_Attr() {}

  //    Empty constructor

  LDOM_Attr(const LDOM_Attr& anOther)
      : LDOM_Node(anOther)
  {
  }

  //    Copy constructor

  LDOM_Attr& operator=(const LDOM_NullPtr* aNull)
  {
    return (LDOM_Attr&)LDOM_Node::operator=(aNull);
  }

  //    Nullify

  LDOM_Attr& operator=(const LDOM_Attr& anOther)
  {
    return (LDOM_Attr&)LDOM_Node::operator=(anOther);
  }

  //    Assignment

  LDOMString getName() const { return getNodeName(); }

  LDOMString getValue() const { return getNodeValue(); }

  Standard_EXPORT void setValue(const LDOMString& aValue);

protected:
  friend class LDOM_Element;
  // ---------- PROTECTED METHODS ----------

  LDOM_Attr(const LDOM_BasicAttribute& anAttr, const Handle(LDOM_MemManager)& aDoc);

private:
  // ---------- PRIVATE FIELDS ----------
};

#endif
