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
class OpenGl_TextureSet;

//! OpenGl resources for custom point sprites.
class OpenGl_AspectsSprite
{
public:
  DEFINE_STANDARD_ALLOC
public:
  //! Empty constructor.
  OpenGl_AspectsSprite() : myMarkerSize (1.0f), myIsSpriteReady  (Standard_False) {}

  Standard_ShortReal MarkerSize() const { return myMarkerSize; }

  //! Return RGB sprite.
  const Handle(OpenGl_TextureSet)& Sprite (const Handle(OpenGl_Context)& theCtx,
                                           const Handle(Graphic3d_Aspects)& theAspecta)
  {
    if (!myIsSpriteReady)
    {
      build (theCtx, theAspecta->MarkerImage(), theAspecta->MarkerType(), theAspecta->MarkerScale(), theAspecta->ColorRGBA(), myMarkerSize);
      myIsSpriteReady = true;
    }
    return mySprite;
  }
  
  //! Return Alpha sprite.
  const Handle(OpenGl_TextureSet)& SpriteA (const Handle(OpenGl_Context)& theCtx,
                                            const Handle(Graphic3d_Aspects)& theAspecta)
  {
    if (!myIsSpriteReady)
    {
      build (theCtx, theAspecta->MarkerImage(), theAspecta->MarkerType(), theAspecta->MarkerScale(), theAspecta->ColorRGBA(), myMarkerSize);
      myIsSpriteReady = true;
    }
    return mySpriteA;
  }
  
  //! Update texture resource up-to-date state.
  Standard_EXPORT void UpdateRediness (const Handle(Graphic3d_Aspects)& theAspect);

  //! Release texture resource.
  Standard_EXPORT void Release (OpenGl_Context* theCtx);

private:

  //! Build texture resources.
  Standard_EXPORT void build (const Handle(OpenGl_Context)& theCtx,
                              const Handle(Graphic3d_MarkerImage)& theMarkerImage,
                              Aspect_TypeOfMarker theType,
                              Standard_ShortReal theScale,
                              const Graphic3d_Vec4& theColor,
                              Standard_ShortReal& theMarkerSize);

  //! Generate resource keys for a sprite.
  static void spriteKeys (const Handle(Graphic3d_MarkerImage)& theMarkerImage,
                          Aspect_TypeOfMarker theType,
                          Standard_ShortReal theScale,
                          const Graphic3d_Vec4& theColor,
                          TCollection_AsciiString& theKey,
                          TCollection_AsciiString& theKeyA);

private:

  Handle(OpenGl_TextureSet) mySprite;
  Handle(OpenGl_TextureSet) mySpriteA;
  Standard_ShortReal myMarkerSize;
  Standard_Boolean myIsSpriteReady;

};

#endif // _OpenGl_Aspects_Header
