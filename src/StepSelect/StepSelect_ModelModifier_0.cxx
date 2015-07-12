// Created on: 1994-12-22
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#include <StepSelect_ModelModifier.hxx>

#include <Standard_Type.hxx>

#include <StepData_StepModel.hxx>
#include <StepData_Protocol.hxx>
#include <IFSelect_ContextModif.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_CopyTool.hxx>

 








#define Handle_Model Handle(StepData_StepModel)
#define Model StepData_StepModel
#define Model_hxx <StepData_StepModel.hxx>
#define Handle_Proto Handle(StepData_Protocol)
#define Proto StepData_Protocol
#define Proto_hxx <StepData_Protocol.hxx>
#define IFSelect_ModelModifier StepSelect_ModelModifier
#define IFSelect_ModelModifier_hxx <StepSelect_ModelModifier.hxx>
#define Handle_IFSelect_ModelModifier Handle(StepSelect_ModelModifier)
#include <IFSelect_ModelModifier.gxx>

