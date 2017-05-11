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
#include <TDataStd_BooleanArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_BooleanArray,TDF_Attribute)

static Standard_Integer DegreeOf2(const Standard_Integer degree)
{
  switch (degree)
  {
    case 0:
      return 1;
    case 1:
      return 2;
    case 2:
      return 4;
    case 3:
      return 8;
    case 4:
      return 16;
    case 5:
      return 32;
    case 6:
      return 64;
    case 7:
      return 128;
    case 8:
      return 256;
  }
  return -1;
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_BooleanArray::GetID() 
{ 
  static Standard_GUID TDataStd_BooleanArrayID ("C7E98E54-B5EA-4aa9-AC99-9164EBD07F10");
  return TDataStd_BooleanArrayID; 
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_BooleanArray) SetAttr(const TDF_Label&       label,
                                             const Standard_Integer lower,
                                             const Standard_Integer upper,
                                             const Standard_GUID&   theGuid) 
{
  Handle(TDataStd_BooleanArray) A;
  if (!label.FindAttribute (theGuid, A)) 
  {
    A = new TDataStd_BooleanArray;
    A->SetID(theGuid);
    A->Init (lower, upper); 
    label.AddAttribute(A);
  }
  else if (lower != A->Lower() || upper != A->Upper())
  {
    A->Init(lower, upper);
  }
  return A;
}

//=======================================================================
//function : TDataStd_BooleanArray
//purpose  : Empty Constructor
//=======================================================================
TDataStd_BooleanArray::TDataStd_BooleanArray() 
{

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TDataStd_BooleanArray::Init(const Standard_Integer lower,
                                 const Standard_Integer upper)
{
  Standard_RangeError_Raise_if(upper < lower,"TDataStd_BooleanArray::Init");
  Backup();
  myLower = lower;
  myUpper = upper;
  myValues = new TColStd_HArray1OfByte(0, Length() >> 3, 0/*initialize to FALSE*/);
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_BooleanArray) TDataStd_BooleanArray::Set(const TDF_Label&       label,
                                                         const Standard_Integer lower,
                                                         const Standard_Integer upper) 
{
  return SetAttr(label, lower, upper, GetID());
}


//=======================================================================
//function : Set
//purpose  : Set user defined attribute with specific ID
//=======================================================================
Handle(TDataStd_BooleanArray) TDataStd_BooleanArray::Set(const TDF_Label&       label,
                                                         const Standard_GUID&   theGuid,
                                                         const Standard_Integer lower,
                                                         const Standard_Integer upper)
{
  return SetAttr(label, lower, upper, theGuid);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void TDataStd_BooleanArray::SetValue (const Standard_Integer index,
                                      const Standard_Boolean value) 
{

  if (myValues.IsNull()) 
    return; 
  Standard_Integer byte_index = (index - myLower) >> 3;
  Standard_Integer degree = index - (byte_index << 3) - myLower;
  Standard_Integer byte_value = DegreeOf2(degree);

  if ((value != 0) == ((myValues->Value(byte_index) & byte_value) > 0))
    return;

  Backup();

  if (value)
  {
    myValues->ChangeValue(byte_index) |= byte_value;
  }
  else
  {
    myValues->ChangeValue(byte_index) ^= byte_value;
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_BooleanArray::Value (const Standard_Integer index) const 
{
  if (myValues.IsNull())
    return Standard_False;
  if (index < myLower || index > myUpper)
    return Standard_False;

  Standard_Integer byte_index = (index - myLower) >> 3;
  Standard_Integer degree = index - (byte_index << 3) - myLower;
  Standard_Integer byte_value = DegreeOf2(degree);

  return (myValues->Value(byte_index) & byte_value) > 0;
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_BooleanArray::Lower (void) const 
{ 
  return myLower;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_BooleanArray::Upper (void) const 
{ 
  return myUpper;
}

//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_BooleanArray::Length (void) const 
{
  return myUpper - myLower + 1;
}

//=======================================================================
//function : InternalArray
//purpose  : 
//=======================================================================
const Handle(TColStd_HArray1OfByte)& TDataStd_BooleanArray::InternalArray () const 
{
  return myValues;
}

//=======================================================================
//function : SetInternalArray
//purpose  : 
//=======================================================================
void TDataStd_BooleanArray::SetInternalArray (const Handle(TColStd_HArray1OfByte)& values)
{
  myValues = values;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_BooleanArray::ID () const 
{ 
  return myID; 
}

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_BooleanArray::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;
  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================
void TDataStd_BooleanArray::SetID()
{
  Backup();
  myID = GetID();
}
//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_BooleanArray::NewEmpty () const
{  
  return new TDataStd_BooleanArray(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_BooleanArray::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(TDataStd_BooleanArray) anArray = Handle(TDataStd_BooleanArray)::DownCast(With);
  if (!anArray->myValues.IsNull()) 
  {
    myLower = anArray->Lower();
    myUpper = anArray->Upper();
    Standard_Integer byte_upper = Length() >> 3;
    myValues = new TColStd_HArray1OfByte(0, byte_upper, 0/*initialize to FALSE*/);
    const TColStd_Array1OfByte& with_array = anArray->myValues->Array1();
    for (Standard_Integer i = 0; i <= byte_upper; i++)
    {
      myValues->SetValue(i, with_array.Value(i));
    }
    myID = anArray->ID();
  }
  else
  {
    myValues.Nullify();
  }
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_BooleanArray::Paste (const Handle(TDF_Attribute)& Into,
				   const Handle(TDF_RelocationTable)& ) const
{
  if (!myValues.IsNull()) 
  {
    Handle(TDataStd_BooleanArray) anArray = Handle(TDataStd_BooleanArray)::DownCast(Into);
    if (!anArray.IsNull())
    {
      anArray->Init(myLower, myUpper);
      for (Standard_Integer i = myLower; i <= myUpper; i++)
      {
        anArray->SetValue(i, Value(i));
      }
    }
    anArray->SetID(myID);
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_BooleanArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "\nBooleanArray: ";
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS <<endl;
  return anOS;
}
