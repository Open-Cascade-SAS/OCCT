// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#include <TDocStd_Document.hxx>

#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_AttributeDeltaList.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Delta.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_ListIteratorOfDeltaList.hxx>
#include <TDF_Reference.hxx>
#include <TDocStd.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_CompoundDelta.hxx>
#include <TDocStd_Context.hxx>
#include <TDocStd_LabelIDMapDataMap.hxx>
#include <TDocStd_Modified.hxx>
#include <TDocStd_Owner.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkIterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_Document, CDM_Document)

// List should have a RemoveLast...
#define TDocStd_List_RemoveLast(theList)                                                           \
  TDF_ListIteratorOfDeltaList it(theList);                                                         \
  Standard_Integer            i, n = theList.Extent();                                             \
  for (i = 1; i < n; i++)                                                                          \
    it.Next();                                                                                     \
  theList.Remove(it);

#undef DEB_TRANS

#undef DEB_DELTA

#define SRN_DELTA_COMPACT

//=================================================================================================

Handle(TDocStd_Document) TDocStd_Document::Get(const TDF_Label& acces)
{
  // avoid creation of Handle(TDF_Data) during TDF_Data destruction
  if (acces.Root().HasAttribute())
  {
    return TDocStd_Owner::GetDocument(acces.Data());
  }
  return Handle(TDocStd_Document)();
}

//=================================================================================================

TDocStd_Document::TDocStd_Document(const TCollection_ExtendedString& aStorageFormat)
    : myStorageFormat(aStorageFormat),
      myData(new TDF_Data()),
      myUndoLimit(0),
      myUndoTransaction("UNDO"),
      mySaveTime(0),
      myIsNestedTransactionMode(0),
      mySaveEmptyLabels(Standard_False),
      myStorageFormatVersion(TDocStd_FormatVersion_CURRENT)
{
  myUndoTransaction.Initialize(myData);
  TDocStd_Owner::SetDocument(myData, this);

#ifdef SRN_DELTA_COMPACT
  myFromUndo.Nullify();
  myFromRedo.Nullify();
#endif
}

//=================================================================================================

Standard_Boolean TDocStd_Document::IsSaved() const
{
  return CDM_Document::IsStored();
}

//=================================================================================================

TCollection_ExtendedString TDocStd_Document::GetName() const
{
  return CDM_Document::MetaData()->Name();
}

//=================================================================================================

TCollection_ExtendedString TDocStd_Document::GetPath() const
{
  return CDM_Document::MetaData()->Path();
}

//=================================================================================================

void TDocStd_Document::SetData(const Handle(TDF_Data)& D)
{
  myData = D;
  myUndoTransaction.Initialize(myData);
}

//=================================================================================================

Handle(TDF_Data) TDocStd_Document::GetData() const
{
  return myData;
}

//=================================================================================================

TDF_Label TDocStd_Document::Main() const
{
  return myData->Root().FindChild(1, Standard_True);
}

//=================================================================================================

Standard_Boolean TDocStd_Document::IsEmpty() const
{
  TDF_AttributeIterator It(Main());
  return !It.More();
}

//=================================================================================================

Standard_Boolean TDocStd_Document::IsValid() const
{
  return TDocStd_Modified::IsEmpty(Main());
}

//=================================================================================================

void TDocStd_Document::SetModified(const TDF_Label& L)
{
  TDocStd_Modified::Add(L);
}

//=================================================================================================

// Standard_Boolean TDocStd_Document::IsModified (const TDF_Label& L) const
//{
//  return TDocStd_Modified::Contains(L);
//}

//=================================================================================================

void TDocStd_Document::PurgeModified()
{
  TDocStd_Modified::Clear(Main());
}

//=================================================================================================

const TDF_LabelMap& TDocStd_Document::GetModified() const
{
  return TDocStd_Modified::Get(Main());
}

//=================================================================================================

void TDocStd_Document::Update(const Handle(CDM_Document)& /*aToDocument*/,
                              const Standard_Integer aReferenceIdentifier,
                              const Standard_Address aModifContext)
{
  const TDocStd_Context* CC = static_cast<TDocStd_Context*>(aModifContext);
  if (CC->ModifiedReferences() || !IsUpToDate(aReferenceIdentifier))
  {
    TCollection_AsciiString aDocEntry(aReferenceIdentifier);
    UpdateReferences(aDocEntry);
    SetIsUpToDate(aReferenceIdentifier);
  }
}

