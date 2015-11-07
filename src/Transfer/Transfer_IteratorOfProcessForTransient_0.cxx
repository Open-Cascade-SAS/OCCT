// Created on: 1992-02-03
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

#include <Transfer_IteratorOfProcessForTransient.hxx>

#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_ProcessForTransient.hxx>
#include <Transfer_TransferMapOfProcessForTransient.hxx>
#include <Transfer_ActorOfProcessForTransient.hxx>
#include <Transfer_Binder.hxx>
 

#define TheStart Handle(Standard_Transient)
#define TheStart_hxx <Standard_Transient.hxx>
#define TheMapHasher TColStd_MapTransientHasher
#define TheMapHasher_hxx <TColStd_MapTransientHasher.hxx>
#define Handle_TheList Handle(TColStd_HSequenceOfTransient)
#define TheList TColStd_HSequenceOfTransient
#define TheList_hxx <TColStd_HSequenceOfTransient.hxx>
#define Transfer_TransferMap Transfer_TransferMapOfProcessForTransient
#define Transfer_TransferMap_hxx <Transfer_TransferMapOfProcessForTransient.hxx>
#define Transfer_Iterator Transfer_IteratorOfProcessForTransient
#define Transfer_Iterator_hxx <Transfer_IteratorOfProcessForTransient.hxx>
#define Transfer_Actor Transfer_ActorOfProcessForTransient
#define Transfer_Actor_hxx <Transfer_ActorOfProcessForTransient.hxx>
#define Handle_Transfer_Actor Handle(Transfer_ActorOfProcessForTransient)
#define Transfer_TransferProcess Transfer_ProcessForTransient
#define Transfer_TransferProcess_hxx <Transfer_ProcessForTransient.hxx>
#define Handle_Transfer_TransferProcess Handle(Transfer_ProcessForTransient)
#include <Transfer_Iterator.gxx>

