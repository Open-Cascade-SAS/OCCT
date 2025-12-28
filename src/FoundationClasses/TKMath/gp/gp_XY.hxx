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
  constexpr gp_XY(const double theX, const double theY) noexcept
      : x(theX),
        y(theY)
  {
  }

  //! modifies the coordinate of range theIndex
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! Raises OutOfRange if theIndex != {1, 2}.
  constexpr void SetCoord(const int theIndex, const double theXi)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 2, NULL);
    if (theIndex == 1)
    {
      x = theXi;
    }
    else
    {
      y = theXi;
    }
  }

  //! For this number pair, assigns
  //! the values theX and theY to its coordinates
  constexpr void SetCoord(const double theX, const double theY) noexcept
  {
    x = theX;
    y = theY;
  }

  //! Assigns the given value to the X coordinate of this number pair.
  constexpr void SetX(const double theX) noexcept { x = theX; }

  //! Assigns the given value to the Y coordinate of this number pair.
  constexpr void SetY(const double theY) noexcept { y = theY; }

  //! returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! Raises OutOfRange if theIndex != {1, 2}.
  constexpr double Coord(const int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 2, NULL);
    if (theIndex == 1)
    {
      return x;
    }
    return y;
  }

  constexpr double& ChangeCoord(const int theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 2, NULL);
    if (theIndex == 1)
    {
      return x;
    }
    return y;
  }

  //! For this number pair, returns its coordinates X and Y.
  constexpr void Coord(double& theX, double& theY) const noexcept
  {
    theX = x;
    theY = y;
  }

  //! Returns the X coordinate of this number pair.
  constexpr double X() const noexcept { return x; }

  //! Returns the Y coordinate of this number pair.
  constexpr double Y() const noexcept { return y; }

  //! Computes std::sqrt(X*X + Y*Y) where X and Y are the two coordinates of this number pair.
  double Modulus() const { return sqrt(SquareModulus()); }

  //! Computes X*X + Y*Y where X and Y are the two coordinates of this number pair.
  constexpr double SquareModulus() const noexcept { return x * x + y * y; }

  //! Returns true if the coordinates of this number pair are
  //! equal to the respective coordinates of the number pair
  //! theOther, within the specified tolerance theTolerance.
  bool IsEqual(const gp_XY& theOther, const double theTolerance) const
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
  [[nodiscard]] constexpr gp_XY Added(const gp_XY& theOther) const noexcept
  {
    return gp_XY(x + theOther.X(), y + theOther.Y());
  }

  [[nodiscard]] constexpr gp_XY operator+(const gp_XY& theOther) const noexcept
  {
    return Added(theOther);
  }

  //! @code
  //! double D = <me>.X() * theOther.Y() - <me>.Y() * theOther.X()
  //! @endcode
  [[nodiscard]] constexpr double Crossed(const gp_XY& theOther) const noexcept
  {
    return x * theOther.y - y * theOther.x;
  }

  [[nodiscard]] constexpr double operator^(const gp_XY& theOther) const noexcept
  {
    return Crossed(theOther);
  }

  //! computes the magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||
  double CrossMagnitude(const gp_XY& theRight) const
  {
    return std::abs(x * theRight.y - y * theRight.x);
  }

  //! computes the square magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||**2
  constexpr double CrossSquareMagnitude(const gp_XY& theRight) const noexcept
  {
    const double aZresult = x * theRight.y - y * theRight.x;
    return aZresult * aZresult;
  }

  //! divides <me> by a real.
  constexpr void Divide(const double theScalar)
  {
    x /= theScalar;
    y /= theScalar;
  }

  constexpr void operator/=(const double theScalar) { Divide(theScalar); }

  //! Divides <me> by a real.
  [[nodiscard]] constexpr gp_XY Divided(const double theScalar) const
  {
    return gp_XY(x / theScalar, y / theScalar);
  }

  [[nodiscard]] constexpr gp_XY operator/(const double theScalar) const
  {
    return Divided(theScalar);
  }

  //! Computes the scalar product between <me> and theOther
  constexpr double Dot(const gp_XY& theOther) const noexcept
  {
    return x * theOther.x + y * theOther.y;
  }

  constexpr double operator*(const gp_XY& theOther) const noexcept { return Dot(theOther); }

  //! @code
  //! <me>.X() = <me>.X() * theScalar;
  //! <me>.Y() = <me>.Y() * theScalar;
  //! @endcode
  constexpr void Multiply(const double theScalar) noexcept
  {
    x *= theScalar;
    y *= theScalar;
  }

  constexpr void operator*=(const double theScalar) noexcept { Multiply(theScalar); }

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
  constexpr void Multiply(const gp_Mat2d& theMatrix) noexcept;

  constexpr void operator*=(const gp_Mat2d& theMatrix) noexcept { Multiply(theMatrix); }

  //! @code
  //! New.X() = <me>.X() * theScalar;
  //! New.Y() = <me>.Y() * theScalar;
  //! @endcode
  [[nodiscard]] constexpr gp_XY Multiplied(const double theScalar) const noexcept
  {
    return gp_XY(x * theScalar, y * theScalar);
  }

  [[nodiscard]] constexpr gp_XY operator*(const double theScalar) const noexcept
  {
    return Multiplied(theScalar);
  }

  //! @code
  //! new.X() = <me>.X() * theOther.X();
  //! new.Y() = <me>.Y() * theOther.Y();
  //! @endcode
  [[nodiscard]] constexpr gp_XY Multiplied(const gp_XY& theOther) const noexcept
  {
    return gp_XY(x * theOther.X(), y * theOther.Y());
  }

  //! New = theMatrix * <me>
  [[nodiscard]] constexpr gp_XY Multiplied(const gp_Mat2d& theMatrix) const noexcept
  {
    return gp_XY(theMatrix.myMat[0][0] * x + theMatrix.myMat[0][1] * y,
                 theMatrix.myMat[1][0] * x + theMatrix.myMat[1][1] * y);
  }

  [[nodiscard]] constexpr gp_XY operator*(const gp_Mat2d& theMatrix) const noexcept
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
  [[nodiscard]] gp_XY Normalized() const
  {
    double aD = Modulus();
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
  [[nodiscard]] constexpr gp_XY Reversed() const noexcept { return gp_XY(-x, -y); }

  [[nodiscard]] constexpr gp_XY operator-() const noexcept { return Reversed(); }

  //! Computes the following linear combination and
  //! assigns the result to this number pair:
  //! @code
  //! theA1 * theXY1 + theA2 * theXY2
  //! @endcode
  constexpr void SetLinearForm(const double theA1,
                               const gp_XY&        theXY1,
                               const double theA2,
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
  constexpr void SetLinearForm(const double theA1,
                               const gp_XY&        theXY1,
                               const double theA2,
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
  constexpr void SetLinearForm(const double theA1,
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
  [[nodiscard]] constexpr gp_XY Subtracted(const gp_XY& theOther) const noexcept
  {
    return gp_XY(x - theOther.x, y - theOther.y);
  }

  [[nodiscard]] constexpr gp_XY operator-(const gp_XY& theOther) const noexcept
  {
    return Subtracted(theOther);
  }

private:
  double x;
  double y;
};

//=================================================================================================

inline constexpr void gp_XY::Multiply(const gp_Mat2d& theMatrix) noexcept
{
  const double aXresult = theMatrix.myMat[0][0] * x + theMatrix.myMat[0][1] * y;
  y                            = theMatrix.myMat[1][0] * x + theMatrix.myMat[1][1] * y;
  x                            = aXresult;
}

//=================================================================================================

inline void gp_XY::Normalize()
{
  double aD = Modulus();
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_XY::Normalize() - vector has zero norm");
  x = x / aD;
  y = y / aD;
}

//=======================================================================
// function :  operator*
// purpose :
//=======================================================================
inline constexpr gp_XY operator*(const gp_Mat2d& theMatrix, const gp_XY& theCoord1) noexcept
{
  return theCoord1.Multiplied(theMatrix);
}

//=======================================================================
// function :  operator*
// purpose :
//=======================================================================
inline constexpr gp_XY operator*(const double theScalar, const gp_XY& theCoord1) noexcept
{
  return theCoord1.Multiplied(theScalar);
}

#endif // _gp_XY_HeaderFile
