// Created on: 2011-09-20
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
