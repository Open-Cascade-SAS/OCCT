// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef NCollection_Vec3_HeaderFile
#define NCollection_Vec3_HeaderFile

#include <cstring>
#include <cmath>
#include <NCollection_Vec2.hxx>

//! Auxiliary macros to define couple of similar access components as vector methods
#define NCOLLECTION_VEC_COMPONENTS_3D(theX, theY, theZ)                                            \
  constexpr NCollection_Vec3<Element_t> theX##theY##theZ() const noexcept                          \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theX(), theY(), theZ());                                    \
  }                                                                                                \
  constexpr NCollection_Vec3<Element_t> theX##theZ##theY() const noexcept                          \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theX(), theZ(), theY());                                    \
  }                                                                                                \
  constexpr NCollection_Vec3<Element_t> theY##theX##theZ() const noexcept                          \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theY(), theX(), theZ());                                    \
  }                                                                                                \
  constexpr NCollection_Vec3<Element_t> theY##theZ##theX() const noexcept                          \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theY(), theZ(), theX());                                    \
  }                                                                                                \
  constexpr NCollection_Vec3<Element_t> theZ##theY##theX() const noexcept                          \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theZ(), theY(), theX());                                    \
  }                                                                                                \
  constexpr NCollection_Vec3<Element_t> theZ##theX##theY() const noexcept                          \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theZ(), theX(), theY());                                    \
  }

//! Generic 3-components vector.
//! To be used as RGB color pixel or XYZ 3D-point.
//! The main target for this class - to handle raw low-level arrays (from/to graphic driver etc.).
template <typename Element_t>
class NCollection_Vec3
{

public:
  //! Returns the number of components.
  static constexpr int Length() noexcept { return 3; }

  //! Empty constructor. Construct the zero vector.
  constexpr NCollection_Vec3() noexcept
      : v{Element_t(0), Element_t(0), Element_t(0)}
  {
  }

  //! Initialize ALL components of vector within specified value.
  explicit constexpr NCollection_Vec3(Element_t theValue) noexcept
      : v{theValue, theValue, theValue}
  {
  }

  //! Per-component constructor.
  explicit constexpr NCollection_Vec3(const Element_t theX,
                                      const Element_t theY,
                                      const Element_t theZ) noexcept
      : v{theX, theY, theZ}
  {
  }

  //! Constructor from 2-components vector + optional 3rd value.
  explicit constexpr NCollection_Vec3(const NCollection_Vec2<Element_t>& theVec2,
                                      Element_t theZ = Element_t(0)) noexcept
      : v{theVec2[0], theVec2[1], theZ}
  {
  }

  //! Conversion constructor (explicitly converts some 3-component vector with other element type
  //! to a new 3-component vector with the element type Element_t,
  //! whose elements are static_cast'ed corresponding elements of theOtherVec3 vector)
  //! @tparam OtherElement_t the element type of the other 3-component vector theOtherVec3
  //! @param theOtherVec3 the 3-component vector that needs to be converted
  template <typename OtherElement_t>
  explicit constexpr NCollection_Vec3(const NCollection_Vec3<OtherElement_t>& theOtherVec3) noexcept
      : v{static_cast<Element_t>(theOtherVec3[0]),
          static_cast<Element_t>(theOtherVec3[1]),
          static_cast<Element_t>(theOtherVec3[2])}
  {
  }

  //! Assign new values to the vector.
  constexpr void SetValues(const Element_t theX,
                           const Element_t theY,
                           const Element_t theZ) noexcept
  {
    v[0] = theX;
    v[1] = theY;
    v[2] = theZ;
  }

  //! Assign new values to the vector.
  constexpr void SetValues(const NCollection_Vec2<Element_t>& theVec2, Element_t theZ) noexcept
  {
    v[0] = theVec2.x();
    v[1] = theVec2.y();
    v[2] = theZ;
  }

  //! Alias to 1st component as X coordinate in XYZ.
  constexpr Element_t x() const noexcept { return v[0]; }

