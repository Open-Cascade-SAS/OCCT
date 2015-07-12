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

#include <StepData_NodeOfWriterLib.hxx>

#include <Standard_Type.hxx>

#include <StepData_GlobalNodeOfWriterLib.hxx>
#include <StepData_NodeOfWriterLib.hxx>
#include <Standard_Transient.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_WriterLib.hxx>

 








#define TheObject Handle(Standard_Transient)
#define TheObject_hxx <Standard_Transient.hxx>
#define Handle_TheModule Handle(StepData_ReadWriteModule)
#define TheModule StepData_ReadWriteModule
#define TheModule_hxx <StepData_ReadWriteModule.hxx>
#define Handle_TheProtocol Handle(StepData_Protocol)
#define TheProtocol StepData_Protocol
#define TheProtocol_hxx <StepData_Protocol.hxx>
#define LibCtl_GlobalNode StepData_GlobalNodeOfWriterLib
#define LibCtl_GlobalNode_hxx <StepData_GlobalNodeOfWriterLib.hxx>
#define LibCtl_Node StepData_NodeOfWriterLib
#define LibCtl_Node_hxx <StepData_NodeOfWriterLib.hxx>
#define Handle_LibCtl_GlobalNode Handle(StepData_GlobalNodeOfWriterLib)
#define Handle_LibCtl_Node Handle(StepData_NodeOfWriterLib)
#define LibCtl_Library StepData_WriterLib
#define LibCtl_Library_hxx <StepData_WriterLib.hxx>
#include <LibCtl_Node.gxx>

