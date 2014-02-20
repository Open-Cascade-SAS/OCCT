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

#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

#include <math_IntegerVector.hxx>

#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>

math_IntegerVector::math_IntegerVector(const Standard_Integer theFirst, const Standard_Integer theLast) :
  FirstIndex(theFirst),
  LastIndex(theLast),
  Array(theFirst, theLast)
{
  Standard_RangeError_Raise_if(theFirst > theLast, " ");
}

math_IntegerVector::math_IntegerVector(const Standard_Integer theFirst,
                                       const Standard_Integer theLast,
                                       const Standard_Integer theInitialValue) :
  FirstIndex(theFirst),
  LastIndex(theLast),
  Array(theFirst, theLast)
{
  Standard_RangeError_Raise_if(theFirst > theLast, " ");
  Array.Init(theInitialValue);
}

math_IntegerVector::math_IntegerVector(const Standard_Address theTab,
                                       const Standard_Integer theFirst,
                                       const Standard_Integer theLast) :
  FirstIndex(theFirst),
  LastIndex(theLast),
  Array(theTab, theFirst, theLast)
{
  Standard_RangeError_Raise_if(theFirst > theLast, " ");
}

void math_IntegerVector::Init(const Standard_Integer theInitialValue)
{
  Array.Init(theInitialValue);
}

math_IntegerVector::math_IntegerVector(const math_IntegerVector& theOther) :
  FirstIndex(theOther.FirstIndex),
  LastIndex(theOther.LastIndex),
  Array(theOther.Array)
{
}

void math_IntegerVector::SetFirst(const Standard_Integer theFirst)
{
  Array.SetLower(theFirst);
  LastIndex = LastIndex - FirstIndex + theFirst;
  FirstIndex = theFirst;
}

Standard_Real math_IntegerVector::Norm() const
{
  Standard_Real Result = 0;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Result = Result + Array(Index) * Array(Index);
  }
  return Sqrt(Result);
}

Standard_Real math_IntegerVector::Norm2() const
{
  Standard_Real Result = 0;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Result = Result + Array(Index) * Array(Index);
  }
  return Result;
}

Standard_Integer math_IntegerVector::Max() const
{
  Standard_Integer I=0;
  Standard_Real X = RealFirst();
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    if(Array(Index) > X)
    {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}

Standard_Integer math_IntegerVector::Min() const
{
  Standard_Integer I=0;
  Standard_Real X = RealLast();
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    if(Array(Index) < X)
    {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}

void math_IntegerVector::Invert()
{
  Standard_Integer J;
  Standard_Integer Temp;

  for(Standard_Integer Index = FirstIndex; Index <= FirstIndex + Length() / 2 ; Index++)
  {
      J = LastIndex + FirstIndex - Index;
      Temp = Array(Index);
      Array(Index) = Array(J);
      Array(J) = Temp;
  }
}

math_IntegerVector math_IntegerVector::Inverse() const
{
  math_IntegerVector Result = *this;
  Result.Invert();
  return Result;
}

void math_IntegerVector::Set(const Standard_Integer theI1,
                             const Standard_Integer theI2,
                             const math_IntegerVector &theV)
{
  Standard_DimensionError_Raise_if((theI1 < FirstIndex) || (theI2 > LastIndex) ||
    (theI1 > theI2) || (theI2 - theI1 + 1 != theV.Length()), " ");

  Standard_Integer I = theV.Lower();
  for(Standard_Integer Index = theI1; Index <= theI2; Index++)
  {
    Array(Index) = theV.Array(I);
    I++;
  }
}

void math_IntegerVector::Multiply(const Standard_Integer theRight)
{
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Array(Index) = Array(Index) * theRight;
  }
}

void math_IntegerVector::Add(const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");

  Standard_Integer I = theRight.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Array(Index) = Array(Index) + theRight.Array(I);
    I++;
  }
}

