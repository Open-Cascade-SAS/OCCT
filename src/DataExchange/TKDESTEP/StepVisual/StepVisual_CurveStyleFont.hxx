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

#ifndef _StepVisual_CurveStyleFont_HeaderFile
#define _StepVisual_CurveStyleFont_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_CurveStyleFontPattern.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_CurveStyleFontPattern;

class StepVisual_CurveStyleFont : public Standard_Transient
{

public:
  //! Returns a CurveStyleFont
  Standard_EXPORT StepVisual_CurveStyleFont();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aName,
    const occ::handle<NCollection_HArray1<occ::handle<StepVisual_CurveStyleFontPattern>>>&
      aPatternList);

  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& aName);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  Standard_EXPORT void SetPatternList(
    const occ::handle<NCollection_HArray1<occ::handle<StepVisual_CurveStyleFontPattern>>>&
      aPatternList);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepVisual_CurveStyleFontPattern>>>
                  PatternList() const;

  Standard_EXPORT occ::handle<StepVisual_CurveStyleFontPattern> PatternListValue(
    const int num) const;

  Standard_EXPORT int NbPatternList() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_CurveStyleFont, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>                                           name;
  occ::handle<NCollection_HArray1<occ::handle<StepVisual_CurveStyleFontPattern>>> patternList;
};

#endif // _StepVisual_CurveStyleFont_HeaderFile
