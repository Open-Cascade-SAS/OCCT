// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _ShapeCustom_Modification_HeaderFile
#define _ShapeCustom_Modification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepTools_Modification.hxx>
#include <Message_Gravity.hxx>
class ShapeExtend_BasicMsgRegistrator;
class TopoDS_Shape;
class Message_Msg;


class ShapeCustom_Modification;
DEFINE_STANDARD_HANDLE(ShapeCustom_Modification, BRepTools_Modification)

//! A base class of Modification's from ShapeCustom.
//! Implements message sending mechanism.
class ShapeCustom_Modification : public BRepTools_Modification
{

public:

  
  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator (const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg);
  
  //! Returns message registrator
  Standard_EXPORT Handle(ShapeExtend_BasicMsgRegistrator) MsgRegistrator() const;
  
  //! Sends a message to be attached to the shape.
  //! Calls corresponding message of message registrator.
  Standard_EXPORT void SendMsg (const TopoDS_Shape& shape, const Message_Msg& message, const Message_Gravity gravity = Message_Info) const;




  DEFINE_STANDARD_RTTIEXT(ShapeCustom_Modification,BRepTools_Modification)

protected:




private:


  Handle(ShapeExtend_BasicMsgRegistrator) myMsgReg;


};







#endif // _ShapeCustom_Modification_HeaderFile
