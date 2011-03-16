// File:	LProp_CurAndInf.cxx
// Created:	Mon Sep  5 10:43:19 1994
// Author:	Yves FRICAUD
//		<yfr@ecolox>


#include <LProp_CurAndInf.ixx>
#include <Standard_OutOfRange.hxx>
#include <ElCLib.hxx>

//=======================================================================
//function : LProp_CurAndInf
//purpose  : 
//=======================================================================

LProp_CurAndInf::LProp_CurAndInf()
{
}


//=======================================================================
//function : AddInflection
//purpose  : 
//=======================================================================

void LProp_CurAndInf::AddInflection(const Standard_Real Param)
{
  if (theParams.IsEmpty()) {
    theParams.Append(Param);
    theTypes .Append(LProp_Inflection);
    return;
  }
  if (Param > theParams.Last()) {
    theParams.Append(Param);
    theTypes .Append(LProp_Inflection);
    return;
  }
  for (Standard_Integer i = 1; i <= theParams.Length(); i++) {
    if (theParams.Value(i) > Param) {
      theParams.InsertBefore(i, Param);
      theTypes .InsertBefore(i, LProp_Inflection);
      break;
    }
  }
}


//=======================================================================
//function : AddExtCur
//purpose  : 
//=======================================================================

void LProp_CurAndInf::AddExtCur(const Standard_Real    Param, 
				const Standard_Boolean IsMin)
{ 
  LProp_CIType TypePoint;
  if (IsMin )  TypePoint = LProp_MinCur; else TypePoint = LProp_MaxCur;

  if (theParams.IsEmpty()) {
    theParams.Append(Param);
    theTypes .Append(TypePoint);
    return;
  }
  if (Param > theParams.Last()) {
    theParams.Append(Param);
    theTypes .Append(TypePoint);
    return;
  }
  for (Standard_Integer i = 1; i <= theParams.Length(); i++) {
    if (theParams.Value(i) > Param) {
      theParams.InsertBefore(i, Param);
      theTypes .InsertBefore(i, TypePoint);
      break;
    }
  }
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void LProp_CurAndInf::Clear()
{
  theParams.Clear();
  theTypes .Clear();
}


//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================

Standard_Boolean LProp_CurAndInf::IsEmpty() const 
{
  return theParams.IsEmpty();
}


//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

Standard_Integer LProp_CurAndInf::NbPoints() const 
{
  return theParams.Length();
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real LProp_CurAndInf::Parameter(const Standard_Integer N) const 
{
  if (N <1 || N > NbPoints ()) {Standard_OutOfRange::Raise();}
  return theParams.Value(N);
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

LProp_CIType LProp_CurAndInf::Type(const Standard_Integer N) const 
{  
  if (N <1 || N > NbPoints()) {Standard_OutOfRange::Raise();}
  return theTypes.Value(N);
}



