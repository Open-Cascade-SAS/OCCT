// Created on: 1992-04-06
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

#ifndef _IGESData_SpecificLib_HeaderFile
#define _IGESData_SpecificLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class IGESData_NodeOfSpecificLib;
class Standard_NoSuchObject;
class IGESData_IGESEntity;
class IGESData_SpecificModule;
class IGESData_Protocol;
class IGESData_GlobalNodeOfSpecificLib;
class Standard_Transient;



class IGESData_SpecificLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void SetGlobal (const Handle(IGESData_SpecificModule)& amodule, const Handle(IGESData_Protocol)& aprotocol);
  
  Standard_EXPORT IGESData_SpecificLib(const Handle(IGESData_Protocol)& aprotocol);
  
  Standard_EXPORT IGESData_SpecificLib();
  
  Standard_EXPORT void AddProtocol (const Handle(Standard_Transient)& aprotocol);
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void SetComplete();
  
  Standard_EXPORT Standard_Boolean Select (const Handle(IGESData_IGESEntity)& obj, Handle(IGESData_SpecificModule)& module, Standard_Integer& CN) const;
  
  Standard_EXPORT void Start();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const Handle(IGESData_SpecificModule)& Module() const;
  
  Standard_EXPORT const Handle(IGESData_Protocol)& Protocol() const;




protected:





private:



  Handle(IGESData_NodeOfSpecificLib) thelist;
  Handle(IGESData_NodeOfSpecificLib) thecurr;


};







#endif // _IGESData_SpecificLib_HeaderFile
