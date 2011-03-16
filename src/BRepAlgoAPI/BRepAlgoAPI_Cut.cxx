// File:	BRepAlgoAPI_Cut.cxx
// Created:	Fri Oct 15 11:37:00 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRepAlgoAPI_Cut.ixx>

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BOP_Operation.hxx>

//=======================================================================
//function : BRepAlgoAPI_Cut
//purpose  : 
//=======================================================================
  BRepAlgoAPI_Cut::BRepAlgoAPI_Cut(const TopoDS_Shape& S1, 
				   const TopoDS_Shape& S2)
: BRepAlgoAPI_BooleanOperation(S1, S2, BOP_CUT)
{
  BRepAlgoAPI_BooleanOperation* pBO=
    (BRepAlgoAPI_BooleanOperation*) (void*) this;
  pBO->Build();
}
//=======================================================================
//function : BRepAlgoAPI_Cut
//purpose  : 
//=======================================================================
  BRepAlgoAPI_Cut::BRepAlgoAPI_Cut(const TopoDS_Shape& S1, 
				   const TopoDS_Shape& S2,
				   const BOPTools_DSFiller& aDSF,
				   const Standard_Boolean bFWD)
: BRepAlgoAPI_BooleanOperation(S1, S2, aDSF, (bFWD) ? BOP_CUT : BOP_CUT21)
{
  BRepAlgoAPI_BooleanOperation* pBO=
    (BRepAlgoAPI_BooleanOperation*) (void*) this;
  pBO->Build();
}
