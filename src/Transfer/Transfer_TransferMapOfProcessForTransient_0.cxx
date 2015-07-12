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

#include <Transfer_TransferMapOfProcessForTransient.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient.hxx>
 

#define TheKey Handle(Standard_Transient)
#define TheKey_hxx <Standard_Transient.hxx>
#define TheItem Handle(Transfer_Binder)
#define TheItem_hxx <Transfer_Binder.hxx>
#define Hasher TColStd_MapTransientHasher
#define Hasher_hxx <TColStd_MapTransientHasher.hxx>
#define TCollection_IndexedDataMapNode Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient
#define TCollection_IndexedDataMapNode_hxx <Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient.hxx>
#define Handle_TCollection_IndexedDataMapNode Handle(Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient)
#define TCollection_IndexedDataMap Transfer_TransferMapOfProcessForTransient
#define TCollection_IndexedDataMap_hxx <Transfer_TransferMapOfProcessForTransient.hxx>
#include <TCollection_IndexedDataMap.gxx>

