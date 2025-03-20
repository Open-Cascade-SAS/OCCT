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

#ifndef _StepVisual_TextLiteral_HeaderFile
#define _StepVisual_TextLiteral_HeaderFile

#include <Standard.hxx>

#include <StepGeom_Axis2Placement.hxx>
#include <StepVisual_TextPath.hxx>
#include <StepVisual_FontSelect.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class TCollection_HAsciiString;

class StepVisual_TextLiteral;
DEFINE_STANDARD_HANDLE(StepVisual_TextLiteral, StepGeom_GeometricRepresentationItem)

class StepVisual_TextLiteral : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a TextLiteral
  Standard_EXPORT StepVisual_TextLiteral();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(TCollection_HAsciiString)& aLiteral,
                            const StepGeom_Axis2Placement&          aPlacement,
                            const Handle(TCollection_HAsciiString)& aAlignment,
                            const StepVisual_TextPath               aPath,
                            const StepVisual_FontSelect&            aFont);

  Standard_EXPORT void SetLiteral(const Handle(TCollection_HAsciiString)& aLiteral);

  Standard_EXPORT Handle(TCollection_HAsciiString) Literal() const;

  Standard_EXPORT void SetPlacement(const StepGeom_Axis2Placement& aPlacement);

  Standard_EXPORT StepGeom_Axis2Placement Placement() const;

  Standard_EXPORT void SetAlignment(const Handle(TCollection_HAsciiString)& aAlignment);

  Standard_EXPORT Handle(TCollection_HAsciiString) Alignment() const;

  Standard_EXPORT void SetPath(const StepVisual_TextPath aPath);

  Standard_EXPORT StepVisual_TextPath Path() const;

  Standard_EXPORT void SetFont(const StepVisual_FontSelect& aFont);

  Standard_EXPORT StepVisual_FontSelect Font() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TextLiteral, StepGeom_GeometricRepresentationItem)

protected:
private:
  Handle(TCollection_HAsciiString) literal;
  StepGeom_Axis2Placement          placement;
  Handle(TCollection_HAsciiString) alignment;
  StepVisual_TextPath              path;
  StepVisual_FontSelect            font;
};

#endif // _StepVisual_TextLiteral_HeaderFile
