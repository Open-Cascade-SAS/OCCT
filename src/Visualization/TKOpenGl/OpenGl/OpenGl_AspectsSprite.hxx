// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _OpenGl_AspectsSprite_Header
#define _OpenGl_AspectsSprite_Header

#include <Graphic3d_Aspects.hxx>
#include <Graphic3d_TextureMap.hxx>

class OpenGl_Context;
class OpenGl_PointSprite;

//! OpenGl resources for custom point sprites.
class OpenGl_AspectsSprite
{
public:
  DEFINE_STANDARD_ALLOC
public:
  //! Empty constructor.
  OpenGl_AspectsSprite()
      : myMarkerSize(1.0f),
        myIsSpriteReady(false)
  {
  }

  float MarkerSize() const { return myMarkerSize; }

  //! Return TRUE if resource is up-to-date.
  bool IsReady() const { return myIsSpriteReady; }

  //! Invalidate resource state.
  void Invalidate() { myIsSpriteReady = false; }

  //! Return TRUE if OpenGl point sprite resource defines texture.
  Standard_EXPORT bool HasPointSprite(const occ::handle<OpenGl_Context>&    theCtx,
                                      const occ::handle<Graphic3d_Aspects>& theAspects);

  //! Return TRUE if OpenGl point sprite resource defined by obsolete Display List (bitmap).
  Standard_EXPORT bool IsDisplayListSprite(const occ::handle<OpenGl_Context>&    theCtx,
                                           const occ::handle<Graphic3d_Aspects>& theAspects);

  //! Return sprite.
  Standard_EXPORT const occ::handle<OpenGl_PointSprite>& Sprite(
    const occ::handle<OpenGl_Context>&    theCtx,
    const occ::handle<Graphic3d_Aspects>& theAspects,
    bool                                  theIsAlphaSprite);

  //! Update texture resource up-to-date state.
  Standard_EXPORT void UpdateRediness(const occ::handle<Graphic3d_Aspects>& theAspect);

  //! Release texture resource.
  Standard_EXPORT void Release(OpenGl_Context* theCtx);

private:
  //! Build texture resources.
  Standard_EXPORT void build(const occ::handle<OpenGl_Context>&        theCtx,
                             const occ::handle<Graphic3d_MarkerImage>& theMarkerImage,
                             Aspect_TypeOfMarker                       theType,
                             float                                     theScale,
                             const NCollection_Vec4<float>&            theColor,
                             float&                                    theMarkerSize);

  //! Generate resource keys for a sprite.
  static void spriteKeys(const occ::handle<Graphic3d_MarkerImage>& theMarkerImage,
                         Aspect_TypeOfMarker                       theType,
                         float                                     theScale,
                         const NCollection_Vec4<float>&            theColor,
                         TCollection_AsciiString&                  theKey,
                         TCollection_AsciiString&                  theKeyA);

private:
  occ::handle<OpenGl_PointSprite> mySprite;
  occ::handle<OpenGl_PointSprite> mySpriteA;
  float                           myMarkerSize;
  bool                            myIsSpriteReady;
};

#endif // _OpenGl_Aspects_Header
