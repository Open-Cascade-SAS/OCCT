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

#include <LDOM_CharReference.hxx>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//   Uncomment this line if you want that your XML files contain codes 0xc0-0xff
//   as defined in Latin-1 code set. Otherwise these codes are written
//   numerically as &#x..;
//#define LDOM_ALLOW_LATIN_1

const int NORMAL_C  = 0;
const int CHAR_REF  = -1;
const int ENTI_AMP  = 1;
const int ENTI_LT   = 2;
const int ENTI_GT   = 3;
const int ENTI_QUOT = 4;
//const int ENTI_APOS = 5;

struct entityRef {
  const char * name;
  const int    length;
  entityRef (const char * aName, const int aLen) : name(aName), length(aLen) {}
  void operator= (const entityRef&);
};

//=======================================================================
//function : Decode
//purpose  : Convertes entity and character references on input
//           Always returns the same string (shortened after replacements)
//=======================================================================

char * LDOM_CharReference::Decode (char * theSrc, Standard_Integer& theLen)
{
#define IS_EQUAL(_ptr,_string) (!memcmp(_ptr, _string, sizeof(_string)-1))

  char * aSrcPtr = theSrc, * aDstPtr = theSrc;
  Standard_Integer anIncrCount = 0;
  for(;;) {
    char * aPtr = strchr (aSrcPtr, '&');
    if (aPtr == NULL) {
      //        End of the loop
      aPtr = strchr (aSrcPtr, '\0');
      if (anIncrCount == 0)
        theLen = (Standard_Integer)(aPtr - theSrc);
      else {
        Standard_Integer aByteCount = (Standard_Integer)(aPtr - aSrcPtr);
        memmove (aDstPtr, aSrcPtr, aByteCount + 1);
        theLen = (Standard_Integer)(aDstPtr - theSrc) + aByteCount;
      }
      break;
    }
    Standard_Integer aByteCount = (Standard_Integer)(aPtr - aSrcPtr);
    if (aByteCount > 0 && aDstPtr != aSrcPtr)
      memmove (aDstPtr, aSrcPtr, aByteCount);
    aSrcPtr = aPtr;
    if (aSrcPtr[1] == '#') {
      unsigned long aChar;
      char *        aNewPtr;
      aDstPtr = aSrcPtr - anIncrCount + 1;
      if (aSrcPtr[2] == 'x')
        aChar = strtoul (&aSrcPtr[3], &aNewPtr, 16);         // hex encoding
      else
        aChar = strtoul (&aSrcPtr[2], &aNewPtr, 10);         // decimal encoding
      if (aNewPtr[0] != ';' || aChar == 0 || aChar > 255UL)
        //      Error reading an XML string
        return NULL;
      aDstPtr[-1] = (char) aChar;
      anIncrCount += (Standard_Integer)(aNewPtr - aSrcPtr);
      aSrcPtr = &aNewPtr[1];
    }
    else if (IS_EQUAL(aSrcPtr+1, "amp;")) {
      aDstPtr = aSrcPtr - anIncrCount + 1;
      aDstPtr[-1] = '&';
      anIncrCount += 4;
      aSrcPtr += 5;
    }
    else if (IS_EQUAL(aSrcPtr+1, "lt;")) {
      aDstPtr = aSrcPtr - anIncrCount + 1;
      aDstPtr[-1] = '<';
      anIncrCount += 3;
      aSrcPtr += 4;
    }
    else if (IS_EQUAL(aSrcPtr+1, "gt;")) {
      aDstPtr = aSrcPtr - anIncrCount + 1;
      aDstPtr[-1] = '>';
      anIncrCount += 3;
      aSrcPtr += 4;
    }
    else if (IS_EQUAL(aSrcPtr+1, "quot;")) {
      aDstPtr = aSrcPtr - anIncrCount + 1;
      aDstPtr[-1] = '\"';
      anIncrCount += 5;
      aSrcPtr += 6;
    }
    else if (IS_EQUAL(aSrcPtr+1, "apos;")) {
      aDstPtr = aSrcPtr - anIncrCount + 1;
      aDstPtr[-1] = '\'';
      anIncrCount += 5;
      aSrcPtr += 6;
    }
    else {
      aDstPtr = aSrcPtr - anIncrCount;
      * aDstPtr++ = * aSrcPtr++;
      continue;
    }
  }
  return theSrc;
}

