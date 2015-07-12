// Created on: 1993-10-15
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BOPAlgo_PaveFiller.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : BRepAlgoAPI_Fuse
//purpose  : 
//=======================================================================
BRepAlgoAPI_Fuse::BRepAlgoAPI_Fuse()
:
  BRepAlgoAPI_BooleanOperation()
{
  myOperation=BOPAlgo_FUSE;
}
//=======================================================================
//function : BRepAlgoAPI_Fuse
//purpose  : 
//=======================================================================
BRepAlgoAPI_Fuse::BRepAlgoAPI_Fuse(const BOPAlgo_PaveFiller& aPF)
:
  BRepAlgoAPI_BooleanOperation(aPF)
{
  myOperation=BOPAlgo_FUSE;
}
//=======================================================================
//function : ~BRepAlgoAPI_Fuse
//purpose  : 
//=======================================================================
BRepAlgoAPI_Fuse::~BRepAlgoAPI_Fuse()
{
}
//=======================================================================
//function : BRepAlgoAPI_Fuse
//purpose  : 
//=======================================================================
BRepAlgoAPI_Fuse::BRepAlgoAPI_Fuse(const TopoDS_Shape& S1, 
                                   const TopoDS_Shape& S2)
: 
  BRepAlgoAPI_BooleanOperation(S1, S2, BOPAlgo_FUSE)
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
                                   const BOPAlgo_PaveFiller& aDSF)
: 
  BRepAlgoAPI_BooleanOperation(S1, S2, aDSF, BOPAlgo_FUSE)
{
  BRepAlgoAPI_BooleanOperation* pBO=
    (BRepAlgoAPI_BooleanOperation*) (void*) this;
  pBO->Build();
}
