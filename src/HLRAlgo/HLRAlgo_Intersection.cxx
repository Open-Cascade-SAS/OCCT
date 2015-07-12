// Created on: 1992-02-19
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <HLRAlgo_Intersection.hxx>

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
