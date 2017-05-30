// Created on: 1992-08-26
// Created by: Jean Louis FRENKEL
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Prs3d_HeaderFile
#define _Prs3d_HeaderFile

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>

class TopoDS_Shape;
class Prs3d_Presentation;

//! The Prs3d package provides the following services
//! -   a presentation object (the context for all
//! modifications to the display, its presentation will be
//! displayed in every view of an active viewer)
//! -   an attribute manager governing how objects such
//! as color, width, and type of line are displayed;
//! these are generic objects, whereas those in
//! StdPrs are specific geometries and topologies.
//! -   generic   algorithms providing default settings for
//! objects such as points, curves, surfaces and shapes
//! -   a root object which provides the abstract
//! framework for the DsgPrs definitions at work in
//! display of dimensions, relations and trihedra.
class Prs3d 
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! draws an arrow at a given location, with respect
  //! to a given direction.
  Standard_EXPORT static Standard_Boolean MatchSegment (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const gp_Pnt& p1, const gp_Pnt& p2, Standard_Real& dist);
  
  //! Computes the absolute deflection value depending on
  //! the type of deflection in theDrawer:
  //! <ul>
  //! <li><b>Aspect_TOD_RELATIVE</b>: the absolute deflection is computed using the relative
  //! deviation coefficient from theDrawer and the shape's bounding box;</li>
  //! <li><b>Aspect_TOD_ABSOLUTE</b>: the maximal chordial deviation from theDrawer is returned.</li>
  //! </ul>
  //! In case of the type of deflection in theDrawer computed relative deflection for shape
  //! is stored as absolute deflection. It is necessary to use it later on for sub-shapes.
  //! This function should always be used to compute the deflection value for building
  //! discrete representations of the shape (triangualtion, wireframe) to avoid incosistencies
  //! between different representations of the shape and undesirable visual artifacts.
  Standard_EXPORT static Standard_Real GetDeflection (const TopoDS_Shape& theShape, const Handle(Prs3d_Drawer)& theDrawer);

  //! Assembles array of primitives for sequence of polylines.
  //! @param thePoints [in] the polylines sequence
  //! @return array of primitives
  Standard_EXPORT static Handle(Graphic3d_ArrayOfPrimitives) PrimitivesFromPolylines (const Prs3d_NListOfSequenceOfPnt& thePoints);

  //! Add primitives into new group in presentation and clear the list of polylines.
  Standard_EXPORT static void AddPrimitivesGroup (const Handle(Prs3d_Presentation)& thePrs,
                                                  const Handle(Prs3d_LineAspect)&   theAspect,
                                                  Prs3d_NListOfSequenceOfPnt&       thePolylines);

};

#endif // _Prs3d_HeaderFile
