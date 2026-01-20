// Created on: 2004-11-22
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Object.hxx>

#include <TObj_Assistant.hxx>
#include <TObj_Model.hxx>
#include <TObj_ObjectIterator.hxx>
#include <TObj_OcafObjectIterator.hxx>
#include <TObj_Persistence.hxx>
#include <TObj_ReferenceIterator.hxx>
#include <TObj_SequenceIterator.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TObj_TModel.hxx>
#include <TObj_TNameContainer.hxx>
#include <TObj_TObject.hxx>
#include <TObj_TReference.hxx>

#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_CopyLabel.hxx>
#include <TDF_Data.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>
#include <TDF_TagSource.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TObj_Object, Standard_Transient)

//=================================================================================================

TObj_Object::TObj_Object(const TDF_Label& theLabel, const bool theSetName)
    : myLabel(theLabel)
{
  occ::handle<TObj_Object> aMe = this;
  TObj_TObject::Set(myLabel, aMe);
  if (theSetName)
    TObj_Model::SetNewName(aMe);
}

//=================================================================================================

occ::handle<TObj_Model> TObj_Object::GetModel() const
{
  occ::handle<TObj_Model> aModel;
  // if object label is null object is not alive
  if (myLabel.IsNull())
    return aModel;

  // TDF_Label aLabel = TDocStd_Document::Get(myLabel)->Main();
  occ::handle<TDF_Data> aData = myLabel.Data();
  if (aData.IsNull())
    return aModel;

  // try get the document from owner attribute manually
  TDF_Label                aLabel = aData->Root();
  occ::handle<TDocStd_Owner>    anOwnerAttr;
  occ::handle<TDocStd_Document> aTDoc;
  if (!aLabel.IsNull() && aLabel.FindAttribute(TDocStd_Owner::GetID(), anOwnerAttr))
    aTDoc = anOwnerAttr->GetDocument();
  if (aTDoc.IsNull())
    return aModel;

  // use main label of the document to find TObj model attribute
  aLabel = aTDoc->Main();
  occ::handle<TObj_TModel> aModelAttr;
  if (!aLabel.IsNull() && aLabel.FindAttribute(TObj_TModel::GetID(), aModelAttr))
    aModel = aModelAttr->Model();

  return aModel;
}

//=======================================================================
// function : GetChildren
// purpose  : Returns iterator for the child objects.
//           This method provides tree-like view of the objects hierarchy.
//           The references to other objects are not considered as children.
//           theType narrows a variety of iterated objects
//=======================================================================

static void addObjToOrderSequence(const occ::handle<TObj_Object>&      theObj,
                                  const int          theOrder,
                                  occ::handle<NCollection_HSequence<occ::handle<TObj_Object>>>& theHSeqOfObj,
                                  const int          theHSeqLength,
                                  int&               theLastIndex,
                                  int&               theLastOrder)
{
  if (theOrder > theLastOrder)
  {
    while (theOrder > theLastOrder)
    {
      // get next object and compare with them
      if (++theLastIndex > theHSeqLength)
      {
        theHSeqOfObj->Append(theObj);
        theLastIndex = theHSeqLength + 1;
        theLastOrder = theOrder;
        return;
      }
      occ::handle<TObj_Object> aNext = theHSeqOfObj->Value(theLastIndex);
      theLastOrder              = aNext->GetOrder();
    }
    // add before current position
    theHSeqOfObj->InsertBefore(theLastIndex, theObj);
    theLastOrder = theOrder;
  }
  else
  {
    while (theOrder < theLastOrder)
    {
      if (--theLastIndex < 1)
      {
        theHSeqOfObj->InsertBefore(1, theObj);
        theLastIndex = 1;
        theLastOrder = theOrder;
        return;
      }
      // get next object and compare with them
      occ::handle<TObj_Object> aNext = theHSeqOfObj->Value(theLastIndex);
      theLastOrder              = aNext->GetOrder();
    }
    // add object after current position
    theHSeqOfObj->InsertAfter(theLastIndex, theObj);
    theLastIndex++;
    theLastOrder = theOrder;
    return;
  }
}

occ::handle<TObj_ObjectIterator> TObj_Object::GetChildren(const occ::handle<Standard_Type>& theType) const
{
  occ::handle<TObj_ObjectIterator> anItr =
    new TObj_OcafObjectIterator(GetChildLabel(), theType, true);
  if (!TestFlags(ObjectState_Ordered))
    return anItr;
  // return object according to their order
  int               aLastIndex = 0;
  int               aLastOrder = 0;
  occ::handle<NCollection_HSequence<occ::handle<TObj_Object>>> aHSeqOfObj = new NCollection_HSequence<occ::handle<TObj_Object>>();
  for (; anItr->More(); anItr->Next())
  {
    occ::handle<TObj_Object> anObj = anItr->Value();
    if (anObj.IsNull())
      continue;
    int anOrder = anObj->GetOrder();
    if (!aLastIndex)
    {
      aHSeqOfObj->Append(anObj);
      aLastIndex = 1;
      aLastOrder = anOrder;
    }
    else
      addObjToOrderSequence(anObj,
                            anOrder,
                            aHSeqOfObj,
                            aHSeqOfObj->Length(),
                            aLastIndex,
                            aLastOrder);
  }
  return new TObj_SequenceIterator(aHSeqOfObj);
}

//=======================================================================
// function : getLabelByRank
// purpose  : Auxiliary function to get a label and attach a name to it
//           Used in debug mode only
//=======================================================================

