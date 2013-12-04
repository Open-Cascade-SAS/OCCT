// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_IntegerVector.ixx>

#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>



math_IntegerVector::math_IntegerVector(const Standard_Integer First,
				       const Standard_Integer Last): 
				       FirstIndex(First),
				       LastIndex(Last),
                                       Array(First, Last) {

  Standard_RangeError_Raise_if(First > Last, " ");
}

math_IntegerVector::math_IntegerVector(const Standard_Integer First,
				       const Standard_Integer Last,
				       const Standard_Integer InitialValue): 
				       FirstIndex(First),
				       LastIndex(Last),
                                       Array(First, Last) {

  Standard_RangeError_Raise_if(First > Last, " ");
  Array.Init(InitialValue);
}


math_IntegerVector::math_IntegerVector(const Standard_Address Tab,
				       const Standard_Integer First,
				       const Standard_Integer Last) :
				       FirstIndex(First),
				       LastIndex(Last),
                                       Array(*((const Standard_Integer *)Tab), 
					     First, Last)
{
  Standard_RangeError_Raise_if(First > Last, " ");
}


void math_IntegerVector::Init(const Standard_Integer InitialValue)
{
  Array.Init(InitialValue);
}


math_IntegerVector::math_IntegerVector(const math_IntegerVector& Other):
 
                                       FirstIndex(Other.FirstIndex),
                                       LastIndex(Other.LastIndex),
                                       Array(Other.Array) {}



void math_IntegerVector::SetFirst(const Standard_Integer First) {
  
  Array.SetLower(First);
  LastIndex = LastIndex - FirstIndex + First;
  FirstIndex = First;
}


Standard_Real math_IntegerVector::Norm() const {
  Standard_Real Result = 0;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Result = Result + Array(Index) * Array(Index);
  }
  return Sqrt(Result);
}

     
Standard_Real math_IntegerVector::Norm2() const {
  Standard_Real Result = 0;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Result = Result + Array(Index) * Array(Index);
  }
  return Result;
}


Standard_Integer math_IntegerVector::Max() const {
  Standard_Integer I=0;
  Standard_Real X = RealFirst();
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    if(Array(Index) > X) {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}


Standard_Integer math_IntegerVector::Min() const {
  Standard_Integer I=0;
  Standard_Real X = RealLast();
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    if(Array(Index) < X) {
      X = Array(Index);
      I = Index;
    }
  }
  return I;
}


void math_IntegerVector::Invert() {
  
  Standard_Integer J;
  Standard_Integer Temp;
  
  for(Standard_Integer Index = FirstIndex; 
      Index <= FirstIndex + Length() / 2 ; Index++) {
    J = LastIndex + FirstIndex - Index;
    Temp = Array(Index);
    Array(Index) = Array(J);
    Array(J) = Temp;
  }
}


math_IntegerVector math_IntegerVector::Inverse() const {
  
  math_IntegerVector Result = *this;
  Result.Invert();
  return Result;
}


void math_IntegerVector::Set(const Standard_Integer I1, 
			     const Standard_Integer I2, 
			     const math_IntegerVector &V) {
  
  Standard_DimensionError_Raise_if((I1 < FirstIndex) || 
				   (I2 > LastIndex) || 
				   (I1 > I2)         ||
				   (I2 - I1 + 1 != V.Length()), " ");
  
  Standard_Integer I = V.Lower();
  for(Standard_Integer Index = I1; Index <= I2; Index++) {
    Array(Index) = V.Array(I);
    I++;
  }
}


void math_IntegerVector::Multiply(const Standard_Integer Right) {
  
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Array(Index) = Array(Index) * Right;
  }
}


void math_IntegerVector::Add(const math_IntegerVector& Right) {
  
  Standard_DimensionError_Raise_if(Length() != Right.Length(), " ");

  Standard_Integer I = Right.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Array(Index) = Array(Index) + Right.Array(I);
    I++;
  }
}    


void math_IntegerVector::Subtract(const math_IntegerVector& Right) {
  
  Standard_DimensionError_Raise_if(Length() != Right.Length(), " ");
  Standard_Integer I = Right.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Array(Index) = Array(Index) - Right.Array(I);
    I++;
  }
}    


