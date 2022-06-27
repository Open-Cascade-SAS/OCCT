// Created on: 1996-09-04
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Transfer_ActorOfTransientProcess_HeaderFile
#define _Transfer_ActorOfTransientProcess_HeaderFile

#include <Standard.hxx>

#include <Transfer_ActorOfProcessForTransient.hxx>
class Transfer_Binder;
class Standard_Transient;
class Transfer_ProcessForTransient;
class Transfer_TransientProcess;

class Transfer_ActorOfTransientProcess;
DEFINE_STANDARD_HANDLE(Transfer_ActorOfTransientProcess, Transfer_ActorOfProcessForTransient)

//! The original class was renamed. Compatibility only
class Transfer_ActorOfTransientProcess : public Transfer_ActorOfProcessForTransient
{

public:

  
  Standard_EXPORT Transfer_ActorOfTransientProcess();
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring
                         (const Handle(Standard_Transient)& start,
                          const Handle(Transfer_ProcessForTransient)& TP,
                          const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer
                         (const Handle(Standard_Transient)& start,
                          const Handle(Transfer_TransientProcess)& TP,
                          const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_EXPORT virtual Handle(Standard_Transient) TransferTransient
                         (const Handle(Standard_Transient)& start,
                          const Handle(Transfer_TransientProcess)& TP,
                          const Message_ProgressRange& theProgress = Message_ProgressRange());




  DEFINE_STANDARD_RTTIEXT(Transfer_ActorOfTransientProcess,Transfer_ActorOfProcessForTransient)

protected:




private:




};







#endif // _Transfer_ActorOfTransientProcess_HeaderFile
