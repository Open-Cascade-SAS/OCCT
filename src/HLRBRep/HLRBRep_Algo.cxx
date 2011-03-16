// File:	HLRBRep_Algo.cxx
// Created:	Thu Aug  4 10:18:44 1994
// Author:	Christophe MARION
//		<cma@ecolox>

#define No_Standard_OutOfRange

#include <HLRBRep_Algo.ixx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRTopoBRep_OutLiner.hxx>

//=======================================================================
//function : HLRBRep_Algo
//purpose  : 
//=======================================================================

HLRBRep_Algo::HLRBRep_Algo ()
{}

//=======================================================================
//function : HLRBRep_Algo
//purpose  : 
//=======================================================================

HLRBRep_Algo::HLRBRep_Algo (const Handle(HLRBRep_Algo)& A) :
HLRBRep_InternalAlgo(A)
{}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  HLRBRep_Algo::Add (const TopoDS_Shape& S,
			 const Handle(MMgt_TShared)& SData,
			 const Standard_Integer nbIso)
{
  Load(new HLRTopoBRep_OutLiner(S),SData,nbIso);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  HLRBRep_Algo::Add (const TopoDS_Shape& S,
			 const Standard_Integer nbIso)
{
  Load(new HLRTopoBRep_OutLiner(S),nbIso);
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_Algo::Index (const TopoDS_Shape& S)
{
  Standard_Integer n = NbShapes();

  for (Standard_Integer i = 1; i <= n; i++) {
    if (ShapeBounds(i).Shape()->OriginalShape() == S) return i;
    if (ShapeBounds(i).Shape()->OutLinedShape() == S) return i;
  }

  return 0;
}

//=======================================================================
//function : OutLinedShapeNullify
//purpose  : 
//=======================================================================

void HLRBRep_Algo::OutLinedShapeNullify ()
{
  Standard_Integer n = NbShapes();

  for (Standard_Integer i = 1; i <= n; i++) {
    ShapeBounds(i).Shape()->OutLinedShape(TopoDS_Shape());
    ShapeBounds(i).Shape()->DataStructure().Clear();
  }
}

