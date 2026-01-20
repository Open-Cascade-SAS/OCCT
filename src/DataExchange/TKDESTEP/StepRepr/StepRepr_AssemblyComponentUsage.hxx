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

#ifndef _StepRepr_AssemblyComponentUsage_HeaderFile
#define _StepRepr_AssemblyComponentUsage_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_ProductDefinitionUsage.hxx>
class TCollection_HAsciiString;
class StepBasic_ProductDefinition;

//! Representation of STEP entity AssemblyComponentUsage
class StepRepr_AssemblyComponentUsage : public StepRepr_ProductDefinitionUsage
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_AssemblyComponentUsage();

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
    const bool                  hasReferenceDesignator,
    const occ::handle<TCollection_HAsciiString>& aReferenceDesignator);

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
    const bool                  hasReferenceDesignator,
    const occ::handle<TCollection_HAsciiString>& aReferenceDesignator);

  //! Returns field ReferenceDesignator
  Standard_EXPORT occ::handle<TCollection_HAsciiString> ReferenceDesignator() const;

  //! Set field ReferenceDesignator
  Standard_EXPORT void SetReferenceDesignator(
    const occ::handle<TCollection_HAsciiString>& ReferenceDesignator);

  //! Returns True if optional field ReferenceDesignator is defined
  Standard_EXPORT bool HasReferenceDesignator() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_AssemblyComponentUsage, StepRepr_ProductDefinitionUsage)

private:
  occ::handle<TCollection_HAsciiString> theReferenceDesignator;
  bool                 defReferenceDesignator;
};

#endif // _StepRepr_AssemblyComponentUsage_HeaderFile
