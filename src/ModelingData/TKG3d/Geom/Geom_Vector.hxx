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

#ifndef _Geom_Vector_HeaderFile
#define _Geom_Vector_HeaderFile

#include <Standard.hxx>

#include <gp_Vec.hxx>
#include <Geom_Geometry.hxx>
#include <Standard_Real.hxx>

//! The abstract class Vector describes the common
//! behavior of vectors in 3D space.
//! The Geom package provides two concrete classes of
//! vectors: Geom_Direction (unit vector) and Geom_VectorWithMagnitude.
class Geom_Vector : public Geom_Geometry
{

public:
  //! Reverses the vector <me>.
  Standard_EXPORT void Reverse();

  //! Returns a copy of <me> reversed.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom_Vector> Reversed() const;

  //! Computes the angular value, in radians, between this
  //! vector and vector Other. The result is a value between 0 and Pi.
  //! Exceptions
  //! gp_VectorWithNullMagnitude if:
  //! - the magnitude of this vector is less than or equal to
  //! gp::Resolution(), or
  //! - the magnitude of vector Other is less than or equal
  //! to gp::Resolution().
  Standard_EXPORT double Angle(const occ::handle<Geom_Vector>& Other) const;

  //! Computes the angular value, in radians, between this
  //! vector and vector Other. The result is a value
  //! between -Pi and Pi. The vector VRef defines the
  //! positive sense of rotation: the angular value is positive
  //! if the cross product this ^ Other has the same
  //! orientation as VRef (in relation to the plane defined
  //! by this vector and vector Other). Otherwise, it is negative.
  //! Exceptions
  //! Standard_DomainError if this vector, vector Other
  //! and vector VRef are coplanar, except if this vector
  //! and vector Other are parallel.
  //! gp_VectorWithNullMagnitude if the magnitude of
  //! this vector, vector Other or vector VRef is less than
  //! or equal to gp::Resolution().
  Standard_EXPORT double AngleWithRef(const occ::handle<Geom_Vector>& Other,
                                             const occ::handle<Geom_Vector>& VRef) const;

  //! Returns the coordinates X, Y and Z of this vector.
  Standard_EXPORT void Coord(double& X, double& Y, double& Z) const;

  //! Returns the Magnitude of <me>.
  Standard_EXPORT virtual double Magnitude() const = 0;

  //! Returns the square magnitude of <me>.
  Standard_EXPORT virtual double SquareMagnitude() const = 0;

  //! Returns the X coordinate of <me>.
  Standard_EXPORT double X() const;

  //! Returns the Y coordinate of <me>.
  Standard_EXPORT double Y() const;

  //! Returns the Z coordinate of <me>.
  Standard_EXPORT double Z() const;

  //! Computes the cross product between <me> and <Other>.
  //!
  //! Raised if <me> is a "Direction" and if <me> and <Other>
  //! are parallel because it is not possible to build a
  //! "Direction" with null length.
  Standard_EXPORT virtual void Cross(const occ::handle<Geom_Vector>& Other) = 0;

  //! Computes the cross product between <me> and <Other>.
  //! A new direction is returned.
  //!
  //! Raised if <me> is a "Direction" and if the two vectors
  //! are parallel because it is not possible to create a
  //! "Direction" with null length.
  Standard_EXPORT virtual occ::handle<Geom_Vector> Crossed(const occ::handle<Geom_Vector>& Other) const = 0;

  //! Computes the triple vector product <me> ^(V1 ^ V2).
  //!
  //! Raised if <me> is a "Direction" and if V1 and V2 are parallel
  //! or <me> and (V1 ^ V2) are parallel
  Standard_EXPORT virtual void CrossCross(const occ::handle<Geom_Vector>& V1,
                                          const occ::handle<Geom_Vector>& V2) = 0;

  //! Computes the triple vector product <me> ^(V1 ^ V2).
  //!
  //! Raised if <me> is a direction and if V1 and V2 are
  //! parallel or <me> and (V1 ^ V2) are parallel
  Standard_EXPORT virtual occ::handle<Geom_Vector> CrossCrossed(const occ::handle<Geom_Vector>& V1,
                                                           const occ::handle<Geom_Vector>& V2) const = 0;

  //! Computes the scalar product of this vector and vector Other.
  Standard_EXPORT double Dot(const occ::handle<Geom_Vector>& Other) const;

  //! Computes the triple scalar product. Returns me . (V1 ^ V2)
  Standard_EXPORT double DotCross(const occ::handle<Geom_Vector>& V1,
                                         const occ::handle<Geom_Vector>& V2) const;

  //! Converts this vector into a gp_Vec vector.
  Standard_EXPORT const gp_Vec& Vec() const;

  DEFINE_STANDARD_RTTIEXT(Geom_Vector, Geom_Geometry)

protected:
  gp_Vec gpVec;

};

#endif // _Geom_Vector_HeaderFile
