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

class ShapePersistent_Geom2d : public ShapePersistent_Geom
{
  typedef geometryBase<Geom2d_Geometry> basic;

public:
  typedef ShapePersistent_Geom::Geometry Geometry;

  typedef subBase_empty<basic>                             Point;
  typedef instance<Point, Geom2d_CartesianPoint, gp_Pnt2d> CartesianPoint;

  typedef subBase_gp<basic, gp_Vec2d>                            Vector;
  typedef instance<Vector, Geom2d_Direction, gp_Dir2d>           Direction;
  typedef instance<Vector, Geom2d_VectorWithMagnitude, gp_Vec2d> VectorWithMagnitude;

  typedef instance<basic, Geom2d_AxisPlacement, gp_Ax2d> AxisPlacement;

  typedef instance<SharedBase<Geom2d_Transformation>, Geom2d_Transformation, gp_Trsf2d>
    Transformation;

  typedef geometryBase<Geom2d_Curve> Curve;

public:
  //! Create a persistent object for a curve
  Standard_EXPORT static Handle(Curve) Translate(const Handle(Geom2d_Curve)&       theCurve,
                                                 StdObjMgt_TransientPersistentMap& theMap);
};

//=======================================================================
// Geometry
//=======================================================================
template <>
inline Standard_CString ShapePersistent_Geom::geometryBase<Geom2d_Geometry>::PName() const
{
  return "PGeom2d_Geometry";
}

//=======================================================================
// Point
//=======================================================================
template <>
inline Standard_CString ShapePersistent_Geom::subBase_empty<
  ShapePersistent_Geom2d::geometryBase<Geom2d_Geometry>>::PName() const
{
  return "PGeom2d_Point";
}

//=======================================================================
// CartesianPoint
//=======================================================================
template <>
inline Standard_CString ShapePersistent_Geom::
  instance<ShapePersistent_Geom2d::Point, Geom2d_CartesianPoint, gp_Pnt2d>::PName() const
{
  return "PGeom2d_CartesianPoint";
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Point,
                                           Geom2d_CartesianPoint,
                                           gp_Pnt2d>::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_CartesianPoint) aMyGeom = Handle(Geom2d_CartesianPoint)::DownCast(myTransient);
  theWriteData << aMyGeom->Pnt2d();
}

//=======================================================================
// Direction
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::
  instance<ShapePersistent_Geom2d::Direction, Geom2d_Direction, gp_Dir2d>::PName() const;

template <>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Direction, Geom2d_Direction, gp_Dir2d>::
  Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// VectorWithMagnitude
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom2d::VectorWithMagnitude,
                                                Geom2d_VectorWithMagnitude,
                                                gp_Vec2d>::PName() const;

template <>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::VectorWithMagnitude,
                                    Geom2d_VectorWithMagnitude,
                                    gp_Vec2d>::Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// AxisPlacement
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::
  instance<ShapePersistent_Geom2d::AxisPlacement, Geom2d_AxisPlacement, gp_Ax2d>::PName() const;

template <>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::AxisPlacement,
                                    Geom2d_AxisPlacement,
                                    gp_Ax2d>::Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// Transformation
//=======================================================================
template <>
Standard_CString ShapePersistent_Geom::
  instance<ShapePersistent_Geom2d::Transformation, Geom2d_Transformation, gp_Trsf2d>::PName() const;

template <>
void ShapePersistent_Geom::
  instance<ShapePersistent_Geom2d::Transformation, Geom2d_Transformation, gp_Trsf2d>::PChildren(
    StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const;

template <>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Transformation,
                                    Geom2d_Transformation,
                                    gp_Trsf2d>::Write(StdObjMgt_WriteData& theWriteData) const;

//=======================================================================
// Curve
//=======================================================================
template <>
inline Standard_CString ShapePersistent_Geom::geometryBase<Geom2d_Curve>::PName() const
{
  return "PGeom2d_Curve";
}

#endif
