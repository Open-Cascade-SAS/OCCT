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

#ifndef _NCollection_Vec2_H__
#define _NCollection_Vec2_H__

//! Auxiliary macros to define couple of similar access components as vector methods.
//! @return 2 components by their names in specified order
#define NCOLLECTION_VEC_COMPONENTS_2D(theX, theY) \
  const NCollection_Vec2<Element_t> theX##theY##() const { return NCollection_Vec2<Element_t>(theX##(), theY##()); } \
  const NCollection_Vec2<Element_t> theY##theX##() const { return NCollection_Vec2<Element_t>(theY##(), theX##()); }

//! Defines the 2D-vector template.
//! The main target for this class - to handle raw low-level arrays (from/to graphic driver etc.).
template<typename Element_t>
class NCollection_Vec2
{

public:

  //! Returns the number of components.
  static int Length()
  {
    return 2;
  }

  //! Empty constructor. Construct the zero vector.
  NCollection_Vec2()
  {
    v[0] = v[1] = Element_t(0);
  }

  //! Initialize ALL components of vector within specified value.
  explicit NCollection_Vec2 (const Element_t theXY)
  {
    v[0] = v[1] = theXY;
  }

  //! Per-component constructor.
  explicit NCollection_Vec2 (const Element_t theX,
                             const Element_t theY)
  {
    v[0] = theX;
    v[1] = theY;
  }

  //! Copy constructor.
  NCollection_Vec2 (const NCollection_Vec2& theVec2)
  {
    v[0] = theVec2[0];
    v[1] = theVec2[1];
  }

  //! Assignment operator.
  const NCollection_Vec2& operator= (const NCollection_Vec2& theVec2)
  {
    v[0] = theVec2[0];
    v[1] = theVec2[1];
    return *this;
  }

  //! Alias to 1st component as X coordinate in XY.
  Element_t x() const { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XY.
  Element_t y() const { return v[1]; }

  //! @return 2 components by their names in specified order (in GLSL-style)
  NCOLLECTION_VEC_COMPONENTS_2D(x, y);

  //! Alias to 1st component as X coordinate in XY.
  Element_t& x() { return v[0]; }

  //! Alias to 2nd component as Y coordinate in XY.
  Element_t& y() { return v[1]; }

  //! Raw access to the data (to simplify OpenGL exchange).
  const Element_t* GetData() const { return v; }
  operator const Element_t*() const { return v; }
  operator Element_t*() { return v; }

  //! Compute per-component summary.
  NCollection_Vec2& operator+= (const NCollection_Vec2& theAdd)
  {
    v[0] += theAdd.v[0];
    v[1] += theAdd.v[1];
    return *this;
  }

  //! Compute per-component summary.
  friend NCollection_Vec2 operator+ (const NCollection_Vec2& theLeft,
                                     const NCollection_Vec2& theRight)
  {
    return NCollection_Vec2 (theLeft.v[0] + theRight.v[0],
                             theLeft.v[1] + theRight.v[1]);
  }

  //! Compute per-component subtraction.
  NCollection_Vec2& operator-= (const NCollection_Vec2& theDec)
  {
    v[0] -= theDec.v[0];
    v[1] -= theDec.v[1];
    return *this;
  }

  //! Compute per-component subtraction.
  friend NCollection_Vec2 operator- (const NCollection_Vec2& theLeft,
                                     const NCollection_Vec2& theRight)
  {
    return NCollection_Vec2 (theLeft.v[0] - theRight.v[0],
                             theLeft.v[1] - theRight.v[1]);
  }

  //! Unary -.
  NCollection_Vec2 operator-() const
  {
    return NCollection_Vec2 (-x(), -y());
  }

  //! Compute per-component multiplication.
  NCollection_Vec2& operator*= (const NCollection_Vec2& theRight)
  {
    v[0] *= theRight.v[0];
    v[1] *= theRight.v[1];
    return *this;
  }

  //! Compute per-component multiplication.
  friend NCollection_Vec2 operator* (const NCollection_Vec2& theLeft,
                                     const NCollection_Vec2& theRight)
  {
    return NCollection_Vec2 (theLeft.v[0] * theRight.v[0],
                             theLeft.v[1] * theRight.v[1]);
  }

  //! Compute per-component multiplication by scale factor.
  void Multiply (const Element_t theFactor)
  {
    v[0] *= theFactor;
    v[1] *= theFactor;
  }

  //! Compute per-component multiplication by scale factor.
  NCollection_Vec2 Multiplied (const Element_t theFactor) const
  {
    return NCollection_Vec2 (v[0] * theFactor,
                             v[1] * theFactor);
  }

  //! Compute per-component multiplication by scale factor.
  NCollection_Vec2& operator*= (const Element_t theFactor)
  {
    Multiply (theFactor);
    return *this;
  }

  //! Compute per-component division by scale factor.
  NCollection_Vec2& operator/= (const Element_t theInvFactor)
  {
    v[0] /= theInvFactor;
    v[1] /= theInvFactor;
    return *this;
  }

  //! Compute per-component multiplication by scale factor.
  NCollection_Vec2 operator* (const Element_t theFactor) const
  {
    return Multiplied (theFactor);
  }

  //! Compute per-component division by scale factor.
  NCollection_Vec2 operator/ (const Element_t theInvFactor) const
  {
    return NCollection_Vec2(v[0] / theInvFactor,
            v[1] / theInvFactor);
  }

  //! Computes the dot product.
  Element_t Dot (const NCollection_Vec2& theOther) const
  {
    return x() * theOther.x() + y() * theOther.y();
  }

  //! Computes the vector modulus (magnitude, length).
  Element_t Modulus() const
  {
    return std::sqrt (x() * x() + y() * y());
  }

  //! Computes the square of vector modulus (magnitude, length).
  //! This method may be used for performance tricks.
  Element_t SquareModulus() const
  {
    return x() * x() + y() * y();
  }

  //! Constuct DX unit vector.
  static NCollection_Vec2 DX()
  {
    return NCollection_Vec2 (Element_t(1), Element_t(0));
  }

  //! Constuct DY unit vector.
  static NCollection_Vec2 DY()
  {
    return NCollection_Vec2 (Element_t(0), Element_t(1));
  }

private:

  Element_t v[2];

};

#endif // _NCollection_Vec2_H__