//=================================================================================================

void TDocStd_Document::NewCommand()
{
#ifdef OCCT_DEBUG_TRANS
  if (myUndoTransaction.IsOpen() && myData->Transaction() > 1)
  {
    throw Standard_DomainError("NewCommand : many open transactions");
  }
#endif

  CommitTransaction();
  OpenTransaction();

#ifdef OCCT_DEBUG_TRANS
  std::cout << "End NewCommand" << std::endl;
#endif
}

//=================================================================================================

Standard_Boolean TDocStd_Document::HasOpenCommand() const
{
  return myUndoTransaction.IsOpen();
}

//=================================================================================================

void TDocStd_Document::OpenCommand()
{
  if (!myIsNestedTransactionMode && myUndoTransaction.IsOpen())
  {
    throw Standard_DomainError("TDocStd_Document::OpenCommand : already open");
  }
  OpenTransaction();
  //  if (myUndoLimit != 0) myUndoTransaction.Open();
}

//=================================================================================================

Standard_Boolean TDocStd_Document::CommitCommand()
{
  return CommitTransaction();
}

//=================================================================================================

void TDocStd_Document::AbortCommand()
{
  AbortTransaction();
}

//=======================================================================
// function : CommitTransaction
// purpose  : Private method.
//=======================================================================

Standard_Boolean TDocStd_Document::CommitTransaction()
{
  myData->AllowModification(Standard_True);

  Standard_Boolean isDone = Standard_False;
  // nested transaction mode
  if (myIsNestedTransactionMode && myUndoTransaction.IsOpen())
  {

    Handle(TDF_Delta)             D = myUndoTransaction.Commit(Standard_True);
    Handle(TDocStd_CompoundDelta) aCompDelta =
      Handle(TDocStd_CompoundDelta)::DownCast(myUndoFILO.First());
    AppendDeltaToTheFirst(aCompDelta, D);
    D = aCompDelta;
    myUndoFILO.RemoveFirst();
    if (myUndoFILO.Extent())
    {
      aCompDelta = Handle(TDocStd_CompoundDelta)::DownCast(myUndoFILO.First());
      AppendDeltaToTheFirst(aCompDelta, D);
      myUndoTransaction.Open();
    }
    else
    {
      if (!D->IsEmpty())
      {
        myUndos.Append(D);
        myRedos.Clear(); // if we push an Undo we clear the redos
        isDone = Standard_True;
      }
    }

    // deny modifications if the transaction is not opened
    if (myOnlyTransactionModification)
    {
      myData->AllowModification(myUndoTransaction.IsOpen() && myUndoLimit ? Standard_True
                                                                          : Standard_False);
    }
  }
  else
  {

    // are we undoing...
    if (myUndoLimit != 0 && myUndoTransaction.IsOpen())
    {

      Handle(TDF_Delta) D = myUndoTransaction.Commit(Standard_True);
      if (!(D.IsNull() || D->IsEmpty()))
      {
        isDone = Standard_True;

        myRedos.Clear();   // if we push an Undo we clear the redos
        myUndos.Append(D); // New undos are at the end of the list
        // Check  the limit to remove the oldest one
        if (myUndos.Extent() > myUndoLimit)
        {
#ifdef SRN_DELTA_COMPACT
          Handle(TDF_Delta) aDelta = myUndos.First();
#endif
          myUndos.RemoveFirst();
#ifdef SRN_DELTA_COMPACT
          if (myFromUndo == aDelta)
          {
            // The oldest Undo delta coincides with `from` delta
            if (myUndos.Extent() == 1)
            { // There is the only Undo
              myFromUndo.Nullify();
              myFromRedo.Nullify();
            }
            else
              myFromUndo = myUndos.First();
          }
#endif
        }
      }
    }

    // deny or allow modifications according to transaction state
    if (myOnlyTransactionModification)
    {
      myData->AllowModification(myUndoTransaction.IsOpen() && myUndoLimit ? Standard_True
                                                                          : Standard_False);
    }
  }
  // Notify CDM_Application of the successful commit
  if (isDone && IsOpened())
  {
    const Handle(TDocStd_Application) anAppli =
      Handle(TDocStd_Application)::DownCast(Application());
    if (!anAppli.IsNull())
      anAppli->OnCommitTransaction(this);
  }
  return isDone;
}

