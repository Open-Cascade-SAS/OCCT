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

#ifndef _math_IntegerVector_HeaderFile
#define _math_IntegerVector_HeaderFile

#include <math_SingleTab.hxx>

// resolve name collisions with X11 headers
#ifdef Opposite
  #undef Opposite
#endif

//! This class implements the real IntegerVector abstract data type.
//! IntegerVectors can have an arbitrary range which must be define at
//! the declaration and cannot be changed after this declaration.
//! Example:
//! @code
//!    math_IntegerVector V1(-3, 5); // an IntegerVector with range [-3..5]
//! @endcode
//!
//! IntegerVector is copied through assignement :
//! @code
//!    math_IntegerVector V2( 1, 9);
//!    ....
//!    V2 = V1;
//!    V1(1) = 2.0; // the IntegerVector V2 will not be modified.
//! @endcode
//!
//! The Exception RangeError is raised when trying to access outside
//! the range of an IntegerVector :
//! @code
//!    V1(11) = 0 // --> will raise RangeError;
//! @endcode
//!
//! The Exception DimensionError is raised when the dimensions of two
//! IntegerVectors are not compatible :
//! @code
//!    math_IntegerVector V3(1, 2);
//!    V3 = V1;    // --> will raise DimensionError;
//!    V1.Add(V3)  // --> will raise DimensionError;
//! @endcode
class math_IntegerVector
{
public:

  DEFINE_STANDARD_ALLOC

  //! contructs an IntegerVector in the range [Lower..Upper]
  Standard_EXPORT math_IntegerVector(const Standard_Integer theFirst, const Standard_Integer theLast);

  //! contructs an IntegerVector in the range [Lower..Upper]
  //! with all the elements set to theInitialValue.
  Standard_EXPORT math_IntegerVector(const Standard_Integer theFirst, const Standard_Integer theLast, const Standard_Integer theInitialValue);

  //! Initialize an IntegerVector with all the elements
  //! set to theInitialValue.
  Standard_EXPORT void Init(const Standard_Integer theInitialValue);

  //! constructs an IntegerVector in the range [Lower..Upper]
  //! which share the "c array" theTab.
  Standard_EXPORT math_IntegerVector(const Standard_Address theTab, const Standard_Integer theFirst, const Standard_Integer theLast);

  //! constructs a copy for initialization.
  //! An exception is raised if the lengths of the IntegerVectors
  //! are different.
  Standard_EXPORT math_IntegerVector(const math_IntegerVector& theOther);

  //! returns the length of an IntegerVector
  inline Standard_Integer Length() const
  {
    return LastIndex - FirstIndex +1;
  }

  //! returns the value of the Lower index of an IntegerVector.
  inline Standard_Integer Lower() const
  {
    return FirstIndex;
  }

  //! returns the value of the Upper index of an IntegerVector.
  inline Standard_Integer Upper() const
  {
    return LastIndex;
  }

  //! returns the value of the norm of an IntegerVector.
  Standard_EXPORT Standard_Real Norm() const;

  //! returns the value of the square of the norm of an IntegerVector.
  Standard_EXPORT Standard_Real Norm2() const;

  //! returns the value of the Index of the maximum element of an IntegerVector.
  Standard_EXPORT Standard_Integer Max() const;

  //! returns the value of the Index of the minimum element of an IntegerVector.
  Standard_EXPORT Standard_Integer Min() const;

  //! inverses an IntegerVector.
  Standard_EXPORT void Invert();

  //! returns the inverse IntegerVector of an IntegerVector.
  Standard_EXPORT math_IntegerVector Inverse() const;

  //! sets an IntegerVector from "theI1" to "theI2" to the IntegerVector "theV";
  //! An exception is raised if "theI1" is less than "LowerIndex" or "theI2" is greater than "UpperIndex" or "theI1" is greater than "theI2".
  //! An exception is raised if "theI2-theI1+1" is different from the Length of "theV".
  Standard_EXPORT void Set(const Standard_Integer theI1, const Standard_Integer theI2, const math_IntegerVector& theV);

  //! slices the values of the IntegerVector between "theI1" and "theI2":
  //! Example: [2, 1, 2, 3, 4, 5] becomes [2, 4, 3, 2, 1, 5] between 2 and 5.
  //! An exception is raised if "theI1" is less than "LowerIndex" or "theI2" is greater than "UpperIndex".
  Standard_EXPORT math_IntegerVector Slice(const Standard_Integer theI1, const Standard_Integer theI2) const;

