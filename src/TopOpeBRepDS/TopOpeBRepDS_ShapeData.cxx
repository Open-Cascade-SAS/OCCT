// Created on: 1995-11-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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


#include <TopOpeBRepDS_ShapeData.ixx>

//=======================================================================
//function : TopOpeBRepDS_ShapeData
//purpose  : 
//=======================================================================

TopOpeBRepDS_ShapeData::TopOpeBRepDS_ShapeData() :
  mySameDomainRef(0),
  mySameDomainOri(TopOpeBRepDS_UNSHGEOMETRY),
  mySameDomainInd(0),
  myOrientation(TopAbs_FORWARD),
  myOrientationDef(Standard_False),
  myAncestorRank(0),
  myKeep(Standard_True)
{
}

//=======================================================================
//function : Interferences
//purpose  : 
//=======================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_ShapeData::Interferences() const
{
  return myInterferences;
}

//=======================================================================
//function : ChangeInterferences
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_ShapeData::ChangeInterferences()
{
  return myInterferences;
}

//=======================================================================
//function : Keep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_ShapeData::Keep() const
{
  return myKeep;
}
//=======================================================================
//function : ChangeKeep
//purpose  : 
//=======================================================================

void TopOpeBRepDS_ShapeData::ChangeKeep(const Standard_Boolean b)
{
  myKeep = b;
}
