// Created on: 1999-02-16
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

#ifndef _StepToGeom_MakeTransformation2d_HeaderFile
#define _StepToGeom_MakeTransformation2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class StepGeom_CartesianTransformationOperator2d;
class gp_Trsf2d;


//! Convert cartesian_transformation_operator_2d to gp_Trsf2d
class StepToGeom_MakeTransformation2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean Convert (const Handle(StepGeom_CartesianTransformationOperator2d)& SCTO, gp_Trsf2d& CT);




protected:





private:





};







#endif // _StepToGeom_MakeTransformation2d_HeaderFile
