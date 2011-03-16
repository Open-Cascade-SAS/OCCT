// File:	BRepAlgoAPI_Common.cxx
// Created:	Fri Oct 15 11:36:28 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>

#include <BRepAlgoAPI_Common.ixx>

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BOP_Operation.hxx>

//=======================================================================
//function : BRepAlgoAPI_Common
//purpose  : 
//=======================================================================
  BRepAlgoAPI_Common::BRepAlgoAPI_Common(const TopoDS_Shape& S1, 
					 const TopoDS_Shape& S2)
: BRepAlgoAPI_BooleanOperation(S1, S2, BOP_COMMON)
{
  BRepAlgoAPI_BooleanOperation* pBO=
    (BRepAlgoAPI_BooleanOperation*) (void*) this;
  pBO->Build();
}
//=======================================================================
//function : BRepAlgoAPI_Common
//purpose  : 
//=======================================================================
  BRepAlgoAPI_Common::BRepAlgoAPI_Common(const TopoDS_Shape& S1, 
					 const TopoDS_Shape& S2,
					 const BOPTools_DSFiller& aDSF)
: BRepAlgoAPI_BooleanOperation(S1, S2, aDSF, BOP_COMMON)
{
  BRepAlgoAPI_BooleanOperation* pBO=
    (BRepAlgoAPI_BooleanOperation*) (void*) this;
  pBO->Build();
}



