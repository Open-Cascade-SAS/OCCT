// Created on: 2000-01-31
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


#include <Message_ListOfMsg.hxx>
#include <Message_Msg.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeExtend_MsgRegistrator,ShapeExtend_BasicMsgRegistrator)

//=======================================================================
//function : ShapeExtend_MsgRegistrator
//purpose  : 
//=======================================================================
ShapeExtend_MsgRegistrator::ShapeExtend_MsgRegistrator() : ShapeExtend_BasicMsgRegistrator()
{
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void ShapeExtend_MsgRegistrator::Send(const Handle(Standard_Transient)& object,
				      const Message_Msg& message,
				      const Message_Gravity) 
{
  if (object.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeExtend_MsgRegistrator::Send: null object" << endl;
#endif
    return;
  }
  if (myMapTransient.IsBound (object)) {
    Message_ListOfMsg& list = myMapTransient.ChangeFind (object);
    list.Append (message);
  }
  else {
    Message_ListOfMsg list;
    list.Append (message);
    myMapTransient.Bind (object, list);
  }
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

 void ShapeExtend_MsgRegistrator::Send(const TopoDS_Shape& shape,
				       const Message_Msg& message,
				       const Message_Gravity) 
{
  if (shape.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeExtend_MsgRegistrator::Send: null shape" << endl;
#endif
    return;
  }
  if (myMapShape.IsBound (shape)) {
    Message_ListOfMsg& list = myMapShape.ChangeFind (shape);
    list.Append (message);
  }
  else {
    Message_ListOfMsg list;
    list.Append (message);
    myMapShape.Bind (shape, list);
  }
}

