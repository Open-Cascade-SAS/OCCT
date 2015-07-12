// Created on: 1995-11-13
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
#include <TopOpeBRepDS_ShapeData.hxx>

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
