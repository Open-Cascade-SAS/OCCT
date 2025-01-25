// Created on: 1998-01-27
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

#ifndef _DsgPrs_EqualDistancePresentation_HeaderFile
#define _DsgPrs_EqualDistancePresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <Prs3d_Presentation.hxx>

class gp_Pnt;
class Geom_Plane;
class gp_Dir;
class gp_Circ;

//! A framework to display equal distances between shapes and a given plane.
//! The distance is the length of a projection from the shape to the plane.
//! These distances are used to compare two shapes by this vector alone.
class DsgPrs_EqualDistancePresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds the points Point1, Point2, Point3 Point4, and the
  //! plane Plane to the presentation object aPresentation.
  //! The display attributes of these elements is defined by the attribute manager aDrawer.
  //! The distance is the length of a projection from the shape to the plane.
  //! These distances are used to compare two shapes by this vector alone.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& aPresentation,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const gp_Pnt&                     Point1,
                                  const gp_Pnt&                     Point2,
                                  const gp_Pnt&                     Point3,
                                  const gp_Pnt&                     Point4,
                                  const Handle(Geom_Plane)&         Plane);

  //! is used for presentation of interval between
  //! two lines or two points or between a line and a point.
  Standard_EXPORT static void AddInterval(const Handle(Prs3d_Presentation)& aPresentation,
                                          const Handle(Prs3d_Drawer)&       aDrawer,
                                          const gp_Pnt&                     aPoint1,
                                          const gp_Pnt&                     aPoint2,
                                          const gp_Dir&                     aDir,
                                          const gp_Pnt&                     aPosition,
                                          const DsgPrs_ArrowSide            anArrowSide,
                                          gp_Pnt&                           anExtremePnt1,
                                          gp_Pnt&                           anExtremePnt2);

  //! is used for presentation of interval between two arcs.
  //! One of arcs can have a zero radius.
  Standard_EXPORT static void AddIntervalBetweenTwoArcs(
    const Handle(Prs3d_Presentation)& aPresentation,
    const Handle(Prs3d_Drawer)&       aDrawer,
    const gp_Circ&                    aCircle1,
    const gp_Circ&                    aCircle2,
    const gp_Pnt&                     aPoint1,
    const gp_Pnt&                     aPoint2,
    const gp_Pnt&                     aPoint3,
    const gp_Pnt&                     aPoint4,
    const DsgPrs_ArrowSide            anArrowSide);

protected:
private:
};

#endif // _DsgPrs_EqualDistancePresentation_HeaderFile
