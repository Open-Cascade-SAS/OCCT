// Created on: 1994-06-06
// Created by: Jean Yves LEBEY
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



#include <TopOpeBRepDS_InterferenceIterator.ixx>


//=======================================================================
//function : TopOpeBRepDS_InterferenceIterator
//purpose  : 
//=======================================================================

TopOpeBRepDS_InterferenceIterator::TopOpeBRepDS_InterferenceIterator() :
  myGKDef(Standard_False),
  myGDef(Standard_False),
  mySKDef(Standard_False),
  mySDef(Standard_False)
{
}


//=======================================================================
//function : TopOpeBRepDS_InterferenceIterator
//purpose  : 
//=======================================================================

TopOpeBRepDS_InterferenceIterator::TopOpeBRepDS_InterferenceIterator
  (const TopOpeBRepDS_ListOfInterference& L) :
  myGKDef(Standard_False),
  myGDef(Standard_False),
  mySKDef(Standard_False),
  mySDef(Standard_False)
{
  Init(L);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_InterferenceIterator::Init(const TopOpeBRepDS_ListOfInterference& L)
{
  myIterator.Initialize(L);
  Match();
}


//=======================================================================
//function : GeometryKind
//purpose  : 
//=======================================================================

void TopOpeBRepDS_InterferenceIterator::GeometryKind(const TopOpeBRepDS_Kind GK)
{
  myGKDef = Standard_True;
  myGK = GK;
}


//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

void TopOpeBRepDS_InterferenceIterator::Geometry(const Standard_Integer G)
{
  myGDef = Standard_True;
  myG = G;
}


//=======================================================================
//function : SupportKind
//purpose  : 
//=======================================================================

void TopOpeBRepDS_InterferenceIterator::SupportKind(const TopOpeBRepDS_Kind ST)
{
  mySKDef = Standard_True;
  mySK = ST;
}


//=======================================================================
//function : Support
//purpose  : 
//=======================================================================

void TopOpeBRepDS_InterferenceIterator::Support(const Standard_Integer S)
{
  mySDef = Standard_True;
  myS = S;
}


//=======================================================================
//function : Match
//purpose  : 
//=======================================================================

void TopOpeBRepDS_InterferenceIterator::Match()
{
  while ( myIterator.More() ) {
    Handle(TopOpeBRepDS_Interference) I = myIterator.Value();
    Standard_Boolean b = MatchInterference(I);
    if ( ! b ) {
      myIterator.Next();
    }
    else break;
  }
}


//=======================================================================
//function : Match
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_InterferenceIterator::MatchInterference
  (const Handle(TopOpeBRepDS_Interference)& I) const 
{
  Standard_Boolean GK = myGKDef ? (I->GeometryType() == myGK) : Standard_True;
  Standard_Boolean SK = mySKDef ? (I->SupportType() == mySK) : Standard_True;
  Standard_Boolean G = myGDef ? (I->Geometry() == myG) : Standard_True;
  Standard_Boolean S = mySDef ? (I->Support() == myS) : Standard_True;
  return (GK && SK && G && S);
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_InterferenceIterator::More() const 
{
  return myIterator.More();
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopOpeBRepDS_InterferenceIterator::Next()
{
  if ( myIterator.More() ) {
    myIterator.Next();
    Match();
  }
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(TopOpeBRepDS_Interference)& TopOpeBRepDS_InterferenceIterator::Value() const 
{
  return myIterator.Value();
}


//=======================================================================
//function : ChangeIterator
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListIteratorOfListOfInterference& 
TopOpeBRepDS_InterferenceIterator::ChangeIterator() 
{
  return myIterator;
}
