// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepGeom_SurfaceBoundary_HeaderFile
#define _StepGeom_SurfaceBoundary_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class StepGeom_BoundaryCurve;
class StepGeom_DegeneratePcurve;


//! Representation of STEP SELECT type SurfaceBoundary
class StepGeom_SurfaceBoundary  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepGeom_SurfaceBoundary();
  
  //! Recognizes a kind of SurfaceBoundary select type
  //! 1 -> BoundaryCurve from StepGeom
  //! 2 -> DegeneratePcurve from StepGeom
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! Returns Value as BoundaryCurve (or Null if another type)
  Standard_EXPORT Handle(StepGeom_BoundaryCurve) BoundaryCurve() const;
  
  //! Returns Value as DegeneratePcurve (or Null if another type)
  Standard_EXPORT Handle(StepGeom_DegeneratePcurve) DegeneratePcurve() const;




protected:





private:





};







#endif // _StepGeom_SurfaceBoundary_HeaderFile
