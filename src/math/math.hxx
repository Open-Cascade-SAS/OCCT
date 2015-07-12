// Created on: 1991-01-21
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _math_HeaderFile
#define _math_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
#include <Standard_Boolean.hxx>
class math_Matrix;
class math_Function;
class math_FunctionWithDerivative;
class math_MultipleVarFunction;
class math_MultipleVarFunctionWithGradient;
class math_MultipleVarFunctionWithHessian;
class math_FunctionSet;
class math_FunctionSetWithDerivatives;
class math_Gauss;
class math_GaussLeastSquare;
class math_SVD;
class math_DirectPolynomialRoots;
class math_FunctionRoots;
class math_BissecNewton;
class math_FunctionRoot;
class math_NewtonFunctionRoot;
class math_BracketedRoot;
class math_FunctionSetRoot;
class math_NewtonFunctionSetRoot;
class math_BracketMinimum;
class math_BrentMinimum;
class math_Powell;
class math_FRPR;
class math_BFGS;
class math_NewtonMinimum;
class math_Jacobi;
class math_GaussSingleIntegration;
class math_GaussMultipleIntegration;
class math_GaussSetIntegration;
class math_FunctionSample;
class math_FunctionAllRoots;
class math_Householder;
class math_Crout;
class math_Uzawa;
class math_TrigonometricFunctionRoots;
class math_KronrodSingleIntegration;
class math_EigenValuesSearcher;
class math_ComputeGaussPointsAndWeights;
class math_ComputeKronrodPointsAndWeights;
class math_DoubleTab;



class math 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Integer GaussPointsMax();
  
  Standard_EXPORT static void GaussPoints (const Standard_Integer Index, math_Vector& Points);
  
  Standard_EXPORT static void GaussWeights (const Standard_Integer Index, math_Vector& Weights);
  
  //! Returns the maximal number of points for that the values
  //! are stored in the table. If the number is greater then
  //! KronrodPointsMax, the points will be computed.
  Standard_EXPORT static Standard_Integer KronrodPointsMax();
  
  //! Returns a vector of Gauss points and a vector of their weights.
  //! The difference with the
  //! method GaussPoints is the following:
  //! - the points are returned in increasing order.
  //! - if Index is greater then GaussPointsMax, the points are
  //! computed.
  //! Returns Standard_True if Index is positive, Points' and Weights'
  //! length is equal to Index, Points and Weights are successfully computed.
  Standard_EXPORT static Standard_Boolean OrderedGaussPointsAndWeights (const Standard_Integer Index, math_Vector& Points, math_Vector& Weights);
  
  //! Returns a vector of Kronrod points and a vector of their
  //! weights for Gauss-Kronrod computation method.
  //! Index should be odd and greater then or equal to 3,
  //! as the number of Kronrod points is equal to 2*N + 1,
  //! where N is a number of Gauss points. Points and Weights should
  //! have the size equal to Index. Each even element of Points
  //! represents a Gauss point value of N-th Gauss quadrature.
  //! The values from Index equal to 3 to 123 are stored in a
  //! table (see the file math_Kronrod.cxx). If Index is greater,
  //! then points and weights will be computed. Returns Standard_True
  //! if Index is odd, it is equal to the size of Points and Weights
  //! and the computation of Points and Weights is performed successfully.
  //! Otherwise this method returns Standard_False.
  Standard_EXPORT static Standard_Boolean KronrodPointsAndWeights (const Standard_Integer Index, math_Vector& Points, math_Vector& Weights);




protected:





private:




friend class math_Matrix;
friend class math_Function;
friend class math_FunctionWithDerivative;
friend class math_MultipleVarFunction;
friend class math_MultipleVarFunctionWithGradient;
friend class math_MultipleVarFunctionWithHessian;
friend class math_FunctionSet;
friend class math_FunctionSetWithDerivatives;
friend class math_Gauss;
friend class math_GaussLeastSquare;
friend class math_SVD;
friend class math_DirectPolynomialRoots;
friend class math_FunctionRoots;
friend class math_BissecNewton;
friend class math_FunctionRoot;
friend class math_NewtonFunctionRoot;
friend class math_BracketedRoot;
friend class math_FunctionSetRoot;
friend class math_NewtonFunctionSetRoot;
friend class math_BracketMinimum;
friend class math_BrentMinimum;
friend class math_Powell;
friend class math_FRPR;
friend class math_BFGS;
friend class math_NewtonMinimum;
friend class math_Jacobi;
friend class math_GaussSingleIntegration;
friend class math_GaussMultipleIntegration;
friend class math_GaussSetIntegration;
friend class math_FunctionSample;
friend class math_FunctionAllRoots;
friend class math_Householder;
friend class math_Crout;
friend class math_Uzawa;
friend class math_TrigonometricFunctionRoots;
friend class math_KronrodSingleIntegration;
friend class math_EigenValuesSearcher;
friend class math_ComputeGaussPointsAndWeights;
friend class math_ComputeKronrodPointsAndWeights;
friend class math_DoubleTab;

};







#endif // _math_HeaderFile
