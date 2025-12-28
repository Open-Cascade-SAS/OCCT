// Created on: 1997-09-11
// Created by: Philippe MANGIN
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

#ifndef _FEmTool_ElementaryCriterion_HeaderFile
#define _FEmTool_ElementaryCriterion_HeaderFile

#include <Standard.hxx>

#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
class math_Matrix;

//! defined J Criteria to used in minimisation
class FEmTool_ElementaryCriterion : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT FEmTool_ElementaryCriterion();

  //! Set the coefficient of the Element (the Curve)
  Standard_EXPORT void Set(const occ::handle<NCollection_HArray2<double>>& Coeff);

  //! Set the definition interval of the Element
  Standard_EXPORT virtual void Set(const double FirstKnot, const double LastKnot);

  //! To know if two dimension are independent.
  Standard_EXPORT virtual occ::handle<NCollection_HArray2<int>> DependenceTable() const = 0;

  //! To Compute J(E) where E is the current Element
  Standard_EXPORT virtual double Value() = 0;

  //! To Compute J(E) the coefficients of Hessian matrix of
  //! J(E) which are crossed derivatives in dimensions <Dim1>
  //! and <Dim2>.
  //! If DependenceTable(Dimension1,Dimension2) is False
  Standard_EXPORT virtual void Hessian(const int Dim1, const int Dim2, math_Matrix& H) = 0;

  //! To Compute the coefficients in the dimension <dim>
  //! of the J(E)'s Gradient where E is the current Element
  Standard_EXPORT virtual void Gradient(const int Dim, math_Vector& G) = 0;

  DEFINE_STANDARD_RTTIEXT(FEmTool_ElementaryCriterion, Standard_Transient)

protected:
  occ::handle<NCollection_HArray2<double>> myCoeff;
  double                                   myFirst;
  double                                   myLast;
};

#endif // _FEmTool_ElementaryCriterion_HeaderFile
