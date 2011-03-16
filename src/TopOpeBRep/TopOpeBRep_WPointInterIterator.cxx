// File:	TopOpeBRep_WPointInterIterator.cxx
// Created:	Tue Nov 16 10:52:21 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

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
