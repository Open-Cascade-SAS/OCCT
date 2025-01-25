// Created on: 1993-06-17
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Transfer_DispatchControl_HeaderFile
#define _Transfer_DispatchControl_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_CopyControl.hxx>
class Transfer_TransientProcess;
class Interface_InterfaceModel;
class Standard_Transient;

class Transfer_DispatchControl;
DEFINE_STANDARD_HANDLE(Transfer_DispatchControl, Interface_CopyControl)

//! This is an auxiliary class for TransferDispatch, which allows
//! to record simple copies, as CopyControl from Interface, but
//! based on a TransientProcess. Hence, it allows in addition
//! more actions (such as recording results of adaptations)
class Transfer_DispatchControl : public Interface_CopyControl
{

public:
  //! Creates the DispatchControl, ready for use
  Standard_EXPORT Transfer_DispatchControl(const Handle(Interface_InterfaceModel)&  model,
                                           const Handle(Transfer_TransientProcess)& TP);

  //! Returns the content of the DispatchControl : it can be used
  //! for a direct call, if the basic methods do not suffice
  Standard_EXPORT const Handle(Transfer_TransientProcess)& TransientProcess() const;

  //! Returns the Model from which the transfer is to be done
  Standard_EXPORT const Handle(Interface_InterfaceModel)& StartingModel() const;

  //! Clears the List of Copied Results
  Standard_EXPORT void Clear() Standard_OVERRIDE;

  //! Binds a (Transient) Result to a (Transient) Starting Entity
  Standard_EXPORT void Bind(const Handle(Standard_Transient)& ent,
                            const Handle(Standard_Transient)& res) Standard_OVERRIDE;

  //! Searches for the Result bound to a Starting Entity
  //! If Found, returns True and fills <res>
  //! Else, returns False and nullifies <res>
  Standard_EXPORT Standard_Boolean Search(const Handle(Standard_Transient)& ent,
                                          Handle(Standard_Transient)& res) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Transfer_DispatchControl, Interface_CopyControl)

protected:
private:
  Handle(Transfer_TransientProcess) theTP;
  Handle(Interface_InterfaceModel)  themodel;
};

#endif // _Transfer_DispatchControl_HeaderFile
