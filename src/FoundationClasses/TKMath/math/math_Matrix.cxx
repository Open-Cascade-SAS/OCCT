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

#include <math_Gauss.hxx>
#include <math_NotSquare.hxx>
#include <math_SingularMatrix.hxx>

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

void math_Matrix::Transpose()
{
  math_NotSquare_Raise_if(RowNumber() != ColNumber(),
                          "math_Matrix::Transpose() - matrix is not square");

  const Standard_Integer aLowerRow = Array.LowerRow();
  const Standard_Integer anUpperRow = Array.UpperRow();
  const Standard_Integer aLowerCol = Array.LowerCol();
  const Standard_Integer anUpperCol = Array.UpperCol();

  Standard_Integer Row = aLowerRow;
  Standard_Integer Col = aLowerCol;
  SetLowerCol(aLowerRow);
  Standard_Real Temp;
  for (Standard_Integer I = aLowerRow; I <= anUpperRow; I++)
  {
    for (Standard_Integer J = I; J <= anUpperCol; J++)
    {
      Temp        = Array(I, J);
      Array(I, J) = Array(J, I);
      Array(J, I) = Temp;
    }
  }
  SetLowerRow(Col);
  SetLowerCol(Row);
}

void math_Matrix::Invert()
{
  math_NotSquare_Raise_if(RowNumber() != ColNumber(),
                          "math_Matrix::Transpose() - matrix is not square");

  math_Gauss Sol(*this);
  if (Sol.IsDone())
  {
    Sol.Invert(*this);
  }
  else
  {
    throw math_SingularMatrix(); // SingularMatrix Exception;
  }
}

math_Matrix math_Matrix::Inverse() const
{

  math_Matrix Result = *this;
  Result.Invert();
  return Result;
}

void math_Matrix::Dump(Standard_OStream& o) const

{
  const Standard_Integer aLowerRow = Array.LowerRow();
  const Standard_Integer anUpperRow = Array.UpperRow();
  const Standard_Integer aLowerCol = Array.LowerCol();
  const Standard_Integer anUpperCol = Array.UpperCol();

  o << "math_Matrix of RowNumber = " << RowNumber();
  o << " and ColNumber = " << ColNumber() << "\n";

  for (Standard_Integer I = aLowerRow; I <= anUpperRow; I++)
  {
    for (Standard_Integer J = aLowerCol; J <= anUpperCol; J++)
    {
      o << "math_Matrix ( " << I << ", " << J << " ) = ";
      o << Array(I, J) << "\n";
    }
  }
}
