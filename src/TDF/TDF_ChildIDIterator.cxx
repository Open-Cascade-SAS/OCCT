// File:	TDF_ChildIDIterator.cxx
//      	-----------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Nov 20 1997	Creation



#include <TDF_ChildIDIterator.ixx>

#include <TDF_ChildIterator.ixx>
#include <TDF_Label.hxx>
#include <TDF_LabelNode.hxx>
#include <TDF_LabelNodePtr.hxx>

#define ChildIDIterator_FindNext \
{ while( myItr.More() &&  !myItr.Value().FindAttribute(myID,myAtt)) myItr.Next(); }



//=======================================================================
//function : TDF_ChildIDIterator
//purpose  : 
//=======================================================================

TDF_ChildIDIterator::TDF_ChildIDIterator()
{}


//=======================================================================
//function : TDF_ChildIDIterator
//purpose  : 
//=======================================================================

TDF_ChildIDIterator::TDF_ChildIDIterator
(const TDF_Label& aLabel,
 const Standard_GUID& anID,
 const Standard_Boolean allLevels)
: myID(anID),
  myItr(aLabel,allLevels)
{ ChildIDIterator_FindNext; }


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::Initialize
(const TDF_Label& aLabel,
 const Standard_GUID& anID,
 const Standard_Boolean allLevels)
{
  myID = anID;
  myItr.Initialize(aLabel,allLevels);
  myAtt.Nullify();
  ChildIDIterator_FindNext;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::Next() 
{
  myAtt.Nullify();
  if (myItr.More()) {
    myItr.Next();
    ChildIDIterator_FindNext;
  }
}


//=======================================================================
//function : NextBrother
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::NextBrother() 
{
  myAtt.Nullify();
  if (myItr.More()) {
    myItr.NextBrother();
    while (myItr.More() && !myItr.Value().FindAttribute(myID,myAtt))
      myItr.NextBrother();
  }
}