//=======================================================================
// function : AbortTransaction
// purpose  : Private method.
//=======================================================================

void TDocStd_Document::AbortTransaction()
{
  myData->AllowModification(Standard_True);

  if (myUndoTransaction.IsOpen())
    if (myUndoLimit != 0)
      myUndoTransaction.Abort();

  if (myIsNestedTransactionMode && myUndoFILO.Extent())
  {
    if (!myUndoFILO.First()->IsEmpty())
      myData->Undo(myUndoFILO.First(), Standard_True);
    myUndoFILO.RemoveFirst();
    if (myUndoFILO.Extent())
      myUndoTransaction.Open();
  }
  // deny or allow modifications according to transaction state
  if (myOnlyTransactionModification)
  {
    myData->AllowModification(myUndoTransaction.IsOpen() && myUndoLimit ? Standard_True
                                                                        : Standard_False);
  }
  // Notify CDM_Application of the event
  if (IsOpened())
  {
    const Handle(TDocStd_Application) anAppli =
      Handle(TDocStd_Application)::DownCast(Application());
    if (!anAppli.IsNull())
      anAppli->OnAbortTransaction(this);
  }
}

//=======================================================================
// function :OpenTransaction
// purpose  : Private method.
//=======================================================================

void TDocStd_Document::OpenTransaction()
{
  myData->AllowModification(Standard_True);

  // nested transaction mode
  if (myIsNestedTransactionMode)
  {

    if (myUndoTransaction.IsOpen())
    {
      Handle(TDF_Delta)             D = myUndoTransaction.Commit(Standard_True);
      Handle(TDocStd_CompoundDelta) aCompDelta =
        Handle(TDocStd_CompoundDelta)::DownCast(myUndoFILO.First());
      AppendDeltaToTheFirst(aCompDelta, D);
    }
    Standard_Integer aLastTime = myData->Time();
    if (myUndoFILO.Extent())
      aLastTime = myUndoFILO.First()->EndTime();
    Handle(TDocStd_CompoundDelta) aCompoundDelta = new TDocStd_CompoundDelta;
    aCompoundDelta->Validity(aLastTime, aLastTime);
    myUndoFILO.Prepend(aCompoundDelta);
  }

  if (myUndoLimit != 0)
    myUndoTransaction.Open();

  // deny or allow modifications according to transaction state
  if (myOnlyTransactionModification)
  {
    myData->AllowModification(myUndoTransaction.IsOpen() && myUndoLimit ? Standard_True
                                                                        : Standard_False);
  }
  // Notify CDM_Application of the event
  if (IsOpened())
  {
    const Handle(TDocStd_Application) anAppli =
      Handle(TDocStd_Application)::DownCast(Application());
    if (!anAppli.IsNull())
      anAppli->OnOpenTransaction(this);
  }
}

//=================================================================================================

void TDocStd_Document::SetUndoLimit(const Standard_Integer L)
{
#ifdef SRN_DELTA_COMPACT
  myFromUndo.Nullify(); // Compaction has to aborted
  myFromRedo.Nullify();
#endif

  CommitTransaction();
  myUndoLimit        = (L > 0) ? L : 0;
  Standard_Integer n = myUndos.Extent() - myUndoLimit;
  while (n > 0)
  {
    myUndos.RemoveFirst();
    --n;
  }
  // deny or allow modifications according to transaction state
  if (myOnlyTransactionModification)
  {
    myData->AllowModification(myUndoTransaction.IsOpen() && myUndoLimit ? Standard_True
                                                                        : Standard_False);
  }
  // OpenTransaction(); dp 15/10/99
}

//=================================================================================================

Standard_Integer TDocStd_Document::GetUndoLimit() const
{
  return myUndoLimit;
}

//=================================================================================================

Standard_Integer TDocStd_Document::GetAvailableUndos() const
{
  return myUndos.Extent();
}

//=================================================================================================

void TDocStd_Document::ClearUndos()
{
  myUndos.Clear();
  myRedos.Clear();
#ifdef SRN_DELTA_COMPACT
  myFromRedo.Nullify();
  myFromUndo.Nullify();
#endif
}

//=================================================================================================

void TDocStd_Document::ClearRedos()
{
  myRedos.Clear();
#ifdef SRN_DELTA_COMPACT
  myFromRedo.Nullify();
#endif
}

