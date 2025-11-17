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

#ifndef _gp_XY_HeaderFile
#define _gp_XY_HeaderFile

#include <gp.hxx>
#include <gp_Mat2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

//! This class describes a cartesian coordinate entity in 2D
//! space {X,Y}. This class is non persistent. This entity used
//! for algebraic calculation. An XY can be transformed with a
//! Trsf2d or a GTrsf2d from package gp.
//! It is used in vectorial computations or for holding this type
//! of information in data structures.
class gp_XY
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates XY object with zero coordinates (0,0).
  constexpr gp_XY() noexcept
      : x(0.),
        y(0.)
  {
  }

  //! a number pair defined by the XY coordinates
  constexpr gp_XY(const Standard_Real theX, const Standard_Real theY) noexcept
      : x(theX),
        y(theY)
  {
  }

  //! modifies the coordinate of range theIndex
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! Raises OutOfRange if theIndex != {1, 2}.
  inline void SetCoord(const Standard_Integer theIndex, const Standard_Real theXi)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 2, NULL);
    (&x)[theIndex - 1] = theXi;
  }

  //! For this number pair, assigns
  //! the values theX and theY to its coordinates
  constexpr void SetCoord(const Standard_Real theX, const Standard_Real theY) noexcept
  {
    x = theX;
    y = theY;
  }

  //! Assigns the given value to the X coordinate of this number pair.
  constexpr void SetX(const Standard_Real theX) noexcept { x = theX; }

  //! Assigns the given value to the Y coordinate of this number pair.
  constexpr void SetY(const Standard_Real theY) noexcept { y = theY; }

  //! returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! Raises OutOfRange if theIndex != {1, 2}.
  inline Standard_Real Coord(const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 2, NULL);
    return (&x)[theIndex - 1];
  }

  inline Standard_Real& ChangeCoord(const Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 2, NULL);
    return (&x)[theIndex - 1];
  }

  //! For this number pair, returns its coordinates X and Y.
  constexpr void Coord(Standard_Real& theX, Standard_Real& theY) const noexcept
  {
    theX = x;
    theY = y;
  }

  //! Returns the X coordinate of this number pair.
  constexpr Standard_Real X() const noexcept { return x; }

  //! Returns the Y coordinate of this number pair.
  constexpr Standard_Real Y() const noexcept { return y; }

  //! Computes std::sqrt(X*X + Y*Y) where X and Y are the two coordinates of this number pair.
  Standard_Real Modulus() const { return sqrt(SquareModulus()); }

  //! Computes X*X + Y*Y where X and Y are the two coordinates of this number pair.
  constexpr Standard_Real SquareModulus() const noexcept { return x * x + y * y; }

  //! Returns true if the coordinates of this number pair are
  //! equal to the respective coordinates of the number pair
  //! theOther, within the specified tolerance theTolerance.
  Standard_Boolean IsEqual(const gp_XY& theOther, const Standard_Real theTolerance) const
  {
    return (std::abs(x - theOther.x) < theTolerance) && (std::abs(y - theOther.y) < theTolerance);
  }

  //! Computes the sum of this number pair and number pair theOther
  //! @code
  //! <me>.X() = <me>.X() + theOther.X()
  //! <me>.Y() = <me>.Y() + theOther.Y()
  //! @endcode
  constexpr void Add(const gp_XY& theOther) noexcept
  {
    x += theOther.x;
    y += theOther.y;
  }

  constexpr void operator+=(const gp_XY& theOther) noexcept { Add(theOther); }

  //! Computes the sum of this number pair and number pair theOther
  //! @code
  //! new.X() = <me>.X() + theOther.X()
  //! new.Y() = <me>.Y() + theOther.Y()
  //! @endcode
  Standard_NODISCARD constexpr gp_XY Added(const gp_XY& theOther) const noexcept
  {
    return gp_XY(x + theOther.X(), y + theOther.Y());
  }

  Standard_NODISCARD constexpr gp_XY operator+(const gp_XY& theOther) const noexcept
  {
    return Added(theOther);
  }

  //! @code
  //! double D = <me>.X() * theOther.Y() - <me>.Y() * theOther.X()
  //! @endcode
  Standard_NODISCARD constexpr Standard_Real Crossed(const gp_XY& theOther) const noexcept
  {
    return x * theOther.y - y * theOther.x;
  }

  Standard_NODISCARD constexpr Standard_Real operator^(const gp_XY& theOther) const noexcept
  {
    return Crossed(theOther);
  }

  //! computes the magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||
  Standard_Real CrossMagnitude(const gp_XY& theRight) const
  {
    return std::abs(x * theRight.y - y * theRight.x);
  }

  //! computes the square magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||**2
  constexpr Standard_Real CrossSquareMagnitude(const gp_XY& theRight) const noexcept
  {
    const Standard_Real aZresult = x * theRight.y - y * theRight.x;
    return aZresult * aZresult;
  }

  //! divides <me> by a real.
  constexpr void Divide(const Standard_Real theScalar)
  {
    x /= theScalar;
    y /= theScalar;
  }

  constexpr void operator/=(const Standard_Real theScalar) { Divide(theScalar); }

  //! Divides <me> by a real.
  Standard_NODISCARD constexpr gp_XY Divided(const Standard_Real theScalar) const
  {
    return gp_XY(x / theScalar, y / theScalar);
  }

  Standard_NODISCARD constexpr gp_XY operator/(const Standard_Real theScalar) const
  {
    return Divided(theScalar);
  }

  //! Computes the scalar product between <me> and theOther
  constexpr Standard_Real Dot(const gp_XY& theOther) const noexcept
  {
    return x * theOther.x + y * theOther.y;
  }

  constexpr Standard_Real operator*(const gp_XY& theOther) const noexcept { return Dot(theOther); }

  //! @code
  //! <me>.X() = <me>.X() * theScalar;
  //! <me>.Y() = <me>.Y() * theScalar;
  //! @endcode
  constexpr void Multiply(const Standard_Real theScalar) noexcept
  {
    x *= theScalar;
    y *= theScalar;
  }

  constexpr void operator*=(const Standard_Real theScalar) noexcept { Multiply(theScalar); }

  //! @code
  //! <me>.X() = <me>.X() * theOther.X();
  //! <me>.Y() = <me>.Y() * theOther.Y();
  //! @endcode
  constexpr void Multiply(const gp_XY& theOther) noexcept
  {
    x *= theOther.x;
    y *= theOther.y;
  }

  constexpr void operator*=(const gp_XY& theOther) noexcept { Multiply(theOther); }

  //! <me> = theMatrix * <me>
  void Multiply(const gp_Mat2d& theMatrix) noexcept;

  void operator*=(const gp_Mat2d& theMatrix) noexcept { Multiply(theMatrix); }

  //! @code
  //! New.X() = <me>.X() * theScalar;
  //! New.Y() = <me>.Y() * theScalar;
  //! @endcode
  Standard_NODISCARD constexpr gp_XY Multiplied(const Standard_Real theScalar) const noexcept
  {
    return gp_XY(x * theScalar, y * theScalar);
  }

  Standard_NODISCARD constexpr gp_XY operator*(const Standard_Real theScalar) const noexcept
  {
    return Multiplied(theScalar);
  }

  //! @code
  //! new.X() = <me>.X() * theOther.X();
  //! new.Y() = <me>.Y() * theOther.Y();
  //! @endcode
  Standard_NODISCARD constexpr gp_XY Multiplied(const gp_XY& theOther) const noexcept
  {
    return gp_XY(x * theOther.X(), y * theOther.Y());
  }

  //! New = theMatrix * <me>
  Standard_NODISCARD gp_XY Multiplied(const gp_Mat2d& theMatrix) const noexcept
  {
    return gp_XY(theMatrix.myMat[0][0] * x + theMatrix.myMat[0][1] * y,
                 theMatrix.myMat[1][0] * x + theMatrix.myMat[1][1] * y);
  }

  Standard_NODISCARD gp_XY operator*(const gp_Mat2d& theMatrix) const noexcept
  {
    return Multiplied(theMatrix);
  }

  //! @code
  //! <me>.X() = <me>.X()/ <me>.Modulus()
  //! <me>.Y() = <me>.Y()/ <me>.Modulus()
  //! @endcode
  //! Raises ConstructionError if <me>.Modulus() <= Resolution from gp
  void Normalize();

  //! @code
  //! New.X() = <me>.X()/ <me>.Modulus()
  //! New.Y() = <me>.Y()/ <me>.Modulus()
  //! @endcode
  //! Raises ConstructionError if <me>.Modulus() <= Resolution from gp
  Standard_NODISCARD gp_XY Normalized() const
  {
    Standard_Real aD = Modulus();
    Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                        "gp_XY::Normalized() - vector has zero norm");
    return gp_XY(x / aD, y / aD);
  }

  //! @code
  //! <me>.X() = -<me>.X()
  //! <me>.Y() = -<me>.Y()
  constexpr void Reverse() noexcept
  {
    x = -x;
    y = -y;
  }

  //! @code
  //! New.X() = -<me>.X()
  //! New.Y() = -<me>.Y()
  //! @endcode
  Standard_NODISCARD constexpr gp_XY Reversed() const noexcept { return gp_XY(-x, -y); }

  Standard_NODISCARD constexpr gp_XY operator-() const noexcept { return Reversed(); }

  //! Computes the following linear combination and
  //! assigns the result to this number pair:
  //! @code
  //! theA1 * theXY1 + theA2 * theXY2
  //! @endcode
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_XY&        theXY1,
                               const Standard_Real theA2,
                               const gp_XY&        theXY2) noexcept
  {
    x = theA1 * theXY1.x + theA2 * theXY2.x;
    y = theA1 * theXY1.y + theA2 * theXY2.y;
  }

  //! Computes the following linear combination and
  //! assigns the result to this number pair:
  //! @code
  //! theA1 * theXY1 + theA2 * theXY2 + theXY3
  //! @endcode
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_XY&        theXY1,
                               const Standard_Real theA2,
                               const gp_XY&        theXY2,
                               const gp_XY&        theXY3) noexcept
  {
    x = theA1 * theXY1.x + theA2 * theXY2.x + theXY3.x;
    y = theA1 * theXY1.y + theA2 * theXY2.y + theXY3.y;
  }

  //! Computes the following linear combination and
  //! assigns the result to this number pair:
  //! @code
  //! theA1 * theXY1 + theXY2
  //! @endcode
  constexpr void SetLinearForm(const Standard_Real theA1,
                               const gp_XY&        theXY1,
                               const gp_XY&        theXY2) noexcept
  {
    x = theA1 * theXY1.x + theXY2.x;
    y = theA1 * theXY1.y + theXY2.y;
  }

  //! Computes the following linear combination and
  //! assigns the result to this number pair:
  //! @code
  //! theXY1 + theXY2
  //! @endcode
  constexpr void SetLinearForm(const gp_XY& theXY1, const gp_XY& theXY2) noexcept
  {
    x = theXY1.x + theXY2.x;
    y = theXY1.y + theXY2.y;
  }

  //! @code
  //! <me>.X() = <me>.X() - theOther.X()
  //! <me>.Y() = <me>.Y() - theOther.Y()
  //! @endcode
  constexpr void Subtract(const gp_XY& theOther) noexcept
  {
    x -= theOther.x;
    y -= theOther.y;
  }

  constexpr void operator-=(const gp_XY& theOther) noexcept { Subtract(theOther); }

  //! @code
  //! new.X() = <me>.X() - theOther.X()
  //! new.Y() = <me>.Y() - theOther.Y()
  //! @endcode
  Standard_NODISCARD constexpr gp_XY Subtracted(const gp_XY& theOther) const noexcept
  {
    return gp_XY(x - theOther.x, y - theOther.y);
  }

  Standard_NODISCARD constexpr gp_XY operator-(const gp_XY& theOther) const noexcept
  {
    return Subtracted(theOther);
  }

private:
  Standard_Real x;
  Standard_Real y;
};

//=================================================================================================

inline void gp_XY::Multiply(const gp_Mat2d& theMatrix) noexcept
{
  const Standard_Real aXresult = theMatrix.myMat[0][0] * x + theMatrix.myMat[0][1] * y;
  y                            = theMatrix.myMat[1][0] * x + theMatrix.myMat[1][1] * y;
  x                            = aXresult;
}

//=================================================================================================

inline void gp_XY::Normalize()
{
  Standard_Real aD = Modulus();
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_XY::Normalize() - vector has zero norm");
  x = x / aD;
  y = y / aD;
}

//=======================================================================
// function :  operator*
// purpose :
//=======================================================================
inline gp_XY operator*(const gp_Mat2d& theMatrix, const gp_XY& theCoord1) noexcept
{
  return theCoord1.Multiplied(theMatrix);
}

//=======================================================================
// function :  operator*
// purpose :
//=======================================================================
inline constexpr gp_XY operator*(const Standard_Real theScalar, const gp_XY& theCoord1) noexcept
{
  return theCoord1.Multiplied(theScalar);
}

#endif // _gp_XY_HeaderFile