//=======================================================================
//function : Encode
//purpose  : This method takes the input string theSrc and returns:
//              - the pointer equal to theSrc if there are no replacements, or
//              - the pointer to a newly allocated string with replacements
//           The output parameter theLen is assigned to the length of
//           the returned string (whatever the case)
//=======================================================================

char * LDOM_CharReference::Encode (const char* theSrc, Standard_Integer& theLen,
                                   const Standard_Boolean isAttribute)
{
  // Initialising the constants
  static const struct entityRef entity_ref[6] = {
    entityRef(NULL,     0),
    entityRef("&amp;",  5),
    entityRef("&lt;",   4),
    entityRef("&gt;",   4),
    entityRef("&quot;", 6),
    entityRef("&apos;", 6)
  };

  const char * endSrc, * ptrSrc = theSrc;
  char       * aDest = (char *) theSrc;
  Standard_Integer aCount = 0;
  //    Analyse if there is a non-standard character in the string
  for(;;) {
    const unsigned int iSrc =
      (const unsigned int) * (const unsigned char *) ptrSrc;
    if (iSrc == 0) {
      endSrc = ptrSrc;
      break;
    }
    if (myTab[iSrc] != NORMAL_C)
      if (isAttribute || myTab[iSrc] != ENTI_QUOT)
        aCount++;
    ptrSrc++;
  }
  //    If there are such, copy the string with replacements
  if (!aCount)
    theLen = (Standard_Integer)(endSrc - theSrc);
  else {
    char * ptrDest = new char [(endSrc - theSrc) + aCount * 5 + 1];
    aDest = ptrDest;
    for (ptrSrc = theSrc; ptrSrc < endSrc; ptrSrc++) {
      const unsigned int iSrc =
        (const unsigned int) * (const unsigned char *) ptrSrc;
      const int aCode = myTab[iSrc];
      if (aCode == NORMAL_C)                    // normal (regular) character
        * ptrDest++ = * ptrSrc;
      else if (aCode == CHAR_REF) {             // character reference
        sprintf (ptrDest, "&#x%02x;", iSrc);
        ptrDest += 6;
      } else                                    // predefined entity reference
        if (isAttribute == Standard_False && aCode == ENTI_QUOT)
          * ptrDest++ = * ptrSrc;
        else {
          memcpy (ptrDest, entity_ref[aCode].name, entity_ref[aCode].length+1);
          ptrDest += entity_ref[aCode].length;
        }
    }
    theLen = (Standard_Integer)(ptrDest - aDest);
    * ptrDest = '\0';
  }
  return aDest;
}

