// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2023 OPEN CASCADE SAS
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

#ifndef _math_VectorBase_HeaderFile
#define _math_VectorBase_HeaderFile

#include <NCollection_Array1.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

// resolve name collisions with X11 headers
#ifdef Opposite
#undef Opposite
#endif

#include <math_Matrix.hxx>

#include <array>

//! This class implements the real vector abstract data type.
//! Vectors can have an arbitrary range which must be defined at
//! the declaration and cannot be changed after this declaration.
//! @code
//!    math_VectorBase<TheItemType> V1(-3, 5); // a vector with range [-3..5]
//! @endcode
//!
//! Vector are copied through assignment:
//! @code
//!    math_VectorBase<TheItemType> V2( 1, 9);
//!    ....
//!    V2 = V1;
//!    V1(1) = 2.0; // the vector V2 will not be modified.
//! @endcode
//!
//! The Exception RangeError is raised when trying to access outside
//! the range of a vector :
//! @code
//!    V1(11) = 0.0 // --> will raise RangeError;
//! @endcode
//!
//! The Exception DimensionError is raised when the dimensions of two
//! vectors are not compatible :
//! @code
//!    math_VectorBase<TheItemType> V3(1, 2);
//!    V3 = V1;    // --> will raise DimensionError;
//!    V1.Add(V3)  // --> will raise DimensionError;
//! @endcode
template<typename TheItemType>
class math_VectorBase
{
  static const int THE_BUFFER_SIZE = 32;

public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC;
  DEFINE_NCOLLECTION_ALLOC;
public:

  //! Constructs a non-initialized vector in the range [theLower..theUpper]
  //! "theLower" and "theUpper" are the indexes of the lower and upper bounds of the constructed vector.
  inline math_VectorBase(const Standard_Integer theLower, const Standard_Integer theUpper);

  //! Constructs a vector in the range [theLower..theUpper]
  //! whose values are all initialized with the value "theInitialValue"
  inline math_VectorBase(const Standard_Integer theLower, const Standard_Integer theUpper, const TheItemType theInitialValue);

  //! Constructs a vector in the range [theLower..theUpper]
  //! whose values are all initialized with the value "theInitialValue"
  inline math_VectorBase(const TheItemType* theTab, const Standard_Integer theLower, const Standard_Integer theUpper);

  //! Constructor for converting gp_XY to math_VectorBase
  inline math_VectorBase(const gp_XY& Other);

  //! Constructor for converting gp_XYZ to math_VectorBase
  inline math_VectorBase(const gp_XYZ& Other);

  //! Initialize all the elements of a vector with "theInitialValue".
  void Init(const TheItemType theInitialValue);

  //! Constructs a copy for initialization.
  //! An exception is raised if the lengths of the vectors are different.
  inline math_VectorBase(const math_VectorBase& theOther);

  //! Returns the length of a vector
  inline Standard_Integer Length() const
  {
    return Array.Length();
  }

  //! Returns the lower index of the vector
  inline Standard_Integer Lower() const
  {
    return Array.Lower();
  }

  //! Returns the upper index of the vector
  inline Standard_Integer Upper() const
  {
    return Array.Upper();
  }

  //! Returns the value or the square  of the norm of this vector.
  inline Standard_Real Norm() const;

  //! Returns the value of the square of the norm of a vector.
  inline Standard_Real Norm2() const;

  //! Returns the index of the maximum element of a vector. (first found)
  inline Standard_Integer Max() const;

  //!  Returns the index of the  minimum element  of a vector. (first found)
  inline Standard_Integer Min() const;

  //! Normalizes this vector (the norm of the result
  //! is equal to 1.0) and assigns the result to this vector
  //! Exceptions
  //! Standard_NullValue if this vector is null (i.e. if its norm is
  //! less than or equal to Standard_Real::RealEpsilon().
  inline void Normalize();

  //! Normalizes this vector (the norm of the result
  //! is equal to 1.0) and creates a new vector
  //! Exceptions
  //! Standard_NullValue if this vector is null (i.e. if its norm is
  //! less than or equal to Standard_Real::RealEpsilon().
  Standard_NODISCARD inline math_VectorBase Normalized() const;

  //! Inverts this vector and assigns the result to this vector.
  inline void Invert();

  //! Inverts this vector and creates a new vector.
  inline math_VectorBase Inverse() const;

  //! sets a vector from "theI1" to "theI2" to the vector "theV";
  //! An exception is raised if "theI1" is less than "LowerIndex" or "theI2" is greater than "UpperIndex" or "theI1" is greater than "theI2".
  //! An exception is raised if "theI2-theI1+1" is different from the "Length" of "theV".
  inline void Set(const Standard_Integer theI1, const Standard_Integer theI2, const math_VectorBase& theV);

  //!Creates a new vector by inverting the values of this vector
  //! between indexes "theI1" and "theI2".
  //! If the values of this vector were (1., 2., 3., 4.,5., 6.),
  //! by slicing it between indexes 2 and 5 the values
  //! of the resulting vector are (1., 5., 4., 3., 2., 6.)
  inline math_VectorBase Slice(const Standard_Integer theI1, const Standard_Integer theI2) const;

  //! Updates current vector by multiplying each element on current value.
  inline void Multiply(const TheItemType theRight);

  void operator *=(const TheItemType theRight)
  {
    Multiply(theRight);
  }

  //! returns the product of a vector and a real value.
  Standard_NODISCARD inline math_VectorBase Multiplied(const TheItemType theRight) const;

