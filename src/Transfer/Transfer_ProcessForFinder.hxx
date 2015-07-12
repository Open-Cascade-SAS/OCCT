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

#ifndef _Transfer_ProcessForFinder_HeaderFile
#define _Transfer_ProcessForFinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <Transfer_TransferMapOfProcessForFinder.hxx>
#include <MMgt_TShared.hxx>
#include <Transfer_HSequenceOfFinder.hxx>
#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
class Message_Messenger;
class Transfer_Finder;
class Transfer_Binder;
class Transfer_ActorOfProcessForFinder;
class Message_ProgressIndicator;
class Interface_InterfaceError;
class Transfer_TransferFailure;
class Transfer_FindHasher;
class Transfer_TransferMapOfProcessForFinder;
class Transfer_IndexedDataMapNodeOfTransferMapOfProcessForFinder;
class Transfer_IteratorOfProcessForFinder;
class Message_Msg;
class Interface_Check;
class Standard_Transient;
class Interface_CheckIterator;


class Transfer_ProcessForFinder;
DEFINE_STANDARD_HANDLE(Transfer_ProcessForFinder, MMgt_TShared)


class Transfer_ProcessForFinder : public MMgt_TShared
{

public:

  
  Standard_EXPORT Transfer_ProcessForFinder(const Standard_Integer nb = 10000);
  
  Standard_EXPORT Transfer_ProcessForFinder(const Handle(Message_Messenger)& printer, const Standard_Integer nb = 10000);
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void Clean();
  
  Standard_EXPORT void Resize (const Standard_Integer nb);
  
  Standard_EXPORT void SetActor (const Handle(Transfer_ActorOfProcessForFinder)& actor);
  
  Standard_EXPORT Handle(Transfer_ActorOfProcessForFinder) Actor() const;
  