//=======================================================================
// function : Undo
// purpose  :
// Some important notice:
// 1) The most recent undo delta is at the end of the list.
// 2) Removing the LAST item of a list is tedious, but it is done only on
//    Undo. Remove first is done at each command if the limit is reached!
// 3) To make fun, the redos are not like the undos: the most recent delta
//    is at the beginning! Like this, it is easier to remove it after use.
//=======================================================================
Standard_Boolean TDocStd_Document::Undo()
{
  // Don't call NewCommand(), because it may commit Interactive Attributes
  // and generate a undesirable Delta!

  Standard_Boolean isOpened = myUndoTransaction.IsOpen();
  Standard_Boolean undoDone = Standard_False;
  // TDF_Label currentObjectLabel = CurrentLabel (); //Sauve pour usage ulterieur.

  if (!myUndos.IsEmpty())
  {
    // Reset the transaction
    AbortTransaction();

    // only for nested transaction mode
    while (myIsNestedTransactionMode && myUndoFILO.Extent())
      AbortTransaction();

    // allow modifications
    myData->AllowModification(Standard_True);

    // Apply the Undo
    // should test the applicability before.
#ifdef OCCT_DEBUG_DELTA
    std::cout << "DF before Undo ==================================" << std::endl;
    TDF_Tool::DeepDump(std::cout, myData);
#endif
    Handle(TDF_Delta) D = myData->Undo(myUndos.Last(), Standard_True);
    D->SetName(myUndos.Last()->Name());
#ifdef OCCT_DEBUG_DELTA
    std::cout << "DF after Undo ==================================" << std::endl;
    TDF_Tool::DeepDump(std::cout, myData);
#endif
    // Push the redo
    myRedos.Prepend(D);
    // Remove the last Undo
    TDocStd_List_RemoveLast(myUndos);
    undoDone = Standard_True;
  }

  if (isOpened && undoDone)
    OpenTransaction();

  // deny or allow modifications according to transaction state
  if (myOnlyTransactionModification)
  {
    myData->AllowModification(myUndoTransaction.IsOpen() && myUndoLimit ? Standard_True
                                                                        : Standard_False);
  }

  return undoDone;
}

//=================================================================================================

Standard_Integer TDocStd_Document::GetAvailableRedos() const
{
  // should test the applicability before.
  return myRedos.Extent();
}

//=================================================================================================

Standard_Boolean TDocStd_Document::Redo()
{
  Standard_Boolean isOpened = myUndoTransaction.IsOpen();
  Standard_Boolean undoDone = Standard_False;
  if (!myRedos.IsEmpty())
  {
    // should test the applicability before.
    // Reset the transaction
    AbortTransaction();

    // only for nested transaction mode
    while (myIsNestedTransactionMode && myUndoFILO.Extent())
      AbortTransaction();

    // allow modifications
    myData->AllowModification(Standard_True);

    // Apply the Redo
#ifdef OCCT_DEBUG_DELTA
    std::cout << "DF before Redo ==================================" << std::endl;
    TDF_Tool::DeepDump(std::cout, myData);
#endif
    Handle(TDF_Delta) D = myData->Undo(myRedos.First(), Standard_True);
    D->SetName(myRedos.First()->Name());
#ifdef OCCT_DEBUG_DELTA
    std::cout << "DF after Redo ==================================" << std::endl;
    TDF_Tool::DeepDump(std::cout, myData);
#endif
    // Push the redo of the redo as an undo (got it !)
    myUndos.Append(D);
    // remove the Redo from the head
    myRedos.RemoveFirst();
    undoDone = Standard_True;
  }

  if (isOpened && undoDone)
    OpenTransaction();

  // deny or allow modifications according to transaction state
  if (myOnlyTransactionModification)
  {
    myData->AllowModification(myUndoTransaction.IsOpen() && myUndoLimit ? Standard_True
                                                                        : Standard_False);
  }

  return undoDone;
}

//=================================================================================================

void TDocStd_Document::UpdateReferences(const TCollection_AsciiString& aDocEntry)
{

  TDF_AttributeList aRefList;
  TDocStd_XLink*    xRefPtr;
  for (TDocStd_XLinkIterator xItr(this); xItr.More(); xItr.Next())
  {
    xRefPtr = xItr.Value();
    if (xRefPtr->DocumentEntry() == aDocEntry)
    {
      aRefList.Append(xRefPtr->Update());
    }
  }
  TDF_ListIteratorOfAttributeList It(aRefList);
  for (; It.More(); It.Next())
  {
    //     // mise a jour import
    SetModified(It.Value()->Label());
  }
}

