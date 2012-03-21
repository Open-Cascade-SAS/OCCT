// Created on: 2004-11-23
// Created by: Pavel TELKOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Object.hxx>
#include <TObj_TReference.hxx>
#include <TObj_TObject.hxx>

#include <Standard_GUID.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_DeltaOnAddition.hxx>

IMPLEMENT_STANDARD_HANDLE(TObj_TReference,TDF_Attribute)
IMPLEMENT_STANDARD_RTTIEXT(TObj_TReference,TDF_Attribute)

//=======================================================================
//function : TObj_TReference
//purpose  : 
//=======================================================================

TObj_TReference::TObj_TReference()
{
}
    
//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TReference::GetID()
{
  static Standard_GUID theGUID ("3bbefb44-e618-11d4-ba38-0060b0ee18ea");
  return theGUID;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TObj_TReference::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TObj_TReference) TObj_TReference::Set
                (const TDF_Label& theLabel, 
                 const Handle(TObj_Object)& theObject,
                 const Handle(TObj_Object)& theMaster)
{
  Handle(TObj_TReference) A;
  if (!theLabel.FindAttribute(TObj_TReference::GetID(), A)) 
  {
    A = new TObj_TReference;
    theLabel.AddAttribute(A);
  }
  else
  {
    Handle(TObj_Object) anObj = A->Get();
    if ( ! anObj.IsNull() ) anObj->RemoveBackReference(theMaster);
  }
  A->Set(theObject, theMaster->GetLabel());
  if ( ! theObject.IsNull() )
    theObject->AddBackReference(theMaster);
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TObj_TReference::Set(const Handle(TObj_Object)& theElem,
                              const TDF_Label& theMasterLabel)
{
  Backup();
  if ( theElem.IsNull() ) 
    myLabel.Nullify();
  else 
    myLabel = theElem->GetLabel();

  myMasterLabel = theMasterLabel;
}

//=======================================================================
//function : Set
//purpose  : for persistent only.
//=======================================================================

void TObj_TReference::Set(const TDF_Label& theLabel,
                              const TDF_Label& theMasterLabel)
{
  Backup();
  myLabel = theLabel;
  myMasterLabel = theMasterLabel;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Handle(TObj_Object) TObj_TReference::Get() const
{
  Handle(TObj_TObject) aTObject;
  Handle(TObj_Object) anObject;
  // Take TObj_TObject from label and get from it TObj_Object
  if ( myLabel.IsNull() || ! myLabel.FindAttribute(TObj_TObject::GetID(), aTObject) ) 
  {
    return anObject;
  }
  anObject = Handle(TObj_Object)::DownCast(aTObject->Get());
  return anObject;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TObj_TReference::NewEmpty () const
{  
  return new TObj_TReference();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TObj_TReference::Restore(const Handle(TDF_Attribute)& theWith) 
{
  Handle(TObj_TReference) aReference = Handle(TObj_TReference)::DownCast (theWith);
  myLabel = aReference->myLabel;
  myMasterLabel = aReference->myMasterLabel;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TObj_TReference::Paste (const Handle(TDF_Attribute)& theInto,
                                 const Handle(TDF_RelocationTable)& RT) const
{ 
  Handle(TObj_TReference) aReference = Handle(TObj_TReference)::DownCast (theInto);
  Handle(TObj_TObject) aObject, aMasterTObj;
  if (myLabel.IsNull())
  {
    //  unvalidity if it neccessary
    aReference->myLabel.Nullify();
    return;
  }

  // get new referenced object
  TDF_Label aRefLabel = myLabel;
  if(!RT->HasRelocation(myLabel, aRefLabel)) 
    aRefLabel = myLabel;
  aRefLabel.FindAttribute(TObj_TObject::GetID(), aObject);
  Handle(TObj_Object) anIObject;
  if ( ! aObject.IsNull() ) 
    anIObject = aObject->Get();
  
  // find correct master label
  Handle(TObj_Object) aMasterObj;
  TObj_Object::GetObj(aReference->Label(), aMasterObj, Standard_True);
  TDF_Label aMasterLabel;
  if ( ! aMasterObj.IsNull() ) aMasterLabel = aMasterObj->GetLabel();
  if ( aMasterLabel.IsNull() || 
       ! aMasterLabel.FindAttribute(TObj_TObject::GetID(), aMasterTObj))
    return;

  // set master and referenced label
  aReference->Set(anIObject, aMasterLabel);

  // update back references
  if ( !anIObject.IsNull() )
    anIObject->AddBackReference(aMasterTObj->Get());
}

//=======================================================================
//function : BeforeForget
//purpose  : for correct tranzaction mechanism.
//=======================================================================

void TObj_TReference::BeforeForget()
{
  // check if master object exist
  if(myMasterLabel.IsNull())
    return;

  // removing back reference
  Handle(TObj_Object) aMasterObject;
  Handle(TObj_TObject) aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject ))
    return;
  aMasterObject = aTObject->Get();
  
  Handle(TObj_Object) anObj = Get();
  if(anObj.IsNull())
    return;
  
  aMasterObject->BeforeForgetReference( GetLabel() );
  anObj->RemoveBackReference( aMasterObject );
}

//=======================================================================
//function : BeforeUndo
//purpose  : 
//=======================================================================

Standard_Boolean TObj_TReference::BeforeUndo(const Handle(TDF_AttributeDelta)& theDelta,
                                                 const Standard_Boolean /*isForced*/)
{
  if (!theDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
    return Standard_True;

  if(myMasterLabel.IsNull())
    return Standard_True;

  Handle(TObj_Object) anObject = Get();
  if( anObject.IsNull() ) 
    return Standard_True;

  Handle(TObj_Object) aMasterObject;
  Handle(TObj_TObject) aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject ))
    return Standard_True;
  aMasterObject = aTObject->Get();

  if( !anObject.IsNull() )
    anObject->RemoveBackReference( aMasterObject );

  return Standard_True;
}

//=======================================================================
//function : AfterUndo
//purpose  : 
//=======================================================================

Standard_Boolean TObj_TReference::AfterUndo(const Handle(TDF_AttributeDelta)& theDelta,
                                                const Standard_Boolean /*isForced*/)
{
  if (!theDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnRemoval)))
    return Standard_True;

  if(myMasterLabel.IsNull())
    return Standard_True;

  Handle(TObj_Object) anObject = Get();
  if( anObject.IsNull() )
    return Standard_True;

  Handle(TObj_Object) aMasterObject;
  Handle(TObj_TObject) aTObject;

  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject ))
    return Standard_True;
  aMasterObject = aTObject->Get();

  if( !anObject.IsNull() )
    anObject->AddBackReference( aMasterObject );

  return Standard_True;
}


//=======================================================================
//function : AfterResume
//purpose  : 
//=======================================================================

void TObj_TReference::AfterResume()
{
  if(myMasterLabel.IsNull())
    return;

  Handle(TObj_Object) aMasterObject;
  Handle(TObj_TObject) aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject ))
    return;
  aMasterObject = aTObject->Get();
  Handle(TObj_Object) anObject = Get();

  if ( !anObject.IsNull() )
    anObject->AddBackReference( aMasterObject );
}

//=======================================================================
//function : AfterRetrieval
//purpose  : 
//=======================================================================

Standard_Boolean TObj_TReference::AfterRetrieval(const Standard_Boolean /*forceIt*/)
{
  if(myMasterLabel.IsNull())
    return Standard_True;

  Handle(TObj_Object) anObject = Get();
  Handle(TObj_Object) aMasterObject;
  Handle(TObj_TObject) aTObject;
  if (!myMasterLabel.FindAttribute(TObj_TObject::GetID(), aTObject ))
    return Standard_False;
  
  aMasterObject = aTObject->Get();
  if ( !anObject.IsNull() )
    anObject->AddBackReference( aMasterObject );

  return Standard_True;
}
