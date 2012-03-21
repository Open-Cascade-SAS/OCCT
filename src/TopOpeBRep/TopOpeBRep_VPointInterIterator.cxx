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


#include <TopOpeBRep_VPointInterIterator.ixx>
#include <Standard_ProgramError.hxx>

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
#ifdef DEB
      Standard_Integer iVP = 
#endif
	CurrentVPIndex();
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
#ifdef DEB
      Standard_Integer iVP = 
#endif
	CurrentVPIndex();
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
    Standard_ProgramError::Raise("TopOpeBRep_VPointInterIterator::CurrentVP");
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
    Standard_ProgramError::Raise("TopOpeBRep_VPointInterIterator::ChangeCurrentVP");
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
    Standard_ProgramError::Raise("TopOpeBRep_VPointInterIterator::CurrentVPIndex");
  return myVPointIndex;
}

TopOpeBRep_PLineInter TopOpeBRep_VPointInterIterator::PLineInterDummy() const {return myLineInter;}
