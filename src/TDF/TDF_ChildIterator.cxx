// File:	TDF_ChildIterator.cxx
//      	----------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Feb  7 1997	Creation



#include <TDF_ChildIterator.ixx>

#include <TDF_Label.hxx>
#include <TDF_LabelNode.hxx>
#include <TDF_LabelNodePtr.hxx>

#define ChildIterator_UpToBrother \
{ \
    while (myNode && (myNode->Depth() > myFirstLevel) && !myNode->Brother()) \
      myNode = myNode->Father(); \
	if (myNode && (myNode->Depth() > myFirstLevel) && myNode->Father()) \
	  myNode = myNode->Brother(); \
	else \
	  myNode = NULL; \
}


//=======================================================================
//function : TDF_ChildIterator
//purpose  : 
//=======================================================================

TDF_ChildIterator::TDF_ChildIterator()
: myNode(NULL),
  myFirstLevel(0)
{}


//=======================================================================
//function : TDF_ChildIterator
//purpose  : 
//=======================================================================

TDF_ChildIterator::TDF_ChildIterator
(const TDF_Label& aLabel,
 const Standard_Boolean allLevels)
: myNode(aLabel.myLabelNode->FirstChild()),
  myFirstLevel(allLevels ? aLabel.Depth() : -1)
{}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDF_ChildIterator::Initialize
(const TDF_Label& aLabel,
 const Standard_Boolean allLevels)
{
  myNode = aLabel.myLabelNode->FirstChild();
  myFirstLevel = allLevels ? aLabel.Depth() : -1;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDF_ChildIterator::Next() 
{
  if (myFirstLevel == -1) {
    myNode = myNode->Brother();
  }
  else {
    if (myNode->FirstChild()) myNode = myNode->FirstChild();
    else ChildIterator_UpToBrother;
  }
}


//=======================================================================
//function : NextBrother
//purpose  : 
//=======================================================================

void TDF_ChildIterator::NextBrother() 
{
  if ((myFirstLevel  == -1) || myNode->Brother()) myNode = myNode->Brother();
  else ChildIterator_UpToBrother;
}
