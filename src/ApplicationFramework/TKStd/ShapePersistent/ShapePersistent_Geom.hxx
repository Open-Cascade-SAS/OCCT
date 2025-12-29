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

#ifndef _ShapePersistent_Geom_HeaderFile
#define _ShapePersistent_Geom_HeaderFile

#include <Standard_NotImplemented.hxx>
#include <Standard_NullObject.hxx>

#include <StdObjMgt_SharedObject.hxx>
#include <StdObjMgt_TransientPersistentMap.hxx>

#include <StdObject_gp_Curves.hxx>
#include <StdObject_gp_Surfaces.hxx>
#include <StdObject_gp_Trsfs.hxx>

#include <Geom_CartesianPoint.hxx>
#include <Geom_Direction.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_Surface.hxx>

class ShapePersistent_Geom : public StdObjMgt_SharedObject
{
public:
  class Geometry : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT void Read(StdObjMgt_ReadData& theReadData) override;
    //! Write persistent data to a file.
    Standard_EXPORT void Write(StdObjMgt_WriteData& theWriteData) const override;
    //! Gets persistent objects
    Standard_EXPORT void PChildren(SequenceOfPersistent& theChildren) const override;

    //! Returns persistent type name
    const char* PName() const override { return "PGeom_Geometry"; }
  };

protected:
  template <class Transient>
  struct geometryBase : public DelayedBase<Geometry, Transient>
  {
    //! Write persistent data to a file.
    void Write(StdObjMgt_WriteData&) const override
    {
      Standard_NotImplemented::Raise("ShapePersistent_Geom::geometryBase::Write - not implemented");
    }

    //! Gets persistent child objects
    void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const override {}

    //! Returns persistent type name
    const char* PName() const override
    {
      Standard_NotImplemented::Raise("ShapePersistent_Geom::geometryBase::PName - not implemented");
      return "";
    }
  };

  template <class Base, class PData>
  class subBase : public Base
  {
  public:
    //! Read persistent data from a file.
    void Read(StdObjMgt_ReadData& theReadData) override { PData().Read(theReadData); }

    //! Write persistent data to a file.
    void Write(StdObjMgt_WriteData& theWriteData) const override { PData().Write(theWriteData); }

    //! Gets persistent child objects
    void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const override
    {
      Standard_NotImplemented::Raise("ShapePersistent_Geom::subBase::PChildren - not implemented");
    }

    //! Returns persistent type name
    const char* PName() const override
    {
      Standard_NotImplemented::Raise("ShapePersistent_Geom::subBase::PName - not implemented");
      return "";
    }
  };

  template <class Base, class GpData>
  struct subBase_gp : public Base
  {
  public:
    //! Read persistent data from a file.
    void Read(StdObjMgt_ReadData&) override {}

    //! Write persistent data to a file.
    void Write(StdObjMgt_WriteData&) const override {}

    //! Gets persistent child objects
    void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const override {}

    //! Returns persistent type name
    const char* PName() const override
    {
      Standard_NotImplemented::Raise("ShapePersistent_Geom::subBase_gp::PName - not implemented");
      return "";
    }
  };

  template <class Base>
  struct subBase_empty : Base
  {
    //! Returns persistent type name
    const char* PName() const override
    {
      Standard_NotImplemented::Raise(
        "ShapePersistent_Geom::subBase_empty::PName - not implemented");
      return "";
    }
  };

  template <class Base, class Target, class Data = void>
  class instance : public Base
  {
  public:
    //! Read persistent data from a file.
    void Read(StdObjMgt_ReadData& theReadData) override
    {
      Data aData;
      theReadData >> aData;
      this->myTransient = new Target(aData);
    }

    //! Gets persistent child objects
    void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const override {}

    //! Write persistent data to a file.
    void Write(StdObjMgt_WriteData&) const override
    {
      Standard_NotImplemented::Raise("ShapePersistent_Geom::instance::Write - not implemented");
    }

    //! Returns persistent type name
    const char* PName() const override
    {
      Standard_NotImplemented::Raise("ShapePersistent_Geom::instance::PName - not implemented");
      return "";
    }
  };

private:
  typedef geometryBase<Geom_Geometry> basic;

public:
  typedef subBase_empty<basic>                         Point;
  typedef instance<Point, Geom_CartesianPoint, gp_Pnt> CartesianPoint;

  typedef subBase_gp<basic, gp_Vec>                          Vector;
  typedef instance<Vector, Geom_Direction, gp_Dir>           Direction;
  typedef instance<Vector, Geom_VectorWithMagnitude, gp_Vec> VectorWithMagnitude;

  typedef subBase_gp<basic, gp_Ax1>                            AxisPlacement;
  typedef instance<AxisPlacement, Geom_Axis1Placement, gp_Ax1> Axis1Placement;
  typedef instance<AxisPlacement, Geom_Axis2Placement>         Axis2Placement;

  typedef instance<SharedBase<Geom_Transformation>, Geom_Transformation, gp_Trsf> Transformation;

  typedef geometryBase<Geom_Curve>   Curve;
  typedef geometryBase<Geom_Surface> Surface;

