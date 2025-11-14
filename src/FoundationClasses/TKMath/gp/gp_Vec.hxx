// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _gp_Vec_HeaderFile
#define _gp_Vec_HeaderFile

#include <gp_VectorWithNullMagnitude.hxx>
#include <gp_XYZ.hxx>
#include <Standard_DomainError.hxx>

class gp_Dir;
class gp_Pnt;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;

//! Defines a non-persistent vector in 3D space.
class gp_Vec
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a zero vector.
  constexpr gp_Vec() noexcept {}

  //! Creates a unitary vector from a direction theV.
  gp_Vec(const gp_Dir& theV);

  //! Creates a vector with a triplet of coordinates.
  constexpr gp_Vec(const gp_XYZ& theCoord) noexcept
      : coord(theCoord)
  {
  }

  //! Creates a point with its three cartesian coordinates.
  constexpr gp_Vec(const Standard_Real theXv,
                   const Standard_Real theYv,
                   const Standard_Real theZv) noexcept
      : coord(theXv, theYv, theZv)
  {
  }

  //! Creates a vector from two points. The length of the vector
  //! is the distance between theP1 and theP2
  gp_Vec(const gp_Pnt& theP1, const gp_Pnt& theP2);

  //! Changes the coordinate of range theIndex
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! theIndex = 3 => Z is modified
  //! Raised if theIndex != {1, 2, 3}.
  void SetCoord(const Standard_Integer theIndex, const Standard_Real theXi)
  {
    coord.SetCoord(theIndex, theXi);
  }

  //! For this vector, assigns
  //! -   the values theXv, theYv and theZv to its three coordinates.
  constexpr void SetCoord(const Standard_Real theXv,
                          const Standard_Real theYv,
                          const Standard_Real theZv) noexcept
  {
    coord.SetX(theXv);
    coord.SetY(theYv);
    coord.SetZ(theZv);
  }

  //! Assigns the given value to the X coordinate of this vector.
  constexpr void SetX(const Standard_Real theX) noexcept { coord.SetX(theX); }

  //! Assigns the given value to the X coordinate of this vector.
  constexpr void SetY(const Standard_Real theY) noexcept { coord.SetY(theY); }

  //! Assigns the given value to the X coordinate of this vector.
  constexpr void SetZ(const Standard_Real theZ) noexcept { coord.SetZ(theZ); }

  //! Assigns the three coordinates of theCoord to this vector.
  constexpr void SetXYZ(const gp_XYZ& theCoord) noexcept { coord = theCoord; }

  //! Returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! theIndex = 3 => Z is returned
  //! Raised if theIndex != {1, 2, 3}.
  Standard_Real Coord(const Standard_Integer theIndex) const { return coord.Coord(theIndex); }

  //! For this vector returns its three coordinates theXv, theYv, and theZv inline
  constexpr void Coord(Standard_Real& theXv,
                       Standard_Real& theYv,
                       Standard_Real& theZv) const noexcept
  {
    theXv = coord.X();
    theYv = coord.Y();
    theZv = coord.Z();
  }

  //! For this vector, returns its X coordinate.
  constexpr Standard_Real X() const noexcept { return coord.X(); }

  //! For this vector, returns its Y coordinate.
  constexpr Standard_Real Y() const noexcept { return coord.Y(); }

  //! For this vector, returns its Z coordinate.
  constexpr Standard_Real Z() const noexcept { return coord.Z(); }

  //! For this vector, returns
  //! -   its three coordinates as a number triple
  constexpr const gp_XYZ& XYZ() const noexcept { return coord; }

  //! Returns True if the two vectors have the same magnitude value
  //! and the same direction. The precision values are theLinearTolerance
  //! for the magnitude and theAngularTolerance for the direction.
  Standard_EXPORT Standard_Boolean IsEqual(const gp_Vec&       theOther,
                                           const Standard_Real theLinearTolerance,
                                           const Standard_Real theAngularTolerance) const;

  //! Returns True if abs(<me>.Angle(theOther) - PI/2.) <= theAngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! theOther.Magnitude() <= Resolution from gp
  Standard_Boolean IsNormal(const gp_Vec& theOther, const Standard_Real theAngularTolerance) const;

  //! Returns True if PI - <me>.Angle(theOther) <= theAngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp
  Standard_Boolean IsOpposite(const gp_Vec& theOther, const Standard_Real theAngularTolerance) const
  {
    const Standard_Real anAng = M_PI - Angle(theOther);
    return anAng <= theAngularTolerance;
  }

  //! Returns True if Angle(<me>, theOther) <= theAngularTolerance or
  //! PI - Angle(<me>, theOther) <= theAngularTolerance
  //! This definition means that two parallel vectors cannot define
  //! a plane but two vectors with opposite directions are considered
  //! as parallel. Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! Other.Magnitude() <= Resolution from gp
  Standard_Boolean IsParallel(const gp_Vec& theOther, const Standard_Real theAngularTolerance) const
  {
    const Standard_Real anAng = Angle(theOther);
    return anAng <= theAngularTolerance || M_PI - anAng <= theAngularTolerance;
  }

  //! Computes the angular value between <me> and <theOther>
  //! Returns the angle value between 0 and PI in radian.
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution from gp or
  //! theOther.Magnitude() <= Resolution because the angular value is
  //! indefinite if one of the vectors has a null magnitude.
  Standard_Real Angle(const gp_Vec& theOther) const;

  //! Computes the angle, in radians, between this vector and
  //! vector theOther. The result is a value between -Pi and Pi.
  //! For this, theVRef defines the positive sense of rotation: the
  //! angular value is positive, if the cross product this ^ theOther
  //! has the same orientation as theVRef relative to the plane
  //! defined by the vectors this and theOther. Otherwise, the
  //! angular value is negative.
  //! Exceptions
  //! gp_VectorWithNullMagnitude if the magnitude of this
  //! vector, the vector theOther, or the vector theVRef is less than or
  //! equal to gp::Resolution().
  //! Standard_DomainError if this vector, the vector theOther,
  //! and the vector theVRef are coplanar, unless this vector and
  //! the vector theOther are parallel.
  Standard_Real AngleWithRef(const gp_Vec& theOther, const gp_Vec& theVRef) const;

  //! Computes the magnitude of this vector.
  Standard_Real Magnitude() const { return coord.Modulus(); }

  //! Computes the square magnitude of this vector.
  constexpr Standard_Real SquareMagnitude() const noexcept { return coord.SquareModulus(); }

  //! Adds two vectors
  constexpr void Add(const gp_Vec& theOther) noexcept { coord.Add(theOther.coord); }

  constexpr void operator+=(const gp_Vec& theOther) noexcept { Add(theOther); }

  //! Adds two vectors
  Standard_NODISCARD constexpr gp_Vec Added(const gp_Vec& theOther) const noexcept
  {
    gp_Vec aV = *this;
    aV.coord.Add(theOther.coord);
    return aV;
  }

  Standard_NODISCARD constexpr gp_Vec operator+(const gp_Vec& theOther) const noexcept
  {
    return Added(theOther);
  }

  //! Subtracts two vectors
  constexpr void Subtract(const gp_Vec& theRight) noexcept { coord.Subtract(theRight.coord); }

  constexpr void operator-=(const gp_Vec& theRight) noexcept { Subtract(theRight); }

  //! Subtracts two vectors
  Standard_NODISCARD constexpr gp_Vec Subtracted(const gp_Vec& theRight) const noexcept
  {
    gp_Vec aV = *this;
    aV.coord.Subtract(theRight.coord);
    return aV;
  }

  Standard_NODISCARD constexpr gp_Vec operator-(const gp_Vec& theRight) const noexcept
  {
    return Subtracted(theRight);
  }

  //! Multiplies a vector by a scalar
  constexpr void Multiply(const Standard_Real theScalar) noexcept { coord.Multiply(theScalar); }

  constexpr void operator*=(const Standard_Real theScalar) noexcept { Multiply(theScalar); }

  //! Multiplies a vector by a scalar
  Standard_NODISCARD constexpr gp_Vec Multiplied(const Standard_Real theScalar) const noexcept
  {
    gp_Vec aV = *this;
    aV.coord.Multiply(theScalar);
    return aV;
  }

  Standard_NODISCARD constexpr gp_Vec operator*(const Standard_Real theScalar) const noexcept
  {
    return Multiplied(theScalar);
  }

  //! Divides a vector by a scalar
  constexpr void Divide(const Standard_Real theScalar) { coord.Divide(theScalar); }

  constexpr void operator/=(const Standard_Real theScalar) { Divide(theScalar); }

  //! Divides a vector by a scalar
  Standard_NODISCARD constexpr gp_Vec Divided(const Standard_Real theScalar) const
  {
    gp_Vec aV = *this;
    aV.coord.Divide(theScalar);
    return aV;
  }

  Standard_NODISCARD constexpr gp_Vec operator/(const Standard_Real theScalar) const
  {
    return Divided(theScalar);
  }

  //! computes the cross product between two vectors
  constexpr void Cross(const gp_Vec& theRight) noexcept { coord.Cross(theRight.coord); }

  constexpr void operator^=(const gp_Vec& theRight) noexcept { Cross(theRight); }

  //! computes the cross product between two vectors
  Standard_NODISCARD constexpr gp_Vec Crossed(const gp_Vec& theRight) const noexcept
  {
    gp_Vec aV = *this;
    aV.coord.Cross(theRight.coord);
    return aV;
  }

  Standard_NODISCARD constexpr gp_Vec operator^(const gp_Vec& theRight) const noexcept
  {
    return Crossed(theRight);
  }

  //! Computes the magnitude of the cross
  //! product between <me> and theRight.
  //! Returns || <me> ^ theRight ||
  Standard_Real CrossMagnitude(const gp_Vec& theRight) const
  {
    return coord.CrossMagnitude(theRight.coord);
  }

  //! Computes the square magnitude of
  //! the cross product between <me> and theRight.
  //! Returns || <me> ^ theRight ||**2
  constexpr Standard_Real CrossSquareMagnitude(const gp_Vec& theRight) const noexcept
  {
    return coord.CrossSquareMagnitude(theRight.coord);
  }

  //! Computes the triple vector product.
  //! <me> ^= (theV1 ^ theV2)
  constexpr void CrossCross(const gp_Vec& theV1, const gp_Vec& theV2) noexcept
  {
    coord.CrossCross(theV1.coord, theV2.coord);
  }

  //! Computes the triple vector product.
  //! <me> ^ (theV1 ^ theV2)
  Standard_NODISCARD constexpr gp_Vec CrossCrossed(const gp_Vec& theV1,
                                                   const gp_Vec& theV2) const noexcept
  {
    gp_Vec aV = *this;
    aV.coord.CrossCross(theV1.coord, theV2.coord);
    return aV;
  }

  //! computes the scalar product
  constexpr Standard_Real Dot(const gp_Vec& theOther) const noexcept
  {
    return coord.Dot(theOther.coord);
  }

  constexpr Standard_Real operator*(const gp_Vec& theOther) const noexcept { return Dot(theOther); }

  //! Computes the triple scalar product <me> * (theV1 ^ theV2).
  constexpr Standard_Real DotCross(const gp_Vec& theV1, const gp_Vec& theV2) const noexcept
  {
    return coord.DotCross(theV1.coord, theV2.coord);
  }

  //! normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from gp.
  void Normalize()
  {
    const Standard_Real aD = coord.Modulus();
    Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                        "gp_Vec::Normalize() - vector has zero norm");
    coord.Divide(aD);
  }

  //! normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from gp.
  Standard_NODISCARD gp_Vec Normalized() const;

  //! Reverses the direction of a vector
  constexpr void Reverse() noexcept { coord.Reverse(); }

  //! Reverses the direction of a vector
  Standard_NODISCARD constexpr gp_Vec Reversed() const noexcept
  {
    gp_Vec aV = *this;
    aV.coord.Reverse();
    return aV;
  }

  Standard_NODISCARD constexpr gp_Vec operator-() const noexcept { return Reversed(); }

  //! <me> is set to the following linear form :
  //! theA1 * theV1 + theA2 * theV2 + theA3 * theV3 + theV4
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_Vec&       theV1,
                               const Standard_Real theA2,
                               const gp_Vec&       theV2,
                               const Standard_Real theA3,
                               const gp_Vec&       theV3,
                               const gp_Vec&       theV4) noexcept
  {
    coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord, theA3, theV3.coord, theV4.coord);
  }

  //! <me> is set to the following linear form :
  //! theA1 * theV1 + theA2 * theV2 + theA3 * theV3
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_Vec&       theV1,
                               const Standard_Real theA2,
                               const gp_Vec&       theV2,
                               const Standard_Real theA3,
                               const gp_Vec&       theV3) noexcept
  {
    coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord, theA3, theV3.coord);
  }

  //! <me> is set to the following linear form :
  //! theA1 * theV1 + theA2 * theV2 + theV3
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_Vec&       theV1,
                               const Standard_Real theA2,
                               const gp_Vec&       theV2,
                               const gp_Vec&       theV3) noexcept
  {
    coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord, theV3.coord);
  }

  //! <me> is set to the following linear form :
  //! theA1 * theV1 + theA2 * theV2
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_Vec&       theV1,
                               const Standard_Real theA2,
                               const gp_Vec&       theV2) noexcept
  {
    coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord);
  }

  //! <me> is set to the following linear form : theA1 * theV1 + theV2
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_Vec&       theV1,
                               const gp_Vec&       theV2) noexcept
  {
    coord.SetLinearForm(theA1, theV1.coord, theV2.coord);
  }

  //! <me> is set to the following linear form : theV1 + theV2
  constexpr void SetLinearForm(const gp_Vec& theV1, const gp_Vec& theV2) noexcept
  {
    coord.SetLinearForm(theV1.coord, theV2.coord);
  }

  Standard_EXPORT void Mirror(const gp_Vec& theV) noexcept;

  //! Performs the symmetrical transformation of a vector
  //! with respect to the vector theV which is the center of
  //! the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Vec Mirrored(const gp_Vec& theV) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax1& theA1) noexcept;

  //! Performs the symmetrical transformation of a vector
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Vec Mirrored(const gp_Ax1& theA1) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax2& theA2) noexcept;

  //! Performs the symmetrical transformation of a vector
  //! with respect to a plane. The axis placement theA2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Vec Mirrored(const gp_Ax2& theA2) const noexcept;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng);

  //! Rotates a vector. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Vec Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Vec aVres = *this;
    aVres.Rotate(theA1, theAng);
    return aVres;
  }

  constexpr void Scale(const Standard_Real theS) noexcept { coord.Multiply(theS); }

  //! Scales a vector. theS is the scaling value.
  Standard_NODISCARD constexpr gp_Vec Scaled(const Standard_Real theS) const noexcept
  {
    gp_Vec aV = *this;
    aV.coord.Multiply(theS);
    return aV;
  }

  //! Transforms a vector with the transformation theT.
  Standard_EXPORT void Transform(const gp_Trsf& theT);

  //! Transforms a vector with the transformation theT.
  Standard_NODISCARD gp_Vec Transformed(const gp_Trsf& theT) const
  {
    gp_Vec aV = *this;
    aV.Transform(theT);
    return aV;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  gp_XYZ coord;
};

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

