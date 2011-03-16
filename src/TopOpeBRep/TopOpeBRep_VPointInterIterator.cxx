// File:	TopOpeBRep_VPointIterator.cxx
// Created:	Tue Nov 16 10:52:21 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

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
