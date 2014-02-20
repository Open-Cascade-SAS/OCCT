// Created on: 1999-01-06
// Created by: Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#include <TopOpeBRepTool_REGUS.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopOpeBRepTool_define.hxx>


//=======================================================================
//function : RegularizeShells
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool::RegularizeShells(const TopoDS_Solid& theSolid,
				     TopTools_DataMapOfShapeListOfShape& OldSheNewShe,
				     TopTools_DataMapOfShapeListOfShape& FSplits) 
{
  OldSheNewShe.Clear();
  FSplits.Clear();
  TopOpeBRepTool_REGUS REGUS;
  REGUS.SetOshNsh(OldSheNewShe);  
  REGUS.SetFsplits(FSplits);

//  Standard_Boolean hastoregu = Standard_False;
  TopExp_Explorer exsh(theSolid,TopAbs_SHELL);
  for (; exsh.More(); exsh.Next()) {

    const TopoDS_Shape& sh = exsh.Current();
    REGUS.Init(sh);
    Standard_Boolean ok = REGUS.MapS();
    if (!ok) return Standard_False;
    ok = REGUS.SplitFaces();
    if (!ok) return Standard_False;
    REGUS.REGU();
  
  }//exsh(theSolid)

  REGUS.GetOshNsh(OldSheNewShe); //??????????????????????????????
  REGUS.GetFsplits(FSplits);     //??????????????????????????????
  return Standard_True;
}
