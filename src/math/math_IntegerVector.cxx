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

#include <math_IntegerVector.hxx>

#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>

math_IntegerVector::math_IntegerVector(const Standard_Integer theFirst, const Standard_Integer theLast)
: myLocArray (theLast - theFirst + 1),
  Array (myLocArray[0], theFirst, theLast)
{
  //
}

math_IntegerVector::math_IntegerVector(const Standard_Integer theFirst,
                                       const Standard_Integer theLast,
                                       const Standard_Integer theInitialValue)
: myLocArray (theLast - theFirst + 1),
  Array (myLocArray[0], theFirst, theLast)
{
  Array.Init(theInitialValue);
}

math_IntegerVector::math_IntegerVector(const Standard_Integer* theTab,
                                       const Standard_Integer theFirst,
                                       const Standard_Integer theLast)
: Array (*theTab, theFirst, theLast)
{
  Standard_RangeError_Raise_if(theFirst > theLast, " ");
}

void math_IntegerVector::Init(const Standard_Integer theInitialValue)
{
  Array.Init(theInitialValue);
}

math_IntegerVector::math_IntegerVector (const math_IntegerVector& theOther)
: myLocArray (theOther.Length()),
  Array (myLocArray[0], theOther.Lower(), theOther.Upper())
{
  memcpy (&myLocArray[0], &theOther.Array.First(), sizeof(Standard_Integer) * theOther.Length());
}

void math_IntegerVector::SetFirst(const Standard_Integer theFirst)
{
  Array.Resize (theFirst, Array.Upper() - Array.Lower() + theFirst, Standard_False);
}

Standard_Real math_IntegerVector::Norm() const
{
  Standard_Real Result = 0;
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Result = Result + Array(Index) * Array(Index);
  }
  return Sqrt(Result);
}

Standard_Real math_IntegerVector::Norm2() const
{
  Standard_Real Result = 0;
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Result = Result + Array(Index) * Array(Index);
  }
  return Result;
}

Standard_Integer math_IntegerVector::Max() const
{
  Standard_Integer I=0;
  Standard_Real X = RealFirst();
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
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
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
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

  for(Standard_Integer Index = Lower(); Index <= Lower() + Length() / 2 ; Index++)
  {
      J = Upper() + Lower() - Index;
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
  Standard_DimensionError_Raise_if((theI1 < Lower()) || (theI2 > Upper()) ||
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
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Array(Index) = Array(Index) * theRight;
  }
}

void math_IntegerVector::Add(const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");

  Standard_Integer I = theRight.Lower();
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Array(Index) = Array(Index) + theRight.Array(I);
    I++;
  }
}

void math_IntegerVector::Subtract(const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");
  Standard_Integer I = theRight.Lower();
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Array(Index) = Array(Index) - theRight.Array(I);
    I++;
  }
}

math_IntegerVector math_IntegerVector::Slice(const Standard_Integer theI1,
                                             const Standard_Integer theI2) const
{
  Standard_DimensionError_Raise_if((theI1 < Lower()) || (theI1 > Upper()) ||
    (theI2 < Lower()) || (theI2 > Upper()), " ");

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

  Standard_Integer I = theRight.Lower();
  for(Standard_Integer Index = 0; Index < Length(); Index++)
  {
    Result = Result + Array(Index) * theRight.Array(I);
    I++;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Multiplied (const Standard_Integer theRight)const
{
  math_IntegerVector Result(Lower(), Upper());

  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Result.Array(Index) = Array(Index) * theRight;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::TMultiplied (const Standard_Integer theRight) const
{
  math_IntegerVector Result(Lower(), Upper());

  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Result.Array(Index) = Array(Index) * theRight;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Added (const math_IntegerVector& theRight) const
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");

  math_IntegerVector Result(Lower(), Upper());

  Standard_Integer I = theRight.Lower();
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Result.Array(Index) = Array(Index) + theRight.Array(I);
    I++;
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Opposite()
{
  math_IntegerVector Result(Lower(), Upper());

  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Result.Array(Index) = - Array(Index);
  }
  return Result;
}

math_IntegerVector math_IntegerVector::Subtracted (const math_IntegerVector& theRight) const
{
  Standard_DimensionError_Raise_if(Length() != theRight.Length(), " ");

  math_IntegerVector Result(Lower(), Upper());

  Standard_Integer I = theRight.Lower();
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
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

  Standard_Integer I = theLeft.Lower();
  Standard_Integer J = theRight.Lower();
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
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

  Standard_Integer I = theLeft.Lower();
  Standard_Integer J = theRight.Lower();
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    Array(Index) = theLeft.Array(I) - theRight.Array(J);
    I++;
    J++;
  }
}

void math_IntegerVector::Multiply(const Standard_Integer theLeft, const math_IntegerVector& theRight)
{
  Standard_DimensionError_Raise_if((Length() != theRight.Length()), " ");
  for(Standard_Integer I = Lower(); I <= Upper(); I++)
  {
    Array(I) = theLeft * theRight.Array(I);
  }
}

math_IntegerVector& math_IntegerVector::Initialized(const math_IntegerVector& theOther)
{
  Standard_DimensionError_Raise_if(Length() != theOther.Length(), " ");
  memmove (&Array.ChangeFirst(), &theOther.Array.First(), sizeof(Standard_Integer) * Array.Length());
  return *this;
}

void math_IntegerVector::Dump(Standard_OStream& theO) const
{
  theO << "math_IntegerVector of Range = " << Length() << "\n";
  for(Standard_Integer Index = Lower(); Index <= Upper(); Index++)
  {
    theO << "math_IntegerVector(" << Index << ") = " << Array(Index) << "\n";
  }
}
