// Created on: 1993-10-06
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _GeomFill_BezierCurves_HeaderFile
#define _GeomFill_BezierCurves_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomFill_FillingStyle.hxx>
class Geom_BezierSurface;
class Geom_BezierCurve;

//! This class provides an algorithm for constructing a Bezier surface filled from
//! contiguous Bezier curves which form its boundaries.
//! The algorithm accepts two, three or four Bezier curves
//! as the boundaries of the target surface.
//! A range of filling styles - more or less rounded, more or less flat - is available.
//! A BezierCurves object provides a framework for:
//! -   defining the boundaries, and the filling style of the surface
//! -   implementing the construction algorithm
//! -   consulting the result.
//! Warning
//! Some problems may show up with rational curves.
class GeomFill_BezierCurves
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty framework for building a Bezier
  //! surface from contiguous Bezier curves.
  //! You use the Init function to define the boundaries of the surface.
  Standard_EXPORT GeomFill_BezierCurves();

  //! Constructs a framework for building a Bezier surface
  //! from the four contiguous Bezier curves, C1, C2, C3 and C4
  //! Raises Standard_ConstructionError if the curves are not contiguous.
  Standard_EXPORT GeomFill_BezierCurves(const Handle(Geom_BezierCurve)& C1,
                                        const Handle(Geom_BezierCurve)& C2,
                                        const Handle(Geom_BezierCurve)& C3,
                                        const Handle(Geom_BezierCurve)& C4,
                                        const GeomFill_FillingStyle     Type);

  //! Constructs a framework for building a Bezier surface
  //! from the three contiguous Bezier curves, C1, C2 and C3
  //! Raises Standard_ConstructionError if the curves are not contiguous.
  Standard_EXPORT GeomFill_BezierCurves(const Handle(Geom_BezierCurve)& C1,
                                        const Handle(Geom_BezierCurve)& C2,
                                        const Handle(Geom_BezierCurve)& C3,
                                        const GeomFill_FillingStyle     Type);

  //! Constructs a framework for building a Bezier surface
  //! from the two contiguous Bezier curves, C1 and C2
  //! Raises Standard_ConstructionError if the curves are not contiguous.
  Standard_EXPORT GeomFill_BezierCurves(const Handle(Geom_BezierCurve)& C1,
                                        const Handle(Geom_BezierCurve)& C2,
                                        const GeomFill_FillingStyle     Type);

  //! if the curves cannot be joined
  Standard_EXPORT void Init(const Handle(Geom_BezierCurve)& C1,
                            const Handle(Geom_BezierCurve)& C2,
                            const Handle(Geom_BezierCurve)& C3,
                            const Handle(Geom_BezierCurve)& C4,
                            const GeomFill_FillingStyle     Type);

  //! if the curves cannot be joined
  Standard_EXPORT void Init(const Handle(Geom_BezierCurve)& C1,
                            const Handle(Geom_BezierCurve)& C2,
                            const Handle(Geom_BezierCurve)& C3,
                            const GeomFill_FillingStyle     Type);

  //! Initializes or reinitializes this algorithm with two, three,
  //! or four curves - C1, C2, C3, and C4 - and Type, one
  //! of the following filling styles:
  //! -   GeomFill_Stretch - the style with the flattest patch
  //! -   GeomFill_Coons - a rounded style of patch with
  //! less depth than that of Curved
  //! -   GeomFill_Curved - the style with the most rounded patch.
  //! Exceptions
  //! Standard_ConstructionError if the curves are not contiguous.
  Standard_EXPORT void Init(const Handle(Geom_BezierCurve)& C1,
                            const Handle(Geom_BezierCurve)& C2,
                            const GeomFill_FillingStyle     Type);

  //! Returns the Bezier surface resulting from the
  //! computation performed by this algorithm.
  const Handle(Geom_BezierSurface)& Surface() const;

protected:
private:
  Handle(Geom_BezierSurface) mySurface;
};

#include <GeomFill_BezierCurves.lxx>

#endif // _GeomFill_BezierCurves_HeaderFile
