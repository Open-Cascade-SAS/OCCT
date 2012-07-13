// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _NCollection_Vec4_H__
#define _NCollection_Vec4_H__

#include <NCollection_Vec3.hxx>

//! Generic 4-components vector.
//! To be used as RGBA color vector or XYZW 3D-point with special W-component
//! for operations with projection / model view matrices.
//! Use this class for 3D-points carefully because declared W-component may
//! results in incorrect results if used without matrices.
template<typename Element_t>
class NCollection_Vec4
{

public:

  //! Returns the number of components.
  static size_t Length()
  {
    return 4;
  }

  //! Empty constructor. Construct the zero vector.
  NCollection_Vec4()
  {
    std::memset (this, 0, sizeof(NCollection_Vec4));
  }

  //! Initialize ALL components of vector within specified value.
  explicit NCollection_Vec4 (const Element_t theValue)
  {
    v[0] = v[1] = v[2] = v[3] = theValue;
  }

  //! Per-component constructor.
  explicit NCollection_Vec4 (const Element_t theX,
                             const Element_t theY,
                             const Element_t theZ,
                             const Element_t theW)
  {
    v[0] = theX;
    v[1] = theY;
    v[2] = theZ;
    v[3] = theW;
  }

  //! Constructor from 2-components vector.
  explicit NCollection_Vec4 (const NCollection_Vec2<Element_t>& theVec2)
  {
    v[0] = theVec2[0];
    v[1] = theVec2[1];
    v[2] = v[3] = Element_t (0);
  }

  //! Constructor from 3-components vector.
  explicit NCollection_Vec4(const NCollection_Vec3<Element_t>& theVec3)
  {
    std::memcpy (this, &theVec3, sizeof(NCollection_Vec3<Element_t>));
    v[3] = Element_t (0);
  }

  //! Constructor from 3-components vector + alpha value.
  explicit NCollection_Vec4(const NCollection_Vec3<Element_t>& theVec3,
                            const Element_t                    theAlpha) {
    std::memcpy (this, &theVec3, sizeof(NCollection_Vec3<Element_t>));
    v[3] = theAlpha;
  }

  //! Copy constructor.
  NCollection_Vec4 (const NCollection_Vec4& theVec4)
  {
    std::memcpy (this, &theVec4, sizeof(NCollection_Vec4));
  }

  //! Assignment operator.
  const NCollection_Vec4& operator= (const NCollection_Vec4& theVec4)
  {
    std::memcpy (this, &theVec4, sizeof(NCollection_Vec4));
    return *this;
  }

  //! Alias to 1st component as X coordinate in XYZW.
  Element_t x() const { return v[0]; }

