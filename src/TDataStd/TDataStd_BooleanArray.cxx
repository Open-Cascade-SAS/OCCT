// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <TDataStd_BooleanArray.ixx>

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
  Backup();

  myLower = lower;
  myUpper = upper;
  if (myUpper >= myLower)
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
  Handle(TDataStd_BooleanArray) A;
  if (!label.FindAttribute (TDataStd_BooleanArray::GetID(), A)) 
  {
    A = new TDataStd_BooleanArray;
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
//function : SetValue
//purpose  : 
//=======================================================================
void TDataStd_BooleanArray::SetValue (const Standard_Integer index,
				      const Standard_Boolean value) 
{
#ifdef DEB
  if (myValues.IsNull()) 
    return;
  if (index < myLower || index > myUpper)
    return;
#endif

  Standard_Integer byte_index = (index - myLower) >> 3;
  Standard_Integer degree = index - (byte_index << 3) - myLower;
  Standard_Integer byte_value = DegreeOf2(degree);

  if (value == ((myValues->Value(byte_index) & byte_value) > 0))
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
  return GetID(); 
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
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_BooleanArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "BooleanArray";
  return anOS;
}
