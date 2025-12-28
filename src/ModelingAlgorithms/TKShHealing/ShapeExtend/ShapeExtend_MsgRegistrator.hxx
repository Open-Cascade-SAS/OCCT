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

#ifndef _ShapeExtend_MsgRegistrator_HeaderFile
#define _ShapeExtend_MsgRegistrator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Message_Msg.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <Message_Gravity.hxx>
class Standard_Transient;
class Message_Msg;
class TopoDS_Shape;

//! Attaches messages to the objects (generic Transient or shape).
//! The objects of this class are transmitted to the Shape Healing
//! algorithms so that they could collect messages occurred during
//! processing.
//!
//! Messages are added to the Maps (stored as a field) that can be
//! used, for instance, by Data Exchange processors to attach those
//! messages to initial file entities.
class ShapeExtend_MsgRegistrator : public ShapeExtend_BasicMsgRegistrator
{

public:
  //! Creates an object.
  Standard_EXPORT ShapeExtend_MsgRegistrator();

  //! Sends a message to be attached to the object.
  //! If the object is in the map then the message is added to the
  //! list, otherwise the object is firstly added to the map.
  Standard_EXPORT virtual void Send(const occ::handle<Standard_Transient>& object,
                                    const Message_Msg&                     message,
                                    const Message_Gravity                  gravity) override;

  //! Sends a message to be attached to the shape.
  //! If the shape is in the map then the message is added to the
  //! list, otherwise the shape is firstly added to the map.
  Standard_EXPORT virtual void Send(const TopoDS_Shape&   shape,
                                    const Message_Msg&    message,
                                    const Message_Gravity gravity) override;

  //! Returns a Map of objects and message list
  const NCollection_DataMap<occ::handle<Standard_Transient>, NCollection_List<Message_Msg>>&
    MapTransient() const;

  //! Returns a Map of shapes and message list
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<Message_Msg>, TopTools_ShapeMapHasher>&
    MapShape() const;

  DEFINE_STANDARD_RTTIEXT(ShapeExtend_MsgRegistrator, ShapeExtend_BasicMsgRegistrator)

private:
  NCollection_DataMap<occ::handle<Standard_Transient>, NCollection_List<Message_Msg>>
    myMapTransient;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<Message_Msg>, TopTools_ShapeMapHasher>
    myMapShape;
};

#include <ShapeExtend_MsgRegistrator.lxx>

#endif // _ShapeExtend_MsgRegistrator_HeaderFile
