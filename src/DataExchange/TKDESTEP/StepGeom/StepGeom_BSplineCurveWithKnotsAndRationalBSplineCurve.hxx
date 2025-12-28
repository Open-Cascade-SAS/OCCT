// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve_HeaderFile
#define _StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_BSplineCurve.hxx>
#include <Standard_Integer.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_KnotType.hxx>
class StepGeom_BSplineCurveWithKnots;
class StepGeom_RationalBSplineCurve;
class TCollection_HAsciiString;

class StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve : public StepGeom_BSplineCurve
{

public:
  //! Returns a BSplineCurveWithKnotsAndRationalBSplineCurve
  Standard_EXPORT StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aName,
    const int                                    aDegree,
    const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>&
                                                       aControlPointsList,
    const StepGeom_BSplineCurveForm                    aCurveForm,
    const StepData_Logical                             aClosedCurve,
    const StepData_Logical                             aSelfIntersect,
    const occ::handle<StepGeom_BSplineCurveWithKnots>& aBSplineCurveWithKnots,
    const occ::handle<StepGeom_RationalBSplineCurve>&  aRationalBSplineCurve);

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aName,
    const int                                    aDegree,
    const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>&
                                                    aControlPointsList,
    const StepGeom_BSplineCurveForm                 aCurveForm,
    const StepData_Logical                          aClosedCurve,
    const StepData_Logical                          aSelfIntersect,
    const occ::handle<NCollection_HArray1<int>>&    aKnotMultiplicities,
    const occ::handle<NCollection_HArray1<double>>& aKnots,
    const StepGeom_KnotType                         aKnotSpec,
    const occ::handle<NCollection_HArray1<double>>& aWeightsData);

  Standard_EXPORT void SetBSplineCurveWithKnots(
    const occ::handle<StepGeom_BSplineCurveWithKnots>& aBSplineCurveWithKnots);

  Standard_EXPORT occ::handle<StepGeom_BSplineCurveWithKnots> BSplineCurveWithKnots() const;

  Standard_EXPORT void SetRationalBSplineCurve(
    const occ::handle<StepGeom_RationalBSplineCurve>& aRationalBSplineCurve);

  Standard_EXPORT occ::handle<StepGeom_RationalBSplineCurve> RationalBSplineCurve() const;

  Standard_EXPORT void SetKnotMultiplicities(
    const occ::handle<NCollection_HArray1<int>>& aKnotMultiplicities);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> KnotMultiplicities() const;

  Standard_EXPORT int KnotMultiplicitiesValue(const int num) const;

  Standard_EXPORT int NbKnotMultiplicities() const;

  Standard_EXPORT void SetKnots(const occ::handle<NCollection_HArray1<double>>& aKnots);

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> Knots() const;

  Standard_EXPORT double KnotsValue(const int num) const;

  Standard_EXPORT int NbKnots() const;

  Standard_EXPORT void SetKnotSpec(const StepGeom_KnotType aKnotSpec);

  Standard_EXPORT StepGeom_KnotType KnotSpec() const;

  Standard_EXPORT void SetWeightsData(const occ::handle<NCollection_HArray1<double>>& aWeightsData);

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> WeightsData() const;

  Standard_EXPORT double WeightsDataValue(const int num) const;

  Standard_EXPORT int NbWeightsData() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve,
                          StepGeom_BSplineCurve)

private:
  occ::handle<StepGeom_BSplineCurveWithKnots> bSplineCurveWithKnots;
  occ::handle<StepGeom_RationalBSplineCurve>  rationalBSplineCurve;
};

#endif // _StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve_HeaderFile