void math_IntegerVector::Subtract(const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");
  Standard_Integer I = theRight.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Array(Index) = Array(Index) - theRight.Array(I);
    I++;
  }
}

math_IntegerVector math_IntegerVector::Slice(const Standard_Integer theI1,
                                             const Standard_Integer theI2) const
{
  Standard_DimensionError_Raise_if((theI1 < FirstIndex) || (theI1 > LastIndex) ||
    (theI2 < FirstIndex) || (theI2 > LastIndex), " ");

  if(theI2 >= theI1)
  {
    math_IntegerVector Result(theI1, theI2);
    for(Standard_Integer Index = theI1; Index <= theI2; Index++)
    {
      Result.Array(Index) = Array(Index);
    }
    return Result;
  }
  else
  {
    math_IntegerVector Result(theI2, theI1);
    for(Standard_Integer Index = theI1; Index >= theI2; Index--)
    {
      Result.Array(Index) = Array(Index);
    }
    return Result;
  }
}

Standard_Integer math_IntegerVector::Multiplied (const math_IntegerVector& theRight) const
{
  Standard_Integer Result = 0;

  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");

  Standard_Integer I = theRight.FirstIndex;
  for(Standard_Integer Index = 0; Index < Length(); Index++)
  {
    Result = Result + Array(Index) * theRight.Array(I);
    I++;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Multiplied (const Standard_Integer theRight)const
{
  math_IntegerVector Result(FirstIndex, LastIndex);

  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Result.Array(Index) = Array(Index) * theRight;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::TMultiplied (const Standard_Integer theRight) const
{
  math_IntegerVector Result(FirstIndex, LastIndex);

  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Result.Array(Index) = Array(Index) * theRight;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Added (const math_IntegerVector& theRight) const
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");

  math_IntegerVector Result(FirstIndex, LastIndex);

  Standard_Integer I = theRight.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Result.Array(Index) = Array(Index) + theRight.Array(I);
    I++;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Opposite()
{
  math_IntegerVector Result(FirstIndex, LastIndex);

  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Result.Array(Index) = - Array(Index);
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Subtracted (const math_IntegerVector& theRight) const
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");

  math_IntegerVector Result(FirstIndex, LastIndex);

  Standard_Integer I = theRight.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Result.Array(Index) = Array(Index) - theRight.Array(I);
    I++;
  }
  return Result;
}

void math_IntegerVector::Add (const math_IntegerVector& theLeft, const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if((Length() != theRight.Length()) ||
    (theRight.Length() != theLeft.Length()), " ");

  Standard_Integer I = theLeft.FirstIndex;
  Standard_Integer J = theRight.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Array(Index) = theLeft.Array(I) + theRight.Array(J);
    I++;
    J++;
  }
}

void math_IntegerVector::Subtract (const math_IntegerVector& theLeft,
                                   const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if((Length() != theRight.Length()) ||
    (theRight.Length() != theLeft.Length()), " ");

  Standard_Integer I = theLeft.FirstIndex;
  Standard_Integer J = theRight.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    Array(Index) = theLeft.Array(I) - theRight.Array(J);
    I++;
    J++;
  }
}

void math_IntegerVector::Multiply(const Standard_Integer theLeft, const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if((Length() != theRight.Length()), " ");
  for(Standard_Integer I = FirstIndex; I <= LastIndex; I++)
  {
    Array(I) = theLeft * theRight.Array(I);
  }
}

math_IntegerVector& math_IntegerVector::Initialized(const math_IntegerVector& theOther)
{
  Standard_DimensionError_Raise_if(Length() != theOther.Length(), " ");

  (theOther.Array).Copy(Array);
  return *this;
}

void math_IntegerVector::Dump(Standard_OStream& theO) const
{
  theO << "math_IntegerVector of Range = " << Length() << "\n";
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++)
  {
    theO << "math_IntegerVector(" << Index << ") = " << Array(Index) << "\n";
  }
}

