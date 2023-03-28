// Created on: 1994-08-26
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _GeomToStep_MakeAxis2Placement2d_HeaderFile
#define _GeomToStep_MakeAxis2Placement2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepData_Factors;
class StepGeom_Axis2Placement2d;
class gp_Ax2;
class gp_Ax22d;


//! This class implements the mapping between classes
//! Axis2Placement from Geom and Ax2, Ax22d from gp, and the class
//! Axis2Placement2d from StepGeom which describes an
//! axis2_placement_2d from Prostep.
class GeomToStep_MakeAxis2Placement2d  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeAxis2Placement2d(const gp_Ax2& A,
                                                  const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT GeomToStep_MakeAxis2Placement2d(const gp_Ax22d& A,
                                                  const StepData_Factors& theLocalFactors);
  
  Standard_EXPORT const Handle(StepGeom_Axis2Placement2d)& Value() const;




protected:





private:



  Handle(StepGeom_Axis2Placement2d) theAxis2Placement2d;


};







#endif // _GeomToStep_MakeAxis2Placement2d_HeaderFile
