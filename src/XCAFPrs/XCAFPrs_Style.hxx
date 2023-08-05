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
#include <XCAFDoc_VisMaterial.hxx>
#include <Standard_HashUtils.hxx>

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

  template<class T>
  friend struct std::hash;

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

namespace std
{
  template <>
  struct hash<XCAFPrs_Style>
  {
    size_t operator()(const XCAFPrs_Style& theStyle) const
    {
      if (!theStyle.myIsVisible)
      {
        return 1;
      }
      size_t aCombination[3];
      int aCount = 0;
      if (theStyle.myHasColorSurf)
      {
        aCombination[aCount++] = std::hash<Quantity_ColorRGBA>{}(theStyle.myColorSurf);
      }
      if (theStyle.myHasColorCurv)
      {
        aCombination[aCount++] = std::hash<Quantity_Color>{}(theStyle.myColorCurv);
      }
      if (!theStyle.myMaterial.IsNull())
      {
        aCombination[aCount++] = std::hash<Handle(XCAFDoc_VisMaterial)>{}(theStyle.myMaterial);
      }
      return aCount > 0 ? opencascade::hashBytes(aCombination, sizeof(size_t) * aCount) : 0;
    }
  };
}

#endif // _XCAFPrs_Style_HeaderFile
