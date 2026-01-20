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

#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <FEmTool_SparseMatrix.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>

//! Symmetric Sparse ProfileMatrix useful for 1D Finite
//! Element methods
class FEmTool_ProfileMatrix : public FEmTool_SparseMatrix
{

public:
  Standard_EXPORT FEmTool_ProfileMatrix(const NCollection_Array1<int>& FirstIndexes);

  Standard_EXPORT void Init(const double Value) override;

  Standard_EXPORT double& ChangeValue(const int I,
                                             const int J) override;

  //! To make a Factorization of <me>
  Standard_EXPORT bool Decompose() override;

  //! Direct Solve of AX = B
  Standard_EXPORT void Solve(const math_Vector& B, math_Vector& X) const override;

  //! Make Preparation to iterative solve
  Standard_EXPORT bool Prepare() override;

  //! Iterative solve of AX = B
  Standard_EXPORT void Solve(const math_Vector&     B,
                             const math_Vector&     Init,
                             math_Vector&           X,
                             math_Vector&           Residual,
                             const double    Tolerance    = 1.0e-8,
                             const int NbIterations = 50) const override;

  //! returns the product of a SparseMatrix by a vector.
  //! An exception is raised if the dimensions are different
  Standard_EXPORT void Multiplied(const math_Vector& X, math_Vector& MX) const override;

  //! returns the row range of a matrix.
  Standard_EXPORT int RowNumber() const override;

  //! returns the column range of the matrix.
  Standard_EXPORT int ColNumber() const override;

  Standard_EXPORT bool IsInProfile(const int i,
                                               const int j) const;

  Standard_EXPORT void OutM() const;

  Standard_EXPORT void OutS() const;

  DEFINE_STANDARD_RTTIEXT(FEmTool_ProfileMatrix, FEmTool_SparseMatrix)

private:
  NCollection_Array2<int>          profile;
  occ::handle<NCollection_HArray1<double>>    ProfileMatrix;
  occ::handle<NCollection_HArray1<double>>    SMatrix;
  occ::handle<NCollection_HArray1<int>> NextCoeff;
  bool                 IsDecomp;
};

#endif // _FEmTool_ProfileMatrix_HeaderFile
