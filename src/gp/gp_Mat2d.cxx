// Copyright (c) 1995-1999 Matra Datavision
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

//  10/09/97 : PMN : Correction BUC40192 (pb avec les matrices negatives)

#ifndef DEB
#define No_Standard_OutOfRange
#define No_Standard_ConstructionError
#endif

#include <gp_Mat2d.ixx>
#include <Standard_ConstructionError.hxx>

#define M00 ((Standard_Real*)M)[0]
#define M01 ((Standard_Real*)M)[1]
#define M10 ((Standard_Real*)M)[2]
#define M11 ((Standard_Real*)M)[3]

#define N00 ((Standard_Real*)N)[0]
#define N01 ((Standard_Real*)N)[1]
#define N10 ((Standard_Real*)N)[2]
#define N11 ((Standard_Real*)N)[3]

gp_Mat2d::gp_Mat2d (const gp_XY& Col1, const gp_XY& Col2)
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  M00 = Col1.X(); M10 = Col1.Y();
  M01 = Col2.X(); M11 = Col2.Y();
}

void gp_Mat2d::SetCol (const Standard_Integer Col,
		       const gp_XY& Value)
{
  Standard_OutOfRange_Raise_if (Col < 1 || Col > 2,"");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if  (Col == 1) {
    M00 = Value.X();
    M10 = Value.Y();
  }
  else {
    M01 = Value.X();
    M11 = Value.Y();
  }
}

void gp_Mat2d::SetCols (const gp_XY& Col1,
			const gp_XY& Col2)
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  M00 = Col1.X(); M10 = Col1.Y();
  M01 = Col2.X(); M11 = Col2.Y();
}

void gp_Mat2d::SetRow (const Standard_Integer Row, const gp_XY& Value)
{
  Standard_OutOfRange_Raise_if (Row < 1 || Row > 2,"");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if (Row == 1) {
    M00 = Value.X();
    M01 = Value.Y();
  }
  else {
    M10 = Value.X();
    M11 = Value.Y();
  }
}

void gp_Mat2d::SetRows (const gp_XY& Row1, const gp_XY& Row2)
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  M00 = Row1.X(); M01 = Row1.Y();
  M10 = Row2.X(); M11 = Row2.Y();
}

gp_XY gp_Mat2d::Column (const Standard_Integer Col) const
{
  Standard_OutOfRange_Raise_if (Col < 1 || Col > 2,"");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if (Col == 1) return gp_XY (M00,M10);
  return gp_XY (M01,M11);
}

gp_XY gp_Mat2d::Diagonal () const
{ 
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  return gp_XY (M00,M11);
}

gp_XY gp_Mat2d::Row (const Standard_Integer Row) const
{
  Standard_OutOfRange_Raise_if (Row < 1 || Row > 2,"");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if (Row == 1) return gp_XY (M00,M01);
  return gp_XY (M10,M11);
}

void gp_Mat2d::Invert ()
{
  Standard_Real new_matrix[2][2],
  det ;
  const Standard_Address N = (Standard_Address)&(new_matrix[0][0]);
  const Standard_Address M = (Standard_Address)&(    matrix[0][0]);
  N00 = M11 ;
  N01 = -M01 ;
  N10 = -M10 ;
  N11 = M00  ;
  det = N00 * N11 - N01 * N10 ;
  Standard_Real val = det;
  if (val < 0) val = - val;
  Standard_ConstructionError_Raise_if (val <= gp::Resolution(),"");
  det = 1.0 / det ;
  M00 = N00 * det ;
  M10 = N10 * det ;
  M01 = N01 * det ;
  M11 = N11 * det ;
}

void gp_Mat2d::Power (const Standard_Integer N)
{
  if      (N ==  1) { }
  else if (N ==  0) { SetIdentity (); }
  else if (N == -1) { Invert(); }
  else {
    if (N < 0) Invert();
    Standard_Integer Npower = N;
    if (Npower < 0) Npower = - Npower;
    Npower--;
    gp_Mat2d Temp = *this;
    while (1) {
      if (IsOdd(Npower)) Multiply (Temp);
      if (Npower == 1)   break;
      Temp.Multiply (Temp);
      Npower = Npower/2;
    }
  }
}

