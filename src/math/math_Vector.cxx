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

#include <math_Vector.ixx>
#include <math_Matrix.hxx>

#include <Standard_DimensionError.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_NullValue.hxx>


math_Vector::math_Vector(const Standard_Integer Lower,
			 const Standard_Integer Upper):
			 
			 LowerIndex(Lower),
			 UpperIndex(Upper),
			 Array(Lower,Upper) {
			   Standard_RangeError_Raise_if(Lower > Upper, "");		       
			 }

math_Vector::math_Vector(const Standard_Integer Lower,
			 const Standard_Integer Upper,
			 const Standard_Real    InitialValue):
			 
			 LowerIndex(Lower),
			 UpperIndex(Upper),
			 Array(Lower,Upper) 
{
  Standard_RangeError_Raise_if(Lower > Upper, "");		       
  Array.Init(InitialValue);
}

math_Vector::math_Vector(const Standard_Address Tab,
			 const Standard_Integer Lower,
			 const Standard_Integer Upper) :
			 
			 LowerIndex(Lower),
			 UpperIndex(Upper),
			 Array(*((const Standard_Real *)Tab), Lower,Upper) 
{
  Standard_RangeError_Raise_if((Lower > Upper) , "");		       
}

void math_Vector::Init(const Standard_Real InitialValue) {
  Array.Init(InitialValue);
}

math_Vector::math_Vector(const math_Vector& Other): 

LowerIndex(Other.LowerIndex),
UpperIndex(Other.UpperIndex),
Array(Other.Array) {}


void math_Vector::SetLower(const Standard_Integer Lower) {

  Array.SetLower(Lower);
  UpperIndex = UpperIndex - LowerIndex + Lower;
  LowerIndex = Lower;
}

Standard_Real math_Vector::Norm() const {

  Standard_Real Result = 0;

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result = Result + Array(Index) * Array(Index);
  }
  return Sqrt(Result);
}

Standard_Real math_Vector::Norm2() const {

  Standard_Real Result = 0;

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result = Result + Array(Index) * Array(Index);
  }
  return Result;
}

Standard_Integer math_Vector::Max() const {

  Standard_Integer I=0;
  Standard_Real X = RealFirst();

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    if(Array(Index) > X) {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}

Standard_Integer math_Vector::Min() const {

  Standard_Integer I=0;
  Standard_Real X = RealLast();

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    if(Array(Index) < X) {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}

void math_Vector::Set(const Standard_Integer I1, 
		      const Standard_Integer I2, 
		      const math_Vector &V) {

  Standard_RangeError_Raise_if((I1 < LowerIndex) || 
			       (I2 > UpperIndex) || 
			       (I1 > I2)         ||
			       (I2 - I1 + 1 != V.Length()), "");
  
  Standard_Integer I = V.Lower();
  for(Standard_Integer Index = I1; Index <= I2; Index++) {
    Array(Index) = V.Array(I);
    I++;
  }
}

void math_Vector::Normalize() {

  Standard_Real Result = Norm();
  Standard_NullValue_Raise_if((Result <= RealEpsilon()), "");
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Array(Index) = Array(Index) / Result;
  }
}

math_Vector math_Vector::Normalized() const {

  math_Vector Result = *this;

  Result.Normalize();
  return Result;
}

void math_Vector::Invert() {
  Standard_Integer J;
  Standard_Real Temp;
  for(Standard_Integer Index = LowerIndex; 
//      Index <= LowerIndex + (Length()) >> 1 ; Index++) {
      Index <= (LowerIndex + Length()) >> 1 ; Index++) {
    J = UpperIndex + LowerIndex - Index;
    Temp = Array(Index);
    Array(Index) = Array(J);
    Array(J) = Temp;
  }
}

math_Vector math_Vector::Inverse() const {
  math_Vector Result = *this;
  Result.Invert();
  return Result;
}

math_Vector math_Vector::Multiplied(const Standard_Real Right) const{

  math_Vector Result (LowerIndex, UpperIndex);

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result.Array(Index) = Array(Index) * Right;
  }
  return Result;
}

math_Vector math_Vector::TMultiplied(const Standard_Real Right) const{

  math_Vector Result (LowerIndex, UpperIndex);

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result.Array(Index) = Array(Index) * Right;
  }
  return Result;
}


