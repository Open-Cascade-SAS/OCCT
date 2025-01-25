// Created on: 1997-10-29
// Created by: Roman BORISOV
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

#ifndef _FEmTool_ProfileMatrix_HeaderFile
#define _FEmTool_ProfileMatrix_HeaderFile

#include <Standard.hxx>

#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <FEmTool_SparseMatrix.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>

class FEmTool_ProfileMatrix;
DEFINE_STANDARD_HANDLE(FEmTool_ProfileMatrix, FEmTool_SparseMatrix)

//! Symmetric Sparse ProfileMatrix useful  for 1D Finite
//! Element methods
class FEmTool_ProfileMatrix : public FEmTool_SparseMatrix
{

public:
  Standard_EXPORT FEmTool_ProfileMatrix(const TColStd_Array1OfInteger& FirstIndexes);

  Standard_EXPORT void Init(const Standard_Real Value) Standard_OVERRIDE;

  Standard_EXPORT Standard_Real& ChangeValue(const Standard_Integer I,
                                             const Standard_Integer J) Standard_OVERRIDE;

  //! To make a Factorization of <me>
  Standard_EXPORT Standard_Boolean Decompose() Standard_OVERRIDE;

  //! Direct Solve of AX = B
  Standard_EXPORT void Solve(const math_Vector& B, math_Vector& X) const Standard_OVERRIDE;

  //! Make Preparation to iterative solve
  Standard_EXPORT Standard_Boolean Prepare() Standard_OVERRIDE;

  //! Iterative solve  of AX = B
  Standard_EXPORT void Solve(const math_Vector&     B,
                             const math_Vector&     Init,
                             math_Vector&           X,
                             math_Vector&           Residual,
                             const Standard_Real    Tolerance    = 1.0e-8,
                             const Standard_Integer NbIterations = 50) const Standard_OVERRIDE;

  //! returns the product of a SparseMatrix by a vector.
  //! An exception is raised if the dimensions are different
  Standard_EXPORT void Multiplied(const math_Vector& X, math_Vector& MX) const Standard_OVERRIDE;

  //! returns the row range of a matrix.
  Standard_EXPORT Standard_Integer RowNumber() const Standard_OVERRIDE;

  //! returns the column range of the matrix.
  Standard_EXPORT Standard_Integer ColNumber() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsInProfile(const Standard_Integer i,
                                               const Standard_Integer j) const;

  Standard_EXPORT void OutM() const;

  Standard_EXPORT void OutS() const;

  DEFINE_STANDARD_RTTIEXT(FEmTool_ProfileMatrix, FEmTool_SparseMatrix)

protected:
private:
  TColStd_Array2OfInteger          profile;
  Handle(TColStd_HArray1OfReal)    ProfileMatrix;
  Handle(TColStd_HArray1OfReal)    SMatrix;
  Handle(TColStd_HArray1OfInteger) NextCoeff;
  Standard_Boolean                 IsDecomp;
};

#endif // _FEmTool_ProfileMatrix_HeaderFile
