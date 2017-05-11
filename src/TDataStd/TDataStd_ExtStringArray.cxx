// Created on: 2002-01-16
// Created by: Michael PONIKAROV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_DeltaOnModificationOfExtStringArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_ExtStringArray,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ExtStringArray::GetID() 
{ 
  static Standard_GUID anExtStringArrayID ("2a96b624-ec8b-11d0-bee7-080009dc3333");
  return anExtStringArrayID; 
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
Handle(TDataStd_ExtStringArray) SetAttr(const TDF_Label&       label,
                                        const Standard_Integer lower,
                                        const Standard_Integer upper,
                                        const Standard_Boolean isDelta,
                                        const Standard_GUID&   theGuid) 
{
  Handle(TDataStd_ExtStringArray) A;
  if (!label.FindAttribute (theGuid, A)) 
  {
    A = new TDataStd_ExtStringArray;
    A->SetID(theGuid);
    A->Init (lower, upper); 
    A->SetDelta(isDelta); 
    label.AddAttribute(A);
  }
  else if (lower != A->Lower() || upper != A->Upper())
  {
    A->Init(lower, upper);
  }
  return A;
}
//=======================================================================
//function : TDataStd_ExtStringArray::TDataStd_ExtStringArray
//purpose  : 
//=======================================================================

TDataStd_ExtStringArray::TDataStd_ExtStringArray() 
    : myIsDelta(Standard_False){}
     
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::Init(const Standard_Integer lower,
                                   const Standard_Integer upper)
{
  Standard_RangeError_Raise_if(upper < lower,"TDataStd_ExtStringArray::Init");
  Backup();
  myValue = new TColStd_HArray1OfExtendedString(lower, upper, "");
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_ExtStringArray) TDataStd_ExtStringArray::Set (
                                          const TDF_Label& label,
                                          const Standard_Integer lower,
                                          const Standard_Integer upper,
                                          const Standard_Boolean isDelta) 

{
  return SetAttr(label, lower, upper, isDelta, GetID());
}

//=======================================================================
//function : Set
//purpose  : Set user defined attribute with specific ID
//=======================================================================

Handle(TDataStd_ExtStringArray) TDataStd_ExtStringArray::Set (
                                          const TDF_Label& label,
                                          const Standard_GUID& theGuid,
                                          const Standard_Integer lower,
                                          const Standard_Integer upper,
                                          const Standard_Boolean isDelta) 

{
  return SetAttr(label, lower, upper, isDelta, theGuid);
}
//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::SetValue(const Standard_Integer index, const TCollection_ExtendedString& value) 
{
  if(myValue.IsNull()) return;
  if( myValue->Value(index) == value)
    return; 

  Backup();
  myValue->SetValue(index, value);
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const TCollection_ExtendedString& TDataStd_ExtStringArray::Value (const Standard_Integer index) const 
{
  if (myValue.IsNull()) 
  {
    static TCollection_ExtendedString staticEmptyValue;
    return staticEmptyValue;
  }
  return myValue->Value(index); 
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ExtStringArray::Lower (void) const 
{ 
  if(myValue.IsNull()) return 0;
  return myValue->Lower(); 
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ExtStringArray::Upper (void) const 
{ 
  if(myValue.IsNull()) return 0;
  return myValue->Upper(); 
}


//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ExtStringArray::Length (void) const 
{
  if(myValue.IsNull()) return 0;
  return myValue->Length(); 
}



//=======================================================================
//function : ChangeArray
//purpose  : If value of <newArray> differs from <myValue>, Backup 
//         : performed and myValue refers to new instance of HArray1OfExtendedString
//         : that holds <newArray>
//=======================================================================

void TDataStd_ExtStringArray::ChangeArray(const Handle(TColStd_HArray1OfExtendedString)& newArray,
                                          const Standard_Boolean isCheckItems) 
{
  Standard_Integer aLower    = newArray->Lower();
  Standard_Integer anUpper   = newArray->Upper();
  Standard_Boolean aDimEqual = Standard_False;
  Standard_Integer i;

  if (Lower() == aLower && Upper() == anUpper ) {
    aDimEqual = Standard_True;
    Standard_Boolean isEqual = Standard_True;
    if(isCheckItems) {
      for(i = aLower; i <= anUpper; i++) {
        if(myValue->Value(i) != newArray->Value(i)) {
          isEqual = Standard_False;
          break;
        }
      }
      if(isEqual)
        return;
    }
  }

  Backup();

// Handles of myValue of current and backuped attributes will be different!!!
  if(myValue.IsNull() || !aDimEqual) 
    myValue = new TColStd_HArray1OfExtendedString(aLower, anUpper);

  for(i = aLower; i <= anUpper; i++) 
    myValue->SetValue(i, newArray->Value(i));
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_ExtStringArray::ID () const { return myID; }

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_ExtStringArray::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;
  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================

void TDataStd_ExtStringArray::SetID()
{  
  Backup();
  myID = GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_ExtStringArray::NewEmpty () const
{  
  return new TDataStd_ExtStringArray(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::Restore(const Handle(TDF_Attribute)& With) 
{
  Standard_Integer i, lower, upper;
  Handle(TDataStd_ExtStringArray) anArray = Handle(TDataStd_ExtStringArray)::DownCast(With);
  if(!anArray->myValue.IsNull()) {
    lower = anArray->Lower();
    upper = anArray->Upper(); 
    myValue = new TColStd_HArray1OfExtendedString(lower, upper);
    for(i = lower; i<=upper; i++)
      myValue->SetValue(i, anArray->Value(i));
    myIsDelta = anArray->myIsDelta;
    myID = anArray->ID();
  }
  else
    myValue.Nullify();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::Paste (const Handle(TDF_Attribute)& Into,
                                     const Handle(TDF_RelocationTable)& ) const
{
  if(!myValue.IsNull()) {
    Handle(TDataStd_ExtStringArray) anAtt = Handle(TDataStd_ExtStringArray)::DownCast(Into);
    if(!anAtt.IsNull()) {
      anAtt->ChangeArray( myValue, Standard_False );
      anAtt->SetDelta(myIsDelta);
      anAtt->SetID(myID);
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_ExtStringArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "\nExtStringArray :";
  if(!myValue.IsNull()) {
    Standard_Integer i, lower, upper;
    lower = myValue->Lower();
    upper = myValue->Upper();
    for(i = lower; i<=upper; i++)
      anOS << "\t" <<myValue->Value(i)<<endl;
  }
  anOS << " Delta is " << (myIsDelta ? "ON":"OFF");
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << endl;
  return anOS;
}

//=======================================================================
//function : DeltaOnModification
//purpose  : 
//=======================================================================

Handle(TDF_DeltaOnModification) TDataStd_ExtStringArray::DeltaOnModification
(const Handle(TDF_Attribute)& OldAttribute) const
{
  if(myIsDelta)
    return new TDataStd_DeltaOnModificationOfExtStringArray(Handle(TDataStd_ExtStringArray)::DownCast (OldAttribute));
  else return new TDF_DefaultDeltaOnModification(OldAttribute);
}