  Standard_EXPORT Handle(Transfer_Binder) Find (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT Standard_Boolean IsBound (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT Standard_Boolean IsAlreadyUsed (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT void Bind (const Handle(Transfer_Finder)& start, const Handle(Transfer_Binder)& binder);
  
  Standard_EXPORT void Rebind (const Handle(Transfer_Finder)& start, const Handle(Transfer_Binder)& binder);
  
  Standard_EXPORT Standard_Boolean Unbind (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT Handle(Transfer_Binder) FindElseBind (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT void SetMessenger (const Handle(Message_Messenger)& messenger);
  
  Standard_EXPORT Handle(Message_Messenger) Messenger() const;
  
  Standard_EXPORT void SetTraceLevel (const Standard_Integer tracelev);
  
  Standard_EXPORT Standard_Integer TraceLevel() const;
  
  Standard_EXPORT void SendFail (const Handle(Transfer_Finder)& start, const Message_Msg& amsg);
  
  Standard_EXPORT void SendWarning (const Handle(Transfer_Finder)& start, const Message_Msg& amsg);
  
  Standard_EXPORT void SendMsg (const Handle(Transfer_Finder)& start, const Message_Msg& amsg);
  
  Standard_EXPORT void AddFail (const Handle(Transfer_Finder)& start, const Standard_CString mess, const Standard_CString orig = "");
  
  Standard_EXPORT void AddError (const Handle(Transfer_Finder)& start, const Standard_CString mess, const Standard_CString orig = "");
  
  Standard_EXPORT void AddFail (const Handle(Transfer_Finder)& start, const Message_Msg& amsg);
  
  Standard_EXPORT void AddWarning (const Handle(Transfer_Finder)& start, const Standard_CString mess, const Standard_CString orig = "");
  
  Standard_EXPORT void AddWarning (const Handle(Transfer_Finder)& start, const Message_Msg& amsg);
  
  Standard_EXPORT void Mend (const Handle(Transfer_Finder)& start, const Standard_CString pref = "");
  
  Standard_EXPORT Handle(Interface_Check) Check (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT void BindTransient (const Handle(Transfer_Finder)& start, const Handle(Standard_Transient)& res);
  
  Standard_EXPORT const Handle(Standard_Transient)& FindTransient (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT void BindMultiple (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT void AddMultiple (const Handle(Transfer_Finder)& start, const Handle(Standard_Transient)& res);
  
  Standard_EXPORT Standard_Boolean FindTypedTransient (const Handle(Transfer_Finder)& start, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& val) const;
  
  Standard_EXPORT Standard_Boolean GetTypedTransient (const Handle(Transfer_Binder)& binder, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& val) const;
  
  Standard_EXPORT Standard_Integer NbMapped() const;
  
  Standard_EXPORT const Handle(Transfer_Finder)& Mapped (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer MapIndex (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT Handle(Transfer_Binder) MapItem (const Standard_Integer num) const;
  
  Standard_EXPORT void SetRoot (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT void SetRootManagement (const Standard_Boolean stat);
  
  Standard_EXPORT Standard_Integer NbRoots() const;
  
  Standard_EXPORT const Handle(Transfer_Finder)& Root (const Standard_Integer num) const;
  
  Standard_EXPORT Handle(Transfer_Binder) RootItem (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer RootIndex (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT Standard_Integer NestingLevel() const;
  
  Standard_EXPORT void ResetNestingLevel();
  
  Standard_EXPORT Standard_Boolean Recognize (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT Handle(Transfer_Binder) Transferring (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT Standard_Boolean Transfer (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT void SetErrorHandle (const Standard_Boolean err);
  
  Standard_EXPORT Standard_Boolean ErrorHandle() const;
  
  Standard_EXPORT void StartTrace (const Handle(Transfer_Binder)& binder, const Handle(Transfer_Finder)& start, const Standard_Integer level, const Standard_Integer mode) const;
  
  Standard_EXPORT virtual void PrintTrace (const Handle(Transfer_Finder)& start, const Handle(Message_Messenger)& S) const;
  
  Standard_EXPORT Standard_Boolean IsLooping (const Standard_Integer alevel) const;
  
  Standard_EXPORT Transfer_IteratorOfProcessForFinder RootResult (const Standard_Boolean withstart = Standard_False) const;
  
  Standard_EXPORT Transfer_IteratorOfProcessForFinder CompleteResult (const Standard_Boolean withstart = Standard_False) const;
  
  Standard_EXPORT Transfer_IteratorOfProcessForFinder AbnormalResult() const;
  
  Standard_EXPORT Interface_CheckIterator CheckList (const Standard_Boolean erronly) const;
  
  Standard_EXPORT Transfer_IteratorOfProcessForFinder ResultOne (const Handle(Transfer_Finder)& start, const Standard_Integer level, const Standard_Boolean withstart = Standard_False) const;
  
  Standard_EXPORT Interface_CheckIterator CheckListOne (const Handle(Transfer_Finder)& start, const Standard_Integer level, const Standard_Boolean erronly) const;
  
  Standard_EXPORT Standard_Boolean IsCheckListEmpty (const Handle(Transfer_Finder)& start, const Standard_Integer level, const Standard_Boolean erronly) const;
  
  Standard_EXPORT void RemoveResult (const Handle(Transfer_Finder)& start, const Standard_Integer level, const Standard_Boolean compute = Standard_True);
  
  Standard_EXPORT virtual Standard_Integer CheckNum (const Handle(Transfer_Finder)& start) const;
  
  Standard_EXPORT void SetProgress (const Handle(Message_ProgressIndicator)& theProgress);
  
  Standard_EXPORT Handle(Message_ProgressIndicator) GetProgress() const;




  DEFINE_STANDARD_RTTI(Transfer_ProcessForFinder,MMgt_TShared)

protected:




private:

  
  Standard_EXPORT Handle(Transfer_Binder) FindAndMask (const Handle(Transfer_Finder)& start);
  
  Standard_EXPORT Handle(Transfer_Binder) TransferProduct (const Handle(Transfer_Finder)& start);

  Standard_Boolean theerrh;
  Standard_Integer thetrace;
  Handle(Message_Messenger) themessenger;
  Standard_Integer thelevel;
  Standard_Integer therootl;
  Standard_Boolean therootm;
  TColStd_IndexedMapOfInteger theroots;
  Handle(Transfer_Finder) thelastobj;
  Handle(Transfer_Binder) thelastbnd;
  Standard_Integer theindex;
  Handle(Transfer_ActorOfProcessForFinder) theactor;
  Transfer_TransferMapOfProcessForFinder themap;
  Handle(Message_ProgressIndicator) myProgress;


};







#endif // _Transfer_ProcessForFinder_HeaderFile
