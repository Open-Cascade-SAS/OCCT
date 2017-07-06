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
#include <TObj_LabelIterator.hxx>
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
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_SequenceOfInteger.hxx>
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


IMPLEMENT_STANDARD_RTTIEXT(TObj_Object,Standard_Transient)

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================

TObj_Object::TObj_Object(const TDF_Label& theLabel,
                                 const Standard_Boolean theSetName)
     : myLabel(theLabel)
{
  Handle(TObj_Object) aMe = this;
  TObj_TObject::Set(myLabel, aMe);
  if (theSetName)
    TObj_Model::SetNewName(aMe);
}

//=======================================================================
//function : GetModel
//purpose  :
//=======================================================================

Handle(TObj_Model) TObj_Object::GetModel() const
{
  Handle(TObj_Model) aModel;
  // if object label is null object is not alive
  if ( myLabel.IsNull() )
    return aModel;

  //TDF_Label aLabel = TDocStd_Document::Get(myLabel)->Main();
  Handle(TDF_Data) aData = myLabel.Data();
  if (aData.IsNull())
    return aModel;
  
  // try get the document from owner attribute manually
  TDF_Label aLabel = aData->Root();
  Handle(TDocStd_Owner) anOwnerAttr;
  Handle(TDocStd_Document) aTDoc;
  if ( !aLabel.IsNull() && aLabel.FindAttribute( TDocStd_Owner::GetID(), anOwnerAttr ) )
    aTDoc = anOwnerAttr->GetDocument();
  if (aTDoc.IsNull())
    return aModel;
  
  // use main label of the document to find TObj model attribute
  aLabel = aTDoc->Main();
  Handle(TObj_TModel) aModelAttr;
  if (!aLabel.IsNull() && aLabel.FindAttribute(TObj_TModel::GetID(), aModelAttr))
    aModel = aModelAttr->Model();

  return aModel;
}

//=======================================================================
//function : GetChildren
//purpose  : Returns iterator for the child objects.
//           This method provides tree-like view of the objects hierarchy.
//           The references to other objects are not considered as children.
//           theType narrows a variety of iterated objects
//=======================================================================

static void addObjToOrderSequence( const Handle(TObj_Object)& theObj,
                                   const Standard_Integer         theOrder,
                                   Handle(TObj_HSequenceOfObject)& theHSeqOfObj,
                                   const Standard_Integer theHSeqLength,
                                   Standard_Integer& theLastIndex,
                                   Standard_Integer& theLastOrder )
{
  if ( theOrder > theLastOrder )
  {
    while ( theOrder > theLastOrder )
    {
      // get next object and compare with them
      if ( ++theLastIndex > theHSeqLength )
      {
        theHSeqOfObj->Append( theObj );
        theLastIndex = theHSeqLength + 1;
        theLastOrder = theOrder;
        return;
      }
      Handle(TObj_Object) aNext = theHSeqOfObj->Value(theLastIndex);
      theLastOrder = aNext->GetOrder();
    }
    // add before current position
    theHSeqOfObj->InsertBefore( theLastIndex, theObj );
    theLastOrder = theOrder;
  }
  else
  {
    while ( theOrder < theLastOrder )
    {
      if ( --theLastIndex < 1 )
      {
        theHSeqOfObj->InsertBefore( 1, theObj );
        theLastIndex = 1;
        theLastOrder = theOrder;
        return;
      }
      // get next object and compare with them
      Handle(TObj_Object) aNext = theHSeqOfObj->Value(theLastIndex);
      theLastOrder = aNext->GetOrder();
    }
    // add object after current position
    theHSeqOfObj->InsertAfter( theLastIndex, theObj );
    theLastIndex++;
    theLastOrder = theOrder;
    return;
  }
}

Handle(TObj_ObjectIterator) TObj_Object::GetChildren
                         (const Handle(Standard_Type)& theType) const
{
  Handle(TObj_ObjectIterator) anItr =
    new TObj_OcafObjectIterator(GetChildLabel(), theType, Standard_True);
  if ( !TestFlags( ObjectState_Ordered ) )
    return anItr;
  // return object according to their order
  Standard_Integer aLastIndex = 0;
  Standard_Integer aLastOrder = 0;
  Handle(TObj_HSequenceOfObject) aHSeqOfObj = new TObj_HSequenceOfObject();
  for ( ; anItr->More(); anItr->Next() )
  {
    Handle(TObj_Object) anObj = anItr->Value();
    if ( anObj.IsNull() )
      continue;
    Standard_Integer anOrder = anObj->GetOrder();
    if ( !aLastIndex )
    {
      aHSeqOfObj->Append( anObj );
      aLastIndex = 1;
      aLastOrder = anOrder;
    }
    else
      addObjToOrderSequence( anObj, anOrder, aHSeqOfObj, aHSeqOfObj->Length(),
                             aLastIndex, aLastOrder );
  }
  return new TObj_SequenceIterator( aHSeqOfObj );
}

//=======================================================================
//function : getLabelByRank
//purpose  : Auxiliary function to get a label and attache a name to it
//           Used in debug mode only
//=======================================================================

#ifdef DFBROWSE
static TDF_Label getLabelByRank(const TDF_Label& theL,
                                const Standard_Integer theRank,
                                const Standard_CString theName)
{
  TDF_Label L = theL.FindChild(theRank,Standard_False);
  if ( L.IsNull() )
  {
    L = theL.FindChild(theRank,Standard_True);
    TDataStd_Name::Set(L, theName);
  }
  return L;
}
#endif

//=======================================================================
//function : GetChildLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Object::GetChildLabel() const
{
#ifdef DFBROWSE
  return getLabelByRank(GetLabel(),4,"Children");
#else
  return GetLabel().FindChild ( 4, Standard_True );
#endif
}

