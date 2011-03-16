// File:	BRep_TFace.cxx
// Created:	Tue Aug 25 15:32:17 1992
// Author:	Remi Lequette
//		<rle@phylox>


#include <BRep_TFace.ixx>
#include <TopAbs.hxx>

//=======================================================================
//function : BRep_TFace
//purpose  : 
//=======================================================================

BRep_TFace::BRep_TFace() :
       TopoDS_TFace(),
       myTolerance(RealEpsilon()),
       myNaturalRestriction(Standard_False)
{
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) BRep_TFace::EmptyCopy() const
{
  Handle(BRep_TFace) TF = 
    new BRep_TFace();
  TF->Surface(mySurface);
  TF->Location(myLocation);
  TF->Tolerance(myTolerance);
  return TF;
}