math_IntegerVector math_IntegerVector::Slice(const Standard_Integer I1, 
					     const Standard_Integer I2) const {
					       
  Standard_DimensionError_Raise_if((I1 < FirstIndex) || (I1 > LastIndex) ||
				   (I2 < FirstIndex) || (I2 > LastIndex),
				   " ");

  if(I2 >= I1) {
    math_IntegerVector Result(I1, I2);
    for(Standard_Integer Index = I1; Index <= I2; Index++) {
      Result.Array(Index) = Array(Index);
    }        
    return Result;  
  }
  else {
    math_IntegerVector Result(I2, I1);
    for(Standard_Integer Index = I1; Index >= I2; Index--) {
      Result.Array(Index) = Array(Index);
    }
    return Result;
  }   
}

Standard_Integer math_IntegerVector::Multiplied (const math_IntegerVector& Right) const {
  
  Standard_Integer Result = 0;
  
  Standard_DimensionError_Raise_if(Length() != Right.Length(), " ");

  Standard_Integer I = Right.FirstIndex;
  for(Standard_Integer Index = 0; Index < Length(); Index++) {
    Result = Result + Array(Index) * Right.Array(I);
    I++;
  }
  return Result;
}    


math_IntegerVector math_IntegerVector::Multiplied (const Standard_Integer Right)const  {
       
  math_IntegerVector Result(FirstIndex, LastIndex);
  
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Result.Array(Index) = Array(Index) * Right;
  }
  return Result;
}    
     
math_IntegerVector math_IntegerVector::TMultiplied (const Standard_Integer Right)const  {
       
  math_IntegerVector Result(FirstIndex, LastIndex);
  
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Result.Array(Index) = Array(Index) * Right;
  }
  return Result;
}    
     

math_IntegerVector math_IntegerVector::Added (const math_IntegerVector& Right) const {
       
  Standard_DimensionError_Raise_if(Length() != Right.Length(), " ");
  
  math_IntegerVector Result(FirstIndex, LastIndex);
  
  Standard_Integer I = Right.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Result.Array(Index) = Array(Index) + Right.Array(I);
    I++;
  }
  return Result;
}    


     
math_IntegerVector math_IntegerVector::Opposite() {
   
  math_IntegerVector Result(FirstIndex, LastIndex);
  
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Result.Array(Index) = - Array(Index);
  }
  return Result;
}    
     

math_IntegerVector math_IntegerVector::Subtracted (const math_IntegerVector& Right) const {
  
  Standard_DimensionError_Raise_if(Length() != Right.Length(), " ");
  
  math_IntegerVector Result(FirstIndex, LastIndex);
  
  Standard_Integer I = Right.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Result.Array(Index) = Array(Index) - Right.Array(I);
    I++;
  }
  return Result;
}    
     
void math_IntegerVector::Add (const math_IntegerVector& Left, 
			      const math_IntegerVector& Right) {
  
  Standard_DimensionError_Raise_if((Length() != Right.Length()) ||
				   (Right.Length() != Left.Length()), " ");

  Standard_Integer I = Left.FirstIndex;
  Standard_Integer J = Right.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Array(Index) = Left.Array(I) + Right.Array(J);
    I++;
    J++;
  }
}    


void math_IntegerVector::Subtract (const math_IntegerVector& Left, 
				    const math_IntegerVector& Right) {
  
  Standard_DimensionError_Raise_if((Length() != Right.Length()) ||
				   (Right.Length() != Left.Length()), " ");
  
  Standard_Integer I = Left.FirstIndex;
  Standard_Integer J = Right.FirstIndex;
  for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
    Array(Index) = Left.Array(I) - Right.Array(J);
    I++;
    J++;
  }
}    


void math_IntegerVector::Multiply(const Standard_Integer Left,
				  const math_IntegerVector& Right) 
{
  Standard_DimensionError_Raise_if((Length() != Right.Length()),
				   " ");
  for(Standard_Integer I = FirstIndex; I <= LastIndex; I++) {
    Array(I) = Left * Right.Array(I);
  }
}


math_IntegerVector& math_IntegerVector::Initialized (const math_IntegerVector& Other) {

  Standard_DimensionError_Raise_if(Length() != Other.Length(), " ");

  (Other.Array).Copy(Array);
  return *this;
}



void math_IntegerVector::Dump(Standard_OStream& o) const
{
   o << "math_IntegerVector of Range = " << Length() << "\n";
   for(Standard_Integer Index = FirstIndex; Index <= LastIndex; Index++) {
     o << "math_IntegerVector(" << Index << ") = " << Array(Index) << "\n";
   }
}






