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

#ifndef _StepFEA_FeaShellMembraneBendingCouplingStiffness_HeaderFile
#define _StepFEA_FeaShellMembraneBendingCouplingStiffness_HeaderFile

#include <Standard.hxx>

#include <StepFEA_SymmetricTensor42d.hxx>
#include <StepFEA_FeaMaterialPropertyRepresentationItem.hxx>
class TCollection_HAsciiString;

class StepFEA_FeaShellMembraneBendingCouplingStiffness;
DEFINE_STANDARD_HANDLE(StepFEA_FeaShellMembraneBendingCouplingStiffness,
                       StepFEA_FeaMaterialPropertyRepresentationItem)

//! Representation of STEP entity FeaShellMembraneBendingCouplingStiffness
class StepFEA_FeaShellMembraneBendingCouplingStiffness
    : public StepFEA_FeaMaterialPropertyRepresentationItem
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaShellMembraneBendingCouplingStiffness();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aRepresentationItem_Name,
                            const StepFEA_SymmetricTensor42d&       aFeaConstants);

  //! Returns field FeaConstants
  Standard_EXPORT StepFEA_SymmetricTensor42d FeaConstants() const;

  //! Set field FeaConstants
  Standard_EXPORT void SetFeaConstants(const StepFEA_SymmetricTensor42d& FeaConstants);

  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaShellMembraneBendingCouplingStiffness,
                          StepFEA_FeaMaterialPropertyRepresentationItem)

protected:
private:
  StepFEA_SymmetricTensor42d theFeaConstants;
};

#endif // _StepFEA_FeaShellMembraneBendingCouplingStiffness_HeaderFile
