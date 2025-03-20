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

#ifndef LDOM_CDATASection_HeaderFile
#define LDOM_CDATASection_HeaderFile

#include <LDOM_Text.hxx>

//  Class LDOM_CDATASection

class LDOM_CDATASection : public LDOM_Text
{
public:
  // ---------- PUBLIC METHODS ----------

  LDOM_CDATASection() {}

  // Empty constructor

  LDOM_CDATASection(const LDOM_CDATASection& theOther)
      : LDOM_Text(theOther)
  {
  }

  // Copy constructor

  LDOM_CDATASection& operator=(const LDOM_NullPtr* theNull)
  {
    return (LDOM_CDATASection&)LDOM_CharacterData::operator=(theNull);
  }

  // Nullify

  LDOM_CDATASection& operator=(const LDOM_CDATASection& theOther)
  {
    return (LDOM_CDATASection&)LDOM_CharacterData::operator=(theOther);
  }

  // Assignment

protected:
  friend class LDOM_Document;

  LDOM_CDATASection(const LDOM_BasicText& theText, const Handle(LDOM_MemManager)& theDoc)
      : LDOM_Text(theText, theDoc)
  {
  }
};

#endif
