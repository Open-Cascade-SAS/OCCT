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

#ifndef _gp_Pnt_HeaderFile
#define _gp_Pnt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

class gp_Ax1;
class gp_Ax2;
class gp_Trsf;
class gp_Vec;

//! Defines a 3D cartesian point.
class gp_Pnt
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a point with zero coordinates.
  constexpr gp_Pnt() noexcept
      : coord()
  {
  }

  //! Creates a point from a XYZ object.
  constexpr gp_Pnt(const gp_XYZ& theCoord) noexcept
      : coord(theCoord)
  {
  }

  //! Creates a point with its 3 cartesian's coordinates: theXp, theYp, theZp.
  constexpr gp_Pnt(const Standard_Real theXp,
                   const Standard_Real theYp,
                   const Standard_Real theZp) noexcept
      : coord(theXp, theYp, theZp)
  {
  }

  //! Changes the coordinate of range theIndex:
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! theIndex = 3 => Z is modified
  //! Raised if theIndex != {1, 2, 3}.
  void SetCoord(const Standard_Integer theIndex, const Standard_Real theXi)
  {
    coord.SetCoord(theIndex, theXi);
  }

  //! For this point, assigns the values theXp, theYp and theZp to its three coordinates.
  constexpr void SetCoord(const Standard_Real theXp,
                          const Standard_Real theYp,
                          const Standard_Real theZp) noexcept
  {
    coord.SetCoord(theXp, theYp, theZp);
  }

  //! Assigns the given value to the X coordinate of this point.
  constexpr void SetX(const Standard_Real theX) noexcept { coord.SetX(theX); }

  //! Assigns the given value to the Y coordinate of this point.
  constexpr void SetY(const Standard_Real theY) noexcept { coord.SetY(theY); }

  //! Assigns the given value to the Z coordinate of this point.
  constexpr void SetZ(const Standard_Real theZ) noexcept { coord.SetZ(theZ); }

  //! Assigns the three coordinates of theCoord to this point.
  constexpr void SetXYZ(const gp_XYZ& theCoord) noexcept { coord = theCoord; }

  //! Returns the coordinate of corresponding to the value of theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! theIndex = 3 => Z is returned
  //! Raises OutOfRange if theIndex != {1, 2, 3}.
  //! Raised if theIndex != {1, 2, 3}.
  Standard_Real Coord(const Standard_Integer theIndex) const { return coord.Coord(theIndex); }

  //! For this point gives its three coordinates theXp, theYp and theZp.
  constexpr void Coord(Standard_Real& theXp,
                       Standard_Real& theYp,
                       Standard_Real& theZp) const noexcept
  {
    coord.Coord(theXp, theYp, theZp);
  }

  //! For this point, returns its X coordinate.
  constexpr Standard_Real X() const noexcept { return coord.X(); }

  //! For this point, returns its Y coordinate.
  constexpr Standard_Real Y() const noexcept { return coord.Y(); }

  //! For this point, returns its Z coordinate.
  constexpr Standard_Real Z() const noexcept { return coord.Z(); }

  //! For this point, returns its three coordinates as a XYZ object.
  constexpr const gp_XYZ& XYZ() const noexcept { return coord; }

  //! For this point, returns its three coordinates as a XYZ object.
  constexpr const gp_XYZ& Coord() const noexcept { return coord; }

  //! Returns the coordinates of this point.
  //! Note: This syntax allows direct modification of the returned value.
  constexpr gp_XYZ& ChangeCoord() noexcept { return coord; }

  //! Assigns the result of the following expression to this point
  //! (theAlpha*this + theBeta*theP) / (theAlpha + theBeta)
  constexpr void BaryCenter(const Standard_Real theAlpha,
                            const gp_Pnt&       theP,
                            const Standard_Real theBeta)
  {
    coord.SetLinearForm(theAlpha, coord, theBeta, theP.coord);
    coord.Divide(theAlpha + theBeta);
  }

  //! Comparison
  //! Returns True if the distance between the two points is
  //! lower or equal to theLinearTolerance.
  Standard_Boolean IsEqual(const gp_Pnt& theOther, const Standard_Real theLinearTolerance) const
  {
    return Distance(theOther) <= theLinearTolerance;
  }

  //! Computes the distance between two points.
  Standard_Real Distance(const gp_Pnt& theOther) const;

  //! Computes the square distance between two points.
  constexpr Standard_Real SquareDistance(const gp_Pnt& theOther) const noexcept;

  //! Performs the symmetrical transformation of a point
  //! with respect to the point theP which is the center of
  //! the symmetry.
  Standard_EXPORT void Mirror(const gp_Pnt& theP) noexcept;

  //! Performs the symmetrical transformation of a point
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Pnt Mirrored(const gp_Pnt& theP) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax1& theA1) noexcept;

  //! Performs the symmetrical transformation of a point
  //! with respect to a plane. The axis placement theA2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Pnt Mirrored(const gp_Ax1& theA1) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax2& theA2) noexcept;

  //! Rotates a point. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD Standard_EXPORT gp_Pnt Mirrored(const gp_Ax2& theA2) const noexcept;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng);

  Standard_NODISCARD gp_Pnt Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Pnt aP = *this;
    aP.Rotate(theA1, theAng);
    return aP;
  }

  //! Scales a point. theS is the scaling value.
  constexpr void Scale(const gp_Pnt& theP, const Standard_Real theS) noexcept;

  Standard_NODISCARD constexpr gp_Pnt Scaled(const gp_Pnt&       theP,
                                             const Standard_Real theS) const noexcept
  {
    gp_Pnt aPres = *this;
    aPres.Scale(theP, theS);
    return aPres;
  }

  //! Transforms a point with the transformation T.
  Standard_EXPORT void Transform(const gp_Trsf& theT);

  Standard_NODISCARD gp_Pnt Transformed(const gp_Trsf& theT) const
  {
    gp_Pnt aP = *this;
    aP.Transform(theT);
    return aP;
  }

  //! Translates a point in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  constexpr void Translate(const gp_Vec& theV) noexcept;

  Standard_NODISCARD constexpr gp_Pnt Translated(const gp_Vec& theV) const noexcept;

  //! Translates a point from the point theP1 to the point theP2.
  constexpr void Translate(const gp_Pnt& theP1, const gp_Pnt& theP2) noexcept
  {
    coord.Add(theP2.coord);
    coord.Subtract(theP1.coord);
  }

  Standard_NODISCARD constexpr gp_Pnt Translated(const gp_Pnt& theP1,
                                                 const gp_Pnt& theP2) const noexcept
  {
    gp_Pnt aP = *this;
    aP.Translate(theP1, theP2);
    return aP;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson(const Standard_SStream& theSStream,
                                                Standard_Integer&       theStreamPos);

private:
  gp_XYZ coord;
};

