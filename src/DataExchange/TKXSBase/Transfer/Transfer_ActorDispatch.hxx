// Created on: 1994-05-24
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Transfer_ActorDispatch_HeaderFile
#define _Transfer_ActorDispatch_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Transfer_TransferDispatch.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
class Interface_InterfaceModel;
class Interface_GeneralLib;
class Interface_Protocol;
class Transfer_Binder;
class Standard_Transient;
class Transfer_TransientProcess;

class Transfer_ActorDispatch;
DEFINE_STANDARD_HANDLE(Transfer_ActorDispatch, Transfer_ActorOfTransientProcess)

//! This class allows to work with a TransferDispatch, i.e. to
//! transfer entities from a data set to another one defined by
//! the same interface norm, with the following features :
//! - ActorDispatch itself acts as a default actor, i.e. it copies
//! entities with the general service Copy, as CopyTool does
//! - it allows to add other actors for specific ways of transfer,
//! which may include data modifications, conversions ...
//! - and other features from TransferDispatch (such as mapping
//! other than one-one)
class Transfer_ActorDispatch : public Transfer_ActorOfTransientProcess
{

public:
  //! Creates an ActorDispatch from a Model. Works with a General
  //! Service Library, given as an Argument
  //! This causes TransferDispatch and its TransientProcess to be
  //! created, with default actor <me>
  Standard_EXPORT Transfer_ActorDispatch(const Handle(Interface_InterfaceModel)& amodel,
                                         const Interface_GeneralLib&             lib);

  //! Same as above, but Library is defined through a Protocol
  Standard_EXPORT Transfer_ActorDispatch(const Handle(Interface_InterfaceModel)& amodel,
                                         const Handle(Interface_Protocol)&       protocol);

  //! Same as above, but works with the Active Protocol
  Standard_EXPORT Transfer_ActorDispatch(const Handle(Interface_InterfaceModel)& amodel);

  //! Utility which adds an actor to the default <me> (it calls
  //! SetActor from the TransientProcess)
  Standard_EXPORT void AddActor(const Handle(Transfer_ActorOfTransientProcess)& actor);

  //! Returns the TransferDispatch, which does the work, records
  //! the intermediate data, etc...
  //! See TransferDispatch & CopyTool, to see the available methods
  Standard_EXPORT Transfer_TransferDispatch& TransferDispatch();

  //! Specific action : it calls the method Transfer from CopyTool
  //! i.e. the general service Copy, then returns the Binder
  //! produced by the TransientProcess
  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer(
    const Handle(Standard_Transient)&        start,
    const Handle(Transfer_TransientProcess)& TP,
    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Transfer_ActorDispatch, Transfer_ActorOfTransientProcess)

protected:
private:
  Transfer_TransferDispatch thetool;
};

#endif // _Transfer_ActorDispatch_HeaderFile
