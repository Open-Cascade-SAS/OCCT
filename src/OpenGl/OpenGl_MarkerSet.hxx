// File:      OpenGl_MarkerSet.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_MarkerSet_Header
#define OpenGl_MarkerSet_Header

#include <InterfaceGraphic_telem.hxx>

#include <OpenGl_Element.hxx>

#include <Graphic3d_Vertex.hxx>

class OpenGl_MarkerSet : public OpenGl_Element
{
 public:

  OpenGl_MarkerSet (const Standard_Integer ANbPoints, const Graphic3d_Vertex *APoints);
  virtual ~OpenGl_MarkerSet ();

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  Tint myNbPoints;
  TEL_POINT *myPoints;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //OpenGl_MarkerSet_Header
