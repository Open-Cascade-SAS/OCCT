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


#ifndef OpenGl_Polyline_Header
#define OpenGl_Polyline_Header

#include <InterfaceGraphic_telem.hxx>

#include <OpenGl_Element.hxx>

#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Array1OfVertexC.hxx>

class OpenGl_Polyline : public OpenGl_Element
{

public:

  OpenGl_Polyline (const Graphic3d_Array1OfVertex& AListVertex);

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

protected:

  virtual ~OpenGl_Polyline();

protected:

  Tint       myNbVertices;    // Number of vertices in vertices array
  tel_point  myVertices;      // Vertices array of length myNbVertices

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Polyline_Header
