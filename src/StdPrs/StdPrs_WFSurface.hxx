// Created on: 1995-08-04
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

#ifndef _StdPrs_WFSurface_HeaderFile
#define _StdPrs_WFSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
class Prs3d_Presentation;
class Adaptor3d_HSurface;


//! Computes the wireframe presentation of surfaces
//! by displaying a given number of U and/or V isoparametric
//! curves. The isoparametric curves are drawn with respect
//! to a given number of points.
class StdPrs_WFSurface  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Draws a surface by drawing the isoparametric curves with respect to
  //! a fixed number of points given by the Drawer.
  //! The number of isoparametric curves to be drawn and their color are
  //! controlled by the furnished Drawer.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Handle(Adaptor3d_HSurface)& aSurface, const Handle(Prs3d_Drawer)& aDrawer);




protected:





private:





};







#endif // _StdPrs_WFSurface_HeaderFile
