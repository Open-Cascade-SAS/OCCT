// Created on: 1993-11-16
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


#include <Standard_ProgramError.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_WPointInter.hxx>
#include <TopOpeBRep_WPointInterIterator.hxx>

//=======================================================================
//function : WPointIterator
//purpose  : 
//=======================================================================
TopOpeBRep_WPointInterIterator::TopOpeBRep_WPointInterIterator() :
myLineInter(NULL),myWPointIndex(0),myWPointNb(0)
{
}

//=======================================================================
//function : WPointIterator
//purpose  : 
//=======================================================================

TopOpeBRep_WPointInterIterator::TopOpeBRep_WPointInterIterator
(const TopOpeBRep_LineInter& LI)
{
  Init(LI);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRep_WPointInterIterator::Init(const TopOpeBRep_LineInter& LI)
{
  myLineInter = (TopOpeBRep_LineInter*)&LI;
  Init();
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRep_WPointInterIterator::Init()
{
  myWPointIndex = 1;
  myWPointNb = myLineInter->NbWPoint();
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_WPointInterIterator::More() const
{
  return (myWPointIndex <= myWPointNb);
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopOpeBRep_WPointInterIterator::Next()
{
  myWPointIndex++;
}

//=======================================================================
//function : CurrentWP
//purpose  :
//=======================================================================

const TopOpeBRep_WPointInter& TopOpeBRep_WPointInterIterator::CurrentWP()
{
  if (!More()) 
    throw Standard_ProgramError("TopOpeBRep_WPointInterIterator::Current");
  const TopOpeBRep_WPointInter& WP = myLineInter->WPoint(myWPointIndex);
  return WP;
}

TopOpeBRep_PLineInter TopOpeBRep_WPointInterIterator::PLineInterDummy() const {return myLineInter;}
