// File:	BRepAlgo_Common.cxx
// Created:	Fri Oct 15 11:36:28 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRepAlgo_Common.ixx>

//=======================================================================
//function : BRepAlgo_Common
//purpose  : 
//=======================================================================
  BRepAlgo_Common::BRepAlgo_Common(const TopoDS_Shape& S1, 
				   const TopoDS_Shape& S2)
: BRepAlgo_BooleanOperation(S1,S2)
{
  InitParameters();
  PerformDS();
  Perform(TopAbs_IN,TopAbs_IN);
}
