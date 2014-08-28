// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OpenGl_GraduatedTrihedron_Header
#define _OpenGl_GraduatedTrihedron_Header

#include <OpenGl_Element.hxx>

#include <OpenGl_Text.hxx>
#include <Graphic3d_CGraduatedTrihedron.hxx>

class Visual3d_View;
class OpenGl_View;

class OpenGl_GraduatedTrihedron : public OpenGl_Element
{

public:

  static void SetMinMax (const Standard_ShortReal xMin, const Standard_ShortReal yMin, const Standard_ShortReal zMin,
                         const Standard_ShortReal xMax, const Standard_ShortReal yMax, const Standard_ShortReal zMax);

public:

  OpenGl_GraduatedTrihedron (const Graphic3d_CGraduatedTrihedron& theData);

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (OpenGl_Context* theCtx);

protected:

  virtual ~OpenGl_GraduatedTrihedron();

protected:

  mutable OpenGl_Text       myLabelX;
  mutable OpenGl_Text       myLabelY;
  mutable OpenGl_Text       myLabelZ;
  mutable OpenGl_Text       myLabelValues;
  mutable OpenGl_AspectText myAspectLabels;
  mutable OpenGl_AspectText myAspectValues;
  TEL_COLOUR myXNameColor;
  TEL_COLOUR myYNameColor;
  TEL_COLOUR myZNameColor;

  bool myToDrawXName;
  bool myToDrawYName;
  bool myToDrawZName;
  bool myToDrawXValues;
  bool myToDrawYValues;
  bool myToDrawZValues;
  bool myToDrawGrid;
  bool myToDrawAxes;
  unsigned int myNbX, myNbY, myNbZ;
  int myXOffset, myYOffset, myZOffset;
  int myXAxisOffset, myYAxisOffset, myZAxisOffset;
  Standard_Boolean myDrawXTickmarks;
  Standard_Boolean myDrawYTickmarks;
  Standard_Boolean myDrawZTickmarks;
  unsigned int myXTickmarkLength, myYTickmarkLength, myZTickmarkLength;
  float myGridColor[3];
  TEL_COLOUR myXColor;
  TEL_COLOUR myYColor;
  TEL_COLOUR myZColor;
  Graphic3d_CGraduatedTrihedron::minMaxValuesCallback myCbCubicAxes;
  Visual3d_View* myPtrVisual3dView;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_GraduatedTrihedron_Header
