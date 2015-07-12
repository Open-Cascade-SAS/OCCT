// Created on: 1995-07-27
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

#ifndef _StdPrs_ShadedSurface_HeaderFile
#define _StdPrs_ShadedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
class Prs3d_Presentation;
class Adaptor3d_Surface;


//! Computes the shading presentation of surfaces.
//! Draws a surface by drawing the isoparametric curves with respect to
//! a maximal chordial deviation.
//! The number of isoparametric curves to be drawn and their color are
//! controlled by the furnished Drawer.
class StdPrs_ShadedSurface  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the surface aSurface to the presentation object aPresentation.
  //! The surface's display attributes are set in the attribute manager aDrawer.
  //! The surface object from Adaptor3d provides data
  //! from a Geom surface in order to use the surface in an algorithm.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Surface& aSurface, const Handle(Prs3d_Drawer)& aDrawer);




protected:





private:





};







#endif // _StdPrs_ShadedSurface_HeaderFile
