// Created on: 2008-01-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <TDataStd_DeltaOnModificationOfIntPackedMap.ixx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

#ifdef DEB
#define MAXUP 1000
#endif

//=======================================================================
//function : TDataStd_DeltaOnModificationOfIntPackedMap
//purpose  : 
//=======================================================================

TDataStd_DeltaOnModificationOfIntPackedMap::TDataStd_DeltaOnModificationOfIntPackedMap(
                             const Handle(TDataStd_IntPackedMap)& OldAtt)
: TDF_DeltaOnModification(OldAtt)
{
  Handle(TDataStd_IntPackedMap) CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(),CurrAtt)) {
    {
      Handle(TColStd_HPackedMapOfInteger) aMap1, aMap2;
      aMap1 = OldAtt->GetHMap();
      aMap2 = CurrAtt->GetHMap();
#ifdef DEB
      if(aMap1.IsNull())
	cout <<"DeltaOnModificationOfIntPackedMap:: Old Map is Null" <<endl;
      if(aMap2.IsNull())
	cout <<"DeltaOnModificationOfIntPackedMap:: Current Map is Null" <<endl;
#endif
      
      if(aMap1.IsNull() || aMap2.IsNull()) return;
      if(aMap1 != aMap2) {
	if(!aMap1->Map().HasIntersection(aMap2->Map()))
	  return; // no intersection: use full-scale backup

	if(aMap1->Map().IsSubset(aMap2->Map())) { 
	  myDeletion = new TColStd_HPackedMapOfInteger();
	  myDeletion->ChangeMap().Subtraction(aMap2->Map(), aMap1->Map());
	} else if(aMap2->Map().IsSubset(aMap1->Map())) { 
	  myAddition = new TColStd_HPackedMapOfInteger();
	  myAddition->ChangeMap().Subtraction(aMap1->Map(), aMap2->Map());
	} else {
	  myAddition = new TColStd_HPackedMapOfInteger();
	  myAddition->ChangeMap().Subtraction(aMap1->Map(), aMap2->Map());
	  myDeletion = new TColStd_HPackedMapOfInteger();
	  myDeletion->ChangeMap().Subtraction(aMap2->Map(), aMap1->Map());
	}
      }
    }
    OldAtt->RemoveMap();
#ifdef DEB
    if(OldAtt->GetHMap().IsNull())
      cout << "BackUp Arr is Nullified" << endl;
#endif
  }
}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDataStd_DeltaOnModificationOfIntPackedMap::Apply()
{

  Handle(TDF_Attribute) aTDFAttribute = Attribute();
  Handle(TDataStd_IntPackedMap) aBackAtt = (*((Handle(TDataStd_IntPackedMap)*)&aTDFAttribute));
  if(aBackAtt.IsNull()) {
#ifdef DEB
    cout << "DeltaOnModificationOfIntPAckedMap::Apply: OldAtt is Null" <<endl;
#endif
    return;
  }
  
  Handle(TDataStd_IntPackedMap) aCurAtt;
  if (!Label().FindAttribute(aBackAtt->ID(),aCurAtt)) {

    Label().AddAttribute(aBackAtt);
  }

  if(aCurAtt.IsNull()) {
#ifdef DEB
    cout << "DeltaOnModificationOfIntAPckedMAp::Apply: CurAtt is Null" <<endl;
#endif
    return;
  }
  else 
    aCurAtt->Backup();

  
  
  Handle(TColStd_HPackedMapOfInteger) IntMap = aCurAtt->GetHMap();
  if(IntMap.IsNull()) return;

  if(myDeletion.IsNull() && myAddition.IsNull())
    return;
  else {
    if(!myDeletion.IsNull()) {
  
      if(myDeletion->Map().Extent())
	IntMap->ChangeMap().Subtract(myDeletion->Map());
    } 
    if(!myAddition.IsNull()) {
      if(myAddition->Map().Extent())
	IntMap->ChangeMap().Unite(myAddition->Map());
    }
  }
  
#ifdef DEB    
  cout << " << Map Dump after Delta Apply >>" <<endl;
  Handle(TColStd_HPackedMapOfInteger) aIntMap = aCurAtt->GetHMap();
  TColStd_MapIteratorOfPackedMapOfInteger it(aIntMap->Map());
  for (Standard_Integer i=1;it.More() && i <= MAXUP; it.Next(), i++) 
    cout << it.Key() << "  ";
  cout <<endl;
#endif
}


