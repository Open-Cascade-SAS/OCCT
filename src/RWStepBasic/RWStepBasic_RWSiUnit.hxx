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

#ifndef _RWStepBasic_RWSiUnit_HeaderFile
#define _RWStepBasic_RWSiUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <StepBasic_SiPrefix.hxx>
#include <Standard_CString.hxx>
#include <StepBasic_SiUnitName.hxx>
class StepData_StepReaderData;
class Interface_Check;
class StepBasic_SiUnit;
class StepData_StepWriter;
class TCollection_AsciiString;


//! Read & Write Module for SiUnit
class RWStepBasic_RWSiUnit 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT RWStepBasic_RWSiUnit();
  
  Standard_EXPORT void ReadStep (const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepBasic_SiUnit)& ent) const;
  
  Standard_EXPORT void WriteStep (StepData_StepWriter& SW, const Handle(StepBasic_SiUnit)& ent) const;
  
  Standard_EXPORT Standard_Boolean DecodePrefix (StepBasic_SiPrefix& aPrefix, const Standard_CString text) const;
  
  Standard_EXPORT Standard_Boolean DecodeName (StepBasic_SiUnitName& aName, const Standard_CString text) const;
  
  Standard_EXPORT TCollection_AsciiString EncodePrefix (const StepBasic_SiPrefix aPrefix) const;
  
  Standard_EXPORT TCollection_AsciiString EncodeName (const StepBasic_SiUnitName aName) const;




protected:





private:





};







#endif // _RWStepBasic_RWSiUnit_HeaderFile
