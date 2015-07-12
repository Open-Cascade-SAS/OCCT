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

#include <Transfer_TransferMapOfProcessForFinder.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FindHasher.hxx>
#include <Transfer_IndexedDataMapNodeOfTransferMapOfProcessForFinder.hxx>
 

#define TheKey Handle(Transfer_Finder)
#define TheKey_hxx <Transfer_Finder.hxx>
#define TheItem Handle(Transfer_Binder)
#define TheItem_hxx <Transfer_Binder.hxx>
#define Hasher Transfer_FindHasher
#define Hasher_hxx <Transfer_FindHasher.hxx>
#define TCollection_IndexedDataMapNode Transfer_IndexedDataMapNodeOfTransferMapOfProcessForFinder
#define TCollection_IndexedDataMapNode_hxx <Transfer_IndexedDataMapNodeOfTransferMapOfProcessForFinder.hxx>
#define Handle_TCollection_IndexedDataMapNode Handle(Transfer_IndexedDataMapNodeOfTransferMapOfProcessForFinder)
#define TCollection_IndexedDataMap Transfer_TransferMapOfProcessForFinder
#define TCollection_IndexedDataMap_hxx <Transfer_TransferMapOfProcessForFinder.hxx>
#include <TCollection_IndexedDataMap.gxx>

