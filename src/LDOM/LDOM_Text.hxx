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

#ifndef LDOM_Text_HeaderFile
#define LDOM_Text_HeaderFile

#include <LDOM_CharacterData.hxx>

//  Class LDOM_Text

class LDOM_Text : public LDOM_CharacterData
{
public:
  // ---------- PUBLIC METHODS ----------

  LDOM_Text() {}

  // Empty constructor

  LDOM_Text(const LDOM_Text& anOther)
      : LDOM_CharacterData(anOther)
  {
  }

  // Copy constructor

  LDOM_Text& operator=(const LDOM_NullPtr* theNull)
  {
    return (LDOM_Text&)LDOM_CharacterData::operator=(theNull);
  }

  // Nullify

  LDOM_Text& operator=(const LDOM_Text& theOther)
  {
    return (LDOM_Text&)LDOM_CharacterData::operator=(theOther);
  }

  // Assignment

protected:
  friend class LDOM_Document;

  // ---------- PROTECTED METHODS ----------

  LDOM_Text(const LDOM_BasicText& theText, const Handle(LDOM_MemManager)& theDoc)
      : LDOM_CharacterData(theText, theDoc)
  {
  }
};

#endif
