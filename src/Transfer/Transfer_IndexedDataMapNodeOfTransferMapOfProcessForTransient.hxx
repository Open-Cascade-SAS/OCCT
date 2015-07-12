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

#ifndef _Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient_HeaderFile
#define _Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_MapNodePtr.hxx>
#include <TCollection_MapNode.hxx>
#include <TColStd_MapTransientHasher.hxx>
class Standard_Transient;
class Transfer_Binder;
class Transfer_TransferMapOfProcessForTransient;


class Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient;
DEFINE_STANDARD_HANDLE(Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient, TCollection_MapNode)


class Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient : public TCollection_MapNode
{

public:

  
    Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient(const Handle(Standard_Transient)& K1, const Standard_Integer K2, const Handle(Transfer_Binder)& I, const TCollection_MapNodePtr& n1, const TCollection_MapNodePtr& n2);
  
    Handle(Standard_Transient)& Key1() const;
  
    Standard_Integer& Key2() const;
  
    TCollection_MapNodePtr& Next2() const;
  
    Handle(Transfer_Binder)& Value() const;




  DEFINE_STANDARD_RTTI(Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient,TCollection_MapNode)

protected:




private:


  Handle(Standard_Transient) myKey1;
  Standard_Integer myKey2;
  Handle(Transfer_Binder) myValue;
  TCollection_MapNodePtr myNext2;


};

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

#include <TCollection_IndexedDataMapNode.lxx>

#undef TheKey
#undef TheKey_hxx
#undef TheItem
#undef TheItem_hxx
#undef Hasher
#undef Hasher_hxx
#undef TCollection_IndexedDataMapNode
#undef TCollection_IndexedDataMapNode_hxx
#undef Handle_TCollection_IndexedDataMapNode
#undef TCollection_IndexedDataMap
#undef TCollection_IndexedDataMap_hxx




#endif // _Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient_HeaderFile
