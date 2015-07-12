// Created on: 1995-05-04
// Created by: Dieter THIEMANN
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

#ifndef _GeomToStep_MakeHyperbola_HeaderFile
#define _GeomToStep_MakeHyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_Hyperbola;
class StdFail_NotDone;
class Geom2d_Hyperbola;
class Geom_Hyperbola;


//! This class implements the mapping between the class
//! Hyperbola from Geom and the class Hyperbola from
//! StepGeom which describes a Hyperbola from ProSTEP
class GeomToStep_MakeHyperbola  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeHyperbola(const Handle(Geom2d_Hyperbola)& C);
  
  Standard_EXPORT GeomToStep_MakeHyperbola(const Handle(Geom_Hyperbola)& C);
  
  Standard_EXPORT const Handle(StepGeom_Hyperbola)& Value() const;




protected:





private:



  Handle(StepGeom_Hyperbola) theHyperbola;


};







#endif // _GeomToStep_MakeHyperbola_HeaderFile
