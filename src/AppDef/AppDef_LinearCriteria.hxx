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

#ifndef _AppDef_LinearCriteria_HeaderFile
#define _AppDef_LinearCriteria_HeaderFile

#include <Standard.hxx>

#include <AppDef_MultiLine.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <AppDef_SmoothCriterion.hxx>
#include <FEmTool_HAssemblyTable.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <math_Vector.hxx>
class FEmTool_ElementaryCriterion;
class FEmTool_Curve;
class math_Matrix;

class AppDef_LinearCriteria;
DEFINE_STANDARD_HANDLE(AppDef_LinearCriteria, AppDef_SmoothCriterion)

//! defined an Linear Criteria to used in variational
//! Smoothing of points.
class AppDef_LinearCriteria : public AppDef_SmoothCriterion
{

public:
  Standard_EXPORT AppDef_LinearCriteria(const AppDef_MultiLine& SSP,
                                        const Standard_Integer  FirstPoint,
                                        const Standard_Integer  LastPoint);

  Standard_EXPORT void SetParameters(const Handle(TColStd_HArray1OfReal)& Parameters)
    Standard_OVERRIDE;

  Standard_EXPORT void SetCurve(const Handle(FEmTool_Curve)& C) Standard_OVERRIDE;

  Standard_EXPORT void GetCurve(Handle(FEmTool_Curve)& C) const Standard_OVERRIDE;

  Standard_EXPORT void SetEstimation(const Standard_Real E1,
                                     const Standard_Real E2,
                                     const Standard_Real E3) Standard_OVERRIDE;

  Standard_EXPORT Standard_Real& EstLength() Standard_OVERRIDE;

  Standard_EXPORT void GetEstimation(Standard_Real& E1,
                                     Standard_Real& E2,
                                     Standard_Real& E3) const Standard_OVERRIDE;

  Standard_EXPORT Handle(FEmTool_HAssemblyTable) AssemblyTable() const Standard_OVERRIDE;

  Standard_EXPORT Handle(TColStd_HArray2OfInteger) DependenceTable() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer QualityValues(const Standard_Real J1min,
                                                 const Standard_Real J2min,
                                                 const Standard_Real J3min,
                                                 Standard_Real&      J1,
                                                 Standard_Real&      J2,
                                                 Standard_Real&      J3) Standard_OVERRIDE;

  Standard_EXPORT void ErrorValues(Standard_Real& MaxError,
                                   Standard_Real& QuadraticError,
                                   Standard_Real& AverageError) Standard_OVERRIDE;

  Standard_EXPORT void Hessian(const Standard_Integer Element,
                               const Standard_Integer Dimension1,
                               const Standard_Integer Dimension2,
                               math_Matrix&           H) Standard_OVERRIDE;

  Standard_EXPORT void Gradient(const Standard_Integer Element,
                                const Standard_Integer Dimension,
                                math_Vector&           G) Standard_OVERRIDE;

  //! Convert the assembly Vector in an Curve;
  Standard_EXPORT void InputVector(const math_Vector&                    X,
                                   const Handle(FEmTool_HAssemblyTable)& AssTable)
    Standard_OVERRIDE;

  Standard_EXPORT void SetWeight(const Standard_Real QuadraticWeight,
                                 const Standard_Real QualityWeight,
                                 const Standard_Real percentJ1,
                                 const Standard_Real percentJ2,
                                 const Standard_Real percentJ3) Standard_OVERRIDE;

  Standard_EXPORT void GetWeight(Standard_Real& QuadraticWeight,
                                 Standard_Real& QualityWeight) const Standard_OVERRIDE;

  Standard_EXPORT void SetWeight(const TColStd_Array1OfReal& Weight) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(AppDef_LinearCriteria, AppDef_SmoothCriterion)

protected:
private:
  Standard_EXPORT void BuildCache(const Standard_Integer E);

  AppDef_MultiLine                    mySSP;
  Handle(TColStd_HArray1OfReal)       myParameters;
  Handle(TColStd_HArray1OfReal)       myCache;
  Handle(FEmTool_ElementaryCriterion) myCriteria[3];
  Standard_Real                       myEstimation[3];
  Standard_Real                       myQuadraticWeight;
  Standard_Real                       myQualityWeight;
  Standard_Real                       myPercent[3];
  TColStd_Array1OfReal                myPntWeight;
  Handle(FEmTool_Curve)               myCurve;
  Standard_Real                       myLength;
  Standard_Integer                    myE;
  Standard_Integer                    IF;
  Standard_Integer                    IL;
};

#endif // _AppDef_LinearCriteria_HeaderFile
