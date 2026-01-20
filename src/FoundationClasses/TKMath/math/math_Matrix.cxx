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

#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <math_Gauss.hxx>
#include <math_NotSquare.hxx>
#include <math_SingularMatrix.hxx>
#include <Standard_DimensionError.hxx>

//==================================================================================================

void math_Matrix::SetRow(const int Row, const math_Vector& V)
{
  Standard_DimensionError_Raise_if(ColNumber() != V.Length(),
                                   "math_Matrix::SetRow() - input vector has wrong dimensions");

  const int aLowerCol  = Array.LowerCol();
  const int anUpperCol = Array.UpperCol();

  int I = V.Lower();
  for (int Index = aLowerCol; Index <= anUpperCol; Index++)
  {
    Array(Row, Index) = V(I);
    I++;
  }
}

//==================================================================================================

void math_Matrix::SetCol(const int Col, const math_Vector& V)
{
  Standard_DimensionError_Raise_if(RowNumber() != V.Length(),
                                   "math_Matrix::SetCol() - input vector has wrong dimensions");

  const int aLowerRow  = Array.LowerRow();
  const int anUpperRow = Array.UpperRow();

  int I = V.Lower();
  for (int Index = aLowerRow; Index <= anUpperRow; Index++)
  {
    Array(Index, Col) = V(I);
    I++;
  }
}

//==================================================================================================

math_Vector math_Matrix::Row(const int Row) const
{
  const int aLowerCol  = Array.LowerCol();
  const int anUpperCol = Array.UpperCol();

  math_Vector Result(aLowerCol, anUpperCol);

  for (int Index = aLowerCol; Index <= anUpperCol; Index++)
  {
    Result(Index) = Array(Row, Index);
  }
  return Result;
}

//==================================================================================================

math_Vector math_Matrix::Col(const int Col) const
{
  const int aLowerRow  = Array.LowerRow();
  const int anUpperRow = Array.UpperRow();

  math_Vector Result(aLowerRow, anUpperRow);

  for (int Index = aLowerRow; Index <= anUpperRow; Index++)
  {
    Result(Index) = Array(Index, Col);
  }
  return Result;
}

//==================================================================================================

void math_Matrix::SwapRow(const int Row1, const int Row2)
{
  math_Vector V1 = Row(Row1);
  math_Vector V2 = Row(Row2);
  SetRow(Row1, V2);
  SetRow(Row2, V1);
}

//==================================================================================================

void math_Matrix::SwapCol(const int Col1, const int Col2)
{
  math_Vector V1 = Col(Col1);
  math_Vector V2 = Col(Col2);
  SetCol(Col1, V2);
  SetCol(Col2, V1);
}

//==================================================================================================

void math_Matrix::Multiply(const math_Vector& Left, const math_Vector& Right)
{
  Standard_DimensionError_Raise_if(
    (RowNumber() != Left.Length()) || (ColNumber() != Right.Length()),
    "math_Matrix::Multiply() - input vectors have incompatible dimensions");

  const int aLowerRow  = Array.LowerRow();
  const int anUpperRow = Array.UpperRow();
  const int aLowerCol  = Array.LowerCol();
  const int anUpperCol = Array.UpperCol();

  for (int I = aLowerRow; I <= anUpperRow; I++)
  {
    for (int J = aLowerCol; J <= anUpperCol; J++)
    {
      Array(I, J) = Left(I) * Right(J);
    }
  }
}

//==================================================================================================

math_Vector math_Matrix::Multiplied(const math_Vector& Right) const
{
  Standard_DimensionError_Raise_if(
    ColNumber() != Right.Length(),
    "math_Matrix::Multiplied() - input vector has incompatible dimensions");

  const int aLowerRow  = Array.LowerRow();
  const int anUpperRow = Array.UpperRow();
  const int aLowerCol  = Array.LowerCol();
  const int anUpperCol = Array.UpperCol();

  math_Vector Result(aLowerRow, anUpperRow);

  for (int I = aLowerRow; I <= anUpperRow; I++)
  {
    Result(I)           = 0.0;
    int II = Right.Lower();
    for (int J = aLowerCol; J <= anUpperCol; J++)
    {
      Result(I) = Result(I) + Array(I, J) * Right(II);
      II++;
    }
  }
  return Result;
}

//==================================================================================================

math_Vector math_Matrix::operator*(const math_Vector& Right) const
{
  return Multiplied(Right);
}

//==================================================================================================

double math_Matrix::Determinant() const
{
  math_Gauss Sol(*this);

  if (Sol.IsDone())
  {
    return Sol.Determinant();
  }
  else
  {
    return 0.0;
  }
}

//==================================================================================================

void math_Matrix::Invert()
{
  math_NotSquare_Raise_if(RowNumber() != ColNumber(),
                          "math_Matrix::Invert() - matrix is not square");

  math_Gauss Sol(*this);
  if (Sol.IsDone())
  {
    Sol.Invert(*this);
  }
  else
  {
    throw math_SingularMatrix();
  }
}

//==================================================================================================

math_Matrix math_Matrix::Inverse() const
{
  math_Matrix Result = *this;
  Result.Invert();
  return Result;
}
