// Created on: 1995-12-08
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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


#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_PointExplorer.hxx>

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
