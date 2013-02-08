// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef OpenGl_AspectText_Header
#define OpenGl_AspectText_Header

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Font_FontAspect.hxx>
#include <Aspect_TypeOfStyleText.hxx>
#include <Aspect_TypeOfDisplayText.hxx>

#include <TCollection_AsciiString.hxx>

#include <OpenGl_Element.hxx>

//! Text representation parameters
class OpenGl_AspectText : public OpenGl_Element
{

public:

  OpenGl_AspectText();
  virtual ~OpenGl_AspectText();

  //! Copy parameters
  void SetContext (const CALL_DEF_CONTEXTTEXT& theContext);

  //! @return font family name
  const TCollection_AsciiString& FontName() const
  {
    return myFont;
  }

  //! @return font family name
  TCollection_AsciiString& ChangeFontName()
  {
    return myFont;
  }

  //! @return is zoomable flag
  bool IsZoomable() const
  {
    return myZoomable;
  }

  //! @return rotation angle
  float Angle() const
  {
    return myAngle;
  }

  //! @return font aspect (regular/bold/italic)
  Font_FontAspect FontAspect() const
  {
    return myFontAspect;
  }

  //! @param theValue font aspect (regular/bold/italic)
  void SetFontAspect (const Font_FontAspect theValue)
  {
    myFontAspect = theValue;
  }

  //! @return text color
  const TEL_COLOUR& Color() const
  {
    return myColor;
  }

  //! @return text color
  TEL_COLOUR& ChangeColor()
  {
    return myColor;
  }

  //! @return annotation style
  Aspect_TypeOfStyleText StyleType() const
  {
    return myStyleType;
  }

  //! @return subtitle style (none/blend/decale/subtitle)
  Aspect_TypeOfDisplayText DisplayType() const
  {
    return myDisplayType;
  }

  void SetDisplayType (const Aspect_TypeOfDisplayText theType)
  {
    myDisplayType = theType;
  }

  //! @return subtitle color
  const TEL_COLOUR& SubtitleColor() const
  {
    return mySubtitleColor;
  }

  //! @return subtitle color
  TEL_COLOUR& ChangeSubtitleColor()
  {
    return mySubtitleColor;
  }

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

protected:

  TCollection_AsciiString  myFont;
  TEL_COLOUR               myColor;
  TEL_COLOUR               mySubtitleColor;
  float                    myAngle;
  Aspect_TypeOfStyleText   myStyleType;
  Aspect_TypeOfDisplayText myDisplayType;
  Font_FontAspect          myFontAspect;
  bool                     myZoomable;

public:

  DEFINE_STANDARD_ALLOC

};

#endif // OpenGl_AspectText_Header
