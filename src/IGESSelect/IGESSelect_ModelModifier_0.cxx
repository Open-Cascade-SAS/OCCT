// Created on: 1994-05-31
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

#include <IGESSelect_ModelModifier.hxx>

#include <Standard_Type.hxx>

#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <IFSelect_ContextModif.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_CopyTool.hxx>

 








#define Handle_Model Handle(IGESData_IGESModel)
#define Model IGESData_IGESModel
#define Model_hxx <IGESData_IGESModel.hxx>
#define Handle_Proto Handle(IGESData_Protocol)
#define Proto IGESData_Protocol
#define Proto_hxx <IGESData_Protocol.hxx>
#define IFSelect_ModelModifier IGESSelect_ModelModifier
#define IFSelect_ModelModifier_hxx <IGESSelect_ModelModifier.hxx>
#define Handle_IFSelect_ModelModifier Handle(IGESSelect_ModelModifier)
#include <IFSelect_ModelModifier.gxx>

