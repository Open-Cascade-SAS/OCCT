// Created on: 1991-12-02
// Created by: Laurent PAINNOT
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

#ifndef _AppDef_Gradient_BFGSOfMyGradientOfCompute_HeaderFile
#define _AppDef_Gradient_BFGSOfMyGradientOfCompute_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_BFGS.hxx>
#include <math_Vector.hxx>
class AppDef_MultiLine;
class AppDef_MyLineTool;
class AppDef_MyGradientOfCompute;
class AppDef_ParLeastSquareOfMyGradientOfCompute;
class AppDef_ResConstraintOfMyGradientOfCompute;
class AppDef_ParFunctionOfMyGradientOfCompute;
class math_MultipleVarFunctionWithGradient;

class AppDef_Gradient_BFGSOfMyGradientOfCompute : public math_BFGS
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT AppDef_Gradient_BFGSOfMyGradientOfCompute(
    math_MultipleVarFunctionWithGradient& F,
    const math_Vector&                    StartingPoint,
    const Standard_Real                   Tolerance3d,
    const Standard_Real                   Tolerance2d,
    const Standard_Real                   Eps,
    const Standard_Integer                NbIterations = 200);

  Standard_EXPORT virtual Standard_Boolean IsSolutionReached(
    math_MultipleVarFunctionWithGradient& F) const Standard_OVERRIDE;

protected:
private:
  Standard_Real myTol3d;
  Standard_Real myTol2d;
};

#endif // _AppDef_Gradient_BFGSOfMyGradientOfCompute_HeaderFile
