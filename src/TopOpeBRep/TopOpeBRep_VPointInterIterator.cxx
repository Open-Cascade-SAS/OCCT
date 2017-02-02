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
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>

//=======================================================================
//function : VPointInterIterator
//purpose  : 
//=======================================================================
TopOpeBRep_VPointInterIterator::TopOpeBRep_VPointInterIterator() : 
myLineInter(NULL),myVPointIndex(0),myVPointNb(0),mycheckkeep(Standard_False)
{}

//=======================================================================
//function : VPointInterIterator
//purpose  : 
//=======================================================================

TopOpeBRep_VPointInterIterator::TopOpeBRep_VPointInterIterator
(const TopOpeBRep_LineInter& LI)
{
  Init(LI);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRep_VPointInterIterator::Init
(const TopOpeBRep_LineInter& LI,
 const Standard_Boolean checkkeep)
{
  myLineInter = (TopOpeBRep_LineInter*)&LI;
  mycheckkeep = checkkeep;
  Init();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRep_VPointInterIterator::Init()
{
  myVPointIndex = 1;
  myVPointNb = myLineInter->NbVPoint();
  if ( mycheckkeep ) {
    while ( More() ) { 
      const TopOpeBRep_VPointInter& VP = CurrentVP();
      if (VP.Keep()) break;
      else myVPointIndex++;
    }
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_VPointInterIterator::More() const 
{
  return (myVPointIndex <= myVPointNb);
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  TopOpeBRep_VPointInterIterator::Next()
{
  myVPointIndex++;
  if ( mycheckkeep ) {
    while ( More() ) { 
      const TopOpeBRep_VPointInter& VP = CurrentVP();
      if (VP.Keep()) break;
      else myVPointIndex++;
    }
  }
}

//=======================================================================
//function : CurrentVP
//purpose  :
//=======================================================================

const TopOpeBRep_VPointInter& TopOpeBRep_VPointInterIterator::CurrentVP()
{
  if (!More())
    throw Standard_ProgramError("TopOpeBRep_VPointInterIterator::CurrentVP");
  const TopOpeBRep_VPointInter& VP = myLineInter->VPoint(myVPointIndex);
  return VP;
}

//=======================================================================
//function : ChangeCurrentVP
//purpose  :
//=======================================================================

TopOpeBRep_VPointInter& TopOpeBRep_VPointInterIterator::ChangeCurrentVP()
{
  if (!More()) 
    throw Standard_ProgramError("TopOpeBRep_VPointInterIterator::ChangeCurrentVP");
  TopOpeBRep_VPointInter& VP = myLineInter->ChangeVPoint(myVPointIndex);
  return VP;
}

//=======================================================================
//function : CurrentVPIndex
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRep_VPointInterIterator::CurrentVPIndex()const
{
  if (!More()) 
    throw Standard_ProgramError("TopOpeBRep_VPointInterIterator::CurrentVPIndex");
  return myVPointIndex;
}

TopOpeBRep_PLineInter TopOpeBRep_VPointInterIterator::PLineInterDummy() const {return myLineInter;}
