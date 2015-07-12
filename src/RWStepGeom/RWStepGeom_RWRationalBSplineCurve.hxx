// Created on: 1995-12-04
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _RWStepGeom_RWRationalBSplineCurve_HeaderFile
#define _RWStepGeom_RWRationalBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepGeom_RationalBSplineCurve;
class StepData_StepWriter;
class Interface_EntityIterator;
class Interface_ShareTool;


//! Read & Write Module for RationalBSplineCurve
//! Check added by CKY , 7-OCT-1996
class RWStepGeom_RWRationalBSplineCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT RWStepGeom_RWRationalBSplineCurve();
  
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepGeom_RationalBSplineCurve)& ent) const;
  
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepGeom_RationalBSplineCurve)& ent) const;
  
  Standard_EXPORT void Share (const Handle(StepGeom_RationalBSplineCurve)& ent, Interface_EntityIterator& iter) const;
  
  Standard_EXPORT void Check (const Handle(StepGeom_RationalBSplineCurve)& ent, const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const;




protected:





private:





};







#endif // _RWStepGeom_RWRationalBSplineCurve_HeaderFile
