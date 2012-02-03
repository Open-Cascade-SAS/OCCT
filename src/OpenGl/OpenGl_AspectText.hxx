// File:      OpenGl_AspectText.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_AspectText_Header
#define OpenGl_AspectText_Header

#include <InterfaceGraphic_telem.hxx>
#include <OSD_FontAspect.hxx>
#include <Aspect_TypeOfStyleText.hxx>
#include <Aspect_TypeOfDisplayText.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_AspectText : public OpenGl_Element
{
 public:

  OpenGl_AspectText ();
  virtual ~OpenGl_AspectText ();

  void SetContext (const CALL_DEF_CONTEXTTEXT &AContext);

  int                      IsZoomable() const { return myZoomable; }
  float                    Angle() const { return myAngle; }
  OSD_FontAspect           FontAspect() const { return myFontAspect; }
  const char *             Font() const { return myFont; }
  const TEL_COLOUR &       Color() const { return myColor; }
  Aspect_TypeOfStyleText   StyleType() const { return myStyleType; }
  Aspect_TypeOfDisplayText DisplayType() const { return myDisplayType; }
  const TEL_COLOUR &       SubtitleColor() const { return mySubtitleColor; }

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  void SetFontName (const char *AFont);

  int                      myZoomable;
  float                    myAngle;
  OSD_FontAspect           myFontAspect;
  const char              *myFont;
  //float                  mySpace;
  //float                  myExpan;
  TEL_COLOUR               myColor;
  Aspect_TypeOfStyleText   myStyleType;
  Aspect_TypeOfDisplayText myDisplayType;
  TEL_COLOUR               mySubtitleColor;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //OpenGl_AspectText_Header
