// Created on: 2002-02-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

  static char * Decode (char * theSrc, Standard_Integer& theLen);

  static char * Encode (const char * theSrc, Standard_Integer& theLen,
                        const Standard_Boolean isAttribute);
  // Encodes the string theSrc containing any byte characters 0x00-0xFF
  // Returns the encoded string. If (return value) != theSrc the returned
  // string should be deleted in caller routine (via delete[]).
  // The output parameter theLen gives the length of the encoded string
  // With isAttribute==True additionally encodes to $quot; for attr values

 private:
  // ---------- PRIVATE FIELDS ----------

  static int myTab [256];
};

#endif
