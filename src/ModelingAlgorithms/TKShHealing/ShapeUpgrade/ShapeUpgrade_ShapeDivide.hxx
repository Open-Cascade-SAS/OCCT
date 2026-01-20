// Created on: 1999-04-26
// Created by: Pavel DURANDIN
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

#ifndef _ShapeUpgrade_ShapeDivide_HeaderFile
#define _ShapeUpgrade_ShapeDivide_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Message_Gravity.hxx>
#include <ShapeExtend_Status.hxx>
class ShapeUpgrade_FaceDivide;
class ShapeBuild_ReShape;
class ShapeExtend_BasicMsgRegistrator;
class Message_Msg;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Divides a all faces in shell with given criteria Shell.
class ShapeUpgrade_ShapeDivide
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ShapeUpgrade_ShapeDivide();

  //! Initialize by a Shape.
  Standard_EXPORT ShapeUpgrade_ShapeDivide(const TopoDS_Shape& S);

  //! Initialize by a Shape.
  Standard_EXPORT void Init(const TopoDS_Shape& S);

  Standard_EXPORT virtual ~ShapeUpgrade_ShapeDivide();

  //! Defines the spatial precision used for splitting
  Standard_EXPORT void SetPrecision(const double Prec);

  //! Sets maximal allowed tolerance
  Standard_EXPORT void SetMaxTolerance(const double maxtol);

  //! Sets minimal allowed tolerance
  Standard_EXPORT void SetMinTolerance(const double mintol);

  //! Purpose sets mode for trimming (segment) surface by
  //! wire UV bounds.
  Standard_EXPORT void SetSurfaceSegmentMode(const bool Segment);

  //! Performs splitting and computes the resulting shape
  //! If newContext is True (default), the internal context
  //! will be cleared at start, else previous substitutions
  //! will be acting.
  Standard_EXPORT virtual bool Perform(
    const bool newContext = true);

  //! Gives the resulting Shape, or Null shape if not done.
  Standard_EXPORT TopoDS_Shape Result() const;

  //! Returns context with all the modifications made during
  //! last call(s) to Perform() recorded
  Standard_EXPORT occ::handle<ShapeBuild_ReShape> GetContext() const;

  //! Sets context with recorded modifications to be applied
  //! during next call(s) to Perform(shape,false)
  Standard_EXPORT void SetContext(const occ::handle<ShapeBuild_ReShape>& context);

  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator(
    const occ::handle<ShapeExtend_BasicMsgRegistrator>& msgreg);

  //! Returns message registrator
  Standard_EXPORT occ::handle<ShapeExtend_BasicMsgRegistrator> MsgRegistrator() const;

  //! Sends a message to be attached to the shape.
  //! Calls corresponding message of message registrator.
  Standard_EXPORT void SendMsg(const TopoDS_Shape&   shape,
                               const Message_Msg&    message,
                               const Message_Gravity gravity = Message_Info) const;

  //! Queries the status of last call to Perform
  //! OK   : no splitting was done (or no call to Perform)
  //! DONE1: some edges were split
  //! DONE2: surface was split
  //! FAIL1: some errors occurred
  Standard_EXPORT bool Status(const ShapeExtend_Status status) const;

  //! Sets the tool for splitting faces.
  Standard_EXPORT void SetSplitFaceTool(const occ::handle<ShapeUpgrade_FaceDivide>& splitFaceTool);

  //! Sets mode for splitting 3d curves from edges.
  //! 0 - only curve 3d from free edges.
  //! 1 - only curve 3d from shared edges.
  //! 2 -  all curve 3d.
  Standard_EXPORT void SetEdgeMode(const int aEdgeMode);

protected:
  //! Returns the tool for splitting faces.
  Standard_EXPORT virtual occ::handle<ShapeUpgrade_FaceDivide> GetSplitFaceTool() const;

  Standard_EXPORT virtual Message_Msg GetFaceMsg() const;

  Standard_EXPORT virtual Message_Msg GetWireMsg() const;

  //! Returns a message describing modification of a shape.
  Standard_EXPORT virtual Message_Msg GetEdgeMsg() const;

  occ::handle<ShapeBuild_ReShape>              myContext;
  occ::handle<ShapeExtend_BasicMsgRegistrator> myMsgReg;
  TopoDS_Shape                            myShape;
  TopoDS_Shape                            myResult;
  double                           myPrecision;
  double                           myMinTol;
  double                           myMaxTol;
  bool                        mySegmentMode;
  int                        myStatus;
  int                        myEdgeMode;

private:
  occ::handle<ShapeUpgrade_FaceDivide> mySplitFaceTool;
};

#endif // _ShapeUpgrade_ShapeDivide_HeaderFile
