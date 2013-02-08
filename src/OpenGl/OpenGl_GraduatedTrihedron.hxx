// Created on: 2011-09-20
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

#ifndef _OpenGl_GraduatedTrihedron_Header
#define _OpenGl_GraduatedTrihedron_Header

#include <OpenGl_Element.hxx>

#include <OpenGl_Text.hxx>
#include <Graphic3d_CGraduatedTrihedron.hxx>

class OpenGl_View;

class OpenGl_GraduatedTrihedron : public OpenGl_Element
{

public:

  static void SetMinMax (const Standard_ShortReal xMin, const Standard_ShortReal yMin, const Standard_ShortReal zMin,
                         const Standard_ShortReal xMax, const Standard_ShortReal yMax, const Standard_ShortReal zMax);

public:

  OpenGl_GraduatedTrihedron (const Graphic3d_CGraduatedTrihedron& theData);

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theCtx);

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
  unsigned char myDrawXTickmarks, myDrawYTickmarks, myDrawZTickmarks;
  unsigned int myXTickmarkLength, myYTickmarkLength, myZTickmarkLength;
  float myGridColor[3];
  TEL_COLOUR myXColor;
  TEL_COLOUR myYColor;
  TEL_COLOUR myZColor;
  minMaxValuesCallback myCbCubicAxes;
  void* myPtrVisual3dView;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_GraduatedTrihedron_Header
