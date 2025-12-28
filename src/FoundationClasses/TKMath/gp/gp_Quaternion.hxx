// Created on: 2010-05-11
// Created by: Kirill GAVRILOV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#ifndef _gp_Quaternion_HeaderFile
#define _gp_Quaternion_HeaderFile

#include <gp_EulerSequence.hxx>
#include <gp_Mat.hxx>
#include <gp_Vec.hxx>

//! Represents operation of rotation in 3d space as quaternion
//! and implements operations with rotations basing on
//! quaternion mathematics.
//!
//! In addition, provides methods for conversion to and from other
//! representations of rotation (3*3 matrix, vector and
//! angle, Euler angles)
class gp_Quaternion
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an identity quaternion
  constexpr gp_Quaternion() noexcept
      : x(0.0),
        y(0.0),
        z(0.0),
        w(1.0)
  {
  }

  //! Creates quaternion directly from component values
  constexpr gp_Quaternion(const double theX,
                          const double theY,
                          const double theZ,
                          const double theW) noexcept
      : x(theX),
        y(theY),
        z(theZ),
        w(theW)
  {
  }

  //! Creates quaternion representing shortest-arc rotation
  //! operator producing vector theVecTo from vector theVecFrom.
  gp_Quaternion(const gp_Vec& theVecFrom, const gp_Vec& theVecTo)
  {
    SetRotation(theVecFrom, theVecTo);
  }

  //! Creates quaternion representing shortest-arc rotation
  //! operator producing vector theVecTo from vector theVecFrom.
  //! Additional vector theHelpCrossVec defines preferred direction for
  //! rotation and is used when theVecTo and theVecFrom are directed
  //! oppositely.
  gp_Quaternion(const gp_Vec& theVecFrom, const gp_Vec& theVecTo, const gp_Vec& theHelpCrossVec)
  {
    SetRotation(theVecFrom, theVecTo, theHelpCrossVec);
  }

  //! Creates quaternion representing rotation on angle
  //! theAngle around vector theAxis
  gp_Quaternion(const gp_Vec& theAxis, const double theAngle)
  {
    SetVectorAndAngle(theAxis, theAngle);
  }

  //! Creates quaternion from rotation matrix 3*3
  //! (which should be orthonormal skew-symmetric matrix)
  gp_Quaternion(const gp_Mat& theMat) { SetMatrix(theMat); }

  //! Simple equal test without precision
  Standard_EXPORT bool IsEqual(const gp_Quaternion& theOther) const;

  //! Sets quaternion to shortest-arc rotation producing
  //! vector theVecTo from vector theVecFrom.
  //! If vectors theVecFrom and theVecTo are opposite then rotation
  //! axis is computed as theVecFrom ^ (1,0,0) or theVecFrom ^ (0,0,1).
  Standard_EXPORT void SetRotation(const gp_Vec& theVecFrom, const gp_Vec& theVecTo);

  //! Sets quaternion to shortest-arc rotation producing
  //! vector theVecTo from vector theVecFrom.
  //! If vectors theVecFrom and theVecTo are opposite then rotation
  //! axis is computed as theVecFrom ^ theHelpCrossVec.
  Standard_EXPORT void SetRotation(const gp_Vec& theVecFrom,
                                   const gp_Vec& theVecTo,
                                   const gp_Vec& theHelpCrossVec);

  //! Create a unit quaternion from Axis+Angle representation
  Standard_EXPORT void SetVectorAndAngle(const gp_Vec& theAxis, const double theAngle);

  //! Convert a quaternion to Axis+Angle representation,
  //! preserve the axis direction and angle from -PI to +PI
  Standard_EXPORT void GetVectorAndAngle(gp_Vec& theAxis, double& theAngle) const;

  //! Create a unit quaternion by rotation matrix
  //! matrix must contain only rotation (not scale or shear)
  //!
  //! For numerical stability we find first the greatest component of quaternion
  //! and than search others from this one
  Standard_EXPORT void SetMatrix(const gp_Mat& theMat);

  //! Returns rotation operation as 3*3 matrix
  Standard_EXPORT gp_Mat GetMatrix() const;

  //! Create a unit quaternion representing rotation defined
  //! by generalized Euler angles
  Standard_EXPORT void SetEulerAngles(const gp_EulerSequence theOrder,
                                      const double           theAlpha,
                                      const double           theBeta,
                                      const double           theGamma);

  //! Returns Euler angles describing current rotation
  Standard_EXPORT void GetEulerAngles(const gp_EulerSequence theOrder,
                                      double&                theAlpha,
                                      double&                theBeta,
                                      double&                theGamma) const;

  constexpr void Set(const double theX,
                     const double theY,
                     const double theZ,
                     const double theW) noexcept;

  constexpr void Set(const gp_Quaternion& theQuaternion) noexcept;

  constexpr double X() const noexcept { return x; }

  constexpr double Y() const noexcept { return y; }

  constexpr double Z() const noexcept { return z; }

  constexpr double W() const noexcept { return w; }

  //! Make identity quaternion (zero-rotation)
  constexpr void SetIdent() noexcept
  {
    x = y = z = 0.0;
    w         = 1.0;
  }

  //! Reverse direction of rotation (conjugate quaternion)
  constexpr void Reverse() noexcept
  {
    x = -x;
    y = -y;
    z = -z;
  }

  //! Return rotation with reversed direction (conjugated quaternion)
  [[nodiscard]] constexpr gp_Quaternion Reversed() const noexcept
  {
    return gp_Quaternion(-x, -y, -z, w);
  }

  //! Inverts quaternion (both rotation direction and norm)
  constexpr void Invert()
  {
    double anIn = 1.0 / SquareNorm();
    Set(-x * anIn, -y * anIn, -z * anIn, w * anIn);
  }

  //! Return inversed quaternion q^-1
  [[nodiscard]] constexpr gp_Quaternion Inverted() const
  {
    double anIn = 1.0 / SquareNorm();
    return gp_Quaternion(-x * anIn, -y * anIn, -z * anIn, w * anIn);
  }

  //! Returns square norm of quaternion
  constexpr double SquareNorm() const noexcept { return x * x + y * y + z * z + w * w; }

  //! Returns norm of quaternion
  double Norm() const { return std::sqrt(SquareNorm()); }

  //! Scale all components by quaternion by theScale; note that
  //! rotation is not changed by this operation (except 0-scaling)
  constexpr void Scale(const double theScale) noexcept;

  void operator*=(const double theScale) { Scale(theScale); }

  //! Returns scaled quaternion
  [[nodiscard]] constexpr gp_Quaternion Scaled(const double theScale) const noexcept
  {
    return gp_Quaternion(x * theScale, y * theScale, z * theScale, w * theScale);
  }

  [[nodiscard]] constexpr gp_Quaternion operator*(const double theScale) const noexcept
  {
    return Scaled(theScale);
  }

  //! Stabilize quaternion length within 1 - 1/4.
  //! This operation is a lot faster than normalization
  //! and preserve length goes to 0 or infinity
  Standard_EXPORT void StabilizeLength();

  //! Scale quaternion that its norm goes to 1.
  //! The appearing of 0 magnitude or near is a error,
  //! so we can be sure that can divide by magnitude
  Standard_EXPORT void Normalize();

  //! Returns quaternion scaled so that its norm goes to 1.
  [[nodiscard]] gp_Quaternion Normalized() const
  {
    gp_Quaternion aNormilizedQ(*this);
    aNormilizedQ.Normalize();
    return aNormilizedQ;
  }

  //! Returns quaternion with all components negated.
  //! Note that this operation does not affect neither
  //! rotation operator defined by quaternion nor its norm.
  [[nodiscard]] constexpr gp_Quaternion Negated() const noexcept
  {
    return gp_Quaternion(-x, -y, -z, -w);
  }

  [[nodiscard]] constexpr gp_Quaternion operator-() const noexcept { return Negated(); }

  //! Makes sum of quaternion components; result is "rotations mix"
  [[nodiscard]] constexpr gp_Quaternion Added(const gp_Quaternion& theOther) const noexcept
  {
    return gp_Quaternion(x + theOther.x, y + theOther.y, z + theOther.z, w + theOther.w);
  }

  [[nodiscard]] constexpr gp_Quaternion operator+(const gp_Quaternion& theOther) const noexcept
  {
    return Added(theOther);
  }

  //! Makes difference of quaternion components; result is "rotations mix"
  [[nodiscard]] constexpr gp_Quaternion Subtracted(const gp_Quaternion& theOther) const noexcept
  {
    return gp_Quaternion(x - theOther.x, y - theOther.y, z - theOther.z, w - theOther.w);
  }

  [[nodiscard]] constexpr gp_Quaternion operator-(const gp_Quaternion& theOther) const noexcept
  {
    return Subtracted(theOther);
  }

  //! Multiply function - work the same as Matrices multiplying.
  //! @code
  //! qq' = (cross(v,v') + wv' + w'v, ww' - dot(v,v'))
  //! @endcode
  //! Result is rotation combination: q' than q (here q=this, q'=theQ).
  //! Notices that:
  //! @code
  //! qq' != q'q;
  //! qq^-1 = q;
  //! @endcode
  [[nodiscard]] constexpr gp_Quaternion Multiplied(const gp_Quaternion& theOther) const noexcept;

  [[nodiscard]] constexpr gp_Quaternion operator*(const gp_Quaternion& theOther) const noexcept
  {
    return Multiplied(theOther);
  }

  //! Adds components of other quaternion; result is "rotations mix"
  constexpr void Add(const gp_Quaternion& theOther) noexcept;

  void operator+=(const gp_Quaternion& theOther) { Add(theOther); }

  //! Subtracts components of other quaternion; result is "rotations mix"
  constexpr void Subtract(const gp_Quaternion& theOther) noexcept;

  void operator-=(const gp_Quaternion& theOther) { Subtract(theOther); }

  //! Adds rotation by multiplication
  void Multiply(const gp_Quaternion& theOther)
  {
    (*this) = Multiplied(theOther); // have no optimization here
  }

  void operator*=(const gp_Quaternion& theOther) { Multiply(theOther); }

  //! Computes inner product / scalar product / Dot
  constexpr double Dot(const gp_Quaternion& theOther) const noexcept
  {
    return x * theOther.x + y * theOther.y + z * theOther.z + w * theOther.w;
  }

  //! Return rotation angle from -PI to PI
  Standard_EXPORT double GetRotationAngle() const;

  //! Rotates vector by quaternion as rotation operator
  Standard_EXPORT gp_Vec Multiply(const gp_Vec& theVec) const;

  gp_Vec operator*(const gp_Vec& theVec) const { return Multiply(theVec); }

