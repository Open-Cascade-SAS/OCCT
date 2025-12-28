// Created on: 2015-09-10
// Created by: Irina Krylova
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#ifndef _STEPCAFControl_GDTProperty_HeaderFile
#define _STEPCAFControl_GDTProperty_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <STEPConstruct_Tool.hxx>
#include <Standard_CString.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceModifier.hxx>
#include <StepDimTol_GeometricToleranceType.hxx>
#include <StepDimTol_DatumReferenceModifier.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <NCollection_Sequence.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>
#include <StepShape_LimitsAndFits.hxx>
#include <NCollection_Sequence.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>
#include <XCAFDimTolObjects_DimensionFormVariance.hxx>
#include <XCAFDimTolObjects_DimensionGrade.hxx>
#include <XCAFDimTolObjects_DimensionType.hxx>
#include <XCAFDimTolObjects_DatumTargetType.hxx>
#include <XCAFDimTolObjects_DimensionQualifier.hxx>
#include <XCAFDimTolObjects_GeomToleranceModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceType.hxx>
#include <XCAFDimTolObjects_GeomToleranceTypeValue.hxx>

//! This class provides tools for access (read)
//! the GDT properties.
class STEPCAFControl_GDTProperty
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT             STEPCAFControl_GDTProperty();
  Standard_EXPORT static void GetDimModifiers(
    const occ::handle<StepRepr_CompoundRepresentationItem>& theCRI,
    NCollection_Sequence<XCAFDimTolObjects_DimensionModif>&      theModifiers);

  Standard_EXPORT static void GetDimClassOfTolerance(const occ::handle<StepShape_LimitsAndFits>& theLAF,
                                                     bool& theHolle,
                                                     XCAFDimTolObjects_DimensionFormVariance& theFV,
                                                     XCAFDimTolObjects_DimensionGrade&        theG);

  Standard_EXPORT static bool GetDimType(
    const occ::handle<TCollection_HAsciiString>& theName,
    XCAFDimTolObjects_DimensionType&        theType);

  Standard_EXPORT static bool GetDatumTargetType(
    const occ::handle<TCollection_HAsciiString>& theDescription,
    XCAFDimTolObjects_DatumTargetType&      theType);

  Standard_EXPORT static bool GetDimQualifierType(
    const occ::handle<TCollection_HAsciiString>& theDescription,
    XCAFDimTolObjects_DimensionQualifier&   theType);

  Standard_EXPORT static bool GetTolValueType(
    const occ::handle<TCollection_HAsciiString>&   theDescription,
    XCAFDimTolObjects_GeomToleranceTypeValue& theType);

  Standard_EXPORT static occ::handle<TCollection_HAsciiString> GetTolValueType(
    const XCAFDimTolObjects_GeomToleranceTypeValue& theType);

  Standard_EXPORT static occ::handle<TCollection_HAsciiString> GetDimTypeName(
    const XCAFDimTolObjects_DimensionType theType);

  Standard_EXPORT static occ::handle<TCollection_HAsciiString> GetDimQualifierName(
    const XCAFDimTolObjects_DimensionQualifier theQualifier);

  Standard_EXPORT static occ::handle<TCollection_HAsciiString> GetDimModifierName(
    const XCAFDimTolObjects_DimensionModif theModifier);

  Standard_EXPORT static occ::handle<StepShape_LimitsAndFits> GetLimitsAndFits(
    bool                        theHole,
    XCAFDimTolObjects_DimensionFormVariance theFormVariance,
    XCAFDimTolObjects_DimensionGrade        theGrade);

  Standard_EXPORT static occ::handle<TCollection_HAsciiString> GetDatumTargetName(
    const XCAFDimTolObjects_DatumTargetType theDatumType);

  Standard_EXPORT static StepDimTol_GeometricToleranceType GetGeomToleranceType(
    const XCAFDimTolObjects_GeomToleranceType theType);

  Standard_EXPORT static XCAFDimTolObjects_GeomToleranceType GetGeomToleranceType(
    const StepDimTol_GeometricToleranceType theType);

  Standard_EXPORT static occ::handle<StepDimTol_GeometricTolerance> GetGeomTolerance(
    const XCAFDimTolObjects_GeomToleranceType theType);

  Standard_EXPORT static StepDimTol_GeometricToleranceModifier GetGeomToleranceModifier(
    const XCAFDimTolObjects_GeomToleranceModif theModifier);

  Standard_EXPORT static occ::handle<NCollection_HArray1<StepDimTol_DatumReferenceModifier>> GetDatumRefModifiers(
    const NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif>& theModifiers,
    const XCAFDimTolObjects_DatumModifWithValue&    theModifWithVal,
    const double                             theValue,
    const StepBasic_Unit&                           theUnit);

  Standard_EXPORT static occ::handle<StepVisual_TessellatedGeometricSet> GetTessellation(
    const TopoDS_Shape& theShape);
};

#endif // _STEPCAFControl_GDTProperty_HeaderFile
