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

#ifndef _IGESData_NodeOfWriterLib_HeaderFile
#define _IGESData_NodeOfWriterLib_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
class IGESData_GlobalNodeOfWriterLib;
class IGESData_IGESEntity;
class IGESData_ReadWriteModule;
class IGESData_Protocol;
class IGESData_WriterLib;


class IGESData_NodeOfWriterLib;
DEFINE_STANDARD_HANDLE(IGESData_NodeOfWriterLib, MMgt_TShared)


class IGESData_NodeOfWriterLib : public MMgt_TShared
{

public:

  
  Standard_EXPORT IGESData_NodeOfWriterLib();
  
  Standard_EXPORT void AddNode (const Handle(IGESData_GlobalNodeOfWriterLib)& anode);
  
  Standard_EXPORT const Handle(IGESData_ReadWriteModule)& Module() const;
  
  Standard_EXPORT const Handle(IGESData_Protocol)& Protocol() const;
  
  Standard_EXPORT const Handle(IGESData_NodeOfWriterLib)& Next() const;




  DEFINE_STANDARD_RTTI(IGESData_NodeOfWriterLib,MMgt_TShared)

protected:




private:


  Handle(IGESData_GlobalNodeOfWriterLib) thenode;
  Handle(IGESData_NodeOfWriterLib) thenext;


};







#endif // _IGESData_NodeOfWriterLib_HeaderFile