#ifdef DFBROWSE
static TDF_Label getLabelByRank(const TDF_Label&       theL,
                                const int theRank,
                                const char* const theName)
{
  TDF_Label L = theL.FindChild(theRank, false);
  if (L.IsNull())
  {
    L = theL.FindChild(theRank, true);
    TDataStd_Name::Set(L, theName);
  }
  return L;
}
#endif

//=================================================================================================

TDF_Label TObj_Object::GetChildLabel() const
{
#ifdef DFBROWSE
  return getLabelByRank(GetLabel(), 4, "Children");
#else
  return GetLabel().FindChild(4, true);
#endif
}

//=================================================================================================

TDF_Label TObj_Object::getChildLabel(const int theRank) const
{
  TDF_Label aLabel = GetChildLabel();
  if (theRank > 0)
    aLabel = aLabel.FindChild(theRank, true);
  return aLabel;
}

//=================================================================================================

TDF_Label TObj_Object::GetLabel() const
{
  return myLabel;
}

//=================================================================================================

bool TObj_Object::SetName(const occ::handle<TCollection_HExtendedString>& theName) const
{
  // check if the name is exactly the same
  occ::handle<TCollection_HExtendedString> anOldName = GetName();
  if (!anOldName.IsNull() && theName->String().IsEqual(anOldName->String()))
    return true;

  // check if name is already registered and do nothing in that case
  const occ::handle<TObj_TNameContainer> aDictionary = GetDictionary();
  occ::handle<TObj_Model>                aModel      = GetModel();
  if (aModel->IsRegisteredName(theName, aDictionary))
    return false;

  // change name and update registry
  if (!anOldName.IsNull())
    aModel->UnRegisterName(anOldName, aDictionary);
  if (theName.IsNull())
    GetLabel().ForgetAttribute(TDataStd_Name::GetID());
  else
  {
    aModel->RegisterName(theName, GetLabel(), aDictionary);
    TDataStd_Name::Set(GetLabel(), theName->String());
  }
  return true;
}

//=================================================================================================

bool TObj_Object::SetName(const occ::handle<TCollection_HAsciiString>& theName) const
{
  occ::handle<TCollection_HExtendedString> aName = new TCollection_HExtendedString(theName);
  return SetName(aName);
}

//=================================================================================================

bool TObj_Object::SetName(const char* const theName) const
{
  occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString(theName);
  return SetName(aName);
}

//=================================================================================================

occ::handle<TCollection_HExtendedString> TObj_Object::GetName() const
{
  occ::handle<TCollection_HExtendedString> aName;
  occ::handle<TDataStd_Name>               A;
  if (GetLabel().FindAttribute(TDataStd_Name::GetID(), A))
    aName = new TCollection_HExtendedString(A->Get());
  else
    aName = new TCollection_HExtendedString("");
  return aName;
}

//=================================================================================================

bool TObj_Object::GetName(TCollection_ExtendedString& theStr) const
{
  occ::handle<TCollection_HExtendedString> aName = GetName();
  theStr                                    = aName->String();
  return theStr.Length() != 0;
}

//=================================================================================================

bool TObj_Object::GetName(TCollection_AsciiString& theName) const
{
  occ::handle<TCollection_HExtendedString> aName = GetName();
  if (aName.IsNull())
    return false;
  theName = TCollection_AsciiString(aName->String());
  return theName.Length() != 0;
}

//=================================================================================================

bool TObj_Object::HasReference(const occ::handle<TObj_Object>& theObject) const
{
  if (theObject.IsNull())
    return false;
  occ::handle<TObj_ObjectIterator> anItr = GetReferences(theObject->DynamicType());
  if (anItr.IsNull() || !anItr->More())
    return false;
  for (; anItr->More(); anItr->Next())
    if (anItr->Value() == theObject)
      return true;
  return false;
}

//=================================================================================================

occ::handle<TObj_ObjectIterator> TObj_Object::GetReferences(const occ::handle<Standard_Type>& theType) const
{
  return new TObj_ReferenceIterator(GetReferenceLabel(), theType);
}

//=================================================================================================

void TObj_Object::RemoveAllReferences()
{
  GetReferenceLabel().ForgetAllAttributes();
  // other implementation may be get all reference by iterator
  // and replace all of them by null handle with help of ::ReplaceReference
}

//=================================================================================================

void TObj_Object::AddBackReference(const occ::handle<TObj_Object>& theObject)
{
  if (myHSeqBackRef.IsNull())
    myHSeqBackRef = new NCollection_HSequence<occ::handle<TObj_Object>>;

  myHSeqBackRef->Append(theObject);
}

//=================================================================================================

void TObj_Object::RemoveBackReference(const occ::handle<TObj_Object>& theObject,
                                      const bool     theSingleOnly)
{
  if (myHSeqBackRef.IsNull()) // to avoid exception.
    return;

  for (int i = 1; i <= myHSeqBackRef->Length(); i++)
  {
    if (theObject != myHSeqBackRef->Value(i))
      continue;

    myHSeqBackRef->Remove(i--);
    if (theSingleOnly)
      break;
  }
  if (myHSeqBackRef->Length() < 1)
    myHSeqBackRef.Nullify(); // do not need to store empty sequence.
}

//=================================================================================================

occ::handle<TObj_ObjectIterator> TObj_Object::GetBackReferences(
  const occ::handle<Standard_Type>& theType) const
{
  return new TObj_SequenceIterator(myHSeqBackRef, theType);
}

//=================================================================================================

void TObj_Object::ClearBackReferences()
{
  myHSeqBackRef.Nullify();
}

//=================================================================================================

