// Created on: 2000-01-28
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <Message_Msg.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeExtend_BasicMsgRegistrator,Standard_Transient)

//=======================================================================
//function : ShapeExtend_BasicMsgRegistrator
//purpose  : 
//=======================================================================
ShapeExtend_BasicMsgRegistrator::ShapeExtend_BasicMsgRegistrator()
{
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

 void ShapeExtend_BasicMsgRegistrator::Send(const Handle(Standard_Transient)& /*object*/,
					    const Message_Msg& /*message*/,
					    const Message_Gravity /*gravity*/) 
{
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

 void ShapeExtend_BasicMsgRegistrator::Send(const TopoDS_Shape& /*shape*/,
					    const Message_Msg& /*message*/,
					    const Message_Gravity /*gravity*/) 
{
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

 void ShapeExtend_BasicMsgRegistrator::Send(const Message_Msg& message,
					    const Message_Gravity gravity) 
{
  Handle(Standard_Transient) dummy;
  Send (dummy, message, gravity);
}

