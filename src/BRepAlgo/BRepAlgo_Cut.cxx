// File:	BRepAlgo_Cut.cxx
// Created:	Fri Oct 15 11:37:00 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRepAlgo_Cut.ixx>

//=======================================================================
//function : BRepAlgo_Cut
//purpose  : 
//=======================================================================

BRepAlgo_Cut::BRepAlgo_Cut(const TopoDS_Shape& S1, 
			   const TopoDS_Shape& S2)
: BRepAlgo_BooleanOperation(S1,S2)
{
  InitParameters();
  PerformDS();
  Perform(TopAbs_OUT,TopAbs_IN);
}
