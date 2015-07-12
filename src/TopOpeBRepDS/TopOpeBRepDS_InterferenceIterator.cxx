// Created on: 1994-06-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceIterator.hxx>

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

const Handle(TopOpeBRepDS_Interference)& TopOpeBRepDS_InterferenceIterator::Value() const 
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
