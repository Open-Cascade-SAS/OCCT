// Created on: 1992-02-11
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _StepData_WriterLib_HeaderFile
#define _StepData_WriterLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class StepData_NodeOfWriterLib;
class Standard_NoSuchObject;
class Standard_Transient;
class StepData_ReadWriteModule;
class StepData_Protocol;
class StepData_GlobalNodeOfWriterLib;



class StepData_WriterLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void SetGlobal (const Handle(StepData_ReadWriteModule)& amodule, const Handle(StepData_Protocol)& aprotocol);
  
  Standard_EXPORT StepData_WriterLib(const Handle(StepData_Protocol)& aprotocol);
  
  Standard_EXPORT StepData_WriterLib();
  
  Standard_EXPORT void AddProtocol (const Handle(Standard_Transient)& aprotocol);
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void SetComplete();
  
  Standard_EXPORT Standard_Boolean Select (const Handle(Standard_Transient)& obj, Handle(StepData_ReadWriteModule)& module, Standard_Integer& CN) const;
  
  Standard_EXPORT void Start();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const Handle(StepData_ReadWriteModule)& Module() const;
  
  Standard_EXPORT const Handle(StepData_Protocol)& Protocol() const;




protected:





private:



  Handle(StepData_NodeOfWriterLib) thelist;
  Handle(StepData_NodeOfWriterLib) thecurr;


};







#endif // _StepData_WriterLib_HeaderFile
