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

#include <StepVisual_TextStyleForDefinedFont.hxx>
#include <StepVisual_TextStyleWithBoxCharacteristics.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TextStyleWithBoxCharacteristics, StepVisual_TextStyle)

StepVisual_TextStyleWithBoxCharacteristics::StepVisual_TextStyleWithBoxCharacteristics() {}

void StepVisual_TextStyleWithBoxCharacteristics::Init(
  const occ::handle<TCollection_HAsciiString>&                    aName,
  const occ::handle<StepVisual_TextStyleForDefinedFont>&          aCharacterAppearance,
  const occ::handle<NCollection_HArray1<StepVisual_BoxCharacteristicSelect>>& aCharacteristics)
{
  // --- classe own fields ---
  characteristics = aCharacteristics;
  // --- classe inherited fields ---
  StepVisual_TextStyle::Init(aName, aCharacterAppearance);
}

void StepVisual_TextStyleWithBoxCharacteristics::SetCharacteristics(
  const occ::handle<NCollection_HArray1<StepVisual_BoxCharacteristicSelect>>& aCharacteristics)
{
  characteristics = aCharacteristics;
}

occ::handle<NCollection_HArray1<StepVisual_BoxCharacteristicSelect>> StepVisual_TextStyleWithBoxCharacteristics::
  Characteristics() const
{
  return characteristics;
}

StepVisual_BoxCharacteristicSelect StepVisual_TextStyleWithBoxCharacteristics::CharacteristicsValue(
  const int num) const
{
  return characteristics->Value(num);
}

int StepVisual_TextStyleWithBoxCharacteristics::NbCharacteristics() const
{
  return characteristics->Length();
}
