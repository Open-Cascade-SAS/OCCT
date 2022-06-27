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

#include <StepShape_HArray1OfShapeDimensionRepresentationItem.hxx>
#include <StepShape_ShapeRepresentation.hxx>


class StepShape_ShapeDimensionRepresentation;
DEFINE_STANDARD_HANDLE(StepShape_ShapeDimensionRepresentation, StepShape_ShapeRepresentation)

//! Representation of STEP entity ShapeDimensionRepresentation
class StepShape_ShapeDimensionRepresentation : public StepShape_ShapeRepresentation
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepShape_ShapeDimensionRepresentation();
  
  //! Initialize all fields AP214
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName, const Handle(StepRepr_HArray1OfRepresentationItem)& theItems, const Handle(StepRepr_RepresentationContext)& theContextOfItems);

  //! Initialize all fields AP242
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName, const Handle(StepShape_HArray1OfShapeDimensionRepresentationItem)& theItems, const Handle(StepRepr_RepresentationContext)& theContextOfItems);

  Standard_EXPORT void SetItemsAP242 (const Handle(StepShape_HArray1OfShapeDimensionRepresentationItem)& theItems);
  
  Standard_EXPORT Handle(StepShape_HArray1OfShapeDimensionRepresentationItem) ItemsAP242() const;
  
  DEFINE_STANDARD_RTTIEXT(StepShape_ShapeDimensionRepresentation,StepShape_ShapeRepresentation)
private:

  Handle(StepShape_HArray1OfShapeDimensionRepresentationItem) itemsAP242;
};
#endif // _StepShape_ShapeDimensionRepresentation_HeaderFile