//=======================================================================
//function : getChildLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Object::getChildLabel(const Standard_Integer theRank) const
{
  TDF_Label aLabel = GetChildLabel();
  if ( theRank > 0 )
    aLabel = aLabel.FindChild( theRank, Standard_True );
  return aLabel;
}

//=======================================================================
//function : GetLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Object::GetLabel() const
{
  return myLabel;
}

//=======================================================================
//function : SetName
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::SetName(const Handle(TCollection_HExtendedString)& theName) const
{
  // check if the name is exactly the same
  Handle(TCollection_HExtendedString) anOldName = GetName();
  if( !anOldName.IsNull() && theName->String().IsEqual(anOldName->String()) )
    return Standard_True;

  // check if name is already registered and do nothing in that case
  const Handle(TObj_TNameContainer) aDictionary = GetDictionary();
  Handle(TObj_Model) aModel = GetModel();
  if( aModel->IsRegisteredName( theName, aDictionary ) )
    return Standard_False;

  // change name and update registry
  if (!anOldName.IsNull())
    aModel->UnRegisterName( anOldName, aDictionary );
  if ( theName.IsNull() ) 
    GetLabel().ForgetAttribute ( TDataStd_Name::GetID() );
  else
  {
    aModel->RegisterName( theName, GetLabel(), aDictionary );
    TDataStd_Name::Set(GetLabel(),theName->String());
  }
  return Standard_True;
}

//=======================================================================
//function : SetName
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::SetName(const Handle(TCollection_HAsciiString)& theName) const
{
  return SetName( new TCollection_HExtendedString ( theName ) );
}

//=======================================================================
//function : SetName
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::SetName(const Standard_CString theName) const
{
  return SetName ( new TCollection_HExtendedString ( theName ) );
}

//=======================================================================
//function : GetName
//purpose  :
//=======================================================================

Handle(TCollection_HExtendedString) TObj_Object::GetName() const
{
  Handle(TCollection_HExtendedString) aName;
  Handle(TDataStd_Name) A;
  if (GetLabel().FindAttribute (TDataStd_Name::GetID(), A))
    aName = new TCollection_HExtendedString(A->Get());
  else aName = new TCollection_HExtendedString("");
  return aName;
}

//=======================================================================
//function : GetName
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::GetName(TCollection_ExtendedString& theStr) const
{
  Handle(TCollection_HExtendedString) aName = GetName();
  theStr = aName->String();
  return theStr.Length() != 0;
}

//=======================================================================
//function : GetName
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::GetName(TCollection_AsciiString& theName) const
{
  Handle(TCollection_HExtendedString) aName = GetName();
  if(aName.IsNull())
    return Standard_False;
  theName = TCollection_AsciiString (aName->String());
  return theName.Length() != 0;
}