//=================================================================================================

const TDF_DeltaList& TDocStd_Document::GetUndos() const
{
  return myUndos;
}

//=================================================================================================

const TDF_DeltaList& TDocStd_Document::GetRedos() const
{
  return myRedos;
}

//=================================================================================================

Standard_Boolean TDocStd_Document::InitDeltaCompaction()
{
#ifdef SRN_DELTA_COMPACT
  if (myUndoLimit == 0 || myUndos.Extent() == 0)
  {
    myFromRedo.Nullify();
    myFromUndo.Nullify();
    return Standard_False; // No Undos to compact
  }

  myFromRedo.Nullify();

  myFromUndo = myUndos.Last();
  if (myRedos.Extent() > 0)
    myFromRedo = myRedos.First();
#endif
  return Standard_True;
}

//=================================================================================================

Standard_Boolean TDocStd_Document::PerformDeltaCompaction()
{
#ifdef SRN_DELTA_COMPACT
  if (myFromUndo.IsNull())
    return Standard_False; // Redo can be Null for this operation

  TDF_DeltaList                        aList;
  Handle(TDocStd_CompoundDelta)        aCompoundDelta = new TDocStd_CompoundDelta;
  TDF_ListIteratorOfDeltaList          anIterator(myUndos);
  TDF_ListIteratorOfAttributeDeltaList aDeltasIterator;
  TDocStd_LabelIDMapDataMap            aMap;
  Standard_Boolean                     isFound = Standard_False, isTimeSet = Standard_False;

  // Process Undos

  for (; anIterator.More(); anIterator.Next())
  {
    if (!isFound)
    {
      if (myFromUndo == anIterator.Value())
        isFound = Standard_True;
      aList.Append(anIterator.Value()); // Fill the list of deltas that precede compound delta
      continue;
    }

    if (!isTimeSet)
    { // Set begin and end time when the compound delta is valid
      aCompoundDelta->Validity(anIterator.Value()->BeginTime(), myUndos.Last()->EndTime());
      isTimeSet = Standard_True;
    }

    aDeltasIterator.Initialize(anIterator.Value()->AttributeDeltas());
    for (; aDeltasIterator.More(); aDeltasIterator.Next())
    {
      if (!aMap.IsBound(aDeltasIterator.Value()->Label()))
      {
        TDF_IDMap* pIDMap = new TDF_IDMap();
        aMap.Bind(aDeltasIterator.Value()->Label(), *pIDMap);
        delete pIDMap;
      }
      if (aMap(aDeltasIterator.Value()->Label())
            .Add(aDeltasIterator.Value()->ID()))                    // The attribute is not
                                                                    // clang-format off
	aCompoundDelta->AddAttributeDelta(aDeltasIterator.Value());                 //already in the delta
                                                                    // clang-format on
    }
  }

  myUndos.Clear();
  myUndos.Assign(aList);
  myUndos.Append(aCompoundDelta);

  // Process Redos

  if (myFromRedo.IsNull())
  {
    myRedos.Clear();
    return Standard_True;
  }

  aList.Clear();

  for (anIterator.Initialize(myRedos); anIterator.More(); anIterator.Next())
  {
    aList.Append(anIterator.Value());
    if (anIterator.Value() == myFromRedo)
      break;
  }

  myRedos.Clear();
  myRedos.Assign(aList);
#endif
  return Standard_True;
}

//=================================================================================================

TCollection_ExtendedString TDocStd_Document::StorageFormat() const
{
  return myStorageFormat;
}

//=================================================================================================

void TDocStd_Document::ChangeStorageFormat(const TCollection_ExtendedString& newStorageFormat)
{
  if (newStorageFormat != myStorageFormat)
  {
    myStorageFormat      = newStorageFormat;
    myResourcesAreLoaded = Standard_False;
    CDM_Document::LoadResources();
  }
}

//=================================================================================================

void TDocStd_Document::Recompute()
{
  if (IsValid())
    return;
  // find the top function and execute it
  //  Handle(TDesign_Function) F;
  //  if (Main().FindAttribute(TDesign_Function::GetID(),F)) {
  // TFunction_Solver slv;
  // slv.SetTouched(GetModified());
  // slv.ExecuteFrom(F);
  PurgeModified();
}