int LDOM_CharReference::myTab [256] = {
  NORMAL_C,     // 000
  CHAR_REF,     // 001
  CHAR_REF,     // 002
  CHAR_REF,     // 003
  CHAR_REF,     // 004
  CHAR_REF,     // 005
  CHAR_REF,     // 006
  CHAR_REF,     // 007
  CHAR_REF,     // 008
  NORMAL_C,     // 009  TAB
  NORMAL_C,     // 00a  LF
  CHAR_REF,     // 00b
  CHAR_REF,     // 00c
  NORMAL_C,     // 00d  CR
  CHAR_REF,     // 00e
  CHAR_REF,     // 00f
  CHAR_REF,     // 010
  CHAR_REF,     // 011
  CHAR_REF,     // 012
  CHAR_REF,     // 013
  CHAR_REF,     // 014
  CHAR_REF,     // 015
  CHAR_REF,     // 016
  CHAR_REF,     // 017
  CHAR_REF,     // 018
  CHAR_REF,     // 019
  CHAR_REF,     // 01a
  CHAR_REF,     // 01b
  CHAR_REF,     // 01c
  CHAR_REF,     // 01d
  CHAR_REF,     // 01e
  CHAR_REF,     // 01f
  NORMAL_C,     // 020:  
  NORMAL_C,     // 021: !
  ENTI_QUOT,    // 022: "
  NORMAL_C,     // 023: #
  NORMAL_C,     // 024: $
  NORMAL_C,     // 025: %
  ENTI_AMP,     // 026: &
//  ENTI_APOS,    // 027: '   Here we do never use apostrophe as delimiter
  NORMAL_C,     // 027: '
  NORMAL_C,     // 028: (
  NORMAL_C,     // 029: )
  NORMAL_C,     // 02a: *
  NORMAL_C,     // 02b: +
  NORMAL_C,     // 02c: ,
  NORMAL_C,     // 02d: -
  NORMAL_C,     // 02e: .
  NORMAL_C,     // 02f: /
  NORMAL_C,     // 030: 0
  NORMAL_C,     // 031: 1
  NORMAL_C,     // 032: 2
  NORMAL_C,     // 033: 3
  NORMAL_C,     // 034: 4
  NORMAL_C,     // 035: 5
  NORMAL_C,     // 036: 6
  NORMAL_C,     // 037: 7
  NORMAL_C,     // 038: 8
  NORMAL_C,     // 039: 9
  NORMAL_C,     // 03a: :
  NORMAL_C,     // 03b: ;
  ENTI_LT,      // 03c: <
  NORMAL_C,     // 03d: =
  ENTI_GT,      // 03e: >
  NORMAL_C,     // 03f: ?
  NORMAL_C,     // 040: @
  NORMAL_C,     // 041: A
  NORMAL_C,     // 042: B
  NORMAL_C,     // 043: C
  NORMAL_C,     // 044: D
  NORMAL_C,     // 045: E
  NORMAL_C,     // 046: F
  NORMAL_C,     // 047: G
  NORMAL_C,     // 048: H
  NORMAL_C,     // 049: I
  NORMAL_C,     // 04a: J
  NORMAL_C,     // 04b: K
  NORMAL_C,     // 04c: L
  NORMAL_C,     // 04d: M
  NORMAL_C,     // 04e: N
  NORMAL_C,     // 04f: O
  NORMAL_C,     // 050: P
  NORMAL_C,     // 051: Q
  NORMAL_C,     // 052: R
  NORMAL_C,     // 053: S
  NORMAL_C,     // 054: T
  NORMAL_C,     // 055: U
  NORMAL_C,     // 056: V
  NORMAL_C,     // 057: W
  NORMAL_C,     // 058: X
  NORMAL_C,     // 059: Y
  NORMAL_C,     // 05a: Z
  NORMAL_C,     // 05b: [
  NORMAL_C,     /* 05c: \	*/
  NORMAL_C,     // 05d: ]
  NORMAL_C,     // 05e: ^
  NORMAL_C,     // 05f: _
  NORMAL_C,     // 060: `
  NORMAL_C,     // 061: a
  NORMAL_C,     // 062: b
  NORMAL_C,     // 063: c
  NORMAL_C,     // 064: d
  NORMAL_C,     // 065: e
  NORMAL_C,     // 066: f
  NORMAL_C,     // 067: g
  NORMAL_C,     // 068: h
  NORMAL_C,     // 069: i
  NORMAL_C,     // 06a: j
  NORMAL_C,     // 06b: k
  NORMAL_C,     // 06c: l
  NORMAL_C,     // 06d: m
  NORMAL_C,     // 06e: n
  NORMAL_C,     // 06f: o
  NORMAL_C,     // 070: p
  NORMAL_C,     // 071: q
  NORMAL_C,     // 072: r
  NORMAL_C,     // 073: s
  NORMAL_C,     // 074: t
  NORMAL_C,     // 075: u
  NORMAL_C,     // 076: v
  NORMAL_C,     // 077: w
  NORMAL_C,     // 078: x
  NORMAL_C,     // 079: y
  NORMAL_C,     // 07a: z
  NORMAL_C,     // 07b: {
  NORMAL_C,     // 07c: |
  NORMAL_C,     // 07d: }
  NORMAL_C,     // 07e: ~
  NORMAL_C,     // 07f: 
  CHAR_REF,     // 080
  CHAR_REF,     // 081
  CHAR_REF,     // 082
  CHAR_REF,     // 083
  CHAR_REF,     // 084
  CHAR_REF,     // 085
  CHAR_REF,     // 086
  CHAR_REF,     // 087
  CHAR_REF,     // 088
  CHAR_REF,     // 089
  CHAR_REF,     // 08a
  CHAR_REF,     // 08b
  CHAR_REF,     // 08c
  CHAR_REF,     // 08d
  CHAR_REF,     // 08e
  CHAR_REF,     // 08f
  CHAR_REF,     // 090
  CHAR_REF,     // 091
  CHAR_REF,     // 092
  CHAR_REF,     // 093
  CHAR_REF,     // 094
  CHAR_REF,     // 095
  CHAR_REF,     // 096
  CHAR_REF,     // 097
  CHAR_REF,     // 098
  CHAR_REF,     // 099
  CHAR_REF,     // 09a
  CHAR_REF,     // 09b
  CHAR_REF,     // 09c
  CHAR_REF,     // 09d
  CHAR_REF,     // 09e
  CHAR_REF,     // 09f
  CHAR_REF,     // 0a0
  CHAR_REF,     // 0a1
  CHAR_REF,     // 0a2
  CHAR_REF,     // 0a3
  CHAR_REF,     // 0a4
  CHAR_REF,     // 0a5
  CHAR_REF,     // 0a6
  CHAR_REF,     // 0a7
  CHAR_REF,     // 0a8
  CHAR_REF,     // 0a9
  CHAR_REF,     // 0aa
  CHAR_REF,     // 0ab
  CHAR_REF,     // 0ac
  CHAR_REF,     // 0ad
  CHAR_REF,     // 0ae
  CHAR_REF,     // 0af
  CHAR_REF,     // 0b0
  CHAR_REF,     // 0b1
  CHAR_REF,     // 0b2
  CHAR_REF,     // 0b3
  CHAR_REF,     // 0b4
  CHAR_REF,     // 0b5
  CHAR_REF,     // 0b6
  CHAR_REF,     // 0b7
  CHAR_REF,     // 0b8
  CHAR_REF,     // 0b9
  CHAR_REF,     // 0ba
  CHAR_REF,     // 0bb
  CHAR_REF,     // 0bc
  CHAR_REF,     // 0bd
  CHAR_REF,     // 0be
  CHAR_REF,     // 0bf
#ifdef LDOM_ALLOW_LATIN_1
  NORMAL_C,     // 0c0
  NORMAL_C,     // 0c1
  NORMAL_C,     // 0c2
  NORMAL_C,     // 0c3
  NORMAL_C,     // 0c4
  NORMAL_C,     // 0c5
  NORMAL_C,     // 0c6
  NORMAL_C,     // 0c7
  NORMAL_C,     // 0c8
  NORMAL_C,     // 0c9
  NORMAL_C,     // 0ca
  NORMAL_C,     // 0cb
  NORMAL_C,     // 0cc
  NORMAL_C,     // 0cd
  NORMAL_C,     // 0ce
  NORMAL_C,     // 0cf
  NORMAL_C,     // 0d0
  NORMAL_C,     // 0d1
  NORMAL_C,     // 0d2
  NORMAL_C,     // 0d3
  NORMAL_C,     // 0d4
  NORMAL_C,     // 0d5
  NORMAL_C,     // 0d6
//  CHAR_REF,     // 0d7
  NORMAL_C,     // 0d7
  NORMAL_C,     // 0d8
  NORMAL_C,     // 0d9
  NORMAL_C,     // 0da
  NORMAL_C,     // 0db
  NORMAL_C,     // 0dc
  NORMAL_C,     // 0dd
  NORMAL_C,     // 0de
  NORMAL_C,     // 0df
  NORMAL_C,     // 0e0
  NORMAL_C,     // 0e1
  NORMAL_C,     // 0e2
  NORMAL_C,     // 0e3
  NORMAL_C,     // 0e4
  NORMAL_C,     // 0e5
  NORMAL_C,     // 0e6
  NORMAL_C,     // 0e7
  NORMAL_C,     // 0e8
  NORMAL_C,     // 0e9
  NORMAL_C,     // 0ea
  NORMAL_C,     // 0eb
  NORMAL_C,     // 0ec
  NORMAL_C,     // 0ed
  NORMAL_C,     // 0ee
  NORMAL_C,     // 0ef
  NORMAL_C,     // 0f0
  NORMAL_C,     // 0f1
  NORMAL_C,     // 0f2
  NORMAL_C,     // 0f3
  NORMAL_C,     // 0f4
  NORMAL_C,     // 0f5
  NORMAL_C,     // 0f6
//  CHAR_REF,     // 0f7
  NORMAL_C,     // 0f7
  NORMAL_C,     // 0f8
  NORMAL_C,     // 0f9
  NORMAL_C,     // 0fa
  NORMAL_C,     // 0fb
  NORMAL_C,     // 0fc
  NORMAL_C,     // 0fd
  NORMAL_C,     // 0fe
  NORMAL_C      // 0ff
#else
  CHAR_REF,     // 0c0
  CHAR_REF,     // 0c1
  CHAR_REF,     // 0c2
  CHAR_REF,     // 0c3
  CHAR_REF,     // 0c4
  CHAR_REF,     // 0c5
  CHAR_REF,     // 0c6
  CHAR_REF,     // 0c7
  CHAR_REF,     // 0c8
  CHAR_REF,     // 0c9
  CHAR_REF,     // 0ca
  CHAR_REF,     // 0cb
  CHAR_REF,     // 0cc
  CHAR_REF,     // 0cd
  CHAR_REF,     // 0ce
  CHAR_REF,     // 0cf
  CHAR_REF,     // 0d0
  CHAR_REF,     // 0d1
  CHAR_REF,     // 0d2
  CHAR_REF,     // 0d3
  CHAR_REF,     // 0d4
  CHAR_REF,     // 0d5
  CHAR_REF,     // 0d6
  CHAR_REF,     // 0d7
  CHAR_REF,     // 0d8
  CHAR_REF,     // 0d9
  CHAR_REF,     // 0da
  CHAR_REF,     // 0db
  CHAR_REF,     // 0dc
  CHAR_REF,     // 0dd
  CHAR_REF,     // 0de
  CHAR_REF,     // 0df
  CHAR_REF,     // 0e0
  CHAR_REF,     // 0e1
  CHAR_REF,     // 0e2
  CHAR_REF,     // 0e3
  CHAR_REF,     // 0e4
  CHAR_REF,     // 0e5
  CHAR_REF,     // 0e6
  CHAR_REF,     // 0e7
  CHAR_REF,     // 0e8
  CHAR_REF,     // 0e9
  CHAR_REF,     // 0ea
  CHAR_REF,     // 0eb
  CHAR_REF,     // 0ec
  CHAR_REF,     // 0ed
  CHAR_REF,     // 0ee
  CHAR_REF,     // 0ef
  CHAR_REF,     // 0f0
  CHAR_REF,     // 0f1
  CHAR_REF,     // 0f2
  CHAR_REF,     // 0f3
  CHAR_REF,     // 0f4
  CHAR_REF,     // 0f5
  CHAR_REF,     // 0f6
  CHAR_REF,     // 0f7
  CHAR_REF,     // 0f8
  CHAR_REF,     // 0f9
  CHAR_REF,     // 0fa
  CHAR_REF,     // 0fb
  CHAR_REF,     // 0fc
  CHAR_REF,     // 0fd
  CHAR_REF,     // 0fe
  CHAR_REF      // 0ff
#endif  // LDOM_ALLOW_LATIN_1
};
