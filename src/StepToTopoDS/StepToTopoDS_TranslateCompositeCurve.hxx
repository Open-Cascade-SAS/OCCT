// Created on: 1999-02-12
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

#ifndef _StepToTopoDS_TranslateCompositeCurve_HeaderFile
#define _StepToTopoDS_TranslateCompositeCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Wire.hxx>
#include <StepToTopoDS_Root.hxx>
class StepGeom_CompositeCurve;
class Transfer_TransientProcess;
class StepGeom_Surface;
class Geom_Surface;


//! Translate STEP entity composite_curve to TopoDS_Wire
//! If surface is given, the curve is assumed to lie on that
//! surface and in case if any segment of it is a
//! curve_on_surface, the pcurve for that segment will be taken.
//! Note: a segment of composite_curve may be itself
//! composite_curve. Only one-level protection against
//! cyclic references is implemented.
class StepToTopoDS_TranslateCompositeCurve  : public StepToTopoDS_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepToTopoDS_TranslateCompositeCurve();
  
  //! Translates standalone composite_curve
  Standard_EXPORT StepToTopoDS_TranslateCompositeCurve(const Handle(StepGeom_CompositeCurve)& CC,
                                                       const Handle(Transfer_TransientProcess)& TP,
                                                       const StepData_Factors& theLocalFactors);
  
  //! Translates composite_curve lying on surface
  Standard_EXPORT StepToTopoDS_TranslateCompositeCurve(const Handle(StepGeom_CompositeCurve)& CC,
                                                       const Handle(Transfer_TransientProcess)& TP,
                                                       const Handle(StepGeom_Surface)& S,
                                                       const Handle(Geom_Surface)& Surf,
                                                       const StepData_Factors& theLocalFactors);
  
  //! Translates standalone composite_curve
  Standard_EXPORT Standard_Boolean Init (const Handle(StepGeom_CompositeCurve)& CC,
                                         const Handle(Transfer_TransientProcess)& TP,
                                         const StepData_Factors& theLocalFactors);
  
  //! Translates composite_curve lying on surface
  Standard_EXPORT Standard_Boolean Init (const Handle(StepGeom_CompositeCurve)& CC,
                                         const Handle(Transfer_TransientProcess)& TP,
                                         const Handle(StepGeom_Surface)& S,
                                         const Handle(Geom_Surface)& Surf,
                                         const StepData_Factors& theLocalFactors);
  
  //! Returns result of last translation or null wire if failed.
  Standard_EXPORT const TopoDS_Wire& Value() const;
  
  //! Returns True if composite_curve contains a segment with infinite parameters.
    Standard_Boolean IsInfiniteSegment() const;




protected:





private:



  TopoDS_Wire myWire;
  Standard_Boolean myInfiniteSegment;


};


#include <StepToTopoDS_TranslateCompositeCurve.lxx>





#endif // _StepToTopoDS_TranslateCompositeCurve_HeaderFile
