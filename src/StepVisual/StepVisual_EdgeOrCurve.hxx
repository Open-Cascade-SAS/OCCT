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

#ifndef _StepVisual_EdgeOrCurve_HeaderFile
#define _StepVisual_EdgeOrCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>

class Standard_Transient;
class StepGeom_Curve;
class StepShape_Edge;

//! Representation of STEP SELECT type EdgeOrCurve
class StepVisual_EdgeOrCurve : public StepData_SelectType
{

public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT StepVisual_EdgeOrCurve();

  //! Recognizes a kind of EdgeOrCurve select type
  //! -- 1 -> Curve
  //! -- 2 -> Edge
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;

  //! Returns Value as Curve (or Null if another type)
  Standard_EXPORT Handle(StepGeom_Curve) Curve() const;

  //! Returns Value as Edge (or Null if another type)
  Standard_EXPORT Handle(StepShape_Edge) Edge() const;

};
#endif // _StepVisual_EdgeOrCurve_HeaderFile