//=======================================================================
//function : HasReference
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::HasReference
                         (const Handle(TObj_Object)& theObject) const
{
  if ( theObject.IsNull() )
    return Standard_False;
  Handle(TObj_ObjectIterator) anItr = GetReferences(theObject->DynamicType());
  if ( anItr.IsNull() || !anItr->More() )
    return Standard_False;
  for ( ; anItr->More(); anItr->Next() )
    if ( anItr->Value() == theObject )
      return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : GetReferences
//purpose  :
//=======================================================================

Handle(TObj_ObjectIterator) TObj_Object::GetReferences
                         (const Handle(Standard_Type)& theType) const
{
  return new TObj_ReferenceIterator(GetReferenceLabel(), theType);
}

//=======================================================================
//function : RemoveAllReferences
//purpose  :
//=======================================================================

void TObj_Object::RemoveAllReferences()
{
  GetReferenceLabel().ForgetAllAttributes();
  // other implementation may be get all reference by iterator
  // and replace all of them by null handle with help of ::ReplaceReference
}

//=======================================================================
//function : AddBackReference
//purpose  :
//=======================================================================

void TObj_Object::AddBackReference (const Handle(TObj_Object)& theObject)
{
  if (myHSeqBackRef.IsNull())
    myHSeqBackRef = new TObj_HSequenceOfObject;

  myHSeqBackRef->Append( theObject );
}

//=======================================================================
//function : RemoveBackReference
//purpose  :
//=======================================================================

void TObj_Object::RemoveBackReference (const Handle(TObj_Object)& theObject,
                                           const Standard_Boolean theSingleOnly)
{
  if (myHSeqBackRef.IsNull()) // to avoid exception.
    return;

  for (Standard_Integer i = 1; i <= myHSeqBackRef->Length(); i++)
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
//=======================================================================
//function : GetBackReferences
//purpose  :
//=======================================================================

Handle(TObj_ObjectIterator) TObj_Object::GetBackReferences
                         (const Handle(Standard_Type)& theType) const
{
  return new TObj_SequenceIterator( myHSeqBackRef, theType );
}

//=======================================================================
//function : ClearBackReferences
//purpose  :
//=======================================================================

void TObj_Object::ClearBackReferences ()
{
  myHSeqBackRef.Nullify();
}

//=======================================================================
//function : HasBackReferences
//purpose  : 
//=======================================================================

Standard_Boolean TObj_Object::HasBackReferences() const
{
  Handle(TObj_ObjectIterator) anItr = GetBackReferences();
  if ( anItr.IsNull() || !anItr->More() )
    return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : CanRemoveReference
//purpose  : 
//=======================================================================

Standard_Boolean TObj_Object::CanRemoveReference
  (const Handle(TObj_Object)& /*theObject*/) const
{
  return Standard_False;
}

//=======================================================================
//function : RemoveReference
//purpose  : 
//=======================================================================

void TObj_Object::RemoveReference (const Handle(TObj_Object)& theObject)
{
  Handle(TObj_Object) aNullObj;
  ReplaceReference (theObject, aNullObj);
}
  
//=======================================================================
//function : CanDetach
//purpose  :
//=======================================================================
 Standard_Boolean TObj_Object::CanDetach(const TObj_DeletingMode theMode )
{
  if( !IsAlive() )
    return Standard_False;
  
  Handle(TObj_ObjectIterator) aRefs = GetBackReferences();
  
  // Free Object can be deleted in any Mode
  if ( aRefs.IsNull() || ! aRefs->More() )
    return Standard_True;
  
  if( theMode == TObj_FreeOnly )
    return Standard_False;

  if( theMode == TObj_Forced )
    return Standard_True;

  // check the last KeepDepending mode
  Handle(TObj_Object) aMe = this;
  for( ; aRefs->More(); aRefs->Next()) 
  {
    Handle(TObj_Object) anObject = aRefs->Value();
    if (! anObject->CanRemoveReference(aMe) )
      return Standard_False; // one of objects could not be unlinked
  }
    
  return Standard_True;
}

//=======================================================================
//function : Detach
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::Detach(const TObj_DeletingMode theMode)
{
  if( !IsAlive() )
    return Standard_False;

  // if object can not be deleted returns False
  if(!RemoveBackReferences(theMode))
    return Standard_False;

  Handle(TCollection_HExtendedString) anOldName = GetName();

  // detaching childs
  Handle(TObj_ObjectIterator) aChildren = GetChildren();

  for(;aChildren->More(); aChildren->Next())
    aChildren->Value()->Detach(theMode);

  // Clearing its own data
  GetReferenceLabel().ForgetAllAttributes();
  // clear back references container
  ClearBackReferences();
  // remove data
  GetDataLabel().ForgetAllAttributes();

  if (!anOldName.IsNull())
  {
    const Handle(TObj_TNameContainer) aDictionary = GetDictionary();
    // unregister only it is registered to me.
    if ( !aDictionary.IsNull() && aDictionary->IsRegistered( anOldName ) )
    {
      TDF_Label aRegisteredLabel = aDictionary->Get().Find( anOldName );
      if ( !aRegisteredLabel.IsNull() && aRegisteredLabel == GetLabel() )
        aDictionary->RemoveName( anOldName );
    }
  }
  GetLabel().ForgetAllAttributes();

  return Standard_True;
}

//=======================================================================
//function : Detach
//purpose  : public static method
//=======================================================================

Standard_Boolean TObj_Object::Detach(const TDF_Label& theLabel,
                                         const  TObj_DeletingMode theMode)
{
  Handle(TObj_Object) anObject;
  if( GetObj(theLabel, anObject) )
    return anObject->Detach(theMode);
  return Standard_True;
}

//=======================================================================
//function : GetObj
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::GetObj(const TDF_Label& theLabel,
                                         Handle(TObj_Object)& theResult,
                                         const Standard_Boolean isSuper)
{
  if(theLabel.IsNull())
    return Standard_False;

  Handle(TObj_TObject) A;

  // find on the current label
  if ( theLabel.FindAttribute(TObj_TObject::GetID(), A) )
    theResult = A->Get();
  else
    theResult.Nullify();

  if( !theResult.IsNull() )
  {
    if( !theResult->myLabel.IsNull() )
      return Standard_True;

    // if the object is not allive then it is a wrong data in the Data Model
    theResult.Nullify();
  }
  else if( isSuper )
  {
    // try to get object from the father label
    return GetObj(theLabel.Father(),theResult,isSuper);
  }

  return Standard_False;
}

//=======================================================================
//function : GetFatherObject
//purpose  : Returns the father object, which may be NULL
//           theType gives type of father object to search
//=======================================================================

Handle(TObj_Object) TObj_Object::GetFatherObject
                         (const Handle(Standard_Type)& theType) const
{
  Handle(TObj_Object) aFather, aSon(this);

  while ( aSon->GetObj( aSon->GetLabel().Father(), aFather, Standard_True ) )
  {
    if (theType.IsNull() || aFather->IsKind( theType ))
      break;
    else
    {
      aSon = aFather;
      aFather.Nullify();
    }
  }

  return aFather;
}


//=======================================================================
//function : AfterRetrieval
//purpose  :
//=======================================================================

void TObj_Object::AfterRetrieval()
{
  // Register the name
  Handle(TObj_Model) aModel = GetModel();
  if ( !aModel.IsNull() )
    aModel->RegisterName( GetName(), GetLabel(), GetDictionary() );
}

//=======================================================================
//function : BeforeStoring
//purpose  : base implementation
//=======================================================================

void TObj_Object::BeforeStoring()
{
}

//=======================================================================
//function : GetReferenceLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Object::GetReferenceLabel() const
{
#ifdef DFBROWSE
  return getLabelByRank(GetLabel(),1,"References");
#else
  return GetLabel().FindChild ( 1, Standard_True );
#endif
}

//=======================================================================
//function : GetDataLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Object::GetDataLabel() const
{
#ifdef DFBROWSE
  return getLabelByRank(GetLabel(),3,"Data");
#else
  return GetLabel().FindChild ( 3, Standard_True );
#endif
}

//=======================================================================
//function : getDataLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Object::getDataLabel (const Standard_Integer theRank1,
                                         const Standard_Integer theRank2) const
{
  TDF_Label aLabel;
  if ( theRank1 > 0 ) // protection
  {
    aLabel = GetDataLabel().FindChild ( theRank1, Standard_True );
    if ( theRank2 > 0 )
      aLabel = aLabel.FindChild ( theRank2, Standard_True );
  }
  return aLabel;
}

//=======================================================================
//function : getReferenceLabel
//purpose  :
//=======================================================================

TDF_Label TObj_Object::getReferenceLabel (const Standard_Integer theRank1,
                                              const Standard_Integer theRank2) const
{
  TDF_Label aLabel;
  if ( theRank1 > 0 ) // protection
  {
    aLabel = GetReferenceLabel().FindChild ( theRank1, Standard_True );
    if ( theRank2 > 0 )
      aLabel = aLabel.FindChild ( theRank2, Standard_True );
  }
  return aLabel;
}

//=======================================================================
//function : isDataAttribute
//purpose  : Returns True if there is an attribute having theGUID on the
//           theRank2-th sublabel of theRank1-th sublabel of the Data
//           label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed, not
//           its sublabel
//=======================================================================

Standard_Boolean TObj_Object::isDataAttribute
                        (const Standard_GUID&   theGUID,
                         const Standard_Integer theRank1,
                         const Standard_Integer theRank2) const
{
  return getDataLabel(theRank1,theRank2).IsAttribute(theGUID);
}

//=======================================================================
//function : getReal
//purpose  :
//=======================================================================

Standard_Real TObj_Object::getReal (const Standard_Integer theRank1,
                                        const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);

  Handle(TDataStd_Real) aReal;
  aLabel.FindAttribute ( TDataStd_Real::GetID(), aReal );
  return aReal.IsNull() ? 0. : aReal->Get();
}

//=======================================================================
//function : setReal
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::setReal (const Standard_Real theValue,
                                           const Standard_Integer theRank1,
                                           const Standard_Integer theRank2,
                                           const Standard_Real theTolerance) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);

  // check that value is actually changed
  Handle(TDataStd_Real) A;
  if ( aLabel.FindAttribute(TDataStd_Real::GetID(), A) &&
       fabs ( A->Get() - theValue ) <= theTolerance ) return Standard_False;

  TDataStd_Real::Set ( aLabel, theValue );
  return Standard_True;
}

