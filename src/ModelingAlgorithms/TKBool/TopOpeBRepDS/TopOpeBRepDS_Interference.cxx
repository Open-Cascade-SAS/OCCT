// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Transition.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_Interference, Standard_Transient)

//=================================================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference() {}

//=================================================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference(const TopOpeBRepDS_Transition& T,
                                                     const TopOpeBRepDS_Kind        ST,
                                                     const int         S,
                                                     const TopOpeBRepDS_Kind        GT,
                                                     const int         G)
    : myTransition(T),
      mySupport(S),
      myGeometry(G),
      mySupportType(ST),
      myGeometryType(GT)
{
}

//=================================================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference(const occ::handle<TopOpeBRepDS_Interference>& I)
    : myTransition(I->Transition()),
      mySupport(I->Support()),
      myGeometry(I->Geometry()),
      mySupportType(I->SupportType()),
      myGeometryType(I->GeometryType())
{
}

//=================================================================================================

const TopOpeBRepDS_Transition& TopOpeBRepDS_Interference::Transition() const
{
  return myTransition;
}

//=================================================================================================

TopOpeBRepDS_Transition& TopOpeBRepDS_Interference::ChangeTransition()
{
  return myTransition;
}

//=================================================================================================

void TopOpeBRepDS_Interference::Transition(const TopOpeBRepDS_Transition& T)
{
  myTransition = T;
}

//=================================================================================================

void TopOpeBRepDS_Interference::GKGSKS(TopOpeBRepDS_Kind& GK,
                                       int&  G,
                                       TopOpeBRepDS_Kind& SK,
                                       int&  S) const
{
  GK = myGeometryType;
  G  = myGeometry;
  SK = mySupportType;
  S  = mySupport;
}

//=================================================================================================

TopOpeBRepDS_Kind TopOpeBRepDS_Interference::SupportType() const
{
  return mySupportType;
}

//=================================================================================================

int TopOpeBRepDS_Interference::Support() const
{
  return mySupport;
}

//=================================================================================================

TopOpeBRepDS_Kind TopOpeBRepDS_Interference::GeometryType() const
{
  return myGeometryType;
}

//=================================================================================================

int TopOpeBRepDS_Interference::Geometry() const
{
  return myGeometry;
}

//=================================================================================================

void TopOpeBRepDS_Interference::SetGeometry(const int GI)
{
  myGeometry = GI;
}

//=================================================================================================

void TopOpeBRepDS_Interference::SupportType(const TopOpeBRepDS_Kind ST)
{
  mySupportType = ST;
}

//=================================================================================================

void TopOpeBRepDS_Interference::Support(const int S)
{
  mySupport = S;
}

//=================================================================================================

void TopOpeBRepDS_Interference::GeometryType(const TopOpeBRepDS_Kind GT)
{
  myGeometryType = GT;
}

//=================================================================================================

void TopOpeBRepDS_Interference::Geometry(const int G)
{
  myGeometry = G;
}

//=================================================================================================

bool TopOpeBRepDS_Interference::HasSameSupport(
  const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  return (mySupportType == I->mySupportType && mySupport == I->mySupport);
}

//=================================================================================================

bool TopOpeBRepDS_Interference::HasSameGeometry(
  const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  return (myGeometryType == I->myGeometryType && myGeometry == I->myGeometry);
}
