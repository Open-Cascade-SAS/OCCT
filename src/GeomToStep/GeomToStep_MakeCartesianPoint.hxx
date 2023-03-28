// Created on: 1993-06-11
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

#ifndef _GeomToStep_MakeCartesianPoint_HeaderFile
#define _GeomToStep_MakeCartesianPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_CartesianPoint;
class gp_Pnt;
class gp_Pnt2d;
class Geom_CartesianPoint;
class Geom2d_CartesianPoint;
class StepData_StepModel;


//! This class implements the mapping between classes
//! CartesianPoint from Geom and Pnt from gp, and the class
//! CartesianPoint from StepGeom which describes a point from
//! Prostep.
class GeomToStep_MakeCartesianPoint  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeCartesianPoint(const gp_Pnt& P,
                                                const Standard_Real aFactor);
  
  Standard_EXPORT GeomToStep_MakeCartesianPoint(const gp_Pnt2d& P,
                                                const Standard_Real aFactor);
  
  Standard_EXPORT GeomToStep_MakeCartesianPoint(const Handle(Geom_CartesianPoint)& P,
                                                const Standard_Real aFactor);
  
  Standard_EXPORT GeomToStep_MakeCartesianPoint(const Handle(Geom2d_CartesianPoint)& P);
  
  Standard_EXPORT const Handle(StepGeom_CartesianPoint)& Value() const;




protected:





private:



  Handle(StepGeom_CartesianPoint) theCartesianPoint;


};







#endif // _GeomToStep_MakeCartesianPoint_HeaderFile
