// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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

#ifndef OpenGl_AspectMarker_Header
#define OpenGl_AspectMarker_Header

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_CGroup.hxx>
#include <TCollection_AsciiString.hxx>

#include <OpenGl_Element.hxx>
#include <Handle_OpenGl_PointSprite.hxx>
#include <Handle_OpenGl_ShaderProgram.hxx>

#include <Image_PixMap_Handle.hxx>

class OpenGl_AspectMarker : public OpenGl_Element
{
public:

  OpenGl_AspectMarker();

  //! Copy parameters
  void SetAspect (const CALL_DEF_CONTEXTMARKER& theAspect);

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
  const Handle(OpenGl_PointSprite)& SpriteRes (const Handle(OpenGl_Workspace)& theWorkspace) const
  {
    if (!myResources.IsSpriteReady())
    {
      myResources.BuildSprites (theWorkspace, myMarkerImage, myType, myScale, myColor, myMarkerSize);
      myResources.SetSpriteReady();
    }

    return myResources.Sprite;
  }

  //! Init and return OpenGl highlight point sprite resource.
  //! @return point sprite texture for highlight.
  const Handle(OpenGl_PointSprite)& SpriteHighlightRes (const Handle(OpenGl_Workspace)& theWorkspace) const
  {
    if (!myResources.IsSpriteReady())
    {
      myResources.BuildSprites (theWorkspace, myMarkerImage, myType, myScale, myColor, myMarkerSize);
      myResources.SetSpriteReady();
    }

    return myResources.SpriteA;
  }

  //! Init and return OpenGl shader program resource.
  //! @return shader program resource.
  const Handle(OpenGl_ShaderProgram)& ShaderProgramRes (const Handle(OpenGl_Workspace)& theWorkspace) const
  {
    if (!myResources.IsShaderReady())
    {
      myResources.BuildShader (theWorkspace, myShaderProgram);
      myResources.SetShaderReady();
    }

    return myResources.ShaderProgram;
  }

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

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

    void BuildSprites (const Handle(OpenGl_Workspace)& theWS,
                       const Handle(Graphic3d_MarkerImage)& theMarkerImage,
                       const Aspect_TypeOfMarker theType,
                       const Standard_ShortReal theScale,
                       const TEL_COLOUR& theColor,
                       Standard_ShortReal& theMarkerSize);

    void BuildShader (const Handle(OpenGl_Workspace)& theWS, const Handle(Graphic3d_ShaderProgram)& theShader);

    void SpriteKeys (const Handle(Graphic3d_MarkerImage)& theMarkerImage,
                     const Aspect_TypeOfMarker theType,
                     const Standard_ShortReal theScale,
                     const TEL_COLOUR& theColor,
                     TCollection_AsciiString& theKey,
                     TCollection_AsciiString& theKeyA);

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
