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

#ifndef _Geom_Direction_HeaderFile
#define _Geom_Direction_HeaderFile

#include <Standard.hxx>

#include <Geom_Vector.hxx>
#include <Standard_Real.hxx>
class gp_Dir;
class gp_Trsf;
class Geom_Geometry;

class Geom_Direction;
DEFINE_STANDARD_HANDLE(Geom_Direction, Geom_Vector)

//! The class Direction specifies a vector that is never null.
//! It is a unit vector.
class Geom_Direction : public Geom_Vector
{

public:
  //! Creates a unit vector with it 3 cartesian coordinates.
  //!
  //! Raised if Sqrt( X*X + Y*Y + Z*Z) <= Resolution from gp.
  Standard_EXPORT Geom_Direction(const Standard_Real X,
                                 const Standard_Real Y,
                                 const Standard_Real Z);

  //! Creates a transient copy of <me>.
  Standard_EXPORT Geom_Direction(const gp_Dir& V);

  //! Sets <me> to X,Y,Z coordinates.
  //!
  //! Raised if Sqrt( X*X + Y*Y + Z*Z) <= Resolution from gp.
  Standard_EXPORT void SetCoord(const Standard_Real X,
                                const Standard_Real Y,
                                const Standard_Real Z);

  //! Converts the gp_Dir unit vector V into this unit vector.
  Standard_EXPORT void SetDir(const gp_Dir& V);

  //! Changes the X coordinate of <me>.
  //!
  //! Raised if Sqrt( X*X + Y*Y + Z*Z) <= Resolution from gp.
  Standard_EXPORT void SetX(const Standard_Real X);

  //! Changes the Y coordinate of <me>.
  //!
  //! Raised if Sqrt( X*X + Y*Y + Z*Z) <= Resolution from gp.
  Standard_EXPORT void SetY(const Standard_Real Y);

  //! Changes the Z coordinate of <me>.
  //!
  //! Raised if Sqrt( X*X + Y*Y + Z*Z) <= Resolution from gp.
  Standard_EXPORT void SetZ(const Standard_Real Z);

  //! Returns the non transient direction with the same
  //! coordinates as <me>.
  Standard_EXPORT gp_Dir Dir() const;

  //! returns 1.0 which is the magnitude of any unit vector.
  Standard_EXPORT Standard_Real Magnitude() const Standard_OVERRIDE;

  //! returns 1.0 which is the square magnitude of any unit vector.
  Standard_EXPORT Standard_Real SquareMagnitude() const Standard_OVERRIDE;

  //! Computes the cross product between <me> and <Other>.
  //!
  //! Raised if the two vectors are parallel because it is
  //! not possible to have a direction with null length.
  Standard_EXPORT void Cross(const Handle(Geom_Vector)& Other) Standard_OVERRIDE;

  //! Computes the triple vector product  <me> ^(V1 ^ V2).
  //!
  //! Raised if V1 and V2 are parallel or <me> and (V1 ^ V2) are
  //! parallel
  Standard_EXPORT void CrossCross(const Handle(Geom_Vector)& V1,
                                  const Handle(Geom_Vector)& V2) Standard_OVERRIDE;

  //! Computes the cross product between <me> and <Other>.
  //! A new direction is returned.
  //!
  //! Raised if the two vectors are parallel because it is
  //! not possible to have a direction with null length.
  Standard_EXPORT Handle(Geom_Vector) Crossed(const Handle(Geom_Vector)& Other) const
    Standard_OVERRIDE;

  //! Computes the triple vector product <me> ^(V1 ^ V2).
  //!
  //! Raised if V1 and V2 are parallel or <me> and (V1 ^ V2) are
  //! parallel
  Standard_EXPORT Handle(Geom_Vector) CrossCrossed(const Handle(Geom_Vector)& V1,
                                                   const Handle(Geom_Vector)& V2) const
    Standard_OVERRIDE;

  //! Applies the transformation T to this unit vector, then normalizes it.
  Standard_EXPORT void Transform(const gp_Trsf& T) Standard_OVERRIDE;

  //! Creates a new object which is a copy of this unit vector.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom_Direction, Geom_Vector)

protected:
private:
};

#endif // _Geom_Direction_HeaderFile
