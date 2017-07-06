// Created on: 1995-10-26
// Created by: Yves FRICAUD
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <BRepAlgo_AsDes.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepAlgo_AsDes,Standard_Transient)

//=======================================================================
//function : BRepAlgo_AsDes
//purpose  : 
//=======================================================================
BRepAlgo_AsDes::BRepAlgo_AsDes()
{
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepAlgo_AsDes::Add(const TopoDS_Shape& S, const TopoDS_Shape& SS)
{
  if (!down.IsBound(S)) {
    TopTools_ListOfShape L;
    down.Bind(S,L);
  }
  down(S).Append(SS);

  if (!up.IsBound(SS)) {    
    TopTools_ListOfShape L;
    up.Bind(SS,L);
  }
  up(SS).Append(S);
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepAlgo_AsDes::Add(const TopoDS_Shape& S, const TopTools_ListOfShape& SS)
{
  TopTools_ListIteratorOfListOfShape it(SS);
  for ( ; it.More(); it.Next()) {
    Add(S,it.Value());
  }
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepAlgo_AsDes::Clear() 
{
  up  .Clear();
  down.Clear();
}


//=======================================================================
//function : HasAscendant
//purpose  : 
//=======================================================================

Standard_Boolean BRepAlgo_AsDes::HasAscendant(const TopoDS_Shape& S) 
const 
{
  return up.IsBound(S);
} 

//=======================================================================
//function : HasDescendant
//purpose  : 
//=======================================================================

Standard_Boolean BRepAlgo_AsDes::HasDescendant(const TopoDS_Shape& S) 
const 
{
  return down.IsBound(S);
} 

//=======================================================================
//function : Ascendant
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepAlgo_AsDes::Ascendant(const TopoDS_Shape& S) const 
{
  if (up.IsBound(S))
    return up(S);
  static TopTools_ListOfShape empty;
  return empty; 
}


//=======================================================================
//function : Descendant
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepAlgo_AsDes::Descendant(const TopoDS_Shape& S) const 
{
  if (down.IsBound(S)) 
    return down(S);
  static TopTools_ListOfShape empty;
  return empty;
}

//=======================================================================
//function : ChangeDescendant
//purpose  : 
//=======================================================================

TopTools_ListOfShape& BRepAlgo_AsDes::ChangeDescendant(const TopoDS_Shape& S) 
{
  if (down.IsBound(S)) 
    return down.ChangeFind(S);
  static TopTools_ListOfShape empty;
  return empty;
}

//=======================================================================
//function : ReplaceInList
//purpose  : 
//=======================================================================

static void ReplaceInList(const TopoDS_Shape&   OldS,
			  const TopoDS_Shape&   NewS,
			  TopTools_ListOfShape& L)
{
  TopTools_MapOfOrientedShape aMS;
  TopTools_ListIteratorOfListOfShape it(L);
  for (; it.More(); it.Next()) {
    aMS.Add(it.Value());
  }
  it.Initialize(L);
  while(it.More()) {
    if (it.Value().IsSame(OldS)) {
      TopAbs_Orientation O = it.Value().Orientation();
      if (aMS.Add(NewS.Oriented(O))) {
        L.InsertBefore(NewS.Oriented(O),it);
      }
      L.Remove(it);
    }
    else it.Next();
  }
}
//=======================================================================
//function : RemoveInList
//purpose  : 
//=======================================================================

static void RemoveInList(const TopoDS_Shape&   S,
			 TopTools_ListOfShape& L)
{
  TopTools_ListIteratorOfListOfShape it(L);
  while(it.More()) {
    if (it.Value().IsSame(S)) {
      L.Remove(it);
      break;
    }
    it.Next();
  }
}

//=======================================================================
//function : HasCommonDescendant
//purpose  : 
//=======================================================================

Standard_Boolean BRepAlgo_AsDes::HasCommonDescendant(const TopoDS_Shape& S1, 
						       const TopoDS_Shape& S2, 
						       TopTools_ListOfShape& LC)
const 
{
  LC.Clear();
  if (HasDescendant (S1) && HasDescendant (S2)) {
    TopTools_ListIteratorOfListOfShape it1(Descendant(S1));
    for (; it1.More(); it1.Next()) {
      const TopoDS_Shape& DS1 = it1.Value();
      TopTools_ListIteratorOfListOfShape it2(Ascendant(DS1));
      for (; it2.More(); it2.Next()) {
	const TopoDS_Shape& ADS1 = it2.Value();
	if (ADS1.IsSame(S2)) {
	  LC.Append(DS1);
	}
      }
    }
  }
  return (!LC.IsEmpty());
} 

//=======================================================================
//function : BackReplace
//purpose  : 
//=======================================================================

void BRepAlgo_AsDes::BackReplace(const TopoDS_Shape&         OldS,
				   const TopoDS_Shape&         NewS,
				   const TopTools_ListOfShape& L,
				   const Standard_Boolean      InUp)
{
  TopTools_ListIteratorOfListOfShape it(L);
  for ( ; it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value();
    if (InUp) {
      if (up.IsBound(S)) {
	ReplaceInList(OldS,NewS,up.ChangeFind(S));
      }
    }
    else {      
      if (down.IsBound(S)) {
	ReplaceInList(OldS,NewS,down.ChangeFind(S));
      }
    }
  }
}

//=======================================================================
//function : Replace
//purpose  : 
//=======================================================================
void BRepAlgo_AsDes::Replace(const TopoDS_Shape& OldS,
                             const TopoDS_Shape& NewS)
{
  for (Standard_Integer i = 0; i < 2; ++i) {
    TopTools_DataMapOfShapeListOfShape& aMap = !i ? up : down;
    TopTools_ListOfShape* pLSOld = aMap.ChangeSeek(OldS);
    if (!pLSOld) {
      continue;
    }
    //
    Standard_Boolean InUp = !i ? Standard_False : Standard_True;
    BackReplace(OldS, NewS, *pLSOld, InUp);
    //
    TopTools_ListOfShape* pLSNew = aMap.ChangeSeek(NewS);
    if (!pLSNew) {
      // filter the list
      TopTools_MapOfOrientedShape aMS;
      TopTools_ListIteratorOfListOfShape aIt(*pLSOld);
      for (; aIt.More(); ) {
        if (!aMS.Add(aIt.Value())) {
          pLSOld->Remove(aIt);
        }
        else {
          aIt.Next();
        }
      }
      aMap.Bind(NewS, *pLSOld);
    }
    else {
      // avoid duplicates
      TopTools_MapOfOrientedShape aMS;
      TopTools_ListIteratorOfListOfShape aIt(*pLSNew);
      for (; aIt.More(); aIt.Next()) {
        aMS.Add(aIt.Value());
      }
      //
      aIt.Initialize(*pLSOld);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aS = aIt.Value();
        if (aMS.Add(aS)) {
          pLSNew->Append(aS);
        }
      }
    }
    //
    aMap.UnBind(OldS);
  }
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void BRepAlgo_AsDes::Remove(const TopoDS_Shape& SS)
{
  if (down.IsBound(SS)) {
    throw Standard_ConstructionError(" BRepAlgo_AsDes::Remove");
  }
  if (!up.IsBound(SS)) {
    throw Standard_ConstructionError(" BRepAlgo_AsDes::Remove");
  }
  TopTools_ListIteratorOfListOfShape it(up(SS));
  for (; it.More(); it.Next()) {
    RemoveInList(SS,down.ChangeFind((it.Value())));
  }
  up.UnBind(SS);
}
