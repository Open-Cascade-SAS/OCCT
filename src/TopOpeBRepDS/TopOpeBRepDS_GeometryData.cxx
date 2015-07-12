// Created on: 1994-05-26
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


#include <TopOpeBRepDS_GeometryData.hxx>
#include <TopOpeBRepDS_Interference.hxx>
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
