// Created on: 2000-09-29
// Created by: Andrey BETENEV
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

#ifndef _STEPConstruct_Tool_HeaderFile
#define _STEPConstruct_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class XSControl_WorkSession;
class Transfer_FinderProcess;
class Transfer_TransientProcess;
class Interface_HGraph;
class Interface_InterfaceModel;
class Interface_Graph;

//! Provides basic functionalities for tools which are intended
//! for encoding/decoding specific STEP constructs
//!
//! It is initialized by WorkSession and allows easy access to
//! its fields and internal data such as Model, TP and FP
//!
//! NOTE: Call to method Graph() with True (or for a first time,
//! if you have updated the model since last computation of model)
//! can take a time, so it is recommended to avoid creation of
//! this (and derived) tool multiple times
class STEPConstruct_Tool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty tool
  Standard_EXPORT STEPConstruct_Tool();

  //! Creates a tool and loads it with worksession
  Standard_EXPORT STEPConstruct_Tool(const Handle(XSControl_WorkSession)& WS);

  //! Returns currently loaded WorkSession
  const Handle(XSControl_WorkSession)& WS() const;

  //! Returns current model (Null if not loaded)
  Handle(Interface_InterfaceModel) Model() const;

  //! Returns current graph (recomputing if necessary)
  const Interface_Graph& Graph(const Standard_Boolean recompute = Standard_False) const;

  //! Returns TransientProcess (reading; Null if not loaded)
  const Handle(Transfer_TransientProcess)& TransientProcess() const;

  //! Returns FinderProcess (writing; Null if not loaded)
  const Handle(Transfer_FinderProcess)& FinderProcess() const;

protected:
  //! Load worksession; returns True if succeeded
  //! Returns False if either FinderProcess of TransientProcess
  //! cannot be obtained or are Null
  Standard_EXPORT Standard_Boolean SetWS(const Handle(XSControl_WorkSession)& WS);

private:
  Handle(XSControl_WorkSession)     myWS;
  Handle(Transfer_FinderProcess)    myFinderProcess;
  Handle(Transfer_TransientProcess) myTransientProcess;
  Handle(Interface_HGraph)          myHGraph;
};

#include <STEPConstruct_Tool.lxx>

#endif // _STEPConstruct_Tool_HeaderFile
