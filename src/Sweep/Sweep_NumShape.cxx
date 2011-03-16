// File:	Sweep_NumShape.cxx
// Created:	Wed Feb  3 17:55:49 1993
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


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

