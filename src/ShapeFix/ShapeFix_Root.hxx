// Created on: 1999-08-09
// Created by: Galina KULIKOVA
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeFix_Root_HeaderFile
#define _ShapeFix_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <Message_Gravity.hxx>
#include <Standard_Integer.hxx>

#include <ShapeExtend_BasicMsgRegistrator.hxx>

class ShapeBuild_ReShape;
class ShapeExtend_BasicMsgRegistrator;
class Message_Msg;


class ShapeFix_Root;
DEFINE_STANDARD_HANDLE(ShapeFix_Root, Standard_Transient)

//! Root class for fixing operations
//! Provides context for recording changes (optional),
//! basic precision value and limit (minimal and
//! maximal) values for tolerances,
//! and message registrator
class ShapeFix_Root : public Standard_Transient
{

public:

  
  //! Empty Constructor (no context is created)
  Standard_EXPORT ShapeFix_Root();
  
  //! Copy all fields from another Root object
  Standard_EXPORT virtual void Set (const Handle(ShapeFix_Root)& Root);
  
  //! Sets context
  Standard_EXPORT virtual void SetContext (const Handle(ShapeBuild_ReShape)& context);
  
  //! Returns context
    Handle(ShapeBuild_ReShape) Context() const;
  
  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator (const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg);
  
  //! Returns message registrator
    Handle(ShapeExtend_BasicMsgRegistrator) MsgRegistrator() const;
  
  //! Sets basic precision value
  Standard_EXPORT virtual void SetPrecision (const Standard_Real preci);
  
  //! Returns basic precision value
    Standard_Real Precision() const;
  
  //! Sets minimal allowed tolerance
  Standard_EXPORT virtual void SetMinTolerance (const Standard_Real mintol);
  
  //! Returns minimal allowed tolerance
    Standard_Real MinTolerance() const;
  
  //! Sets maximal allowed tolerance
  Standard_EXPORT virtual void SetMaxTolerance (const Standard_Real maxtol);
  
  //! Returns maximal allowed tolerance
    Standard_Real MaxTolerance() const;
  
  //! Returns tolerance limited by [myMinTol,myMaxTol]
    Standard_Real LimitTolerance (const Standard_Real toler) const;
  
  //! Sends a message to be attached to the shape.
  //! Calls corresponding message of message registrator.
  Standard_EXPORT void SendMsg (const TopoDS_Shape& shape, const Message_Msg& message, const Message_Gravity gravity = Message_Info) const;
  
  //! Sends a message to be attached to myShape.
  //! Calls previous method.
    void SendMsg (const Message_Msg& message, const Message_Gravity gravity = Message_Info) const;
  
  //! Sends a warning to be attached to the shape.
  //! Calls SendMsg with gravity set to Message_Warning.
    void SendWarning (const TopoDS_Shape& shape, const Message_Msg& message) const;
  
  //! Calls previous method for myShape.
    void SendWarning (const Message_Msg& message) const;
  
  //! Sends a fail to be attached to the shape.
  //! Calls SendMsg with gravity set to Message_Fail.
    void SendFail (const TopoDS_Shape& shape, const Message_Msg& message) const;
  
  //! Calls previous method for myShape.
    void SendFail (const Message_Msg& message) const;




  DEFINE_STANDARD_RTTIEXT(ShapeFix_Root,Standard_Transient)

protected:

  
  //! Auxiliary method for work with three-position
  //! (on/off/default) flags (modes) in ShapeFix.
    static Standard_Boolean NeedFix (const Standard_Integer flag, const Standard_Boolean def = Standard_True);

  TopoDS_Shape myShape;


private:


  Handle(ShapeBuild_ReShape) myContext;
  Handle(ShapeExtend_BasicMsgRegistrator) myMsgReg;
  Standard_Real myPrecision;
  Standard_Real myMinTol;
  Standard_Real myMaxTol;


};


#include <ShapeFix_Root.lxx>





#endif // _ShapeFix_Root_HeaderFile
