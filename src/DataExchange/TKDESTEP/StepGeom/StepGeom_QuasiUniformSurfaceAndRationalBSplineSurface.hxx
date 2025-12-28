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

#ifndef _StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface_HeaderFile
#define _StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_BSplineSurface.hxx>
#include <Standard_Integer.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepData_Logical.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
class StepGeom_QuasiUniformSurface;
class StepGeom_RationalBSplineSurface;
class TCollection_HAsciiString;

class StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface : public StepGeom_BSplineSurface
{

public:
  //! Returns a QuasiUniformSurfaceAndRationalBSplineSurface
  Standard_EXPORT StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&         aName,
                            const int                          aUDegree,
                            const int                          aVDegree,
                            const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
                            const StepGeom_BSplineSurfaceForm               aSurfaceForm,
                            const StepData_Logical                          aUClosed,
                            const StepData_Logical                          aVClosed,
                            const StepData_Logical                          aSelfIntersect,
                            const occ::handle<StepGeom_QuasiUniformSurface>&     aQuasiUniformSurface,
                            const occ::handle<StepGeom_RationalBSplineSurface>& aRationalBSplineSurface);

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&         aName,
                            const int                          aUDegree,
                            const int                          aVDegree,
                            const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
                            const StepGeom_BSplineSurfaceForm               aSurfaceForm,
                            const StepData_Logical                          aUClosed,
                            const StepData_Logical                          aVClosed,
                            const StepData_Logical                          aSelfIntersect,
                            const occ::handle<NCollection_HArray2<double>>&            aWeightsData);

  Standard_EXPORT void SetQuasiUniformSurface(
    const occ::handle<StepGeom_QuasiUniformSurface>& aQuasiUniformSurface);

  Standard_EXPORT occ::handle<StepGeom_QuasiUniformSurface> QuasiUniformSurface() const;

  Standard_EXPORT void SetRationalBSplineSurface(
    const occ::handle<StepGeom_RationalBSplineSurface>& aRationalBSplineSurface);

  Standard_EXPORT occ::handle<StepGeom_RationalBSplineSurface> RationalBSplineSurface() const;

  Standard_EXPORT void SetWeightsData(const occ::handle<NCollection_HArray2<double>>& aWeightsData);

  Standard_EXPORT occ::handle<NCollection_HArray2<double>> WeightsData() const;

  Standard_EXPORT double WeightsDataValue(const int num1,
                                                 const int num2) const;

  Standard_EXPORT int NbWeightsDataI() const;

  Standard_EXPORT int NbWeightsDataJ() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface,
                          StepGeom_BSplineSurface)

private:
  occ::handle<StepGeom_QuasiUniformSurface>    quasiUniformSurface;
  occ::handle<StepGeom_RationalBSplineSurface> rationalBSplineSurface;
};

#endif // _StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface_HeaderFile
