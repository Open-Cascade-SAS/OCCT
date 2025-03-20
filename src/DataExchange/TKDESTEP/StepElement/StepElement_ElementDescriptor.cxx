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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepElement_ElementDescriptor.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_ElementDescriptor, Standard_Transient)

//=================================================================================================

StepElement_ElementDescriptor::StepElement_ElementDescriptor() {}

//=================================================================================================

void StepElement_ElementDescriptor::Init(const StepElement_ElementOrder          aTopologyOrder,
                                         const Handle(TCollection_HAsciiString)& aDescription)
{

  theTopologyOrder = aTopologyOrder;

  theDescription = aDescription;
}

//=================================================================================================

StepElement_ElementOrder StepElement_ElementDescriptor::TopologyOrder() const
{
  return theTopologyOrder;
}

//=================================================================================================

void StepElement_ElementDescriptor::SetTopologyOrder(const StepElement_ElementOrder aTopologyOrder)
{
  theTopologyOrder = aTopologyOrder;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepElement_ElementDescriptor::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepElement_ElementDescriptor::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}
