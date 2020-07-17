// Created on: 2007-12-04
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

#include <TDataStd_DeltaOnModificationOfExtStringArray.hxx>

#include <Standard_Type.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfExtStringArray,TDF_DeltaOnModification)

#ifdef OCCT_DEBUG
#define MAXUP 1000
#endif
//=======================================================================
//function : TDataStd_DeltaOnModificationOfExtStringArray
//purpose  : 
//=======================================================================

TDataStd_DeltaOnModificationOfExtStringArray::TDataStd_DeltaOnModificationOfExtStringArray(const Handle(TDataStd_ExtStringArray)& OldAtt)
: TDF_DeltaOnModification(OldAtt),
  myUp1(0),
  myUp2(0)
{
  Handle(TDataStd_ExtStringArray) CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(),CurrAtt)) {
    {
      Handle(TColStd_HArray1OfExtendedString) Arr1, Arr2;
      Arr1 = OldAtt->Array();
      Arr2 = CurrAtt->Array();
#ifdef OCCT_DEBUG
      if(Arr1.IsNull())
	std::cout <<"DeltaOnModificationOfExtStringArray:: Old IntArray is Null" <<std::endl;
      if(Arr2.IsNull())
	std::cout <<"DeltaOnModificationOfExtStringArray:: Current IntArray is Null" <<std::endl;
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
	  myValues = new TColStd_HArray1OfExtendedString(1,aList.Extent());
	  TColStd_ListIteratorOfListOfInteger anIt(aList);
	  for(i =1;anIt.More();anIt.Next(),i++) {
	    myIndxes->SetValue(i, anIt.Value());
	    myValues->SetValue(i, Arr1->Value(anIt.Value()));
	  }
	}
      }
    }
    OldAtt->RemoveArray();
#ifdef OCCT_DEBUG
    if(OldAtt->Array().IsNull())
      std::cout << "BackUp Arr is Nullified" << std::endl;
#endif
  }
}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDataStd_DeltaOnModificationOfExtStringArray::Apply()
{

  Handle(TDF_Attribute) TDFAttribute = Attribute();
  Handle(TDataStd_ExtStringArray) BackAtt = Handle(TDataStd_ExtStringArray)::DownCast (TDFAttribute);
  if(BackAtt.IsNull()) {
#ifdef OCCT_DEBUG
    std::cout << "DeltaOnModificationOfExtStringArray::Apply: OldAtt is Null" <<std::endl;
#endif
    return;
  }
  
  Handle(TDataStd_ExtStringArray) aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(),aCurAtt)) {

    Label().AddAttribute(BackAtt);
  }

  if(aCurAtt.IsNull()) {
#ifdef OCCT_DEBUG
    std::cout << "DeltaOnModificationOfExtStringArray::Apply: CurAtt is Null" <<std::endl;
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
  Handle(TColStd_HArray1OfExtendedString) aStrArr = aCurAtt->Array();
  if(aStrArr.IsNull()) return;

  if(aCase == 1)   
    for(i = 1; i <= myIndxes->Upper();i++) 
      aStrArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
  else if(aCase == 2) {    
    Handle(TColStd_HArray1OfExtendedString) strArr = 
      new TColStd_HArray1OfExtendedString(aStrArr->Lower(), myUp1);
    for(i = aStrArr->Lower(); i <= myUp1 && i <= aStrArr->Upper(); i++) 
      strArr->SetValue(i, aStrArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) 
	strArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
    aCurAtt->myValue = strArr;
  }
  else { // == 3
    Standard_Integer low = aStrArr->Lower();
    Handle(TColStd_HArray1OfExtendedString) strArr = 
      new TColStd_HArray1OfExtendedString(low, myUp1);
    for(i = aStrArr->Lower(); i <= myUp2 && i <= aStrArr->Upper(); i++) 
      strArr->SetValue(i, aStrArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) {
#ifdef OCCT_DEBUG
	std::cout << "i = " << i << "  myIndxes->Upper = " << myIndxes->Upper() << std::endl;
	std::cout << "myIndxes->Value(i) = " << myIndxes->Value(i) << std::endl;
	std::cout << "myValues->Value(i) = " << myValues->Value(i) << std::endl;
#endif
	strArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));      
      }
    aCurAtt->myValue = strArr;
  }

  
#ifdef OCCT_DEBUG
  std::cout << " << Array Dump after Delta Apply >>" <<std::endl;
  Handle(TColStd_HArray1OfExtendedString) aStrArr2 = aCurAtt->Array();
  for(i=aStrArr2->Lower(); i<= aStrArr2->Upper() && i<= MAXUP;i++)
    std::cout << aStrArr2->Value(i) << "  ";
  std::cout <<std::endl;
#endif
}


