// Created on: 1993-11-19
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



# include <MAT2d_BiInt.ixx>

MAT2d_BiInt::MAT2d_BiInt(const Standard_Integer I1,
			 const Standard_Integer I2)
     :i1(I1),i2(I2)
{
}


Standard_Integer  MAT2d_BiInt::FirstIndex()const 
{
  return i1;
}

Standard_Integer  MAT2d_BiInt::SecondIndex()const
{
  return i2;
}

void  MAT2d_BiInt::FirstIndex(const Standard_Integer I1)
{
  i1 = I1;
}

void  MAT2d_BiInt::SecondIndex(const Standard_Integer I2)
{
  i2 = I2;
}

Standard_Boolean MAT2d_BiInt::IsEqual(const MAT2d_BiInt& B)
const 
{
  return (i1 == B.FirstIndex() && i2 == B.SecondIndex());
}
     