void math_Vector::Multiply(const Standard_Real Right) {

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Array(Index) = Array(Index) * Right;
  }
}


void math_Vector::Divide(const Standard_Real Right) {

  Standard_DivideByZero_Raise_if(Abs(Right) <= RealEpsilon(), "");

  for(Standard_Integer Index =LowerIndex; Index <=UpperIndex; Index++) {
    Array(Index) = Array(Index) / Right;
  }
}


math_Vector math_Vector::Divided (const Standard_Real Right) const {
  
  Standard_DivideByZero_Raise_if(Abs(Right) <= RealEpsilon(), "");
  math_Vector temp = Multiplied(1./Right);
  return temp;
}


void math_Vector::Add(const math_Vector& Right) {

  Standard_DimensionError_Raise_if(Length() != Right.Length(), "");

  Standard_Integer I = Right.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Array(Index) = Array(Index) + Right.Array(I);
    I++;
  }
}    

math_Vector math_Vector::Added(const math_Vector& Right) const{

  Standard_DimensionError_Raise_if(Length() != Right.Length(), "");

  math_Vector Result(LowerIndex, UpperIndex);
  
  Standard_Integer I = Right.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result.Array(Index) = Array(Index) + Right.Array(I);
    I++;
  }
  return Result;
}    



void math_Vector::Subtract(const math_Vector& Right) {

  Standard_DimensionError_Raise_if(Length() != Right.Length(), "");

  Standard_Integer I = Right.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Array(Index) = Array(Index) - Right.Array(I);
    I++;
  }
}    


math_Vector math_Vector::Subtracted (const math_Vector& Right) const {

  Standard_DimensionError_Raise_if(Length() != Right.Length(), "");
  
  math_Vector Result(LowerIndex, UpperIndex);
  
  Standard_Integer I = Right.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result.Array(Index) = Array(Index) - Right.Array(I);
    I++;
  }
  return Result;
}    


math_Vector math_Vector::Slice(const Standard_Integer I1, 
			       const Standard_Integer I2) const 
{
  
  Standard_RangeError_Raise_if((I1 < LowerIndex) ||
			       (I1 > UpperIndex) ||
			       (I2 < LowerIndex) ||
			       (I2 > UpperIndex) , "");
  

  if(I2 >= I1) {
    math_Vector Result(I1, I2);
    for(Standard_Integer Index = I1; Index <= I2; Index++) {
      Result.Array(Index) = Array(Index);
    }        
    return Result;  
  }
  else {
    math_Vector Result(I2, I1);
    for(Standard_Integer Index = I1; Index >= I2; Index--) {
      Result.Array(Index) = Array(Index);
    }
    return Result;
  }   
}


void math_Vector::Add (const math_Vector& Left, const math_Vector& Right) {

  Standard_DimensionError_Raise_if((Length() != Right.Length()) ||
				   (Right.Length() != Left.Length()), "");

  
  Standard_Integer I = Left.LowerIndex;
  Standard_Integer J = Right.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Array(Index) = Left.Array(I) + Right.Array(J);
    I++;
    J++;
  }
}    

void math_Vector::Subtract (const math_Vector& Left, 
			    const math_Vector& Right) {

  Standard_DimensionError_Raise_if((Length() != Right.Length()) ||
				   (Right.Length() != Left.Length()), "");
  
  Standard_Integer I = Left.LowerIndex;
  Standard_Integer J = Right.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Array(Index) = Left.Array(I) - Right.Array(J);
    I++;
    J++;
  }
}    

void math_Vector::Multiply(const math_Matrix& Left, 
			   const math_Vector& Right) {

  Standard_DimensionError_Raise_if((Length() != Left.RowNumber()) ||
				   (Left.ColNumber() != Right.Length()),
				   "");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer I = Left.LowerRowIndex; I <= Left.UpperRowIndex; I++) {
    Array(Index) = 0.0;
    Standard_Integer K = Right.LowerIndex;
    for(Standard_Integer J = Left.LowerColIndex; J <= Left.UpperColIndex; J++) {
      Array(Index) = Array(Index) + Left.Array(I, J) * Right.Array(K);
      K++;
    }
    Index++;
  }
}    

