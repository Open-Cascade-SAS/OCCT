// File:	BOPTools_Interference.cxx
// Created:	Tue Nov 21 11:44:31 2000
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOPTools_Interference.ixx>

//=======================================================================
//function : BOPTools_Interference::BOPTools_Interference
//purpose  : 
//=======================================================================
  BOPTools_Interference::BOPTools_Interference()
:
  myWith(0),
  myType(BooleanOperations_UnknownInterference),
  myIndex(0)
{
}

//=======================================================================
//function : BOPTools_Interference::BOPTools_Interference
//purpose  : 
//=======================================================================
  BOPTools_Interference::BOPTools_Interference(const Standard_Integer aWith,
					       const BooleanOperations_KindOfInterference aType,
					       const Standard_Integer anIndex)
:
  myWith(aWith),
  myType(aType),
  myIndex(anIndex)
{
}
//=======================================================================
//function : SetWith
//purpose  : 
//=======================================================================
  void BOPTools_Interference::SetWith(const Standard_Integer aWith) 
{
  myWith=aWith;
}

//=======================================================================
//function : SetType
//purpose  : 
//=======================================================================
  void BOPTools_Interference::SetType(const BooleanOperations_KindOfInterference aType) 
{
  myType=aType;
}

//=======================================================================
//function : SetIndex
//purpose  : 
//=======================================================================
  void BOPTools_Interference::SetIndex(const Standard_Integer anIndex) 
{
  myIndex=anIndex;
}

//=======================================================================
//function : With
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_Interference::With() const
{
  return myWith;
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
  BooleanOperations_KindOfInterference BOPTools_Interference::Type() const
{
  return myType;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_Interference::Index() const
{
  return myIndex;
}
