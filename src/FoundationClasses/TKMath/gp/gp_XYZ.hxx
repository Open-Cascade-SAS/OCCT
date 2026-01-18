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

#ifndef _gp_XYZ_HeaderFile
#define _gp_XYZ_HeaderFile

#include <gp.hxx>
#include <gp_Mat.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>

#include <cmath>

//! This class describes a cartesian coordinate entity in
//! 3D space {X,Y,Z}. This entity is used for algebraic
//! calculation. This entity can be transformed
//! with a "Trsf" or a "GTrsf" from package "gp".
//! It is used in vectorial computations or for holding this type
//! of information in data structures.
class gp_XYZ
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an XYZ object with zero coordinates (0,0,0)
  constexpr gp_XYZ() noexcept
      : x(0.),
        y(0.),
        z(0.)
  {
  }

  //! creates an XYZ with given coordinates
  constexpr gp_XYZ(const double theX, const double theY, const double theZ) noexcept
      : x(theX),
        y(theY),
        z(theZ)
  {
  }

  //! For this XYZ object, assigns
  //! the values theX, theY and theZ to its three coordinates
  constexpr void SetCoord(const double theX, const double theY, const double theZ) noexcept
  {
    x = theX;
    y = theY;
    z = theZ;
  }

  //! modifies the coordinate of range theIndex
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! theIndex = 3 => Z is modified
  //! Raises OutOfRange if theIndex != {1, 2, 3}.
  constexpr void SetCoord(const int theIndex, const double theXi)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 3, nullptr);
    if (theIndex == 1)
    {
      x = theXi;
    }
    else if (theIndex == 2)
    {
      y = theXi;
    }
    else
    {
      z = theXi;
    }
  }

  //! Assigns the given value to the X coordinate
  constexpr void SetX(const double theX) noexcept { x = theX; }

  //! Assigns the given value to the Y coordinate
  constexpr void SetY(const double theY) noexcept { y = theY; }

  //! Assigns the given value to the Z coordinate
  constexpr void SetZ(const double theZ) noexcept { z = theZ; }

  //! returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! theIndex = 3 => Z is returned
  //!
  //! Raises OutOfRange if theIndex != {1, 2, 3}.
  constexpr double Coord(const int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 3, nullptr);
    if (theIndex == 1)
    {
      return x;
    }
    else if (theIndex == 2)
    {
      return y;
    }
    return z;
  }

  constexpr double& ChangeCoord(const int theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > 3, nullptr);
    if (theIndex == 1)
    {
      return x;
    }
    else if (theIndex == 2)
    {
      return y;
    }
    return z;
  }

  constexpr void Coord(double& theX, double& theY, double& theZ) const noexcept
  {
    theX = x;
    theY = y;
    theZ = z;
  }

  //! Returns a const ptr to coordinates location.
  //! Is useful for algorithms, but DOES NOT PERFORM
  //! ANY CHECKS!
  constexpr const double* GetData() const noexcept { return (&x); }

  //! Returns a ptr to coordinates location.
  //! Is useful for algorithms, but DOES NOT PERFORM
  //! ANY CHECKS!
  double* ChangeData() noexcept { return (&x); }

  //! Returns the X coordinate
  constexpr double X() const noexcept { return x; }

  //! Returns the Y coordinate
  constexpr double Y() const noexcept { return y; }

  //! Returns the Z coordinate
  constexpr double Z() const noexcept { return z; }

  //! Computes std::sqrt(X*X + Y*Y + Z*Z) where X, Y and Z are the three coordinates of this XYZ
  //! object.
  double Modulus() const { return std::sqrt(x * x + y * y + z * z); }

  //! Computes X*X + Y*Y + Z*Z where X, Y and Z are the three coordinates of this XYZ object.
  constexpr double SquareModulus() const noexcept { return (x * x + y * y + z * z); }

  //! Returns True if he coordinates of this XYZ object are
  //! equal to the respective coordinates Other,
  //! within the specified tolerance theTolerance.
  bool IsEqual(const gp_XYZ& theOther, const double theTolerance) const

  {
    return (std::abs(x - theOther.x) < theTolerance) && (std::abs(y - theOther.y) < theTolerance)
           && (std::abs(z - theOther.z) < theTolerance);
  }

  //! @code
  //! <me>.X() = <me>.X() + theOther.X()
  //! <me>.Y() = <me>.Y() + theOther.Y()
  //! <me>.Z() = <me>.Z() + theOther.Z()
  //! @endcode
  constexpr void Add(const gp_XYZ& theOther) noexcept
  {
    x += theOther.x;
    y += theOther.y;
    z += theOther.z;
  }

  constexpr void operator+=(const gp_XYZ& theOther) noexcept { Add(theOther); }

  //! @code
  //! new.X() = <me>.X() + theOther.X()
  //! new.Y() = <me>.Y() + theOther.Y()
  //! new.Z() = <me>.Z() + theOther.Z()
  //! @endcode
  [[nodiscard]] constexpr gp_XYZ Added(const gp_XYZ& theOther) const noexcept
  {
    return gp_XYZ(x + theOther.x, y + theOther.y, z + theOther.z);
  }

  [[nodiscard]] constexpr gp_XYZ operator+(const gp_XYZ& theOther) const noexcept
  {
    return Added(theOther);
  }

  //! @code
  //! <me>.X() = <me>.Y() * theOther.Z() - <me>.Z() * theOther.Y()
  //! <me>.Y() = <me>.Z() * theOther.X() - <me>.X() * theOther.Z()
  //! <me>.Z() = <me>.X() * theOther.Y() - <me>.Y() * theOther.X()
  //! @endcode
  constexpr void Cross(const gp_XYZ& theOther) noexcept;

  constexpr void operator^=(const gp_XYZ& theOther) noexcept { Cross(theOther); }

  //! @code
  //! new.X() = <me>.Y() * theOther.Z() - <me>.Z() * theOther.Y()
  //! new.Y() = <me>.Z() * theOther.X() - <me>.X() * theOther.Z()
  //! new.Z() = <me>.X() * theOther.Y() - <me>.Y() * theOther.X()
  //! @endcode
  [[nodiscard]] constexpr gp_XYZ Crossed(const gp_XYZ& theOther) const noexcept
  {
    return gp_XYZ(y * theOther.z - z * theOther.y,
                  z * theOther.x - x * theOther.z,
                  x * theOther.y - y * theOther.x);
  }

  [[nodiscard]] constexpr gp_XYZ operator^(const gp_XYZ& theOther) const noexcept
  {
    return Crossed(theOther);
  }

  //! Computes the magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||
  double CrossMagnitude(const gp_XYZ& theRight) const;

  //! Computes the square magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||**2
  constexpr double CrossSquareMagnitude(const gp_XYZ& theRight) const noexcept;

  //! Triple vector product
  //! Computes <me> = <me>.Cross(theCoord1.Cross(theCoord2))
  constexpr void CrossCross(const gp_XYZ& theCoord1, const gp_XYZ& theCoord2) noexcept;

  //! Triple vector product
  //! computes New = <me>.Cross(theCoord1.Cross(theCoord2))
  [[nodiscard]] constexpr gp_XYZ CrossCrossed(const gp_XYZ& theCoord1,
                                              const gp_XYZ& theCoord2) const noexcept
  {
    gp_XYZ aCoord0 = *this;
    aCoord0.CrossCross(theCoord1, theCoord2);
    return aCoord0;
  }

  //! divides <me> by a real.
  constexpr void Divide(const double theScalar)
  {
    x /= theScalar;
    y /= theScalar;
    z /= theScalar;
  }

  constexpr void operator/=(const double theScalar) { Divide(theScalar); }

  //! divides <me> by a real.
  [[nodiscard]] constexpr gp_XYZ Divided(const double theScalar) const
  {
    return gp_XYZ(x / theScalar, y / theScalar, z / theScalar);
  }

  [[nodiscard]] constexpr gp_XYZ operator/(const double theScalar) const
  {
    return Divided(theScalar);
  }

  //! Computes the scalar product between <me> and theOther.
  constexpr double Dot(const gp_XYZ& theOther) const noexcept
  {
    return (x * theOther.x + y * theOther.y + z * theOther.z);
  }

  constexpr double operator*(const gp_XYZ& theOther) const noexcept { return Dot(theOther); }

  //! Computes the triple scalar product.
  constexpr double DotCross(const gp_XYZ& theCoord1, const gp_XYZ& theCoord2) const noexcept;

  //! @code
  //! <me>.X() = <me>.X() * theScalar;
  //! <me>.Y() = <me>.Y() * theScalar;
  //! <me>.Z() = <me>.Z() * theScalar;
  //! @endcode
  constexpr void Multiply(const double theScalar) noexcept
  {
    x *= theScalar;
    y *= theScalar;
    z *= theScalar;
  }

  constexpr void operator*=(const double theScalar) noexcept { Multiply(theScalar); }

  //! @code
  //! <me>.X() = <me>.X() * theOther.X();
  //! <me>.Y() = <me>.Y() * theOther.Y();
  //! <me>.Z() = <me>.Z() * theOther.Z();
  //! @endcode
  constexpr void Multiply(const gp_XYZ& theOther) noexcept
  {
    x *= theOther.x;
    y *= theOther.y;
    z *= theOther.z;
  }

  constexpr void operator*=(const gp_XYZ& theOther) noexcept { Multiply(theOther); }

  //! <me> = theMatrix * <me>
  constexpr void Multiply(const gp_Mat& theMatrix) noexcept;

  constexpr void operator*=(const gp_Mat& theMatrix) noexcept { Multiply(theMatrix); }

  //! @code
  //! New.X() = <me>.X() * theScalar;
  //! New.Y() = <me>.Y() * theScalar;
  //! New.Z() = <me>.Z() * theScalar;
  //! @endcode
  [[nodiscard]] constexpr gp_XYZ Multiplied(const double theScalar) const noexcept
  {
    return gp_XYZ(x * theScalar, y * theScalar, z * theScalar);
  }

  [[nodiscard]] constexpr gp_XYZ operator*(const double theScalar) const noexcept
  {
    return Multiplied(theScalar);
  }

  //! @code
  //! new.X() = <me>.X() * theOther.X();
  //! new.Y() = <me>.Y() * theOther.Y();
  //! new.Z() = <me>.Z() * theOther.Z();
  //! @endcode
  [[nodiscard]] constexpr gp_XYZ Multiplied(const gp_XYZ& theOther) const noexcept
  {
    return gp_XYZ(x * theOther.x, y * theOther.y, z * theOther.z);
  }

  //! New = theMatrix * <me>
  [[nodiscard]] constexpr gp_XYZ Multiplied(const gp_Mat& theMatrix) const noexcept
  {
    // Direct access to matrix data for optimal performance (gp_XYZ is friend of gp_Mat)
    return gp_XYZ(theMatrix.myMat[0][0] * x + theMatrix.myMat[0][1] * y + theMatrix.myMat[0][2] * z,
                  theMatrix.myMat[1][0] * x + theMatrix.myMat[1][1] * y + theMatrix.myMat[1][2] * z,
                  theMatrix.myMat[2][0] * x + theMatrix.myMat[2][1] * y
                    + theMatrix.myMat[2][2] * z);
  }

  [[nodiscard]] constexpr gp_XYZ operator*(const gp_Mat& theMatrix) const noexcept
  {
    return Multiplied(theMatrix);
  }

  //! @code
  //! <me>.X() = <me>.X()/ <me>.Modulus()
  //! <me>.Y() = <me>.Y()/ <me>.Modulus()
  //! <me>.Z() = <me>.Z()/ <me>.Modulus()
  //! @endcode
  //! Raised if <me>.Modulus() <= Resolution from gp
  void Normalize();

  //! @code
  //! New.X() = <me>.X()/ <me>.Modulus()
  //! New.Y() = <me>.Y()/ <me>.Modulus()
  //! New.Z() = <me>.Z()/ <me>.Modulus()
  //! @endcode
  //! Raised if <me>.Modulus() <= Resolution from gp
  [[nodiscard]] gp_XYZ Normalized() const
  {
    const double aD = Modulus();
    Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                        "gp_XYZ::Normalized() - vector has zero norm");
    return gp_XYZ(x / aD, y / aD, z / aD);
  }

  //! @code
  //! <me>.X() = -<me>.X()
  //! <me>.Y() = -<me>.Y()
  //! <me>.Z() = -<me>.Z()
  //! @endcode
  constexpr void Reverse() noexcept
  {
    x = -x;
    y = -y;
    z = -z;
  }

  //! @code
  //! New.X() = -<me>.X()
  //! New.Y() = -<me>.Y()
  //! New.Z() = -<me>.Z()
  //! @endcode
  [[nodiscard]] constexpr gp_XYZ Reversed() const noexcept { return gp_XYZ(-x, -y, -z); }

  //! @code
  //! <me>.X() = <me>.X() - theOther.X()
  //! <me>.Y() = <me>.Y() - theOther.Y()
  //! <me>.Z() = <me>.Z() - theOther.Z()
  //! @endcode
  constexpr void Subtract(const gp_XYZ& theOther) noexcept
  {
    x -= theOther.x;
    y -= theOther.y;
    z -= theOther.z;
  }

  constexpr void operator-=(const gp_XYZ& theOther) noexcept { Subtract(theOther); }

  //! @code
  //! new.X() = <me>.X() - theOther.X()
  //! new.Y() = <me>.Y() - theOther.Y()
  //! new.Z() = <me>.Z() - theOther.Z()
  //! @endcode
  [[nodiscard]] constexpr gp_XYZ Subtracted(const gp_XYZ& theOther) const noexcept
  {
    return gp_XYZ(x - theOther.x, y - theOther.y, z - theOther.z);
  }

  [[nodiscard]] constexpr gp_XYZ operator-(const gp_XYZ& theOther) const noexcept
  {
    return Subtracted(theOther);
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2 + theA3 * theXYZ3 + theXYZ4
  //! @endcode
  constexpr void SetLinearForm(const double  theA1,
                               const gp_XYZ& theXYZ1,
                               const double  theA2,
                               const gp_XYZ& theXYZ2,
                               const double  theA3,
                               const gp_XYZ& theXYZ3,
                               const gp_XYZ& theXYZ4) noexcept
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x + theA3 * theXYZ3.x + theXYZ4.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y + theA3 * theXYZ3.y + theXYZ4.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z + theA3 * theXYZ3.z + theXYZ4.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2 + theA3 * theXYZ3
  //! @endcode
  constexpr void SetLinearForm(const double  theA1,
                               const gp_XYZ& theXYZ1,
                               const double  theA2,
                               const gp_XYZ& theXYZ2,
                               const double  theA3,
                               const gp_XYZ& theXYZ3) noexcept
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x + theA3 * theXYZ3.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y + theA3 * theXYZ3.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z + theA3 * theXYZ3.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2 + theXYZ3
  //! @endcode
  constexpr void SetLinearForm(const double  theA1,
                               const gp_XYZ& theXYZ1,
                               const double  theA2,
                               const gp_XYZ& theXYZ2,
                               const gp_XYZ& theXYZ3) noexcept
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x + theXYZ3.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y + theXYZ3.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z + theXYZ3.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theA2 * theXYZ2
  //! @endcode
  constexpr void SetLinearForm(const double  theA1,
                               const gp_XYZ& theXYZ1,
                               const double  theA2,
                               const gp_XYZ& theXYZ2) noexcept
  {
    x = theA1 * theXYZ1.x + theA2 * theXYZ2.x;
    y = theA1 * theXYZ1.y + theA2 * theXYZ2.y;
    z = theA1 * theXYZ1.z + theA2 * theXYZ2.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theA1 * theXYZ1 + theXYZ2
  //! @endcode
  constexpr void SetLinearForm(const double  theA1,
                               const gp_XYZ& theXYZ1,
                               const gp_XYZ& theXYZ2) noexcept
  {
    x = theA1 * theXYZ1.x + theXYZ2.x;
    y = theA1 * theXYZ1.y + theXYZ2.y;
    z = theA1 * theXYZ1.z + theXYZ2.z;
  }

  //! <me> is set to the following linear form :
  //! @code
  //! theXYZ1 + theXYZ2
  //! @endcode
  constexpr void SetLinearForm(const gp_XYZ& theXYZ1, const gp_XYZ& theXYZ2) noexcept
  {
    x = theXYZ1.x + theXYZ2.x;
    y = theXYZ1.y + theXYZ2.y;
    z = theXYZ1.z + theXYZ2.z;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT bool InitFromJson(const Standard_SStream& theSStream, int& theStreamPos);

private:
  double x;
  double y;
  double z;
};

//=================================================================================================

inline constexpr void gp_XYZ::Cross(const gp_XYZ& theRight) noexcept
{
  const double aXresult = y * theRight.z - z * theRight.y;
  const double aYresult = z * theRight.x - x * theRight.z;
  z                     = x * theRight.y - y * theRight.x;
  x                     = aXresult;
  y                     = aYresult;
}

//=================================================================================================

inline double gp_XYZ::CrossMagnitude(const gp_XYZ& theRight) const
{
  return sqrt(CrossSquareMagnitude(theRight));
}

//=================================================================================================

inline constexpr double gp_XYZ::CrossSquareMagnitude(const gp_XYZ& theRight) const noexcept
{
  const double aXresult = y * theRight.z - z * theRight.y;
  const double aYresult = z * theRight.x - x * theRight.z;
  const double aZresult = x * theRight.y - y * theRight.x;
  return aXresult * aXresult + aYresult * aYresult + aZresult * aZresult;
}

//=================================================================================================

inline constexpr void gp_XYZ::CrossCross(const gp_XYZ& theCoord1, const gp_XYZ& theCoord2) noexcept
{
  // First compute theCoord1 * theCoord2
  const double aCrossX = theCoord1.y * theCoord2.z - theCoord1.z * theCoord2.y;
  const double aCrossY = theCoord1.z * theCoord2.x - theCoord1.x * theCoord2.z;
  const double aCrossZ = theCoord1.x * theCoord2.y - theCoord1.y * theCoord2.x;

  // Then compute this * (theCoord1 * theCoord2)
  const double aXresult = y * aCrossZ - z * aCrossY;
  const double aYresult = z * aCrossX - x * aCrossZ;

  z = x * aCrossY - y * aCrossX;
  x = aXresult;
  y = aYresult;
}

//=================================================================================================

inline constexpr double gp_XYZ::DotCross(const gp_XYZ& theCoord1,
                                         const gp_XYZ& theCoord2) const noexcept
{
  const double aXresult  = theCoord1.y * theCoord2.z - theCoord1.z * theCoord2.y;
  const double anYresult = theCoord1.z * theCoord2.x - theCoord1.x * theCoord2.z;
  const double aZresult  = theCoord1.x * theCoord2.y - theCoord1.y * theCoord2.x;
  return (x * aXresult + y * anYresult + z * aZresult);
}

//=================================================================================================

inline constexpr void gp_XYZ::Multiply(const gp_Mat& theMatrix) noexcept
{
  // Cache original coordinates to avoid aliasing issues
  const double aOrigX = x;
  const double aOrigY = y;
  const double aOrigZ = z;

  // Matrix-vector multiplication: this = theMatrix * this
  x = theMatrix.myMat[0][0] * aOrigX + theMatrix.myMat[0][1] * aOrigY
      + theMatrix.myMat[0][2] * aOrigZ;
  y = theMatrix.myMat[1][0] * aOrigX + theMatrix.myMat[1][1] * aOrigY
      + theMatrix.myMat[1][2] * aOrigZ;
  z = theMatrix.myMat[2][0] * aOrigX + theMatrix.myMat[2][1] * aOrigY
      + theMatrix.myMat[2][2] * aOrigZ;
}

//=================================================================================================

inline void gp_XYZ::Normalize()
{
  double aD = Modulus();
  Standard_ConstructionError_Raise_if(aD <= gp::Resolution(),
                                      "gp_XYZ::Normalize() - vector has zero norm");
  x = x / aD;
  y = y / aD;
  z = z / aD;
}

//=================================================================================================

inline constexpr gp_XYZ operator*(const gp_Mat& theMatrix, const gp_XYZ& theCoord1) noexcept
{
  return theCoord1.Multiplied(theMatrix);
}

//=================================================================================================

inline constexpr gp_XYZ operator*(const double theScalar, const gp_XYZ& theCoord1) noexcept
{
  return theCoord1.Multiplied(theScalar);
}

#endif // _gp_XYZ_HeaderFile
