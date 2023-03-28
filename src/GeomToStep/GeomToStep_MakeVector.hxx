// Created on: 1993-06-14
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _GeomToStep_MakeVector_HeaderFile
#define _GeomToStep_MakeVector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepData_Factors;
class StepGeom_Vector;
class gp_Vec;
class gp_Vec2d;
class Geom_Vector;
class Geom2d_Vector;


//! This class implements the mapping between classes
//! Vector from Geom, Geom2d and Vec, Vec2d from gp, and the class
//! Vector from StepGeom which describes a Vector from
//! Prostep.
class GeomToStep_MakeVector  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeVector(const gp_Vec& V,
                                        const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT GeomToStep_MakeVector(const gp_Vec2d& V,
                                        const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT GeomToStep_MakeVector(const Handle(Geom_Vector)& V,
                                        const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT GeomToStep_MakeVector(const Handle(Geom2d_Vector)& V,
                                        const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT const Handle(StepGeom_Vector)& Value() const;




protected:





private:



  Handle(StepGeom_Vector) theVector;


};







#endif // _GeomToStep_MakeVector_HeaderFile
