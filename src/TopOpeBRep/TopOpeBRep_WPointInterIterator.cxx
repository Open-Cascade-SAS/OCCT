// Created on: 1993-11-16
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <TopOpeBRep_WPointInterIterator.ixx>
#include <Standard_ProgramError.hxx>

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
    Standard_ProgramError::Raise("TopOpeBRep_WPointInterIterator::Current");
  const TopOpeBRep_WPointInter& WP = myLineInter->WPoint(myWPointIndex);
  return WP;
}

TopOpeBRep_PLineInter TopOpeBRep_WPointInterIterator::PLineInterDummy() const {return myLineInter;}
