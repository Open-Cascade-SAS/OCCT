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

#ifndef _gp_Dir_HeaderFile
#define _gp_Dir_HeaderFile

#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

class gp_Vec;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;

//! Describes a unit vector in 3D space. This unit vector is also called "Direction".
//! See Also
//! gce_MakeDir which provides functions for more complex
//! unit vector constructions
//! Geom_Direction which provides additional functions for
//! constructing unit vectors and works, in particular, with the
//! parametric equations of unit vectors.
class gp_Dir
{
public:
  //! Standard directions in 3D space for optimized constexpr construction
  enum class D
  {
    X,  //!< Direction along positive X axis (1, 0, 0)
    Y,  //!< Direction along positive Y axis (0, 1, 0)
    Z,  //!< Direction along positive Z axis (0, 0, 1)
    NX, //!< Direction along negative X axis (-1, 0, 0)
    NY, //!< Direction along negative Y axis (0, -1, 0)
    NZ  //!< Direction along negative Z axis (0, 0, -1)
  };

  DEFINE_STANDARD_ALLOC

  //! Creates a direction corresponding to X axis.
  constexpr gp_Dir() noexcept
      : coord(1., 0., 0.)
  {
  }

  //! Creates a direction from a standard direction enumeration.
  constexpr explicit gp_Dir(const D theDir) noexcept
      : coord(theDir == D::X    ? 1.0
              : theDir == D::NX ? -1.0
                                : 0.0,
              theDir == D::Y    ? 1.0
              : theDir == D::NY ? -1.0
                                : 0.0,
              theDir == D::Z    ? 1.0
              : theDir == D::NZ ? -1.0
                                : 0.0)
  {
  }

  //! Normalizes the vector theV and creates a direction. Raises ConstructionError if
  //! theV.Magnitude() <= Resolution.
  //! @note Constexpr-compatible when input is already normalized.
  constexpr gp_Dir(const gp_Vec& theV);

  //! Creates a direction from a triplet of coordinates. Raises ConstructionError if
  //! theCoord.Modulus() <= Resolution from gp.
  //! @note Constexpr-compatible when input is already normalized.
  constexpr gp_Dir(const gp_XYZ& theCoord);

  //! Creates a direction with its 3 cartesian coordinates. Raises ConstructionError if
  //! std::sqrt(theXv*theXv + theYv*theYv + theZv*theZv) <= Resolution Modification of the
  //! direction's coordinates If std::sqrt (theXv*theXv + theYv*theYv + theZv*theZv) <= Resolution
  //! from gp where theXv, theYv ,theZv are the new coordinates it is not possible to construct the
  //! direction and the method raises the exception ConstructionError.
  //! @note Constexpr-compatible when input is already normalized.
  constexpr gp_Dir(const double theXv, const double theYv, const double theZv);

  constexpr gp_Dir(const gp_Dir&) noexcept = default;
  constexpr gp_Dir(gp_Dir&&) noexcept      = default;

  constexpr gp_Dir& operator=(const gp_Dir&) noexcept = default;
  constexpr gp_Dir& operator=(gp_Dir&&) noexcept      = default;

  //! For this unit vector, assigns the value Xi to:
  //! -   the X coordinate if theIndex is 1, or
  //! -   the Y coordinate if theIndex is 2, or
  //! -   the Z coordinate if theIndex is 3,
  //! and then normalizes it.
  //! Warning:
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_OutOfRange if theIndex is not 1, 2, or 3.
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   std::sqrt(Xv*Xv + Yv*Yv + Zv*Zv), or
  //! -   the modulus of the number triple formed by the new
  //! value theXi and the two other coordinates of this vector
  //! that were not directly modified.
  //! @note Constexpr-compatible when result is already normalized.
  constexpr void SetCoord(const int theIndex, const double theXi);

  //! For this unit vector, assigns the values theXv, theYv and theZv to its three coordinates.
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! @note Constexpr-compatible when input is already normalized.
  constexpr void SetCoord(const double theXv,
                          const double theYv,
                          const double theZv);

  //! Assigns the given value to the X coordinate of this unit vector.
  //! @note Constexpr-compatible when result is already normalized.
  constexpr void SetX(const double theX);

  //! Assigns the given value to the Y coordinate of this unit vector.
  //! @note Constexpr-compatible when result is already normalized.
  constexpr void SetY(const double theY);

  //! Assigns the given value to the Z coordinate of this unit vector.
  //! @note Constexpr-compatible when result is already normalized.
  constexpr void SetZ(const double theZ);

