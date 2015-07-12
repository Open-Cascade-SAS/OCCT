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

#ifndef _IGESData_GlobalNodeOfSpecificLib_HeaderFile
#define _IGESData_GlobalNodeOfSpecificLib_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class IGESData_SpecificModule;
class IGESData_Protocol;
class IGESData_IGESEntity;
class IGESData_SpecificLib;
class IGESData_NodeOfSpecificLib;


class IGESData_GlobalNodeOfSpecificLib;
DEFINE_STANDARD_HANDLE(IGESData_GlobalNodeOfSpecificLib, Standard_Transient)


class IGESData_GlobalNodeOfSpecificLib : public Standard_Transient
{

public:

  
  Standard_EXPORT IGESData_GlobalNodeOfSpecificLib();
  
  Standard_EXPORT void Add (const Handle(IGESData_SpecificModule)& amodule, const Handle(IGESData_Protocol)& aprotocol);
  
  Standard_EXPORT const Handle(IGESData_SpecificModule)& Module() const;
  
  Standard_EXPORT const Handle(IGESData_Protocol)& Protocol() const;
  
  Standard_EXPORT const Handle(IGESData_GlobalNodeOfSpecificLib)& Next() const;




  DEFINE_STANDARD_RTTI(IGESData_GlobalNodeOfSpecificLib,Standard_Transient)

protected:




private:


  Handle(IGESData_SpecificModule) themod;
  Handle(IGESData_Protocol) theprot;
  Handle(IGESData_GlobalNodeOfSpecificLib) thenext;


};







#endif // _IGESData_GlobalNodeOfSpecificLib_HeaderFile
