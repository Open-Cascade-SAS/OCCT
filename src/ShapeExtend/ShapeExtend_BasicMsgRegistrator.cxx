// File:	ShapeExtend_BasicMsgRegistrator.cxx
// Created:	Fri Jan 28 19:52:05 2000
// Author:	data exchange team
//		<det@kinox>


#include <ShapeExtend_BasicMsgRegistrator.ixx>

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

