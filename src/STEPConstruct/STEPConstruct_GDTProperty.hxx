// Created on: 1999-09-08
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _STEPConstruct_GDTProperty_HeaderFile
#define _STEPConstruct_GDTProperty_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <STEPConstruct_Tool.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <XCAFDimTolObjects_DimensionModifiersSequence.hxx>
#include <StepShape_LimitsAndFits.hxx>
#include <XCAFDimTolObjects_DimensionFormVariance.hxx>
#include <XCAFDimTolObjects_DimensionGrade.hxx>
#include <XCAFDimTolObjects_DimensionType.hxx>
#include <XCAFDimTolObjects_DatumTargetType.hxx>
#include <XCAFDimTolObjects_DimensionQualifier.hxx>
#include <XCAFDimTolObjects_GeomToleranceTypeValue.hxx>

//! This class provides tools for access (read)
//! the GDT properties.
class STEPConstruct_GDTProperty
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT STEPConstruct_GDTProperty();
  Standard_EXPORT static void GetDimModifiers(const Handle(StepRepr_CompoundRepresentationItem)& theCRI,
                                              XCAFDimTolObjects_DimensionModifiersSequence& theModifiers);

  Standard_EXPORT static void GetDimClassOfTolerance(const Handle(StepShape_LimitsAndFits)& theLAF,
                                                     Standard_Boolean theHolle,
                                                     XCAFDimTolObjects_DimensionFormVariance theFV,
                                                     XCAFDimTolObjects_DimensionGrade theG);

  Standard_EXPORT static Standard_Boolean GetDimType(const Handle(TCollection_HAsciiString)& theName,
                                                     XCAFDimTolObjects_DimensionType& theType);

  Standard_EXPORT static Standard_Boolean GetDatumTargetType(const Handle(TCollection_HAsciiString)& theDescription,
                                                     XCAFDimTolObjects_DatumTargetType& theType);

  Standard_EXPORT static Standard_Boolean GetDimQualifierType(const Handle(TCollection_HAsciiString)& theDescription,
                                                     XCAFDimTolObjects_DimensionQualifier& theType);

  Standard_EXPORT static Standard_Boolean GetTolValueType(const Handle(TCollection_HAsciiString)& theDescription,
                                                     XCAFDimTolObjects_GeomToleranceTypeValue& theType);

};

#endif // _STEPConstruct_GDTProperty_HeaderFile
