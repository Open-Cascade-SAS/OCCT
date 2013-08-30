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

#include <Image_PixMap_Handle.hxx>

class OpenGl_AspectMarker : public OpenGl_Element
{
public:

  OpenGl_AspectMarker();

  void Init (const Handle(OpenGl_Context)& theContext,
             const CALL_DEF_CONTEXTMARKER& theAspect);

  const TEL_COLOUR&   Color()      const { return myColor; }
  Aspect_TypeOfMarker Type()       const { return myType;  }
  Standard_ShortReal  Scale()      const { return myScale; }
  Standard_ShortReal  MarkerSize() const { return myMarkerSize; }
  const Handle(OpenGl_PointSprite)& Sprite()          const { return mySprite;  }
  const Handle(OpenGl_PointSprite)& SpriteHighlight() const { return mySpriteA; }

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

protected:

  TEL_COLOUR                 myColor;
  Aspect_TypeOfMarker        myType;
  Standard_ShortReal         myScale;
  Standard_ShortReal         myMarkerSize;
  TCollection_AsciiString    mySpriteKey;    //!< shared resource ID
  TCollection_AsciiString    mySpriteAKey;   //!< shared resource ID
  Handle(OpenGl_PointSprite) mySprite;       //!< normal sprite
  Handle(OpenGl_PointSprite) mySpriteA;      //!< extra alphs-only sprite for RGB sprites

public:

  DEFINE_STANDARD_ALLOC

};

#endif // OpenGl_AspectMarker_Header
