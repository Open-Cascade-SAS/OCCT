// Created on: 2007-06-27
// Created by: Sergey ZARITCHNY
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


#include <PDataStd_NamedData.ixx>
#define ROW_NUMBER 6
//=======================================================================
//function : PDataStd_NamedData
//purpose  : 
//=======================================================================
PDataStd_NamedData::PDataStd_NamedData() { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void PDataStd_NamedData::Init(const Handle(TColStd_HArray2OfInteger)& theDim)
{
  if(theDim.IsNull()) return;
  if(theDim->RowLength() > 0 && (theDim->UpperRow() - theDim->LowerRow() + 1) == ROW_NUMBER ) {
    myDimensions = new PColStd_HArray2OfInteger(theDim->LowerRow(),theDim->UpperRow(),theDim->LowerCol(),theDim->UpperCol(), 0);
    Standard_Integer i, j = theDim->LowerCol();

    for(i = theDim->LowerRow();i<=theDim->UpperRow(); i++) {
      myDimensions->SetValue(i,j, theDim->Value(i,j));
      myDimensions->SetValue(i,j+1, theDim->Value(i,j+1));
    }
  
    i = theDim->LowerRow();
    //1.Integers
    Standard_Integer aSize = theDim->Value(i,j+1) - theDim->Value(i,j);
    if(aSize>=0 && (theDim->Value(i,j) | theDim->Value(i,j+1))) {
      myIntKeys = new  PColStd_HArray1OfExtendedString (theDim->Value(i,j), theDim->Value(i,j+1));
      myIntValues = new PColStd_HArray1OfInteger(theDim->Value(i,j), theDim->Value(i,j+1));
    }
    //2.Reals
    i++;
    aSize = theDim->Value(i,j+1) - theDim->Value(i,j);
    if(aSize>=0 && (theDim->Value(i,j) | theDim->Value(i,j+1))) {
      myRealKeys = new  PColStd_HArray1OfExtendedString (theDim->Value(i,j), theDim->Value(i,j+1));
      myRealValues = new PColStd_HArray1OfReal(theDim->Value(i,j), theDim->Value(i,j+1));
    }
    //3.Strings
    i++;
    aSize = theDim->Value(i,j+1) - theDim->Value(i,j);
    if(aSize>=0 && (theDim->Value(i,j) | theDim->Value(i,j+1))) {
      myStrKeys   = new PColStd_HArray1OfExtendedString (theDim->Value(i,j), theDim->Value(i,j+1));
      myStrValues = new PColStd_HArray1OfExtendedString (theDim->Value(i,j), theDim->Value(i,j+1));
    }

    //4.Bytes
    i++;
    aSize = theDim->Value(i,j+1) - theDim->Value(i,j);
    if(aSize>=0 && (theDim->Value(i,j) | theDim->Value(i,j+1))) {
      myByteKeys   = new PColStd_HArray1OfExtendedString (theDim->Value(i,j), theDim->Value(i,j+1));
      myByteValues = new PDataStd_HArray1OfByte(theDim->Value(i,j), theDim->Value(i,j+1));
    }

    //5.ArraysOfIntegers
    i++;
    aSize = theDim->Value(i,j+1) - theDim->Value(i,j);
    if(aSize>=0 && (theDim->Value(i,j) | theDim->Value(i,j+1))) {
      myArrIntKeys   = new PColStd_HArray1OfExtendedString (theDim->Value(i,j), theDim->Value(i,j+1));
      myArrIntValues = new PDataStd_HArray1OfHArray1OfInteger(theDim->Value(i,j), theDim->Value(i,j+1));
    }

    //6.
    i++;
    aSize = theDim->Value(i,j+1) - theDim->Value(i,j);
    if(aSize>=0 && (theDim->Value(i,j) | theDim->Value(i,j+1))) {
      myArrRealKeys   = new PColStd_HArray1OfExtendedString (theDim->Value(i,j), theDim->Value(i,j+1));
      myArrRealValues = new PDataStd_HArray1OfHArray1OfReal(theDim->Value(i,j), theDim->Value(i,j+1));
    }

  }
}

//=======================================================================
//function : LowerI
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::LowerI() const
{
  return myDimensions->Value(myDimensions->LowerRow(),myDimensions->LowerCol());
}

//=======================================================================
//function : UpperI
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::UpperI() const
{
  return myDimensions->Value(myDimensions->LowerRow(),myDimensions->UpperCol());
}

//=======================================================================
//function : LowerR
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::LowerR() const
{
  return myDimensions->Value(myDimensions->LowerRow()+1,myDimensions->LowerCol());
}

//=======================================================================
//function : UpperR
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::UpperR() const
{
  return myDimensions->Value(myDimensions->LowerRow()+1,myDimensions->UpperCol());
}

//=======================================================================
//function : LowerS
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::LowerS() const
{
  return myDimensions->Value(myDimensions->LowerRow()+2,myDimensions->LowerCol());
}

//=======================================================================
//function : UpperS
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::UpperS() const
{
  return myDimensions->Value(myDimensions->LowerRow()+2,myDimensions->UpperCol());
}

//=======================================================================
//function : LowerB
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::LowerB() const
{
  return myDimensions->Value(myDimensions->LowerRow()+3,myDimensions->LowerCol());
}

//=======================================================================
//function : UpperB
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::UpperB() const
{
  return myDimensions->Value(myDimensions->LowerRow()+3,myDimensions->UpperCol());
}

//=======================================================================
//function : LowerAI
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::LowerAI() const
{
  return myDimensions->Value(myDimensions->LowerRow()+4,myDimensions->LowerCol());
}

//=======================================================================
//function : UpperAI
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::UpperAI() const
{
  return myDimensions->Value(myDimensions->LowerRow()+4,myDimensions->UpperCol());
}

//=======================================================================
//function : LowerAR
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::LowerAR() const
{
  return myDimensions->Value(myDimensions->LowerRow()+5,myDimensions->LowerCol());
}

//=======================================================================
//function : UpperAR
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::UpperAR() const
{
  return myDimensions->Value(myDimensions->LowerRow()+5,myDimensions->UpperCol());
}


//=======================================================================
//function : SetIntDataItem
//purpose  : 
//=======================================================================
void PDataStd_NamedData::SetIntDataItem(const Standard_Integer index, 
					const Handle(PCollection_HExtendedString)& key,
					const Standard_Integer value)
{
  if(myIntKeys.IsNull() || myIntValues.IsNull()) return;
  myIntKeys->SetValue(index, key);
  myIntValues->SetValue(index, value);
}

//=======================================================================
//function : IntDataItemValue
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_NamedData::IntDataItemValue(const Standard_Integer index, 
						      Handle(PCollection_HExtendedString)& key) const
{
  if(myIntKeys.IsNull() || myIntValues.IsNull()) return 0;
  key = myIntKeys->Value(index);
  return myIntValues->Value(index);
}

//=======================================================================
//function : SetRealDataItem
//purpose  : 
//=======================================================================
void PDataStd_NamedData::SetRealDataItem(const Standard_Integer index, 
					const Handle(PCollection_HExtendedString)& key,
					const Standard_Real value)
{
  if(myRealKeys.IsNull() || myRealValues.IsNull()) return;
  myRealKeys->SetValue(index, key);
  myRealValues->SetValue(index, value);
}

//=======================================================================
//function : RealDataItemValue
//purpose  : 
//=======================================================================
Standard_Real PDataStd_NamedData::RealDataItemValue(const Standard_Integer index, 
						      Handle(PCollection_HExtendedString)& key) const
{
  if(myRealKeys.IsNull() || myRealValues.IsNull()) return 0.;
  key = myRealKeys->Value(index);
  return myRealValues->Value(index);
}

//=======================================================================
//function : SetStrDataItem
//purpose  : 
//=======================================================================
void PDataStd_NamedData::SetStrDataItem(const Standard_Integer index, 
					const Handle(PCollection_HExtendedString)& key,
					const Handle(PCollection_HExtendedString)& value)
{
  if(myStrKeys.IsNull() || myStrValues.IsNull()) return;
  myStrKeys->SetValue(index, key);
  myStrValues->SetValue(index, value);
}

//=======================================================================
//function : StrDataItemValue
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PDataStd_NamedData::StrDataItemValue
                                           (const Standard_Integer index, 
 	                  Handle(PCollection_HExtendedString)& key) const
{
  Handle(PCollection_HExtendedString) aVal;
  if(myStrKeys.IsNull() || myStrValues.IsNull()) return aVal;
  key = myStrKeys->Value(index);
  return myStrValues->Value(index);
}

//=======================================================================
//function : SetByteDataItem
//purpose  : 
//=======================================================================
void PDataStd_NamedData::SetByteDataItem(const Standard_Integer index, 
					const Handle(PCollection_HExtendedString)& key,
					const Standard_Byte value)
{
  if(myByteKeys.IsNull() || myByteValues.IsNull()) return;
  myByteKeys->SetValue(index, key);
  myByteValues->SetValue(index, value);
}

//=======================================================================
//function : ByteDataItemValue
//purpose  : 
//=======================================================================
Standard_Byte PDataStd_NamedData::ByteDataItemValue(const Standard_Integer index, 
						    Handle(PCollection_HExtendedString)& key) const
{
  if(myByteKeys.IsNull() || myByteValues.IsNull()) return 0x00;
  key = myByteKeys->Value(index);
  return myByteValues->Value(index);
}

//=======================================================================
//function : SetArrIntDataItem
//purpose  : 
//=======================================================================
void PDataStd_NamedData::SetArrIntDataItem(const Standard_Integer index, 
					   const Handle(PCollection_HExtendedString)& key,
					   const Handle(PColStd_HArray1OfInteger)& value)
{
  if(myArrIntKeys.IsNull() || myArrIntValues.IsNull()) return;
  myArrIntKeys->SetValue(index, key);
  myArrIntValues->SetValue(index, value);
}

//=======================================================================
//function : ArrIntDataItemValue
//purpose  : 
//=======================================================================
Handle(PColStd_HArray1OfInteger) PDataStd_NamedData::ArrIntDataItemValue
                                           (const Standard_Integer index, 
 	                  Handle(PCollection_HExtendedString)& key) const
{
  Handle(PColStd_HArray1OfInteger) aVal;
  if(myArrIntKeys.IsNull() || myArrIntValues.IsNull()) return aVal;
  key = myArrIntKeys->Value(index);
  return myArrIntValues->Value(index);
}

//=======================================================================
//function : SetArrRealDataItem
//purpose  : 
//=======================================================================
void PDataStd_NamedData::SetArrRealDataItem(const Standard_Integer index, 
					   const Handle(PCollection_HExtendedString)& key,
					   const Handle(PColStd_HArray1OfReal)& value)
{
  if(myArrRealKeys.IsNull() || myArrRealValues.IsNull()) return;
  myArrRealKeys->SetValue(index, key);
  myArrRealValues->SetValue(index, value);
}

//=======================================================================
//function : ArrRealDataItemValue
//purpose  : 
//=======================================================================
Handle(PColStd_HArray1OfReal) PDataStd_NamedData::ArrRealDataItemValue
                                           (const Standard_Integer index, 
 	                  Handle(PCollection_HExtendedString)& key) const
{
  Handle(PColStd_HArray1OfReal) aVal;
  if(myArrRealKeys.IsNull() || myArrRealValues.IsNull()) return aVal;
  key = myArrRealKeys->Value(index);
  return myArrRealValues->Value(index);
}

//=======================================================================
//function : HasIntegers
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_NamedData::HasIntegers() const
{
  return !myIntKeys.IsNull();
}
//=======================================================================
//function : HasReals
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_NamedData::HasReals() const
{
  return !myRealKeys.IsNull();
}

//=======================================================================
//function : HasStrings
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_NamedData::HasStrings() const
{
  return !myStrKeys.IsNull();
}

//=======================================================================
//function : HasBytes
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_NamedData::HasBytes() const
{
  return !myByteKeys.IsNull();
}

//=======================================================================
//function : HasArraysOfIntegers
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_NamedData::HasArraysOfIntegers() const
{
  return !myArrIntKeys.IsNull();
}

//=======================================================================
//function : HasArraysOfReals
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_NamedData::HasArraysOfReals() const
{
  return !myArrRealKeys.IsNull();
}
