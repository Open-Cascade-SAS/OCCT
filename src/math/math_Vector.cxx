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

#include <stdio.h>

#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <Standard_DimensionError.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_NullValue.hxx>

math_Vector::math_Vector(const Standard_Integer theLower, const Standard_Integer theUpper) :
  LowerIndex(theLower),
  UpperIndex(theUpper),
  Array(theLower,theUpper)
{
  Standard_RangeError_Raise_if (theLower > theUpper, "math_Vector() - invalid dimensions");
}

math_Vector::math_Vector(const Standard_Integer theLower,
                         const Standard_Integer theUpper,
                         const Standard_Real    theInitialValue):
  LowerIndex(theLower),
  UpperIndex(theUpper),
  Array(theLower,theUpper)
{
  Standard_RangeError_Raise_if (theLower > theUpper, "math_Vector() - invalid dimensions");
  Array.Init(theInitialValue);
}

math_Vector::math_Vector(const Standard_Address theTab,
                         const Standard_Integer theLower,
                         const Standard_Integer theUpper) :
  LowerIndex(theLower),
  UpperIndex(theUpper),
  Array(theTab, theLower,theUpper)
{
  Standard_RangeError_Raise_if ((theLower > theUpper), "math_Vector() - invalid dimensions");
}

math_Vector::math_Vector(const gp_XY& theOther):
  LowerIndex(1),
  UpperIndex(2),
  Array(1,2)
{
  Array(1) = theOther.X();
  Array(2) = theOther.Y();
}

math_Vector::math_Vector(const gp_XYZ& theOther):
  LowerIndex(1),
  UpperIndex(3),
  Array(1, 3)
{
  Array(1) = theOther.X();
  Array(2) = theOther.Y();
  Array(3) = theOther.Z();
}

void math_Vector::Init(const Standard_Real theInitialValue)
{
  Array.Init(theInitialValue);
}

math_Vector::math_Vector(const math_Vector& theOther) :
  LowerIndex(theOther.LowerIndex),
  UpperIndex(theOther.UpperIndex),
  Array(theOther.Array)
{
}

void math_Vector::SetLower(const Standard_Integer theLower)
{
  Array.SetLower(theLower);
  UpperIndex = UpperIndex - LowerIndex + theLower;
  LowerIndex = theLower;
}

Standard_Real math_Vector::Norm() const
{
  Standard_Real Result = 0;

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result = Result + Array(Index) * Array(Index);
  }
  return Sqrt(Result);
}

Standard_Real math_Vector::Norm2() const
{
  Standard_Real Result = 0;

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result = Result + Array(Index) * Array(Index);
  }
  return Result;
}

Standard_Integer math_Vector::Max() const
{
  Standard_Integer I=0;
  Standard_Real X = RealFirst();

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    if(Array(Index) > X)
    {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}

Standard_Integer math_Vector::Min() const
{
  Standard_Integer I=0;
  Standard_Real X = RealLast();

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    if(Array(Index) < X)
    {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}

void math_Vector::Set(const Standard_Integer theI1,
                      const Standard_Integer theI2,
                      const math_Vector &theV)
{
  Standard_RangeError_Raise_if ((theI1 < LowerIndex) || (theI2 > UpperIndex)
                             || (theI1 > theI2) || (theI2 - theI1 + 1 != theV.Length()),
                                "math_Vector::Set() - invalid indices");

  Standard_Integer I = theV.Lower();
  for(Standard_Integer Index = theI1; Index <= theI2; Index++)
  {
    Array(Index) = theV.Array(I);
    I++;
  }
}

void math_Vector::Normalize()
{
  Standard_Real Result = Norm();
  Standard_NullValue_Raise_if ((Result <= RealEpsilon()),
                                "math_Vector::Normalize() - vector has zero norm");
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Array(Index) = Array(Index) / Result;
  }
}

math_Vector math_Vector::Normalized() const
{
  math_Vector Result = *this;

  Result.Normalize();
  return Result;
}

void math_Vector::Invert()
{
  Standard_Integer J;
  Standard_Real Temp;
  for(Standard_Integer Index = LowerIndex; Index <= (LowerIndex + Length()) >> 1 ; Index++)
  {
    J = UpperIndex + LowerIndex - Index;
    Temp = Array(Index);
    Array(Index) = Array(J);
    Array(J) = Temp;
  }
}

math_Vector math_Vector::Inverse() const
{
  math_Vector Result = *this;
  Result.Invert();
  return Result;
}

math_Vector math_Vector::Multiplied(const Standard_Real theRight) const
{
  math_Vector Result (LowerIndex, UpperIndex);

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result.Array(Index) = Array(Index) * theRight;
  }
  return Result;
}

math_Vector math_Vector::TMultiplied(const Standard_Real theRight) const
{
  math_Vector Result (LowerIndex, UpperIndex);

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result.Array(Index) = Array(Index) * theRight;
  }
  return Result;
}

