// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _ShapePersistent_Geom2d_HeaderFile
#define _ShapePersistent_Geom2d_HeaderFile

#include <ShapePersistent_Geom.hxx>

#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Direction.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <Geom2d_AxisPlacement.hxx>
#include <Geom2d_Transformation.hxx>
#include <Geom2d_Curve.hxx>


class ShapePersistent_Geom2d : protected ShapePersistent_Geom
{
  typedef geometryBase<Geom2d_Geometry> basic;

public:
  typedef ShapePersistent_Geom::Geometry                         Geometry;

  typedef subBase_empty<basic>                                   Point;
  typedef instance<Point, Geom2d_CartesianPoint, gp_Pnt2d>       CartesianPoint;

  typedef subBase_gp<basic, gp_Vec2d>                            Vector;
  typedef instance<Vector, Geom2d_Direction          , gp_Dir2d> Direction;
  typedef instance<Vector, Geom2d_VectorWithMagnitude, gp_Vec2d> VectorWithMagnitude;

  typedef instance<basic, Geom2d_AxisPlacement, gp_Ax2d>         AxisPlacement;

  typedef instance <SharedBase<Geom2d_Transformation>,
                    Geom2d_Transformation,
                    gp_Trsf2d>                                   Transformation;

  typedef geometryBase<Geom2d_Curve>                             Curve;
};

#endif
