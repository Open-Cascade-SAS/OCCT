// File:      OpenGl_Marker.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_Marker_Header
#define OpenGl_Marker_Header

#include <InterfaceGraphic_telem.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_Marker : public OpenGl_Element
{
 public:

  OpenGl_Marker (const TEL_POINT &APoint) : myPoint(APoint) {}
  virtual ~OpenGl_Marker () {}

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  TEL_POINT myPoint;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //OpenGl_Marker_Header
