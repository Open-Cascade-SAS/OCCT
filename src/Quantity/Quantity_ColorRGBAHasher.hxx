// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Quantity_ColorRGBAHasher_HeaderFile
#define _Quantity_ColorRGBAHasher_HeaderFile

#include <Quantity_ColorRGBA.hxx>
#include <Quantity_ColorHasher.hxx>

//! Hasher of Quantity_ColorRGBA.
struct Quantity_ColorRGBAHasher
{

  //! Returns hash code for the given RGBA color, in the range [1, theUpperBound]
  //! @param theColor the RGBA color object which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode (const Quantity_ColorRGBA& theColor,
                                    const Standard_Integer    theUpperBound)
  {
    const NCollection_Vec4<float>& aColor = theColor;
    uint32_t aColor32 = (uint32_t(aColor.a() * 255.0f) << 24)
                      + (uint32_t(aColor.b() * 255.0f) << 16)
                      + (uint32_t(aColor.g() * 255.0f) << 8)
                      +  uint32_t(aColor.r() * 255.0f);
    return ::HashCode(aColor32, theUpperBound);
  }

  //! Returns true if two colors are equal.
  static Standard_Boolean IsEqual (const Quantity_ColorRGBA& theColor1,
                                   const Quantity_ColorRGBA& theColor2)
  {
    return theColor1 == theColor2;
  }

};

#endif // _Quantity_ColorRGBAHasher_HeaderFile
