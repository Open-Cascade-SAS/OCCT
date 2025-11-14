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

#ifndef _gp_Dir2d_HeaderFile
#define _gp_Dir2d_HeaderFile

#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

class gp_Vec2d;
class gp_Ax2d;
class gp_Trsf2d;

//! Describes a unit vector in the plane (2D space). This unit
//! vector is also called "Direction".
//! See Also
//! gce_MakeDir2d which provides functions for more
//! complex unit vector constructions
//! Geom2d_Direction which provides additional functions
//! for constructing unit vectors and works, in particular, with
//! the parametric equations of unit vectors
class gp_Dir2d
{
public:
  //! Standard directions in 2D space for optimized constexpr construction
  enum class D
  {
    X,  //!< Direction along positive X axis (1, 0)
    Y,  //!< Direction along positive Y axis (0, 1)
    NX, //!< Direction along negative X axis (-1, 0)
    NY  //!< Direction along negative Y axis (0, -1)
  };

  DEFINE_STANDARD_ALLOC

  //! Creates a direction corresponding to X axis.
  constexpr gp_Dir2d() noexcept
      : coord(1., 0.)
  {
  }

  //! Creates a direction from a standard direction enumeration.
  constexpr explicit gp_Dir2d(const D theDir) noexcept
      : coord(theDir == D::X    ? 1.0
              : theDir == D::NX ? -1.0
                                : 0.0,
              theDir == D::Y    ? 1.0
              : theDir == D::NY ? -1.0
                                : 0.0)
  {
  }

  //! Normalizes the vector theV and creates a Direction. Raises ConstructionError if
  //! theV.Magnitude() <= Resolution from gp.
  gp_Dir2d(const gp_Vec2d& theV);

  //! Creates a Direction from a doublet of coordinates. Raises ConstructionError if
  //! theCoord.Modulus() <= Resolution from gp.
  gp_Dir2d(const gp_XY& theCoord);

  //! Creates a Direction with its 2 cartesian coordinates. Raises ConstructionError if
  //! std::sqrt(theXv*theXv + theYv*theYv) <= Resolution from gp.
  gp_Dir2d(const Standard_Real theXv, const Standard_Real theYv);

  //! For this unit vector, assigns:
  //! the value theXi to:
  //! -   the X coordinate if theIndex is 1, or
  //! -   the Y coordinate if theIndex is 2, and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_OutOfRange if theIndex is not 1 or 2.
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   std::sqrt(theXv*theXv + theYv*theYv), or
  //! -   the modulus of the number pair formed by the new
  //! value theXi and the other coordinate of this vector that
  //! was not directly modified.
  //! Raises OutOfRange if theIndex != {1, 2}.
  void SetCoord(const Standard_Integer theIndex, const Standard_Real theXi);

  //! For this unit vector, assigns:
  //! -   the values theXv and theYv to its two coordinates,
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_OutOfRange if theIndex is not 1 or 2.
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   std::sqrt(theXv*theXv + theYv*theYv), or
  //! -   the modulus of the number pair formed by the new
  //! value Xi and the other coordinate of this vector that
  //! was not directly modified.
  //! Raises OutOfRange if theIndex != {1, 2}.
  void SetCoord(const Standard_Real theXv, const Standard_Real theYv);

  //! Assigns the given value to the X coordinate of this unit vector,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   the modulus of Coord, or
  //! -   the modulus of the number pair formed from the new
  //! X or Y coordinate and the other coordinate of this
  //! vector that was not directly modified.
  void SetX(const Standard_Real theX);

  //! Assigns the given value to the Y coordinate of this unit vector,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   the modulus of Coord, or
  //! -   the modulus of the number pair formed from the new
  //! X or Y coordinate and the other coordinate of this
  //! vector that was not directly modified.
  void SetY(const Standard_Real theY);

  //! Assigns:
  //! -   the two coordinates of theCoord to this unit vector,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   the modulus of theCoord, or
  //! -   the modulus of the number pair formed from the new
  //! X or Y coordinate and the other coordinate of this
  //! vector that was not directly modified.
  void SetXY(const gp_XY& theCoord);

  //! For this unit vector returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! Raises OutOfRange if theIndex != {1, 2}.
  Standard_Real Coord(const Standard_Integer theIndex) const { return coord.Coord(theIndex); }

  //! For this unit vector returns its two coordinates theXv and theYv.
  //! Raises OutOfRange if theIndex != {1, 2}.
  constexpr void Coord(Standard_Real& theXv, Standard_Real& theYv) const noexcept
  {
    coord.Coord(theXv, theYv);
  }

  //! For this unit vector, returns its X coordinate.
  constexpr Standard_Real X() const noexcept { return coord.X(); }

