// Created on: 1995-01-05
// Created by: Frederic MAUPAS
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

#ifndef _StepToTopoDS_GeometricTool_HeaderFile
#define _StepToTopoDS_GeometricTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class StepGeom_SurfaceCurve;
class StepGeom_Surface;
class StepGeom_Pcurve;
class StepShape_Edge;
class StepShape_EdgeLoop;
class Geom_Curve;


//! This class contains some algorithmic services
//! specific to the mapping STEP to CAS.CADE
class StepToTopoDS_GeometricTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Integer PCurve (const Handle(StepGeom_SurfaceCurve)& SC, const Handle(StepGeom_Surface)& S, Handle(StepGeom_Pcurve)& PC, const Standard_Integer last = 0);
  
  Standard_EXPORT static Standard_Boolean IsSeamCurve (const Handle(StepGeom_SurfaceCurve)& SC, const Handle(StepGeom_Surface)& S, const Handle(StepShape_Edge)& E, const Handle(StepShape_EdgeLoop)& EL);
  
  Standard_EXPORT static Standard_Boolean IsLikeSeam (const Handle(StepGeom_SurfaceCurve)& SC, const Handle(StepGeom_Surface)& S, const Handle(StepShape_Edge)& E, const Handle(StepShape_EdgeLoop)& EL);
  
  Standard_EXPORT static Standard_Boolean UpdateParam3d (const Handle(Geom_Curve)& C, Standard_Real& w1, Standard_Real& w2, const Standard_Real preci);




protected:





private:





};







#endif // _StepToTopoDS_GeometricTool_HeaderFile
