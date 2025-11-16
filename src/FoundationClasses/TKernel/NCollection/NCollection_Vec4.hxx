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

#ifndef NCollection_Vec4_HeaderFile
#define NCollection_Vec4_HeaderFile

#include <NCollection_Vec3.hxx>

//! Generic 4-components vector.
//! To be used as RGBA color vector or XYZW 3D-point with special W-component
//! for operations with projection / model view matrices.
//! Use this class for 3D-points carefully because declared W-component may
//! results in incorrect results if used without matrices.
template <typename Element_t>
class NCollection_Vec4
{

public:
  //! Returns the number of components.
  static constexpr int Length() noexcept { return 4; }

  //! Empty constructor. Construct the zero vector.
  constexpr NCollection_Vec4() noexcept
      : v{Element_t(0), Element_t(0), Element_t(0), Element_t(0)}
  {
  }

  //! Initialize ALL components of vector within specified value.
  explicit constexpr NCollection_Vec4(const Element_t theValue) noexcept
      : v{theValue, theValue, theValue, theValue}
  {
  }

  //! Per-component constructor.
  explicit constexpr NCollection_Vec4(const Element_t theX,
                                      const Element_t theY,
                                      const Element_t theZ,
                                      const Element_t theW) noexcept
      : v{theX, theY, theZ, theW}
  {
  }

  //! Constructor from 2-components vector.
  explicit constexpr NCollection_Vec4(const NCollection_Vec2<Element_t>& theVec2) noexcept
      : v{theVec2[0], theVec2[1], Element_t(0), Element_t(0)}
  {
  }

  //! Constructor from 3-components vector + optional 4th value.
  explicit NCollection_Vec4(const NCollection_Vec3<Element_t>& theVec3,
                            const Element_t                    theW = Element_t(0))
  {
    std::memcpy(this, &theVec3, sizeof(NCollection_Vec3<Element_t>));
    v[3] = theW;
  }

  //! Conversion constructor (explicitly converts some 4-component vector with other element type
  //! to a new 4-component vector with the element type Element_t,
  //! whose elements are static_cast'ed corresponding elements of theOtherVec4 vector)
  //! @tparam OtherElement_t the element type of the other 4-component vector theOtherVec4
  //! @param theOtherVec4 the 4-component vector that needs to be converted
  template <typename OtherElement_t>
  explicit constexpr NCollection_Vec4(const NCollection_Vec4<OtherElement_t>& theOtherVec4) noexcept
      : v{static_cast<Element_t>(theOtherVec4[0]),
          static_cast<Element_t>(theOtherVec4[1]),
          static_cast<Element_t>(theOtherVec4[2]),
          static_cast<Element_t>(theOtherVec4[3])}
  {
  }

  //! Assign new values to the vector.
  constexpr void SetValues(const Element_t theX,
                           const Element_t theY,
                           const Element_t theZ,
                           const Element_t theW) noexcept
  {
    v[0] = theX;
    v[1] = theY;
    v[2] = theZ;
    v[3] = theW;
  }

  //! Assign new values as 3-component vector and a 4-th value.
  constexpr void SetValues(const NCollection_Vec3<Element_t>& theVec3,
                           const Element_t                    theW) noexcept
  {
    v[0] = theVec3.x();
    v[1] = theVec3.y();
    v[2] = theVec3.z();
    v[3] = theW;
  }

  //! Alias to 1st component as X coordinate in XYZW.
  constexpr Element_t x() const noexcept { return v[0]; }

