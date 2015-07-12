// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
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

#ifndef _BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox_HeaderFile
#define _BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <math_BFGS.hxx>
#include <math_Vector.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class BRepApprox_TheMultiLineOfApprox;
class BRepApprox_TheMultiLineToolOfApprox;
class BRepApprox_MyGradientOfTheComputeLineBezierOfApprox;
class BRepApprox_ParLeastSquareOfMyGradientOfTheComputeLineBezierOfApprox;
class BRepApprox_ResConstraintOfMyGradientOfTheComputeLineBezierOfApprox;
class BRepApprox_ParFunctionOfMyGradientOfTheComputeLineBezierOfApprox;
class math_MultipleVarFunctionWithGradient;



class BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox  : public math_BFGS
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox(math_MultipleVarFunctionWithGradient& F, const math_Vector& StartingPoint, const Standard_Real Tolerance3d, const Standard_Real Tolerance2d, const Standard_Real Eps, const Standard_Integer NbIterations = 200);
  
  Standard_EXPORT virtual Standard_Boolean IsSolutionReached (math_MultipleVarFunctionWithGradient& F) const Standard_OVERRIDE;




protected:





private:



  Standard_Real myTol3d;
  Standard_Real myTol2d;


};







#endif // _BRepApprox_Gradient_BFGSOfMyGradientOfTheComputeLineBezierOfApprox_HeaderFile
