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

#ifndef _StepVisual_SurfaceSideStyle_HeaderFile
#define _StepVisual_SurfaceSideStyle_HeaderFile

#include <Standard.hxx>

#include <StepVisual_HArray1OfSurfaceStyleElementSelect.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_SurfaceStyleElementSelect;

class StepVisual_SurfaceSideStyle;
DEFINE_STANDARD_HANDLE(StepVisual_SurfaceSideStyle, Standard_Transient)

class StepVisual_SurfaceSideStyle : public Standard_Transient
{

public:
  //! Returns a SurfaceSideStyle
  Standard_EXPORT StepVisual_SurfaceSideStyle();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&                      aName,
                            const Handle(StepVisual_HArray1OfSurfaceStyleElementSelect)& aStyles);

  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& aName);

  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  Standard_EXPORT void SetStyles(
    const Handle(StepVisual_HArray1OfSurfaceStyleElementSelect)& aStyles);

  Standard_EXPORT Handle(StepVisual_HArray1OfSurfaceStyleElementSelect) Styles() const;

  Standard_EXPORT StepVisual_SurfaceStyleElementSelect
    StylesValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbStyles() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceSideStyle, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)                      name;
  Handle(StepVisual_HArray1OfSurfaceStyleElementSelect) styles;
};

#endif // _StepVisual_SurfaceSideStyle_HeaderFile
