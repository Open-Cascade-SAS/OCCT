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

#ifndef _Transfer_TransferInput_HeaderFile
#define _Transfer_TransferInput_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Interface_EntityIterator;
class Transfer_TransferIterator;
class Transfer_TransientProcess;
class Interface_InterfaceModel;
class Interface_Protocol;
class Transfer_FinderProcess;


//! A TransferInput is a Tool which fills an InterfaceModel with
//! the result of the Transfer of CasCade Objects, once determined
//! The Result comes from a TransferProcess, either from
//! Transient (the Complete Result is considered, it must contain
//! only Transient Objects)
class Transfer_TransferInput 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a TransferInput ready to use
  Standard_EXPORT Transfer_TransferInput();
  
  //! Takes the transient items stored in a TransferIterator
  Standard_EXPORT Interface_EntityIterator Entities (Transfer_TransferIterator& list) const;
  
  //! Fills an InterfaceModel with the Complete Result of a Transfer
  //! stored in a TransientProcess (Starting Objects are Transient)
  //! The complete result is exactly added to the model
  Standard_EXPORT void FillModel (const Handle(Transfer_TransientProcess)& proc, const Handle(Interface_InterfaceModel)& amodel) const;
  
  //! Fills an InterfaceModel with results of the Transfer recorded
  //! in a TransientProcess (Starting Objects are Transient) :
  //! Root Result if <roots> is True (Default), Complete Result else
  //! The entities added to the model are determined from the result
  //! by by adding the referenced entities
  Standard_EXPORT void FillModel (const Handle(Transfer_TransientProcess)& proc, const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_Protocol)& proto, const Standard_Boolean roots = Standard_True) const;
  
  //! Fills an InterfaceModel with the Complete Result of a Transfer
  //! stored in a TransientProcess (Starting Objects are Transient)
  //! The complete result is exactly added to the model
  Standard_EXPORT void FillModel (const Handle(Transfer_FinderProcess)& proc, const Handle(Interface_InterfaceModel)& amodel) const;
  
  //! Fills an InterfaceModel with results of the Transfer recorded
  //! in a TransientProcess (Starting Objects are Transient) :
  //! Root Result if <roots> is True (Default), Complete Result else
  //! The entities added to the model are determined from the result
  //! by by adding the referenced entities
  Standard_EXPORT void FillModel (const Handle(Transfer_FinderProcess)& proc, const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_Protocol)& proto, const Standard_Boolean roots = Standard_True) const;




protected:





private:





};







#endif // _Transfer_TransferInput_HeaderFile