//=======================================================================
//function : getExtString
//purpose  :
//=======================================================================

Handle(TCollection_HExtendedString) TObj_Object::getExtString
       (const Standard_Integer theRank1,
        const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);

  Handle(TDataStd_Name) aName;
  aLabel.FindAttribute ( TDataStd_Name::GetID(), aName );
  return aName.IsNull() ? 0 : new TCollection_HExtendedString(aName->Get());
}

//=======================================================================
//function : setExtString
//purpose  :
//=======================================================================

void TObj_Object::setExtString
  (const Handle(TCollection_HExtendedString)& theValue,
   const Standard_Integer theRank1,
   const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  if ( !theValue.IsNull() )
    TDataStd_Name::Set ( aLabel, theValue->String() );
  else
    aLabel.ForgetAttribute( TDataStd_Name::GetID() );
}

//=======================================================================
//function : getAsciiString
//purpose  :
//=======================================================================

Handle(TCollection_HAsciiString) TObj_Object::getAsciiString
       (const Standard_Integer theRank1,
        const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);

  Handle(TDataStd_AsciiString) aStrAttr;
  aLabel.FindAttribute ( TDataStd_AsciiString::GetID(), aStrAttr );
  return aStrAttr.IsNull() ? 0 : new TCollection_HAsciiString( aStrAttr->Get() );
}

//=======================================================================
//function : setAsciiString
//purpose  :
//=======================================================================

void TObj_Object::setAsciiString
  (const Handle(TCollection_HAsciiString)& theValue,
   const Standard_Integer theRank1,
   const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  if ( !theValue.IsNull() )
    TDataStd_AsciiString::Set ( aLabel, theValue->String() );
  else
    aLabel.ForgetAttribute( TDataStd_AsciiString::GetID() );
}

//=======================================================================
//function : getInteger
//purpose  :
//=======================================================================

Standard_Integer TObj_Object::getInteger (const Standard_Integer theRank1,
                                              const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);

  Handle(TDataStd_Integer) aNum;
  aLabel.FindAttribute ( TDataStd_Integer::GetID(), aNum );
  return aNum.IsNull() ? 0 : aNum->Get();
}

//=======================================================================
//function : setInteger
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::setInteger (const Standard_Integer theValue,
                                              const Standard_Integer theRank1,
                                              const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);

  // check that value is actually changed
  Handle(TDataStd_Integer) A;
  if ( aLabel.FindAttribute(TDataStd_Integer::GetID(), A) &&
       A->Get() == theValue ) return Standard_False;

  TDataStd_Integer::Set ( aLabel, theValue );
  return Standard_True;
}

//=======================================================================
//function : getReference
//purpose  :
//=======================================================================

Handle(TObj_Object) TObj_Object::getReference (const Standard_Integer theRank1,
                                                       const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getReferenceLabel(theRank1,theRank2);

  Handle(TObj_TReference) aRef;
  aLabel.FindAttribute ( TObj_TReference::GetID(), aRef );
  return aRef.IsNull() ? Handle(TObj_Object)() : aRef->Get();
}

//=======================================================================
//function : setReference
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::setReference (const Handle(TObj_Object) & theObject,
                                                const Standard_Integer theRank1,
                                                const Standard_Integer theRank2)
{
  TDF_Label aLabel = getReferenceLabel(theRank1,theRank2);

  if ( theObject.IsNull() )
    return aLabel.ForgetAttribute ( TObj_TReference::GetID() );
  
  // check that reference is actually changed
  Handle(TObj_TReference) A;
  if ( aLabel.FindAttribute(TObj_TReference::GetID(), A) &&
       A->Get() == theObject ) return Standard_False;

  // 27.07.05, PTv: remove reference attribute before create new reference (for Undo/Redo)
  aLabel.ForgetAttribute( TObj_TReference::GetID() );

  Handle(TObj_Object) me = this;
  TObj_TReference::Set ( aLabel, theObject, me);
  return Standard_True;
}

//=======================================================================
//function : addReference
//purpose  :
//=======================================================================

