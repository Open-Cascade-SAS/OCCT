// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_NodeRepresentation_HeaderFile
#define _StepFEA_NodeRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class StepFEA_FeaModel;
class TCollection_HAsciiString;
class StepRepr_RepresentationContext;

//! Representation of STEP entity NodeRepresentation
class StepFEA_NodeRepresentation : public StepRepr_Representation
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_NodeRepresentation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aRepresentation_Name,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>&
                                                       aRepresentation_Items,
    const occ::handle<StepRepr_RepresentationContext>& aRepresentation_ContextOfItems,
    const occ::handle<StepFEA_FeaModel>&               aModelRef);

  //! Returns field ModelRef
  Standard_EXPORT occ::handle<StepFEA_FeaModel> ModelRef() const;

  //! Set field ModelRef
  Standard_EXPORT void SetModelRef(const occ::handle<StepFEA_FeaModel>& ModelRef);

  DEFINE_STANDARD_RTTIEXT(StepFEA_NodeRepresentation, StepRepr_Representation)

private:
  occ::handle<StepFEA_FeaModel> theModelRef;
};

#endif // _StepFEA_NodeRepresentation_HeaderFile
