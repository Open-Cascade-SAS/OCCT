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

#ifndef _GeomToStep_MakeDirection_HeaderFile
#define _GeomToStep_MakeDirection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_Direction;
class StdFail_NotDone;
class gp_Dir;
class gp_Dir2d;
class Geom_Direction;
class Geom2d_Direction;


//! This class implements the mapping between classes
//! Direction from Geom, Geom2d and Dir, Dir2d from gp, and the
//! class Direction from StepGeom which describes a direction
//! from Prostep.
class GeomToStep_MakeDirection  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeDirection(const gp_Dir& D);
  
  Standard_EXPORT GeomToStep_MakeDirection(const gp_Dir2d& D);
  
  Standard_EXPORT GeomToStep_MakeDirection(const Handle(Geom_Direction)& D);
  
  Standard_EXPORT GeomToStep_MakeDirection(const Handle(Geom2d_Direction)& D);
  
  Standard_EXPORT const Handle(StepGeom_Direction)& Value() const;




protected:





private:



  Handle(StepGeom_Direction) theDirection;


};







#endif // _GeomToStep_MakeDirection_HeaderFile
