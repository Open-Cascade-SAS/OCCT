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

#ifndef _StdPrs_Curve_HeaderFile
#define _StdPrs_Curve_HeaderFile

#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>

class Adaptor3d_Curve;

//! A framework to define display of lines, arcs of circles
//! and conic sections.
//! This is done with a fixed number of points, which can be modified.
class StdPrs_Curve : public Prs3d_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds to the presentation aPresentation the drawing of the curve aCurve.
  //! The aspect is defined by LineAspect in aDrawer.
  //! If drawCurve equals false the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                  const Adaptor3d_Curve&            aCurve,
                                  const occ::handle<Prs3d_Drawer>&       aDrawer,
                                  const bool            drawCurve = true);

  //! Adds to the presentation aPresentation the drawing of the curve aCurve.
  //! The aspect is defined by LineAspect in aDrawer.
  //! The drawing will be limited between the points of parameter U1 and U2.
  //! If drawCurve equals false the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                  const Adaptor3d_Curve&            aCurve,
                                  const double               U1,
                                  const double               U2,
                                  const occ::handle<Prs3d_Drawer>&       aDrawer,
                                  const bool            drawCurve = true);

  //! adds to the presentation aPresentation the drawing of the curve aCurve.
  //! The aspect is the current aspect.
  //! aDeflection is used in the circle case.
  //! Points give a sequence of curve points.
  //! If drawCurve equals false the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                  const Adaptor3d_Curve&            aCurve,
                                  const occ::handle<Prs3d_Drawer>&       aDrawer,
                                  NCollection_Sequence<gp_Pnt>&             Points,
                                  const bool            drawCurve = true);

  //! adds to the presentation aPresentation the drawing of the curve
  //! aCurve.
  //! The aspect is the current aspect.
  //! The drawing will be limited between the points of parameter
  //! U1 and U2.
  //! aDeflection is used in the circle case.
  //! Points give a sequence of curve points.
  //! If drawCurve equals false the curve will not be displayed,
  //! it is used if the curve is a part of some shape and PrimitiveArray
  //! visualization approach is activated (it is activated by default).
  Standard_EXPORT static void Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                  const Adaptor3d_Curve&            aCurve,
                                  const double               U1,
                                  const double               U2,
                                  NCollection_Sequence<gp_Pnt>&             Points,
                                  const int            aNbPoints = 30,
                                  const bool            drawCurve = true);

  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve is less than aDistance.
  Standard_EXPORT static bool Match(const double         X,
                                                const double         Y,
                                                const double         Z,
                                                const double         aDistance,
                                                const Adaptor3d_Curve&      aCurve,
                                                const occ::handle<Prs3d_Drawer>& aDrawer);

  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve is less than aDistance.
  Standard_EXPORT static bool Match(const double    X,
                                                const double    Y,
                                                const double    Z,
                                                const double    aDistance,
                                                const Adaptor3d_Curve& aCurve,
                                                const double    aDeflection,
                                                const double    aLimit,
                                                const int aNbPoints);

  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve aCurve is less than aDistance.
  //! The drawing is considered between the points
  //! of parameter U1 and U2;
  Standard_EXPORT static bool Match(const double         X,
                                                const double         Y,
                                                const double         Z,
                                                const double         aDistance,
                                                const Adaptor3d_Curve&      aCurve,
                                                const double         U1,
                                                const double         U2,
                                                const occ::handle<Prs3d_Drawer>& aDrawer);

  //! returns true if the distance between the point (X,Y,Z) and the
  //! drawing of the curve aCurve is less than aDistance.
  //! The drawing is considered between the points
  //! of parameter U1 and U2;
  Standard_EXPORT static bool Match(const double    X,
                                                const double    Y,
                                                const double    Z,
                                                const double    aDistance,
                                                const Adaptor3d_Curve& aCurve,
                                                const double    U1,
                                                const double    U2,
                                                const double    aDeflection,
                                                const int aNbPoints);
};

#endif // _StdPrs_Curve_HeaderFile
