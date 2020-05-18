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

#ifndef _StepVisual_SurfaceStyleElementSelect_HeaderFile
#define _StepVisual_SurfaceStyleElementSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepVisual_SurfaceStyleFillArea;
class StepVisual_SurfaceStyleBoundary;
class StepVisual_SurfaceStyleParameterLine;
class StepVisual_SurfaceStyleRendering;


class StepVisual_SurfaceStyleElementSelect  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a SurfaceStyleElementSelect SelectType
  Standard_EXPORT StepVisual_SurfaceStyleElementSelect();
  
  //! Recognizes a SurfaceStyleElementSelect Kind Entity that is :
  //! 1 -> SurfaceStyleFillArea
  //! 2 -> SurfaceStyleBoundary
  //! 3 -> SurfaceStyleParameterLine
  //! 4 -> SurfaceStyleSilhouette
  //! 5 -> SurfaceStyleSegmentationCurve
  //! 6 -> SurfaceStyleControlGrid
  //! 7 -> SurfaceStyleRendering
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! returns Value as a SurfaceStyleFillArea (Null if another type)
  Standard_EXPORT Handle(StepVisual_SurfaceStyleFillArea) SurfaceStyleFillArea() const;
  
  //! returns Value as a SurfaceStyleBoundary (Null if another type)
  Standard_EXPORT Handle(StepVisual_SurfaceStyleBoundary) SurfaceStyleBoundary() const;
  
  //! returns Value as a SurfaceStyleParameterLine (Null if another type)
  Standard_EXPORT Handle(StepVisual_SurfaceStyleParameterLine) SurfaceStyleParameterLine() const;

  //! returns Value as a SurfaceStyleRendering (Null if another type)
  Standard_EXPORT Handle(StepVisual_SurfaceStyleRendering) SurfaceStyleRendering() const;



protected:





private:





};







#endif // _StepVisual_SurfaceStyleElementSelect_HeaderFile
