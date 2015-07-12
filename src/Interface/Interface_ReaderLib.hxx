// Created on: 1992-02-03
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

#ifndef _Interface_ReaderLib_HeaderFile
#define _Interface_ReaderLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Interface_NodeOfReaderLib;
class Standard_NoSuchObject;
class Standard_Transient;
class Interface_ReaderModule;
class Interface_Protocol;
class Interface_GlobalNodeOfReaderLib;



class Interface_ReaderLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void SetGlobal (const Handle(Interface_ReaderModule)& amodule, const Handle(Interface_Protocol)& aprotocol);
  
  Standard_EXPORT Interface_ReaderLib(const Handle(Interface_Protocol)& aprotocol);
  
  Standard_EXPORT Interface_ReaderLib();
  
  Standard_EXPORT void AddProtocol (const Handle(Standard_Transient)& aprotocol);
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void SetComplete();
  
  Standard_EXPORT Standard_Boolean Select (const Handle(Standard_Transient)& obj, Handle(Interface_ReaderModule)& module, Standard_Integer& CN) const;
  
  Standard_EXPORT void Start();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const Handle(Interface_ReaderModule)& Module() const;
  
  Standard_EXPORT const Handle(Interface_Protocol)& Protocol() const;




protected:





private:



  Handle(Interface_NodeOfReaderLib) thelist;
  Handle(Interface_NodeOfReaderLib) thecurr;


};







#endif // _Interface_ReaderLib_HeaderFile