bool TObj_Object::HasBackReferences() const
{
  occ::handle<TObj_ObjectIterator> anItr = GetBackReferences();
  if (anItr.IsNull() || !anItr->More())
    return false;
  return true;
}

//=================================================================================================

bool TObj_Object::CanRemoveReference(const occ::handle<TObj_Object>& /*theObject*/) const
{
  return false;
}

//=================================================================================================

void TObj_Object::RemoveReference(const occ::handle<TObj_Object>& theObject)
{
  occ::handle<TObj_Object> aNullObj;
  ReplaceReference(theObject, aNullObj);
}

//=================================================================================================

bool TObj_Object::CanDetach(const TObj_DeletingMode theMode)
{
  if (!IsAlive())
    return false;

  occ::handle<TObj_ObjectIterator> aRefs = GetBackReferences();

  // Free Object can be deleted in any Mode
  if (aRefs.IsNull() || !aRefs->More())
    return true;

  if (theMode == TObj_FreeOnly)
    return false;

  if (theMode == TObj_Forced)
    return true;

  // check the last KeepDepending mode
  occ::handle<TObj_Object> aMe = this;
  for (; aRefs->More(); aRefs->Next())
  {
    occ::handle<TObj_Object> anObject = aRefs->Value();
    if (!anObject->CanRemoveReference(aMe))
      return false; // one of objects could not be unlinked
  }

  return true;
}

//=================================================================================================

bool TObj_Object::Detach(const TObj_DeletingMode theMode)
{
  if (!IsAlive())
    return false;

  // if object can not be deleted returns False
  if (!RemoveBackReferences(theMode))
    return false;

  occ::handle<TCollection_HExtendedString> anOldName = GetName();

  // detaching childs
  occ::handle<TObj_ObjectIterator> aChildren = GetChildren();

  for (; aChildren->More(); aChildren->Next())
    aChildren->Value()->Detach(theMode);

  // Clearing its own data
  GetReferenceLabel().ForgetAllAttributes();
  // clear back references container
  ClearBackReferences();
  // remove data
  GetDataLabel().ForgetAllAttributes();

  if (!anOldName.IsNull())
  {
    const occ::handle<TObj_TNameContainer> aDictionary = GetDictionary();
    // unregister only it is registered to me.
    if (!aDictionary.IsNull() && aDictionary->IsRegistered(anOldName))
    {
      TDF_Label aRegisteredLabel = aDictionary->Get().Find(anOldName);
      if (!aRegisteredLabel.IsNull() && aRegisteredLabel == GetLabel())
        aDictionary->RemoveName(anOldName);
    }
  }
  GetLabel().ForgetAllAttributes();

  return true;
}

//=================================================================================================

bool TObj_Object::Detach(const TDF_Label& theLabel, const TObj_DeletingMode theMode)
{
  occ::handle<TObj_Object> anObject;
  if (GetObj(theLabel, anObject))
    return anObject->Detach(theMode);
  return true;
}

//=================================================================================================

bool TObj_Object::GetObj(const TDF_Label&       theLabel,
                                     occ::handle<TObj_Object>&   theResult,
                                     const bool isSuper)
{
  if (theLabel.IsNull())
    return false;

  occ::handle<TObj_TObject> A;

  // find on the current label
  if (theLabel.FindAttribute(TObj_TObject::GetID(), A))
    theResult = A->Get();
  else
    theResult.Nullify();

  if (!theResult.IsNull())
  {
    if (!theResult->myLabel.IsNull())
      return true;

    // if the object is not allive then it is a wrong data in the Data Model
    theResult.Nullify();
  }
  else if (isSuper)
  {
    // try to get object from the father label
    return GetObj(theLabel.Father(), theResult, isSuper);
  }

  return false;
}

//=======================================================================
// function : GetFatherObject
// purpose  : Returns the father object, which may be NULL
//           theType gives type of father object to search
//=======================================================================

occ::handle<TObj_Object> TObj_Object::GetFatherObject(const occ::handle<Standard_Type>& theType) const
{
  occ::handle<TObj_Object> aFather;

  if (myLabel.IsNull())
    return aFather;

  occ::handle<TObj_Object> aSon(this);
  while (aSon->GetObj(aSon->GetLabel().Father(), aFather, true))
  {
    if (theType.IsNull() || aFather->IsKind(theType))
      break;
    else
    {
      aSon = aFather;
      aFather.Nullify();
    }
  }

  return aFather;
}

//=================================================================================================

void TObj_Object::AfterRetrieval()
{
  // Register the name
  occ::handle<TObj_Model> aModel = GetModel();
  if (!aModel.IsNull())
    aModel->RegisterName(GetName(), GetLabel(), GetDictionary());
}

//=================================================================================================

void TObj_Object::BeforeStoring() {}

//=================================================================================================

TDF_Label TObj_Object::GetReferenceLabel() const
{
#ifdef DFBROWSE
  return getLabelByRank(GetLabel(), 1, "References");
#else
  return GetLabel().FindChild(1, true);
#endif
}

//=================================================================================================

TDF_Label TObj_Object::GetDataLabel() const
{
#ifdef DFBROWSE
  return getLabelByRank(GetLabel(), 3, "Data");
#else
  return GetLabel().FindChild(3, true);
#endif
}

//=================================================================================================

TDF_Label TObj_Object::getDataLabel(const int theRank1,
                                    const int theRank2) const
{
  TDF_Label aLabel;
  if (theRank1 > 0) // protection
  {
    aLabel = GetDataLabel().FindChild(theRank1, true);
    if (theRank2 > 0)
      aLabel = aLabel.FindChild(theRank2, true);
  }
  return aLabel;
}

