// Created on: 2007-09-06
// Created by: Sergey ZARITCHNY
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


#include <Standard_Type.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDataStd_DeltaOnModificationOfIntArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfIntArray,TDF_DeltaOnModification)

#ifdef OCCT_DEBUG
#define MAXUP 1000
#endif
//=======================================================================
//function : TDataStd_DeltaOnModificationOfIntArray
//purpose  : 
//=======================================================================

TDataStd_DeltaOnModificationOfIntArray::TDataStd_DeltaOnModificationOfIntArray(const Handle(TDataStd_IntegerArray)& OldAtt)
: TDF_DeltaOnModification(OldAtt)
{
  Handle(TDataStd_IntegerArray) CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(),CurrAtt)) {
    {
      Handle(TColStd_HArray1OfInteger) Arr1, Arr2;
      Arr1 = OldAtt->Array();
      Arr2 = CurrAtt->Array();
#ifdef OCCT_DEBUG
      if(Arr1.IsNull())
	cout <<"DeltaOnModificationOfIntArray:: Old IntArray is Null" <<endl;
      if(Arr2.IsNull())
	cout <<"DeltaOnModificationOfIntArray:: Current IntArray is Null" <<endl;
#endif

      if(Arr1.IsNull() || Arr2.IsNull()) return;
      if(Arr1 != Arr2) {
	myUp1 = Arr1->Upper();
	myUp2 = Arr2->Upper();
	Standard_Integer i, N=0, aCase=0; 
	if(myUp1 == myUp2) 
	  {aCase = 1; N = myUp1;}
	else if(myUp1 < myUp2) 
	  {aCase = 2; N = myUp1;}
	else 
	  {aCase = 3; N = myUp2;}//Up1 > Up2

	TColStd_ListOfInteger aList;
	for(i=Arr1->Lower();i <= N; i++)
	  if(Arr1->Value(i) != Arr2->Value(i)) 
	    aList.Append(i);
	if(aCase == 3) {
	  for(i = N+1;i <= myUp1; i++)
	    aList.Append(i);
	}
	if(aList.Extent()) {
	  myIndxes = new TColStd_HArray1OfInteger(1,aList.Extent());
	  myValues = new TColStd_HArray1OfInteger(1,aList.Extent());
	  TColStd_ListIteratorOfListOfInteger anIt(aList);
	  for(i=1;anIt.More();anIt.Next(),i++) {
	    myIndxes->SetValue(i, anIt.Value());
	    myValues->SetValue(i, Arr1->Value(anIt.Value()));
	  }
	}
      }
    }
    OldAtt->RemoveArray();
#ifdef OCCT_DEBUG
    if(OldAtt->Array().IsNull())
      cout << "BackUp Arr is Nullified" << endl;
#endif
  }
}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDataStd_DeltaOnModificationOfIntArray::Apply()
{

  Handle(TDF_Attribute) TDFAttribute = Attribute();
  Handle(TDataStd_IntegerArray) BackAtt = Handle(TDataStd_IntegerArray)::DownCast (TDFAttribute);
  if(BackAtt.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "DeltaOnModificationOfIntArray::Apply: OldAtt is Null" <<endl;
#endif
    return;
  }
  
  Handle(TDataStd_IntegerArray) aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(),aCurAtt)) {

    Label().AddAttribute(BackAtt);
  }

  if(aCurAtt.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "DeltaOnModificationOfIntArray::Apply: CurAtt is Null" <<endl;
#endif
    return;
  }
  else 
    aCurAtt->Backup();

  Standard_Integer aCase;
  if(myUp1 == myUp2) 
    aCase = 1;
  else if(myUp1 < myUp2) 
    aCase = 2;
  else 
    aCase = 3;//Up1 > Up2
////
  if (aCase == 1 && (myIndxes.IsNull() || myValues.IsNull()))
    return;
  
  Standard_Integer i;
  Handle(TColStd_HArray1OfInteger) IntArr = aCurAtt->Array();
  if(IntArr.IsNull()) return;
  if(aCase == 1) 
    for(i = 1; i <= myIndxes->Upper();i++) 
      IntArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
  else if(aCase == 2) {    
    Handle(TColStd_HArray1OfInteger) intArr = new TColStd_HArray1OfInteger(IntArr->Lower(), myUp1);
    for(i = IntArr->Lower(); i <= myUp1 && i <= IntArr->Upper(); i++) 
      intArr->SetValue(i, IntArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) 
	intArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
    aCurAtt->myValue = intArr;
  }
  else { // aCase == 3
    Standard_Integer low = IntArr->Lower();
    Handle(TColStd_HArray1OfInteger) intArr = new TColStd_HArray1OfInteger(low, myUp1);
    for(i = IntArr->Lower(); i <= myUp2 && i <= IntArr->Upper(); i++) 
      intArr->SetValue(i, IntArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) {
#ifdef OCCT_DEBUG
	cout << "i = " << i << "  myIndxes->Upper = " << myIndxes->Upper() << endl;
	cout << "myIndxes->Value(i) = " << myIndxes->Value(i) << endl;
	cout << "myValues->Value(i) = " << myValues->Value(i) << endl;
#endif
	intArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));      
      }
    aCurAtt->myValue = intArr;
  }
  
#ifdef OCCT_DEBUG
  cout << " << Array Dump after Delta Apply >>" <<endl;
  Handle(TColStd_HArray1OfInteger) IntArr2 = aCurAtt->Array();
  for(i=IntArr2->Lower(); i<=IntArr2->Upper() && i <= MAXUP;i++)
    cout << IntArr2->Value(i) << "  ";
  cout <<endl;
#endif
}