  //! returns the product of an IntegerVector by an integer value.
  Standard_EXPORT void Multiply(const Standard_Integer theRight);

  void operator *=(const Standard_Integer theRight)
  {
    Multiply(theRight);
  }

  //! returns the product of an IntegerVector by an integer value.
  Standard_EXPORT math_IntegerVector Multiplied(const Standard_Integer theRight) const;

  math_IntegerVector operator*(const Standard_Integer theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the product of a vector and a real value.
  Standard_EXPORT math_IntegerVector TMultiplied(const Standard_Integer theRight) const;

  friend inline math_IntegerVector operator* (const Standard_Integer theLeft, const math_IntegerVector& theRight)
  {
    return theRight.Multiplied(theLeft);
  }

  //! adds the IntegerVector "theRight" to an IntegerVector.
  //! An exception is raised if the IntegerVectors have not the same length.
  //! An exception is raised if the lengths are not equal.
  Standard_EXPORT void Add(const math_IntegerVector& theRight);

  void operator +=(const math_IntegerVector& theRight)
  {
    Add(theRight);
  }

  //! adds the IntegerVector "theRight" to an IntegerVector.
  //! An exception is raised if the IntegerVectors have not the same length.
  //! An exception is raised if the lengths are not equal.
  Standard_EXPORT math_IntegerVector Added(const math_IntegerVector& theRight) const;

  math_IntegerVector operator+(const math_IntegerVector& theRight) const
  {
    return Added(theRight);
  }

  //! sets an IntegerVector to the sum of the IntegerVector
  //! "theLeft" and the IntegerVector "theRight".
  //! An exception is raised if the lengths are different.
  Standard_EXPORT void Add(const math_IntegerVector& theLeft, const math_IntegerVector& theRight);

  //! sets an IntegerVector to the substraction of "theRight" from "theLeft".
  //! An exception is raised if the IntegerVectors have not the same length.
  Standard_EXPORT void Subtract(const math_IntegerVector& theLeft, const math_IntegerVector& theRight);

  //! accesses (in read or write mode) the value of index theNum of an IntegerVector.
  inline Standard_Integer& Value(const Standard_Integer theNum) const
  {
    Standard_RangeError_Raise_if(theNum < FirstIndex || theNum > LastIndex, " ");
    return Array(theNum);
  }

  Standard_EXPORT Standard_Integer& operator()(const Standard_Integer theNum) const
  {
    return Value(theNum);
  }

  //! Initialises an IntegerVector by copying "theOther".
  //! An exception is raised if the Lengths are different.
  Standard_EXPORT math_IntegerVector& Initialized(const math_IntegerVector& theOther);

  math_IntegerVector& operator=(const math_IntegerVector& theOther) 
  {
    return Initialized(theOther);
  }

  //! returns the inner product of 2 IntegerVectors.
  //! An exception is raised if the lengths are not equal.
  Standard_EXPORT Standard_Integer Multiplied(const math_IntegerVector& theRight) const;

  Standard_Integer operator*(const math_IntegerVector& theRight) const
  {
    return Multiplied(theRight);
  }

  //! returns the opposite of an IntegerVector.
  Standard_EXPORT math_IntegerVector Opposite();

  math_IntegerVector operator-()
  {
    return Opposite();
  }

  //! returns the subtraction of "theRight" from "me".
  //! An exception is raised if the IntegerVectors have not the same length.
  Standard_EXPORT void Subtract(const math_IntegerVector& theRight);

  void operator-=(const math_IntegerVector& theRight)
  {
    Subtract(theRight);
  }

  //! returns the subtraction of "theRight" from "me".
  //! An exception is raised if the IntegerVectors have not the same length.
  Standard_EXPORT math_IntegerVector Subtracted(const math_IntegerVector& theRight) const;

  math_IntegerVector operator-(const math_IntegerVector& theRight) const
  {
    return Subtracted(theRight);
  }

  //! returns the multiplication of an integer by an IntegerVector.
  Standard_EXPORT void Multiply(const Standard_Integer theLeft,const math_IntegerVector& theRight);

  //! Prints on the stream theO information on the current state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump(Standard_OStream& theO) const;

  friend inline Standard_OStream& operator<<(Standard_OStream& theO, const math_IntegerVector& theVec)
  {
    theVec.Dump(theO);
    return theO;
  }

protected:

  //! is used internally to set the Lower value of the IntegerVector.
  void SetFirst(const Standard_Integer theFirst);

private:

  Standard_Integer FirstIndex;
  Standard_Integer LastIndex;
  math_SingleTab<Standard_Integer> Array;
};

#endif

