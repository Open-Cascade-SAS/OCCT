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

#ifndef _FEmTool_SparseMatrix_HeaderFile
#define _FEmTool_SparseMatrix_HeaderFile

#include <Standard.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>


class FEmTool_SparseMatrix;
DEFINE_STANDARD_HANDLE(FEmTool_SparseMatrix, Standard_Transient)

//! Sparse Matrix definition
class FEmTool_SparseMatrix : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual void Init (const Standard_Real Value) = 0;
  
  Standard_EXPORT virtual Standard_Real& ChangeValue (const Standard_Integer I, const Standard_Integer J) = 0;
  
  //! To make a Factorization of <me>
  Standard_EXPORT virtual Standard_Boolean Decompose() = 0;
  
  //! Direct Solve of AX = B
  Standard_EXPORT virtual void Solve (const math_Vector& B, math_Vector& X) const = 0;
  
  //! Make Preparation to iterative solve
  Standard_EXPORT virtual Standard_Boolean Prepare() = 0;
  
  //! Iterative solve  of AX = B
  Standard_EXPORT virtual void Solve (const math_Vector& B, const math_Vector& Init, math_Vector& X, math_Vector& Residual, const Standard_Real Tolerance = 1.0e-8, const Standard_Integer NbIterations = 50) const = 0;
  
  //! returns the product of a SparseMatrix by a vector.
  //! An exception is raised if the dimensions are different
  Standard_EXPORT virtual void Multiplied (const math_Vector& X, math_Vector& MX) const = 0;
  
  //! returns the row range of a matrix.
  Standard_EXPORT virtual Standard_Integer RowNumber() const = 0;
  
  //! returns the column range of the matrix.
  Standard_EXPORT virtual Standard_Integer ColNumber() const = 0;




  DEFINE_STANDARD_RTTIEXT(FEmTool_SparseMatrix,Standard_Transient)

protected:




private:




};







#endif // _FEmTool_SparseMatrix_HeaderFile
