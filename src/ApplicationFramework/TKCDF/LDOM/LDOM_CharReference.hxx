// Created on: 2002-02-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef LDOM_CharReference_HeaderFile
#define LDOM_CharReference_HeaderFile

#include <Standard_TypeDef.hxx>

//  Class LDOM_CharReference: treatment of character reference and internal
//     entities in input and output streams
//  On output all 256 characters are classified as this:
//     For string values of attributes:
//         0x09,0x0a,0x0d,0x20,0x21,0x23-0x25,0x27-0x3b,0x3d,0x3f-0x7f,0xc0-0xff
//              are treated as normal characters (no relacement)
//         0x22(&quote;), 0x26(&amp;), 0x3c(&lt;), 0x3e(&gt;)
//              are replaced by predefined entity reference.
//         0x01-0x08,0x0b,0x0c,0x0e-0x1f,0x80-0xbf
//              are replaced by character references
//     For other strings (text):
//         0x09,0x0a,0x0d,0x20-0x25,0x27-0x3b,0x3d,0x3f-0x7f,0xc0-0xff
//              are treated as normal characters (no relacement)
//         0x26(&amp;), 0x3c(&lt;), 0x3e(&gt;)
//              are replaced by predefined entity reference.
//         0x01-0x08,0x0b,0x0c,0x0e-0x1f,0x80-0xbf
//              are replaced by character references
//  For CDATA, element tag names and attribute names no replacements are made
//  Note that apostrophe (\') is not treated as markup on output (all relevant
//  markup is produced by quote characters (\")).

class LDOM_CharReference
{
public:
  // ---------- PUBLIC METHODS ----------

  static char* Decode(char* theSrc, Standard_Integer& theLen);

  static char* Encode(const char*            theSrc,
                      Standard_Integer&      theLen,
                      const Standard_Boolean isAttribute);
  // Encodes the string theSrc containing any byte characters 0x00-0xFF
  // Returns the encoded string. If (return value) != theSrc the returned
  // string should be deleted in caller routine (via delete[]).
  // The output parameter theLen gives the length of the encoded string
  // With isAttribute==True additionally encodes to $quot; for attr values

private:
  // ---------- PRIVATE FIELDS ----------

  static int myTab[256];
};

#endif
