// Created on: 1999-03-24
// Created by: data exchange team
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

#ifndef _STEPSelections_AssemblyComponent_HeaderFile
#define _STEPSelections_AssemblyComponent_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <STEPSelections_HSequenceOfAssemblyLink.hxx>
#include <Standard_Transient.hxx>
class StepShape_ShapeDefinitionRepresentation;

class STEPSelections_AssemblyComponent;
DEFINE_STANDARD_HANDLE(STEPSelections_AssemblyComponent, Standard_Transient)

class STEPSelections_AssemblyComponent : public Standard_Transient
{

public:
  Standard_EXPORT STEPSelections_AssemblyComponent();

  Standard_EXPORT STEPSelections_AssemblyComponent(
    const Handle(StepShape_ShapeDefinitionRepresentation)& sdr,
    const Handle(STEPSelections_HSequenceOfAssemblyLink)&  list);

  Handle(StepShape_ShapeDefinitionRepresentation) GetSDR() const;

  Handle(STEPSelections_HSequenceOfAssemblyLink) GetList() const;

  void SetSDR(const Handle(StepShape_ShapeDefinitionRepresentation)& sdr);

  void SetList(const Handle(STEPSelections_HSequenceOfAssemblyLink)& list);

  DEFINE_STANDARD_RTTIEXT(STEPSelections_AssemblyComponent, Standard_Transient)

protected:
private:
  Handle(StepShape_ShapeDefinitionRepresentation) mySDR;
  Handle(STEPSelections_HSequenceOfAssemblyLink)  myList;
};

#include <STEPSelections_AssemblyComponent.lxx>

#endif // _STEPSelections_AssemblyComponent_HeaderFile
