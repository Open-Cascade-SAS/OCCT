// File:	BRepAlgoAPI_Fuse.cxx
// Created:	Fri Oct 15 11:35:59 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>

#include <BRepAlgoAPI_Fuse.ixx>

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BOP_Operation.hxx>

//=======================================================================
//function : BRepAlgoAPI_Fuse
//purpose  : 
//=======================================================================
  BRepAlgoAPI_Fuse::BRepAlgoAPI_Fuse(const TopoDS_Shape& S1, 
				     const TopoDS_Shape& S2)
: BRepAlgoAPI_BooleanOperation(S1, S2, BOP_FUSE)
{
  BRepAlgoAPI_BooleanOperation* pBO=
    (BRepAlgoAPI_BooleanOperation*) (void*) this;
  pBO->Build();
}

//=======================================================================
//function : BRepAlgoAPI_Fuse
//purpose  : 
//=======================================================================
  BRepAlgoAPI_Fuse::BRepAlgoAPI_Fuse(const TopoDS_Shape& S1, 
				     const TopoDS_Shape& S2,
				     const BOPTools_DSFiller& aDSF)
: BRepAlgoAPI_BooleanOperation(S1, S2, aDSF, BOP_FUSE)
{
  BRepAlgoAPI_BooleanOperation* pBO=
    (BRepAlgoAPI_BooleanOperation*) (void*) this;
  pBO->Build();
}
