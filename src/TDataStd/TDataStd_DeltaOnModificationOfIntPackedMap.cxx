// Created on: 2008-01-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TDataStd_DeltaOnModificationOfIntPackedMap.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfIntPackedMap,TDF_DeltaOnModification)

#ifdef OCCT_DEBUG
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
  if (Label().FindAttribute(OldAtt->ID(), CurrAtt))
  {
    Handle(TColStd_HPackedMapOfInteger) aMap1, aMap2;
    aMap1 = OldAtt->GetHMap();
    aMap2 = CurrAtt->GetHMap();
#ifdef OCCT_DEBUG_disable
    if (aMap1.IsNull())
      cout <<"DeltaOnModificationOfIntPackedMap:: Old Map is Null" <<endl;
    if (aMap2.IsNull())
      cout <<"DeltaOnModificationOfIntPackedMap:: Current Map is Null" <<endl;
#endif
      
    if (aMap1.IsNull() || aMap2.IsNull()) return;
    if (aMap1 != aMap2) {
      const TColStd_PackedMapOfInteger& map1 = aMap1->Map();
      const TColStd_PackedMapOfInteger& map2 = aMap2->Map();
      if (map1.IsSubset(map2)) {
        myDeletion = new TColStd_HPackedMapOfInteger();
        myDeletion->ChangeMap().Subtraction(map2, map1);
      } else if (map2.IsSubset(map1)) { 
        myAddition = new TColStd_HPackedMapOfInteger();
        myAddition->ChangeMap().Subtraction(map1, map2);
      } else if (map1.HasIntersection(map2)) {
        myAddition = new TColStd_HPackedMapOfInteger();
        myAddition->ChangeMap().Subtraction(map1, map2);
        myDeletion = new TColStd_HPackedMapOfInteger();
        myDeletion->ChangeMap().Subtraction(map2, map1);
      } else {
        myAddition = new TColStd_HPackedMapOfInteger(map1);
        myDeletion = new TColStd_HPackedMapOfInteger(map2);
      }
    }
  }
}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDataStd_DeltaOnModificationOfIntPackedMap::Apply()
{

  Handle(TDF_Attribute) aTDFAttribute = Attribute();
  Handle(TDataStd_IntPackedMap) aBackAtt = Handle(TDataStd_IntPackedMap)::DownCast (aTDFAttribute);
  if(aBackAtt.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "DeltaOnModificationOfIntPAckedMap::Apply: OldAtt is Null" <<endl;
#endif
    return;
  }
  
  Handle(TDataStd_IntPackedMap) aCurAtt;
  if (!Label().FindAttribute(aBackAtt->ID(),aCurAtt)) {

    Label().AddAttribute(aBackAtt);
  }

  if(aCurAtt.IsNull()) {
#ifdef OCCT_DEBUG
    cout << "DeltaOnModificationOfIntAPckedMAp::Apply: CurAtt is Null" <<endl;
#endif
    return;
  }
  else 
    aCurAtt->Backup();

  
  
  Handle(TColStd_HPackedMapOfInteger) IntMap = aCurAtt->GetHMap();
  if (IntMap.IsNull()) return;

  if (myDeletion.IsNull() && myAddition.IsNull())
    return;

  if (!myDeletion.IsNull()) {
    if (myDeletion->Map().Extent())
      IntMap->ChangeMap().Subtract(myDeletion->Map());
  }
  if (!myAddition.IsNull()) {
    if (myAddition->Map().Extent())
      IntMap->ChangeMap().Unite(myAddition->Map());
  }
  
#ifdef OCCT_DEBUG_disable
  cout << " << Map Dump after Delta Apply >>" <<endl;
  Handle(TColStd_HPackedMapOfInteger) aIntMap = aCurAtt->GetHMap();
  TColStd_MapIteratorOfPackedMapOfInteger it(aIntMap->Map());
  for (Standard_Integer i=1;it.More() && i <= MAXUP; it.Next(), i++) 
    cout << it.Key() << "  ";
  cout <<endl;
#endif
}
