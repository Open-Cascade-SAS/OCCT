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

#ifndef _ShapeExtend_BasicMsgRegistrator_HeaderFile
#define _ShapeExtend_BasicMsgRegistrator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Message_Gravity.hxx>
class Message_Msg;
class TopoDS_Shape;


class ShapeExtend_BasicMsgRegistrator;
DEFINE_STANDARD_HANDLE(ShapeExtend_BasicMsgRegistrator, Standard_Transient)

//! Abstract class that can be used for attaching messages
//! to the objects (e.g. shapes).
//! It is used by ShapeHealing algorithms to attach a message
//! describing encountered case (e.g. removing small edge from
//! a wire).
//!
//! The methods of this class are empty and redefined, for instance,
//! in the classes for Data Exchange processors for attaching
//! messages to interface file entities or CAS.CADE shapes.
class ShapeExtend_BasicMsgRegistrator : public Standard_Transient
{

public:

  
  //! Empty constructor.
  Standard_EXPORT ShapeExtend_BasicMsgRegistrator();
  
  //! Sends a message to be attached to the object.
  //! Object can be of any type interpreted by redefined MsgRegistrator.
  Standard_EXPORT virtual void Send (const Handle(Standard_Transient)& object, const Message_Msg& message, const Message_Gravity gravity);
  
  //! Sends a message to be attached to the shape.
  Standard_EXPORT virtual void Send (const TopoDS_Shape& shape, const Message_Msg& message, const Message_Gravity gravity);
  
  //! Calls Send method with Null Transient.
  Standard_EXPORT virtual void Send (const Message_Msg& message, const Message_Gravity gravity);




  DEFINE_STANDARD_RTTIEXT(ShapeExtend_BasicMsgRegistrator,Standard_Transient)

protected:




private:




};







#endif // _ShapeExtend_BasicMsgRegistrator_HeaderFile