TDF_Label TObj_Object::addReference (const Standard_Integer theRank1,
                                         const Handle(TObj_Object) & theObject)
{
  TDF_Label aRefLabel = GetReferenceLabel();
  if ( theRank1 > 0 )
    aRefLabel = aRefLabel.FindChild ( theRank1, Standard_True );

  TDF_TagSource aTag;
  TDF_Label aLabel = aTag.NewChild(aRefLabel);

  Handle(TObj_Object) me = this;
  TObj_TReference::Set ( aLabel, theObject, me);
  return aLabel;
}

//=======================================================================
//function : getRealArray
//purpose  : Returns an existing or create a new real array on theRank2-th
//           sublabel of theRank1-th sublabel of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//           A newly created array has 1 and theLength bounds and is initialized
//           with zero
//WARNING  : call setArray() after array contents modification
//           in order to assure Undo work
//=======================================================================

Handle(TColStd_HArray1OfReal) TObj_Object::getRealArray
                        (const Standard_Integer theLength,
                         const Standard_Integer theRank1,
                         const Standard_Integer theRank2,
                         const Standard_Real    theInitialValue) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  Handle(TDataStd_RealArray) anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_RealArray::GetID(), anArrAttribute))
    if ( theLength > 0 )
    {
      anArrAttribute = TDataStd_RealArray::Set (aLabel, 1, theLength);
      anArrAttribute->Array()->Init( theInitialValue );
    }
  Handle(TColStd_HArray1OfReal) anArr;
  if ( !anArrAttribute.IsNull() )
    anArr = anArrAttribute->Array();
  return anArr;
}

//=======================================================================
//function : getIntegerArray
//purpose  : Returns an existing or create a new integer array on theRank2-th
//           sublabel of theRank1-th sublabel of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//           A newly created array has 1 and theLength bounds and is initialized
//           with zero
//WARNING  : call setArray() after array contents modification
//           in order to assure Undo work
//=======================================================================

Handle(TColStd_HArray1OfInteger) TObj_Object::getIntegerArray
                        (const Standard_Integer theLength,
                         const Standard_Integer theRank1,
                         const Standard_Integer theRank2,
                         const Standard_Integer theInitialValue) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  Handle(TDataStd_IntegerArray) anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anArrAttribute))
    if ( theLength > 0 )
    {
      anArrAttribute = TDataStd_IntegerArray::Set (aLabel, 1, theLength);
      anArrAttribute->Array()->Init( theInitialValue );
    }
  Handle(TColStd_HArray1OfInteger) anArr;
  if ( !anArrAttribute.IsNull() )
    anArr = anArrAttribute->Array();
  return anArr;
}

//=======================================================================
//function : getExtStringArray
//purpose  : Returns an existing or create a new string array on theRank2-th
//           sublabel of theRank1-th sublabel of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//           A newly created array has 1 and theLength bounds
//           NOTE: new created array is NOT initialized.
//WARNING  : call setArray() after array contents modification
//           in order to assure Undo work
//=======================================================================

Handle(TColStd_HArray1OfExtendedString) TObj_Object::getExtStringArray
                        (const Standard_Integer theLength,
                         const Standard_Integer theRank1,
                         const Standard_Integer theRank2) const
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  Handle(TDataStd_ExtStringArray) anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), anArrAttribute))
    if ( theLength > 0 )
      anArrAttribute = TDataStd_ExtStringArray::Set (aLabel, 1, theLength);

  Handle(TColStd_HArray1OfExtendedString) anArr;
  if ( !anArrAttribute.IsNull() )
    anArr = anArrAttribute->Array();
  return anArr;
}

//=======================================================================
//function : setArray
//purpose  : Store theArray on theRank2-th sublabel of theRank1-th sublabel
//           of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//=======================================================================

void TObj_Object::setArray (const Handle(TColStd_HArray1OfReal)& theArray,
                                const Standard_Integer theRank1,
                                const Standard_Integer theRank2)
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  Handle(TDataStd_RealArray) anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_RealArray::GetID(), anArrAttribute) &&
      !theArray.IsNull())
    anArrAttribute = TDataStd_RealArray::Set (aLabel, 1, 1);

  if (theArray.IsNull()) {
    // deletion mode 
    if (!anArrAttribute.IsNull())
      aLabel.ForgetAttribute(anArrAttribute);
    return;
  }

  if (anArrAttribute->Array() == theArray)
    // Backup wont happen but we want it
    anArrAttribute->Init(1,1);

  anArrAttribute->ChangeArray( theArray );
}

//=======================================================================
//function : setArray
//purpose  : Store theArray on theRank2-th sublabel of theRank1-th sublabel
//           of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//=======================================================================

void TObj_Object::setArray (const Handle(TColStd_HArray1OfInteger)& theArray,
                                const Standard_Integer theRank1,
                                const Standard_Integer theRank2)
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  Handle(TDataStd_IntegerArray) anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anArrAttribute) &&
      !theArray.IsNull())
    anArrAttribute = TDataStd_IntegerArray::Set (aLabel, 1, 1);

  if (theArray.IsNull()) {
    // deletion mode 
    if (!anArrAttribute.IsNull())
      aLabel.ForgetAttribute(anArrAttribute);
    return;
  }

  if (anArrAttribute->Array() == theArray)
    // Backup wont happen but we want it
    anArrAttribute->Init(1,1);

  anArrAttribute->ChangeArray( theArray );
}

//=======================================================================
//function : setArray
//purpose  : Store theArray on theRank2-th sublabel of theRank1-th sublabel
//           of the Data label of the object.
//           If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
//=======================================================================

