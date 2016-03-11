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

#include <StdObjMgt_SharedObject.hxx>

#include <StdObject_gp_Vectors.hxx>
#include <StdObject_gp_Axes.hxx>
#include <StdObject_gp_Curves.hxx>
#include <StdObject_gp_Surfaces.hxx>
#include <StdObject_gp_Trsfs.hxx>

#include <Geom_CartesianPoint.hxx>
#include <Geom_Direction.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>


class ShapePersistent_Geom : protected StdObjMgt_SharedObject
{
public:
  class Geometry : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);
  };

protected:
  template <class Transient>
  struct geometryBase : DelayedBase<Geometry, Transient> {};

  template <class Base, class PData>
  class subBase : public Base
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData)
      { PData().Read (theReadData); }
  };

  template <class Base, class GpData>
  struct subBase_gp : public Base
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData)
    {
      GpData aData;
      theReadData >> aData;
    }
  };

  template <class Base>
  struct subBase_empty : Base {};

  template <class Base, class Target, class Data = void>
  class instance : public Base
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData)
    {
      Data aData;
      theReadData >> aData;
      this->myTransient = new Target (aData);
    }
  };

private:
  typedef geometryBase<Geom_Geometry> basic;

public:
  typedef subBase_empty<basic>                                 Point;
  typedef instance<Point, Geom_CartesianPoint, gp_Pnt>         CartesianPoint;

  typedef subBase_gp<basic, gp_Vec>                            Vector;
  typedef instance<Vector, Geom_Direction          , gp_Dir>   Direction;
  typedef instance<Vector, Geom_VectorWithMagnitude, gp_Vec>   VectorWithMagnitude;

  typedef subBase_gp<basic, gp_Ax1>                            AxisPlacement;
  typedef instance<AxisPlacement, Geom_Axis1Placement, gp_Ax1> Axis1Placement;
  typedef instance<AxisPlacement, Geom_Axis2Placement>         Axis2Placement;

  typedef instance <SharedBase<Geom_Transformation>,
                    Geom_Transformation,
                    gp_Trsf>                                   Transformation;

  typedef geometryBase<Geom_Curve>                             Curve;
  typedef geometryBase<Geom_Surface>                           Surface;
};

template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::AxisPlacement,
                                    Geom_Axis2Placement>
  ::Read (StdObjMgt_ReadData& theReadData);

#endif
