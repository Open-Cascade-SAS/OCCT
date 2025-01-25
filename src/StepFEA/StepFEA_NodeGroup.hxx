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

#ifndef _StepFEA_NodeGroup_HeaderFile
#define _StepFEA_NodeGroup_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_HArray1OfNodeRepresentation.hxx>
#include <StepFEA_FeaGroup.hxx>
class TCollection_HAsciiString;
class StepFEA_FeaModel;

class StepFEA_NodeGroup;
DEFINE_STANDARD_HANDLE(StepFEA_NodeGroup, StepFEA_FeaGroup)

//! Representation of STEP entity NodeGroup
class StepFEA_NodeGroup : public StepFEA_FeaGroup
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_NodeGroup();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&            aGroup_Name,
                            const Handle(TCollection_HAsciiString)&            aGroup_Description,
                            const Handle(StepFEA_FeaModel)&                    aFeaGroup_ModelRef,
                            const Handle(StepFEA_HArray1OfNodeRepresentation)& aNodes);

  //! Returns field Nodes
  Standard_EXPORT Handle(StepFEA_HArray1OfNodeRepresentation) Nodes() const;

  //! Set field Nodes
  Standard_EXPORT void SetNodes(const Handle(StepFEA_HArray1OfNodeRepresentation)& Nodes);

  DEFINE_STANDARD_RTTIEXT(StepFEA_NodeGroup, StepFEA_FeaGroup)

protected:
private:
  Handle(StepFEA_HArray1OfNodeRepresentation) theNodes;
};

#endif // _StepFEA_NodeGroup_HeaderFile