void TObj_Object::setArray (const Handle(TColStd_HArray1OfExtendedString)& theArray,
                                const Standard_Integer theRank1,
                                const Standard_Integer theRank2)
{
  TDF_Label aLabel = getDataLabel(theRank1,theRank2);
  Handle(TDataStd_ExtStringArray) anArrAttribute;
  if (!aLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), anArrAttribute) &&
      !theArray.IsNull())
    anArrAttribute = TDataStd_ExtStringArray::Set (aLabel, 1, 1);

  if (theArray.IsNull()) {
    // deletion mode 
    if (!anArrAttribute.IsNull())
      aLabel.ForgetAttribute(anArrAttribute);
    return;
  }

  if (anArrAttribute->Array() == theArray)
    // Backup wont happen but we want it
    anArrAttribute->Init(1,1);

  anArrAttribute->ChangeArray( theArray );
}

//=======================================================================
//function : copyTagSources
//purpose  : copy TagSource attributes on label and its sublabels
//=======================================================================

static void copyTagSources (const TDF_Label& theSourceLabel, const TDF_Label& theTargetLabel)
{
  // copy tag source on current label
  Handle(TDF_Attribute) anAttr;
  if(theSourceLabel.FindAttribute(TDF_TagSource::GetID(), anAttr))
  {
    Handle(TDF_TagSource) aTagSource = Handle(TDF_TagSource)::DownCast(anAttr);
    Handle(TDF_TagSource) aTargetTagSource = TDF_TagSource::Set(theTargetLabel);
    aTargetTagSource->Set(aTagSource->Get());
  }

  // copy recursively to sub-labels; note that iteration is made by target label,
  // to avoid copying tag sources where data are not copied
  TDF_ChildIterator aLI(theTargetLabel);
  for(; aLI.More(); aLI.Next())
  {
    TDF_Label aSourceLabel = theSourceLabel.FindChild(aLI.Value().Tag(), Standard_False);
    if (! aSourceLabel.IsNull())
      copyTagSources (aSourceLabel, aLI.Value());
  }
}

//=======================================================================
//function : Clone
//purpose  :
//=======================================================================

Handle(TObj_Object) TObj_Object::Clone
       (const TDF_Label&            theTargetLabel,
        Handle(TDF_RelocationTable) theRelocTable)
{
  Handle(TDF_RelocationTable) aRelocTable = theRelocTable;
  if (theRelocTable.IsNull())
    aRelocTable = new TDF_RelocationTable;
  Handle(TObj_Object) aNewObj;
  // take current model for restoring it after creating object.
  const Handle(TObj_Model)& aCurrentModel = TObj_Assistant::GetCurrentModel();

  // take target model
  Handle(TObj_Model) aTargetModel;
  TDF_Label aLabel = TDocStd_Document::Get(theTargetLabel)->Main();
  Handle(TObj_TModel) aModelAttr;
  if (aLabel.FindAttribute(TObj_TModel::GetID(), aModelAttr))
    aTargetModel = aModelAttr->Model();

  if (aCurrentModel != aTargetModel)
    TObj_Assistant::SetCurrentModel (aTargetModel);
  // crete new object
  aNewObj = TObj_Persistence::CreateNewObject (DynamicType()->Name(), theTargetLabel);

  if (!aNewObj.IsNull())
  {
    TObj_TObject::Set(theTargetLabel,aNewObj);

    // adding a record to the reloation table
    aRelocTable->SetRelocation(GetLabel(), theTargetLabel);

    // now set name of object.
    const Handle(TCollection_HExtendedString) aCloneName = GetNameForClone( aNewObj );
    if( !aCloneName.IsNull() && !aCloneName->IsEmpty() )
      aNewObj->SetName( new TCollection_HExtendedString( aCloneName ) );

    // copy the data
    copyData (aNewObj);

    // copy children
    TDF_Label aTargetLabel = aNewObj->GetChildLabel();
    CopyChildren(aTargetLabel, aRelocTable);

    // copy TagSource for the children
    copyTagSources (GetChildLabel(), aTargetLabel);

    // copy the references
    if(theRelocTable.IsNull())
      CopyReferences(aNewObj, aRelocTable);
  }

  // restore the model for persistence.
  if (aCurrentModel != aTargetModel)
    TObj_Assistant::SetCurrentModel (aCurrentModel);

  return aNewObj;
}

//=======================================================================
//function : copyData
//purpose  : protected
//=======================================================================

Standard_Boolean TObj_Object::copyData
                (const Handle(TObj_Object)& theTargetObject)
{
  Standard_Boolean IsDone = Standard_False;
  if ( !theTargetObject->DynamicType()->SubType( DynamicType() ) )
    return IsDone;
  // init the copier by labels.
  TDF_Label aDataLabel = GetDataLabel();
  TDF_Label aNewDataLabel = theTargetObject->GetDataLabel();
  // check if object has any data.
  if (aDataLabel.IsNull() || aNewDataLabel.IsNull())
    return IsDone;

  TDF_CopyLabel aCopier(aDataLabel, aNewDataLabel);
  aCopier.Perform();

  return aCopier.IsDone();
}

//=======================================================================
//function : CopyChildren
//purpose  :
//=======================================================================

void TObj_Object::CopyChildren
                (TDF_Label&                         theTargetLabel,
                 const Handle(TDF_RelocationTable)& theRelocTable)
{
  TDF_Label aSourceChildLabel = GetChildLabel();
  Handle(TObj_ObjectIterator) aChildren = // GetChildren();
    new TObj_OcafObjectIterator (aSourceChildLabel, NULL, Standard_True); // to support children on sublabels of child label
  for(;aChildren->More(); aChildren->Next())
  {
    Handle(TObj_Object) aChild = aChildren->Value();
    if(!aChild.IsNull())
    {
      // to support childs on sublabels of sublabel of child label
      TColStd_SequenceOfInteger aTags;
      TDF_Label aCurChildLab = aChild->GetLabel();
      while ( !aCurChildLab.IsNull() && aCurChildLab != aSourceChildLabel )
      {
        aTags.Append( aCurChildLab.Tag() );
        aCurChildLab = aCurChildLab.Father();
      }
      TDF_Label aChildLabel = theTargetLabel;
      for ( Standard_Integer i = aTags.Length(); i > 0 ; i-- )
        aChildLabel = aChildLabel.FindChild( aTags.Value( i ), Standard_True );

      aChild->Clone(aChildLabel, theRelocTable);
    }
  }
}

