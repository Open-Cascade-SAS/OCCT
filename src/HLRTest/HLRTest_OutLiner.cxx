// File:	HLRTest_OutLiner.cxx
// Created:	Wed Apr  5 17:06:37 1995
// Author:	Christophe MARION
//		<cma@ecolox>

#include <HLRTest_OutLiner.ixx>

//=======================================================================
//function : HLRTest_OutLiner
//purpose  : 
//=======================================================================

HLRTest_OutLiner::HLRTest_OutLiner (const TopoDS_Shape& S)
{
  myOutLiner = new HLRTopoBRep_OutLiner(S);
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void HLRTest_OutLiner::DrawOn (Draw_Display& dis) const 
{
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) HLRTest_OutLiner::Copy () const 
{
  Handle(HLRTest_OutLiner) O = 
    new HLRTest_OutLiner(myOutLiner->OriginalShape());
  return O;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void HLRTest_OutLiner::Dump (Standard_OStream& S) const 
{
  S << "This is an outliner" << endl;
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void HLRTest_OutLiner::Whatis (Draw_Interpretor& I) const 
{
  I << "outliner";
}

