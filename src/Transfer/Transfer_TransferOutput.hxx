// Created on: 1992-02-04
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Transfer_TransferOutput_HeaderFile
#define _Transfer_TransferOutput_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Message_ProgressRange.hxx>

class Transfer_TransientProcess;
class Interface_InterfaceModel;
class Transfer_ActorOfTransientProcess;
class Standard_Transient;
class Interface_Protocol;
class Interface_Graph;
class Interface_EntityIterator;

//! A TransferOutput is a Tool which manages the transfer of
//! entities created by an Interface, stored in an InterfaceModel,
//! into a set of Objects suitable for an Application
//! Objects to be transferred are given, by method Transfer
//! (which calls Transfer from TransientProcess)
//! A default action is available to get all roots of the Model
//! Result is given as a TransferIterator (see TransferProcess)
//! Also, it is possible to pilot directly the TransientProcess
class Transfer_TransferOutput 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a TransferOutput ready to use, with a TransientProcess
  Standard_EXPORT Transfer_TransferOutput(const Handle(Transfer_ActorOfTransientProcess)& actor, const Handle(Interface_InterfaceModel)& amodel);
  
  //! Creates a TransferOutput from an already existing
  //! TransientProcess, and a Model
  //! Returns (by Reference, hence can be changed) the Mode for
  //! Scope Management. False (D) means Scope is ignored.
  //! True means that each individual Transfer (direct or through
  //! TransferRoots) is regarded as one Scope
  Standard_EXPORT Transfer_TransferOutput(const Handle(Transfer_TransientProcess)& proc, const Handle(Interface_InterfaceModel)& amodel);
  
  //! Returns the Starting Model
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;
  
  //! Returns the TransientProcess used to work
  Standard_EXPORT Handle(Transfer_TransientProcess) TransientProcess() const;
  
  //! Transfer checks that all taken Entities come from the same
  //! Model, then calls Transfer from TransientProcess
  Standard_EXPORT void Transfer (const Handle(Standard_Transient)& obj,
                                 const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Runs transfer on the roots of the Interface Model
  //! The Roots are computed with a ShareFlags created from a
  //! Protocol given as Argument
  Standard_EXPORT void TransferRoots (const Handle(Interface_Protocol)& protocol,
                                      const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Runs transfer on the roots defined by a Graph of dependences
  //! (which detains also a Model and its Entities)
  //! Roots are computed with a ShareFlags created from the Graph
  Standard_EXPORT void TransferRoots (const Interface_Graph& G,
                                      const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Runs transfer on the roots of the Interface Model
  //! Remark : the Roots are computed with a ShareFlags created
  //! from the Active Protocol
  Standard_EXPORT void TransferRoots(const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Returns the list of Starting Entities with these criteria :
  //! - <normal> False, gives the entities bound with ABNORMAL STATUS
  //! (e.g. : Fail recorded, Exception raised during Transfer)
  //! - <normal> True, gives Entities with or without a Result, but
  //! with no Fail, no Exception (Warnings are not counted)
  //! - <roots> False, considers all entities recorded (either for
  //! Result, or for at least one Fail or Warning message)
  //! - <roots> True (Default), considers only roots of Transfer
  //! (the Entities recorded at highest level)
  //! This method is based on AbnormalResult from TransferProcess
  Standard_EXPORT Interface_EntityIterator ListForStatus (const Standard_Boolean normal, const Standard_Boolean roots = Standard_True) const;
  
  //! Fills a Model with the list determined by ListForStatus
  //! This model starts from scratch (made by NewEmptyModel from the
  //! current Model), then is filled by AddWithRefs
  //!
  //! Useful to get separately from a transfer, the entities which
  //! have caused problem, in order to furtherly analyse them (with
  //! normal = False), or the "good" entities, to obtain a data set
  //! "which works well" (with normal = True)
  Standard_EXPORT Handle(Interface_InterfaceModel) ModelForStatus (const Handle(Interface_Protocol)& protocol, const Standard_Boolean normal, const Standard_Boolean roots = Standard_True) const;




protected:





private:



  Handle(Transfer_TransientProcess) theproc;
  Handle(Interface_InterfaceModel) themodel;


};







#endif // _Transfer_TransferOutput_HeaderFile
