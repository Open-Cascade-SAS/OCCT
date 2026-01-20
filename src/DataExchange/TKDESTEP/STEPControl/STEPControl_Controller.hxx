// Created on: 1995-02-20
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _STEPControl_Controller_HeaderFile
#define _STEPControl_Controller_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XSControl_Controller.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_Integer.hxx>
class Interface_InterfaceModel;
class XSControl_WorkSession;
class TopoDS_Shape;
class Transfer_FinderProcess;

//! defines basic controller for STEP processor
class STEPControl_Controller : public XSControl_Controller
{

public:
  //! Initializes the use of STEP Norm (the first time) and
  //! returns a Controller
  Standard_EXPORT STEPControl_Controller();

  //! Creates a new empty Model ready to receive data of the Norm.
  //! It is taken from STEP Template Model
  Standard_EXPORT occ::handle<Interface_InterfaceModel> NewModel() const override;

  //! Returns the Actor for Read attached to the pair (norm,appli)
  Standard_EXPORT occ::handle<Transfer_ActorOfTransientProcess> ActorRead(
    const occ::handle<Interface_InterfaceModel>& theModel) const override;

  Standard_EXPORT virtual void Customise(occ::handle<XSControl_WorkSession>& WS) override;

  //! Takes one Shape and transfers it to the InterfaceModel
  //! (already created by NewModel for instance)
  //! <modeshape> is to be interpreted by each kind of XstepAdaptor
  //! Returns a status : 0 OK  1 No result  2 Fail  -1 bad modeshape
  //! -2 bad model (requires a StepModel)
  //! modeshape : 1 Facetted BRep, 2 Shell, 3 Manifold Solid
  Standard_EXPORT virtual IFSelect_ReturnStatus TransferWriteShape(
    const TopoDS_Shape&                     shape,
    const occ::handle<Transfer_FinderProcess>&   FP,
    const occ::handle<Interface_InterfaceModel>& model,
    const int                  modetrans = 0,
    const Message_ProgressRange& theProgress = Message_ProgressRange()) const override;

  //! Standard Initialisation. It creates a Controller for STEP
  //! and records it to various names, available to select it later
  //! Returns True when done, False if could not be done
  Standard_EXPORT static bool Init();

  DEFINE_STANDARD_RTTIEXT(STEPControl_Controller, XSControl_Controller)

};

#endif // _STEPControl_Controller_HeaderFile
