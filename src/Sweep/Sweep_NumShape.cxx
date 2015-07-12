// Created on: 1993-02-03
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


#include <Sweep_NumShape.hxx>

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

