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

#ifndef _StepGeom_BSplineSurfaceWithKnots_HeaderFile
#define _StepGeom_BSplineSurfaceWithKnots_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepGeom_KnotType.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <Standard_Integer.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepData_Logical.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;

class StepGeom_BSplineSurfaceWithKnots : public StepGeom_BSplineSurface
{

public:
  //! Returns a BSplineSurfaceWithKnots
  Standard_EXPORT StepGeom_BSplineSurfaceWithKnots();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&         aName,
                            const int                          aUDegree,
                            const int                          aVDegree,
                            const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
                            const StepGeom_BSplineSurfaceForm               aSurfaceForm,
                            const StepData_Logical                          aUClosed,
                            const StepData_Logical                          aVClosed,
                            const StepData_Logical                          aSelfIntersect,
                            const occ::handle<NCollection_HArray1<int>>&         aUMultiplicities,
                            const occ::handle<NCollection_HArray1<int>>&         aVMultiplicities,
                            const occ::handle<NCollection_HArray1<double>>&            aUKnots,
                            const occ::handle<NCollection_HArray1<double>>&            aVKnots,
                            const StepGeom_KnotType                         aKnotSpec);

  Standard_EXPORT void SetUMultiplicities(const occ::handle<NCollection_HArray1<int>>& aUMultiplicities);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> UMultiplicities() const;

  Standard_EXPORT int UMultiplicitiesValue(const int num) const;

  Standard_EXPORT int NbUMultiplicities() const;

  Standard_EXPORT void SetVMultiplicities(const occ::handle<NCollection_HArray1<int>>& aVMultiplicities);

  Standard_EXPORT occ::handle<NCollection_HArray1<int>> VMultiplicities() const;

  Standard_EXPORT int VMultiplicitiesValue(const int num) const;

  Standard_EXPORT int NbVMultiplicities() const;

  Standard_EXPORT void SetUKnots(const occ::handle<NCollection_HArray1<double>>& aUKnots);

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> UKnots() const;

  Standard_EXPORT double UKnotsValue(const int num) const;

  Standard_EXPORT int NbUKnots() const;

  Standard_EXPORT void SetVKnots(const occ::handle<NCollection_HArray1<double>>& aVKnots);

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> VKnots() const;

  Standard_EXPORT double VKnotsValue(const int num) const;

  Standard_EXPORT int NbVKnots() const;

  Standard_EXPORT void SetKnotSpec(const StepGeom_KnotType aKnotSpec);

  Standard_EXPORT StepGeom_KnotType KnotSpec() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_BSplineSurfaceWithKnots, StepGeom_BSplineSurface)

private:
  occ::handle<NCollection_HArray1<int>> uMultiplicities;
  occ::handle<NCollection_HArray1<int>> vMultiplicities;
  occ::handle<NCollection_HArray1<double>>    uKnots;
  occ::handle<NCollection_HArray1<double>>    vKnots;
  StepGeom_KnotType                knotSpec;
};

#endif // _StepGeom_BSplineSurfaceWithKnots_HeaderFile