  //! Alias to 1st component as RED channel in RGB.
  constexpr Element_t r() const noexcept { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZ.
  constexpr Element_t y() const noexcept { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGB.
  constexpr Element_t g() const noexcept { return v[1]; }

  //! Alias to 3rd component as Z coordinate in XYZ.
  constexpr Element_t z() const noexcept { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGB.
  constexpr Element_t b() const noexcept { return v[2]; }

  //! @return 2 components by their names in specified order (in GLSL-style)
  NCOLLECTION_VEC_COMPONENTS_2D(x, y)
  NCOLLECTION_VEC_COMPONENTS_2D(x, z)
  NCOLLECTION_VEC_COMPONENTS_2D(y, z)

  //! @return 3 components by their names in specified order (in GLSL-style)
  NCOLLECTION_VEC_COMPONENTS_3D(x, y, z)

  //! Alias to 1st component as X coordinate in XYZ.
  constexpr Element_t& x() noexcept { return v[0]; }

  //! Alias to 1st component as RED channel in RGB.
  constexpr Element_t& r() noexcept { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZ.
  constexpr Element_t& y() noexcept { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGB.
  constexpr Element_t& g() noexcept { return v[1]; }

  //! Alias to 3rd component as Z coordinate in XYZ.
  constexpr Element_t& z() noexcept { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGB.
  constexpr Element_t& b() noexcept { return v[2]; }

  //! Check this vector with another vector for equality (without tolerance!).
  constexpr bool IsEqual(const NCollection_Vec3& theOther) const noexcept
  {
    return v[0] == theOther.v[0] && v[1] == theOther.v[1] && v[2] == theOther.v[2];
  }

  //! Check this vector with another vector for equality (without tolerance!).
  constexpr bool operator==(const NCollection_Vec3& theOther) const noexcept
  {
    return IsEqual(theOther);
  }

  //! Check this vector with another vector for non-equality (without tolerance!).
  constexpr bool operator!=(const NCollection_Vec3& theOther) const noexcept
  {
    return !IsEqual(theOther);
  }

  //! Raw access to the data (for OpenGL exchange).
  constexpr const Element_t* GetData() const noexcept { return v; }

  constexpr Element_t* ChangeData() noexcept { return v; }

  constexpr operator const Element_t*() const noexcept { return v; }

  constexpr operator Element_t*() noexcept { return v; }

  //! Compute per-component summary.
  constexpr NCollection_Vec3& operator+=(const NCollection_Vec3& theAdd) noexcept
  {
    v[0] += theAdd.v[0];
    v[1] += theAdd.v[1];
    v[2] += theAdd.v[2];
    return *this;
  }

  //! Compute per-component summary.
  friend constexpr NCollection_Vec3 operator+(const NCollection_Vec3& theLeft,
                                              const NCollection_Vec3& theRight) noexcept
  {
    NCollection_Vec3 aSumm = NCollection_Vec3(theLeft);
    return aSumm += theRight;
  }

  //! Unary -.
  constexpr NCollection_Vec3 operator-() const noexcept
  {
    return NCollection_Vec3(-x(), -y(), -z());
  }

  //! Compute per-component subtraction.
  constexpr NCollection_Vec3& operator-=(const NCollection_Vec3& theDec) noexcept
  {
    v[0] -= theDec.v[0];
    v[1] -= theDec.v[1];
    v[2] -= theDec.v[2];
    return *this;
  }

  //! Compute per-component subtraction.
  friend constexpr NCollection_Vec3 operator-(const NCollection_Vec3& theLeft,
                                              const NCollection_Vec3& theRight) noexcept
  {
    NCollection_Vec3 aSumm = NCollection_Vec3(theLeft);
    return aSumm -= theRight;
  }

  //! Compute per-component multiplication by scale factor.
  constexpr void Multiply(const Element_t theFactor) noexcept
  {
    v[0] *= theFactor;
    v[1] *= theFactor;
    v[2] *= theFactor;
  }

  //! Compute per-component multiplication.
  constexpr NCollection_Vec3& operator*=(const NCollection_Vec3& theRight) noexcept
  {
    v[0] *= theRight.v[0];
    v[1] *= theRight.v[1];
    v[2] *= theRight.v[2];
    return *this;
  }

  //! Compute per-component multiplication.
  friend constexpr NCollection_Vec3 operator*(const NCollection_Vec3& theLeft,
                                              const NCollection_Vec3& theRight) noexcept
  {
    NCollection_Vec3 aResult = NCollection_Vec3(theLeft);
    return aResult *= theRight;
  }

  //! Compute per-component multiplication by scale factor.
  constexpr NCollection_Vec3& operator*=(const Element_t theFactor) noexcept
  {
    Multiply(theFactor);
    return *this;
  }

  //! Compute per-component multiplication by scale factor.
  constexpr NCollection_Vec3 operator*(const Element_t theFactor) const noexcept
  {
    return Multiplied(theFactor);
  }

  //! Compute per-component multiplication by scale factor.
  constexpr NCollection_Vec3 Multiplied(const Element_t theFactor) const noexcept
  {
    NCollection_Vec3 aCopyVec3(*this);
    aCopyVec3 *= theFactor;
    return aCopyVec3;
  }

  //! Compute component-wise minimum of two vectors.
  constexpr NCollection_Vec3 cwiseMin(const NCollection_Vec3& theVec) const noexcept
  {
    return NCollection_Vec3(v[0] < theVec.v[0] ? v[0] : theVec.v[0],
                            v[1] < theVec.v[1] ? v[1] : theVec.v[1],
                            v[2] < theVec.v[2] ? v[2] : theVec.v[2]);
  }

  //! Compute component-wise maximum of two vectors.
  constexpr NCollection_Vec3 cwiseMax(const NCollection_Vec3& theVec) const noexcept
  {
    return NCollection_Vec3(v[0] > theVec.v[0] ? v[0] : theVec.v[0],
                            v[1] > theVec.v[1] ? v[1] : theVec.v[1],
                            v[2] > theVec.v[2] ? v[2] : theVec.v[2]);
  }

  //! Compute component-wise modulus of the vector.
  NCollection_Vec3 cwiseAbs() const noexcept
  {
    return NCollection_Vec3(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]));
  }

  //! Compute maximum component of the vector.
  constexpr Element_t maxComp() const noexcept
  {
    return v[0] > v[1] ? (v[0] > v[2] ? v[0] : v[2]) : (v[1] > v[2] ? v[1] : v[2]);
  }

  //! Compute minimum component of the vector.
  constexpr Element_t minComp() const noexcept
  {
    return v[0] < v[1] ? (v[0] < v[2] ? v[0] : v[2]) : (v[1] < v[2] ? v[1] : v[2]);
  }

  //! Compute per-component division by scale factor.
  constexpr NCollection_Vec3& operator/=(const Element_t theInvFactor)
  {
    v[0] /= theInvFactor;
    v[1] /= theInvFactor;
    v[2] /= theInvFactor;
    return *this;
  }

  //! Compute per-component division.
  constexpr NCollection_Vec3& operator/=(const NCollection_Vec3& theRight)
  {
    v[0] /= theRight.v[0];
    v[1] /= theRight.v[1];
    v[2] /= theRight.v[2];
    return *this;
  }

  //! Compute per-component division by scale factor.
  constexpr NCollection_Vec3 operator/(const Element_t theInvFactor) const
  {
    NCollection_Vec3 aResult(*this);
    return aResult /= theInvFactor;
  }

  //! Compute per-component division.
  friend constexpr NCollection_Vec3 operator/(const NCollection_Vec3& theLeft,
                                              const NCollection_Vec3& theRight)
  {
    NCollection_Vec3 aResult = NCollection_Vec3(theLeft);
    return aResult /= theRight;
  }

  //! Computes the dot product.
  constexpr Element_t Dot(const NCollection_Vec3& theOther) const noexcept
  {
    return x() * theOther.x() + y() * theOther.y() + z() * theOther.z();
  }

  //! Computes the vector modulus (magnitude, length).
  Element_t Modulus() const noexcept { return std::sqrt(x() * x() + y() * y() + z() * z()); }

  //! Computes the square of vector modulus (magnitude, length).
  //! This method may be used for performance tricks.
  constexpr Element_t SquareModulus() const noexcept { return x() * x() + y() * y() + z() * z(); }

  //! Normalize the vector.
  void Normalize()
  {
    Element_t aModulus = Modulus();
    if (aModulus != Element_t(0)) // just avoid divide by zero
    {
      x() = x() / aModulus;
      y() = y() / aModulus;
      z() = z() / aModulus;
    }
  }

  //! Normalize the vector.
  NCollection_Vec3 Normalized() const
  {
    NCollection_Vec3 aCopy(*this);
    aCopy.Normalize();
    return aCopy;
  }

  //! Computes the cross product.
  static constexpr NCollection_Vec3 Cross(const NCollection_Vec3& theVec1,
                                          const NCollection_Vec3& theVec2) noexcept
  {
    return NCollection_Vec3(theVec1.y() * theVec2.z() - theVec1.z() * theVec2.y(),
                            theVec1.z() * theVec2.x() - theVec1.x() * theVec2.z(),
                            theVec1.x() * theVec2.y() - theVec1.y() * theVec2.x());
  }

  //! Compute linear interpolation between to vectors.
  //! @param theT - interpolation coefficient 0..1;
  //! @return interpolation result.
  static constexpr NCollection_Vec3 GetLERP(const NCollection_Vec3& theFrom,
                                            const NCollection_Vec3& theTo,
                                            const Element_t         theT) noexcept
  {
    return theFrom * (Element_t(1) - theT) + theTo * theT;
  }

  //! Construct DX unit vector.
  static constexpr NCollection_Vec3 DX() noexcept
  {
    return NCollection_Vec3(Element_t(1), Element_t(0), Element_t(0));
  }

  //! Construct DY unit vector.
  static constexpr NCollection_Vec3 DY() noexcept
  {
    return NCollection_Vec3(Element_t(0), Element_t(1), Element_t(0));
  }

  //! Construct DZ unit vector.
  static constexpr NCollection_Vec3 DZ() noexcept
  {
    return NCollection_Vec3(Element_t(0), Element_t(0), Element_t(1));
  }

  //! Dumps the content of me into the stream
  void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
  {
    (void)theDepth;
    OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "Vec3", 3, v[0], v[1], v[2])
  }

private:
  Element_t v[3]; //!< define the vector as array to avoid structure alignment issues
};

//! Optimized concretization for float type.
template <>
inline constexpr NCollection_Vec3<float>& NCollection_Vec3<float>::operator/=(
  const float theInvFactor)
{
  Multiply(1.0f / theInvFactor);
  return *this;
}

//! Optimized concretization for double type.
template <>
inline constexpr NCollection_Vec3<double>& NCollection_Vec3<double>::operator/=(
  const double theInvFactor)
{
  Multiply(1.0 / theInvFactor);
  return *this;
}

#endif // _NCollection_Vec3_H__
