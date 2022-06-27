// Created on: 1992-02-07
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

#ifndef _Transfer_TransferDispatch_HeaderFile
#define _Transfer_TransferDispatch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_CopyTool.hxx>
class Interface_InterfaceModel;
class Interface_GeneralLib;
class Interface_Protocol;
class Transfer_TransientProcess;
class Standard_Transient;


//! A TransferDispatch is aimed to dispatch Entities between two
//! Interface Models, by default by copying them, as CopyTool, but
//! with more capabilities of adapting : Copy is redefined to
//! firstly pass the hand to a TransferProcess. If this gives no
//! result, standard Copy is called.
//!
//! This allow, for instance, to modify the copied Entity (such as
//! changing a Name for a VDA Entity), or to do a deeper work
//! (such as Substituting a kind of Entity to another one).
//!
//! For these reasons, TransferDispatch is basically a CopyTool,
//! but uses a more sophiscated control, which is TransferProcess,
//! and its method Copy is redefined
class Transfer_TransferDispatch  : public Interface_CopyTool
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a TransferDispatch from a Model. Works with a General
  //! Service Library, given as an Argument
  //! A TransferDispatch is created as a CopyTool in which the
  //! Control is set to TransientProcess
  Standard_EXPORT Transfer_TransferDispatch(const Handle(Interface_InterfaceModel)& amodel, const Interface_GeneralLib& lib);
  
  //! Same as above, but Library is defined through a Protocol
  Standard_EXPORT Transfer_TransferDispatch(const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_Protocol)& protocol);
  
  //! Same as above, but works with the Active Protocol
  Standard_EXPORT Transfer_TransferDispatch(const Handle(Interface_InterfaceModel)& amodel);
  
  //! Returns the content of Control Object, as a TransientProcess
  Standard_EXPORT Handle(Transfer_TransientProcess) TransientProcess() const;
  
  //! Copies an Entity by calling the method Transferring from the
  //! TransferProcess. If this called produces a Null Binder, then
  //! the standard, inherited Copy is called
  Standard_EXPORT virtual Standard_Boolean Copy (const Handle(Standard_Transient)& entfrom, Handle(Standard_Transient)& entto, const Standard_Boolean mapped, const Standard_Boolean errstat) Standard_OVERRIDE;




protected:





private:





};







#endif // _Transfer_TransferDispatch_HeaderFile
