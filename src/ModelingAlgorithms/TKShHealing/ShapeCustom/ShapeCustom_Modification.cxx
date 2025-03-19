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

#include <Message_Msg.hxx>
#include <ShapeCustom_Modification.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_Modification, BRepTools_Modification)

//=================================================================================================

void ShapeCustom_Modification::SetMsgRegistrator(
  const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg)
{
  myMsgReg = msgreg;
}

//=======================================================================
// function : MsgRegistrator
// purpose  : Returns message registrator
//=======================================================================

Handle(ShapeExtend_BasicMsgRegistrator) ShapeCustom_Modification::MsgRegistrator() const
{
  return myMsgReg;
}

//=================================================================================================

void ShapeCustom_Modification::SendMsg(const TopoDS_Shape&   shape,
                                       const Message_Msg&    message,
                                       const Message_Gravity gravity) const
{
  if (!myMsgReg.IsNull())
    myMsgReg->Send(shape, message, gravity);
}
