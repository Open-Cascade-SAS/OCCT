// File:	BRepAlgo_Fuse.cxx
// Created:	Fri Oct 15 11:35:59 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRepAlgo_Fuse.ixx>

//=======================================================================
//function : BRepAlgo_Fuse
//purpose  : 
//=======================================================================
  BRepAlgo_Fuse::BRepAlgo_Fuse(const TopoDS_Shape& S1, 
			       const TopoDS_Shape& S2)
: BRepAlgo_BooleanOperation(S1,S2)
{
  InitParameters();
  PerformDS();
  Perform(TopAbs_OUT,TopAbs_OUT);
}
