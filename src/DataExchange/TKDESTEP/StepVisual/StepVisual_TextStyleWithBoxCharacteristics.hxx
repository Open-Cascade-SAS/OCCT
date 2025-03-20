// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepVisual_TextStyleWithBoxCharacteristics_HeaderFile
#define _StepVisual_TextStyleWithBoxCharacteristics_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_HArray1OfBoxCharacteristicSelect.hxx>
#include <StepVisual_TextStyle.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_TextStyleForDefinedFont;
class StepVisual_BoxCharacteristicSelect;

class StepVisual_TextStyleWithBoxCharacteristics;
DEFINE_STANDARD_HANDLE(StepVisual_TextStyleWithBoxCharacteristics, StepVisual_TextStyle)

class StepVisual_TextStyleWithBoxCharacteristics : public StepVisual_TextStyle
{

public:
  //! Returns a TextStyleWithBoxCharacteristics
  Standard_EXPORT StepVisual_TextStyleWithBoxCharacteristics();

  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&                    aName,
    const Handle(StepVisual_TextStyleForDefinedFont)&          aCharacterAppearance,
    const Handle(StepVisual_HArray1OfBoxCharacteristicSelect)& aCharacteristics);

  Standard_EXPORT void SetCharacteristics(
    const Handle(StepVisual_HArray1OfBoxCharacteristicSelect)& aCharacteristics);

  Standard_EXPORT Handle(StepVisual_HArray1OfBoxCharacteristicSelect) Characteristics() const;

  Standard_EXPORT StepVisual_BoxCharacteristicSelect
    CharacteristicsValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbCharacteristics() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TextStyleWithBoxCharacteristics, StepVisual_TextStyle)

protected:
private:
  Handle(StepVisual_HArray1OfBoxCharacteristicSelect) characteristics;
};

#endif // _StepVisual_TextStyleWithBoxCharacteristics_HeaderFile
