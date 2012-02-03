// File:      OpenGl_AspectMarker.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_AspectMarker_Header
#define OpenGl_AspectMarker_Header

#include <InterfaceGraphic_telem.hxx>
#include <Aspect_TypeOfMarker.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_AspectMarker : public OpenGl_Element
{
 public:

  OpenGl_AspectMarker ();
  virtual ~OpenGl_AspectMarker () {}

  void SetContext (const CALL_DEF_CONTEXTMARKER &AContext);

  const TEL_COLOUR &  Color() const { return myColor; }
  Aspect_TypeOfMarker Type() const { return myType; }
  float               Scale() const { return myScale; }

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  TEL_COLOUR          myColor;
  Aspect_TypeOfMarker myType;
  float               myScale;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //OpenGl_AspectMarker_Header