//=======================================================================
//function : CopyReferences
//purpose  :
//=======================================================================

void TObj_Object::CopyReferences
                (const Handle(TObj_Object)& theTargetObject,
                 const Handle(TDF_RelocationTable)& theRelocTable)
{
  // recursive copy of references
  Handle(TObj_ObjectIterator) aSrcChildren = //GetChildren();
  // to support childs on sublabels of sublabel of child label
   new TObj_OcafObjectIterator(GetChildLabel(), NULL, Standard_True);
  for(; aSrcChildren->More(); aSrcChildren->Next())
  {
    Handle(TObj_Object) aSrcChild = aSrcChildren->Value();
    TDF_Label aSrcL = aSrcChild->GetLabel();
    TDF_Label aDestLabel;
    if( !theRelocTable->HasRelocation(aSrcL, aDestLabel) )
      continue;
    Handle(TObj_Object) aDstChild;
    if ( !TObj_Object::GetObj( aDestLabel, aDstChild ) )
      continue;
    if ( aDstChild.IsNull() || !aDstChild->IsAlive() || aSrcChild->DynamicType() != aDstChild->DynamicType() )
      continue; // should not be with relocation table

    aSrcChild->CopyReferences(aDstChild, theRelocTable);
  }
  // copy of my references
  theTargetObject->GetReferenceLabel().ForgetAllAttributes(Standard_True);

  TDF_Label aTargetLabel = theTargetObject->GetReferenceLabel();
  copyReferences(GetReferenceLabel(), aTargetLabel, theRelocTable);
}

//=======================================================================
//function : copyReferences
//purpose  : protected
//=======================================================================

void TObj_Object::copyReferences
                         (const TDF_Label&                   theSourceLabel,
                          TDF_Label&                         theTargetLabel,
                          const Handle(TDF_RelocationTable)& theRelocTable)
{
  TDF_AttributeIterator anIter(theSourceLabel);
  for(; anIter.More(); anIter.Next())
  {
    Handle(TDF_Attribute) anAttr = anIter.Value()->NewEmpty();
    theTargetLabel.AddAttribute(anAttr);
    anIter.Value()->Paste(anAttr, theRelocTable);

  }
  TDF_ChildIterator aLI(theSourceLabel);
  TDF_Label aTargetLabel;
  for(; aLI.More(); aLI.Next())
  {
    aTargetLabel = theTargetLabel.FindChild(aLI.Value().Tag(), Standard_True);
    copyReferences(aLI.Value(), aTargetLabel, theRelocTable);
  }
}

//=======================================================================
//function : ReplaceReference
//purpose  :
//=======================================================================

void TObj_Object::ReplaceReference (const Handle(TObj_Object)& theOldObject,
                                        const Handle(TObj_Object)& theNewObject)
{
  Handle(TObj_LabelIterator) anItr =
    Handle(TObj_LabelIterator)::DownCast( GetReferences() );
  if (anItr.IsNull())
    return;
  // iterates on references.
  for (; anItr->More(); anItr->Next() )
  {
    Handle(TObj_Object) anObj = anItr->Value();
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
    Handle(TObj_Object) me = this;
    TObj_TReference::Set ( aRefLabel, theNewObject, me);
    break;
  }
}

