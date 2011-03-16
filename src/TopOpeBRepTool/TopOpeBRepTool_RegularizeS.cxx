// File:	TopOpeBRepTool_RegularizeS.cxx
// Created:	Wed Jan  6 10:58:51 1999
// Author:      Xuan PHAM PHU
//		<xpu@poulopox.paris1.matra-dtv.fr>


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