private:
  double x;
  double y;
  double z;
  double w;
};

//=================================================================================================

inline constexpr void gp_Quaternion::Set(double theX,
                                         double theY,
                                         double theZ,
                                         double theW) noexcept
{
  this->x = theX;
  this->y = theY;
  this->z = theZ;
  this->w = theW;
}

//=================================================================================================

inline constexpr void gp_Quaternion::Set(const gp_Quaternion& theQuaternion) noexcept
{
  x = theQuaternion.x;
  y = theQuaternion.y;
  z = theQuaternion.z;
  w = theQuaternion.w;
}

//=================================================================================================

inline constexpr void gp_Quaternion::Scale(const double theScale) noexcept
{
  x *= theScale;
  y *= theScale;
  z *= theScale;
  w *= theScale;
}

//=================================================================================================

inline constexpr gp_Quaternion gp_Quaternion::Multiplied(const gp_Quaternion& theQ) const noexcept
{
  return gp_Quaternion(w * theQ.x + x * theQ.w + y * theQ.z - z * theQ.y,
                       w * theQ.y + y * theQ.w + z * theQ.x - x * theQ.z,
                       w * theQ.z + z * theQ.w + x * theQ.y - y * theQ.x,
                       w * theQ.w - x * theQ.x - y * theQ.y - z * theQ.z);
  // 16 multiplications    12 addidtions    0 variables
}

//=================================================================================================

inline constexpr void gp_Quaternion::Add(const gp_Quaternion& theQ) noexcept
{
  x += theQ.x;
  y += theQ.y;
  z += theQ.z;
  w += theQ.w;
}

//=================================================================================================

inline constexpr void gp_Quaternion::Subtract(const gp_Quaternion& theQ) noexcept
{
  x -= theQ.x;
  y -= theQ.y;
  z -= theQ.z;
  w -= theQ.w;
}

#endif // _gp_Quaternion_HeaderFile