//=================================================================================================

inline gp_Vec::gp_Vec(const gp_Dir& theV)
{
  coord = theV.XYZ();
}

//=================================================================================================

inline gp_Vec::gp_Vec(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  coord = theP2.XYZ().Subtracted(theP1.XYZ());
}

//=================================================================================================

inline Standard_Boolean gp_Vec::IsNormal(const gp_Vec&       theOther,
                                         const Standard_Real theAngularTolerance) const
{
  const Standard_Real anAng = std::abs(M_PI_2 - Angle(theOther));
  return anAng <= theAngularTolerance;
}

//=================================================================================================

inline Standard_Real gp_Vec::Angle(const gp_Vec& theOther) const
{
  gp_VectorWithNullMagnitude_Raise_if(coord.Modulus() <= gp::Resolution()
                                        || theOther.coord.Modulus() <= gp::Resolution(),
                                      " ");
  return (gp_Dir(coord)).Angle(theOther);
}

//=================================================================================================

inline Standard_Real gp_Vec::AngleWithRef(const gp_Vec& theOther, const gp_Vec& theVRef) const
{
  gp_VectorWithNullMagnitude_Raise_if(coord.Modulus() <= gp::Resolution()
                                        || theVRef.coord.Modulus() <= gp::Resolution()
                                        || theOther.coord.Modulus() <= gp::Resolution(),
                                      " ");
  return (gp_Dir(coord)).AngleWithRef(theOther, theVRef);
}

//=================================================================================================

inline gp_Vec gp_Vec::Normalized() const
{
  const Standard_Real aD = coord.Modulus();
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Vec::Normalized() - vector has zero norm");
  gp_Vec aV = *this;
  aV.coord.Divide(aD);
  return aV;
}

//=================================================================================================

inline void gp_Vec::Rotate(const gp_Ax1& theA1, const Standard_Real theAng)
{
  gp_Trsf aT;
  aT.SetRotation(theA1, theAng);
  coord.Multiply(aT.VectorialPart());
}

//=======================================================================
// function :  operator*
// purpose :
//=======================================================================
inline constexpr gp_Vec operator*(const Standard_Real theScalar, const gp_Vec& theV) noexcept
{
  return theV.Multiplied(theScalar);
}

#endif // _gp_Vec_HeaderFile
