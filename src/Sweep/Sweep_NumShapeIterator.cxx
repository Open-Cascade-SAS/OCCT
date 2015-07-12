// Created on: 1993-06-03
// Created by: Laurent BOURESCHE
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


#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Sweep_NumShape.hxx>
#include <Sweep_NumShapeIterator.hxx>

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




