// Created on: 1995-12-08
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


#include <TopOpeBRepDS_PointExplorer.ixx>
#define MYDS (*((TopOpeBRepDS_DataStructure*)myDS))

//=======================================================================
//function : TopOpeBRepDS_PointExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_PointExplorer::TopOpeBRepDS_PointExplorer() 
: myIndex(1),
  myMax(0),
  myDS(NULL),
  myFound(Standard_False),
  myFindKeep(Standard_False)
{
}

//=======================================================================
//function : TopOpeBRepDS_PointExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_PointExplorer::TopOpeBRepDS_PointExplorer
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{ 
  Init(DS,FindKeep);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_PointExplorer::Init
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{
  myIndex = 1; 
  myMax = DS.NbPoints();
  myDS = (TopOpeBRepDS_DataStructure*)&DS;
  myFindKeep = FindKeep;
  Find();
}


//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

void TopOpeBRepDS_PointExplorer::Find()
{
  myFound = Standard_False;
  while (myIndex <= myMax) {
    if (myFindKeep) {
      myFound = IsPointKeep(myIndex);
    }
    else {
      myFound = IsPoint(myIndex);
    }
    if (myFound) break;
    else myIndex++;
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_PointExplorer::More() const
{
  return myFound;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopOpeBRepDS_PointExplorer::Next()
{
  myIndex++;
  Find();
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Point& TopOpeBRepDS_PointExplorer::Point()const
{
  if ( myFound ) {
    return MYDS.Point(myIndex);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : IsPoint
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_PointExplorer::IsPoint
(const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.myPoints.IsBound(I);
  return b;
}

//=======================================================================
//function : IsPointKeep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_PointExplorer::IsPointKeep
(const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.myPoints.IsBound(I);
  if (b) b = MYDS.Point(I).Keep();
  return b;
}


//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Point& TopOpeBRepDS_PointExplorer::Point
   (const Standard_Integer I)const
{
  if ( IsPoint(I) ) {
    return MYDS.Point(I);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : NbPoint
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_PointExplorer::NbPoint()
{
  myIndex = 1; myMax = MYDS.NbPoints();
  Find();
  Standard_Integer n = 0;
  for (; More(); Next() ) n++;
  return n;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_PointExplorer::Index()const
{
  return myIndex;
}
