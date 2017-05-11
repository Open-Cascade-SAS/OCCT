// Created on: 2000-08-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_Style_HeaderFile
#define _XCAFPrs_Style_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Quantity_ColorRGBAHasher.hxx>

//! Represents a set of styling settings applicable to a (sub)shape
class XCAFPrs_Style 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor - colors are unset, visibility is TRUE.
  Standard_EXPORT XCAFPrs_Style();

  //! Return TRUE if surface color has been defined.
  Standard_Boolean IsSetColorSurf() const { return myHasColorSurf; }

  //! Return surface color.
  const Quantity_Color& GetColorSurf() const { return myColorSurf.GetRGB(); }

  //! Set surface color.
  void SetColorSurf (const Quantity_Color& theColor) { SetColorSurf  (Quantity_ColorRGBA (theColor)); }

  //! Return surface color.
  const Quantity_ColorRGBA& GetColorSurfRGBA() const { return myColorSurf; }

  //! Set surface color.
  Standard_EXPORT void SetColorSurf  (const Quantity_ColorRGBA& theColor);

  //! Manage surface color setting
  Standard_EXPORT void UnSetColorSurf();
  
  //! Return TRUE if curve color has been defined.
  Standard_Boolean IsSetColorCurv() const { return myHasColorCurv; }

  //! Return curve color.
  const Quantity_Color& GetColorCurv() const { return myColorCurv; }

  //! Set curve color.
  Standard_EXPORT void SetColorCurv (const Quantity_Color& col);
  
  //! Manage curve color setting
  Standard_EXPORT void UnSetColorCurv();

  //! Assign visibility.
  void SetVisibility (const Standard_Boolean theVisibility) { myIsVisible = theVisibility; }

  //! Manage visibility.
  Standard_Boolean IsVisible() const { return myIsVisible; }

  //! Returns True if styles are the same
  //! Methods for using Style as key in maps
  Standard_Boolean IsEqual (const XCAFPrs_Style& theOther) const
  {
    if (myIsVisible != theOther.myIsVisible)
    {
      return false;
    }
    else if (!myIsVisible)
    {
      return true;
    }

    return myHasColorSurf == theOther.myHasColorSurf
        && myHasColorCurv == theOther.myHasColorCurv
        && (!myHasColorSurf || myColorSurf == theOther.myColorSurf)
        && (!myHasColorCurv || myColorCurv == theOther.myColorCurv);
  }

  //! Returns True if styles are the same.
  Standard_Boolean operator== (const XCAFPrs_Style& theOther) const
  {
    return IsEqual (theOther);
  }

  //! Returns a HasCode value.
  static Standard_Integer HashCode (const XCAFPrs_Style& theStyle,
                                    const Standard_Integer theUpper)
  {
    if (!theStyle.myIsVisible)
    {
      return 1;
    }

    int aHashCode = 0;
    if (theStyle.myHasColorSurf)
    {
      aHashCode = aHashCode ^ Quantity_ColorRGBAHasher::HashCode (theStyle.myColorSurf, theUpper);
    }
    if (theStyle.myHasColorCurv)
    {
      aHashCode = aHashCode ^ Quantity_ColorHasher::HashCode (theStyle.myColorCurv, theUpper);
    }
    return ((aHashCode & 0x7fffffff) % theUpper) + 1;
  }

  //! Returns True when the two keys are the same.
  static Standard_Boolean IsEqual (const XCAFPrs_Style& theS1, const XCAFPrs_Style& theS2)
  {
    return theS1.IsEqual (theS2);
  }

protected:

  Quantity_ColorRGBA myColorSurf;
  Quantity_Color     myColorCurv;
  Standard_Boolean   myHasColorSurf;
  Standard_Boolean   myHasColorCurv;
  Standard_Boolean   myIsVisible;

};

#endif // _XCAFPrs_Style_HeaderFile
