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

#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <math_Vector.hxx>
class FEmTool_Curve;
class math_Matrix;

//! defined criterion to smooth points in curve
class AppDef_SmoothCriterion : public Standard_Transient
{

public:
  Standard_EXPORT virtual void SetParameters(
    const occ::handle<NCollection_HArray1<double>>& Parameters) = 0;

  Standard_EXPORT virtual void SetCurve(const occ::handle<FEmTool_Curve>& C) = 0;

  Standard_EXPORT virtual void GetCurve(occ::handle<FEmTool_Curve>& C) const = 0;

  Standard_EXPORT virtual void SetEstimation(const double E1, const double E2, const double E3) = 0;

  Standard_EXPORT virtual double& EstLength() = 0;

  Standard_EXPORT virtual void GetEstimation(double& E1, double& E2, double& E3) const = 0;

  Standard_EXPORT virtual occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>
    AssemblyTable() const = 0;

  Standard_EXPORT virtual occ::handle<NCollection_HArray2<int>> DependenceTable() const = 0;

  Standard_EXPORT virtual int QualityValues(const double J1min,
                                            const double J2min,
                                            const double J3min,
                                            double&      J1,
                                            double&      J2,
                                            double&      J3) = 0;

  Standard_EXPORT virtual void ErrorValues(double& MaxError,
                                           double& QuadraticError,
                                           double& AverageError) = 0;

  Standard_EXPORT virtual void Hessian(const int    Element,
                                       const int    Dimension1,
                                       const int    Dimension2,
                                       math_Matrix& H) = 0;

  Standard_EXPORT virtual void Gradient(const int Element, const int Dimension, math_Vector& G) = 0;

  //! Convert the assembly Vector in an Curve;
  Standard_EXPORT virtual void InputVector(
    const math_Vector&                                                             X,
    const occ::handle<NCollection_HArray2<occ::handle<NCollection_HArray1<int>>>>& AssTable) = 0;

  Standard_EXPORT virtual void SetWeight(const double QuadraticWeight,
                                         const double QualityWeight,
                                         const double percentJ1,
                                         const double percentJ2,
                                         const double percentJ3) = 0;

  Standard_EXPORT virtual void GetWeight(double& QuadraticWeight, double& QualityWeight) const = 0;

  Standard_EXPORT virtual void SetWeight(const NCollection_Array1<double>& Weight) = 0;

  DEFINE_STANDARD_RTTIEXT(AppDef_SmoothCriterion, Standard_Transient)
};

#endif // _AppDef_SmoothCriterion_HeaderFile
