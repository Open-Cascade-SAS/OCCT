// File:	BRepCheck_Result.cxx
// Created:	Thu Dec  7 10:41:49 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepCheck_Result.ixx>
#include <BRepCheck.hxx>


//=======================================================================
//function : BRepCheck_Result
//purpose  : 
//=======================================================================

BRepCheck_Result::BRepCheck_Result() :
   myMin(Standard_False),myBlind(Standard_False)
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepCheck_Result::Init(const TopoDS_Shape& S)
{
  myShape = S;
  myMin = Standard_False;
  myBlind = Standard_False;
  myMap.Clear();
  Minimum();
}

//=======================================================================
//function : SetFailStatus
//purpose  : 
//=======================================================================
void BRepCheck_Result::SetFailStatus(const TopoDS_Shape& S) 
{
  if(!myMap.IsBound(S)) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(S, thelist);
  }
  BRepCheck::Add(myMap(S), BRepCheck_CheckFail);
}


//=======================================================================
//function : StatusOnShape
//purpose  : 
//=======================================================================

const BRepCheck_ListOfStatus& BRepCheck_Result::StatusOnShape
   (const TopoDS_Shape& S)
{
  if (!myMap.IsBound(S)) {
    InContext(S);
  }
  return myMap(S);
}


//=======================================================================
//function : InitContextIterator
//purpose  : 
//=======================================================================

void BRepCheck_Result::InitContextIterator()
{
  myIter.Initialize(myMap);
  // Au minimum 1 element : le Shape lui meme
  if (myIter.Key().IsSame(myShape)) {
    myIter.Next();
  }
}


//=======================================================================
//function : NextShapeInContext
//purpose  : 
//=======================================================================

void BRepCheck_Result::NextShapeInContext()
{
  myIter.Next();
  if (myIter.More() && myIter.Key().IsSame(myShape)) {
    myIter.Next();
  }
}  