  //! Assigns the three coordinates of theCoord to this unit vector.
  //! @note Constexpr-compatible when input is already normalized.
  constexpr void SetXYZ(const gp_XYZ& theCoord);

  //! Returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! theIndex = 3 => Z is returned
  //! Exceptions
  //! Standard_OutOfRange if theIndex is not 1, 2, or 3.
  constexpr double Coord(const int theIndex) const
  {
    return coord.Coord(theIndex);
  }

  //! Returns for the unit vector its three coordinates theXv, theYv, and theZv.
  constexpr void Coord(double& theXv,
                       double& theYv,
                       double& theZv) const noexcept
  {
    coord.Coord(theXv, theYv, theZv);
  }

  //! Returns the X coordinate for a unit vector.
  constexpr double X() const noexcept { return coord.X(); }

  //! Returns the Y coordinate for a unit vector.
  constexpr double Y() const noexcept { return coord.Y(); }

  //! Returns the Z coordinate for a unit vector.
  constexpr double Z() const noexcept { return coord.Z(); }

  //! for this unit vector, returns its three coordinates as a number triple.
  constexpr const gp_XYZ& XYZ() const noexcept { return coord; }

  //! Returns True if the angle between the two directions is
  //! lower or equal to theAngularTolerance.
  bool IsEqual(const gp_Dir& theOther, const double theAngularTolerance) const
  {
    return Angle(theOther) <= theAngularTolerance;
  }

  //! Returns True if the angle between this unit vector and the unit vector theOther is equal to
  //! Pi/2 (normal).
  bool IsNormal(const gp_Dir& theOther, const double theAngularTolerance) const
  {
    double anAng = M_PI / 2.0 - Angle(theOther);
    if (anAng < 0)
    {
      anAng = -anAng;
    }
    return anAng <= theAngularTolerance;
  }

  //! Returns True if the angle between this unit vector and the unit vector theOther is equal to
  //! Pi (opposite).
  bool IsOpposite(const gp_Dir& theOther, const double theAngularTolerance) const
  {
    return M_PI - Angle(theOther) <= theAngularTolerance;
  }

  //! Returns true if the angle between this unit vector and the
  //! unit vector theOther is equal to 0 or to Pi.
  //! Note: the tolerance criterion is given by theAngularTolerance.
  bool IsParallel(const gp_Dir& theOther, const double theAngularTolerance) const
  {
    double anAng = Angle(theOther);
    return anAng <= theAngularTolerance || M_PI - anAng <= theAngularTolerance;
  }

  //! Computes the angular value in radians between <me> and
  //! <theOther>. This value is always positive in 3D space.
  //! Returns the angle in the range [0, PI]
  Standard_EXPORT double Angle(const gp_Dir& theOther) const;

  //! Computes the angular value between <me> and <theOther>.
  //! <theVRef> is the direction of reference normal to <me> and <theOther>
  //! and its orientation gives the positive sense of rotation.
  //! If the cross product <me> ^ <theOther> has the same orientation
  //! as <theVRef> the angular value is positive else negative.
  //! Returns the angular value in the range -PI and PI (in radians). Raises DomainError if <me>
  //! and <theOther> are not parallel this exception is raised when <theVRef> is in the same plane
  //! as <me> and <theOther> The tolerance criterion is Resolution from package gp.
  Standard_EXPORT double AngleWithRef(const gp_Dir& theOther, const gp_Dir& theVRef) const;

  //! Computes the cross product between two directions
  //! Raises the exception ConstructionError if the two directions
  //! are parallel because the computed vector cannot be normalized
  //! to create a direction.
  //! @note Constexpr-compatible when result is already normalized.
  constexpr void Cross(const gp_Dir& theRight);

  constexpr void operator^=(const gp_Dir& theRight) { Cross(theRight); }

  //! Computes the triple vector product.
  //! <me> ^ (V1 ^ V2)
  //! Raises the exception ConstructionError if V1 and V2 are parallel
  //! or <me> and (V1^V2) are parallel because the computed vector
  //! can't be normalized to create a direction.
  //! @note Constexpr-compatible when result is already normalized.
  [[nodiscard]] constexpr gp_Dir Crossed(const gp_Dir& theRight) const;

  [[nodiscard]] constexpr gp_Dir operator^(const gp_Dir& theRight) const
  {
    return Crossed(theRight);
  }

