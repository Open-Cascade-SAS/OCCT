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

#include <IGESData_NodeOfWriterLib.hxx>

#include <Standard_Type.hxx>

#include <IGESData_GlobalNodeOfWriterLib.hxx>
#include <IGESData_NodeOfWriterLib.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_ReadWriteModule.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_WriterLib.hxx>

 








#define TheObject Handle(IGESData_IGESEntity)
#define TheObject_hxx <IGESData_IGESEntity.hxx>
#define Handle_TheModule Handle(IGESData_ReadWriteModule)
#define TheModule IGESData_ReadWriteModule
#define TheModule_hxx <IGESData_ReadWriteModule.hxx>
#define Handle_TheProtocol Handle(IGESData_Protocol)
#define TheProtocol IGESData_Protocol
#define TheProtocol_hxx <IGESData_Protocol.hxx>
#define LibCtl_GlobalNode IGESData_GlobalNodeOfWriterLib
#define LibCtl_GlobalNode_hxx <IGESData_GlobalNodeOfWriterLib.hxx>
#define LibCtl_Node IGESData_NodeOfWriterLib
#define LibCtl_Node_hxx <IGESData_NodeOfWriterLib.hxx>
#define Handle_LibCtl_GlobalNode Handle(IGESData_GlobalNodeOfWriterLib)
#define Handle_LibCtl_Node Handle(IGESData_NodeOfWriterLib)
#define LibCtl_Library IGESData_WriterLib
#define LibCtl_Library_hxx <IGESData_WriterLib.hxx>
#include <LibCtl_Node.gxx>

