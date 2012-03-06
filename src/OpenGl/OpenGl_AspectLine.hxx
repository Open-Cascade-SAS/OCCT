// File:      OpenGl_AspectLine.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef _OpenGl_AspectLine_Header
#define _OpenGl_AspectLine_Header

#include <InterfaceGraphic_telem.hxx>
#include <Aspect_TypeOfLine.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_AspectLine : public OpenGl_Element
{
 public:

  OpenGl_AspectLine ();
  OpenGl_AspectLine (const OpenGl_AspectLine &AnOther);
  virtual ~OpenGl_AspectLine () {}

  void SetContext (const CALL_DEF_CONTEXTLINE &AContext);

  const TEL_COLOUR & Color() const { return myColor; }
  Aspect_TypeOfLine  Type() const { return myType; }
  float              Width() const { return myWidth; }

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  TEL_COLOUR        myColor;
  Aspect_TypeOfLine myType;
  float             myWidth;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_AspectLine_Header