void math_Vector::Multiply(const Standard_Real theRight)
{
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Array(Index) = Array(Index) * theRight;
  }
}

void math_Vector::Divide(const Standard_Real theRight)
{
  Standard_DivideByZero_Raise_if (Abs(theRight) <= RealEpsilon(),
                                  "math_Vector::Divide() - devisor is zero");

  for(Standard_Integer Index =LowerIndex; Index <=UpperIndex; Index++)
  {
    Array(Index) = Array(Index) / theRight;
  }
}

math_Vector math_Vector::Divided (const Standard_Real theRight) const
{
  Standard_DivideByZero_Raise_if (Abs(theRight) <= RealEpsilon(),
                                  "math_Vector::Divided() - devisor is zero");
  math_Vector temp = Multiplied(1./theRight);
  return temp;
}

void math_Vector::Add(const math_Vector& theRight)
{
  Standard_DimensionError_Raise_if (Length() != theRight.Length(),
                                    "math_Vector::Add() - input vector has wrong dimensions");

  Standard_Integer I = theRight.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Array(Index) = Array(Index) + theRight.Array(I);
    I++;
  }
}

math_Vector math_Vector::Added(const math_Vector& theRight) const
{
  Standard_DimensionError_Raise_if (Length() != theRight.Length(),
                                    "math_Vector::Added() - input vector has wrong dimensions");

  math_Vector Result(LowerIndex, UpperIndex);

  Standard_Integer I = theRight.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result.Array(Index) = Array(Index) + theRight.Array(I);
    I++;
  }
  return Result;
}

void math_Vector::Subtract(const math_Vector& theRight)
{
  Standard_DimensionError_Raise_if (Length() != theRight.Length(),
                                    "math_Vector::Subtract() - input vector has wrong dimensions");

  Standard_Integer I = theRight.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Array(Index) = Array(Index) - theRight.Array(I);
    I++;
  }
}

math_Vector math_Vector::Subtracted (const math_Vector& theRight) const
{
  Standard_DimensionError_Raise_if (Length() != theRight.Length(),
                                    "math_Vector::Subtracted() - input vector has wrong dimensions");

  math_Vector Result(LowerIndex, UpperIndex);

  Standard_Integer I = theRight.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result.Array(Index) = Array(Index) - theRight.Array(I);
    I++;
  }
  return Result;
}

math_Vector math_Vector::Slice(const Standard_Integer theI1, const Standard_Integer theI2) const
{
  Standard_RangeError_Raise_if ((theI1 < LowerIndex) || (theI1 > UpperIndex) || (theI2 < LowerIndex) || (theI2 > UpperIndex),
                                "math_Vector::Slice() - invalid indices");

  if(theI2 >= theI1)
  {
    math_Vector Result(theI1, theI2);
    for(Standard_Integer Index = theI1; Index <= theI2; Index++)
    {
      Result.Array(Index) = Array(Index);
    }
    return Result;
  }
  else
  {
    math_Vector Result(theI2, theI1);
    for(Standard_Integer Index = theI1; Index >= theI2; Index--)
    {
      Result.Array(Index) = Array(Index);
    }
    return Result;
  }
}

void math_Vector::Add (const math_Vector& theLeft, const math_Vector& theRight)
{
  Standard_DimensionError_Raise_if ((Length() != theRight.Length()) || (theRight.Length() != theLeft.Length()),
                                    "math_Vector::Add() - input vectors have wrong dimensions");

  Standard_Integer I = theLeft.LowerIndex;
  Standard_Integer J = theRight.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Array(Index) = theLeft.Array(I) + theRight.Array(J);
    I++;
    J++;
  }
}

void math_Vector::Subtract (const math_Vector& theLeft, const math_Vector& theRight)
{
  Standard_DimensionError_Raise_if ((Length() != theRight.Length()) || (theRight.Length() != theLeft.Length()),
                                     "math_Vector::Subtract() - input vectors have wrong dimensions");

  Standard_Integer I = theLeft.LowerIndex;
  Standard_Integer J = theRight.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Array(Index) = theLeft.Array(I) - theRight.Array(J);
    I++;
    J++;
  }
}

void math_Vector::Multiply(const math_Matrix& theLeft, const math_Vector& theRight)
{
  Standard_DimensionError_Raise_if ((Length() != theLeft.RowNumber())
                                 || (theLeft.ColNumber() != theRight.Length()),
                                    "math_Vector::Multiply() - input matrix and/or vector have wrong dimensions");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer I = theLeft.LowerRowIndex; I <= theLeft.UpperRowIndex; I++)
  {
    Array(Index) = 0.0;
    Standard_Integer K = theRight.LowerIndex;
    for(Standard_Integer J = theLeft.LowerColIndex; J <= theLeft.UpperColIndex; J++)
    {
      Array(Index) = Array(Index) + theLeft.Array(I, J) * theRight.Array(K);
      K++;
    }
    Index++;
  }
}

