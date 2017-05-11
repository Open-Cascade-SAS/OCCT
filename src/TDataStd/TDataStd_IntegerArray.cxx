// Created on: 1999-06-16
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_DeltaOnModificationOfIntArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_IntegerArray,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_IntegerArray::GetID() 
{ 
  static Standard_GUID TDataStd_IntegerArrayID ("2a96b61d-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_IntegerArrayID; 
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_IntegerArray) SetAttr(const TDF_Label&       label,
                                             const Standard_Integer lower,
                                             const Standard_Integer upper,
                                             const Standard_Boolean isDelta,
                                             const Standard_GUID&   theGuid) 
{
  Handle(TDataStd_IntegerArray) A;
  if (!label.FindAttribute (theGuid, A)) 
  {
    A = new TDataStd_IntegerArray;
    A->Init (lower, upper);
    A->SetDelta(isDelta); 
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  else if (lower != A->Lower() || upper != A->Upper())
  {
    A->Init(lower, upper);
  }
  return A;
}

//=======================================================================
//function : TDataStd_IntegerArray
//purpose  : Empty Constructor
//=======================================================================

TDataStd_IntegerArray::TDataStd_IntegerArray()
  :myIsDelta(Standard_False)
{}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TDataStd_IntegerArray::Init(const Standard_Integer lower,
                                 const Standard_Integer upper)
{
  Standard_RangeError_Raise_if(upper < lower,"TDataStd_IntegerArray::Init");  
  Backup();
  myValue = new TColStd_HArray1OfInteger(lower, upper, 0);
}

//=======================================================================
//function : Set
//purpose  : isDelta applicable only for new attributes
//=======================================================================

Handle(TDataStd_IntegerArray) TDataStd_IntegerArray::Set
                                                (const TDF_Label&       label,
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

Handle(TDataStd_IntegerArray) TDataStd_IntegerArray::Set
                                                (const TDF_Label&       label,
                                                 const Standard_GUID&   theGuid,
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

void TDataStd_IntegerArray::SetValue(const Standard_Integer index, const Standard_Integer value) 
{
  if(myValue.IsNull()) return;
  if( myValue->Value(index) == value)
    return;
  Backup();
  myValue->SetValue(index, value);
}


//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================

Standard_Integer TDataStd_IntegerArray::Value (const Standard_Integer index) const 
{
  if(myValue.IsNull()) return 0;
  return myValue->Value(index); 
}



//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_IntegerArray::Lower (void) const 
{ 
  if(myValue.IsNull()) return 0;
  return myValue->Lower(); 
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_IntegerArray::Upper (void) const 
{ 
  if(myValue.IsNull()) return 0; 
  return myValue->Upper(); 
}


//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_IntegerArray::Length (void) const 
{
  if(myValue.IsNull()) return 0;
  return myValue->Length(); 
}


//=======================================================================
//function : ChangeArray
//purpose  : If value of <newArray> differs from <myValue>, Backup 
//         : performed and myValue refers to new instance of HArray1OfInteger 
//         : that holds <newArray>
//=======================================================================

void TDataStd_IntegerArray::ChangeArray(const Handle(TColStd_HArray1OfInteger)& newArray,
                                        const Standard_Boolean isCheckItems) 
{
  Standard_Integer aLower    = newArray->Lower();
  Standard_Integer anUpper   = newArray->Upper();
  Standard_Boolean aDimEqual = Standard_False;
  Standard_Integer i;

  if(Lower() == aLower && Upper() == anUpper ) {
    aDimEqual = Standard_True;
    if(isCheckItems) {
      Standard_Boolean isEqual = Standard_True;
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
// Handles of myValue of current and backuped attributes will be different!
  if(myValue.IsNull() || !aDimEqual) 
    myValue = new TColStd_HArray1OfInteger(aLower, anUpper);  

  for(i = aLower; i <= anUpper; i++) 
    myValue->SetValue(i, newArray->Value(i));
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_IntegerArray::ID () const { return myID; }

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_IntegerArray::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;
  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================

void TDataStd_IntegerArray::SetID()
{  
  Backup();
  myID = GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_IntegerArray::NewEmpty () const
{  
  return new TDataStd_IntegerArray(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_IntegerArray::Restore(const Handle(TDF_Attribute)& With) 
{
  Standard_Integer i, lower, upper;
  Handle(TDataStd_IntegerArray) anArray = Handle(TDataStd_IntegerArray)::DownCast(With);
  if(!anArray->myValue.IsNull()) {
    lower = anArray->Lower();
    upper = anArray->Upper(); 
    myValue = new TColStd_HArray1OfInteger(lower, upper); 
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

void TDataStd_IntegerArray::Paste (const Handle(TDF_Attribute)& Into,
                                   const Handle(TDF_RelocationTable)& ) const
{

  if(!myValue.IsNull()) {
    Handle(TDataStd_IntegerArray) anAtt = Handle(TDataStd_IntegerArray)::DownCast(Into);
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

Standard_OStream& TDataStd_IntegerArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "\nIntegerArray:: " << this <<" :";
  if(!myValue.IsNull()) {
    Standard_Integer i, lower, upper;
    lower = myValue->Lower();
    upper = myValue->Upper();
    for(i = lower; i<=upper; i++)
      anOS << " " <<myValue->Value(i);
  }
  anOS << " Delta is " << (myIsDelta ? "ON":"OFF");
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << endl;

  // anOS <<"\nAttribute fields: ";
  //  anOS << TDF_Attribute::Dump(anOS);

  return anOS;
}

//=======================================================================
//function : DeltaOnModification
//purpose  : 
//=======================================================================

Handle(TDF_DeltaOnModification) TDataStd_IntegerArray::DeltaOnModification
(const Handle(TDF_Attribute)& OldAttribute) const
{
  if(myIsDelta)
    return new TDataStd_DeltaOnModificationOfIntArray(Handle(TDataStd_IntegerArray)::DownCast (OldAttribute));
  else return new TDF_DefaultDeltaOnModification(OldAttribute);
}
