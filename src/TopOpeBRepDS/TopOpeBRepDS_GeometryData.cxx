// Created on: 1994-05-26
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


#include <TopOpeBRepDS_GeometryData.ixx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>

//=======================================================================
//function : TopOpeBRepDS_GeometryData
//purpose  : 
//=======================================================================

TopOpeBRepDS_GeometryData::TopOpeBRepDS_GeometryData()
{
}

//modified by NIZNHY-PKV Tue Oct 30 09:25:59 2001 f
//=======================================================================
//function : TopOpeBRepDS_GeometryData::TopOpeBRepDS_GeometryData
//purpose  : 
//=======================================================================
TopOpeBRepDS_GeometryData::TopOpeBRepDS_GeometryData(const TopOpeBRepDS_GeometryData& Other)
{
  Assign(Other);
}
//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================
void TopOpeBRepDS_GeometryData::Assign(const TopOpeBRepDS_GeometryData& Other)
{
  myInterferences.Clear();

  TopOpeBRepDS_ListIteratorOfListOfInterference anIt(Other.myInterferences);
  for (; anIt.More(); anIt.Next()) {
    myInterferences.Append(anIt.Value());
  }
}
//modified by NIZNHY-PKV Tue Oct 30 09:25:49 2001 t

//=======================================================================
//function : Interferences
//purpose  : 
//=======================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_GeometryData::Interferences() const 
{
  return myInterferences;
}

//=======================================================================
//function : ChangeInterferences
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_GeometryData::ChangeInterferences() 
{
  return myInterferences;
}

//=======================================================================
//function : AddInterference
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GeometryData::AddInterference(const Handle(TopOpeBRepDS_Interference)& I)
{
  myInterferences.Append(I);
}
