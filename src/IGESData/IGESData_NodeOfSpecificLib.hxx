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

#ifndef _IGESData_NodeOfSpecificLib_HeaderFile
#define _IGESData_NodeOfSpecificLib_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class IGESData_GlobalNodeOfSpecificLib;
class IGESData_IGESEntity;
class IGESData_SpecificModule;
class IGESData_Protocol;
class IGESData_SpecificLib;


class IGESData_NodeOfSpecificLib;
DEFINE_STANDARD_HANDLE(IGESData_NodeOfSpecificLib, Standard_Transient)


class IGESData_NodeOfSpecificLib : public Standard_Transient
{

public:

  
  //! Creates an empty Node, with no Next
  Standard_EXPORT IGESData_NodeOfSpecificLib();
  
  //! Adds a couple (Module,Protocol), that is, stores it into
  //! itself if not yet done, else creates a Next Node to do it
  Standard_EXPORT void AddNode (const Handle(IGESData_GlobalNodeOfSpecificLib)& anode);
  
  //! Returns the Module designated by a precise Node
  Standard_EXPORT const Handle(IGESData_SpecificModule)& Module() const;
  
  //! Returns the Protocol designated by a precise Node
  Standard_EXPORT const Handle(IGESData_Protocol)& Protocol() const;
  
  //! Returns the Next Node. If none was defined, returned value
  //! is a Null Handle
  Standard_EXPORT const Handle(IGESData_NodeOfSpecificLib)& Next() const;




  DEFINE_STANDARD_RTTI_INLINE(IGESData_NodeOfSpecificLib,Standard_Transient)

protected:




private:


  Handle(IGESData_GlobalNodeOfSpecificLib) thenode;
  Handle(IGESData_NodeOfSpecificLib) thenext;


};







#endif // _IGESData_NodeOfSpecificLib_HeaderFile
