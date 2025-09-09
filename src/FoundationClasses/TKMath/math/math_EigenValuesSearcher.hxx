// Created on: 2005-12-15
// Created by: Julia GERASIMOVA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _math_EigenValuesSearcher_HeaderFile
#define _math_EigenValuesSearcher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

//! This class finds eigenvalues and eigenvectors of real symmetric tridiagonal matrices.
//!
//! The implementation uses the QR algorithm with implicit shifts for numerical stability.
//! All computed eigenvalues are real (since the matrix is symmetric), and eigenvectors
//! are orthonormal. The class handles the complete eigendecomposition:
//! A * V = V * D, where A is the input matrix, V contains eigenvectors as columns,
//! and D is diagonal with eigenvalues.
//!
//! Key features:
//! - Robust QR algorithm implementation
//! - Numerical stability through implicit shifts
//! - Complete eigenvalue/eigenvector computation
//! - Proper handling of degenerate cases
class math_EigenValuesSearcher
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT math_EigenValuesSearcher(const TColStd_Array1OfReal& theDiagonal,
                                           const TColStd_Array1OfReal& theSubdiagonal);

  //! Returns Standard_True if computation is performed successfully.
  //! Computation may fail due to numerical issues or invalid input.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the dimension of the tridiagonal matrix.
  Standard_EXPORT Standard_Integer Dimension() const;

  //! Returns the specified eigenvalue.
  //! Eigenvalues are returned in the order they were computed by the algorithm,
  //! which may not be sorted. Use sorting if ordered eigenvalues are needed.
  //!
  //! @param theIndex index of the desired eigenvalue (1-based indexing)
  //! @return the eigenvalue at the specified index
  Standard_EXPORT Standard_Real EigenValue(const Standard_Integer theIndex) const;

  //! Returns the specified eigenvector.
  //! The returned eigenvector is normalized and orthogonal to all other eigenvectors.
  //! The eigenvector satisfies: A * v = lambda * v, where A is the original matrix,
  //! v is the eigenvector, and lambda is the corresponding eigenvalue.
  //!
  //! @param theIndex index of the desired eigenvector (1-based indexing)
  //! @return the normalized eigenvector corresponding to EigenValue(theIndex)
  Standard_EXPORT math_Vector EigenVector(const Standard_Integer theIndex) const;

private:
  NCollection_Array1<Standard_Real> myDiagonal;     //!< Copy of input diagonal elements
  NCollection_Array1<Standard_Real> mySubdiagonal;  //!< Copy of input subdiagonal elements
  Standard_Boolean                  myIsDone;       //!< Computation success flag
  Standard_Integer                  myN;            //!< Matrix dimension
  NCollection_Array1<Standard_Real> myEigenValues;  //!< Computed eigenvalues
  NCollection_Array2<Standard_Real> myEigenVectors; //!< Computed eigenvectors stored column-wise
};

#endif // _math_EigenValuesSearcher_HeaderFile
