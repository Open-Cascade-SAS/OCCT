// File:	BRepSweep_Builder.cxx
// Created:	Thu Feb  4 15:22:43 1993
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <BRepSweep_Builder.ixx>
#include <TopoDS.hxx>

//=======================================================================
//function : BRepSweep_Builder
//purpose  : 
//=======================================================================

BRepSweep_Builder::BRepSweep_Builder(const BRep_Builder& aBuilder) :
       myBuilder(aBuilder)
{
}

//=======================================================================
//function : MakeCompound
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeCompound(TopoDS_Shape& aCompound)const 
{
  myBuilder.MakeCompound(TopoDS::Compound(aCompound));
}


//=======================================================================
//function : MakeCompSolid
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeCompSolid(TopoDS_Shape& aCompSolid)const 
{
  myBuilder.MakeCompSolid(TopoDS::CompSolid(aCompSolid));
}


//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeSolid(TopoDS_Shape& aSolid)const 
{
  myBuilder.MakeSolid(TopoDS::Solid(aSolid));
}


//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeShell(TopoDS_Shape& aShell)const 
{
  myBuilder.MakeShell(TopoDS::Shell(aShell));
}


//=======================================================================
//function : MakeWire
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::MakeWire(TopoDS_Shape& aWire)const 
{
  myBuilder.MakeWire(TopoDS::Wire(aWire));
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::Add(TopoDS_Shape& aShape1, 
			     const TopoDS_Shape& aShape2, 
			     const TopAbs_Orientation Orient)const 
{
  TopoDS_Shape aComp = aShape2;
  aComp.Orientation(Orient);
  myBuilder.Add(aShape1,aComp);
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepSweep_Builder::Add(TopoDS_Shape& aShape1, 
			     const TopoDS_Shape& aShape2)const 
{
  myBuilder.Add(aShape1,aShape2);
}


