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

#ifndef _Transfer_ActorOfFinderProcess_HeaderFile
#define _Transfer_ActorOfFinderProcess_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Transfer_ActorOfProcessForFinder.hxx>
class Transfer_Binder;
class Transfer_Finder;
class Transfer_ProcessForFinder;
class Transfer_FinderProcess;
class Standard_Transient;


class Transfer_ActorOfFinderProcess;
DEFINE_STANDARD_HANDLE(Transfer_ActorOfFinderProcess, Transfer_ActorOfProcessForFinder)

//! The original class was renamed. Compatibility only
//!
//! ModeTrans : a simple way of transmitting a transfer mode from
//! a user. To be interpreted for each norm
class Transfer_ActorOfFinderProcess : public Transfer_ActorOfProcessForFinder
{

public:

  
  Standard_EXPORT Transfer_ActorOfFinderProcess();
  
  //! Returns the Transfer Mode, modifiable
  Standard_EXPORT Standard_Integer& ModeTrans();
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring
                   (const Handle(Transfer_Finder)& start,
                    const Handle(Transfer_ProcessForFinder)& TP,
                    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer
                   (const Handle(Transfer_Finder)& start,
                    const Handle(Transfer_FinderProcess)& TP,
                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_EXPORT virtual Handle(Standard_Transient) TransferTransient
                   (const Handle(Standard_Transient)& start,
                    const Handle(Transfer_FinderProcess)& TP,
                    const Message_ProgressRange& theProgress = Message_ProgressRange());




  DEFINE_STANDARD_RTTIEXT(Transfer_ActorOfFinderProcess,Transfer_ActorOfProcessForFinder)

protected:


  Standard_Integer themodetrans;


private:




};







#endif // _Transfer_ActorOfFinderProcess_HeaderFile
