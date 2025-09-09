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

#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>

//! This class finds eigen values and vectors of
//! real symmetric tridiagonal matrix
class math_EigenValuesSearcher
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT math_EigenValuesSearcher(const TColStd_Array1OfReal& Diagonal,
                                           const TColStd_Array1OfReal& Subdiagonal);

  //! Returns Standard_True if computation is performed
  //! successfully.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns the dimension of matrix
  Standard_EXPORT Standard_Integer Dimension() const;

  //! Returns the specified eigenvalue.
  //!
  //! @param[in] theIndex index of the desired eigenvalue (1-based indexing)
  //! @return the eigenvalue at the specified index
  //!
  //! @pre IsDone() == Standard_True
  //! @pre 1 <= theIndex <= Dimension()
  //!
  //! @note Eigenvalues are returned in the order they were computed by the algorithm,
  //!       which may not be sorted. Use sorting if ordered eigenvalues are needed.
  Standard_EXPORT Standard_Real EigenValue(const Standard_Integer theIndex) const;

  //! Returns the specified eigenvector.
  //!
  //! @param[in] theIndex index of the desired eigenvector (1-based indexing)
  //! @return the normalized eigenvector corresponding to EigenValue(theIndex)
  //!
  //! @pre IsDone() == Standard_True
  //! @pre 1 <= theIndex <= Dimension()
  //!
  //! @note The returned eigenvector is normalized and orthogonal to all other eigenvectors.
  //!       The eigenvector satisfies: A * v = λ * v, where A is the original matrix,
  //!       v is the eigenvector, and λ is the corresponding eigenvalue.
  Standard_EXPORT math_Vector EigenVector(const Standard_Integer theIndex) const;

private:
  //! @name Private data members

  Handle(TColStd_HArray1OfReal) myDiagonal;     //!< Copy of input diagonal elements
  Handle(TColStd_HArray1OfReal) mySubdiagonal;  //!< Copy of input subdiagonal elements
  Standard_Boolean              myIsDone;       //!< Computation success flag
  Standard_Integer              myN;            //!< Matrix dimension
  Handle(TColStd_HArray1OfReal) myEigenValues;  //!< Computed eigenvalues
  Handle(TColStd_HArray2OfReal) myEigenVectors; //!< Computed eigenvectors (column-wise)
};

#endif // _math_EigenValuesSearcher_HeaderFile
