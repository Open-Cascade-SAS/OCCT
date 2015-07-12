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

#ifndef _AppDef_TheGradient_HeaderFile
#define _AppDef_TheGradient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiCurve.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
class Standard_OutOfRange;
class StdFail_NotDone;
class AppDef_MultiLine;
class AppDef_MyLineTool;
class AppDef_ParLeastSquareOfTheGradient;
class AppDef_ResConstraintOfTheGradient;
class AppDef_ParFunctionOfTheGradient;
class AppDef_Gradient_BFGSOfTheGradient;
class AppParCurves_MultiCurve;



class AppDef_TheGradient 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AppDef_TheGradient(const AppDef_MultiLine& SSP, const Standard_Integer FirstPoint, const Standard_Integer LastPoint, const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints, math_Vector& Parameters, const Standard_Integer Deg, const Standard_Real Tol3d, const Standard_Real Tol2d, const Standard_Integer NbIterations = 200);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT AppParCurves_MultiCurve Value() const;
  
  Standard_EXPORT Standard_Real Error (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Real MaxError3d() const;
  
  Standard_EXPORT Standard_Real MaxError2d() const;
  
  Standard_EXPORT Standard_Real AverageError() const;




protected:





private:



  AppParCurves_MultiCurve SCU;
  math_Vector ParError;
  Standard_Real AvError;
  Standard_Real MError3d;
  Standard_Real MError2d;
  Standard_Boolean Done;


};







#endif // _AppDef_TheGradient_HeaderFile