  //! @note Constexpr-compatible when result is already normalized.
  constexpr void CrossCross(const gp_Dir& theV1, const gp_Dir& theV2);

  //! Computes the double vector product this ^ (theV1 ^ theV2).
  //! -   CrossCrossed creates a new unit vector.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! -   theV1 and theV2 are parallel, or
  //! -   this unit vector and (theV1 ^ theV2) are parallel.
  //! This is because, in these conditions, the computed vector
  //! is null and cannot be normalized.
  //! @note Constexpr-compatible when result is already normalized.
  [[nodiscard]] constexpr gp_Dir CrossCrossed(const gp_Dir& theV1, const gp_Dir& theV2) const;

  //! Computes the scalar product
  constexpr double Dot(const gp_Dir& theOther) const noexcept
  {
    return coord.Dot(theOther.coord);
  }

  constexpr double operator*(const gp_Dir& theOther) const noexcept { return Dot(theOther); }

  //! Computes the triple scalar product <me> * (theV1 ^ theV2).
  //! Warnings :
  //! The computed vector theV1' = theV1 ^ theV2 is not normalized
  //! to create a unitary vector. So this method never
  //! raises an exception even if theV1 and theV2 are parallel.
  constexpr double DotCross(const gp_Dir& theV1, const gp_Dir& theV2) const noexcept
  {
    return coord.Dot(theV1.coord.Crossed(theV2.coord));
  }

  constexpr void Reverse() noexcept { coord.Reverse(); }

  //! Reverses the orientation of a direction
  //! geometric transformations
  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction V which is the center of
  //! the symmetry.
  [[nodiscard]] constexpr gp_Dir Reversed() const noexcept
  {
    gp_Dir aV = *this;
    aV.coord.Reverse();
    return aV;
  }

  [[nodiscard]] constexpr gp_Dir operator-() const noexcept { return Reversed(); }

  Standard_EXPORT void Mirror(const gp_Dir& theV) noexcept;

  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction theV which is the center
  //! of the symmetry.
  [[nodiscard]] Standard_EXPORT gp_Dir Mirrored(const gp_Dir& theV) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax1& theA1) noexcept;

  //! Performs the symmetrical transformation of a direction
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  [[nodiscard]] Standard_EXPORT gp_Dir Mirrored(const gp_Ax1& theA1) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax2& theA2) noexcept;

  //! Performs the symmetrical transformation of a direction
  //! with respect to a plane. The axis placement theA2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  [[nodiscard]] Standard_EXPORT gp_Dir Mirrored(const gp_Ax2& theA2) const noexcept;

  void Rotate(const gp_Ax1& theA1, const double theAng);

  //! Rotates a direction. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  [[nodiscard]] gp_Dir Rotated(const gp_Ax1& theA1, const double theAng) const
  {
    gp_Dir aV = *this;
    aV.Rotate(theA1, theAng);
    return aV;
  }

  Standard_EXPORT void Transform(const gp_Trsf& theT);

  //! Transforms a direction with a "Trsf" from gp.
  //! Warnings :
  //! If the scale factor of the "Trsf" theT is negative then the
  //! direction <me> is reversed.
  [[nodiscard]] gp_Dir Transformed(const gp_Trsf& theT) const
  {
    gp_Dir aV = *this;
    aV.Transform(theT);
    return aV;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT bool InitFromJson(const Standard_SStream& theSStream,
                                                int&       theStreamPos);

private:
  gp_XYZ coord;
};

#include <gp_Trsf.hxx>

//=================================================================================================

inline constexpr gp_Dir::gp_Dir(const gp_Vec& theV)
    : gp_Dir(theV.XYZ())
{
}

//=================================================================================================

inline constexpr gp_Dir::gp_Dir(const gp_XYZ& theXYZ)
    : gp_Dir(theXYZ.X(), theXYZ.Y(), theXYZ.Z())
{
}

//=================================================================================================

inline constexpr gp_Dir::gp_Dir(const double theXv,
                                const double theYv,
                                const double theZv)
{
  const double aSqMod = theXv * theXv + theYv * theYv + theZv * theZv;

  // Fast path: already normalized - fully constexpr
  if (aSqMod >= (1.0 - gp::Resolution()) && aSqMod <= (1.0 + gp::Resolution()))
  {
    coord.SetCoord(theXv, theYv, theZv);
    return;
  }

  // Slow path: runtime only (sqrt not constexpr - compile error if reached in constexpr context)
  Standard_ConstructionError_Raise_if(aSqMod <= gp::Resolution() * gp::Resolution(),
                                      "gp_Dir() - input vector has zero norm");
  const double aD = sqrt(aSqMod);
  coord.SetCoord(theXv / aD, theYv / aD, theZv / aD);
}