//=================================================================================================

TDF_Label TObj_Object::getReferenceLabel(const int theRank1,
                                         const int theRank2) const
{
  TDF_Label aLabel;
  if (theRank1 > 0) // protection
  {
    aLabel = GetReferenceLabel().FindChild(theRank1, true);
    if (theRank2 > 0)
      aLabel = aLabel.FindChild(theRank2, true);
  }
  return aLabel;
}

//=======================================================================
// function : isDataAttribute
// purpose  : Returns True if there is an attribute having theGUID on the
//           theRank2-th sublabel of theRank1-th sublabel of the Data
//           label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed, not
//           its sublabel
//=======================================================================

bool TObj_Object::isDataAttribute(const Standard_GUID&   theGUID,
                                              const int theRank1,
                                              const int theRank2) const
{
  return getDataLabel(theRank1, theRank2).IsAttribute(theGUID);
}

//=================================================================================================

double TObj_Object::getReal(const int theRank1,
                                   const int theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);

  occ::handle<TDataStd_Real> aReal;
  aLabel.FindAttribute(TDataStd_Real::GetID(), aReal);
  return aReal.IsNull() ? 0. : aReal->Get();
}

//=================================================================================================

bool TObj_Object::setReal(const double    theValue,
                                      const int theRank1,
                                      const int theRank2,
                                      const double    theTolerance) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);

  // check that value is actually changed
  occ::handle<TDataStd_Real> A;
  if (aLabel.FindAttribute(TDataStd_Real::GetID(), A) && fabs(A->Get() - theValue) <= theTolerance)
    return false;

  TDataStd_Real::Set(aLabel, theValue);
  return true;
}

//=================================================================================================

occ::handle<TCollection_HExtendedString> TObj_Object::getExtString(const int theRank1,
                                                              const int theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);

  occ::handle<TDataStd_Name> aName;
  aLabel.FindAttribute(TDataStd_Name::GetID(), aName);
  return aName.IsNull() ? 0 : new TCollection_HExtendedString(aName->Get());
}

//=================================================================================================

void TObj_Object::setExtString(const occ::handle<TCollection_HExtendedString>& theValue,
                               const int                     theRank1,
                               const int                     theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);
  if (!theValue.IsNull())
    TDataStd_Name::Set(aLabel, theValue->String());
  else
    aLabel.ForgetAttribute(TDataStd_Name::GetID());
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> TObj_Object::getAsciiString(const int theRank1,
                                                             const int theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);

  occ::handle<TDataStd_AsciiString> aStrAttr;
  aLabel.FindAttribute(TDataStd_AsciiString::GetID(), aStrAttr);
  return aStrAttr.IsNull() ? 0 : new TCollection_HAsciiString(aStrAttr->Get());
}

//=================================================================================================

void TObj_Object::setAsciiString(const occ::handle<TCollection_HAsciiString>& theValue,
                                 const int                  theRank1,
                                 const int                  theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);
  if (!theValue.IsNull())
    TDataStd_AsciiString::Set(aLabel, theValue->String());
  else
    aLabel.ForgetAttribute(TDataStd_AsciiString::GetID());
}

//=================================================================================================

int TObj_Object::getInteger(const int theRank1,
                                         const int theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);

  occ::handle<TDataStd_Integer> aNum;
  aLabel.FindAttribute(TDataStd_Integer::GetID(), aNum);
  return aNum.IsNull() ? 0 : aNum->Get();
}

//=================================================================================================

bool TObj_Object::setInteger(const int theValue,
                                         const int theRank1,
                                         const int theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1, theRank2);

  // check that value is actually changed
  occ::handle<TDataStd_Integer> A;
  if (aLabel.FindAttribute(TDataStd_Integer::GetID(), A) && A->Get() == theValue)
    return false;

  TDataStd_Integer::Set(aLabel, theValue);
  return true;
}

//=================================================================================================

occ::handle<TObj_Object> TObj_Object::getReference(const int theRank1,
                                              const int theRank2) const
{
  TDF_Label aLabel = getReferenceLabel(theRank1, theRank2);

  occ::handle<TObj_TReference> aRef;
  aLabel.FindAttribute(TObj_TReference::GetID(), aRef);
  return aRef.IsNull() ? occ::handle<TObj_Object>() : aRef->Get();
}

//=================================================================================================

bool TObj_Object::setReference(const occ::handle<TObj_Object>& theObject,
                                           const int     theRank1,
                                           const int     theRank2)
{
  TDF_Label aLabel = getReferenceLabel(theRank1, theRank2);

  if (theObject.IsNull())
    return aLabel.ForgetAttribute(TObj_TReference::GetID());

  // check that reference is actually changed
  occ::handle<TObj_TReference> A;
  if (aLabel.FindAttribute(TObj_TReference::GetID(), A) && A->Get() == theObject)
    return false;

  // 27.07.05, PTv: remove reference attribute before create new reference (for Undo/Redo)
  aLabel.ForgetAttribute(TObj_TReference::GetID());

  occ::handle<TObj_Object> me = this;
  TObj_TReference::Set(aLabel, theObject, me);
  return true;
}

//=================================================================================================

TDF_Label TObj_Object::addReference(const int     theRank1,
                                    const occ::handle<TObj_Object>& theObject)
{
  TDF_Label aRefLabel = GetReferenceLabel();
  if (theRank1 > 0)
    aRefLabel = aRefLabel.FindChild(theRank1, true);

  TDF_TagSource aTag;
  TDF_Label     aLabel = aTag.NewChild(aRefLabel);

  occ::handle<TObj_Object> me = this;
  TObj_TReference::Set(aLabel, theObject, me);
  return aLabel;
}

