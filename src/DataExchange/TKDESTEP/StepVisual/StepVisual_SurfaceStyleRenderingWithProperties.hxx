// Created on : Thu May 14 15:13:19 2020
// Created by: Igor KHOZHANOV
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2020
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

#ifndef _StepVisual_SurfaceStyleRenderingWithProperties_HeaderFile_
#define _StepVisual_SurfaceStyleRenderingWithProperties_HeaderFile_

#include <Standard.hxx>
#include <StepVisual_SurfaceStyleRendering.hxx>

#include <StepVisual_ShadingSurfaceMethod.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_RenderingPropertiesSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! Representation of STEP entity SurfaceStyleRenderingWithProperties
class StepVisual_SurfaceStyleRenderingWithProperties : public StepVisual_SurfaceStyleRendering
{
public:
  //! default constructor
  Standard_EXPORT StepVisual_SurfaceStyleRenderingWithProperties();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const StepVisual_ShadingSurfaceMethod theSurfaceStyleRendering_RenderingMethod,
    const occ::handle<StepVisual_Colour>& theSurfaceStyleRendering_SurfaceColour,
    const occ::handle<NCollection_HArray1<StepVisual_RenderingPropertiesSelect>>& theProperties);

  //! Returns field Properties
  Standard_EXPORT occ::handle<NCollection_HArray1<StepVisual_RenderingPropertiesSelect>>
                  Properties() const;
  //! Sets field Properties
  Standard_EXPORT void SetProperties(
    const occ::handle<NCollection_HArray1<StepVisual_RenderingPropertiesSelect>>& theProperties);

  DEFINE_STANDARD_RTTIEXT(StepVisual_SurfaceStyleRenderingWithProperties,
                          StepVisual_SurfaceStyleRendering)

private:
  occ::handle<NCollection_HArray1<StepVisual_RenderingPropertiesSelect>> myProperties;
};
#endif // _StepVisual_SurfaceStyleRenderingWithProperties_HeaderFile_