void math_Vector::Multiply(const math_Vector& theLeft, const math_Matrix& theRight)
{
  Standard_DimensionError_Raise_if ((Length() != theRight.ColNumber())
                                 || (theLeft.Length() != theRight.RowNumber()),
                                    "math_Vector::Multiply() - input matrix and/or vector have wrong dimensions");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer J = theRight.LowerColIndex; J <= theRight.UpperColIndex; J++)
  {
    Array(Index) = 0.0;
    Standard_Integer K = theLeft.LowerIndex;
    for(Standard_Integer I = theRight.LowerRowIndex; I <= theRight.UpperRowIndex; I++)
    {
      Array(Index) = Array(Index) + theLeft.Array(K) * theRight.Array(I, J);
      K++;
    }
    Index++;
  }
}

void math_Vector::TMultiply(const math_Matrix& theTLeft, const math_Vector&  theRight)
{
  Standard_DimensionError_Raise_if ((Length() != theTLeft.ColNumber())
                                 || (theTLeft.RowNumber() != theRight.Length()),
                                    "math_Vector::TMultiply() - input matrix and/or vector have wrong dimensions");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer I = theTLeft.LowerColIndex; I <= theTLeft.UpperColIndex; I++)
  {
    Array(Index) = 0.0;
    Standard_Integer K = theRight.LowerIndex;
    for(Standard_Integer J = theTLeft.LowerRowIndex; J <= theTLeft.UpperRowIndex; J++)
    {
      Array(Index) = Array(Index) + theTLeft.Array(J, I) * theRight.Array(K);
      K++;
    }
    Index++;
  }
}

void math_Vector::TMultiply(const math_Vector&  theLeft, const math_Matrix& theTRight)
{
  Standard_DimensionError_Raise_if ((Length() != theTRight.RowNumber())
                                 || (theLeft.Length() != theTRight.ColNumber()),
                                    "math_Vector::TMultiply() - input matrix and/or vector have wrong dimensions");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer J = theTRight.LowerRowIndex; J <= theTRight.UpperRowIndex; J++)
  {
    Array(Index) = 0.0;
    Standard_Integer K = theLeft.LowerIndex;
    for(Standard_Integer I = theTRight.LowerColIndex;
      I <= theTRight.UpperColIndex; I++)
    {
        Array(Index) = Array(Index) + theLeft.Array(K) * theTRight.Array(J, I);
        K++;
    }
    Index++;
  }
}

Standard_Real math_Vector::Multiplied(const math_Vector& theRight) const
{
  Standard_Real Result = 0;

  Standard_DimensionError_Raise_if (Length() != theRight.Length(),
                                    "math_Vector::Multiplied() - input vector has wrong dimensions");

  Standard_Integer I = theRight.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result = Result + Array(Index) * theRight.Array(I);
    I++;
  }
  return Result;
}

math_Vector math_Vector::Opposite()
{
  math_Vector Result(LowerIndex, UpperIndex);

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    Result.Array(Index) = - Array(Index);
  }
  return Result;
}

math_Vector math_Vector::Multiplied(const math_Matrix& theRight)const
{
  Standard_DimensionError_Raise_if (Length() != theRight.RowNumber(),
                                    "math_Vector::Multiplied() - input matrix has wrong dimensions");

  math_Vector Result(theRight.LowerColIndex, theRight.UpperColIndex);
  for(Standard_Integer J2 = theRight.LowerColIndex; J2 <= theRight.UpperColIndex; J2++)
  {
      Array(J2) = 0.0;
      Standard_Integer theI2 = theRight.LowerRowIndex;
      for(Standard_Integer I = LowerIndex; I <= UpperIndex; I++)
      {
        Result.Array(J2) = Result.Array(J2) + Array(I) * theRight.Array(theI2, J2);
        theI2++;
      }
  }
  return Result;
}

void math_Vector::Multiply(const Standard_Real theLeft, const math_Vector& theRight)
{
  Standard_DimensionError_Raise_if ((Length() != theRight.Length()),
                                    "math_Vector::Multiply() - input vector has wrong dimensions");
  for(Standard_Integer I = LowerIndex; I <= UpperIndex; I++)
  {
    Array(I) = theLeft * theRight.Array(I);
  }
}

math_Vector& math_Vector::Initialized(const math_Vector& theOther)
{
  Standard_DimensionError_Raise_if (Length() != theOther.Length(),
                                    "math_Vector::Initialized() - input vector has wrong dimensions");

  (theOther.Array).Copy(Array);
  return *this;
}

void math_Vector::Dump(Standard_OStream& theO) const
{
  theO << "math_Vector of Length = " << Length() << "\n";
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++)
  {
    theO << "math_Vector(" << Index << ") = " << Array(Index) << "\n";
  }
}