  Standard_NODISCARD math_VectorBase operator*(const TheItemType theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the product of a vector and a real value.
  Standard_NODISCARD inline math_VectorBase TMultiplied(const TheItemType theRight) const;

  friend inline math_VectorBase operator* (const TheItemType theLeft, const math_VectorBase& theRight)
  {
    return theRight.Multiplied(theLeft);
  }

  //! divides a vector by the value "theRight".
  //! An exception is raised if "theRight" = 0.
  inline void Divide(const TheItemType theRight);

  void operator /=(const TheItemType theRight)
  {
    Divide(theRight);
  }

  //! Returns new vector as dividing current vector with the value "theRight".
  //! An exception is raised if "theRight" = 0.
  Standard_NODISCARD inline math_VectorBase Divided(const TheItemType theRight) const;

  Standard_NODISCARD math_VectorBase operator/(const TheItemType theRight) const
  {
    return Divided(theRight);
  }

  //! adds the vector "theRight" to a vector.
  //! An exception is raised if the vectors have not the same length.
  //! Warning
  //! In order to avoid time-consuming copying of vectors, it
  //! is preferable to use operator += or the function Add whenever possible.
  inline void Add(const math_VectorBase& theRight);

  void operator +=(const math_VectorBase& theRight)
  {
    Add(theRight);
  }

  //! Returns new vector as adding curent vector with the value "theRight".
  //! An exception is raised if the vectors have not the same length.
  //! An exception is raised if the lengths are not equal.
  Standard_NODISCARD inline math_VectorBase Added(const math_VectorBase& theRight) const;

  Standard_NODISCARD math_VectorBase operator+(const math_VectorBase& theRight) const
  {
    return Added(theRight);
  }

  //! sets a vector to the product of the vector "theLeft"
  //! with the matrix "theRight".
  inline void Multiply(const math_VectorBase& theLeft, const math_Matrix& theRight);

  //!sets a vector to the product of the matrix "theLeft"
  //! with the vector "theRight".
  inline void Multiply(const math_Matrix& theLeft, const math_VectorBase& theRight);

  //! sets a vector to the product of the transpose
  //! of the matrix "theTLeft" by the vector "theRight".
  inline void TMultiply(const math_Matrix& theTLeft, const math_VectorBase& theRight);

  //! sets a vector to the product of the vector
  //! "theLeft" by the transpose of the matrix "theTRight".
  inline void TMultiply(const math_VectorBase& theLeft, const math_Matrix& theTRight);

  //! sets a vector to the sum of the vector "theLeft"
  //! and the vector "theRight".
  //! An exception is raised if the lengths are different.
  inline void Add(const math_VectorBase& theLeft, const math_VectorBase& theRight);

  //! sets a vector to the Subtraction of the
  //! vector theRight from the vector theLeft.
  //! An exception is raised if the vectors have not the same length.
  //! Warning
  //! In order to avoid time-consuming copying of vectors, it
  //! is preferable to use operator -= or the function
  //! Subtract whenever possible.
  inline void Subtract(const math_VectorBase& theLeft, const math_VectorBase& theRight);

  //! accesses the value of index "theNum" of a vector.
  const TheItemType& Value(const Standard_Integer theNum) const
  {
    return Array(theNum);
  }

  //! accesses (in read or write mode) the value of index "theNum" of a vector.
  inline TheItemType& Value(const Standard_Integer theNum)
  {
    return Array(theNum);
  }

  const TheItemType& operator()(const Standard_Integer theNum) const
  {
    return Value(theNum);
  }

  TheItemType& operator()(const Standard_Integer theNum)
  {
    return Value(theNum);
  }

  //! Initialises a vector by copying "theOther".
  //! An exception is raised if the Lengths are different.
  inline math_VectorBase& Initialized(const math_VectorBase& theOther);

  math_VectorBase& operator=(const math_VectorBase& theOther)
  {
    return Initialized(theOther);
  }

  //! returns the inner product of 2 vectors.
  //! An exception is raised if the lengths are not equal.
  Standard_NODISCARD inline TheItemType Multiplied(const math_VectorBase& theRight) const;
  Standard_NODISCARD inline TheItemType operator*(const math_VectorBase& theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the product of a vector by a matrix.
  Standard_NODISCARD inline math_VectorBase Multiplied(const math_Matrix& theRight) const;

  Standard_NODISCARD math_VectorBase operator*(const math_Matrix& theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the opposite of a vector.
  inline math_VectorBase Opposite();

  math_VectorBase operator-()
  {
    return Opposite();
  }

  //! returns the subtraction of "theRight" from "me".
  //! An exception is raised if the vectors have not the same length.
  inline void Subtract(const math_VectorBase& theRight);

  void operator-=(const math_VectorBase& theRight)
  {
    Subtract(theRight);
  }

  //! returns the subtraction of "theRight" from "me".
  //! An exception is raised if the vectors have not the same length.
  Standard_NODISCARD inline math_VectorBase Subtracted(const math_VectorBase& theRight) const;

  Standard_NODISCARD math_VectorBase operator-(const math_VectorBase& theRight) const
  {
    return Subtracted(theRight);
  }

  //! returns the multiplication of a real by a vector.
  //! "me" = "theLeft" * "theRight"
  inline void Multiply(const TheItemType theLeft, const math_VectorBase& theRight);

  //! Prints information on the current state of the object.
  //! Is used to redefine the operator <<.
  inline void Dump(Standard_OStream& theO) const;

  friend inline Standard_OStream& operator<<(Standard_OStream& theO, const math_VectorBase& theVec)
  {
    theVec.Dump(theO);
    return theO;
  }

  friend class math_Matrix;

protected:

  //! Is used internally to set the "theLower" value of the vector.
  inline void SetLower(const Standard_Integer theLower);

private:
  std::array<TheItemType, THE_BUFFER_SIZE> myBuffer;
  NCollection_Array1<TheItemType> Array;
};

#include <math_VectorBase.lxx>

#endif
