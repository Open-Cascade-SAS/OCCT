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

#ifndef _RWStepVisual_RWSurfaceStyleRenderingWithProperties_HeaderFile_
#define _RWStepVisual_RWSurfaceStyleRenderingWithProperties_HeaderFile_

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepData_StepReaderData;
class Interface_Check;
class StepData_StepWriter;
class Interface_EntityIterator;
class StepVisual_SurfaceStyleRenderingWithProperties;

//! Read & Write tool for SurfaceStyleRenderingWithProperties
class RWStepVisual_RWSurfaceStyleRenderingWithProperties
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT RWStepVisual_RWSurfaceStyleRenderingWithProperties();

  Standard_EXPORT void ReadStep(const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& ent) const;

  Standard_EXPORT void WriteStep(StepData_StepWriter& SW, const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& ent) const;

  Standard_EXPORT void Share(const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& ent, Interface_EntityIterator& iter) const;

};
#endif // _RWStepVisual_RWSurfaceStyleRenderingWithProperties_HeaderFile_
