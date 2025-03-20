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

#ifndef _StepData_NodeOfWriterLib_HeaderFile
#define _StepData_NodeOfWriterLib_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepData_GlobalNodeOfWriterLib;
class Standard_Transient;
class StepData_ReadWriteModule;
class StepData_Protocol;
class StepData_WriterLib;

class StepData_NodeOfWriterLib;
DEFINE_STANDARD_HANDLE(StepData_NodeOfWriterLib, Standard_Transient)

class StepData_NodeOfWriterLib : public Standard_Transient
{

public:
  //! Creates an empty Node, with no Next
  Standard_EXPORT StepData_NodeOfWriterLib();

  //! Adds a couple (Module,Protocol), that is, stores it into
  //! itself if not yet done, else creates a Next Node to do it
  Standard_EXPORT void AddNode(const Handle(StepData_GlobalNodeOfWriterLib)& anode);

  //! Returns the Module designated by a precise Node
  Standard_EXPORT const Handle(StepData_ReadWriteModule)& Module() const;

  //! Returns the Protocol designated by a precise Node
  Standard_EXPORT const Handle(StepData_Protocol)& Protocol() const;

  //! Returns the Next Node. If none was defined, returned value
  //! is a Null Handle
  Standard_EXPORT const Handle(StepData_NodeOfWriterLib)& Next() const;

  DEFINE_STANDARD_RTTI_INLINE(StepData_NodeOfWriterLib, Standard_Transient)

protected:
private:
  Handle(StepData_GlobalNodeOfWriterLib) thenode;
  Handle(StepData_NodeOfWriterLib)       thenext;
};

#endif // _StepData_NodeOfWriterLib_HeaderFile