  //! Alias to 1st component as RED channel in RGBA.
  Element_t r() const { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZW.
  Element_t y() const { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGBA.
  Element_t g() const { return v[1]; }

  //! Alias to 3rd component as Z coordinate in XYZW.
  Element_t z() const { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGBA.
  Element_t b() const { return v[2]; }

  //! Alias to 4th component as W coordinate in XYZW.
  Element_t w() const { return v[3]; }

  //! Alias to 4th component as ALPHA channel in RGBA.
  Element_t a() const { return v[3]; }

  //! @return 2 of XYZW components in specified order as vector in GLSL-style
  NCOLLECTION_VEC_COMPONENTS_2D(x, y);
  NCOLLECTION_VEC_COMPONENTS_2D(x, z);
  NCOLLECTION_VEC_COMPONENTS_2D(x, w);
  NCOLLECTION_VEC_COMPONENTS_2D(y, z);
  NCOLLECTION_VEC_COMPONENTS_2D(y, w);
  NCOLLECTION_VEC_COMPONENTS_2D(z, w);

  //! @return 3 of XYZW components in specified order as vector in GLSL-style
  NCOLLECTION_VEC_COMPONENTS_3D(x, y, z);
  NCOLLECTION_VEC_COMPONENTS_3D(x, y, w);
  NCOLLECTION_VEC_COMPONENTS_3D(x, z, w);
  NCOLLECTION_VEC_COMPONENTS_3D(y, z, w);

  //! @return RGB components as vector
  NCOLLECTION_VEC_COMPONENTS_3D(r, g, b);

  //! Alias to 1st component as X coordinate in XYZW.
  Element_t& x() { return v[0]; }

  //! Alias to 1st component as RED channel in RGBA.
  Element_t& r() { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XYZW.
  Element_t& y() { return v[1]; }

  //! Alias to 2nd component as GREEN channel in RGBA.
  Element_t& g() { return v[1]; } // Green color

  //! Alias to 3rd component as Z coordinate in XYZW.
  Element_t& z() { return v[2]; }

  //! Alias to 3rd component as BLUE channel in RGBA.
  Element_t& b() { return v[2]; }

  //! Alias to 4th component as W coordinate in XYZW.
  Element_t& w() { return v[3]; }

  //! Alias to 4th component as ALPHA channel in RGBA.
  Element_t& a() { return v[3]; }

  //! @return XY-components modifiable vector
  NCollection_Vec2<Element_t>& xy()
  {
    return *((NCollection_Vec2<Element_t>* )&v[0]);
  }

  //! @return YZ-components modifiable vector
  NCollection_Vec2<Element_t>& yz()
  {
    return *((NCollection_Vec2<Element_t>* )&v[1]);
  }

  //! @return YZ-components modifiable vector
  NCollection_Vec2<Element_t>& zw()
  {
    return *((NCollection_Vec2<Element_t>* )&v[2]);
  }

  //! @return XYZ-components modifiable vector
  NCollection_Vec3<Element_t>& xyz()
  {
    return *((NCollection_Vec3<Element_t>* )&v[0]);
  }

  //! @return YZW-components modifiable vector
  NCollection_Vec3<Element_t>& yzw()
  {
    return *((NCollection_Vec3<Element_t>* )&v[1]);
  }

  //! Raw access to the data (for OpenGL exchange).
  const Element_t* GetData() const { return v; }
  operator const Element_t*() const { return v; }
  operator Element_t*() { return v; }

  //! Compute per-component summary.
  NCollection_Vec4& operator+= (const NCollection_Vec4& theAdd)
  {
    v[0] += theAdd.v[0];
    v[1] += theAdd.v[1];
    v[2] += theAdd.v[2];
    v[3] += theAdd.v[3];
    return *this;
  }

  //! Compute per-component summary.
  friend NCollection_Vec4 operator+ (const NCollection_Vec4& theLeft,
                                     const NCollection_Vec4& theRight)
  {
    NCollection_Vec4 aSumm = NCollection_Vec4 (theLeft);
    return aSumm += theRight;
  }

  //! Compute per-component subtraction.
  NCollection_Vec4& operator-= (const NCollection_Vec4& theDec)
  {
    v[0] -= theDec.v[0];
    v[1] -= theDec.v[1];
    v[2] -= theDec.v[2];
    v[3] -= theDec.v[3];
    return *this;
  }

  //! Compute per-component subtraction.
  friend NCollection_Vec4 operator- (const NCollection_Vec4& theLeft,
                                     const NCollection_Vec4& theRight)
  {
    NCollection_Vec4 aSumm = NCollection_Vec4 (theLeft);
    return aSumm -= theRight;
  }

  //! Compute per-component multiplication.
  NCollection_Vec4& operator*= (const NCollection_Vec4& theRight)
  {
    v[0] *= theRight.v[0];
    v[1] *= theRight.v[1];
    v[2] *= theRight.v[2];
    v[3] *= theRight.v[3];
    return *this;
  }

  //! Compute per-component multiplication.
  friend NCollection_Vec4 operator* (const NCollection_Vec4& theLeft,
                                     const NCollection_Vec4& theRight)
  {
    NCollection_Vec4 aResult = NCollection_Vec4 (theLeft);
    return aResult *= theRight;
  }

  //! Compute per-component multiplication.
  void Multiply (const Element_t theFactor)
  {
    v[0] *= theFactor;
    v[1] *= theFactor;
    v[2] *= theFactor;
    v[3] *= theFactor;
  }

  //! Compute per-component multiplication.
  NCollection_Vec4& operator*=(const Element_t theFactor)
  {
    Multiply (theFactor);
    return *this;
  }

  //! Compute per-component multiplication.
  NCollection_Vec4 operator* (const Element_t theFactor) const
  {
    return Multiplied (theFactor);
  }

  //! Compute per-component multiplication.
  NCollection_Vec4 Multiplied (const Element_t theFactor) const
  {
    NCollection_Vec4 aCopyVec4 (*this);
    aCopyVec4 *= theFactor;
    return aCopyVec4;
  }

  //! Compute per-component division by scale factor.
  NCollection_Vec4& operator/= (const Element_t theInvFactor)
  {
    v[0] /= theInvFactor;
    v[1] /= theInvFactor;
    v[2] /= theInvFactor;
    v[3] /= theInvFactor;
    return *this;
  }

  //! Compute per-component division by scale factor.
  NCollection_Vec4 operator/ (const Element_t theInvFactor)
  {
    NCollection_Vec4 aResult(this);
    return aResult /= theInvFactor;
  }

private:

  Element_t v[4]; //!< define the vector as array to avoid structure alignment issues

};

//! Optimized concretization for float type.
template<> inline NCollection_Vec4<float>& NCollection_Vec4<float>::operator/= (const float theInvFactor)
{
  Multiply (1.0f / theInvFactor);
  return *this;
}

//! Optimized concretization for double type.
template<> inline NCollection_Vec4<double>& NCollection_Vec4<double>::operator/= (const double theInvFactor)
{
  Multiply (1.0 / theInvFactor);
  return *this;
}

#endif // _NCollection_Vec4_H__
