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

#ifndef _Transfer_ActorOfProcessForFinder_HeaderFile
#define _Transfer_ActorOfProcessForFinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <MMgt_TShared.hxx>
#include <Transfer_HSequenceOfFinder.hxx>
class Standard_DomainError;
class Transfer_Finder;
class Transfer_FindHasher;
class Transfer_ProcessForFinder;
class Transfer_TransferMapOfProcessForFinder;
class Transfer_IndexedDataMapNodeOfTransferMapOfProcessForFinder;
class Transfer_IteratorOfProcessForFinder;
class Transfer_Binder;
class Transfer_SimpleBinderOfTransient;
class Standard_Transient;


class Transfer_ActorOfProcessForFinder;
DEFINE_STANDARD_HANDLE(Transfer_ActorOfProcessForFinder, MMgt_TShared)


class Transfer_ActorOfProcessForFinder : public MMgt_TShared
{

public:

  
  Standard_EXPORT Transfer_ActorOfProcessForFinder();
  
  Standard_EXPORT virtual Standard_Boolean Recognize (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT virtual Handle(Transfer_Binder) Transferring (const Handle(Transfer_Finder)& start, const Handle(Transfer_ProcessForFinder)& TP);
  
  Standard_EXPORT Handle(Transfer_SimpleBinderOfTransient) TransientResult (const Handle(Standard_Transient)& res) const;
  
  Standard_EXPORT Handle(Transfer_Binder) NullResult() const;
  
  Standard_EXPORT void SetLast (const Standard_Boolean mode = Standard_True);
  
  Standard_EXPORT Standard_Boolean IsLast() const;
  
  Standard_EXPORT void SetNext (const Handle(Transfer_ActorOfProcessForFinder)& next);
  
  Standard_EXPORT Handle(Transfer_ActorOfProcessForFinder) Next() const;




  DEFINE_STANDARD_RTTI(Transfer_ActorOfProcessForFinder,MMgt_TShared)

protected:




private:


  Handle(Transfer_ActorOfProcessForFinder) thenext;
  Standard_Boolean thelast;


};







#endif // _Transfer_ActorOfProcessForFinder_HeaderFile
