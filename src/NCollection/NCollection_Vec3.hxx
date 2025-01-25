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
  const NCollection_Vec3<Element_t> theX##theY##theZ() const                                       \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theX(), theY(), theZ());                                    \
  }                                                                                                \
  const NCollection_Vec3<Element_t> theX##theZ##theY() const                                       \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theX(), theZ(), theY());                                    \
  }                                                                                                \
  const NCollection_Vec3<Element_t> theY##theX##theZ() const                                       \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theY(), theX(), theZ());                                    \
  }                                                                                                \
  const NCollection_Vec3<Element_t> theY##theZ##theX() const                                       \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theY(), theZ(), theX());                                    \
  }                                                                                                \
  const NCollection_Vec3<Element_t> theZ##theY##theX() const                                       \
  {                                                                                                \
    return NCollection_Vec3<Element_t>(theZ(), theY(), theX());                                    \
  }                                                                                                \
  const NCollection_Vec3<Element_t> theZ##theX##theY() const                                       \
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
  static int Length() { return 3; }

  //! Empty constructor. Construct the zero vector.
  NCollection_Vec3() { std::memset(this, 0, sizeof(NCollection_Vec3)); }

  //! Initialize ALL components of vector within specified value.
  explicit NCollection_Vec3(Element_t theValue) { v[0] = v[1] = v[2] = theValue; }

  //! Per-component constructor.
  explicit NCollection_Vec3(const Element_t theX, const Element_t theY, const Element_t theZ)
  {
    v[0] = theX;
    v[1] = theY;
    v[2] = theZ;
  }

  //! Constructor from 2-components vector + optional 3rd value.
  explicit NCollection_Vec3(const NCollection_Vec2<Element_t>& theVec2,
                            Element_t                          theZ = Element_t(0))
  {
    v[0] = theVec2[0];
    v[1] = theVec2[1];
    v[2] = theZ;
  }

  //! Conversion constructor (explicitly converts some 3-component vector with other element type
  //! to a new 3-component vector with the element type Element_t,
  //! whose elements are static_cast'ed corresponding elements of theOtherVec3 vector)
  //! @tparam OtherElement_t the element type of the other 3-component vector theOtherVec3
  //! @param theOtherVec3 the 3-component vector that needs to be converted
  template <typename OtherElement_t>
  explicit NCollection_Vec3(const NCollection_Vec3<OtherElement_t>& theOtherVec3)
  {
    v[0] = static_cast<Element_t>(theOtherVec3[0]);
    v[1] = static_cast<Element_t>(theOtherVec3[1]);
    v[2] = static_cast<Element_t>(theOtherVec3[2]);
  }

  //! Assign new values to the vector.
  void SetValues(const Element_t theX, const Element_t theY, const Element_t theZ)
  {
    v[0] = theX;
    v[1] = theY;
    v[2] = theZ;
  }

  //! Assign new values to the vector.
  void SetValues(const NCollection_Vec2<Element_t>& theVec2, Element_t theZ)
  {
    v[0] = theVec2.x();
    v[1] = theVec2.y();
    v[2] = theZ;
  }

  //! Alias to 1st component as X coordinate in XYZ.
  Element_t x() const { return v[0]; }

  //! Alias to 1st component as RED channel in RGB.
  Element_t r() const { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZ.
  Element_t y() const { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGB.
  Element_t g() const { return v[1]; }

  //! Alias to 3rd component as Z coordinate in XYZ.
  Element_t z() const { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGB.
  Element_t b() const { return v[2]; }

  //! @return 2 components by their names in specified order (in GLSL-style)
  NCOLLECTION_VEC_COMPONENTS_2D(x, y)
  NCOLLECTION_VEC_COMPONENTS_2D(x, z)
  NCOLLECTION_VEC_COMPONENTS_2D(y, z)

  //! @return 3 components by their names in specified order (in GLSL-style)
  NCOLLECTION_VEC_COMPONENTS_3D(x, y, z)

  //! Alias to 1st component as X coordinate in XYZ.
  Element_t& x() { return v[0]; }

  //! Alias to 1st component as RED channel in RGB.
  Element_t& r() { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZ.
  Element_t& y() { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGB.
  Element_t& g() { return v[1]; }

  //! Alias to 3rd component as Z coordinate in XYZ.
  Element_t& z() { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGB.
  Element_t& b() { return v[2]; }

  //! Check this vector with another vector for equality (without tolerance!).
  bool IsEqual(const NCollection_Vec3& theOther) const
  {
    return v[0] == theOther.v[0] && v[1] == theOther.v[1] && v[2] == theOther.v[2];
  }

  //! Check this vector with another vector for equality (without tolerance!).
  bool operator==(const NCollection_Vec3& theOther) const { return IsEqual(theOther); }

  //! Check this vector with another vector for non-equality (without tolerance!).
  bool operator!=(const NCollection_Vec3& theOther) const { return !IsEqual(theOther); }

  //! Raw access to the data (for OpenGL exchange).
  const Element_t* GetData() const { return v; }

  Element_t* ChangeData() { return v; }

  operator const Element_t*() const { return v; }

  operator Element_t*() { return v; }

  //! Compute per-component summary.
  NCollection_Vec3& operator+=(const NCollection_Vec3& theAdd)
  {
    v[0] += theAdd.v[0];
    v[1] += theAdd.v[1];
    v[2] += theAdd.v[2];
    return *this;
  }

  //! Compute per-component summary.
  friend NCollection_Vec3 operator+(const NCollection_Vec3& theLeft,
                                    const NCollection_Vec3& theRight)
  {
    NCollection_Vec3 aSumm = NCollection_Vec3(theLeft);
    return aSumm += theRight;
  }

  //! Unary -.
  NCollection_Vec3 operator-() const { return NCollection_Vec3(-x(), -y(), -z()); }

  //! Compute per-component subtraction.
  NCollection_Vec3& operator-=(const NCollection_Vec3& theDec)
  {
    v[0] -= theDec.v[0];
    v[1] -= theDec.v[1];
    v[2] -= theDec.v[2];
    return *this;
  }

  //! Compute per-component subtraction.
  friend NCollection_Vec3 operator-(const NCollection_Vec3& theLeft,
                                    const NCollection_Vec3& theRight)
  {
    NCollection_Vec3 aSumm = NCollection_Vec3(theLeft);
    return aSumm -= theRight;
  }

  //! Compute per-component multiplication by scale factor.
  void Multiply(const Element_t theFactor)
  {
    v[0] *= theFactor;
    v[1] *= theFactor;
    v[2] *= theFactor;
  }

  //! Compute per-component multiplication.
  NCollection_Vec3& operator*=(const NCollection_Vec3& theRight)
  {
    v[0] *= theRight.v[0];
    v[1] *= theRight.v[1];
    v[2] *= theRight.v[2];
    return *this;
  }

  //! Compute per-component multiplication.
  friend NCollection_Vec3 operator*(const NCollection_Vec3& theLeft,
                                    const NCollection_Vec3& theRight)
  {
    NCollection_Vec3 aResult = NCollection_Vec3(theLeft);
    return aResult *= theRight;
  }

  //! Compute per-component multiplication by scale factor.
  NCollection_Vec3& operator*=(const Element_t theFactor)
  {
    Multiply(theFactor);
    return *this;
  }

  //! Compute per-component multiplication by scale factor.
  NCollection_Vec3 operator*(const Element_t theFactor) const { return Multiplied(theFactor); }

  //! Compute per-component multiplication by scale factor.
  NCollection_Vec3 Multiplied(const Element_t theFactor) const
  {
    NCollection_Vec3 aCopyVec3(*this);
    aCopyVec3 *= theFactor;
    return aCopyVec3;
  }

  //! Compute component-wise minimum of two vectors.
  NCollection_Vec3 cwiseMin(const NCollection_Vec3& theVec) const
  {
    return NCollection_Vec3(v[0] < theVec.v[0] ? v[0] : theVec.v[0],
                            v[1] < theVec.v[1] ? v[1] : theVec.v[1],
                            v[2] < theVec.v[2] ? v[2] : theVec.v[2]);
  }

  //! Compute component-wise maximum of two vectors.
  NCollection_Vec3 cwiseMax(const NCollection_Vec3& theVec) const
  {
    return NCollection_Vec3(v[0] > theVec.v[0] ? v[0] : theVec.v[0],
                            v[1] > theVec.v[1] ? v[1] : theVec.v[1],
                            v[2] > theVec.v[2] ? v[2] : theVec.v[2]);
  }

  //! Compute component-wise modulus of the vector.
  NCollection_Vec3 cwiseAbs() const
  {
    return NCollection_Vec3(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]));
  }

  //! Compute maximum component of the vector.
  Element_t maxComp() const
  {
    return v[0] > v[1] ? (v[0] > v[2] ? v[0] : v[2]) : (v[1] > v[2] ? v[1] : v[2]);
  }

  //! Compute minimum component of the vector.
  Element_t minComp() const
  {
    return v[0] < v[1] ? (v[0] < v[2] ? v[0] : v[2]) : (v[1] < v[2] ? v[1] : v[2]);
  }

  //! Compute per-component division by scale factor.
  NCollection_Vec3& operator/=(const Element_t theInvFactor)
  {
    v[0] /= theInvFactor;
    v[1] /= theInvFactor;
    v[2] /= theInvFactor;
    return *this;
  }

  //! Compute per-component division.
  NCollection_Vec3& operator/=(const NCollection_Vec3& theRight)
  {
    v[0] /= theRight.v[0];
    v[1] /= theRight.v[1];
    v[2] /= theRight.v[2];
    return *this;
  }

  //! Compute per-component division by scale factor.
  NCollection_Vec3 operator/(const Element_t theInvFactor) const
  {
    NCollection_Vec3 aResult(*this);
    return aResult /= theInvFactor;
  }

  //! Compute per-component division.
  friend NCollection_Vec3 operator/(const NCollection_Vec3& theLeft,
                                    const NCollection_Vec3& theRight)
  {
    NCollection_Vec3 aResult = NCollection_Vec3(theLeft);
    return aResult /= theRight;
  }

  //! Computes the dot product.
  Element_t Dot(const NCollection_Vec3& theOther) const
  {
    return x() * theOther.x() + y() * theOther.y() + z() * theOther.z();
  }

  //! Computes the vector modulus (magnitude, length).
  Element_t Modulus() const { return std::sqrt(x() * x() + y() * y() + z() * z()); }

  //! Computes the square of vector modulus (magnitude, length).
  //! This method may be used for performance tricks.
  Element_t SquareModulus() const { return x() * x() + y() * y() + z() * z(); }

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
  static NCollection_Vec3 Cross(const NCollection_Vec3& theVec1, const NCollection_Vec3& theVec2)
  {
    return NCollection_Vec3(theVec1.y() * theVec2.z() - theVec1.z() * theVec2.y(),
                            theVec1.z() * theVec2.x() - theVec1.x() * theVec2.z(),
                            theVec1.x() * theVec2.y() - theVec1.y() * theVec2.x());
  }

  //! Compute linear interpolation between to vectors.
  //! @param theT - interpolation coefficient 0..1;
  //! @return interpolation result.
  static NCollection_Vec3 GetLERP(const NCollection_Vec3& theFrom,
                                  const NCollection_Vec3& theTo,
                                  const Element_t         theT)
  {
    return theFrom * (Element_t(1) - theT) + theTo * theT;
  }

  //! Construct DX unit vector.
  static NCollection_Vec3 DX()
  {
    return NCollection_Vec3(Element_t(1), Element_t(0), Element_t(0));
  }

  //! Construct DY unit vector.
  static NCollection_Vec3 DY()
  {
    return NCollection_Vec3(Element_t(0), Element_t(1), Element_t(0));
  }

  //! Construct DZ unit vector.
  static NCollection_Vec3 DZ()
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
inline NCollection_Vec3<float>& NCollection_Vec3<float>::operator/=(const float theInvFactor)
{
  Multiply(1.0f / theInvFactor);
  return *this;
}

//! Optimized concretization for double type.
template <>
inline NCollection_Vec3<double>& NCollection_Vec3<double>::operator/=(const double theInvFactor)
{
  Multiply(1.0 / theInvFactor);
  return *this;
}

#endif // _NCollection_Vec3_H__
