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

#ifndef _StepVisual_PointStyle_HeaderFile
#define _StepVisual_PointStyle_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_MarkerSelect.hxx>
#include <StepBasic_SizeSelect.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepVisual_Colour;

class StepVisual_PointStyle;
DEFINE_STANDARD_HANDLE(StepVisual_PointStyle, Standard_Transient)

class StepVisual_PointStyle : public Standard_Transient
{

public:
  //! Returns a PointStyle
  Standard_EXPORT StepVisual_PointStyle();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const StepVisual_MarkerSelect&          aMarker,
                            const StepBasic_SizeSelect&             aMarkerSize,
                            const Handle(StepVisual_Colour)&        aMarkerColour);

  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& aName);

  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  Standard_EXPORT void SetMarker(const StepVisual_MarkerSelect& aMarker);

  Standard_EXPORT StepVisual_MarkerSelect Marker() const;

  Standard_EXPORT void SetMarkerSize(const StepBasic_SizeSelect& aMarkerSize);

  Standard_EXPORT StepBasic_SizeSelect MarkerSize() const;

  Standard_EXPORT void SetMarkerColour(const Handle(StepVisual_Colour)& aMarkerColour);

  Standard_EXPORT Handle(StepVisual_Colour) MarkerColour() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PointStyle, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString) name;
  StepVisual_MarkerSelect          marker;
  StepBasic_SizeSelect             markerSize;
  Handle(StepVisual_Colour)        markerColour;
};

#endif // _StepVisual_PointStyle_HeaderFile
