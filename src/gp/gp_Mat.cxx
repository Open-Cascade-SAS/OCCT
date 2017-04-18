// Copyright (c) 1995-1999 Matra Datavision
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

//  10/09/97 : PMN : Correction BUC40192 (pb avec les matrices negatives)

#ifndef OCCT_DEBUG
#define No_Standard_OutOfRange
#define No_Standard_ConstructionError
#endif


#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

#define M00 ((Standard_Real*)M)[0]
#define M01 ((Standard_Real*)M)[1]
#define M02 ((Standard_Real*)M)[2]
#define M10 ((Standard_Real*)M)[3]
#define M11 ((Standard_Real*)M)[4]
#define M12 ((Standard_Real*)M)[5]
#define M20 ((Standard_Real*)M)[6]
#define M21 ((Standard_Real*)M)[7]
#define M22 ((Standard_Real*)M)[8]

#define N00 ((Standard_Real*)N)[0]
#define N01 ((Standard_Real*)N)[1]
#define N02 ((Standard_Real*)N)[2]
#define N10 ((Standard_Real*)N)[3]
#define N11 ((Standard_Real*)N)[4]
#define N12 ((Standard_Real*)N)[5]
#define N20 ((Standard_Real*)N)[6]
#define N21 ((Standard_Real*)N)[7]
#define N22 ((Standard_Real*)N)[8]

gp_Mat::gp_Mat (const gp_XYZ& Col1,
		const gp_XYZ& Col2,
		const gp_XYZ& Col3)
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  M00 = Col1.X(); M10 = Col1.Y(); M20 = Col1.Z();
  M01 = Col2.X(); M11 = Col2.Y(); M21 = Col2.Z();
  M02 = Col3.X(); M12 = Col3.Y(); M22 = Col3.Z();
}

void gp_Mat::SetCol (const Standard_Integer Col,
		     const gp_XYZ& Value) {

  Standard_OutOfRange_Raise_if (Col < 1 || Col > 3, " ");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if      (Col == 1) {
    M00 = Value.X(); M10 = Value.Y(); M20 = Value.Z();
  }
  else if (Col == 2) {
    M01 = Value.X(); M11 = Value.Y(); M21 = Value.Z();
  }
  else {
    M02 = Value.X(); M12 = Value.Y(); M22 = Value.Z();
  }
}

void gp_Mat::SetCols (const gp_XYZ& Col1,
		      const gp_XYZ& Col2,
		      const gp_XYZ& Col3)
{
  Mat00 = Col1.X(); Mat10 = Col1.Y(); Mat20 = Col1.Z();
  Mat01 = Col2.X(); Mat11 = Col2.Y(); Mat21 = Col2.Z();
  Mat02 = Col3.X(); Mat12 = Col3.Y(); Mat22 = Col3.Z();
}

void gp_Mat::SetCross (const gp_XYZ& Ref)
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  Standard_Real X = Ref.X();
  Standard_Real Y = Ref.Y();
  Standard_Real Z = Ref.Z();
  M00 = M11 = M22 = 0.0;
  M01 = - Z;  
  M02 = Y;
  M12 = - X;
  M10 = Z;
  M20 = - Y;
  M21 = X;
}

void gp_Mat::SetDot (const gp_XYZ& Ref)
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  Standard_Real X = Ref.X();
  Standard_Real Y = Ref.Y();
  Standard_Real Z = Ref.Z();
  M00 = X * X;
  M11 = Y * Y;
  M22 = Z * Z;
  M01 = X * Y;
  M02 = X * Z;
  M12 = Y * Z;
  M10 = M01;
  M20 = M02;
  M21 = M12;
}

void gp_Mat::SetRotation (const gp_XYZ& Axis,
			  const Standard_Real Ang)
{
  //    Rot = I + sin(Ang) * M + (1. - cos(Ang)) * M*M
  //    avec  M . XYZ = Axis ^ XYZ

//  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  gp_XYZ V = Axis.Normalized();
  SetCross (V);
  Multiply (sin(Ang));
  gp_Mat Temp;
  Temp.SetScale (1.0);
  Add (Temp);
  Standard_Real A = V.X();
  Standard_Real B = V.Y();
  Standard_Real C = V.Z();
  Temp.SetRow (1, gp_XYZ(- C*C - B*B,      A*B,           A*C     ));
  Temp.SetRow (2, gp_XYZ(     A*B,      -A*A - C*C,        B*C    ));
  Temp.SetRow (3, gp_XYZ(     A*C,          B*C,       - A*A - B*B));
  Temp.Multiply (1.0 - cos(Ang));
  Add (Temp);
}

void gp_Mat::SetRow (const Standard_Integer Row,
		     const gp_XYZ& Value)
{
  Standard_OutOfRange_Raise_if (Row < 1 || Row > 3, " ");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if      (Row == 1) {
    M00 = Value.X(); M01 = Value.Y(); M02 = Value.Z();
  }
  else if (Row == 2) {
    M10 = Value.X(); M11 = Value.Y(); M12 = Value.Z();
  }
  else {
    M20 = Value.X(); M21 = Value.Y(); M22 = Value.Z();
  }
}

