// Created on: 1996-10-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceExplorer.hxx>

#define MYDS (*((TopOpeBRepDS_DataStructure*)myDS))

//=======================================================================
//function : TopOpeBRepDS_SurfaceExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceExplorer::TopOpeBRepDS_SurfaceExplorer() 
: myIndex(1),myMax(0),myDS(NULL),myFound(Standard_False)
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceExplorer::TopOpeBRepDS_SurfaceExplorer
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{ 
  Init(DS,FindKeep);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_SurfaceExplorer::Init
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{
  myIndex = 1; 
  myMax = DS.NbSurfaces();
  myDS = (TopOpeBRepDS_DataStructure*)&DS;
  myFindKeep = FindKeep;
  Find();
}


//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

void TopOpeBRepDS_SurfaceExplorer::Find()
{
  myFound = Standard_False;
  while (myIndex <= myMax) {
    if (myFindKeep) {
      myFound = IsSurfaceKeep(myIndex);
    }
    else {
      myFound = IsSurface(myIndex);
    }
    if (myFound) break;
    else myIndex++;
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_SurfaceExplorer::More() const
{
  return myFound;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopOpeBRepDS_SurfaceExplorer::Next()
{
  myIndex++;
  Find();
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Surface& TopOpeBRepDS_SurfaceExplorer::Surface()const
{
  if ( myFound ) {
    return MYDS.Surface(myIndex);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : IsSurface
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_SurfaceExplorer::IsSurface
   (const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.mySurfaces.IsBound(I);
  return b;
}

//=======================================================================
//function : IsSurfaceKeep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_SurfaceExplorer::IsSurfaceKeep
   (const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.mySurfaces.IsBound(I);
  if (b) b = MYDS.Surface(I).Keep();
  return b;
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Surface& TopOpeBRepDS_SurfaceExplorer::Surface
   (const Standard_Integer I)const
{
  if ( IsSurface(I) ) {
    return MYDS.Surface(I);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : NbSurface
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_SurfaceExplorer::NbSurface()
{
  myIndex = 1; myMax = MYDS.NbSurfaces();
  Find();
  Standard_Integer n = 0;
  for (; More(); Next() ) n++;
  return n;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_SurfaceExplorer::Index()const
{
  return myIndex;
}
