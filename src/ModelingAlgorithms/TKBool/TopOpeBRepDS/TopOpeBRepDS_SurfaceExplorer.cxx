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

//=================================================================================================

TopOpeBRepDS_SurfaceExplorer::TopOpeBRepDS_SurfaceExplorer()
    : myIndex(1),
      myMax(0),
      myDS(NULL),
      myFound(false)
{
}

//=================================================================================================

TopOpeBRepDS_SurfaceExplorer::TopOpeBRepDS_SurfaceExplorer(const TopOpeBRepDS_DataStructure& DS,
                                                           const bool FindKeep)
{
  Init(DS, FindKeep);
}

//=================================================================================================

void TopOpeBRepDS_SurfaceExplorer::Init(const TopOpeBRepDS_DataStructure& DS, const bool FindKeep)
{
  myIndex    = 1;
  myMax      = DS.NbSurfaces();
  myDS       = (TopOpeBRepDS_DataStructure*)&DS;
  myFindKeep = FindKeep;
  Find();
}

//=================================================================================================

void TopOpeBRepDS_SurfaceExplorer::Find()
{
  myFound = false;
  while (myIndex <= myMax)
  {
    if (myFindKeep)
    {
      myFound = IsSurfaceKeep(myIndex);
    }
    else
    {
      myFound = IsSurface(myIndex);
    }
    if (myFound)
      break;
    else
      myIndex++;
  }
}

//=================================================================================================

bool TopOpeBRepDS_SurfaceExplorer::More() const
{
  return myFound;
}

//=================================================================================================

void TopOpeBRepDS_SurfaceExplorer::Next()
{
  myIndex++;
  Find();
}

//=================================================================================================

const TopOpeBRepDS_Surface& TopOpeBRepDS_SurfaceExplorer::Surface() const
{
  if (myFound)
  {
    return MYDS.Surface(myIndex);
  }
  else
  {
    return myEmpty;
  }
}

//=================================================================================================

bool TopOpeBRepDS_SurfaceExplorer::IsSurface(const int I) const
{
  bool b = MYDS.mySurfaces.IsBound(I);
  return b;
}

//=================================================================================================

bool TopOpeBRepDS_SurfaceExplorer::IsSurfaceKeep(const int I) const
{
  bool b = MYDS.mySurfaces.IsBound(I);
  if (b)
    b = MYDS.Surface(I).Keep();
  return b;
}

//=================================================================================================

const TopOpeBRepDS_Surface& TopOpeBRepDS_SurfaceExplorer::Surface(const int I) const
{
  if (IsSurface(I))
  {
    return MYDS.Surface(I);
  }
  else
  {
    return myEmpty;
  }
}

//=================================================================================================

int TopOpeBRepDS_SurfaceExplorer::NbSurface()
{
  myIndex = 1;
  myMax   = MYDS.NbSurfaces();
  Find();
  int n = 0;
  for (; More(); Next())
    n++;
  return n;
}

//=================================================================================================

int TopOpeBRepDS_SurfaceExplorer::Index() const
{
  return myIndex;
}
