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

#ifndef _StepVisual_RenderingPropertiesSelect_HeaderFile
#define _StepVisual_RenderingPropertiesSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepVisual_SurfaceStyleReflectanceAmbient;
class StepVisual_SurfaceStyleTransparent;

//! Representation of STEP SELECT type RenderingPropertiesSelect
class StepVisual_RenderingPropertiesSelect : public StepData_SelectType
{

public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepVisual_RenderingPropertiesSelect();

  //! Recognizes a kind of RenderingPropertiesSelect select type
  //! -- 1 -> SurfaceStyleReflectanceAmbient
  //! -- 2 -> SurfaceStyleTransparent
  Standard_EXPORT Standard_Integer
    CaseNum(const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Returns Value as SurfaceStyleReflectanceAmbient (or Null if another type)
  Standard_EXPORT Handle(StepVisual_SurfaceStyleReflectanceAmbient) SurfaceStyleReflectanceAmbient()
    const;

  //! Returns Value as SurfaceStyleTransparent (or Null if another type)
  Standard_EXPORT Handle(StepVisual_SurfaceStyleTransparent) SurfaceStyleTransparent() const;
};
#endif // _StepVisual_RenderingPropertiesSelect_HeaderFile
