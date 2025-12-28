// Created on: 2001-12-28
// Created by: Andrey BETENEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_EdgeBasedWireframeModel, StepGeom_GeometricRepresentationItem)

//=================================================================================================

StepShape_EdgeBasedWireframeModel::StepShape_EdgeBasedWireframeModel() = default;

//=================================================================================================

void StepShape_EdgeBasedWireframeModel::Init(
  const occ::handle<TCollection_HAsciiString>& aRepresentationItem_Name,
  const occ::handle<NCollection_HArray1<occ::handle<StepShape_ConnectedEdgeSet>>>& aEbwmBoundary)
{
  StepGeom_GeometricRepresentationItem::Init(aRepresentationItem_Name);

  theEbwmBoundary = aEbwmBoundary;
}

//=================================================================================================

occ::handle<NCollection_HArray1<occ::handle<StepShape_ConnectedEdgeSet>>>
  StepShape_EdgeBasedWireframeModel::EbwmBoundary() const
{
  return theEbwmBoundary;
}

//=================================================================================================

void StepShape_EdgeBasedWireframeModel::SetEbwmBoundary(
  const occ::handle<NCollection_HArray1<occ::handle<StepShape_ConnectedEdgeSet>>>& aEbwmBoundary)
{
  theEbwmBoundary = aEbwmBoundary;
}
