// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Standard_NullObject.hxx>

#include <ShapePersistent_Geom2d.hxx>
#include <ShapePersistent_Geom2d_Curve.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>

//=======================================================================
// Geometry
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::geometryBase<Geom2d_Geometry>
  ::PName() const { return "PGeom2d_Geometry"; }

//=======================================================================
// Point
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::subBase_empty<ShapePersistent_Geom2d::geometryBase<Geom2d_Geometry> >
  ::PName() const { return "PGeom2d_Point"; }

//=======================================================================
// CartesianPoint
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Point,
                                                  Geom2d_CartesianPoint,
                                                  gp_Pnt2d>
  ::PName() const { return "PGeom2d_CartesianPoint"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Point,
                                      Geom2d_CartesianPoint,
                                      gp_Pnt2d>
::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_CartesianPoint) aMyGeom =
    Handle(Geom2d_CartesianPoint)::DownCast(myTransient);
  theWriteData << aMyGeom->Pnt2d();
}

//=======================================================================
// Direction
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Direction,
                                                  Geom2d_Direction,
                                                  gp_Dir2d>
  ::PName() const { return "PGeom2d_Direction"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Direction,
                                    Geom2d_Direction,
                                    gp_Dir2d>
::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_Direction) aMyGeom =
    Handle(Geom2d_Direction)::DownCast(myTransient);
  theWriteData << aMyGeom->Dir2d();
}

//=======================================================================
// VectorWithMagnitude
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom2d::VectorWithMagnitude,
                                                  Geom2d_VectorWithMagnitude,
                                                  gp_Vec2d>
  ::PName() const { return "PGeom2d_VectorWithMagnitude"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::VectorWithMagnitude,
                                      Geom2d_VectorWithMagnitude,
                                      gp_Vec2d>
::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_VectorWithMagnitude) aMyGeom =
    Handle(Geom2d_VectorWithMagnitude)::DownCast(myTransient);
  theWriteData << aMyGeom->Vec2d();
}

//=======================================================================
// AxisPlacement
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom2d::AxisPlacement,
                                                  Geom2d_AxisPlacement,
                                                  gp_Ax2d>
  ::PName() const { return "PGeom2d_AxisPlacement"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::AxisPlacement,
                                      Geom2d_AxisPlacement,
                                      gp_Ax2d>
::Write(StdObjMgt_WriteData& theWriteData) const
{
  Handle(Geom2d_AxisPlacement) aMyGeom =
    Handle(Geom2d_AxisPlacement)::DownCast(myTransient);
  write(theWriteData, aMyGeom->Ax2d());
}

//=======================================================================
// Transformation
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Transformation,
                                                  Geom2d_Transformation,
                                                  gp_Trsf2d>
  ::PName() const { return "PGeom2d_Transformation"; }

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom2d::Transformation,
                                    Geom2d_Transformation,
                                    gp_Trsf2d>
  ::PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const
{
}

template<>
void ShapePersistent_Geom2d::instance<ShapePersistent_Geom2d::Transformation,
                                      Geom2d_Transformation,
                                      gp_Trsf2d>
::Write(StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myTransient->Trsf2d();
}

//=======================================================================
// Curve
//=======================================================================
template<>
Standard_CString ShapePersistent_Geom::geometryBase<Geom2d_Curve>
  ::PName() const { return "PGeom2d_Curve"; }

Handle(ShapePersistent_Geom2d::Curve)
ShapePersistent_Geom2d::Translate(const Handle(Geom2d_Curve)& theCurve,
                                  StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ShapePersistent_Geom2d::Curve) aPC;
  if (!theCurve.IsNull())
  {
    if (theMap.IsBound(theCurve))
      aPC = Handle(ShapePersistent_Geom2d::Curve)::DownCast(theMap.Find(theCurve));
    else
    {
      Handle(Standard_Type) aCT = theCurve->DynamicType();
      if (aCT == STANDARD_TYPE(Geom2d_Line)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_Line)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_Circle)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_Circle)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_Ellipse)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_Ellipse)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_Hyperbola)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_Hyperbola)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_Parabola)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_Parabola)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_BezierCurve)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_BezierCurve)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_BSplineCurve)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_BSplineCurve)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_TrimmedCurve)::DownCast(theCurve), theMap);
      }
      else if (aCT == STANDARD_TYPE(Geom2d_OffsetCurve)) {
        aPC = ShapePersistent_Geom2d_Curve::Translate(Handle(Geom2d_OffsetCurve)::DownCast(theCurve), theMap);
      }
      else {
        Standard_NullObject::Raise("No mapping for the current Transient Curve");
      }
      theMap.Bind(theCurve, aPC);
    }
  }
  return aPC;
}