  //! Alias to 1st component as RED channel in RGBA.
  constexpr Element_t r() const noexcept { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZW.
  constexpr Element_t y() const noexcept { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGBA.
  constexpr Element_t g() const noexcept { return v[1]; }

  //! Alias to 3rd component as Z coordinate in XYZW.
  constexpr Element_t z() const noexcept { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGBA.
  constexpr Element_t b() const noexcept { return v[2]; }

  //! Alias to 4th component as W coordinate in XYZW.
  constexpr Element_t w() const noexcept { return v[3]; }

  //! Alias to 4th component as ALPHA channel in RGBA.
  constexpr Element_t a() const noexcept { return v[3]; }

  //! @return 2 of XYZW components in specified order as vector in GLSL-style
  NCOLLECTION_VEC_COMPONENTS_2D(x, y)
  NCOLLECTION_VEC_COMPONENTS_2D(x, z)
  NCOLLECTION_VEC_COMPONENTS_2D(x, w)
  NCOLLECTION_VEC_COMPONENTS_2D(y, z)
  NCOLLECTION_VEC_COMPONENTS_2D(y, w)
  NCOLLECTION_VEC_COMPONENTS_2D(z, w)

  //! @return 3 of XYZW components in specified order as vector in GLSL-style
  NCOLLECTION_VEC_COMPONENTS_3D(x, y, z)
  NCOLLECTION_VEC_COMPONENTS_3D(x, y, w)
  NCOLLECTION_VEC_COMPONENTS_3D(x, z, w)
  NCOLLECTION_VEC_COMPONENTS_3D(y, z, w)

  //! @return RGB components as vector
  NCOLLECTION_VEC_COMPONENTS_3D(r, g, b)

  //! Alias to 1st component as X coordinate in XYZW.
  constexpr Element_t& x() noexcept { return v[0]; }

  //! Alias to 1st component as RED channel in RGBA.
  constexpr Element_t& r() noexcept { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZW.
  constexpr Element_t& y() noexcept { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGBA.
  constexpr Element_t& g() noexcept { return v[1]; } // Green color

  //! Alias to 3rd component as Z coordinate in XYZW.
  constexpr Element_t& z() noexcept { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGBA.
  constexpr Element_t& b() noexcept { return v[2]; }

  //! Alias to 4th component as W coordinate in XYZW.
  constexpr Element_t& w() noexcept { return v[3]; }

  //! Alias to 4th component as ALPHA channel in RGBA.
  constexpr Element_t& a() noexcept { return v[3]; }

  //! Check this vector with another vector for equality (without tolerance!).
  constexpr bool IsEqual(const NCollection_Vec4& theOther) const noexcept
  {
    return v[0] == theOther.v[0] && v[1] == theOther.v[1] && v[2] == theOther.v[2]
           && v[3] == theOther.v[3];
  }

  //! Check this vector with another vector for equality (without tolerance!).
  constexpr bool operator==(const NCollection_Vec4& theOther) const noexcept
  {
    return IsEqual(theOther);
  }

  //! Check this vector with another vector for non-equality (without tolerance!).
  constexpr bool operator!=(const NCollection_Vec4& theOther) const noexcept
  {
    return !IsEqual(theOther);
  }

  //! Raw access to the data (for OpenGL exchange).
  constexpr const Element_t* GetData() const noexcept { return v; }

  constexpr Element_t* ChangeData() noexcept { return v; }

  constexpr operator const Element_t*() const noexcept { return v; }

  constexpr operator Element_t*() noexcept { return v; }

  //! Compute per-component summary.
  constexpr NCollection_Vec4& operator+=(const NCollection_Vec4& theAdd) noexcept
  {
    v[0] += theAdd.v[0];
    v[1] += theAdd.v[1];
    v[2] += theAdd.v[2];
    v[3] += theAdd.v[3];
    return *this;
  }

  //! Compute per-component summary.
  friend constexpr NCollection_Vec4 operator+(const NCollection_Vec4& theLeft,
                                              const NCollection_Vec4& theRight) noexcept
  {
    NCollection_Vec4 aSumm = NCollection_Vec4(theLeft);
    return aSumm += theRight;
  }

  //! Unary -.
  constexpr NCollection_Vec4 operator-() const noexcept
  {
    return NCollection_Vec4(-x(), -y(), -z(), -w());
  }

  //! Compute per-component subtraction.
  constexpr NCollection_Vec4& operator-=(const NCollection_Vec4& theDec) noexcept
  {
    v[0] -= theDec.v[0];
    v[1] -= theDec.v[1];
    v[2] -= theDec.v[2];
    v[3] -= theDec.v[3];
    return *this;
  }

  //! Compute per-component subtraction.
  friend constexpr NCollection_Vec4 operator-(const NCollection_Vec4& theLeft,
                                              const NCollection_Vec4& theRight) noexcept
  {
    NCollection_Vec4 aSumm = NCollection_Vec4(theLeft);
    return aSumm -= theRight;
  }

  //! Compute per-component multiplication.
  constexpr NCollection_Vec4& operator*=(const NCollection_Vec4& theRight) noexcept
  {
    v[0] *= theRight.v[0];
    v[1] *= theRight.v[1];
    v[2] *= theRight.v[2];
    v[3] *= theRight.v[3];
    return *this;
  }

  //! Compute per-component multiplication.
  friend constexpr NCollection_Vec4 operator*(const NCollection_Vec4& theLeft,
                                              const NCollection_Vec4& theRight) noexcept
  {
    NCollection_Vec4 aResult = NCollection_Vec4(theLeft);
    return aResult *= theRight;
  }

  //! Compute per-component multiplication.
  constexpr void Multiply(const Element_t theFactor) noexcept
  {
    v[0] *= theFactor;
    v[1] *= theFactor;
    v[2] *= theFactor;
    v[3] *= theFactor;
  }

  //! Compute per-component multiplication.
  constexpr NCollection_Vec4& operator*=(const Element_t theFactor) noexcept
  {
    Multiply(theFactor);
    return *this;
  }

  //! Compute per-component multiplication.
  constexpr NCollection_Vec4 operator*(const Element_t theFactor) const noexcept
  {
    return Multiplied(theFactor);
  }

  //! Compute per-component multiplication.
  constexpr NCollection_Vec4 Multiplied(const Element_t theFactor) const noexcept
  {
    NCollection_Vec4 aCopyVec4(*this);
    aCopyVec4 *= theFactor;
    return aCopyVec4;
  }

  //! Compute component-wise minimum of two vectors.
  constexpr NCollection_Vec4 cwiseMin(const NCollection_Vec4& theVec) const noexcept
  {
    return NCollection_Vec4(v[0] < theVec.v[0] ? v[0] : theVec.v[0],
                            v[1] < theVec.v[1] ? v[1] : theVec.v[1],
                            v[2] < theVec.v[2] ? v[2] : theVec.v[2],
                            v[3] < theVec.v[3] ? v[3] : theVec.v[3]);
  }

  //! Compute component-wise maximum of two vectors.
  constexpr NCollection_Vec4 cwiseMax(const NCollection_Vec4& theVec) const noexcept
  {
    return NCollection_Vec4(v[0] > theVec.v[0] ? v[0] : theVec.v[0],
                            v[1] > theVec.v[1] ? v[1] : theVec.v[1],
                            v[2] > theVec.v[2] ? v[2] : theVec.v[2],
                            v[3] > theVec.v[3] ? v[3] : theVec.v[3]);
  }

  //! Compute component-wise modulus of the vector.
  NCollection_Vec4 cwiseAbs() const noexcept
  {
    return NCollection_Vec4(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]), std::abs(v[3]));
  }

  //! Compute maximum component of the vector.
  constexpr Element_t maxComp() const noexcept
  {
    const Element_t aMax1 = v[0] > v[1] ? v[0] : v[1];
    const Element_t aMax2 = v[2] > v[3] ? v[2] : v[3];

    return aMax1 > aMax2 ? aMax1 : aMax2;
  }

  //! Compute minimum component of the vector.
  constexpr Element_t minComp() const noexcept
  {
    const Element_t aMin1 = v[0] < v[1] ? v[0] : v[1];
    const Element_t aMin2 = v[2] < v[3] ? v[2] : v[3];

    return aMin1 < aMin2 ? aMin1 : aMin2;
  }

  //! Computes the dot product.
  constexpr Element_t Dot(const NCollection_Vec4& theOther) const noexcept
  {
    return x() * theOther.x() + y() * theOther.y() + z() * theOther.z() + w() * theOther.w();
  }

  //! Compute per-component division by scale factor.
  constexpr NCollection_Vec4& operator/=(const Element_t theInvFactor)
  {
    v[0] /= theInvFactor;
    v[1] /= theInvFactor;
    v[2] /= theInvFactor;
    v[3] /= theInvFactor;
    return *this;
  }

  //! Compute per-component division.
  constexpr NCollection_Vec4& operator/=(const NCollection_Vec4& theRight)
  {
    v[0] /= theRight.v[0];
    v[1] /= theRight.v[1];
    v[2] /= theRight.v[2];
    v[3] /= theRight.v[3];
    return *this;
  }

  //! Compute per-component division by scale factor.
  constexpr NCollection_Vec4 operator/(const Element_t theInvFactor) const
  {
    NCollection_Vec4 aResult(*this);
    return aResult /= theInvFactor;
  }

  //! Compute per-component division.
  friend constexpr NCollection_Vec4 operator/(const NCollection_Vec4& theLeft,
                                              const NCollection_Vec4& theRight)
  {
    NCollection_Vec4 aResult = NCollection_Vec4(theLeft);
    return aResult /= theRight;
  }

  //! Dumps the content of me into the stream
  void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
  {
    (void)theDepth;
    OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "Vec4", 4, v[0], v[1], v[2], v[3])
  }

private:
  Element_t v[4]; //!< define the vector as array to avoid structure alignment issues
};

//! Optimized concretization for float type.
template <>
inline constexpr NCollection_Vec4<float>& NCollection_Vec4<float>::operator/=(
  const float theInvFactor)
{
  Multiply(1.0f / theInvFactor);
  return *this;
}

//! Optimized concretization for double type.
template <>
inline constexpr NCollection_Vec4<double>& NCollection_Vec4<double>::operator/=(
  const double theInvFactor)
{
  Multiply(1.0 / theInvFactor);
  return *this;
}

#endif // _NCollection_Vec4_H__
