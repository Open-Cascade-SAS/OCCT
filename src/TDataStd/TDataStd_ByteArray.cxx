// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_DeltaOnModificationOfByteArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_ByteArray,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ByteArray::GetID() 
{ 
  static Standard_GUID TDataStd_ByteArrayID ("FD9B918F-2980-4c66-85E0-D71965475290");
  return TDataStd_ByteArrayID; 
}

//=======================================================================
//function : TDataStd_ByteArray
//purpose  : Empty Constructor
//=======================================================================
TDataStd_ByteArray::TDataStd_ByteArray() : myIsDelta(Standard_False)
{}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_ByteArray) SetAttr(const TDF_Label&       label,
                                          const Standard_Integer lower,
                                          const Standard_Integer upper,
                                          const Standard_Boolean isDelta,
                                          const Standard_GUID&   theGuid) 
{
  Handle(TDataStd_ByteArray) A;
  if (!label.FindAttribute (theGuid, A)) 
  {
    A = new TDataStd_ByteArray;
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
//function : Init
//purpose  : 
//=======================================================================
void TDataStd_ByteArray::Init(const Standard_Integer lower,
                              const Standard_Integer upper)
{
  Standard_RangeError_Raise_if(upper < lower,"TDataStd_ByteArray::Init");
  Backup();
  myValue = new TColStd_HArray1OfByte(lower, upper, 0x00);
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_ByteArray) TDataStd_ByteArray::Set(const TDF_Label&       label,
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
Handle(TDataStd_ByteArray) TDataStd_ByteArray::Set(const TDF_Label&       label,
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
void TDataStd_ByteArray::SetValue (const Standard_Integer index,
                                   const Standard_Byte value) 
{
  if (myValue.IsNull()) 
    return;
  if (value == myValue->Value(index))
    return;
  Backup();

  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Byte TDataStd_ByteArray::Value (const Standard_Integer index) const 
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ByteArray::Lower (void) const 
{ 
  if (myValue.IsNull())
    return 0;
  return myValue->Lower();
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ByteArray::Upper (void) const 
{ 
  if (myValue.IsNull())  return -1;
  return myValue->Upper();
}

//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ByteArray::Length (void) const 
{
  if (myValue.IsNull())
    return 0;
  return myValue->Length();
}

//=======================================================================
//function : ChangeArray
//purpose  : If value of <newArray> differs from <myValue>, Backup 
//         : performed and myValue refers to new instance of HArray1OfByte 
//         : that holds <newArray>
//=======================================================================
void TDataStd_ByteArray::ChangeArray (const Handle(TColStd_HArray1OfByte)& newArray,
                                      const Standard_Boolean isCheckItems)
{

  Standard_Integer aLower    = newArray->Lower();
  Standard_Integer anUpper   = newArray->Upper();
  Standard_Boolean aDimEqual = Standard_False;
  Standard_Integer i;

  if ( Lower() == aLower && Upper() == anUpper ) {
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
    myValue = new TColStd_HArray1OfByte(aLower, anUpper);  

  for(i = aLower; i <= anUpper; i++) 
    myValue->SetValue(i, newArray->Value(i));
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ByteArray::ID () const 
{ 
  return myID; 
}

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_ByteArray::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;
  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================

void TDataStd_ByteArray::SetID()
{
  Backup();
  myID = GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_ByteArray::NewEmpty () const
{  
  return new TDataStd_ByteArray(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_ByteArray::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(TDataStd_ByteArray) anArray = Handle(TDataStd_ByteArray)::DownCast(With);
  if (!anArray->myValue.IsNull()) 
  {
    const TColStd_Array1OfByte& with_array = anArray->myValue->Array1();
    Standard_Integer lower = with_array.Lower(), i = lower, upper = with_array.Upper();
    myValue = new TColStd_HArray1OfByte(lower, upper);
    for (; i <= upper; i++)
      myValue->SetValue(i, with_array.Value(i));
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
void TDataStd_ByteArray::Paste (const Handle(TDF_Attribute)& Into,
                                const Handle(TDF_RelocationTable)& ) const
{
  if (!myValue.IsNull()) 
  {
    Handle(TDataStd_ByteArray) anAtt = Handle(TDataStd_ByteArray)::DownCast(Into);
    if (!anAtt.IsNull())
    {
      anAtt->ChangeArray( myValue, Standard_False);
      anAtt->SetDelta(myIsDelta);
      anAtt->SetID(myID);
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_ByteArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "\nByteArray: ";
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid << endl;
  return anOS;
}

//=======================================================================
//function : DeltaOnModification
//purpose  : 
//=======================================================================

Handle(TDF_DeltaOnModification) TDataStd_ByteArray::DeltaOnModification
(const Handle(TDF_Attribute)& OldAttribute) const
{
  if(myIsDelta)
    return new TDataStd_DeltaOnModificationOfByteArray(Handle(TDataStd_ByteArray)::DownCast (OldAttribute));
  else return new TDF_DefaultDeltaOnModification(OldAttribute);
}

