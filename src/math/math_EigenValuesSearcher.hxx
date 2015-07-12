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
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <math_Vector.hxx>
class StdFail_NotDone;


//! This class finds eigen values and vectors of
//! real symmetric tridiagonal matrix
class math_EigenValuesSearcher 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT math_EigenValuesSearcher(const TColStd_Array1OfReal& Diagonal, const TColStd_Array1OfReal& Subdiagonal);
  
  //! Returns Standard_True if computation is performed
  //! successfully.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the dimension of matrix
  Standard_EXPORT Standard_Integer Dimension() const;
  
  //! Returns the Index_th eigen value of matrix
  //! Index must be in [1, Dimension()]
  Standard_EXPORT Standard_Real EigenValue (const Standard_Integer Index) const;
  
  //! Returns the Index_th eigen vector of matrix
  //! Index must be in [1, Dimension()]
  Standard_EXPORT math_Vector EigenVector (const Standard_Integer Index) const;




protected:





private:



  Handle(TColStd_HArray1OfReal) myDiagonal;
  Handle(TColStd_HArray1OfReal) mySubdiagonal;
  Standard_Boolean myIsDone;
  Standard_Integer myN;
  Handle(TColStd_HArray1OfReal) myEigenValues;
  Handle(TColStd_HArray2OfReal) myEigenVectors;


};







#endif // _math_EigenValuesSearcher_HeaderFile
