// File:	TDataStd_DeltaOnModificationOfByteArray.cxx
// Created:	Wed Dec  5 16:18:51 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
//Copyright:    Open CasCade SA 2007

#include <TDataStd_DeltaOnModificationOfByteArray.ixx>
#include <TDataStd_ByteArray.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TDF_AttributeIterator.hxx>

#ifdef DEB
#define MAXUP 1000
#endif
//=======================================================================
//function : TDataStd_DeltaOnModificationOfByteArray
//purpose  : 
//=======================================================================

TDataStd_DeltaOnModificationOfByteArray::TDataStd_DeltaOnModificationOfByteArray(const Handle(TDataStd_ByteArray)& OldAtt)
: TDF_DeltaOnModification(OldAtt)
{
  Handle(TDataStd_ByteArray) CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(),CurrAtt)) {
    {
      Handle(TColStd_HArray1OfByte) Arr1, Arr2;
      Arr1 = OldAtt->InternalArray();
      Arr2 = CurrAtt->InternalArray();
#ifdef DEB_DELTA
      if(Arr1.IsNull())
	cout <<"DeltaOnModificationOfByteArray:: Old ByteArray is Null" <<endl;
      if(Arr2.IsNull())
	cout <<"DeltaOnModificationOfByteArray:: Current ByteArray is Null" <<endl;
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
	for(i=Arr1->Lower();i<= N; i++)
	  if(Arr1->Value(i) != Arr2->Value(i)) 
	    aList.Append(i);
	if(aCase == 3) {
	  for(i = N+1;i <= myUp1; i++)
	    aList.Append(i);
	}

	if(aList.Extent()) {
	  myIndxes = new TColStd_HArray1OfInteger(1,aList.Extent());
	  myValues = new TColStd_HArray1OfByte(1,aList.Extent());
	  TColStd_ListIteratorOfListOfInteger anIt(aList);
	  for(i =1;anIt.More();anIt.Next(),i++) {
	    myIndxes->SetValue(i, anIt.Value());
	    myValues->SetValue(i, Arr1->Value(anIt.Value()));
	  }
	}
      }
    }
    OldAtt->RemoveArray();
#ifdef DEB
    if(OldAtt->InternalArray().IsNull())
      cout << "BackUp Arr is Nullified" << endl;
#endif
  }
}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDataStd_DeltaOnModificationOfByteArray::Apply()
{

  Handle(TDF_Attribute) TDFAttribute = Attribute();
  Handle(TDataStd_ByteArray) BackAtt = (*((Handle(TDataStd_ByteArray)*)&TDFAttribute));
  if(BackAtt.IsNull()) {
#ifdef DEB
    cout << "DeltaOnModificationOfByteArray::Apply: OldAtt is Null" <<endl;
#endif
    return;
  }
  
  Handle(TDataStd_ByteArray) aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(),aCurAtt)) {

    Label().AddAttribute(BackAtt);
  }

  if(aCurAtt.IsNull()) {
#ifdef DEB
    cout << "DeltaOnModificationOfByteArray::Apply: CurAtt is Null" <<endl;
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
  Handle(TColStd_HArray1OfByte) BArr = aCurAtt->InternalArray();
  if(BArr.IsNull()) return;
  if(aCase == 1)   
    for(i = 1; i <= myIndxes->Upper();i++) 
      BArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
  else if(aCase == 2) {    
    Handle(TColStd_HArray1OfByte) byteArr = new TColStd_HArray1OfByte(BArr->Lower(), myUp1);
    for(i = BArr->Lower(); i <= myUp1 && i <= BArr->Upper(); i++) 
      byteArr->SetValue(i, BArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) 
	byteArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
    aCurAtt->myValue = byteArr;
  }
  else { // aCase == 3
    Standard_Integer low = BArr->Lower();
    Handle(TColStd_HArray1OfByte) byteArr = new TColStd_HArray1OfByte(low, myUp1);
    for(i = BArr->Lower(); i <= myUp2 && i <= BArr->Upper(); i++) 
      byteArr->SetValue(i, BArr->Value(i));
    if(!myIndxes.IsNull() && !myValues.IsNull())
      for(i = 1; i <= myIndxes->Upper();i++) {
#ifdef DEB  
	cout << "i = " << i << "  myIndxes->Upper = " << myIndxes->Upper() << endl;
	cout << "myIndxes->Value(i) = " << myIndxes->Value(i) << endl;
	cout << "myValues->Value(i) = " << myValues->Value(i) << endl;
#endif
	byteArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));      
      }
    aCurAtt->myValue = byteArr;
  }
  
#ifdef DEB
  cout << " << Array Dump after Delta Apply >>" <<endl;
  Handle(TColStd_HArray1OfByte) BArr2 = aCurAtt->InternalArray();
  for(i=BArr2->Lower(); i<=BArr2->Upper() && i<= MAXUP;i++)
    cout << BArr2->Value(i) << "  ";
  cout <<endl;
#endif
}