//=======================================================================
// function : getRealArray
// purpose  : Returns an existing or create a new real array on theRank2-th
//           sublabel of theRank1-th sublabel of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//           A newly created array has 1 and theLength bounds and is initialized
//           with zero
// WARNING  : call setArray() after array contents modification
//           in order to assure Undo work
//=======================================================================

occ::handle<NCollection_HArray1<double>> TObj_Object::getRealArray(const int theLength,
                                                        const int theRank1,
                                                        const int theRank2,
                                                        const double theInitialValue) const
{
  TDF_Label                  aLabel = getDataLabel(theRank1, theRank2);
  occ::handle<TDataStd_RealArray> anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_RealArray::GetID(), anArrAttribute))
    if (theLength > 0)
    {
      anArrAttribute = TDataStd_RealArray::Set(aLabel, 1, theLength);
      anArrAttribute->Array()->Init(theInitialValue);
    }
  occ::handle<NCollection_HArray1<double>> anArr;
  if (!anArrAttribute.IsNull())
    anArr = anArrAttribute->Array();
  return anArr;
}

//=======================================================================
// function : getIntegerArray
// purpose  : Returns an existing or create a new integer array on theRank2-th
//           sublabel of theRank1-th sublabel of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//           A newly created array has 1 and theLength bounds and is initialized
//           with zero
// WARNING  : call setArray() after array contents modification
//           in order to assure Undo work
//=======================================================================

occ::handle<NCollection_HArray1<int>> TObj_Object::getIntegerArray(
  const int theLength,
  const int theRank1,
  const int theRank2,
  const int theInitialValue) const
{
  TDF_Label                     aLabel = getDataLabel(theRank1, theRank2);
  occ::handle<TDataStd_IntegerArray> anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anArrAttribute))
    if (theLength > 0)
    {
      anArrAttribute = TDataStd_IntegerArray::Set(aLabel, 1, theLength);
      anArrAttribute->Array()->Init(theInitialValue);
    }
  occ::handle<NCollection_HArray1<int>> anArr;
  if (!anArrAttribute.IsNull())
    anArr = anArrAttribute->Array();
  return anArr;
}

//=======================================================================
// function : getExtStringArray
// purpose  : Returns an existing or create a new string array on theRank2-th
//           sublabel of theRank1-th sublabel of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//           A newly created array has 1 and theLength bounds
//           NOTE: new created array is NOT initialized.
// WARNING  : call setArray() after array contents modification
//           in order to assure Undo work
//=======================================================================

occ::handle<NCollection_HArray1<TCollection_ExtendedString>> TObj_Object::getExtStringArray(
  const int theLength,
  const int theRank1,
  const int theRank2) const
{
  TDF_Label                       aLabel = getDataLabel(theRank1, theRank2);
  occ::handle<TDataStd_ExtStringArray> anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), anArrAttribute))
    if (theLength > 0)
      anArrAttribute = TDataStd_ExtStringArray::Set(aLabel, 1, theLength);

  occ::handle<NCollection_HArray1<TCollection_ExtendedString>> anArr;
  if (!anArrAttribute.IsNull())
    anArr = anArrAttribute->Array();
  return anArr;
}

//=======================================================================
// function : setArray
// purpose  : Store theArray on theRank2-th sublabel of theRank1-th sublabel
//           of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//=======================================================================

void TObj_Object::setArray(const occ::handle<NCollection_HArray1<double>>& theArray,
                           const int               theRank1,
                           const int               theRank2)
{
  TDF_Label                  aLabel = getDataLabel(theRank1, theRank2);
  occ::handle<TDataStd_RealArray> anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_RealArray::GetID(), anArrAttribute) && !theArray.IsNull())
    anArrAttribute = TDataStd_RealArray::Set(aLabel, 1, 1);

  if (theArray.IsNull())
  {
    // deletion mode
    if (!anArrAttribute.IsNull())
      aLabel.ForgetAttribute(anArrAttribute);
    return;
  }

  if (anArrAttribute->Array() == theArray)
    // Backup wont happen but we want it
    anArrAttribute->Init(1, 1);

  anArrAttribute->ChangeArray(theArray);
}

//=======================================================================
// function : setArray
// purpose  : Store theArray on theRank2-th sublabel of theRank1-th sublabel
//           of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//=======================================================================

void TObj_Object::setArray(const occ::handle<NCollection_HArray1<int>>& theArray,
                           const int                  theRank1,
                           const int                  theRank2)
{
  TDF_Label                     aLabel = getDataLabel(theRank1, theRank2);
  occ::handle<TDataStd_IntegerArray> anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anArrAttribute) && !theArray.IsNull())
    anArrAttribute = TDataStd_IntegerArray::Set(aLabel, 1, 1);

  if (theArray.IsNull())
  {
    // deletion mode
    if (!anArrAttribute.IsNull())
      aLabel.ForgetAttribute(anArrAttribute);
    return;
  }

  if (anArrAttribute->Array() == theArray)
    // Backup wont happen but we want it
    anArrAttribute->Init(1, 1);

  anArrAttribute->ChangeArray(theArray);
}

//=======================================================================
// function : setArray
// purpose  : Store theArray on theRank2-th sublabel of theRank1-th sublabel
//           of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//=======================================================================

