// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#include <GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>

#include <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <GeomInt_TheMultiLineToolOfWLApprox.hxx>
#include <GeomInt_MyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <GeomInt_BSpParLeastSquareOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>

GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox::
  GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox(
    math_MultipleVarFunctionWithGradient& F,
    const math_Vector&                    StartingPoint,
    const Standard_Real                   Tolerance3d,
    const Standard_Real                   Tolerance2d,
    const Standard_Real                   Eps,
    const Standard_Integer                NbIterations)
    : math_BFGS(F.NbVariables(), Eps, NbIterations, Eps),
      myTol3d(Tolerance3d),
      myTol2d(Tolerance2d)
{
  Perform(F, StartingPoint);
}

Standard_Boolean GeomInt_BSpGradient_BFGSOfMyBSplGradientOfTheComputeLineOfWLApprox::
  IsSolutionReached(math_MultipleVarFunctionWithGradient& F) const
{
  Standard_Boolean                                                  Result, Result2;
  GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox* F1 =
    (GeomInt_BSpParFunctionOfMyBSplGradientOfTheComputeLineOfWLApprox*)&F;

  Result               = (2.0 * fabs(TheMinimum - PreviousMinimum)
            <= 1.e-10 * (fabs(TheMinimum) + fabs(PreviousMinimum)) + 1.e-12);
  Standard_Real MErr3d = F1->MaxError3d();
  Standard_Real MErr2d = F1->MaxError2d();
  Result2              = ((MErr3d <= myTol3d) && (MErr2d <= myTol2d));

  return (Result || Result2);
}
