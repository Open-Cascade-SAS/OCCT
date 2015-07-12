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

#ifndef _GeomToStep_MakeCircle_HeaderFile
#define _GeomToStep_MakeCircle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_Circle;
class StdFail_NotDone;
class gp_Circ;
class Geom_Circle;
class Geom2d_Circle;


//! This class implements the mapping between classes
//! Circle from Geom, and Circ from gp, and the class
//! Circle from StepGeom which describes a circle from
//! Prostep.
class GeomToStep_MakeCircle  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeCircle(const gp_Circ& C);
  
  Standard_EXPORT GeomToStep_MakeCircle(const Handle(Geom_Circle)& C);
  
  Standard_EXPORT GeomToStep_MakeCircle(const Handle(Geom2d_Circle)& C);
  
  Standard_EXPORT const Handle(StepGeom_Circle)& Value() const;




protected:





private:



  Handle(StepGeom_Circle) theCircle;


};







#endif // _GeomToStep_MakeCircle_HeaderFile
