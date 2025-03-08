// Created on: 1993-03-10
// Created by: JCV
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

#ifndef _Geom_CartesianPoint_HeaderFile
#define _Geom_CartesianPoint_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt.hxx>
#include <Geom_Point.hxx>
#include <Standard_Real.hxx>
class gp_Trsf;
class Geom_Geometry;

class Geom_CartesianPoint;
DEFINE_STANDARD_HANDLE(Geom_CartesianPoint, Geom_Point)

//! Describes a point in 3D space. A
//! Geom_CartesianPoint is defined by a gp_Pnt point,
//! with its three Cartesian coordinates X, Y and Z.
class Geom_CartesianPoint : public Geom_Point
{

public:
  //! Returns a transient copy of P.
  Standard_EXPORT Geom_CartesianPoint(const gp_Pnt& P);

  //! Constructs a point defined by its three Cartesian coordinates X, Y and Z.
  Standard_EXPORT Geom_CartesianPoint(const Standard_Real X,
                                      const Standard_Real Y,
                                      const Standard_Real Z);

  //! Assigns the coordinates X, Y and Z to this point.
  Standard_EXPORT void SetCoord(const Standard_Real X,
                                const Standard_Real Y,
                                const Standard_Real Z);

  //! Set <me> to P.X(), P.Y(), P.Z() coordinates.
  Standard_EXPORT void SetPnt(const gp_Pnt& P);

  //! Changes the X coordinate of me.
  Standard_EXPORT void SetX(const Standard_Real X);

  //! Changes the Y coordinate of me.
  Standard_EXPORT void SetY(const Standard_Real Y);

  //! Changes the Z coordinate of me.
  Standard_EXPORT void SetZ(const Standard_Real Z);

  //! Returns the coordinates of <me>.
  Standard_EXPORT void Coord(Standard_Real& X,
                             Standard_Real& Y,
                             Standard_Real& Z) const Standard_OVERRIDE;

  //! Returns a non transient cartesian point with
  //! the same coordinates as <me>.
  Standard_EXPORT gp_Pnt Pnt() const Standard_OVERRIDE;

  //! Returns the X coordinate of <me>.
  Standard_EXPORT Standard_Real X() const Standard_OVERRIDE;

  //! Returns the Y coordinate of <me>.
  Standard_EXPORT Standard_Real Y() const Standard_OVERRIDE;

  //! Returns the Z coordinate of <me>.
  Standard_EXPORT Standard_Real Z() const Standard_OVERRIDE;

  //! Applies the transformation T to this point.
  Standard_EXPORT void Transform(const gp_Trsf& T) Standard_OVERRIDE;

  //! Creates a new object which is a copy of this point.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom_CartesianPoint, Geom_Point)

protected:
private:
  gp_Pnt gpPnt;
};

#endif // _Geom_CartesianPoint_HeaderFile
