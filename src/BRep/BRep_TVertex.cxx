// File:	BRep_TVertex.cxx
// Created:	Tue Aug 25 15:47:49 1992
// Author:	Modelistation
//		<model@phylox>


#include <BRep_TVertex.ixx>
#include <TopAbs.hxx>

//=======================================================================
//function : BRep_TVertex
//purpose  : 
//=======================================================================

BRep_TVertex::BRep_TVertex() :
       TopoDS_TVertex(),
       myTolerance(RealEpsilon())
{
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) BRep_TVertex::EmptyCopy() const
{
  Handle(BRep_TVertex) TV = 
    new BRep_TVertex();
  TV->Pnt(myPnt);
  TV->Tolerance(myTolerance);
  return TV;
}

