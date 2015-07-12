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
#include <TDataStd_ReferenceArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ReferenceArray::GetID() 
{ 
  static Standard_GUID TDataStd_ReferenceArrayID ("7EE745A6-BB50-446c-BB0B-C195B23AB5CA");
  return TDataStd_ReferenceArrayID; 
}

//=======================================================================
//function : TDataStd_ReferenceArray
//purpose  : Empty Constructor
//=======================================================================
TDataStd_ReferenceArray::TDataStd_ReferenceArray() 
{

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TDataStd_ReferenceArray::Init(const Standard_Integer lower,
				   const Standard_Integer upper)
{
  Standard_RangeError_Raise_if(upper < lower,"TDataStd_ReferenceArray::Init");
  Backup();
  myArray = new TDataStd_HLabelArray1(lower, upper);
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_ReferenceArray) TDataStd_ReferenceArray::Set(const TDF_Label&       label,
							     const Standard_Integer lower,
							     const Standard_Integer upper) 
{
  Handle(TDataStd_ReferenceArray) A;
  if (!label.FindAttribute (TDataStd_ReferenceArray::GetID(), A)) 
  {
    A = new TDataStd_ReferenceArray;
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
void TDataStd_ReferenceArray::SetValue (const Standard_Integer index,
					const TDF_Label&       value) 
{
  if(myArray.IsNull()) return;
  if (value == myArray->Value(index))
    return;
  Backup();

  myArray->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
TDF_Label TDataStd_ReferenceArray::Value (const Standard_Integer index) const 
{
  return myArray->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ReferenceArray::Lower () const 
{ 
  if (myArray.IsNull())
    return 0;
  return myArray->Lower();
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ReferenceArray::Upper () const 
{ 
  if (myArray.IsNull())
    return -1;
  return myArray->Upper();
}

//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ReferenceArray::Length () const 
{
  if (myArray.IsNull())
    return 0;
  return myArray->Length();
}

//=======================================================================
//function : InternalArray
//purpose  : 
//=======================================================================
const Handle(TDataStd_HLabelArray1)& TDataStd_ReferenceArray::InternalArray () const 
{
  return myArray;
}

//=======================================================================
//function : SetInternalArray
//purpose  : 
//=======================================================================
void TDataStd_ReferenceArray::SetInternalArray (const Handle(TDataStd_HLabelArray1)& values,
                                                const Standard_Boolean)
{
//  myArray = values;
  Standard_Integer aLower    = values->Lower();
  Standard_Integer anUpper   = values->Upper();
  Standard_Boolean aDimEqual = Standard_False;
  Standard_Integer i;

#ifdef OCC2932
  if (Lower() == aLower && Upper() == anUpper ) {
    aDimEqual = Standard_True;
    Standard_Boolean isEqual = Standard_True;
    if(isCheckItems) {
      for(i = aLower; i <= anUpper; i++) {
	if(myArray->Value(i) != values->Value(i)) {
	  isEqual = Standard_False;
	  break;
	}
      }
      if(isEqual)
	return;
    }
  }
#endif

  Backup();

  if(myArray.IsNull() || !aDimEqual) 
    myArray = new TDataStd_HLabelArray1(aLower, anUpper);

  for(i = aLower; i <= anUpper; i++) 
    myArray->SetValue(i, values->Value(i));
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ReferenceArray::ID () const 
{ 
  return GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_ReferenceArray::NewEmpty () const
{  
  return new TDataStd_ReferenceArray(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_ReferenceArray::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(TDataStd_ReferenceArray) anArray = Handle(TDataStd_ReferenceArray)::DownCast(With);
  if (!anArray->myArray.IsNull()) 
  {
    const TDataStd_LabelArray1& arr = anArray->myArray->Array1();
    Standard_Integer lower = arr.Lower(), i = lower, upper = arr.Upper();
    Init(lower, upper);
    for (; i <= upper; i++)
    {
      myArray->SetValue(i, arr.Value(i));
    }
  }
  else
  {
    myArray.Nullify();
  }
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_ReferenceArray::Paste (const Handle(TDF_Attribute)& Into,
				     const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_ReferenceArray) anArray = Handle(TDataStd_ReferenceArray)::DownCast(Into);
  if (myArray.IsNull())
  {
    anArray->myArray.Nullify();
    return;
  }
  const TDataStd_LabelArray1& arr = myArray->Array1();
  Standard_Integer lower = arr.Lower(), i = lower, upper = arr.Upper();
  if (lower != anArray->Lower() || upper != anArray->Upper())
    anArray->Init(lower, upper);
  for (; i <= upper; i++)
  {
    TDF_Label L = arr.Value(i), rL;
    if (!L.IsNull())
    {
      if (!RT->HasRelocation(L, rL))
	rL = L;
      anArray->myArray->SetValue(i, rL);
    }
  }
}

//=======================================================================
//function : References
//purpose  : Adds the referenced attributes or labels.
//=======================================================================
void TDataStd_ReferenceArray::References(const Handle(TDF_DataSet)& aDataSet) const
{
  if (!Label().IsImported() && !myArray.IsNull()) 
  {
    const TDataStd_LabelArray1& arr = myArray->Array1();
    Standard_Integer lower = arr.Lower(), i = lower, upper = arr.Upper();
    for (; i <= upper; i++)
    {
      if (!arr.Value(i).IsNull())
	aDataSet->AddLabel(arr.Value(i));
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_ReferenceArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "ReferenceArray";
  return anOS;
}
