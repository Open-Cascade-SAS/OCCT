// File:	HLRAlgo_Intersection.cxx
// Created:	Wed Feb 19 14:22:05 1992
// Author:	Christophe MARION
//		<cma@sdsun2>

#include <HLRAlgo_Intersection.ixx>

//=======================================================================
//function : HLRAlgo_Intersection
//purpose  : 
//=======================================================================

HLRAlgo_Intersection::HLRAlgo_Intersection()
{}

//=======================================================================
//function : HLRAlgo_Intersection
//purpose  : 
//=======================================================================

HLRAlgo_Intersection::HLRAlgo_Intersection
  (const TopAbs_Orientation Ori,
   const Standard_Integer Lev,
   const Standard_Integer SegInd,
   const Standard_Integer Ind,
   const Standard_Real P,
   const Standard_ShortReal Tol,
   const TopAbs_State S) :
  myOrien(Ori),
  mySegIndex(SegInd),
  myIndex(Ind),
  myLevel(Lev),
  myParam(P),
  myToler(Tol),
  myState(S)
{}

