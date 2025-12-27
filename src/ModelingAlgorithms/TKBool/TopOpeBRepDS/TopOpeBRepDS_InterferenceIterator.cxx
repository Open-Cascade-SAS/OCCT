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

#include <TopOpeBRepDS_InterferenceIterator.hxx>

//=================================================================================================

TopOpeBRepDS_InterferenceIterator::TopOpeBRepDS_InterferenceIterator()
    : myGKDef(false),
      myGDef(false),
      mySKDef(false),
      mySDef(false)
{
}

//=================================================================================================

TopOpeBRepDS_InterferenceIterator::TopOpeBRepDS_InterferenceIterator(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L)
    : myGKDef(false),
      myGDef(false),
      mySKDef(false),
      mySDef(false)
{
  Init(L);
}

//=================================================================================================

void TopOpeBRepDS_InterferenceIterator::Init(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L)
{
  myIterator.Initialize(L);
  Match();
}

//=================================================================================================

void TopOpeBRepDS_InterferenceIterator::GeometryKind(const TopOpeBRepDS_Kind GK)
{
  myGKDef = true;
  myGK    = GK;
}

//=================================================================================================

void TopOpeBRepDS_InterferenceIterator::Geometry(const int G)
{
  myGDef = true;
  myG    = G;
}

//=================================================================================================

void TopOpeBRepDS_InterferenceIterator::SupportKind(const TopOpeBRepDS_Kind ST)
{
  mySKDef = true;
  mySK    = ST;
}

//=================================================================================================

void TopOpeBRepDS_InterferenceIterator::Support(const int S)
{
  mySDef = true;
  myS    = S;
}

//=================================================================================================

void TopOpeBRepDS_InterferenceIterator::Match()
{
  while (myIterator.More())
  {
    occ::handle<TopOpeBRepDS_Interference> I = myIterator.Value();
    bool                  b = MatchInterference(I);
    if (!b)
    {
      myIterator.Next();
    }
    else
      break;
  }
}

//=================================================================================================

bool TopOpeBRepDS_InterferenceIterator::MatchInterference(
  const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  bool GK = myGKDef ? (I->GeometryType() == myGK) : true;
  bool SK = mySKDef ? (I->SupportType() == mySK) : true;
  bool G  = myGDef ? (I->Geometry() == myG) : true;
  bool S  = mySDef ? (I->Support() == myS) : true;
  return (GK && SK && G && S);
}

//=================================================================================================

bool TopOpeBRepDS_InterferenceIterator::More() const
{
  return myIterator.More();
}

//=================================================================================================

void TopOpeBRepDS_InterferenceIterator::Next()
{
  if (myIterator.More())
  {
    myIterator.Next();
    Match();
  }
}

//=================================================================================================

const occ::handle<TopOpeBRepDS_Interference>& TopOpeBRepDS_InterferenceIterator::Value() const
{
  return myIterator.Value();
}

//=================================================================================================

NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& TopOpeBRepDS_InterferenceIterator::ChangeIterator()
{
  return myIterator;
}
