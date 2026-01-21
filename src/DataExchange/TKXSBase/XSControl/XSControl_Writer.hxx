// Created on: 1997-05-14
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _XSControl_Writer_HeaderFile
#define _XSControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_Integer.hxx>
#include <Message_ProgressRange.hxx>

class XSControl_WorkSession;
class Interface_InterfaceModel;
class TopoDS_Shape;

//! This class gives a simple way to create then write a
//! Model compliant to a given norm, from a Shape
//! The model can then be edited by tools by other appropriate tools
class XSControl_Writer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a Writer from scratch
  Standard_EXPORT XSControl_Writer();

  //! Creates a Writer from scratch, with a norm name which
  //! identifie a Controller
  Standard_EXPORT XSControl_Writer(const char* const norm);

  //! Creates a Writer from an already existing Session
  //! If <scratch> is True (D), clears already recorded data
  Standard_EXPORT XSControl_Writer(const occ::handle<XSControl_WorkSession>& WS,
                                   const bool                                scratch = true);

  //! Sets a specific norm to <me>
  //! Returns True if done, False if <norm> is not available
  Standard_EXPORT bool SetNorm(const char* const norm);

  //! Sets a specific session to <me>
  Standard_EXPORT void SetWS(const occ::handle<XSControl_WorkSession>& WS,
                             const bool                                scratch = true);

  //! Returns the session used in <me>
  Standard_EXPORT occ::handle<XSControl_WorkSession> WS() const;

  //! Returns the produced model. Produces a new one if not yet done
  //! or if <newone> is True
  //! This method allows for instance to edit product or header
  //! data before writing
  Standard_EXPORT occ::handle<Interface_InterfaceModel> Model(const bool newone = false);

  //! Transfers a Shape according to the mode
  Standard_EXPORT IFSelect_ReturnStatus
    TransferShape(const TopoDS_Shape&          sh,
                  const int                    mode        = 0,
                  const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes the produced model
  Standard_EXPORT IFSelect_ReturnStatus WriteFile(const char* const filename);

  //! Prints Statistics about Transfer
  Standard_EXPORT void PrintStatsTransfer(const int what, const int mode = 0) const;

private:
  occ::handle<XSControl_WorkSession> thesession;
};

#endif // _XSControl_Writer_HeaderFile
