// Created on: 1994-09-05
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