namespace std
{
template <>
struct hash<gp_Pnt>
{
  size_t operator()(const gp_Pnt& thePnt) const noexcept
  {
    union {
      Standard_Real    R[3];
      Standard_Integer I[6];
    } U;

    thePnt.Coord(U.R[0], U.R[1], U.R[2]);

    return std::hash<double>{}(U.I[0] / 23 + U.I[1] / 19 + U.I[2] / 17 + U.I[3] / 13 + U.I[4] / 11
                               + U.I[5] / 7);
  }
};

template <>
struct equal_to<gp_Pnt>
{
  bool operator()(const gp_Pnt& thePnt1, const gp_Pnt& thePnt2) const noexcept
  {
    if (std::abs(thePnt1.X() - thePnt2.X()) > Epsilon(thePnt2.X()))
      return false;
    if (std::abs(thePnt1.Y() - thePnt2.Y()) > Epsilon(thePnt2.Y()))
      return false;
    if (std::abs(thePnt1.Z() - thePnt2.Z()) > Epsilon(thePnt2.Z()))
      return false;
    return true;
  }
};
} // namespace std

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

//=================================================================================================

inline Standard_Real gp_Pnt::Distance(const gp_Pnt& theOther) const
{
  return sqrt(SquareDistance(theOther));
}

//=================================================================================================

inline constexpr Standard_Real gp_Pnt::SquareDistance(const gp_Pnt& theOther) const noexcept
{
  const gp_XYZ&       aXYZ = theOther.coord;
  const Standard_Real aDx  = coord.X() - aXYZ.X();
  const Standard_Real aDy  = coord.Y() - aXYZ.Y();
  const Standard_Real aDz  = coord.Z() - aXYZ.Z();
  return aDx * aDx + aDy * aDy + aDz * aDz;
}

//=================================================================================================

inline void gp_Pnt::Rotate(const gp_Ax1& theA1, const Standard_Real theAng)
{
  gp_Trsf aT;
  aT.SetRotation(theA1, theAng);
  aT.Transforms(coord);
}

//=================================================================================================

inline constexpr void gp_Pnt::Scale(const gp_Pnt& theP, const Standard_Real theS) noexcept
{
  gp_XYZ aXYZ = theP.coord;
  aXYZ.Multiply(1.0 - theS);
  coord.Multiply(theS);
  coord.Add(aXYZ);
}

//=================================================================================================

inline constexpr void gp_Pnt::Translate(const gp_Vec& theV) noexcept
{
  coord.Add(theV.XYZ());
}

//=================================================================================================

inline constexpr gp_Pnt gp_Pnt::Translated(const gp_Vec& theV) const noexcept
{
  gp_Pnt aP = *this;
  aP.coord.Add(theV.XYZ());
  return aP;
}

#endif // _gp_Pnt_HeaderFile
