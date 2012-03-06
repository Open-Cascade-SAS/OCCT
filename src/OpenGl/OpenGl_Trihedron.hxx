// File:      OpenGl_Trihedron.hxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef _OpenGl_Trihedron_Header
#define _OpenGl_Trihedron_Header

#include <Handle_OpenGl_Trihedron.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectText.hxx>

class OpenGl_Trihedron : public MMgt_TShared
{
 public:
  OpenGl_Trihedron (const Aspect_TypeOfTriedronPosition APosition, const Quantity_NameOfColor AColor, const Standard_Real AScale, const Standard_Boolean AsWireframe);
  virtual ~OpenGl_Trihedron ();

  static void Setup (const Quantity_NameOfColor XColor, const Quantity_NameOfColor YColor, const Quantity_NameOfColor ZColor,
                     const Standard_Real SizeRatio, const Standard_Real AxisDiametr, const Standard_Integer NbFacettes);

  void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

  // Type definition
  //
  DEFINE_STANDARD_RTTI(OpenGl_Trihedron)

 protected:

  void Redraw (const Handle(OpenGl_Workspace) &AWorkspace) const;
  void RedrawZBuffer (const Handle(OpenGl_Workspace) &AWorkspace) const;

  Aspect_TypeOfTriedronPosition myPos;
  Standard_Real myScale;
  Standard_Boolean myIsWireframe;
  // Parameters for zbuffered mode
  TEL_COLOUR myXColor;
  TEL_COLOUR myYColor;
  TEL_COLOUR myZColor;
  float myRatio;
  float myDiameter;
  int   myNbFacettes;

  OpenGl_AspectLine myAspectLine;
  OpenGl_AspectText myAspectText;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_Trihedron_Header
