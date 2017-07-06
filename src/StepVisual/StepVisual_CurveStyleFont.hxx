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

#include <StepVisual_HArray1OfCurveStyleFontPattern.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_CurveStyleFontPattern;


class StepVisual_CurveStyleFont;
DEFINE_STANDARD_HANDLE(StepVisual_CurveStyleFont, Standard_Transient)


class StepVisual_CurveStyleFont : public Standard_Transient
{

public:

  
  //! Returns a CurveStyleFont
  Standard_EXPORT StepVisual_CurveStyleFont();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepVisual_HArray1OfCurveStyleFontPattern)& aPatternList);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetPatternList (const Handle(StepVisual_HArray1OfCurveStyleFontPattern)& aPatternList);
  
  Standard_EXPORT Handle(StepVisual_HArray1OfCurveStyleFontPattern) PatternList() const;
  
  Standard_EXPORT Handle(StepVisual_CurveStyleFontPattern) PatternListValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbPatternList() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_CurveStyleFont,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(StepVisual_HArray1OfCurveStyleFontPattern) patternList;


};







#endif // _StepVisual_CurveStyleFont_HeaderFile