//=================================================================================================

inline constexpr void gp_Dir::SetCoord(const int theIndex, const double theXi)
{
  Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 3,
                               "gp_Dir::SetCoord() - index is out of range [1, 3]");
  if (theIndex == 1)
  {
    SetX(theXi);
  }
  else if (theIndex == 2)
  {
    SetY(theXi);
  }
  else
  {
    SetZ(theXi);
  }
}

//=================================================================================================

inline constexpr void gp_Dir::SetCoord(const double theXv,
                                       const double theYv,
                                       const double theZv)
{
  const double aSqMod = theXv * theXv + theYv * theYv + theZv * theZv;

  // Fast path: already normalized - fully constexpr
  if (aSqMod >= (1.0 - gp::Resolution()) && aSqMod <= (1.0 + gp::Resolution()))
  {
    coord.SetCoord(theXv, theYv, theZv);
    return;
  }

  // Slow path: runtime only (sqrt not constexpr - compile error if reached in constexpr context)
  Standard_ConstructionError_Raise_if(aSqMod <= gp::Resolution() * gp::Resolution(),
                                      "gp_Dir::SetCoord() - input vector has zero norm");
  const double aD = sqrt(aSqMod);
  coord.SetCoord(theXv / aD, theYv / aD, theZv / aD);
}

//=================================================================================================

inline constexpr void gp_Dir::SetX(const double theX)
{
  SetCoord(theX, coord.Y(), coord.Z());
}

//=================================================================================================

inline constexpr void gp_Dir::SetY(const double theY)
{
  SetCoord(coord.X(), theY, coord.Z());
}

//=================================================================================================

inline constexpr void gp_Dir::SetZ(const double theZ)
{
  SetCoord(coord.X(), coord.Y(), theZ);
}

//=================================================================================================

inline constexpr void gp_Dir::SetXYZ(const gp_XYZ& theXYZ)
{
  SetCoord(theXYZ.X(), theXYZ.Y(), theXYZ.Z());
}

//=================================================================================================

inline constexpr void gp_Dir::Cross(const gp_Dir& theRight)
{
  coord.Cross(theRight.coord);
  const double aSqMod = coord.SquareModulus();

  // Fast path: already normalized - fully constexpr
  if (aSqMod >= (1.0 - gp::Resolution()) && aSqMod <= (1.0 + gp::Resolution()))
  {
    return;
  }

  // Slow path: runtime only (sqrt not constexpr - compile error if reached in constexpr context)
  Standard_ConstructionError_Raise_if(aSqMod <= gp::Resolution() * gp::Resolution(),
                                      "gp_Dir::Cross() - result vector has zero norm");
  coord.Divide(sqrt(aSqMod));
}

//=================================================================================================

inline constexpr gp_Dir gp_Dir::Crossed(const gp_Dir& theRight) const
{
  gp_Dir aV = *this;
  aV.Cross(theRight);
  return aV;
}

//=================================================================================================

inline constexpr void gp_Dir::CrossCross(const gp_Dir& theV1, const gp_Dir& theV2)
{
  coord.CrossCross(theV1.coord, theV2.coord);
  const double aSqMod = coord.SquareModulus();

  // Fast path: already normalized - fully constexpr
  if (aSqMod >= (1.0 - gp::Resolution()) && aSqMod <= (1.0 + gp::Resolution()))
  {
    return;
  }

  // Slow path: runtime only (sqrt not constexpr - compile error if reached in constexpr context)
  Standard_ConstructionError_Raise_if(aSqMod <= gp::Resolution() * gp::Resolution(),
                                      "gp_Dir::CrossCross() - result vector has zero norm");
  coord.Divide(sqrt(aSqMod));
}

//=================================================================================================

inline constexpr gp_Dir gp_Dir::CrossCrossed(const gp_Dir& theV1, const gp_Dir& theV2) const
{
  gp_Dir aV = *this;
  aV.CrossCross(theV1, theV2);
  return aV;
}

//=================================================================================================

inline void gp_Dir::Rotate(const gp_Ax1& theA1, const double theAng)
{
  gp_Trsf aT;
  aT.SetRotation(theA1, theAng);
  coord.Multiply(aT.HVectorialPart());
}

#endif // _gp_Dir_HeaderFile