  //! For this unit vector, returns its Y coordinate.
  constexpr Standard_Real Y() const noexcept { return coord.Y(); }

  //! For this unit vector, returns its two coordinates as a number pair.
  //! Comparison between Directions
  //! The precision value is an input data.
  constexpr const gp_XY& XY() const noexcept { return coord; }

  //! Returns True if the two vectors have the same direction
  //! i.e. the angle between this unit vector and the
  //! unit vector theOther is less than or equal to theAngularTolerance.
  Standard_Boolean IsEqual(const gp_Dir2d& theOther, const Standard_Real theAngularTolerance) const;

  //! Returns True if the angle between this unit vector and the
  //! unit vector theOther is equal to Pi/2 or -Pi/2 (normal)
  //! i.e. std::abs(std::abs(<me>.Angle(theOther)) - PI/2.) <= theAngularTolerance
  Standard_Boolean IsNormal(const gp_Dir2d&     theOther,
                            const Standard_Real theAngularTolerance) const;

  //! Returns True if the angle between this unit vector and the
  //! unit vector theOther is equal to Pi or -Pi (opposite).
  //! i.e.  PI - std::abs(<me>.Angle(theOther)) <= theAngularTolerance
  Standard_Boolean IsOpposite(const gp_Dir2d&     theOther,
                              const Standard_Real theAngularTolerance) const;

  //! Returns True if the angle between this unit vector and unit
  //! vector theOther is equal to 0, Pi or -Pi.
  //! i.e.  std::abs(Angle(<me>, theOther)) <= theAngularTolerance or
  //! PI - std::abs(Angle(<me>, theOther)) <= theAngularTolerance
  Standard_Boolean IsParallel(const gp_Dir2d&     theOther,
                              const Standard_Real theAngularTolerance) const;

  //! Computes the angular value in radians between <me> and
  //! <theOther>. Returns the angle in the range [-PI, PI].
  Standard_EXPORT Standard_Real Angle(const gp_Dir2d& theOther) const;

  //! Computes the cross product between two directions.
  Standard_NODISCARD constexpr Standard_Real Crossed(const gp_Dir2d& theRight) const noexcept
  {
    return coord.Crossed(theRight.coord);
  }

  Standard_NODISCARD constexpr Standard_Real operator^(const gp_Dir2d& theRight) const noexcept
  {
    return Crossed(theRight);
  }

  //! Computes the scalar product
  constexpr Standard_Real Dot(const gp_Dir2d& theOther) const noexcept
  {
    return coord.Dot(theOther.coord);
  }

  constexpr Standard_Real operator*(const gp_Dir2d& theOther) const noexcept
  {
    return Dot(theOther);
  }

  constexpr void Reverse() noexcept { coord.Reverse(); }

  //! Reverses the orientation of a direction
  Standard_NODISCARD constexpr gp_Dir2d Reversed() const noexcept
  {
    gp_Dir2d aV = *this;
    aV.coord.Reverse();
    return aV;
  }

  Standard_NODISCARD constexpr gp_Dir2d operator-() const noexcept { return Reversed(); }

  Standard_EXPORT void Mirror(const gp_Dir2d& theV) noexcept;

  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction theV which is the center of
  //! the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Dir2d Mirrored(const gp_Dir2d& theV) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax2d& theA) noexcept;

  //! Performs the symmetrical transformation of a direction
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Dir2d Mirrored(const gp_Ax2d& theA) const noexcept;

  void Rotate(const Standard_Real Ang);

  //! Rotates a direction. theAng is the angular value of
  //! the rotation in radians.
  Standard_NODISCARD gp_Dir2d Rotated(const Standard_Real theAng) const
  {
    gp_Dir2d aV = *this;
    aV.Rotate(theAng);
    return aV;
  }

  Standard_EXPORT void Transform(const gp_Trsf2d& theT) noexcept;

