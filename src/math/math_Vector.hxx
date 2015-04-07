// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _math_Vector_HeaderFile
#define _math_Vector_HeaderFile

#include <math_SingleTab.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

class Standard_DimensionError;
class Standard_DivideByZero;
class Standard_RangeError;
class Standard_NullValue;
class math_Matrix;

//! This class implements the real vector abstract data type.
//! Vectors can have an arbitrary range which must be defined at
//! the declaration and cannot be changed after this declaration.
//! @code
//!    math_Vector V1(-3, 5); // a vector with range [-3..5]
//! @endcode
//!
//! Vector are copied through assignement :
//! @code
//!    math_Vector V2( 1, 9);
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
//!    math_Vector V3(1, 2);
//!    V3 = V1;    // --> will raise DimensionError;
//!    V1.Add(V3)  // --> will raise DimensionError;
//! @endcode
class math_Vector
{
public:

  DEFINE_STANDARD_ALLOC

  //! Contructs a non-initialized vector in the range [theLower..theUpper]
  //! "theLower" and "theUpper" are the indexes of the lower and upper bounds of the constructed vector.
  Standard_EXPORT math_Vector(const Standard_Integer theLower, const Standard_Integer theUpper);

  //! Contructs a vector in the range [theLower..theUpper]
  //! whose values are all initialized with the value "theInitialValue"
  Standard_EXPORT math_Vector(const Standard_Integer theLower, const Standard_Integer theUpper, const Standard_Real theInitialValue);

  //! Constructs a vector in the range [theLower..theUpper]
  //! with the "c array" theTab.
  Standard_EXPORT math_Vector(const Standard_Address theTab, const Standard_Integer theLower, const Standard_Integer theUpper);

  //! Constructor for converting gp_XY to math_Vector
  Standard_EXPORT math_Vector(const gp_XY& Other);
  
  //! Constructor for converting gp_XYZ to math_Vector
  Standard_EXPORT math_Vector(const gp_XYZ& Other);

  //! Initialize all the elements of a vector with "theInitialValue".
  Standard_EXPORT void Init(const Standard_Real theInitialValue);

  //! Constructs a copy for initialization.
  //! An exception is raised if the lengths of the vectors are different.
  Standard_EXPORT math_Vector(const math_Vector& theOther);

  //! Returns the length of a vector
  inline Standard_Integer Length() const
  {
    return UpperIndex - LowerIndex +1;
  }

  //! Returns the value of the theLower index of a vector.
  inline Standard_Integer Lower() const
  {
    return LowerIndex;
  }

  //! Returns the value of the theUpper index of a vector.
  inline Standard_Integer Upper() const
  {
    return UpperIndex;
  }

  //! Returns the value or the square  of the norm of this vector.
  Standard_EXPORT Standard_Real Norm() const;

  //! Returns the value of the square of the norm of a vector.
  Standard_EXPORT Standard_Real Norm2() const;

  //! Returns the value of the "Index" of the maximum element of a vector.
  Standard_EXPORT Standard_Integer Max() const;

  //! Returns the value of the "Index" of the minimum element  of a vector.
  Standard_EXPORT Standard_Integer Min() const;

  //! Normalizes this vector (the norm of the result
  //! is equal to 1.0) and assigns the result to this vector
  //! Exceptions
  //! Standard_NullValue if this vector is null (i.e. if its norm is
  //! less than or equal to Standard_Real::RealEpsilon().
  Standard_EXPORT void Normalize();

  //! Normalizes this vector (the norm of the result
  //! is equal to 1.0) and creates a new vector
  //! Exceptions
  //! Standard_NullValue if this vector is null (i.e. if its norm is
  //! less than or equal to Standard_Real::RealEpsilon().
  Standard_EXPORT math_Vector Normalized() const;

  //! Inverts this vector and assigns the result to this vector.
  Standard_EXPORT void Invert();

  //! Inverts this vector and creates a new vector.
  Standard_EXPORT math_Vector Inverse() const;

  //! sets a vector from "theI1" to "theI2" to the vector "theV";
  //! An exception is raised if "theI1" is less than "LowerIndex" or "theI2" is greater than "UpperIndex" or "theI1" is greater than "theI2".
  //! An exception is raised if "theI2-theI1+1" is different from the "Length" of "theV".
  Standard_EXPORT void Set(const Standard_Integer theI1, const Standard_Integer theI2, const math_Vector& theV);

  //!Creates a new vector by inverting the values of this vector
  //! between indexes "theI1" and "theI2".
  //! If the values of this vector were (1., 2., 3., 4.,5., 6.),
  //! by slicing it between indexes 2 and 5 the values
  //! of the resulting vector are (1., 5., 4., 3., 2., 6.)
  Standard_EXPORT math_Vector Slice(const Standard_Integer theI1, const Standard_Integer theI2) const;

  //! returns the product of a vector and a real value.
  Standard_EXPORT void Multiply(const Standard_Real theRight);

  void operator *=(const Standard_Real theRight)
  {
    Multiply(theRight);
  }

  //! returns the product of a vector and a real value.
  Standard_EXPORT math_Vector Multiplied(const Standard_Real theRight) const;

