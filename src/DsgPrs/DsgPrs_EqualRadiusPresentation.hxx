// Created on: 1998-01-17
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _DsgPrs_EqualRadiusPresentation_HeaderFile
#define _DsgPrs_EqualRadiusPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>

class gp_Pnt;
class Geom_Plane;

//! A framework to define display of equality in radii.
class DsgPrs_EqualRadiusPresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds the points FirstCenter, SecondCenter,
  //! FirstPoint, SecondPoint, and the plane Plane to the
  //! presentation object aPresentation.
  //! The display attributes of these elements is defined by
  //! the attribute manager aDrawer.
  //! FirstCenter and SecondCenter are the centers of the
  //! first and second shapes respectively, and FirstPoint
  //! and SecondPoint are the attachment points of the radii to arcs.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const gp_Pnt&                     FirstCenter,
                                  const gp_Pnt&                     SecondCenter,
                                  const gp_Pnt&                     FirstPoint,
                                  const gp_Pnt&                     SecondPoint,
                                  const Handle(Geom_Plane)&         Plane);

protected:
private:
};

#endif // _DsgPrs_EqualRadiusPresentation_HeaderFile
