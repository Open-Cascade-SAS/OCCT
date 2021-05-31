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
#include <XCAFDoc_VisMaterial.hxx>

//! Represents a set of styling settings applicable to a (sub)shape
class XCAFPrs_Style 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor - colors are unset, visibility is TRUE.
  Standard_EXPORT XCAFPrs_Style();

  //! Return TRUE if style is empty - does not override any properties.
  Standard_Boolean IsEmpty() const
  {
    return !myHasColorSurf
        && !myHasColorCurv
        &&  myMaterial.IsNull()
        &&  myIsVisible;
  }

  //! Return material.
  const Handle(XCAFDoc_VisMaterial)& Material() const { return myMaterial; }

  //! Set material.
  void SetMaterial (const Handle(XCAFDoc_VisMaterial)& theMaterial) { myMaterial = theMaterial; }

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

  //! Return base color texture.
  const Handle(Image_Texture)& BaseColorTexture() const
  {
    static const Handle(Image_Texture) THE_NULL_TEXTURE;
    if (myMaterial.IsNull())
    {
      return THE_NULL_TEXTURE;
    }
    else if (myMaterial->HasPbrMaterial()
         && !myMaterial->PbrMaterial().BaseColorTexture.IsNull())
    {
      return myMaterial->PbrMaterial().BaseColorTexture;
    }
    else if (myMaterial->HasCommonMaterial()
         && !myMaterial->CommonMaterial().DiffuseTexture.IsNull())
    {
      return myMaterial->CommonMaterial().DiffuseTexture;
    }
    return THE_NULL_TEXTURE;
  }

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
        && myMaterial == theOther.myMaterial
        && (!myHasColorSurf || myColorSurf == theOther.myColorSurf)
        && (!myHasColorCurv || myColorCurv == theOther.myColorCurv);
  }

  //! Returns True if styles are the same.
  Standard_Boolean operator== (const XCAFPrs_Style& theOther) const
  {
    return IsEqual (theOther);
  }

  //! Computes a hash code for the given set of styling settings, in the range [1, theUpperBound]
  //! @param theStyle the set of styling settings which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode (const XCAFPrs_Style& theStyle, const Standard_Integer theUpperBound)
  {
    if (!theStyle.myIsVisible)
    {
      return 1;
    }

    Standard_Integer aHashCode = 0;
    if (theStyle.myHasColorSurf)
    {
      aHashCode = aHashCode ^ Quantity_ColorRGBAHasher::HashCode (theStyle.myColorSurf, theUpperBound);
    }
    if (theStyle.myHasColorCurv)
    {
      aHashCode = aHashCode ^ Quantity_ColorHasher::HashCode (theStyle.myColorCurv, theUpperBound);
    }
    if (!theStyle.myMaterial.IsNull())
    {
      aHashCode = aHashCode ^ ::HashCode (theStyle.myMaterial, theUpperBound);
    }
    return ::HashCode (aHashCode, theUpperBound);
  }

  //! Returns True when the two keys are the same.
  static Standard_Boolean IsEqual (const XCAFPrs_Style& theS1, const XCAFPrs_Style& theS2)
  {
    return theS1.IsEqual (theS2);
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:

  Handle(XCAFDoc_VisMaterial) myMaterial;
  Quantity_ColorRGBA myColorSurf;
  Quantity_Color     myColorCurv;
  Standard_Boolean   myHasColorSurf;
  Standard_Boolean   myHasColorCurv;
  Standard_Boolean   myIsVisible;

};

#endif // _XCAFPrs_Style_HeaderFile
