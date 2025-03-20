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

#ifndef _StepFEA_FeaGroup_HeaderFile
#define _StepFEA_FeaGroup_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Group.hxx>
class StepFEA_FeaModel;
class TCollection_HAsciiString;

class StepFEA_FeaGroup;
DEFINE_STANDARD_HANDLE(StepFEA_FeaGroup, StepBasic_Group)

//! Representation of STEP entity FeaGroup
class StepFEA_FeaGroup : public StepBasic_Group
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaGroup();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aGroup_Name,
                            const Handle(TCollection_HAsciiString)& aGroup_Description,
                            const Handle(StepFEA_FeaModel)&         aModelRef);

  //! Returns field ModelRef
  Standard_EXPORT Handle(StepFEA_FeaModel) ModelRef() const;

  //! Set field ModelRef
  Standard_EXPORT void SetModelRef(const Handle(StepFEA_FeaModel)& ModelRef);

  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaGroup, StepBasic_Group)

protected:
private:
  Handle(StepFEA_FeaModel) theModelRef;
};

#endif // _StepFEA_FeaGroup_HeaderFile
