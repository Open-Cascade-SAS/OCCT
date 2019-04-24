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

#ifndef _StdPrs_PoleCurve_HeaderFile
#define _StdPrs_PoleCurve_HeaderFile

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>

class Adaptor3d_Curve;

//! A framework to provide display of Bezier or BSpline curves
//! (by drawing a broken line linking the poles of the curve).
class StdPrs_PoleCurve  : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines display of BSpline and Bezier curves.
  //! Adds the 3D curve aCurve to the
  //! StdPrs_PoleCurve algorithm. This shape is found in
  //! the presentation object aPresentation, and its display
  //! attributes are set in the attribute manager aDrawer.
  //! The curve object from Adaptor3d provides data from
  //! a Geom curve. This makes it possible to use the
  //! surface in a geometric algorithm.
  Standard_EXPORT static void Add (const Handle(Prs3d_Presentation)& aPresentation, const Adaptor3d_Curve& aCurve, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! returns true if the distance between the point (X,Y,Z) and the
  //! broken line made of the poles is less then aDistance.
  Standard_EXPORT static Standard_Boolean Match (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Adaptor3d_Curve& aCurve, const Handle(Prs3d_Drawer)& aDrawer);
  
  //! returns the pole  the most near of the point (X,Y,Z) and
  //! returns its range. The distance between the pole and
  //! (X,Y,Z) must be less then aDistance. If no pole corresponds, 0 is returned.
  Standard_EXPORT static Standard_Integer Pick (const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Standard_Real aDistance, const Adaptor3d_Curve& aCurve, const Handle(Prs3d_Drawer)& aDrawer);

};

#endif // _StdPrs_PoleCurve_HeaderFile
