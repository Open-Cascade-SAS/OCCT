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

#ifndef _OpenGl_Trihedron_Header
#define _OpenGl_Trihedron_Header

#include <OpenGl_Element.hxx>

#include <Aspect_TypeOfTriedronPosition.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Cylinder.hxx>
#include <OpenGl_Disk.hxx>
#include <OpenGl_Sphere.hxx>
#include <OpenGl_Text.hxx>
#include <Quantity_NameOfColor.hxx>

//! Class render trihedron
class OpenGl_Trihedron : public OpenGl_Element
{
public:

  //! Default constructor.
  OpenGl_Trihedron();

  //! Destructor.
  virtual ~OpenGl_Trihedron();

  //! Render the element.
  virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Release OpenGL resources.
  virtual void Release (OpenGl_Context* theCtx);

public:

  //! Switch wireframe / shaded trihedron.
  void SetWireframe (const Standard_Boolean theAsWireframe) { myIsWireframe = theAsWireframe; }

  //! Setup the corner to draw the trihedron.
  void SetPosition (const Aspect_TypeOfTriedronPosition thePosition) { myPos = thePosition; }

  //! Setup the scale factor.
  void SetScale (const Standard_Real theScale);

  //! Setup the size ratio factor.
  void SetSizeRatio (const Standard_Real theRatio);

  //! Setup the arrow diameter.
  void SetArrowDiameter (const Standard_Real theDiam);

  //! Setup the number of facets for tessellation.
  void SetNbFacets (const Standard_Integer theNbFacets);

  //! Setup color of text labels.
  void SetLabelsColor (const Quantity_Color& theColor);

  //! Setup per-arrow color.
  void SetArrowsColors (const Quantity_Color& theColorX,
                        const Quantity_Color& theColorY,
                        const Quantity_Color& theColorZ);

protected:

  //! Invalidate Primitve Arrays.
  void invalidate();

  void redraw(const Handle(OpenGl_Workspace)& theWorkspace) const;
  void redrawZBuffer(const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Resets current model-view and projection transformations and sets
  //! translation for trihedron position
  //! @sa Aspect_TypeOfTriedronPosition
  void resetTransformations (const Handle(OpenGl_Workspace)& theWorkspace) const;

protected:

  Aspect_TypeOfTriedronPosition myPos;
  Standard_Real myScale;
  Standard_Boolean myIsWireframe;
  OpenGl_Vec4 myXColor;
  OpenGl_Vec4 myYColor;
  OpenGl_Vec4 myZColor;
  float myRatio;
  float myDiameter;
  int   myNbFacettes;

  OpenGl_AspectLine              myAspectLine;
  OpenGl_AspectText              myAspectText;
  mutable OpenGl_Text            myLabelX;
  mutable OpenGl_Text            myLabelY;
  mutable OpenGl_Text            myLabelZ;
  mutable OpenGl_Cylinder        myCylinder;
  mutable OpenGl_Sphere          mySphere;
  mutable OpenGl_Cylinder        myCone;
  mutable OpenGl_Disk            myDisk;
  mutable OpenGl_PrimitiveArray  myLine;
  mutable OpenGl_PrimitiveArray  myCircle;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Trihedron_Header