void TObj_Object::setArray(const occ::handle<NCollection_HArray1<TCollection_ExtendedString>>& theArray,
                           const int                         theRank1,
                           const int                         theRank2)
{
  TDF_Label                       aLabel = getDataLabel(theRank1, theRank2);
  occ::handle<TDataStd_ExtStringArray> anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), anArrAttribute) && !theArray.IsNull())
    anArrAttribute = TDataStd_ExtStringArray::Set(aLabel, 1, 1);

  if (theArray.IsNull())
  {
    // deletion mode
    if (!anArrAttribute.IsNull())
      aLabel.ForgetAttribute(anArrAttribute);
    return;
  }

  if (anArrAttribute->Array() == theArray)
    // Backup wont happen but we want it
    anArrAttribute->Init(1, 1);

  anArrAttribute->ChangeArray(theArray);
}

//=======================================================================
// function : copyTagSources
// purpose  : copy TagSource attributes on label and its sublabels
//=======================================================================

static void copyTagSources(const TDF_Label& theSourceLabel, const TDF_Label& theTargetLabel)
{
  // copy tag source on current label
  occ::handle<TDF_Attribute> anAttr;
  if (theSourceLabel.FindAttribute(TDF_TagSource::GetID(), anAttr))
  {
    occ::handle<TDF_TagSource> aTagSource       = occ::down_cast<TDF_TagSource>(anAttr);
    occ::handle<TDF_TagSource> aTargetTagSource = TDF_TagSource::Set(theTargetLabel);
    aTargetTagSource->Set(aTagSource->Get());
  }

  // copy recursively to sub-labels; note that iteration is made by target label,
  // to avoid copying tag sources where data are not copied
  TDF_ChildIterator aLI(theTargetLabel);
  for (; aLI.More(); aLI.Next())
  {
    TDF_Label aSourceLabel = theSourceLabel.FindChild(aLI.Value().Tag(), false);
    if (!aSourceLabel.IsNull())
      copyTagSources(aSourceLabel, aLI.Value());
  }
}

//=================================================================================================

occ::handle<TObj_Object> TObj_Object::Clone(const TDF_Label&            theTargetLabel,
                                       occ::handle<TDF_RelocationTable> theRelocTable)
{
  occ::handle<TDF_RelocationTable> aRelocTable = theRelocTable;
  if (theRelocTable.IsNull())
    aRelocTable = new TDF_RelocationTable;
  occ::handle<TObj_Object> aNewObj;
  // take current model for restoring it after creating object.
  const occ::handle<TObj_Model>& aCurrentModel = TObj_Assistant::GetCurrentModel();

  // take target model
  occ::handle<TObj_Model>  aTargetModel;
  TDF_Label           aLabel = TDocStd_Document::Get(theTargetLabel)->Main();
  occ::handle<TObj_TModel> aModelAttr;
  if (aLabel.FindAttribute(TObj_TModel::GetID(), aModelAttr))
    aTargetModel = aModelAttr->Model();

  if (aCurrentModel != aTargetModel)
    TObj_Assistant::SetCurrentModel(aTargetModel);
  // crete new object
  aNewObj = TObj_Persistence::CreateNewObject(DynamicType()->Name(), theTargetLabel);

  if (!aNewObj.IsNull())
  {
    TObj_TObject::Set(theTargetLabel, aNewObj);

    // adding a record to the reloation table
    aRelocTable->SetRelocation(GetLabel(), theTargetLabel);

    // now set name of object.
    const occ::handle<TCollection_HExtendedString> aCloneName = GetNameForClone(aNewObj);
    if (!aCloneName.IsNull() && !aCloneName->IsEmpty())
      aNewObj->SetName(new TCollection_HExtendedString(aCloneName));

    // copy the data
    copyData(aNewObj);

    // copy children
    TDF_Label aTargetLabel = aNewObj->GetChildLabel();
    CopyChildren(aTargetLabel, aRelocTable);

    // copy TagSource for the children
    copyTagSources(GetChildLabel(), aTargetLabel);

    // copy the references
    if (theRelocTable.IsNull())
      CopyReferences(aNewObj, aRelocTable);
  }

  // restore the model for persistence.
  if (aCurrentModel != aTargetModel)
    TObj_Assistant::SetCurrentModel(aCurrentModel);

  return aNewObj;
}

//=================================================================================================

bool TObj_Object::copyData(const occ::handle<TObj_Object>& theTargetObject)
{
  bool IsDone = false;
  if (!theTargetObject->DynamicType()->SubType(DynamicType()))
    return IsDone;
  // init the copier by labels.
  TDF_Label aDataLabel    = GetDataLabel();
  TDF_Label aNewDataLabel = theTargetObject->GetDataLabel();
  // check if object has any data.
  if (aDataLabel.IsNull() || aNewDataLabel.IsNull())
    return IsDone;

  TDF_CopyLabel aCopier(aDataLabel, aNewDataLabel);
  aCopier.Perform();

  return aCopier.IsDone();
}

//=================================================================================================

void TObj_Object::CopyChildren(TDF_Label&                         theTargetLabel,
                               const occ::handle<TDF_RelocationTable>& theRelocTable)
{
  TDF_Label                   aSourceChildLabel = GetChildLabel();
  occ::handle<TObj_ObjectIterator> aChildren         = // GetChildren();
                                                  // clang-format off
    new TObj_OcafObjectIterator (aSourceChildLabel, NULL, true); // to support children on sublabels of child label
                                                  // clang-format on
  for (; aChildren->More(); aChildren->Next())
  {
    occ::handle<TObj_Object> aChild = aChildren->Value();
    if (!aChild.IsNull())
    {
      // to support childs on sublabels of sublabel of child label
      NCollection_Sequence<int> aTags;
      TDF_Label                 aCurChildLab = aChild->GetLabel();
      while (!aCurChildLab.IsNull() && aCurChildLab != aSourceChildLabel)
      {
        aTags.Append(aCurChildLab.Tag());
        aCurChildLab = aCurChildLab.Father();
      }
      TDF_Label aChildLabel = theTargetLabel;
      for (int i = aTags.Length(); i > 0; i--)
        aChildLabel = aChildLabel.FindChild(aTags.Value(i), true);

      aChild->Clone(aChildLabel, theRelocTable);
    }
  }
}

