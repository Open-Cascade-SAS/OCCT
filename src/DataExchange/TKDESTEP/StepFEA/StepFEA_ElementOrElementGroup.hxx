// Created on: 2003-02-04
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_ElementOrElementGroup_HeaderFile
#define _StepFEA_ElementOrElementGroup_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepFEA_ElementRepresentation;
class StepFEA_ElementGroup;

//! Representation of STEP SELECT type ElementOrElementGroup
class StepFEA_ElementOrElementGroup : public StepData_SelectType
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepFEA_ElementOrElementGroup();

  //! Recognizes a kind of ElementOrElementGroup select type
  //! 1 -> ElementRepresentation from StepFEA
  //! 2 -> ElementGroup from StepFEA
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum(const Handle(Standard_Transient)& ent) const;

  //! Returns Value as ElementRepresentation (or Null if another type)
  Standard_EXPORT Handle(StepFEA_ElementRepresentation) ElementRepresentation() const;

  //! Returns Value as ElementGroup (or Null if another type)
  Standard_EXPORT Handle(StepFEA_ElementGroup) ElementGroup() const;

protected:
private:
};

#endif // _StepFEA_ElementOrElementGroup_HeaderFile
