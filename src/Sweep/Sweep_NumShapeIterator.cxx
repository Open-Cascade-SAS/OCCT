// File:	Sweep_NumShapeIterator.cxx
// Created:	Thu Jun  3 10:46:16 1993
// Author:	Laurent BOURESCHE
//		<lbo@phylox>

#include <Sweep_NumShapeIterator.ixx>

//=======================================================================
//function : Sweep_NumShapeIterator
//purpose  : 
//=======================================================================

Sweep_NumShapeIterator::Sweep_NumShapeIterator():
       myNumShape(0,TopAbs_SHAPE),
       myCurrentNumShape(0,TopAbs_SHAPE)
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  Sweep_NumShapeIterator::Init(const Sweep_NumShape& aShape)
{
  myNumShape = aShape;
  if (myNumShape.Type()==TopAbs_EDGE){
    Standard_Integer nbvert = myNumShape.Index();
    myMore = (nbvert >= 1);
    if (myMore){
      myCurrentRange = 1;
      myCurrentNumShape = Sweep_NumShape(1,TopAbs_VERTEX,
					 myNumShape.Closed(),
					 Standard_False,
					 Standard_False);
      if (nbvert==1){
	if (myNumShape.BegInfinite()){
	  myCurrentOrientation = TopAbs_REVERSED;
	}
	else {
	  myCurrentOrientation = TopAbs_FORWARD;
	}
      }
      else {
	myCurrentOrientation = TopAbs_FORWARD;
      }
    }
  }
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  Sweep_NumShapeIterator::Next()
{
  myCurrentRange++;
  myMore = myCurrentRange <= myNumShape.Index();
  if (myMore){
    if (myNumShape.Type()==TopAbs_EDGE){
      myCurrentNumShape = Sweep_NumShape(myCurrentRange,TopAbs_VERTEX,
					 myNumShape.Closed(),
					 Standard_False,
					 Standard_False);
      myCurrentOrientation = TopAbs_REVERSED;
    }
  }
}




