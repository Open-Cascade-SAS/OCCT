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

#ifndef LDOM_CharacterData_HeaderFile
#define LDOM_CharacterData_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicText;

//  Class LDOM_CharacterData
//

class LDOM_CharacterData : public LDOM_Node
{
public:
  // ---------- PUBLIC METHODS ----------

  LDOM_CharacterData()
      : myLength(-1)
  {
  }

  // Empty constructor

  LDOM_CharacterData(const LDOM_CharacterData& theOther)
      : LDOM_Node(theOther),
        myLength(-1)
  {
  }

  // Copy constructor

  Standard_EXPORT LDOM_CharacterData& operator=(const LDOM_NullPtr* aNull);
  // Nullify

  Standard_EXPORT LDOM_CharacterData& operator=(const LDOM_CharacterData& anOther);

  // Assignment

  LDOMString getData() const { return getNodeValue(); }

  // Query data

  Standard_EXPORT void setData(const LDOMString& aValue);
  // Assign to data

  Standard_EXPORT Standard_Integer getLength() const;
  // Length of the string

protected:
  // ---------- PROTECTED METHODS ----------

  LDOM_CharacterData(const LDOM_BasicText& aText, const Handle(LDOM_MemManager)& aDoc);

private:
  // ------------ PRIVATE FIELDS -----------
  Standard_Integer myLength;
};

#endif