  //! Transforms a direction with the "Trsf" theT.
  //! Warnings :
  //! If the scale factor of the "Trsf" theT is negative then the
  //! direction <me> is reversed.
  Standard_NODISCARD gp_Dir2d Transformed(const gp_Trsf2d& theT) const
  {
    gp_Dir2d aV = *this;
    aV.Transform(theT);
    return aV;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  gp_XY coord;
};

#include <gp_Ax2d.hxx>
#include <gp_Trsf2d.hxx>

//=================================================================================================

inline gp_Dir2d::gp_Dir2d(const gp_Vec2d& theV)
{
  const gp_XY&  aXY = theV.XY();
  Standard_Real aX  = aXY.X();
  Standard_Real anY = aXY.Y();
  Standard_Real aD  = sqrt(aX * aX + anY * anY);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d() - input vector has zero norm");
  coord.SetX(aX / aD);
  coord.SetY(anY / aD);
}

//=================================================================================================

inline gp_Dir2d::gp_Dir2d(const gp_XY& theXY)
{
  Standard_Real aX  = theXY.X();
  Standard_Real anY = theXY.Y();
  Standard_Real aD  = sqrt(aX * aX + anY * anY);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d() - input vector has zero norm");
  coord.SetX(aX / aD);
  coord.SetY(anY / aD);
}

//=================================================================================================

inline gp_Dir2d::gp_Dir2d(const Standard_Real theXv, const Standard_Real theYv)
{
  Standard_Real aD = sqrt(theXv * theXv + theYv * theYv);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d() - input vector has zero norm");
  coord.SetX(theXv / aD);
  coord.SetY(theYv / aD);
}

//=================================================================================================

inline void gp_Dir2d::SetCoord(const Standard_Integer theIndex, const Standard_Real theXi)
{
  Standard_Real aX  = coord.X();
  Standard_Real anY = coord.Y();
  Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 2,
                               "gp_Dir2d::SetCoord() - index is out of range [1, 2]");
  if (theIndex == 1)
  {
    aX = theXi;
  }
  else
  {
    anY = theXi;
  }
  Standard_Real aD = sqrt(aX * aX + anY * anY);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d::SetCoord() - result vector has zero norm");
  coord.SetX(aX / aD);
  coord.SetY(anY / aD);
}

//=================================================================================================

inline void gp_Dir2d::SetCoord(const Standard_Real theXv, const Standard_Real theYv)
{
  Standard_Real aD = sqrt(theXv * theXv + theYv * theYv);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d::SetCoord() - result vector has zero norm");
  coord.SetX(theXv / aD);
  coord.SetY(theYv / aD);
}

//=================================================================================================

inline void gp_Dir2d::SetX(const Standard_Real theX)
{
  Standard_Real anY = coord.Y();
  Standard_Real aD  = sqrt(theX * theX + anY * anY);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d::SetX() - result vector has zero norm");
  coord.SetX(theX / aD);
  coord.SetY(anY / aD);
}

//=================================================================================================

inline void gp_Dir2d::SetY(const Standard_Real theY)
{
  Standard_Real aX = coord.X();
  Standard_Real aD = sqrt(aX * aX + theY * theY);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d::SetY() - result vector has zero norm");
  coord.SetX(aX / aD);
  coord.SetY(theY / aD);
}

//=================================================================================================

inline void gp_Dir2d::SetXY(const gp_XY& theXY)
{
  Standard_Real aX  = theXY.X();
  Standard_Real anY = theXY.Y();
  Standard_Real aD  = sqrt(aX * aX + anY * anY);
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_Dir2d::SetZ() - result vector has zero norm");
  coord.SetX(aX / aD);
  coord.SetY(anY / aD);
}

//=================================================================================================

inline Standard_Boolean gp_Dir2d::IsEqual(const gp_Dir2d&     theOther,
                                          const Standard_Real theAngularTolerance) const
{
  Standard_Real anAng = Angle(theOther);
  if (anAng < 0)
  {
    anAng = -anAng;
  }
  return anAng <= theAngularTolerance;
}

//=================================================================================================

inline Standard_Boolean gp_Dir2d::IsNormal(const gp_Dir2d&     theOther,
                                           const Standard_Real theAngularTolerance) const
{
  Standard_Real anAng = Angle(theOther);
  if (anAng < 0)
  {
    anAng = -anAng;
  }
  anAng = M_PI / 2.0 - anAng;
  if (anAng < 0)
  {
    anAng = -anAng;
  }
  return anAng <= theAngularTolerance;
}

//=================================================================================================

inline Standard_Boolean gp_Dir2d::IsOpposite(const gp_Dir2d&     theOther,
                                             const Standard_Real theAngularTolerance) const
{
  Standard_Real anAng = Angle(theOther);
  if (anAng < 0)
  {
    anAng = -anAng;
  }
  return M_PI - anAng <= theAngularTolerance;
}

//=================================================================================================

inline Standard_Boolean gp_Dir2d::IsParallel(const gp_Dir2d&     theOther,
                                             const Standard_Real theAngularTolerance) const
{
  Standard_Real anAng = Angle(theOther);
  if (anAng < 0)
  {
    anAng = -anAng;
  }
  return anAng <= theAngularTolerance || M_PI - anAng <= theAngularTolerance;
}

//=================================================================================================

inline void gp_Dir2d::Rotate(const Standard_Real theAng)
{
  gp_Trsf2d aT;
  aT.SetRotation(gp_Pnt2d(0.0, 0.0), theAng);
  coord.Multiply(aT.HVectorialPart());
}

#endif // _gp_Dir2d_HeaderFile
