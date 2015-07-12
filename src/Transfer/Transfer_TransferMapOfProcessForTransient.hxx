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

#ifndef _Transfer_TransferMapOfProcessForTransient_HeaderFile
#define _Transfer_TransferMapOfProcessForTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_BasicMap.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Address.hxx>
class Standard_DomainError;
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_Transient;
class Transfer_Binder;
class Transfer_IndexedDataMapNodeOfTransferMapOfProcessForTransient;



class Transfer_TransferMapOfProcessForTransient  : public TCollection_BasicMap
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Transfer_TransferMapOfProcessForTransient(const Standard_Integer NbBuckets = 1);
  
  Standard_EXPORT Transfer_TransferMapOfProcessForTransient& Assign (const Transfer_TransferMapOfProcessForTransient& Other);
Transfer_TransferMapOfProcessForTransient& operator = (const Transfer_TransferMapOfProcessForTransient& Other)
{
  return Assign(Other);
}
  
  Standard_EXPORT void ReSize (const Standard_Integer NbBuckets);
  
  Standard_EXPORT void Clear();
~Transfer_TransferMapOfProcessForTransient()
{
  Clear();
}
  
  Standard_EXPORT Standard_Integer Add (const Handle(Standard_Transient)& K, const Handle(Transfer_Binder)& I);
  
  Standard_EXPORT void Substitute (const Standard_Integer I, const Handle(Standard_Transient)& K, const Handle(Transfer_Binder)& T);
  
  Standard_EXPORT void RemoveLast();
  
  Standard_EXPORT Standard_Boolean Contains (const Handle(Standard_Transient)& K) const;
  
  Standard_EXPORT const Handle(Standard_Transient)& FindKey (const Standard_Integer I) const;
  
  Standard_EXPORT const Handle(Transfer_Binder)& FindFromIndex (const Standard_Integer I) const;
const Handle(Transfer_Binder)& operator () (const Standard_Integer I) const
{
  return FindFromIndex(I);
}
  
  Standard_EXPORT Handle(Transfer_Binder)& ChangeFromIndex (const Standard_Integer I);
Handle(Transfer_Binder)& operator () (const Standard_Integer I)
{
  return ChangeFromIndex(I);
}
  
  Standard_EXPORT Standard_Integer FindIndex (const Handle(Standard_Transient)& K) const;
  
  Standard_EXPORT const Handle(Transfer_Binder)& FindFromKey (const Handle(Standard_Transient)& K) const;
  
  Standard_EXPORT Handle(Transfer_Binder)& ChangeFromKey (const Handle(Standard_Transient)& K);
  
  Standard_EXPORT Standard_Address FindFromKey1 (const Handle(Standard_Transient)& K) const;
  
  Standard_EXPORT Standard_Address ChangeFromKey1 (const Handle(Standard_Transient)& K);




protected:





private:

  
  Standard_EXPORT Transfer_TransferMapOfProcessForTransient(const Transfer_TransferMapOfProcessForTransient& Other);




};







#endif // _Transfer_TransferMapOfProcessForTransient_HeaderFile
