// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_ShapeDimensionRepresentation_HeaderFile
#define _StepShape_ShapeDimensionRepresentation_HeaderFile

#include <Standard.hxx>

#include <StepShape_ShapeDimensionRepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_ShapeRepresentation.hxx>

//! Representation of STEP entity ShapeDimensionRepresentation
class StepShape_ShapeDimensionRepresentation : public StepShape_ShapeRepresentation
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_ShapeDimensionRepresentation();

  //! Initialize all fields AP214
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                                      theName,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& theItems,
    const occ::handle<StepRepr_RepresentationContext>& theContextOfItems);

  //! Initialize all fields AP242
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                                        theName,
    const occ::handle<NCollection_HArray1<StepShape_ShapeDimensionRepresentationItem>>& theItems,
    const occ::handle<StepRepr_RepresentationContext>& theContextOfItems);

  Standard_EXPORT void SetItemsAP242(
    const occ::handle<NCollection_HArray1<StepShape_ShapeDimensionRepresentationItem>>& theItems);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepShape_ShapeDimensionRepresentationItem>>
                  ItemsAP242() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_ShapeDimensionRepresentation, StepShape_ShapeRepresentation)
private:
  occ::handle<NCollection_HArray1<StepShape_ShapeDimensionRepresentationItem>> itemsAP242;
};
#endif // _StepShape_ShapeDimensionRepresentation_HeaderFile