//=================================================================================================

void TObj_Object::CopyReferences(const occ::handle<TObj_Object>&         theTargetObject,
                                 const occ::handle<TDF_RelocationTable>& theRelocTable)
{
  // recursive copy of references
  occ::handle<TObj_ObjectIterator>
    aSrcChildren = // GetChildren();
                   // to support childs on sublabels of sublabel of child label
    new TObj_OcafObjectIterator(GetChildLabel(), NULL, true);
  for (; aSrcChildren->More(); aSrcChildren->Next())
  {
    occ::handle<TObj_Object> aSrcChild = aSrcChildren->Value();
    TDF_Label           aSrcL     = aSrcChild->GetLabel();
    TDF_Label           aDestLabel;
    if (!theRelocTable->HasRelocation(aSrcL, aDestLabel))
      continue;
    occ::handle<TObj_Object> aDstChild;
    if (!TObj_Object::GetObj(aDestLabel, aDstChild))
      continue;
    if (aDstChild.IsNull() || !aDstChild->IsAlive()
        || aSrcChild->DynamicType() != aDstChild->DynamicType())
      continue; // should not be with relocation table

    aSrcChild->CopyReferences(aDstChild, theRelocTable);
  }
  // copy of my references
  theTargetObject->GetReferenceLabel().ForgetAllAttributes(true);

  TDF_Label aTargetLabel = theTargetObject->GetReferenceLabel();
  copyReferences(GetReferenceLabel(), aTargetLabel, theRelocTable);
}

//=================================================================================================

void TObj_Object::copyReferences(const TDF_Label&                   theSourceLabel,
                                 TDF_Label&                         theTargetLabel,
                                 const occ::handle<TDF_RelocationTable>& theRelocTable)
{
  TDF_AttributeIterator anIter(theSourceLabel);
  for (; anIter.More(); anIter.Next())
  {
    occ::handle<TDF_Attribute> anAttr = anIter.Value()->NewEmpty();
    theTargetLabel.AddAttribute(anAttr);
    anIter.Value()->Paste(anAttr, theRelocTable);
  }
  TDF_ChildIterator aLI(theSourceLabel);
  TDF_Label         aTargetLabel;
  for (; aLI.More(); aLI.Next())
  {
    aTargetLabel = theTargetLabel.FindChild(aLI.Value().Tag(), true);
    copyReferences(aLI.Value(), aTargetLabel, theRelocTable);
  }
}

//=================================================================================================

void TObj_Object::ReplaceReference(const occ::handle<TObj_Object>& theOldObject,
                                   const occ::handle<TObj_Object>& theNewObject)
{
  occ::handle<TObj_LabelIterator> anItr = occ::down_cast<TObj_LabelIterator>(GetReferences());
  if (anItr.IsNull())
    return;
  // iterates on references.
  for (; anItr->More(); anItr->Next())
  {
    occ::handle<TObj_Object> anObj = anItr->Value();
    if (anObj != theOldObject)
      continue;

    TDF_Label aRefLabel = anItr->LabelValue();
    // if new object is null then simple remove reference.
    if (theNewObject.IsNull())
    {
      aRefLabel.ForgetAllAttributes();
      break;
    }
    // set reference to new object.
    occ::handle<TObj_Object> me = this;
    TObj_TReference::Set(aRefLabel, theNewObject, me);
    break;
  }
}

//=================================================================================================

bool TObj_Object::IsAlive() const
{
  if (myLabel.IsNull())
    return false;

  occ::handle<TObj_Object> anObj;
  if (!GetObj(myLabel, anObj))
    return false;

  return true;
}

//=================================================================================================

int TObj_Object::GetFlags() const
{
  return getInteger(DataTag_Flags);
}

//=================================================================================================

void TObj_Object::SetFlags(const int theMask)
{
  int aFlags = getInteger(DataTag_Flags);
  aFlags                  = aFlags | theMask;
  setInteger(aFlags, DataTag_Flags);
}

//=================================================================================================

bool TObj_Object::TestFlags(const int theMask) const
{
  int aFlags = getInteger(DataTag_Flags);
  return (aFlags & theMask) != 0;
}

//=================================================================================================

void TObj_Object::ClearFlags(const int theMask)
{
  int aFlags = getInteger(DataTag_Flags);
  aFlags                  = aFlags & (~theMask);
  setInteger(aFlags, DataTag_Flags);
}

//=================================================================================================

