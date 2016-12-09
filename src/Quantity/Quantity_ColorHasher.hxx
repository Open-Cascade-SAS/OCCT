// Created on: 2016-12-13
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Quantity_ColorHasher_HeaderFile
#define _Quantity_ColorHasher_HeaderFile

#include <Quantity_Color.hxx>

//! Hasher of Quantity_Color.
struct Quantity_ColorHasher
{
  //! Returns hash code for the given color.
  static Standard_Integer HashCode (const Quantity_Color&  theColor,
                                    const Standard_Integer theUpper)
  {
    Standard_Integer aRed   = Standard_Integer (255 * theColor.Red());
    Standard_Integer aGreen = Standard_Integer (255 * theColor.Green());
    Standard_Integer aBlue  = Standard_Integer (255 * theColor.Blue());

    Standard_Integer aHash = 0;
    updateHash (aHash, aRed);
    updateHash (aHash, aGreen);
    updateHash (aHash, aBlue);
    aHash += (aHash << 3);
    aHash ^= (aHash >> 11);
    aHash += (aHash << 15);
    return ((aHash & 0x7fff) % theUpper) + 1;
  }

  //! Returns true if two colors are equal.
  static Standard_Boolean IsEqual (const Quantity_Color& theColor1,
                                   const Quantity_Color& theColor2)
  {
    return theColor1 == theColor2;
  }

protected:
  static void updateHash (Standard_Integer&      theHash,
                          const Standard_Integer theValue)
  {
    theHash += theValue;
    theHash += (theHash << 10);
    theHash ^= (theHash >> 6);
  }
};

#endif
