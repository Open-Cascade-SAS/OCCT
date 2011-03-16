// File:	TopOpeBRepBuild_ShellToSolid.cxx
// Created:	Thu Oct  2 15:21:18 1997
// Author:	Xuan Trang PHAM PHU
//		<xpu@poulopox.paris1.matra-dtv.fr>


#include <TopOpeBRepBuild_ShellToSolid.ixx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_Builder.hxx>

#include <TopOpeBRepDS_BuildTool.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : TopOpeBRepBuild_ShellToSolid
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShellToSolid::TopOpeBRepBuild_ShellToSolid()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellToSolid::Init()
{
  myLSh.Clear();
}


//=======================================================================
//function : AddShell
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellToSolid::AddShell(const TopoDS_Shell& Sh)
{
  myLSh.Append(Sh);
}

//=======================================================================
//function : MakeSolids
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellToSolid::MakeSolids(const TopoDS_Solid& So,
					      TopTools_ListOfShape& LSo)
{
  LSo.Clear();
  
  TopOpeBRepBuild_ShellFaceSet sfs(So);
  for (TopTools_ListIteratorOfListOfShape it(myLSh);it.More();it.Next())
    sfs.AddShape(it.Value());
  
  Standard_Boolean ForceClass = Standard_True;
  TopOpeBRepBuild_SolidBuilder SB;  
  SB.InitSolidBuilder(sfs,ForceClass);  

  TopOpeBRepDS_BuildTool BT;
  TopOpeBRepBuild_Builder B(BT);
  B.MakeSolids(SB,LSo);

}
