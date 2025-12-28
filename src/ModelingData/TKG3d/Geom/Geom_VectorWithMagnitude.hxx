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

#ifndef _Geom_VectorWithMagnitude_HeaderFile
#define _Geom_VectorWithMagnitude_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_Vector.hxx>
class gp_Vec;
class gp_Pnt;
class gp_Trsf;
class Geom_Geometry;

//! Defines a vector with magnitude.
//! A vector with magnitude can have a zero length.
class Geom_VectorWithMagnitude : public Geom_Vector
{

public:
  //! Creates a transient copy of V.
  Standard_EXPORT Geom_VectorWithMagnitude(const gp_Vec& V);

  //! Creates a vector with three cartesian coordinates.
  Standard_EXPORT Geom_VectorWithMagnitude(const double X, const double Y, const double Z);

  //! Creates a vector from the point P1 to the point P2.
  //! The magnitude of the vector is the distance between P1 and P2
  Standard_EXPORT Geom_VectorWithMagnitude(const gp_Pnt& P1, const gp_Pnt& P2);

  //! Assigns the values X, Y and Z to the coordinates of this vector.
  Standard_EXPORT void SetCoord(const double X, const double Y, const double Z);

  //! Converts the gp_Vec vector V into this vector.
  Standard_EXPORT void SetVec(const gp_Vec& V);

  //! Changes the X coordinate of <me>.
  Standard_EXPORT void SetX(const double X);

  //! Changes the Y coordinate of <me>
  Standard_EXPORT void SetY(const double Y);

  //! Changes the Z coordinate of <me>.
  Standard_EXPORT void SetZ(const double Z);

  //! Returns the magnitude of <me>.
  Standard_EXPORT double Magnitude() const override;

  //! Returns the square magnitude of <me>.
  Standard_EXPORT double SquareMagnitude() const override;

  //! Adds the Vector Other to <me>.
  Standard_EXPORT void Add(const occ::handle<Geom_Vector>& Other);

  //! Adds the vector Other to <me>.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom_VectorWithMagnitude> Added(
    const occ::handle<Geom_Vector>& Other) const;

  //! Computes the cross product between <me> and Other
  //! <me> ^ Other.
  Standard_EXPORT void Cross(const occ::handle<Geom_Vector>& Other) override;

  //! Computes the cross product between <me> and Other
  //! <me> ^ Other. A new vector is returned.
  Standard_EXPORT occ::handle<Geom_Vector> Crossed(
    const occ::handle<Geom_Vector>& Other) const override;

  //! Computes the triple vector product <me> ^ (V1 ^ V2).
  Standard_EXPORT void CrossCross(const occ::handle<Geom_Vector>& V1,
                                  const occ::handle<Geom_Vector>& V2) override;

  //! Computes the triple vector product <me> ^ (V1 ^ V2).
  //! A new vector is returned.
  Standard_EXPORT occ::handle<Geom_Vector> CrossCrossed(
    const occ::handle<Geom_Vector>& V1,
    const occ::handle<Geom_Vector>& V2) const override;

  //! Divides <me> by a scalar.
  Standard_EXPORT void Divide(const double Scalar);

  //! Divides <me> by a scalar. A new vector is returned.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom_VectorWithMagnitude> Divided(
    const double Scalar) const;

  //! Computes the product of the vector <me> by a scalar.
  //! A new vector is returned.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom_VectorWithMagnitude> Multiplied(
    const double Scalar) const;

  //! Computes the product of the vector <me> by a scalar.
  Standard_EXPORT void Multiply(const double Scalar);

  //! Normalizes <me>.
  //!
  //! Raised if the magnitude of the vector is lower or equal to
  //! Resolution from package gp.
  Standard_EXPORT void Normalize();

  //! Returns a copy of <me> Normalized.
  //!
  //! Raised if the magnitude of the vector is lower or equal to
  //! Resolution from package gp.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom_VectorWithMagnitude> Normalized() const;

  //! Subtracts the Vector Other to <me>.
  Standard_EXPORT void Subtract(const occ::handle<Geom_Vector>& Other);

  //! Subtracts the vector Other to <me>. A new vector is returned.
  [[nodiscard]] Standard_EXPORT occ::handle<Geom_VectorWithMagnitude> Subtracted(
    const occ::handle<Geom_Vector>& Other) const;

  //! Applies the transformation T to this vector.
  Standard_EXPORT void Transform(const gp_Trsf& T) override;

  //! Creates a new object which is a copy of this vector.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const override;

  DEFINE_STANDARD_RTTIEXT(Geom_VectorWithMagnitude, Geom_Vector)
};

#endif // _Geom_VectorWithMagnitude_HeaderFile
