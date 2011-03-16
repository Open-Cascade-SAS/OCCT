// File:	BRepAlgo_AsDes.cxx
// Created:	Thu Oct 26 14:31:13 1995
// Author:	Yves FRICAUD
//		<yfr@stylox>


#include <BRepAlgo_AsDes.ixx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

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
  TopTools_ListIteratorOfListOfShape it(L);
  
  while(it.More()) {
    if (it.Value().IsSame(OldS)) {
      TopAbs_Orientation O = it.Value().Orientation();
      L.InsertBefore(NewS.Oriented(O),it);
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
  Standard_Boolean InUp;

  if (up.IsBound(OldS)) {
    InUp = Standard_False;
    BackReplace (OldS,NewS,up(OldS),InUp);
    if (up.IsBound(NewS)) {
      up(NewS).Append(up(OldS));
    }
    else {
      up.Bind(NewS,up(OldS));
    }
    up.UnBind(OldS);
  }
  
  if (down.IsBound(OldS)) {
    InUp = Standard_True;
    BackReplace(OldS,NewS,down (OldS),InUp);
    if (down.IsBound(NewS)) {
      down(NewS).Append(down(OldS));
    }
    else {
      down.Bind(NewS,down(OldS));
    }
    down.UnBind(OldS);
  }
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void BRepAlgo_AsDes::Remove(const TopoDS_Shape& SS)
{
  if (down.IsBound(SS)) {
    Standard_ConstructionError::Raise(" BRepAlgo_AsDes::Remove");
  }
  if (!up.IsBound(SS)) {
    Standard_ConstructionError::Raise(" BRepAlgo_AsDes::Remove");
  }
  TopTools_ListIteratorOfListOfShape it(up(SS));
  for (; it.More(); it.Next()) {
    RemoveInList(SS,down.ChangeFind((it.Value())));
  }
  up.UnBind(SS);
}
