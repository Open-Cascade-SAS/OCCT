// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_AspectMarker_Header
#define OpenGl_AspectMarker_Header

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_CAspectMarker.hxx>
#include <TCollection_AsciiString.hxx>

#include <OpenGl_Element.hxx>
#include <Handle_OpenGl_PointSprite.hxx>
#include <Handle_OpenGl_ShaderProgram.hxx>

#include <Image_PixMap_Handle.hxx>

class OpenGl_AspectMarker : public OpenGl_Element
{
public:

  Standard_EXPORT OpenGl_AspectMarker();

  //! Copy parameters
  Standard_EXPORT void SetAspect (const CALL_DEF_CONTEXTMARKER& theAspect);

  //! @return marker color
  const TEL_COLOUR& Color() const
  {
    return myColor;
  }

  //! @return maker type
  Aspect_TypeOfMarker Type() const
  {
    return myType;
  }

  //! @return marker scale
  Standard_ShortReal Scale() const
  {
    return myScale;
  }

  //! @return marker size
  Standard_ShortReal MarkerSize() const
  {
    return myMarkerSize;
  }

  //! Init and return OpenGl point sprite resource.
  //! @return point sprite texture.
  const Handle(OpenGl_PointSprite)& SpriteRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsSpriteReady())
    {
      myResources.BuildSprites (theCtx, myMarkerImage, myType, myScale, myColor, myMarkerSize);
      myResources.SetSpriteReady();
    }

    return myResources.Sprite;
  }

  //! Init and return OpenGl highlight point sprite resource.
  //! @return point sprite texture for highlight.
  const Handle(OpenGl_PointSprite)& SpriteHighlightRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsSpriteReady())
    {
      myResources.BuildSprites (theCtx, myMarkerImage, myType, myScale, myColor, myMarkerSize);
      myResources.SetSpriteReady();
    }

    return myResources.SpriteA;
  }

  //! Init and return OpenGl shader program resource.
  //! @return shader program resource.
  const Handle(OpenGl_ShaderProgram)& ShaderProgramRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsShaderReady())
    {
      myResources.BuildShader (theCtx, myShaderProgram);
      myResources.SetShaderReady();
    }

    return myResources.ShaderProgram;
  }

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

protected: //! @name ordinary aspect properties

  TEL_COLOUR                      myColor;
  Aspect_TypeOfMarker             myType;
  Standard_ShortReal              myScale;
  mutable Standard_ShortReal      myMarkerSize;
  Handle(Graphic3d_MarkerImage)   myMarkerImage;
  Handle(Graphic3d_ShaderProgram) myShaderProgram;

protected: //! @name OpenGl resources

  //! OpenGl resources
  mutable struct Resources
  {
  public:

    Resources() :
        SpriteKey (""),
        SpriteAKey (""),
        myIsSpriteReady (Standard_False),
        myIsShaderReady (Standard_False) {}

    Standard_Boolean IsSpriteReady() const { return myIsSpriteReady; }
    Standard_Boolean IsShaderReady() const { return myIsShaderReady; }
    void SetSpriteReady() { myIsSpriteReady = Standard_True; }
    void SetShaderReady() { myIsShaderReady = Standard_True; }
    void ResetSpriteReadiness() { myIsSpriteReady = Standard_False; }
    void ResetShaderReadiness() { myIsShaderReady = Standard_False; }

    Standard_EXPORT void BuildSprites (const Handle(OpenGl_Context)&        theCtx,
                                       const Handle(Graphic3d_MarkerImage)& theMarkerImage,
                                       const Aspect_TypeOfMarker            theType,
                                       const Standard_ShortReal             theScale,
                                       const TEL_COLOUR&                    theColor,
                                       Standard_ShortReal&                  theMarkerSize);

    Standard_EXPORT void BuildShader (const Handle(OpenGl_Context)&          theCtx,
                                      const Handle(Graphic3d_ShaderProgram)& theShader);

    Standard_EXPORT void SpriteKeys (const Handle(Graphic3d_MarkerImage)& theMarkerImage,
                                     const Aspect_TypeOfMarker            theType,
                                     const Standard_ShortReal             theScale,
                                     const TEL_COLOUR&                    theColor,
                                     TCollection_AsciiString&             theKey,
                                     TCollection_AsciiString&             theKeyA);

    Handle(OpenGl_PointSprite)   Sprite;
    TCollection_AsciiString      SpriteKey;

    Handle(OpenGl_PointSprite)   SpriteA;
    TCollection_AsciiString      SpriteAKey;

    Handle(OpenGl_ShaderProgram) ShaderProgram;
    TCollection_AsciiString      ShaderProgramId;

  private:

    Standard_Boolean myIsSpriteReady;
    Standard_Boolean myIsShaderReady;

  } myResources;

public:

  DEFINE_STANDARD_ALLOC

};

#endif // OpenGl_AspectMarker_Header
