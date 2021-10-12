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

#ifndef _AppDef_SmoothCriterion_HeaderFile
#define _AppDef_SmoothCriterion_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>
#include <FEmTool_HAssemblyTable.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>
class FEmTool_Curve;
class math_Matrix;


class AppDef_SmoothCriterion;
DEFINE_STANDARD_HANDLE(AppDef_SmoothCriterion, Standard_Transient)

//! defined criterion to smooth  points in  curve
class AppDef_SmoothCriterion : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual void SetParameters (const Handle(TColStd_HArray1OfReal)& Parameters) = 0;
  
  Standard_EXPORT virtual void SetCurve (const Handle(FEmTool_Curve)& C) = 0;
  
  Standard_EXPORT virtual void GetCurve (Handle(FEmTool_Curve)& C) const = 0;
  
  Standard_EXPORT virtual void SetEstimation (const Standard_Real E1, const Standard_Real E2, const Standard_Real E3) = 0;
  
  Standard_EXPORT virtual Standard_Real& EstLength() = 0;
  
  Standard_EXPORT virtual void GetEstimation (Standard_Real& E1, Standard_Real& E2, Standard_Real& E3) const = 0;
  
  Standard_EXPORT virtual Handle(FEmTool_HAssemblyTable) AssemblyTable() const = 0;
  
  Standard_EXPORT virtual Handle(TColStd_HArray2OfInteger) DependenceTable() const = 0;
  
  Standard_EXPORT virtual Standard_Integer QualityValues (const Standard_Real J1min, const Standard_Real J2min, const Standard_Real J3min, Standard_Real& J1, Standard_Real& J2, Standard_Real& J3) = 0;
  
  Standard_EXPORT virtual void ErrorValues (Standard_Real& MaxError, Standard_Real& QuadraticError, Standard_Real& AverageError) = 0;
  
  Standard_EXPORT virtual void Hessian (const Standard_Integer Element, const Standard_Integer Dimension1, const Standard_Integer Dimension2, math_Matrix& H) = 0;
  
  Standard_EXPORT virtual void Gradient (const Standard_Integer Element, const Standard_Integer Dimension, math_Vector& G) = 0;
  
  //! Convert the assembly Vector in an Curve;
  Standard_EXPORT virtual void InputVector (const math_Vector& X, const Handle(FEmTool_HAssemblyTable)& AssTable) = 0;
  
  Standard_EXPORT virtual void SetWeight (const Standard_Real QuadraticWeight, const Standard_Real QualityWeight, const Standard_Real percentJ1, const Standard_Real percentJ2, const Standard_Real percentJ3) = 0;
  
  Standard_EXPORT virtual void GetWeight (Standard_Real& QuadraticWeight, Standard_Real& QualityWeight) const = 0;
  
  Standard_EXPORT virtual void SetWeight (const TColStd_Array1OfReal& Weight) = 0;




  DEFINE_STANDARD_RTTIEXT(AppDef_SmoothCriterion,Standard_Transient)

protected:




private:




};







#endif // _AppDef_SmoothCriterion_HeaderFile
