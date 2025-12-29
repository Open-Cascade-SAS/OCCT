// Created on: 2004-11-23
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
#include <TObj_TReference.hxx>
#include <TObj_TObject.hxx>

#include <Standard_GUID.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_DeltaOnRemoval.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TObj_TReference, TDF_Attribute)

//=================================================================================================

TObj_TReference::TObj_TReference() = default;

//=================================================================================================

const Standard_GUID& TObj_TReference::GetID()
{
  static Standard_GUID theGUID("3bbefb44-e618-11d4-ba38-0060b0ee18ea");
  return theGUID;
}

//=================================================================================================

const Standard_GUID& TObj_TReference::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TObj_TReference> TObj_TReference::Set(const TDF_Label&                theLabel,
                                                  const occ::handle<TObj_Object>& theObject,
                                                  const occ::handle<TObj_Object>& theMaster)
{
  occ::handle<TObj_TReference> A;
  if (!theLabel.FindAttribute(TObj_TReference::GetID(), A))
  {
    A = new TObj_TReference;
    theLabel.AddAttribute(A);
  }
  else
  {
    occ::handle<TObj_Object> anObj = A->Get();
    if (!anObj.IsNull())
      anObj->RemoveBackReference(theMaster);
  }
  A->Set(theObject, theMaster->GetLabel());
  if (!theObject.IsNull())
    theObject->AddBackReference(theMaster);
  return A;
}

//=================================================================================================

void TObj_TReference::Set(const occ::handle<TObj_Object>& theElem, const TDF_Label& theMasterLabel)
{
  Backup();
  if (theElem.IsNull())
    myLabel.Nullify();
  else
    myLabel = theElem->GetLabel();

  myMasterLabel = theMasterLabel;
}

//=================================================================================================

void TObj_TReference::Set(const TDF_Label& theLabel, const TDF_Label& theMasterLabel)
{
  Backup();
  myLabel       = theLabel;
  myMasterLabel = theMasterLabel;
}

//=================================================================================================

occ::handle<TObj_Object> TObj_TReference::Get() const
{
  occ::handle<TObj_TObject> aTObject;
  occ::handle<TObj_Object>  anObject;
  // Take TObj_TObject from label and get from it TObj_Object
  if (myLabel.IsNull() || !myLabel.FindAttribute(TObj_TObject::GetID(), aTObject))
  {
    return anObject;
  }
  anObject = aTObject->Get();
  return anObject;
}

//=================================================================================================

occ::handle<TDF_Attribute> TObj_TReference::NewEmpty() const
{
  return new TObj_TReference();
}

//=================================================================================================

void TObj_TReference::Restore(const occ::handle<TDF_Attribute>& theWith)
{
  occ::handle<TObj_TReference> aReference = occ::down_cast<TObj_TReference>(theWith);
  myLabel                                 = aReference->myLabel;
  myMasterLabel                           = aReference->myMasterLabel;
}

//=================================================================================================

void TObj_TReference::Paste(const occ::handle<TDF_Attribute>&       theInto,
                            const occ::handle<TDF_RelocationTable>& RT) const
{
  occ::handle<TObj_TReference> aReference = occ::down_cast<TObj_TReference>(theInto);
  occ::handle<TObj_TObject>    aObject, aMasterTObj;
  if (myLabel.IsNull())
  {
    //  unvalidity if it necessary
    aReference->myLabel.Nullify();
    return;
  }

  // get new referenced object
  TDF_Label aRefLabel = myLabel;
  if (!RT->HasRelocation(myLabel, aRefLabel))
    aRefLabel = myLabel;
  aRefLabel.FindAttribute(TObj_TObject::GetID(), aObject);
  occ::handle<TObj_Object> anIObject;
  if (!aObject.IsNull())
    anIObject = aObject->Get();

  // find correct master label
  occ::handle<TObj_Object> aMasterObj;
  TObj_Object::GetObj(aReference->Label(), aMasterObj, true);
  TDF_Label aMasterLabel;
  if (!aMasterObj.IsNull())
    aMasterLabel = aMasterObj->GetLabel();
  if (aMasterLabel.IsNull() || !aMasterLabel.FindAttribute(TObj_TObject::GetID(), aMasterTObj))
    return;

  // set master and referenced label
  aReference->Set(anIObject, aMasterLabel);

  // update back references
  if (!anIObject.IsNull())
    anIObject->AddBackReference(aMasterTObj->Get());
}

//=======================================================================
// function : BeforeForget
// purpose  : for correct tranzaction mechanism.
//=======================================================================

void TObj_TReference::BeforeForget()
{
  // check if master object exist
  if (myMasterLabel.IsNull())
    return;

  // removing back reference
  occ::handle<TObj_Object>  aMasterObject;
  occ::handle<TObj_TObject> aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject))
    return;
  aMasterObject = aTObject->Get();

  occ::handle<TObj_Object> anObj = Get();
  if (anObj.IsNull())
    return;

  aMasterObject->BeforeForgetReference(GetLabel());
  anObj->RemoveBackReference(aMasterObject);
}

//=================================================================================================

bool TObj_TReference::BeforeUndo(const occ::handle<TDF_AttributeDelta>& theDelta,
                                 const bool /*isForced*/)
{
  if (!theDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
    return true;

  if (myMasterLabel.IsNull())
    return true;

  occ::handle<TObj_Object> anObject = Get();
  if (anObject.IsNull())
    return true;

  occ::handle<TObj_Object>  aMasterObject;
  occ::handle<TObj_TObject> aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject))
    return true;
  aMasterObject = aTObject->Get();

  if (!anObject.IsNull())
    anObject->RemoveBackReference(aMasterObject);

  return true;
}

//=================================================================================================

bool TObj_TReference::AfterUndo(const occ::handle<TDF_AttributeDelta>& theDelta,
                                const bool /*isForced*/)
{
  if (!theDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval)))
    return true;

  if (myMasterLabel.IsNull())
    return true;

  occ::handle<TObj_Object> anObject = Get();
  if (anObject.IsNull())
    return true;

  occ::handle<TObj_Object>  aMasterObject;
  occ::handle<TObj_TObject> aTObject;

  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject))
    return true;
  aMasterObject = aTObject->Get();

  if (!anObject.IsNull())
    anObject->AddBackReference(aMasterObject);

  return true;
}

//=================================================================================================

void TObj_TReference::AfterResume()
{
  if (myMasterLabel.IsNull())
    return;

  occ::handle<TObj_Object>  aMasterObject;
  occ::handle<TObj_TObject> aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject))
    return;
  aMasterObject                     = aTObject->Get();
  occ::handle<TObj_Object> anObject = Get();

  if (!anObject.IsNull())
    anObject->AddBackReference(aMasterObject);
}

//=================================================================================================

bool TObj_TReference::AfterRetrieval(const bool /*forceIt*/)
{
  if (myMasterLabel.IsNull())
    return true;

  occ::handle<TObj_Object>  anObject = Get();
  occ::handle<TObj_Object>  aMasterObject;
  occ::handle<TObj_TObject> aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject))
    return false;

  aMasterObject = aTObject->Get();
  if (!anObject.IsNull())
    anObject->AddBackReference(aMasterObject);

  return true;
}
