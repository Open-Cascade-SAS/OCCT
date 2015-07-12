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

#ifndef _StepToGeom_MakeConic2d_HeaderFile
#define _StepToGeom_MakeConic2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class StepGeom_Conic;
class Geom2d_Conic;


//! This class implements the mapping between classes
//! Conic from StepGeom
//! which describes a Conic from prostep and Conic from Geom2d.
//! As Conic is an abstract class
//! this class is an access to the sub-class required.
class StepToGeom_MakeConic2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean Convert (const Handle(StepGeom_Conic)& SC, Handle(Geom2d_Conic)& CC);




protected:





private:





};







#endif // _StepToGeom_MakeConic2d_HeaderFile
