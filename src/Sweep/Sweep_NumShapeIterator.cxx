// Created on: 1993-06-03
// Created by: Laurent BOURESCHE
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




