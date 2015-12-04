// Created on: 2007-10-30
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
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDataStd_DeltaOnModificationOfRealArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfRealArray,TDF_DeltaOnModification)

#ifdef OCCT_DEBUG
#define MAXUP 1000
#endif
//=======================================================================
//function : TDataStd_DeltaOnModificationOfRealArray
//purpose  : 
//=======================================================================

TDataStd_DeltaOnModificationOfRealArray::
  TDataStd_DeltaOnModificationOfRealArray(const Handle(TDataStd_RealArray)& OldAtt)
: TDF_DeltaOnModification(OldAtt)
{
  Handle(TDataStd_RealArray) CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(),CurrAtt)) {
    Handle(TColStd_HArray1OfReal) Arr1, Arr2;
    Arr1 = OldAtt->Array();
    Arr2 = CurrAtt->Array();
#ifdef OCCT_DEBUG
    if(Arr1.IsNull())
      cout <<"DeltaOnModificationOfRealArray:: Old Array is Null" <<endl;
    if(Arr2.IsNull())
      cout <<"DeltaOnModificationOfRealArray:: Current Array is Null" <<endl;
#endif

    if(Arr1.IsNull() || Arr2.IsNull()) return;
    if(Arr1 != Arr2) {
      myUp1 = Arr1->Upper();
      myUp2 = Arr2->Upper();
      Standard_Integer i, N =0, aCase=0; 
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
	myValues = new TColStd_HArray1OfReal(1,aList.Extent());
	TColStd_ListIteratorOfListOfInteger anIt(aList);
	for(i =1;anIt.More();anIt.Next(),i++) {
	  myIndxes->SetValue(i, anIt.Value());
	  myValues->SetValue(i, Arr1->Value(anIt.Value()));
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

void TDataStd_DeltaOnModificationOfRealArray::Apply()
{

  Handle(TDF_Attribute) TDFAttribute = Attribute();
  Handle(TDataStd_RealArray) BackAtt = Handle(TDataStd_RealArray)::DownCast (TDFAttribute);
  if(BackAtt.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "DeltaOnModificationOfRealArray::Apply: OldAtt is Null" <<endl;
#endif
    return;
  }
  
  Handle(TDataStd_RealArray) aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(),aCurAtt)) {

    Label().AddAttribute(BackAtt);
  }

  if(aCurAtt.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "DeltaOnModificationOfRealArray::Apply: CurAtt is Null" <<endl;
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

  if (aCase == 1 && (myIndxes.IsNull() || myValues.IsNull()))
    return;
  
  Standard_Integer i;
  Handle(TColStd_HArray1OfReal) aRealArr = aCurAtt->Array();
  if(aRealArr.IsNull()) return;
  if(aCase == 1)   
    for(i = 1; i <= myIndxes->Upper();i++) 
      aRealArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
  else if(aCase == 2) {    
    Handle(TColStd_HArray1OfReal) realArr = new TColStd_HArray1OfReal(aRealArr->Lower(), myUp1);
    for(i = aRealArr->Lower(); i <= myUp1 && i <= aRealArr->Upper(); i++) 
      realArr->SetValue(i, aRealArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) 
	realArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
    aCurAtt->myValue = realArr;
  }
  else { // == 3
    Standard_Integer low = aRealArr->Lower();
    Handle(TColStd_HArray1OfReal) realArr = new TColStd_HArray1OfReal(low, myUp1);
    for(i = aRealArr->Lower(); i <= myUp2 && i <= aRealArr->Upper(); i++) 
      realArr->SetValue(i, aRealArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) {
#ifdef OCCT_DEBUG
	cout << "i = " << i << "  myIndxes->Upper = " << myIndxes->Upper() << endl;
	cout << "myIndxes->Value(i) = " << myIndxes->Value(i) << endl;
	cout << "myValues->Value(i) = " << myValues->Value(i) << endl;
#endif
	realArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));      
      }
    aCurAtt->myValue = realArr;
  }
    

#ifdef OCCT_DEBUG
  cout << " << RealArray Dump after Delta Apply >>" <<endl;
  Handle(TColStd_HArray1OfReal) aRArr = aCurAtt->Array();
  for(i=aRArr->Lower(); i<=aRArr->Upper() && i <= MAXUP;i++)
    cout << aRArr->Value(i) << "  ";
  cout <<endl;
#endif
}