//=======================================================================
//function : IsAlive
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::IsAlive() const
{
  if (myLabel.IsNull())
    return Standard_False;

  Handle(TObj_Object) anObj;
  if ( !GetObj( myLabel, anObj ) )
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : GetFlags
//purpose  :
//=======================================================================

Standard_Integer TObj_Object::GetFlags() const
{
  return getInteger(DataTag_Flags);
}

//=======================================================================
//function : SetFlags
//purpose  :
//=======================================================================

void TObj_Object::SetFlags(const Standard_Integer theMask)
{
  Standard_Integer aFlags = getInteger(DataTag_Flags);
  aFlags = aFlags | theMask;
  setInteger(aFlags, DataTag_Flags);
}

//=======================================================================
//function : TestFlags
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::TestFlags(const Standard_Integer theMask) const
{
  Standard_Integer aFlags = getInteger(DataTag_Flags);
  return (aFlags & theMask) != 0;
}

//=======================================================================
//function : ClearFlags
//purpose  :
//=======================================================================

void TObj_Object::ClearFlags(const Standard_Integer theMask)
{
  Standard_Integer aFlags = getInteger(DataTag_Flags);
  aFlags = aFlags & (~theMask);
  setInteger(aFlags, DataTag_Flags);
}

//=======================================================================
//function : RemoveBackReferences
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::RemoveBackReferences(const TObj_DeletingMode theMode)
{
  Handle(TObj_ObjectIterator) aRefs = GetBackReferences();

  // Free Object can be deleted in any Mode
  if ( aRefs.IsNull() || !aRefs->More()) return Standard_True;

  if( theMode == TObj_FreeOnly) return Standard_False;

  // Defining the sequence of objects which are referenced to this one. The
  // first sequence stores containers the second one object with strong
  // relation.
  TObj_SequenceOfObject aContainers;
  TObj_SequenceOfObject aStrongs;
  Handle(TObj_Object) aMe = this;

  // Sorting the referencing objects
  for( ; aRefs->More() ; aRefs->Next())
  {
    Handle(TObj_Object) anObject = aRefs->Value();
    if ( anObject.IsNull() || !anObject->IsAlive() )
      continue;
    if ( anObject->CanRemoveReference(aMe) )
      aContainers.Append(anObject);
    else
      aStrongs.Append(anObject);
  }
  // Can not be removed without deletion of referenced objects mode
  if( theMode == TObj_KeepDepending && aStrongs.Length() > 0 )
    return Standard_False;
  // Delete or link off the referencing objects
  Standard_Integer i;
  Handle(TDF_Data) anOwnData = GetLabel().Data();
  for (i = 1; i <= aContainers.Length(); i++)
  {
    Handle(TObj_Object) anObj = aContainers(i);
    if ( anObj.IsNull() || anObj->GetLabel().IsNull() )
      continue; // undead object on dead label
    Handle(TDF_Data) aData = anObj->GetLabel().Data();
    Standard_Boolean aModifMode = aData->IsModificationAllowed();
    if ( anOwnData != aData )
      aData->AllowModification( Standard_True );
    anObj->RemoveReference(aMe);
    if ( anOwnData != aData )
      aData->AllowModification( aModifMode );
  }
  /* PTv 21.11.2006
  object from other document refers to current object and must be killed
  when current object become dead for just want to remove references to it
  if ( theMode != TObj_Forced ) // cause leads to exception when
    // try to remove objects during close document
  */
  for (i = 1; i <= aStrongs.Length(); i++)
  {
    Handle(TObj_Object) anObj = aStrongs(i);
    if ( anObj.IsNull() || anObj->GetLabel().IsNull() )
      continue; // undead object on dead label
    Handle(TDF_Data) aData = anObj->GetLabel().Data();
    Standard_Boolean aModifMode = aData->IsModificationAllowed();
    if ( anOwnData != aData )
      aData->AllowModification( Standard_True );
    anObj->Detach(theMode);
    if ( anOwnData != aData )
      aData->AllowModification( aModifMode );
  }

  return Standard_True;
}

//=======================================================================
//function : RelocateReferences
//purpose  : Make that each reference pointing to a descendant label of
//           theFromRoot to point to an equivalent label under theToRoot.
//           Return False if a resulting reference does not point to
//           an TObj_Object
//Example  :
//           a referred object label = 0:3:24:7:2:7
//           theFromRoot             = 0:3:24
//           theToRoot               = 0:2
//           a new referred label    = 0:2:7:2:7
//=======================================================================

Standard_Boolean TObj_Object::RelocateReferences
                         (const TDF_Label&       theFromRoot,
                          const TDF_Label&       theToRoot,
                          const Standard_Boolean theUpdateBackRefs)
{
  TDF_ChildIDIterator aRefIt (GetReferenceLabel(),
                              TObj_TReference::GetID(),
                              Standard_True );
  Handle(TObj_Object) anObj;
  for (  ; aRefIt.More(); aRefIt.Next() )
  {
    Handle(TObj_TReference) aRef =
      Handle(TObj_TReference)::DownCast( aRefIt.Value() );

    TDF_Label aNewLabel, aLabel = aRef->GetLabel();
    if ( aLabel.Data() != theFromRoot.Data() ||
         aLabel.IsDescendant( theToRoot ))
      continue; // need not to relocate

    TDF_Tool::RelocateLabel( aLabel, theFromRoot, theToRoot, aNewLabel );
    if ( aNewLabel.IsNull() || !TObj_Object::GetObj( aNewLabel, anObj ))
      return Standard_False;

    // care of back references
    if (theUpdateBackRefs)
    {
      Handle(TObj_Object) me = this;
      // a new referred object
      anObj->AddBackReference( me );
      // an old object
      anObj = aRef->Get();
      if (!anObj.IsNull())
        anObj->RemoveBackReference( me );
    }

    aRef->Set( aNewLabel, aRef->GetMasterLabel() );
  }

  return Standard_True;
}

//=======================================================================
//function : GetBadReference
//purpose  : 
//=======================================================================

Standard_Boolean TObj_Object::GetBadReference
                         (const TDF_Label& theRoot,
                          TDF_Label&       theBadReference) const
{
  TDF_ChildIDIterator aRefIt (GetReferenceLabel(),
                              TObj_TReference::GetID(),
                              Standard_True );
  Handle(TObj_Object) anObj;
  for (  ; aRefIt.More(); aRefIt.Next() )
  {
    Handle(TObj_TReference) aRef =
      Handle(TObj_TReference)::DownCast( aRefIt.Value() );

    TDF_Label aLabel = aRef->GetLabel();
    if ( aLabel.Data() == theRoot.Data() &&
         !aLabel.IsDescendant( theRoot ))
    {
      theBadReference = aLabel;
      return Standard_True;
    }
  }

  return Standard_False;
}

//=======================================================================
//function : TypeFlags
//purpose  : 
//=======================================================================

Standard_Integer TObj_Object::GetTypeFlags() const
{
  return Visible;
}

//=======================================================================
//function : GetDictionary
//purpose  : default implementation
//=======================================================================

Handle(TObj_TNameContainer) TObj_Object::GetDictionary() const
{
  Handle(TObj_Model) aModel = GetModel();
  if ( !aModel.IsNull() )
    return aModel->GetDictionary();
  return NULL; 
}

//=======================================================================
//function : SetOrder
//purpose  :
//=======================================================================

Standard_Boolean TObj_Object::SetOrder( const Standard_Integer& theIndx )
{ 
  setInteger( theIndx, DataTag_Order );
  return Standard_True;
}

//=======================================================================
//function : GetOrder
//purpose  :
//=======================================================================

Standard_Integer TObj_Object::GetOrder() const
{
  Standard_Integer order = getInteger( DataTag_Order );
  if ( !order )
    order = GetLabel().Tag();
  return order;
}