void gp_Mat::SetRows (const gp_XYZ& Row1,
		      const gp_XYZ& Row2,
		      const gp_XYZ& Row3)
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  M00 = Row1.X(); M01 = Row1.Y(); M02 = Row1.Z();
  M10 = Row2.X(); M11 = Row2.Y(); M12 = Row2.Z();
  M20 = Row3.X(); M21 = Row3.Y(); M22 = Row3.Z();
}

gp_XYZ gp_Mat::Column (const Standard_Integer Col) const
{
  Standard_OutOfRange_Raise_if (Col < 1 || Col > 3, "gp_Mat::Column() - wrong index");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if (Col == 1) return gp_XYZ (M00,M10,M20);
  if (Col == 2) return gp_XYZ (M01,M11,M21);
  return gp_XYZ (M02,M12,M22);
}

gp_XYZ gp_Mat::Diagonal () const
{
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  return gp_XYZ (M00, M11, M22);
}

gp_XYZ gp_Mat::Row (const Standard_Integer Row) const
{
  Standard_OutOfRange_Raise_if (Row < 1 || Row > 3, "gp_Mat::Row() - wrong index");
  const Standard_Address M = (Standard_Address)&(matrix[0][0]);
  if (Row == 1) return gp_XYZ (M00,M01,M02);
  if (Row == 2) return gp_XYZ (M10,M11,M12);
  return gp_XYZ (M20,M21,M22);
}

void gp_Mat::Invert ()
{ 
  Standard_Real new_array[3][3] ;
  const Standard_Address M = (Standard_Address)&(   matrix[0][0]);
  const Standard_Address N = (Standard_Address)&(new_array[0][0]);  

  //
  // calcul de  la transposee de la commatrice
  //
  N00 = M11 * M22 - M12 * M21 ;
  N10 = -(M10 * M22 - M20 * M12) ;
  N20 = M10 * M21 - M20 * M11 ;
  N01 = - (M01 * M22 - M21 * M02) ;
  N11 = M00 * M22 - M20 * M02 ;
  N21 = -(M00 * M21 - M20 * M01) ;
  N02 = M01 * M12 - M11 * M02 ;
  N12 = -(M00 * M12 - M10 * M02) ;
  N22 = M00 * M11 - M01 * M10 ;
  Standard_Real det =  M00 * N00 + M01* N10 + M02 * N20 ;
  Standard_Real val = det;
  if (val < 0) val = - val;
  Standard_ConstructionError_Raise_if (val <= gp::Resolution(), "gp_Mat::Invert() - matrix has zero determinant");
  det = 1.0e0 / det ;
  M00 = N00;
  M10 = N10;
  M20 = N20;
  M01 = N01;
  M11 = N11;
  M21 = N21;
  M02 = N02;
  M12 = N12;
  M22 = N22;
  Multiply(det) ;
}

gp_Mat gp_Mat::Inverted () const
{ 
  gp_Mat NewMat;
  const Standard_Address M = (Standard_Address)&(       matrix[0][0]);
  const Standard_Address N = (Standard_Address)&(NewMat.matrix[0][0]);
  //
  // calcul de  la transposee de la commatrice
  //
  N00 = M11 * M22 - M12 * M21 ;
  N10 = -(M10 * M22 - M20 * M12) ;
  N20 = M10 * M21 - M20 * M11 ;
  N01 = - (M01 * M22 - M21 * M02) ;
  N11 = M00 * M22 - M20 * M02 ;
  N21 = -(M00 * M21 - M20 * M01) ;
  N02 = M01 * M12 - M11 * M02 ;
  N12 = -(M00 * M12 - M10 * M02) ;
  N22 = M00 * M11 - M01 * M10 ;
  Standard_Real det =  M00 * N00 + M01* N10 + M02 * N20 ;
  Standard_Real val = det;
  if (val < 0) val = - val;
  Standard_ConstructionError_Raise_if (val <= gp::Resolution(), "gp_Mat::Inverted() - matrix has zero determinant");
  det = 1.0e0 / det ;
  NewMat.Multiply(det) ;
  return NewMat;
}

void gp_Mat::Power (const Standard_Integer N)
{
  if (N == 1) { }
  else if (N == 0)  { SetIdentity() ; }
  else if (N == -1) { Invert(); }
  else {
    if (N < 0) { Invert(); }
    Standard_Integer Npower = N;
    if (Npower < 0) Npower = - Npower;
    Npower--;
    gp_Mat Temp = *this;
    for(;;) {
      if (IsOdd(Npower)) Multiply (Temp);
      if (Npower == 1)   break; 
      Temp.Multiply (Temp);
      Npower>>=1;
    }
  }
}

