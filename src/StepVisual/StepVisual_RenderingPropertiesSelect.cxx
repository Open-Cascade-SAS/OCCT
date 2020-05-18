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

#include <StepVisual_RenderingPropertiesSelect.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbient.hxx>
#include <StepVisual_SurfaceStyleTransparent.hxx>

//=======================================================================
//function : StepVisual_RenderingPropertiesSelect
//purpose  :
//=======================================================================

StepVisual_RenderingPropertiesSelect::StepVisual_RenderingPropertiesSelect ()
{
}

//=======================================================================
//function : CaseNum
//purpose  :
//=======================================================================

Standard_Integer StepVisual_RenderingPropertiesSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleReflectanceAmbient))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepVisual_SurfaceStyleTransparent))) return 2;
  return 0;
}

//=======================================================================
//function : SurfaceStyleReflectanceAmbient
//purpose  :
//=======================================================================

Handle(StepVisual_SurfaceStyleReflectanceAmbient) StepVisual_RenderingPropertiesSelect::SurfaceStyleReflectanceAmbient () const
{
  return Handle(StepVisual_SurfaceStyleReflectanceAmbient)::DownCast(Value());
}

//=======================================================================
//function : SurfaceStyleTransparent
//purpose  :
//=======================================================================

Handle(StepVisual_SurfaceStyleTransparent) StepVisual_RenderingPropertiesSelect::SurfaceStyleTransparent () const
{
  return Handle(StepVisual_SurfaceStyleTransparent)::DownCast(Value());
}