bool TObj_Object::RemoveBackReferences(const TObj_DeletingMode theMode)
{
  occ::handle<TObj_ObjectIterator> aRefs = GetBackReferences();

  // Free Object can be deleted in any Mode
  if (aRefs.IsNull() || !aRefs->More())
    return true;

  if (theMode == TObj_FreeOnly)
    return false;

  // Defining the sequence of objects which are referenced to this one. The
  // first sequence stores containers the second one object with strong
  // relation.
  NCollection_Sequence<occ::handle<TObj_Object>> aContainers;
  NCollection_Sequence<occ::handle<TObj_Object>> aStrongs;
  occ::handle<TObj_Object>   aMe = this;

  // Sorting the referencing objects
  for (; aRefs->More(); aRefs->Next())
  {
    occ::handle<TObj_Object> anObject = aRefs->Value();
    if (anObject.IsNull() || !anObject->IsAlive())
      continue;
    if (anObject->CanRemoveReference(aMe))
      aContainers.Append(anObject);
    else
      aStrongs.Append(anObject);
  }
  // Can not be removed without deletion of referenced objects mode
  if (theMode == TObj_KeepDepending && aStrongs.Length() > 0)
    return false;
  // Delete or link off the referencing objects
  int i;
  occ::handle<TDF_Data> anOwnData = GetLabel().Data();
  for (i = 1; i <= aContainers.Length(); i++)
  {
    occ::handle<TObj_Object> anObj = aContainers(i);
    if (anObj.IsNull() || anObj->GetLabel().IsNull())
      continue; // undead object on dead label
    occ::handle<TDF_Data> aData      = anObj->GetLabel().Data();
    bool aModifMode = aData->IsModificationAllowed();
    if (anOwnData != aData)
      aData->AllowModification(true);
    anObj->RemoveReference(aMe);
    if (anOwnData != aData)
      aData->AllowModification(aModifMode);
  }
  /* PTv 21.11.2006
  object from other document refers to current object and must be killed
  when current object become dead for just want to remove references to it
  if ( theMode != TObj_Forced ) // cause leads to exception when
    // try to remove objects during close document
  */
  for (i = 1; i <= aStrongs.Length(); i++)
  {
    occ::handle<TObj_Object> anObj = aStrongs(i);
    if (anObj.IsNull() || anObj->GetLabel().IsNull())
      continue; // undead object on dead label
    occ::handle<TDF_Data> aData      = anObj->GetLabel().Data();
    bool aModifMode = aData->IsModificationAllowed();
    if (anOwnData != aData)
      aData->AllowModification(true);
    anObj->Detach(theMode);
    if (anOwnData != aData)
      aData->AllowModification(aModifMode);
  }

  return true;
}

//=======================================================================
// function : RelocateReferences
// purpose  : Make that each reference pointing to a descendant label of
//           theFromRoot to point to an equivalent label under theToRoot.
//           Return False if a resulting reference does not point to
//           an TObj_Object
// Example  :
//           a referred object label = 0:3:24:7:2:7
//           theFromRoot             = 0:3:24
//           theToRoot               = 0:2
//           a new referred label    = 0:2:7:2:7
//=======================================================================

bool TObj_Object::RelocateReferences(const TDF_Label&       theFromRoot,
                                                 const TDF_Label&       theToRoot,
                                                 const bool theUpdateBackRefs)
{
  TDF_ChildIDIterator aRefIt(GetReferenceLabel(), TObj_TReference::GetID(), true);
  occ::handle<TObj_Object> anObj;
  for (; aRefIt.More(); aRefIt.Next())
  {
    occ::handle<TObj_TReference> aRef = occ::down_cast<TObj_TReference>(aRefIt.Value());

    TDF_Label aNewLabel, aLabel = aRef->GetLabel();
    if (aLabel.Data() != theFromRoot.Data() || aLabel.IsDescendant(theToRoot))
      continue; // need not to relocate

    TDF_Tool::RelocateLabel(aLabel, theFromRoot, theToRoot, aNewLabel);
    if (aNewLabel.IsNull() || !TObj_Object::GetObj(aNewLabel, anObj))
      return false;

    // care of back references
    if (theUpdateBackRefs)
    {
      occ::handle<TObj_Object> me = this;
      // a new referred object
      anObj->AddBackReference(me);
      // an old object
      anObj = aRef->Get();
      if (!anObj.IsNull())
        anObj->RemoveBackReference(me);
    }

    aRef->Set(aNewLabel, aRef->GetMasterLabel());
  }

  return true;
}

//=================================================================================================

bool TObj_Object::GetBadReference(const TDF_Label& theRoot,
                                              TDF_Label&       theBadReference) const
{
  TDF_ChildIDIterator aRefIt(GetReferenceLabel(), TObj_TReference::GetID(), true);
  occ::handle<TObj_Object> anObj;
  for (; aRefIt.More(); aRefIt.Next())
  {
    occ::handle<TObj_TReference> aRef = occ::down_cast<TObj_TReference>(aRefIt.Value());

    TDF_Label aLabel = aRef->GetLabel();
    if (aLabel.Data() == theRoot.Data() && !aLabel.IsDescendant(theRoot))
    {
      theBadReference = aLabel;
      return true;
    }
  }

  return false;
}

//=================================================================================================

int TObj_Object::GetTypeFlags() const
{
  return Visible;
}

//=======================================================================
// function : GetDictionary
// purpose  : default implementation
//=======================================================================

occ::handle<TObj_TNameContainer> TObj_Object::GetDictionary() const
{
  occ::handle<TObj_Model> aModel = GetModel();
  if (!aModel.IsNull())
    return aModel->GetDictionary();
  return NULL;
}

//=================================================================================================

bool TObj_Object::SetOrder(const int& theIndx)
{
  setInteger(theIndx, DataTag_Order);
  return true;
}

//=================================================================================================

int TObj_Object::GetOrder() const
{
  int order = getInteger(DataTag_Order);
  if (!order)
    order = GetLabel().Tag();
  return order;
}

//=================================================================================================

bool TObj_Object::HasModifications() const
{
  return (!IsAlive() ? false : GetLabel().MayBeModified());
}