// Created on: 1993-06-22
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

#ifndef _GeomToStep_MakeSweptSurface_HeaderFile
#define _GeomToStep_MakeSweptSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_SweptSurface;
class Geom_SweptSurface;


//! This class implements the mapping between classes
//! SweptSurface from Geom and the class SweptSurface from
//! StepGeom which describes a SweptSurface from prostep.
//! As SweptSurface is an abstract SweptSurface this class
//! is an access to the sub-class required.
class GeomToStep_MakeSweptSurface  : public GeomToStep_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToStep_MakeSweptSurface(const Handle(Geom_SweptSurface)& S);
  
  Standard_EXPORT const Handle(StepGeom_SweptSurface)& Value() const;




protected:





private:



  Handle(StepGeom_SweptSurface) theSweptSurface;


};







#endif // _GeomToStep_MakeSweptSurface_HeaderFile