void math_Vector::Multiply(const math_Vector& Left,
			   const math_Matrix& Right) {

  Standard_DimensionError_Raise_if((Length() != Right.ColNumber()) ||
				   (Left.Length() != Right.RowNumber()),
				   "");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer J = Right.LowerColIndex; J <= Right.UpperColIndex; J++) {
    Array(Index) = 0.0;
    Standard_Integer K = Left.LowerIndex;
    for(Standard_Integer I = Right.LowerRowIndex; I <= Right.UpperRowIndex; I++) {
      Array(Index) = Array(Index) + Left.Array(K) * Right.Array(I, J);
      K++;
    }
    Index++;
  }
}    

void math_Vector::TMultiply(const math_Matrix& TLeft,
			    const math_Vector&  Right) {

  Standard_DimensionError_Raise_if((Length() != TLeft.ColNumber()) ||
				   (TLeft.RowNumber() != Right.Length()),
				   "");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer I = TLeft.LowerColIndex; I <= TLeft.UpperColIndex; I++) {
    Array(Index) = 0.0;
    Standard_Integer K = Right.LowerIndex;
    for(Standard_Integer J = TLeft.LowerRowIndex; J <= TLeft.UpperRowIndex; J++) {
      Array(Index) = Array(Index) + TLeft.Array(J, I) * Right.Array(K);
      K++;
    }
    Index++;
  }
}    

void math_Vector::TMultiply(const math_Vector&  Left,
			    const math_Matrix& TRight) {

  Standard_DimensionError_Raise_if((Length() != TRight.RowNumber()) ||
				   (Left.Length() != TRight.ColNumber()),
				   "");

  Standard_Integer Index = LowerIndex;
  for(Standard_Integer J = TRight.LowerRowIndex; J <= TRight.UpperRowIndex; J++) {
    Array(Index) = 0.0;
    Standard_Integer K = Left.LowerIndex;
    for(Standard_Integer I = TRight.LowerColIndex; 
	I <= TRight.UpperColIndex; I++) {
      Array(Index) = Array(Index) + Left.Array(K) * TRight.Array(J, I);
      K++;
    }
    Index++;
  }
}    




Standard_Real math_Vector::Multiplied(const math_Vector& Right) const{
  Standard_Real Result = 0;

  Standard_DimensionError_Raise_if(Length() != Right.Length(), "");

  Standard_Integer I = Right.LowerIndex;
  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result = Result + Array(Index) * Right.Array(I);
    I++;
  }
  return Result;
}    

math_Vector math_Vector::Opposite() {
  math_Vector Result(LowerIndex, UpperIndex);

  for(Standard_Integer Index = LowerIndex; Index <= UpperIndex; Index++) {
    Result.Array(Index) = - Array(Index);
  }
  return Result;
}    

math_Vector math_Vector::Multiplied(const math_Matrix& Right)const {    
  Standard_DimensionError_Raise_if(Length() != Right.RowNumber(), "");

  math_Vector Result(Right.LowerColIndex, Right.UpperColIndex);
  for(Standard_Integer J2 = Right.LowerColIndex; 
      J2 <= Right.UpperColIndex; J2++) {
    Array(J2) = 0.0;
    Standard_Integer I2 = Right.LowerRowIndex;
    for(Standard_Integer I = LowerIndex; I <= UpperIndex; I++) {
      Result.Array(J2) = Result.Array(J2) + Array(I) * 
	Right.Array(I2, J2);
      I2++;
    }
  }
  return Result;
}    


void math_Vector::Multiply(const Standard_Real Left,
			   const math_Vector& Right) 
{
  Standard_DimensionError_Raise_if((Length() != Right.Length()),
				   "");
  for(Standard_Integer I = LowerIndex; I <= UpperIndex; I++) {
    Array(I) = Left * Right.Array(I);
  }
}


math_Vector& math_Vector::Initialized(const math_Vector& Other) {

  Standard_DimensionError_Raise_if(Length() != Other.Length(), "");
  
  (Other.Array).Copy(Array);
  return *this;
}



void math_Vector::Dump(Standard_OStream& o) const
{
  o << "math_Vector of Length = " << Length() << "\n";
  for(Standard_Integer Index = LowerIndex; 
      Index <= UpperIndex; Index++) {
    o << "math_Vector(" << Index << ") = " << Array(Index) << "\n";
  }
}
