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

#ifndef _StepVisual_CurveStyle_HeaderFile
#define _StepVisual_CurveStyle_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_CurveStyleFontSelect.hxx>
#include <StepBasic_SizeSelect.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepVisual_Colour;


class StepVisual_CurveStyle;
DEFINE_STANDARD_HANDLE(StepVisual_CurveStyle, Standard_Transient)


class StepVisual_CurveStyle : public Standard_Transient
{

public:

  
  //! Returns a CurveStyle
  Standard_EXPORT StepVisual_CurveStyle();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const StepVisual_CurveStyleFontSelect& aCurveFont, const StepBasic_SizeSelect& aCurveWidth, const Handle(StepVisual_Colour)& aCurveColour);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetCurveFont (const StepVisual_CurveStyleFontSelect& aCurveFont);
  
  Standard_EXPORT StepVisual_CurveStyleFontSelect CurveFont() const;
  
  Standard_EXPORT void SetCurveWidth (const StepBasic_SizeSelect& aCurveWidth);
  
  Standard_EXPORT StepBasic_SizeSelect CurveWidth() const;
  
  Standard_EXPORT void SetCurveColour (const Handle(StepVisual_Colour)& aCurveColour);
  
  Standard_EXPORT Handle(StepVisual_Colour) CurveColour() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_CurveStyle,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  StepVisual_CurveStyleFontSelect curveFont;
  StepBasic_SizeSelect curveWidth;
  Handle(StepVisual_Colour) curveColour;


};







#endif // _StepVisual_CurveStyle_HeaderFile
