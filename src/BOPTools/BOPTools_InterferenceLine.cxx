// File:	BOPTools_InterferenceLine.cxx
// Created:	Tue Nov 21 12:19:30 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_InterferenceLine.ixx>
#include <BOPTools_ListIteratorOfListOfInterference.hxx>

//=======================================================================
//function : BOPTools_InterferenceLine::BOPTools_InterferenceLine
//purpose  : 
//=======================================================================
  BOPTools_InterferenceLine::BOPTools_InterferenceLine() {}

//=======================================================================
//function : RealList
//purpose  : 
//=======================================================================
  const BOPTools_ListOfInterference& BOPTools_InterferenceLine::RealList() const
{
  Standard_Integer anInd;
  List();
  BOPTools_ListOfInterference aTmpList;
  BOPTools_ListIteratorOfListOfInterference anIt;

  anIt.Initialize(myList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    aTmpList.Append(anInterference);
  }
  //
  BOPTools_ListOfInterference* pList=(BOPTools_ListOfInterference*)&myList;
  pList->Clear();
  //
  anIt.Initialize(aTmpList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    anInd=anInterference.Index();
    if (anInd) {
      pList->Append(anInterference);
    }
  }
  return myList;
}

//=======================================================================
//function : List
//purpose  : 
//=======================================================================
  const BOPTools_ListOfInterference& BOPTools_InterferenceLine::List () const
{
  BOPTools_ListOfInterference* pList=(BOPTools_ListOfInterference*)&myList;
  
  pList->Clear();

  BOPTools_ListIteratorOfListOfInterference anIt;

  anIt.Initialize(mySSList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    pList->Append(anInterference);
  }

  anIt.Initialize(myESList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    pList->Append(anInterference);
  }

  anIt.Initialize(myVSList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    pList->Append(anInterference);
  }

  anIt.Initialize(myEEList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    pList->Append(anInterference);
  }

  anIt.Initialize(myVEList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    pList->Append(anInterference);
  }

  anIt.Initialize(myVVList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    pList->Append(anInterference);
  }

  return myList;
}

//=======================================================================
//function : GetOnType
//purpose  : 
//=======================================================================
  const BOPTools_ListOfInterference& BOPTools_InterferenceLine::GetOnType
                       (const BooleanOperations_KindOfInterference theType) const
{
  switch (theType) {
  case BooleanOperations_SurfaceSurface:
    return mySSList;

  case BooleanOperations_EdgeSurface:
    return myESList;
    
  case BooleanOperations_VertexSurface:
    return myVSList;
    
  case BooleanOperations_EdgeEdge:
    return myEEList;

  case BooleanOperations_VertexEdge:
    return myVEList;
    
  case BooleanOperations_VertexVertex:
    return  myVVList; 
  default:
    return  myEmptyList;
  }
  
}

//=======================================================================
//function : HasInterference
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_InterferenceLine::HasInterference () const
{
  Standard_Integer anInd;
  Standard_Boolean bFlag=Standard_False;
  BOPTools_ListIteratorOfListOfInterference anIt;
  //
  List();
  //
  anIt.Initialize(myList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    anInd=anInterference.Index();
    if (anInd) {
      return !bFlag;
    }
  }
  return bFlag;
}

//=======================================================================
//function : IsComputed
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_InterferenceLine::IsComputed 
           (const Standard_Integer theWith,
	    const BooleanOperations_KindOfInterference theType)const
{
  BooleanOperations_KindOfInterference aType;
  Standard_Integer aWith, anInd;

  const BOPTools_ListOfInterference& aList=GetOnType(theType);
  BOPTools_ListIteratorOfListOfInterference anIt(aList);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Interference& anInterference=anIt.Value();
    aType=anInterference.Type();
    aWith=anInterference.With();
    //
    anInd=anInterference.Index();
    //
    if ((aType==theType && aWith==theWith) && anInd) {
      return Standard_True;
    }
  }
  return Standard_False;
}
//=======================================================================
//function : AddInterference
//purpose  : 
//=======================================================================
  void BOPTools_InterferenceLine::AddInterference(const BOPTools_Interference& anInterference) 
{
  Standard_Integer aWith, anInd;
  BooleanOperations_KindOfInterference aType;

  aWith=anInterference.With();
  aType=anInterference.Type();
  anInd=anInterference.Index();
  AddInterference(aWith, aType, anInd);
}


//=======================================================================
//function : AddInterference
//purpose  : 
//=======================================================================
  void BOPTools_InterferenceLine::AddInterference(const Standard_Integer aWith,
						  const BooleanOperations_KindOfInterference aType,
						  const Standard_Integer anIndex) 
{
  BOPTools_Interference anInterference(aWith, aType, anIndex);
  const BOPTools_ListOfInterference& aList=GetOnType(aType);
  BOPTools_ListOfInterference* pList=(BOPTools_ListOfInterference*) &aList;
  pList->Append(anInterference);
}

