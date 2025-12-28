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

#include <StepVisual_CurveStyleFont.hxx>
#include <StepVisual_CurveStyleFontPattern.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CurveStyleFont, Standard_Transient)

StepVisual_CurveStyleFont::StepVisual_CurveStyleFont() {}

void StepVisual_CurveStyleFont::Init(
  const occ::handle<TCollection_HAsciiString>& aName,
  const occ::handle<NCollection_HArray1<occ::handle<StepVisual_CurveStyleFontPattern>>>&
    aPatternList)
{
  // --- classe own fields ---
  name        = aName;
  patternList = aPatternList;
}

void StepVisual_CurveStyleFont::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  name = aName;
}

occ::handle<TCollection_HAsciiString> StepVisual_CurveStyleFont::Name() const
{
  return name;
}

void StepVisual_CurveStyleFont::SetPatternList(
  const occ::handle<NCollection_HArray1<occ::handle<StepVisual_CurveStyleFontPattern>>>&
    aPatternList)
{
  patternList = aPatternList;
}

occ::handle<NCollection_HArray1<occ::handle<StepVisual_CurveStyleFontPattern>>>
  StepVisual_CurveStyleFont::PatternList() const
{
  return patternList;
}

occ::handle<StepVisual_CurveStyleFontPattern> StepVisual_CurveStyleFont::PatternListValue(
  const int num) const
{
  return patternList->Value(num);
}

int StepVisual_CurveStyleFont::NbPatternList() const
{
  if (patternList.IsNull())
    return 0;
  return patternList->Length();
}
