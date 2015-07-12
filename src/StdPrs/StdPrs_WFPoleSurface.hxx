// Created on: 1995-07-24
// Created by: Modelistation
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

#ifndef _StdPrs_WFPoleSurface_HeaderFile
#define _StdPrs_WFPoleSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
class Prs3d_Presentation;
class Adaptor3d_Surface;


//! Computes the presentation of surfaces by drawing a
//! double network of lines linking the poles of the surface
//! in the two parametric direction.
//! The number of lines to be drawn is controlled
//! by the NetworkNumber of the given Drawer.
class StdPrs_WFPoleSurface  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the surface aSurface to the presentation object aPresentation.
  //! The shape's display attributes are set in the attribute manager aDrawer.
  //! The surface aSurface is a surface object from
  //! Adaptor3d, and provides data from a Geom surface.
  //! This makes it possible to use the surface in a geometric algorithm.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Surface& aSurface, const Handle(Prs3d_Drawer)& aDrawer);




protected:





private:





};







#endif // _StdPrs_WFPoleSurface_HeaderFile
