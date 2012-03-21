// Created on: 1999-01-06
// Created by: Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
