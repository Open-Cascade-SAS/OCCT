// Created on : Thu Mar 24 18:30:12 2022 
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#ifndef _StepVisual_PathOrCompositeCurve_HeaderFile
#define _StepVisual_PathOrCompositeCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepGeom_CompositeCurve;
class StepShape_Path;

//! Representation of STEP SELECT type PathOrCompositeCurve
class StepVisual_PathOrCompositeCurve : public StepData_SelectType
{

public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepVisual_PathOrCompositeCurve();

  //! Recognizes a kind of PathOrCompositeCurve select type
  //! -- 1 -> CompositeCurve
  //! -- 2 -> Path
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Returns Value as CompositeCurve (or Null if another type)
  Standard_EXPORT Handle(StepGeom_CompositeCurve) CompositeCurve() const;

  //! Returns Value as Path (or Null if another type)
  Standard_EXPORT Handle(StepShape_Path) Path() const;

};
#endif // _StepVisual_PathOrCompositeCurve_HeaderFile
