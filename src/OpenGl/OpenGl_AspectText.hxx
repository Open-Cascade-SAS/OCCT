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


#ifndef OpenGl_AspectText_Header
#define OpenGl_AspectText_Header

#include <InterfaceGraphic_telem.hxx>
#include <Font_FontAspect.hxx>
#include <Aspect_TypeOfStyleText.hxx>
#include <Aspect_TypeOfDisplayText.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_AspectText : public OpenGl_Element
{

public:

  OpenGl_AspectText();
  virtual ~OpenGl_AspectText();

  void SetContext (const CALL_DEF_CONTEXTTEXT &AContext);

  int                      IsZoomable() const { return myZoomable; }
  float                    Angle() const { return myAngle; }
  Font_FontAspect          FontAspect() const { return myFontAspect; }
  const char *             Font() const { return myFont; }
  const TEL_COLOUR &       Color() const { return myColor; }
  Aspect_TypeOfStyleText   StyleType() const { return myStyleType; }
  Aspect_TypeOfDisplayText DisplayType() const { return myDisplayType; }
  const TEL_COLOUR &       SubtitleColor() const { return mySubtitleColor; }

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

protected:

  void SetFontName (const char *AFont);

protected:

  int                      myZoomable;
  float                    myAngle;
  Font_FontAspect          myFontAspect;
  const char              *myFont;
  //float                  mySpace;
  //float                  myExpan;
  TEL_COLOUR               myColor;
  Aspect_TypeOfStyleText   myStyleType;
  Aspect_TypeOfDisplayText myDisplayType;
  TEL_COLOUR               mySubtitleColor;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_AspectText_Header
