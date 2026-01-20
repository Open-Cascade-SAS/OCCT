// Created on: 2016-03-30
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StepBasic_ProductDefinitionReferenceWithLocalRepresentation_HeaderFile
#define _StepBasic_ProductDefinitionReferenceWithLocalRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ProductDefinition.hxx>

class StepBasic_ExternalSource;

class StepBasic_ProductDefinitionReferenceWithLocalRepresentation
    : public StepBasic_ProductDefinition
{

public:
  //! Returns a ProductDefinitionReferenceWithLocalRepresentation
  Standard_EXPORT StepBasic_ProductDefinitionReferenceWithLocalRepresentation();

  Standard_EXPORT void Init(const occ::handle<StepBasic_ExternalSource>&             theSource,
                            const occ::handle<TCollection_HAsciiString>&             theId,
                            const occ::handle<TCollection_HAsciiString>&             theDescription,
                            const occ::handle<StepBasic_ProductDefinitionFormation>& theFormation,
                            const occ::handle<StepBasic_ProductDefinitionContext>& theFrameOfReference);

  //! Returns field Source
  inline occ::handle<StepBasic_ExternalSource> Source() const { return mySource; }

  //! Set field Source
  inline void SetSource(const occ::handle<StepBasic_ExternalSource>& theSource) { mySource = theSource; }

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionReferenceWithLocalRepresentation,
                          StepBasic_ProductDefinition)

private:
  occ::handle<StepBasic_ExternalSource> mySource;
};
#endif // _StepBasic_ProductDefinitionReferenceWithLocalRepresentation_HeaderFile
