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

#ifndef _StepShape_DimensionalCharacteristicRepresentation_HeaderFile
#define _StepShape_DimensionalCharacteristicRepresentation_HeaderFile

#include <Standard.hxx>

#include <StepShape_DimensionalCharacteristic.hxx>
#include <Standard_Transient.hxx>
class StepShape_ShapeDimensionRepresentation;

class StepShape_DimensionalCharacteristicRepresentation;
DEFINE_STANDARD_HANDLE(StepShape_DimensionalCharacteristicRepresentation, Standard_Transient)

//! Representation of STEP entity DimensionalCharacteristicRepresentation
class StepShape_DimensionalCharacteristicRepresentation : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_DimensionalCharacteristicRepresentation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const StepShape_DimensionalCharacteristic&            aDimension,
                            const Handle(StepShape_ShapeDimensionRepresentation)& aRepresentation);

  //! Returns field Dimension
  Standard_EXPORT StepShape_DimensionalCharacteristic Dimension() const;

  //! Set field Dimension
  Standard_EXPORT void SetDimension(const StepShape_DimensionalCharacteristic& Dimension);

  //! Returns field Representation
  Standard_EXPORT Handle(StepShape_ShapeDimensionRepresentation) Representation() const;

  //! Set field Representation
  Standard_EXPORT void SetRepresentation(
    const Handle(StepShape_ShapeDimensionRepresentation)& Representation);

  DEFINE_STANDARD_RTTIEXT(StepShape_DimensionalCharacteristicRepresentation, Standard_Transient)

protected:
private:
  StepShape_DimensionalCharacteristic            theDimension;
  Handle(StepShape_ShapeDimensionRepresentation) theRepresentation;
};

#endif // _StepShape_DimensionalCharacteristicRepresentation_HeaderFile