//=======================================================================
// function : AppendDeltaToTheFirst
// purpose  : Appends delta to the first delta in the myUndoFILO
//=======================================================================

void TDocStd_Document::AppendDeltaToTheFirst(const Handle(TDocStd_CompoundDelta)& theDelta1,
                                             const Handle(TDF_Delta)&             theDelta2)
{
  if (theDelta2->IsEmpty())
    return;
  TDocStd_LabelIDMapDataMap aMap;

  TDF_ListIteratorOfAttributeDeltaList aDeltasIterator1(theDelta1->AttributeDeltas());
  for (; aDeltasIterator1.More(); aDeltasIterator1.Next())
  {
    TDF_Label aLabel = aDeltasIterator1.Value()->Label();
    if (!aMap.IsBound(aLabel))
    {
      TDF_IDMap aTmpIDMap;
      aMap.Bind(aLabel, aTmpIDMap);
    }
    Standard_GUID anID    = aDeltasIterator1.Value()->ID();
    TDF_IDMap&    anIDMap = aMap.ChangeFind(aLabel);
    anIDMap.Add(anID);
  }

  theDelta1->Validity(theDelta1->BeginTime(), theDelta2->EndTime());
  TDF_ListIteratorOfAttributeDeltaList aDeltasIterator2(theDelta2->AttributeDeltas());
  for (; aDeltasIterator2.More(); aDeltasIterator2.Next())
  {
    TDF_Label     aLabel = aDeltasIterator2.Value()->Label();
    Standard_GUID anID   = aDeltasIterator2.Value()->ID();
    if (aMap.IsBound(aLabel))
    {
      const TDF_IDMap& anIDMap = aMap.Find(aLabel);
      if (anIDMap.Contains(anID))
        continue;
    }
    theDelta1->AddAttributeDelta(aDeltasIterator2.Value());
  }
}

//=================================================================================================

void TDocStd_Document::RemoveFirstUndo()
{
  if (myUndos.IsEmpty())
    return;
  myUndos.RemoveFirst();
}

//=================================================================================================

void TDocStd_Document::BeforeClose()
{
  SetModificationMode(Standard_False);
  AbortTransaction();
  if (myIsNestedTransactionMode)
    myUndoFILO.Clear();
  ClearUndos();
}

//=================================================================================================

TDocStd_FormatVersion TDocStd_Document::StorageFormatVersion() const
{
  return myStorageFormatVersion;
}

//=======================================================================
// function : ChangeStorageFormatVersion
// purpose  : Sets <theVersion> of the format to be used to store the document
//=======================================================================
void TDocStd_Document::ChangeStorageFormatVersion(const TDocStd_FormatVersion theVersion)
{
  myStorageFormatVersion = theVersion;
}

//=======================================================================
// function : CurrentStorageFormatVersion
// purpose  : Returns current storage format version of the document.
//=======================================================================
TDocStd_FormatVersion TDocStd_Document::CurrentStorageFormatVersion()
{
  return TDocStd_FormatVersion_CURRENT;
}

//=================================================================================================

void TDocStd_Document::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, CDM_Document)

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myStorageFormat)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsSaved())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsChanged())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsEmpty())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsValid())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, GetAvailableUndos())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, GetAvailableRedos())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, HasOpenCommand())

  for (TDF_DeltaList::Iterator anUndoIt(myUndos); anUndoIt.More(); anUndoIt.Next())
  {
    const Handle(TDF_Delta)& anUndo = anUndoIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anUndo.get())
  }

  for (TDF_DeltaList::Iterator aRedoIt(myRedos); aRedoIt.More(); aRedoIt.Next())
  {
    const Handle(TDF_Delta)& aRedo = aRedoIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aRedo.get())
  }

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myData.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUndoLimit)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myUndoTransaction)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myFromUndo.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myFromRedo.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySaveTime)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsNestedTransactionMode)

  for (TDF_DeltaList::Iterator anUndoFILOIt(myUndoFILO); anUndoFILOIt.More(); anUndoFILOIt.Next())
  {
    const Handle(TDF_Delta)& anUndoFILO = anUndoFILOIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anUndoFILO.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOnlyTransactionModification)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySaveEmptyLabels)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myStorageFormatVersion)
}
