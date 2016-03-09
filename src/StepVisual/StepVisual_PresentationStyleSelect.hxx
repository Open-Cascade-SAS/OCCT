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

#ifndef _StepVisual_PresentationStyleSelect_HeaderFile
#define _StepVisual_PresentationStyleSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepVisual_PointStyle;
class StepVisual_CurveStyle;
class StepVisual_NullStyleMember;
class StepVisual_SurfaceStyleUsage;



class StepVisual_PresentationStyleSelect  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a PresentationStyleSelect SelectType
  Standard_EXPORT StepVisual_PresentationStyleSelect();
  
  //! Recognizes a PresentationStyleSelect Kind Entity that is :
  //! 1 -> PointStyle
  //! 2 -> CurveStyle
  //! 3 -> SurfaceStyleUsage
  //! 4 -> SymbolStyle
  //! 5 -> FillAreaStyle
  //! 6 -> TextStyle
  //! 7 -> NullStyle
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! returns Value as a PointStyle (Null if another type)
  Standard_EXPORT Handle(StepVisual_PointStyle) PointStyle() const;
  
  //! returns Value as a CurveStyle (Null if another type)
  Standard_EXPORT Handle(StepVisual_CurveStyle) CurveStyle() const;

  //! returns Value as a NullStyleMember (Null if another type)
  Standard_EXPORT Handle(StepVisual_NullStyleMember) NullStyle() const;
  
  //! returns Value as a SurfaceStyleUsage (Null if another type)
  Standard_EXPORT Handle(StepVisual_SurfaceStyleUsage) SurfaceStyleUsage() const;




protected:





private:





};







#endif // _StepVisual_PresentationStyleSelect_HeaderFile