public:
  //! Create a persistent object for a curve
  Standard_EXPORT static occ::handle<Curve> Translate(
    const occ::handle<Geom_Curve>& theCurve,
    NCollection_DataMap<occ::handle<Standard_Transient>, occ::handle<StdObjMgt_Persistent>>&
      theMap);
  //! Create a persistent object for a curve
  Standard_EXPORT static occ::handle<Surface> Translate(
    const occ::handle<Geom_Surface>& theSurf,
    NCollection_DataMap<occ::handle<Standard_Transient>, occ::handle<StdObjMgt_Persistent>>&
      theMap);
};

//=======================================================================
// Point
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::subBase_empty<ShapePersistent_Geom::basic>::PName() const
{
  return "PGeom_Point";
}

//=======================================================================
// CartesianPoint
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::
  instance<ShapePersistent_Geom::Point, Geom_CartesianPoint, gp_Pnt>::PName() const
{
  return "PGeom_CartesianPoint";
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom::Point,
                                           Geom_CartesianPoint,
                                           gp_Pnt>::Write(StdObjMgt_WriteData& theWriteData) const
{
  occ::handle<Geom_CartesianPoint> aMyGeom = occ::down_cast<Geom_CartesianPoint>(myTransient);
  theWriteData << aMyGeom->Pnt();
}

//=======================================================================
// Vector
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::basic, gp_Vec>::PName()
  const
{
  return "PGeom_Vector";
}

//=======================================================================
// Direction
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::
  instance<ShapePersistent_Geom::Direction, Geom_Direction, gp_Dir>::PName() const
{
  return "PGeom_Direction";
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom::Direction,
                                           Geom_Direction,
                                           gp_Dir>::Write(StdObjMgt_WriteData& theWriteData) const
{
  occ::handle<Geom_Direction> aMyGeom = occ::down_cast<Geom_Direction>(myTransient);
  theWriteData << aMyGeom->Dir();
}

//=======================================================================
// VectorWithMagnitude
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::instance<ShapePersistent_Geom::VectorWithMagnitude,
                                                  Geom_VectorWithMagnitude,
                                                  gp_Vec>::PName() const
{
  return "PGeom_VectorWithMagnitude";
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom::VectorWithMagnitude,
                                           Geom_VectorWithMagnitude,
                                           gp_Vec>::Write(StdObjMgt_WriteData& theWriteData) const
{
  occ::handle<Geom_VectorWithMagnitude> aMyGeom =
    occ::down_cast<Geom_VectorWithMagnitude>(myTransient);
  theWriteData << aMyGeom->Vec();
}

//=======================================================================
// AxisPlacement
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::subBase_gp<ShapePersistent_Geom::basic, gp_Ax1>::PName()
  const
{
  return "PGeom_AxisPlacement";
}

//=======================================================================
// Axis1Placement
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::
  instance<ShapePersistent_Geom::Axis1Placement, Geom_Axis1Placement, gp_Ax1>::PName() const
{
  return "PGeom_Axis1Placement";
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom::Axis1Placement,
                                           Geom_Axis1Placement,
                                           gp_Ax1>::Write(StdObjMgt_WriteData& theWriteData) const
{
  occ::handle<Geom_Axis1Placement> aMyGeom = occ::down_cast<Geom_Axis1Placement>(myTransient);
  write(theWriteData, aMyGeom->Ax1());
}

//=======================================================================
// Axis2Placement
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::instance<ShapePersistent_Geom::AxisPlacement,
                                                  Geom_Axis2Placement>::PName() const
{
  return "PGeom_Axis2Placement";
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom::AxisPlacement,
                                           Geom_Axis2Placement>::Read(StdObjMgt_ReadData&
                                                                        theReadData)
{
  gp_Ax1 anAxis;
  gp_Dir anXDirection;

  theReadData >> anAxis >> anXDirection;

  myTransient = new Geom_Axis2Placement(anAxis.Location(), anAxis.Direction(), anXDirection);
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom::AxisPlacement,
                                           Geom_Axis2Placement>::Write(StdObjMgt_WriteData&
                                                                         theWriteData) const
{
  occ::handle<Geom_Axis2Placement> aMyGeom      = occ::down_cast<Geom_Axis2Placement>(myTransient);
  const gp_Ax1&                    anAxis       = aMyGeom->Axis();
  const gp_Dir&                    anXDirection = aMyGeom->Direction();
  write(theWriteData, anAxis) << anXDirection;
}

//=======================================================================
// Transformation
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::
  instance<ShapePersistent_Geom::Transformation, Geom_Transformation, gp_Trsf>::PName() const
{
  return "PGeom_Transformation";
}

template <>
inline void ShapePersistent_Geom::instance<ShapePersistent_Geom::Transformation,
                                           Geom_Transformation,
                                           gp_Trsf>::Write(StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myTransient->Trsf();
}

//=======================================================================
// Geometry
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::geometryBase<Geom_Geometry>::PName() const
{
  return "PGeom_Geometry";
}

//=======================================================================
// Curve
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::geometryBase<Geom_Curve>::PName() const
{
  return "PGeom_Curve";
}

//=======================================================================
// Surface
//=======================================================================

template <>
inline const char* ShapePersistent_Geom::geometryBase<Geom_Surface>::PName() const
{
  return "PGeom_Surface";
}

#endif
