// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_SpecifiedHigherUsageOccurrence_HeaderFile
#define _StepRepr_SpecifiedHigherUsageOccurrence_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_AssemblyComponentUsage.hxx>
class StepRepr_NextAssemblyUsageOccurrence;
class TCollection_HAsciiString;
class StepBasic_ProductDefinition;

//! Representation of STEP entity SpecifiedHigherUsageOccurrence
class StepRepr_SpecifiedHigherUsageOccurrence : public StepRepr_AssemblyComponentUsage
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_SpecifiedHigherUsageOccurrence();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Id,
    const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Name,
    const bool                  hasProductDefinitionRelationship_Description,
    const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Description,
    const occ::handle<StepBasic_ProductDefinition>&
      aProductDefinitionRelationship_RelatingProductDefinition,
    const occ::handle<StepBasic_ProductDefinition>&
                                            aProductDefinitionRelationship_RelatedProductDefinition,
    const bool                  hasAssemblyComponentUsage_ReferenceDesignator,
    const occ::handle<TCollection_HAsciiString>& aAssemblyComponentUsage_ReferenceDesignator,
    const occ::handle<StepRepr_AssemblyComponentUsage>&      aUpperUsage,
    const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& aNextUsage);

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Id,
    const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Name,
    const bool                  hasProductDefinitionRelationship_Description,
    const occ::handle<TCollection_HAsciiString>& aProductDefinitionRelationship_Description,
    const StepBasic_ProductDefinitionOrReference&
      aProductDefinitionRelationship_RelatingProductDefinition,
    const StepBasic_ProductDefinitionOrReference&
                                            aProductDefinitionRelationship_RelatedProductDefinition,
    const bool                  hasAssemblyComponentUsage_ReferenceDesignator,
    const occ::handle<TCollection_HAsciiString>& aAssemblyComponentUsage_ReferenceDesignator,
    const occ::handle<StepRepr_AssemblyComponentUsage>&      aUpperUsage,
    const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& aNextUsage);

  //! Returns field UpperUsage
  Standard_EXPORT occ::handle<StepRepr_AssemblyComponentUsage> UpperUsage() const;

  //! Set field UpperUsage
  Standard_EXPORT void SetUpperUsage(const occ::handle<StepRepr_AssemblyComponentUsage>& UpperUsage);

  //! Returns field NextUsage
  Standard_EXPORT occ::handle<StepRepr_NextAssemblyUsageOccurrence> NextUsage() const;

  //! Set field NextUsage
  Standard_EXPORT void SetNextUsage(const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& NextUsage);

  DEFINE_STANDARD_RTTIEXT(StepRepr_SpecifiedHigherUsageOccurrence, StepRepr_AssemblyComponentUsage)

private:
  occ::handle<StepRepr_AssemblyComponentUsage>      theUpperUsage;
  occ::handle<StepRepr_NextAssemblyUsageOccurrence> theNextUsage;
};

#endif // _StepRepr_SpecifiedHigherUsageOccurrence_HeaderFile
