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

void math_Matrix::SetRow(const Standard_Integer Row, const math_Vector& V)
{
  Standard_DimensionError_Raise_if(ColNumber() != V.Length(),
                                   "math_Matrix::SetRow() - input vector has wrong dimensions");

  const Standard_Integer aLowerCol  = Array.LowerCol();
  const Standard_Integer anUpperCol = Array.UpperCol();

  Standard_Integer I = V.Lower();
  for (Standard_Integer Index = aLowerCol; Index <= anUpperCol; Index++)
  {
    Array(Row, Index) = V(I);
    I++;
  }
}

//==================================================================================================

void math_Matrix::SetCol(const Standard_Integer Col, const math_Vector& V)
{
  Standard_DimensionError_Raise_if(RowNumber() != V.Length(),
                                   "math_Matrix::SetCol() - input vector has wrong dimensions");

  const Standard_Integer aLowerRow  = Array.LowerRow();
  const Standard_Integer anUpperRow = Array.UpperRow();

  Standard_Integer I = V.Lower();
  for (Standard_Integer Index = aLowerRow; Index <= anUpperRow; Index++)
  {
    Array(Index, Col) = V(I);
    I++;
  }
}

//==================================================================================================

math_Vector math_Matrix::Row(const Standard_Integer Row) const
{
  const Standard_Integer aLowerCol  = Array.LowerCol();
  const Standard_Integer anUpperCol = Array.UpperCol();

  math_Vector Result(aLowerCol, anUpperCol);

  for (Standard_Integer Index = aLowerCol; Index <= anUpperCol; Index++)
  {
    Result(Index) = Array(Row, Index);
  }
  return Result;
}

//==================================================================================================

math_Vector math_Matrix::Col(const Standard_Integer Col) const
{
  const Standard_Integer aLowerRow  = Array.LowerRow();
  const Standard_Integer anUpperRow = Array.UpperRow();

  math_Vector Result(aLowerRow, anUpperRow);

  for (Standard_Integer Index = aLowerRow; Index <= anUpperRow; Index++)
  {
    Result(Index) = Array(Index, Col);
  }
  return Result;
}

//==================================================================================================

void math_Matrix::SwapRow(const Standard_Integer Row1, const Standard_Integer Row2)
{
  math_Vector V1 = Row(Row1);
  math_Vector V2 = Row(Row2);
  SetRow(Row1, V2);
  SetRow(Row2, V1);
}

//==================================================================================================

void math_Matrix::SwapCol(const Standard_Integer Col1, const Standard_Integer Col2)
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

  const Standard_Integer aLowerRow  = Array.LowerRow();
  const Standard_Integer anUpperRow = Array.UpperRow();
  const Standard_Integer aLowerCol  = Array.LowerCol();
  const Standard_Integer anUpperCol = Array.UpperCol();

  for (Standard_Integer I = aLowerRow; I <= anUpperRow; I++)
  {
    for (Standard_Integer J = aLowerCol; J <= anUpperCol; J++)
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

  const Standard_Integer aLowerRow  = Array.LowerRow();
  const Standard_Integer anUpperRow = Array.UpperRow();
  const Standard_Integer aLowerCol  = Array.LowerCol();
  const Standard_Integer anUpperCol = Array.UpperCol();

  math_Vector Result(aLowerRow, anUpperRow);

  for (Standard_Integer I = aLowerRow; I <= anUpperRow; I++)
  {
    Result(I)           = 0.0;
    Standard_Integer II = Right.Lower();
    for (Standard_Integer J = aLowerCol; J <= anUpperCol; J++)
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

Standard_Real math_Matrix::Determinant() const
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
