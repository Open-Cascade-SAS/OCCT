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

#include <Transfer_ActorOfProcessForFinder.hxx>

#include <Standard_Type.hxx>

#include <Transfer_ActorOfProcessForFinder.hxx>
#include <Standard_DomainError.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FindHasher.hxx>
#include <Transfer_ProcessForFinder.hxx>
#include <Transfer_TransferMapOfProcessForFinder.hxx>
#include <Transfer_IteratorOfProcessForFinder.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Standard_Transient.hxx>

#define TheStart Handle(Transfer_Finder)
#define TheStart_hxx <Transfer_Finder.hxx>
#define TheMapHasher Transfer_FindHasher
#define TheMapHasher_hxx <Transfer_FindHasher.hxx>
#define Handle_TheList Handle(Transfer_HSequenceOfFinder)
#define TheList Transfer_HSequenceOfFinder
#define TheList_hxx <Transfer_HSequenceOfFinder.hxx>
#define Transfer_TransferMap Transfer_TransferMapOfProcessForFinder
#define Transfer_TransferMap_hxx <Transfer_TransferMapOfProcessForFinder.hxx>
#define Transfer_Iterator Transfer_IteratorOfProcessForFinder
#define Transfer_Iterator_hxx <Transfer_IteratorOfProcessForFinder.hxx>
#define Transfer_Actor Transfer_ActorOfProcessForFinder
#define Transfer_Actor_hxx <Transfer_ActorOfProcessForFinder.hxx>
#define Handle_Transfer_Actor Handle(Transfer_ActorOfProcessForFinder)
#define Transfer_TransferProcess Transfer_ProcessForFinder
#define Transfer_TransferProcess_hxx <Transfer_ProcessForFinder.hxx>
#define Handle_Transfer_TransferProcess Handle(Transfer_ProcessForFinder)
#include <Transfer_Actor.gxx>

