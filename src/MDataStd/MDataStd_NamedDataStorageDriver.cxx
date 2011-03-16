// File:	MDataStd_NamedDataStorageDriver.cxx
// Created:	Wed Jun 27 17:25:05 2007
// Author:	Sergey ZARITCHNY
//		<szy@friendox>
// Copyright:   Open CASCADE SA 2007

#include <MDataStd_NamedDataStorageDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <PCollection_HExtendedString.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <PColStd_HArray1OfReal.hxx>
#include <PColStd_HArray1OfInteger.hxx>
#include <PDataStd_NamedData.hxx>
#include <TDataStd_NamedData.hxx>
#include <TColStd_DataMapOfStringInteger.hxx>
#include <TDataStd_DataMapOfStringReal.hxx>
#include <TDataStd_DataMapOfStringString.hxx>
#include <TDataStd_DataMapOfStringByte.hxx>
#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapOfStringHArray1OfReal.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfStringInteger.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringReal.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringString.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringByte.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger.hxx>
#include <TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal.hxx>

#define ROW_NUMBER 6

//=======================================================================
//function : MDataStd_NamedDataStorageDriver
//purpose  : 
//=======================================================================
MDataStd_NamedDataStorageDriver::MDataStd_NamedDataStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{

}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================
Standard_Integer MDataStd_NamedDataStorageDriver::VersionNumber() const
{ 
  return 0;
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================
Handle(Standard_Type) MDataStd_NamedDataStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TDataStd_NamedData);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(PDF_Attribute) MDataStd_NamedDataStorageDriver::NewEmpty() const 
{
  return new PDataStd_NamedData();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void MDataStd_NamedDataStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
					    const Handle(PDF_Attribute)& Target,
					    const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_NamedData) S = Handle(TDataStd_NamedData)::DownCast (Source);
  Handle(PDataStd_NamedData) T = Handle(PDataStd_NamedData)::DownCast (Target);
  
  if(S.IsNull() || T.IsNull()) return;
  Standard_Integer i=1;
  Handle(TColStd_HArray2OfInteger) aDim = new TColStd_HArray2OfInteger(1,ROW_NUMBER, 1, 2, 0);
  if(S->HasIntegers() && !S->GetIntegersContainer().IsEmpty()) {
    aDim->SetValue (i,1,1);
    aDim->SetValue (i,2,S->GetIntegersContainer().Extent());    
  }
  i++;
  if(S->HasReals() && !S->GetRealsContainer().IsEmpty()) {
    aDim->SetValue (i,1,1);
    aDim->SetValue (i,2,S->GetRealsContainer().Extent());
  }
  i++;
  if(S->HasStrings() && !S->GetStringsContainer().IsEmpty()) {
    aDim->SetValue (i,1,1);
    aDim->SetValue (i,2,S->GetStringsContainer().Extent());    
  }
  i++;
  if(S->HasBytes() && !S->GetBytesContainer().IsEmpty()) {
    aDim->SetValue (i,1,1);
    aDim->SetValue (i,2,S->GetBytesContainer().Extent());    
  }
  i++;
  if(S->HasArraysOfIntegers() && !S->GetArraysOfIntegersContainer().IsEmpty()) {
    aDim->SetValue (i,1,1);
    aDim->SetValue (i,2,S->GetArraysOfIntegersContainer().Extent());    
  }
  i++;
  if(S->HasArraysOfReals() && !S->GetArraysOfRealsContainer().IsEmpty()) {
    aDim->SetValue (i,1,1);
    aDim->SetValue (i,2,S->GetArraysOfRealsContainer().Extent());
  }

  T->Init(aDim);

  if(S->HasIntegers() && !S->GetIntegersContainer().IsEmpty()) {
    TColStd_DataMapIteratorOfDataMapOfStringInteger itr(S->GetIntegersContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      Handle(PCollection_HExtendedString) aKey = new PCollection_HExtendedString(itr.Key());
      T->SetIntDataItem (i, aKey, itr.Value());
    }
  }
  if(S->HasReals() && !S->GetRealsContainer().IsEmpty()) {
    TDataStd_DataMapIteratorOfDataMapOfStringReal itr(S->GetRealsContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      Handle(PCollection_HExtendedString) aKey = new PCollection_HExtendedString(itr.Key());
      T->SetRealDataItem (i, aKey, itr.Value());
    }
  }
  if(S->HasStrings() && !S->GetStringsContainer().IsEmpty()) {
    TDataStd_DataMapIteratorOfDataMapOfStringString itr(S->GetStringsContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      Handle(PCollection_HExtendedString) aKey   = new PCollection_HExtendedString(itr.Key());
      Handle(PCollection_HExtendedString) aValue = new PCollection_HExtendedString(itr.Value());
      T->SetStrDataItem (i, aKey, aValue);
    }
  }
  if(S->HasBytes() && !S->GetBytesContainer().IsEmpty()) {
    TDataStd_DataMapIteratorOfDataMapOfStringByte itr(S->GetBytesContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      Handle(PCollection_HExtendedString) aKey = new PCollection_HExtendedString(itr.Key());
      T->SetByteDataItem (i, aKey, itr.Value());
    }
  }
  if(S->HasArraysOfIntegers() && !S->GetArraysOfIntegersContainer().IsEmpty()) {
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfInteger itr(S->GetArraysOfIntegersContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      Handle(PCollection_HExtendedString) aKey = new PCollection_HExtendedString(itr.Key());
      const Handle(TColStd_HArray1OfInteger)& anArr1 = itr.Value();
      Handle(PColStd_HArray1OfInteger) anArr2 = 
	new PColStd_HArray1OfInteger(anArr1->Lower(), anArr1->Upper());
      for(Standard_Integer j= anArr1->Lower(); j<=anArr1->Upper();j++) {
	anArr2->SetValue(j, anArr1->Value(j));
      }
      T->SetArrIntDataItem (i, aKey, anArr2);
    }
  }
  if(S->HasArraysOfReals() && !S->GetArraysOfRealsContainer().IsEmpty()) {
    TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal itr(S->GetArraysOfRealsContainer());
    for (i=1; itr.More(); itr.Next(),i++) {
      Handle(PCollection_HExtendedString) aKey = new PCollection_HExtendedString(itr.Key());
      const Handle(TColStd_HArray1OfReal)& anArr1 = itr.Value();
      Handle(PColStd_HArray1OfReal) anArr2 = 
	new PColStd_HArray1OfReal(anArr1->Lower(), anArr1->Upper());
      for(Standard_Integer j= anArr1->Lower(); j<=anArr1->Upper();j++) {
	anArr2->SetValue(j, anArr1->Value(j));
      }
      T->SetArrRealDataItem (i, aKey, anArr2);
    }
  }
}