  math_Vector operator*(const Standard_Real theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the product of a vector and a real value.
  Standard_EXPORT math_Vector TMultiplied(const Standard_Real theRight) const;

  friend inline math_Vector operator* (const Standard_Real theLeft, const math_Vector& theRight) 
  {
    return theRight.Multiplied(theLeft);
  }

  //! divides a vector by the value "theRight".
  //! An exception is raised if "theRight" = 0.
  Standard_EXPORT void Divide(const Standard_Real theRight);

  void operator /=(const Standard_Real theRight) 
  {
    Divide(theRight);
  }

  //! divides a vector by the value "theRight".
  //! An exception is raised if "theRight" = 0.
  Standard_EXPORT math_Vector Divided(const Standard_Real theRight) const;

  math_Vector operator/(const Standard_Real theRight) const
  {
    return Divided(theRight);
  }

  //! adds the vector "theRight" to a vector.
  //! An exception is raised if the vectors have not the same length.
  //! Warning
  //! In order to avoid time-consuming copying of vectors, it
  //! is preferable to use operator += or the function Add whenever possible.
  Standard_EXPORT void Add(const math_Vector& theRight);

  void operator +=(const math_Vector& theRight) 
  {
    Add(theRight);
  }

  //! adds the vector theRight to a vector.
  //! An exception is raised if the vectors have not the same length.
  //! An exception is raised if the lengths are not equal.
  Standard_EXPORT math_Vector Added(const math_Vector& theRight) const;

  math_Vector operator+(const math_Vector& theRight) const
  {
    return Added(theRight);
  }

  //! sets a vector to the product of the vector "theLeft"
  //! with the matrix "theRight".
  Standard_EXPORT void Multiply(const math_Vector& theLeft, const math_Matrix& theRight);

  //!sets a vector to the product of the matrix "theLeft"
  //! with the vector "theRight".
  Standard_EXPORT void Multiply(const math_Matrix& theLeft, const math_Vector& theRight);

  //! sets a vector to the product of the transpose
  //! of the matrix "theTLeft" by the vector "theRight".
  Standard_EXPORT void TMultiply(const math_Matrix& theTLeft, const math_Vector& theRight);

  //! sets a vector to the product of the vector
  //! "theLeft" by the transpose of the matrix "theTRight".
  Standard_EXPORT void TMultiply(const math_Vector& theLeft, const math_Matrix& theTRight);

  //! sets a vector to the sum of the vector "theLeft"
  //! and the vector "theRight".
  //! An exception is raised if the lengths are different.
  Standard_EXPORT void Add(const math_Vector& theLeft, const math_Vector& theRight);

  //! sets a vector to the Subtraction of the
  //! vector theRight from the vector theLeft.
  //! An exception is raised if the vectors have not the same length.
  //! Warning
  //! In order to avoid time-consuming copying of vectors, it
  //! is preferable to use operator -= or the function
  //! Subtract whenever possible.
  Standard_EXPORT void Subtract(const math_Vector& theLeft,const math_Vector& theRight);

  //! accesses (in read or write mode) the value of index "theNum" of a vector.
  inline Standard_Real& Value(const Standard_Integer theNum) const
  {
    Standard_RangeError_Raise_if(theNum < LowerIndex || theNum > UpperIndex, " ");
    return Array(theNum);
  }

  Standard_Real& operator()(const Standard_Integer theNum) const
  {
    return Value(theNum);
  }

  //! Initialises a vector by copying "theOther".
  //! An exception is raised if the Lengths are differents.
  Standard_EXPORT math_Vector& Initialized(const math_Vector& theOther);

  math_Vector& operator=(const math_Vector& theOther)
  {
    return Initialized(theOther);
  }

  //! returns the inner product of 2 vectors.
  //! An exception is raised if the lengths are not equal.
  Standard_EXPORT Standard_Real Multiplied(const math_Vector& theRight) const;
  Standard_Real operator*(const math_Vector& theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the product of a vector by a matrix.
  Standard_EXPORT math_Vector Multiplied(const math_Matrix& theRight) const;

  math_Vector operator*(const math_Matrix& theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the opposite of a vector.
  Standard_EXPORT math_Vector Opposite();

  math_Vector operator-()
  {
    return Opposite();
  }

  //! returns the subtraction of "theRight" from "me".
  //! An exception is raised if the vectors have not the same length.
  Standard_EXPORT void Subtract(const math_Vector& theRight);

  void operator-=(const math_Vector& theRight)
  {
    Subtract(theRight);
  }

  //! returns the subtraction of "theRight" from "me".
  //! An exception is raised if the vectors have not the same length.
  Standard_EXPORT math_Vector Subtracted(const math_Vector& theRight) const;

  math_Vector operator-(const math_Vector& theRight) const
  {
    return Subtracted(theRight);
  }

  //! returns the multiplication of a real by a vector.
  //! "me" = "theLeft" * "theRight"
  Standard_EXPORT void Multiply(const Standard_Real theLeft,const math_Vector& theRight);

  //! Prints information on the current state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump(Standard_OStream& theO) const;

  friend inline Standard_OStream& operator<<(Standard_OStream& theO, const math_Vector& theVec)
  {
    theVec.Dump(theO);
    return theO;
  }

  friend class math_Matrix;

protected:

  //! Is used internally to set the "theLower" value of the vector.
  void SetLower(const Standard_Integer theLower);

private:

  Standard_Integer LowerIndex;
  Standard_Integer UpperIndex;
  math_SingleTab<Standard_Real> Array;
};

#endif
