// Created on: 1993-02-03
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



#include <Sweep_NumShape.ixx>


//=======================================================================
//function : Sweep_NumShape
//purpose  : 
//=======================================================================

Sweep_NumShape::Sweep_NumShape():
       myType(TopAbs_SHAPE),
       myIndex(0),
       myClosed(Standard_False),
       myBegInf(Standard_False),
       myEndInf(Standard_False)

{
}


//=======================================================================
//function : Sweep_NumShape
//purpose  : 
//=======================================================================

Sweep_NumShape::Sweep_NumShape(const Standard_Integer Index, 
			       const TopAbs_ShapeEnum Type,
			       const Standard_Boolean Closed,
			       const Standard_Boolean BegInf,
			       const Standard_Boolean EndInf):
       myType(Type),
       myIndex(Index),
       myClosed(Closed),
       myBegInf(BegInf),
       myEndInf(EndInf)
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Sweep_NumShape::Init(const Standard_Integer Index, 
			  const TopAbs_ShapeEnum Type,
			  const Standard_Boolean Closed,
			  const Standard_Boolean BegInf,
			  const Standard_Boolean EndInf)
{
  myIndex = Index;
  myType = Type;
  myClosed = Closed;
  myBegInf = BegInf;
  myEndInf = EndInf;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation Sweep_NumShape::Orientation () const
{
  if (myType==TopAbs_EDGE){
    return TopAbs_FORWARD;
  }
  else{
    if(myIndex == 2){
      return TopAbs_FORWARD;
    }
    else{
      return TopAbs_REVERSED;
    }
  }
}

