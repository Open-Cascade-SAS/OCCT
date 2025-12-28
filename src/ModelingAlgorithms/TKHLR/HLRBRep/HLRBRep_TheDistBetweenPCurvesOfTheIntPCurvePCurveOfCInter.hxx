// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter_HeaderFile
#define _HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRBRep_TypeDef.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
class HLRBRep_CurveTool;
class math_Matrix;

class HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter
    : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter(
    const HLRBRep_CurvePtr& curve1,
    const HLRBRep_CurvePtr& curve2);

  //! returns 2.
  Standard_EXPORT int NbVariables() const override;

  //! returns 2.
  Standard_EXPORT int NbEquations() const override;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D) override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override;

private:
  HLRBRep_CurvePtr thecurve1;
  HLRBRep_CurvePtr thecurve2;
};

#endif // _HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter_HeaderFile
