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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Quantity_Length.hxx>
#include <Standard_Real.hxx>
#include <Prs3d_Drawer.hxx>
class gp_Pnt;
class TopoDS_Shape;
class Prs3d_Presentation;
class Prs3d_BasicAspect;
class Prs3d_PointAspect;
class Prs3d_LineAspect;
class Prs3d_ShadingAspect;
class Prs3d_TextAspect;
class Prs3d_IsoAspect;
class Prs3d_ArrowAspect;
class Prs3d_PlaneAspect;
class Prs3d_DimensionAspect;
class Prs3d_DatumAspect;
class Prs3d_Projector;
class Prs3d_PlaneSet;
class Prs3d_Root;
class Prs3d_Text;
class Prs3d_ShapeTool;
class Prs3d_Arrow;


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
  Standard_EXPORT static Standard_Boolean MatchSegment (const Quantity_Length X, const Quantity_Length Y, const Quantity_Length Z, const Quantity_Length aDistance, const gp_Pnt& p1, const gp_Pnt& p2, Quantity_Length& dist);
  
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




protected:





private:




friend class Prs3d_Presentation;
friend class Prs3d_BasicAspect;
friend class Prs3d_PointAspect;
friend class Prs3d_LineAspect;
friend class Prs3d_ShadingAspect;
friend class Prs3d_TextAspect;
friend class Prs3d_IsoAspect;
friend class Prs3d_ArrowAspect;
friend class Prs3d_PlaneAspect;
friend class Prs3d_DimensionAspect;
friend class Prs3d_DatumAspect;
friend class Prs3d_Projector;
friend class Prs3d_PlaneSet;
friend class Prs3d_Root;
friend class Prs3d_Text;
friend class Prs3d_ShapeTool;
friend class Prs3d_Arrow;

};







#endif // _Prs3d_HeaderFile
