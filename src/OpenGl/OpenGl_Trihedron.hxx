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

  static void Setup (const Quantity_NameOfColor theXColor,
                     const Quantity_NameOfColor theYColor,
                     const Quantity_NameOfColor theZColor,
                     const Standard_Real        theSizeRatio,
                     const Standard_Real        theAxisDiametr,
                     const Standard_Integer     theNbFacettes);

public:

  OpenGl_Trihedron (const Aspect_TypeOfTriedronPosition thePosition,
                    const Quantity_NameOfColor          theColor,
                    const Standard_Real                 theScale,
                    const Standard_Boolean              theAsWireframe);

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (OpenGl_Context* theCtx);

protected:

  virtual ~OpenGl_Trihedron();

  void redraw        (const Handle(OpenGl_Workspace)& theWorkspace) const;
  void redrawZBuffer (const Handle(OpenGl_Workspace)& theWorkspace) const;

protected:

  Aspect_TypeOfTriedronPosition myPos;
  Standard_Real myScale;
  Standard_Boolean myIsWireframe;
  // Parameters for z-buffered mode
  TEL_COLOUR myXColor;
  TEL_COLOUR myYColor;
  TEL_COLOUR myZColor;
  float myRatio;
  float myDiameter;
  int   myNbFacettes;

  OpenGl_AspectLine       myAspectLine;
  OpenGl_AspectText       myAspectText;
  mutable OpenGl_Text     myLabelX;
  mutable OpenGl_Text     myLabelY;
  mutable OpenGl_Text     myLabelZ;
  mutable OpenGl_Cylinder myCylinder;
  mutable OpenGl_Sphere   mySphere;
  mutable OpenGl_Cylinder myCone;
  mutable OpenGl_Disk     myDisk;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Trihedron_Header
