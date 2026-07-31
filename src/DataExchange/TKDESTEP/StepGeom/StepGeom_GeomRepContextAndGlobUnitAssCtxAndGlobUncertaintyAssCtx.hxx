// Created on: 1995-12-07
// Created by: Frederic MAUPAS
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

#ifndef _StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx_HeaderFile
#define _StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationContext.hxx>
#include <Standard_Integer.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
class StepGeom_GeometricRepresentationContext;
class StepRepr_GlobalUnitAssignedContext;
class StepRepr_GlobalUncertaintyAssignedContext;
class TCollection_HAsciiString;
class StepBasic_NamedUnit;
class StepBasic_UncertaintyMeasureWithUnit;

class StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx
    : public StepRepr_RepresentationContext
{

public:
  Standard_EXPORT StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                  aContextIdentifier,
    const occ::handle<TCollection_HAsciiString>&                  aContextType,
    const occ::handle<StepGeom_GeometricRepresentationContext>&   aGeometricRepresentationCtx,
    const occ::handle<StepRepr_GlobalUnitAssignedContext>&        aGlobalUnitAssignedCtx,
    const occ::handle<StepRepr_GlobalUncertaintyAssignedContext>& aGlobalUncertaintyAssignedCtx);

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aContextIdentifier,
    const occ::handle<TCollection_HAsciiString>& aContextType,
    const int                                    aCoordinateSpaceDimension,
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>>& aUnits,
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>&
      anUncertainty);

  Standard_EXPORT void SetGeometricRepresentationContext(
    const occ::handle<StepGeom_GeometricRepresentationContext>& aGeometricRepresentationContext);

  Standard_EXPORT occ::handle<StepGeom_GeometricRepresentationContext>
                  GeometricRepresentationContext() const;

  Standard_EXPORT void SetGlobalUnitAssignedContext(
    const occ::handle<StepRepr_GlobalUnitAssignedContext>& aGlobalUnitAssignedContext);

  Standard_EXPORT occ::handle<StepRepr_GlobalUnitAssignedContext> GlobalUnitAssignedContext() const;

  Standard_EXPORT void SetGlobalUncertaintyAssignedContext(
    const occ::handle<StepRepr_GlobalUncertaintyAssignedContext>& aGlobalUncertaintyAssignedCtx);

  Standard_EXPORT occ::handle<StepRepr_GlobalUncertaintyAssignedContext>
                  GlobalUncertaintyAssignedContext() const;

  Standard_EXPORT void SetCoordinateSpaceDimension(const int aCoordinateSpaceDimension);

  Standard_EXPORT int CoordinateSpaceDimension() const;

  Standard_EXPORT void SetUnits(
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>>& aUnits);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepBasic_NamedUnit>>> Units() const;

  Standard_EXPORT occ::handle<StepBasic_NamedUnit> UnitsValue(const int num) const;

  Standard_EXPORT int NbUnits() const;

  Standard_EXPORT void SetUncertainty(
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>&
      aUncertainty);

  Standard_EXPORT occ::handle<
    NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>
    Uncertainty() const;

  Standard_EXPORT occ::handle<StepBasic_UncertaintyMeasureWithUnit> UncertaintyValue(
    const int num) const;

  Standard_EXPORT int NbUncertainty() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx,
                          StepRepr_RepresentationContext)

private:
  occ::handle<StepGeom_GeometricRepresentationContext>   geometricRepresentationContext;
  occ::handle<StepRepr_GlobalUnitAssignedContext>        globalUnitAssignedContext;
  occ::handle<StepRepr_GlobalUncertaintyAssignedContext> globalUncertaintyAssignedContext;
};

#endif // _StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx_HeaderFile
