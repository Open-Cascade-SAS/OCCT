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

#ifndef _StepFEA_NodeSet_HeaderFile
#define _StepFEA_NodeSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_NodeRepresentation.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class TCollection_HAsciiString;

//! Representation of STEP entity NodeSet
class StepFEA_NodeSet : public StepGeom_GeometricRepresentationItem
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_NodeSet();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aRepresentationItem_Name,
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_NodeRepresentation>>>& aNodes);

  //! Returns field Nodes
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepFEA_NodeRepresentation>>> Nodes()
    const;

  //! Set field Nodes
  Standard_EXPORT void SetNodes(
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_NodeRepresentation>>>& Nodes);

  DEFINE_STANDARD_RTTIEXT(StepFEA_NodeSet, StepGeom_GeometricRepresentationItem)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepFEA_NodeRepresentation>>> theNodes;
};

#endif // _StepFEA_NodeSet_HeaderFile
