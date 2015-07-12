// Created on: 1995-09-19
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StdPrs_HLRPolyShape_HeaderFile
#define _StdPrs_HLRPolyShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
class Prs3d_Presentation;
class TopoDS_Shape;
class Prs3d_Projector;


//! Instantiates Prs3d_PolyHLRShape to define a
//! display of a shape where hidden and visible lines are
//! identified with respect to a given projection.
//! StdPrs_HLRPolyShape works with a polyhedral
//! simplification of the shape whereas
//! StdPrs_HLRShape takes the shape itself into
//! account. When you use StdPrs_HLRShape, you
//! obtain an exact result, whereas, when you use
//! StdPrs_HLRPolyShape, you reduce computation
//! time but obtain polygonal segments.
//! The polygonal algorithm is used.
class StdPrs_HLRPolyShape  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines the hidden line removal display of the
  //! topology aShape in the projection defined by
  //! aProjector. The shape and the projection are added
  //! to the display aPresentation, and the attributes of the
  //! elements present in the aPresentation are defined by
  //! the attribute manager aDrawer.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const TopoDS_Shape& aShape, const Handle(Prs3d_Drawer)& aDrawer, const Handle(Prs3d_Projector)& aProjector);




protected:





private:





};







#endif // _StdPrs_HLRPolyShape_HeaderFile
